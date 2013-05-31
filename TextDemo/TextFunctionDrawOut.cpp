#include "pch.h"
#include "TextFunctionDrawOut.h"
using namespace Windows::UI::Core;
using namespace D2D1;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

Haozhaopian::TextFunctionDrawOut::TextFunctionDrawOut(CoreWindow^ window, float dpi){
	DirectXBase::Initialize(window,dpi);
	InitializeAdapter();
}
void Haozhaopian::TextFunctionDrawOut::setBitmapArgus(double width,double height,double orgScale){
	bitmapSize.width=width;
	bitmapSize.height=height;
	
	UINT32 size=m_d2dContext->GetMaximumBitmapSize();

	this->orgScale=orgScale;

	if(width>size || height>size){
		double offset=width>height?size/width:size/height;
		bitmapSize.width*=offset;
		bitmapSize.height*=offset;
		
		this->orgScale/=offset;	
	}
	
	BitmapWidth=bitmapSize.width;
	BitmapHeight=bitmapSize.height;
	
}

	// DirectXBase 方法。
void Haozhaopian::TextFunctionDrawOut::CreateDeviceIndependentResources() {
	DirectXBase::CreateDeviceIndependentResources();
}
void Haozhaopian::TextFunctionDrawOut::CreateDeviceResources() {
	DirectXBase::CreateDeviceResources();
}
void Haozhaopian::TextFunctionDrawOut::CreateWindowSizeDependentResources() {
	DirectXBase::CreateDeviceIndependentResources();
}
void Haozhaopian::TextFunctionDrawOut::drawBitmap(TextAttribute^ attribute){
			TemplateAttribute^ tmpatt=attribute->templateAttribute;
			
			//绘模版
			if(tmpatt==nullptr)return;
			Microsoft::WRL::ComPtr<ID2D1Bitmap>             m_Bitmap;
			 ComPtr<IWICBitmapDecoder> wicBitmapDecoder;
			 Platform::String^ mystr=L"Assets\\graphicFont\\"+tmpatt->name;
			 DX::ThrowIfFailed(
					m_wicFactory->CreateDecoderFromFilename(
					mystr->Data(),
						nullptr,
						GENERIC_READ,
						WICDecodeMetadataCacheOnDemand,
						&wicBitmapDecoder
						)
					);

				ComPtr<IWICBitmapFrameDecode> wicBitmapFrame;
				DX::ThrowIfFailed(
					wicBitmapDecoder->GetFrame(0, &wicBitmapFrame)
					);

				ComPtr<IWICFormatConverter> wicFormatConverter;
				DX::ThrowIfFailed(
					m_wicFactory->CreateFormatConverter(&wicFormatConverter)
					);

				DX::ThrowIfFailed(
					wicFormatConverter->Initialize(
						wicBitmapFrame.Get(),
						GUID_WICPixelFormat32bppPBGRA,
						WICBitmapDitherTypeNone,
						nullptr,
						0.0,
						WICBitmapPaletteTypeCustom  // the BGRA format has no palette so this value is ignored
						)
					);

				double dpiX = 96.0f;
				double dpiY = 96.0f;
				DX::ThrowIfFailed(
					wicFormatConverter->GetResolution(&dpiX, &dpiY)
					);

				


				DX::ThrowIfFailed(
					m_d2dContext->CreateBitmapFromWicBitmap(
						wicFormatConverter.Get(),
						BitmapProperties(
							PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
							static_cast<float>(dpiX),
							static_cast<float>(dpiY)
							),
						&m_Bitmap
						)
					);
				float mwidth=(1024/3)/orgScale;
				float mheight=(1024/3)/orgScale;
				float mTop=(attribute->top+16)/orgScale;
				float mLeft=(attribute->left+16)/orgScale;
				Matrix3x2F translation=	Matrix3x2F::Rotation(attribute->angle,D2D1::Point2F(mwidth / 2.0f+mLeft , mheight / 2.0f+mTop));
				
				translation.SetProduct(
						translation,
						Matrix3x2F::Scale(attribute->scale,attribute->scale,D2D1::Point2F(mwidth / 2.0f+mLeft , mheight / 2.0f+mTop)));
				
			m_d2dContext->SetTransform(translation);
			
			m_d2dContext->DrawBitmap(
				m_Bitmap.Get(),
			D2D1::RectF(mLeft,mTop,mwidth+mLeft,mheight+mTop),attribute->alpha/100.0
            );

}
void Haozhaopian::TextFunctionDrawOut::drawText(TextAttribute^ attribute){
	DirectXBase::CreateDeviceIndependentResources();	
	Microsoft::WRL::ComPtr<IDWriteTextFormat> m_textFormat;
	Microsoft::WRL::ComPtr<IDWriteTextLayout> m_textLayout;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pBrush;
	DWRITE_TEXT_METRICS m_textMetrics;
	DX::ThrowIfFailed(
		m_dwriteFactory->CreateTextFormat(
		attribute->textFamily->Data(),
			nullptr,
			attribute->style&Haozhaopian::FontStyle::STYLE_BOLD?DWRITE_FONT_WEIGHT_BOLD:DWRITE_FONT_WEIGHT_NORMAL,
			attribute->style& Haozhaopian::FontStyle::STYLE_OBLIQUE?DWRITE_FONT_STYLE_OBLIQUE:DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			attribute->size/orgScale,
			L"en-US",
			&m_textFormat
			)
		);
	DX::ThrowIfFailed(
		m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING)
		);
	
	DX::ThrowIfFailed(
		m_d2dContext->CreateSolidColorBrush(
		D2D1::ColorF(attribute->color.R/255.0,attribute->color.G/255.0,attribute->color.B/255.0,attribute->alpha/100.0),
			&m_pBrush
			)
		);
	Platform::String^ text = attribute->textContent;
	float width=5000;
	float height=5000;
	if(attribute->templateAttribute!=nullptr){
		width=(attribute->templateAttribute->width/3)/orgScale;
		height=(attribute->templateAttribute->height/3)/orgScale;
	}
	
	DX::ThrowIfFailed(
		m_dwriteFactory->CreateTextLayout(
			text->Data(),
			text->Length(),
			m_textFormat.Get(),
			width, // maxWidth。
			height, // maxHeight。
			&m_textLayout
			)
		);

	DWRITE_TEXT_RANGE textRange;
	textRange.length=text->Length();
	textRange.startPosition=0;


	DX::ThrowIfFailed(
		m_textLayout->SetUnderline(attribute->style&Haozhaopian::FontStyle::STYLE_UNDERLINE?true:false,
		textRange)
		);
	
	//字体适配
	FontBaseLine^ baseline=getFontBaseLineFromName(attribute->textFamily);
	if(baseline!=nullptr){
		DX::ThrowIfFailed(
			m_textLayout->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_UNIFORM,
			baseline->lineSpace/orgScale*(attribute->size/40),
			baseline->baseLine/orgScale*(attribute->size/40))
			);
	}
	DX::ThrowIfFailed(
		m_textLayout->GetMetrics(&m_textMetrics)
		);
	float left=(attribute->left+16)/orgScale;
	float top=(attribute->top+16)/orgScale;
	
	float centerX;
	float centerY;
	if(attribute->templateAttribute!=nullptr){
		//1024==template原始Width,height
		//3==在UI中被缩放了3倍
		//
		float templateImageCenterX=(1024/3/2)/orgScale;
		float templateImageCenterY=(1024/3/2)/orgScale;
		centerX=left+templateImageCenterX;
		centerY=top+templateImageCenterY;
		left+=(attribute->templateAttribute->left/3)/orgScale;
		top+=(attribute->templateAttribute->top/3)/orgScale;	
	}else{
		centerX=left+(attribute->width/2)/orgScale;
		centerY=top+(attribute->height/2)/orgScale;
	}
	
	Matrix3x2F translation = Matrix3x2F::Rotation(attribute->angle,
		D2D1::Point2F(
		centerX,centerY
		)
		);	
	translation.SetProduct(
		translation,
		Matrix3x2F::Scale(attribute->scale,attribute->scale,D2D1::Point2F(
		centerX,centerY
		))
		);

	m_d2dContext->SetTransform(translation);
	m_d2dContext->DrawTextLayout(
		Point2F(left, top),
		m_textLayout.Get(),
		m_pBrush.Get(),
		D2D1_DRAW_TEXT_OPTIONS_NO_SNAP
		);
}
//onDraw
void Haozhaopian::TextFunctionDrawOut::Render() {
	if(m_pTextAttributes==nullptr) return;
	ComPtr<ID2D1Bitmap1> m_d2d1TargetBitmap;
	D2D1_BITMAP_PROPERTIES1 renderTargetProperties = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET,
		D2D1::PixelFormat(
		DXGI_FORMAT_B8G8R8A8_UNORM,
		D2D1_ALPHA_MODE_PREMULTIPLIED
		)
		);
	m_d2dContext->CreateBitmap(
		D2D1::SizeU(static_cast<UINT32>(bitmapSize.width) ,static_cast<UINT32>(bitmapSize.height)),
		nullptr,
		0,
		&renderTargetProperties,
		&m_d2d1TargetBitmap
		);
	m_d2dContext->SetTarget(m_d2d1TargetBitmap.Get());
	m_d2dContext->BeginDraw();	

	m_d2dContext->Clear(ColorF(ColorF(0.0f,0.0f,0.0f,0.0f)));
	
	Matrix3x2F oriMatrix(1.0f,0.0f,0.0f,1.0f,0.0f,0.0f);
	m_d2dContext->SetTransform(oriMatrix);

	int width = static_cast<int>(bitmapSize.width);
	int height = static_cast<int>(bitmapSize.height);

	//==========================================================
	for(int i=0;i<m_pTextAttributes->Length;i++){
		TextAttribute^ attribute=m_pTextAttributes[i];
		if(attribute==nullptr){
			continue;
		}
		TemplateAttribute^ tmpatt=attribute->templateAttribute;
		//绘模版
		if(tmpatt!=nullptr){
			drawBitmap(attribute);
			if(tmpatt->width==0 || tmpatt->height==0){
				//模版不支持文字
				continue;
			}
		}
		//绘文字
		drawText(attribute);
	}




	//==============================================================
	HRESULT hr = m_d2dContext->EndDraw();

	
	if (hr != D2DERR_RECREATE_TARGET)
	{		
		D2D1_BITMAP_PROPERTIES1 prop = D2D1::BitmapProperties1(  
			D2D1_BITMAP_OPTIONS_CPU_READ | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,  
			D2D1::PixelFormat(  
			DXGI_FORMAT_B8G8R8A8_UNORM,  
			D2D1_ALPHA_MODE_PREMULTIPLIED  
			)  
			);  
		
		HRESULT hr = m_d2dContext->CreateBitmap(  
			D2D1::SizeU(static_cast<UINT32>(bitmapSize.width) ,static_cast<UINT32>(bitmapSize.height)),  
			nullptr,  
			0,  
			&prop,  
			&m_pRenderBitmap  
			);  
		D2D1_POINT_2U point;
		point.x = 0;
		point.y = 0;
		
		D2D1_RECT_U rect = D2D1::RectU(0,0,static_cast<UINT32>(bitmapSize.width),static_cast<UINT32>(bitmapSize.height));
		hr = m_pRenderBitmap->CopyFromBitmap(&point,m_d2d1TargetBitmap.Get(),&rect);
		D2D1_MAP_OPTIONS options = D2D1_MAP_OPTIONS_READ;
		D2D1_MAPPED_RECT mappedRect;
		hr = m_pRenderBitmap->Map(options, &mappedRect);
		pData = ref new Platform::Array<byte>(width * height * 4);
		byte *srcData = mappedRect.bits;
		byte *dstData = pData->Data;
		for(int i = 0; i < height; i++)
		{
			memcpy(dstData,srcData,width * 4);
			srcData += mappedRect.pitch;
			dstData += width * 4;
		}
		m_pRenderBitmap->Unmap();
		DX::ThrowIfFailed(hr);
	}
}
Haozhaopian::FontBaseLine^ Haozhaopian::TextFunctionDrawOut::getFontBaseLineFromName(Platform::String^ fontname){
	FontBaseLine^ result;
	for(int i=0;i<m_pFontAdapterArray->Size;i++){
		if(m_pFontAdapterArray->GetAt(i)->fontName->Equals(fontname)){
			return m_pFontAdapterArray->GetAt(i);
		}
	}
	return result;
}
void Haozhaopian::TextFunctionDrawOut::InitializeAdapter(){

	m_pFontAdapterArray=ref new Platform::Collections::Vector<Haozhaopian::FontBaseLine^>();
	m_pFontAdapterArray->Append(ref new FontBaseLine("Arial",36,46));
	m_pFontAdapterArray->Append(ref new FontBaseLine("Agency FB",38,47));
	m_pFontAdapterArray->Append(ref new FontBaseLine("Aharoni",30,40));
	m_pFontAdapterArray->Append(ref new FontBaseLine("Aldhabi",27,70));
	m_pFontAdapterArray->Append(ref new FontBaseLine("Algerian",38,52));
	m_pFontAdapterArray->Append(ref new FontBaseLine("Andalus",44,61));
	m_pFontAdapterArray->Append(ref new FontBaseLine("Batang",34,46));
	m_pFontAdapterArray->Append(ref new FontBaseLine("BatangChe",34,46));
	m_pFontAdapterArray->Append(ref new FontBaseLine("Bauhaus 93",37,58));
	m_pFontAdapterArray->Append(ref new FontBaseLine("Cambria Math",32,47));
	m_pFontAdapterArray->Append(ref new FontBaseLine("DFKai-SB",33,48));
	m_pFontAdapterArray->Append(ref new FontBaseLine("DokChampa",40,53));
	m_pFontAdapterArray->Append(ref new FontBaseLine("Dotum",34,46));
	m_pFontAdapterArray->Append(ref new FontBaseLine("DotumChe",34,46));
	m_pFontAdapterArray->Append(ref new FontBaseLine("Forte",34,55));
	m_pFontAdapterArray->Append(ref new FontBaseLine("Gabriola",29,68));
	m_pFontAdapterArray->Append(ref new FontBaseLine("Gulim",36,46));
	m_pFontAdapterArray->Append(ref new FontBaseLine("GulimChe",36,46));
	m_pFontAdapterArray->Append(ref new FontBaseLine("Gungsuh",36,46));
	m_pFontAdapterArray->Append(ref new FontBaseLine("GungsuhChe",36,46));
	m_pFontAdapterArray->Append(ref new FontBaseLine("Informal Roman",32,48));
	m_pFontAdapterArray->Append(ref new FontBaseLine("Kalinga",40,64));
	m_pFontAdapterArray->Append(ref new FontBaseLine("Latha",41,66));
	m_pFontAdapterArray->Append(ref new FontBaseLine("Levenim MT",38,56));
	m_pFontAdapterArray->Append(ref new FontBaseLine("Lucida Bright",38,47));
	m_pFontAdapterArray->Append(ref new FontBaseLine("Lucida Calligraphy",38,54));
	m_pFontAdapterArray->Append(ref new FontBaseLine("Lucida Handwriting",38,56));
	m_pFontAdapterArray->Append(ref new FontBaseLine("MingLiU",34,47));
	m_pFontAdapterArray->Append(ref new FontBaseLine("MingLiU-ExtB",34,47));
	m_pFontAdapterArray->Append(ref new FontBaseLine("MingLiU_HKSCS",34,47));
	m_pFontAdapterArray->Append(ref new FontBaseLine("MingLiU_HKSCS-ExtB",34,47));
	m_pFontAdapterArray->Append(ref new FontBaseLine("PMingLiU",34,47));
	m_pFontAdapterArray->Append(ref new FontBaseLine("PMingLiU-ExtB",34,47));
	m_pFontAdapterArray->Append(ref new FontBaseLine("Raavi",41,71));
	m_pFontAdapterArray->Append(ref new FontBaseLine("Shruti",42,73));
	m_pFontAdapterArray->Append(ref new FontBaseLine("Tunga",41,71));
	m_pFontAdapterArray->Append(ref new FontBaseLine("仿宋",36,45));
	m_pFontAdapterArray->Append(ref new FontBaseLine("华文中宋",39,52));
	m_pFontAdapterArray->Append(ref new FontBaseLine("华文仿宋",36,52));
	m_pFontAdapterArray->Append(ref new FontBaseLine("华文宋体",36,52));
	m_pFontAdapterArray->Append(ref new FontBaseLine("华文楷体",36,52));
	m_pFontAdapterArray->Append(ref new FontBaseLine("华文细黑",36,55));
	m_pFontAdapterArray->Append(ref new FontBaseLine("宋体",36,45));
	m_pFontAdapterArray->Append(ref new FontBaseLine("幼圆",36,44));
	m_pFontAdapterArray->Append(ref new FontBaseLine("新宋体",36,45));
	m_pFontAdapterArray->Append(ref new FontBaseLine("楷体",36,45));
	m_pFontAdapterArray->Append(ref new FontBaseLine("隶书",36,45));
	m_pFontAdapterArray->Append(ref new FontBaseLine("黑体",36,45));
}