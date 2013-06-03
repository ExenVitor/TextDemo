#include "pch.h"
#include "DirectXHelper.h"
#include "TextMask.h"

using namespace D2D1;
using namespace Platform;
using namespace TextDemo;
using namespace Microsoft::WRL;
using namespace Windows::Storage;
using namespace Windows::ApplicationModel;

const float defaultDpi = 96.0;

TextMask::TextMask(int pixelWidth, int pixelHeight, bool isOpaque):
	SurfaceImageSource(pixelWidth,pixelHeight,isOpaque)
{
	m_width = pixelWidth;
	m_height = pixelHeight;
	CreateDeviceIndependentResources();
	CreateDeviceResources();
}

void TextMask::SetDpi(float dpi)
{
	m_d2dContext->SetDpi(dpi,dpi);
}

// Initialize resources that are independent of hardware.
void TextMask::CreateDeviceIndependentResources()
{
	// Query for ISurfaceImageSourceNative interface.
	DX::ThrowIfFailed(
		reinterpret_cast<IUnknown*>(this)->QueryInterface(IID_PPV_ARGS(&m_sisNative))
		);

	D2D1_FACTORY_OPTIONS options;
	ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));

#if defined(_DEBUG)
	// If the project is in a debug build, enable Direct2D debugging via SDK Layers.
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

	DX::ThrowIfFailed(
		D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		__uuidof(ID2D1Factory1),
		&options,
		&m_d2dFactory
		)
		);

	DX::ThrowIfFailed(
		DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		&m_dwriteFactory
		)
		);
}

// Initialize hardware-dependent resources.
void TextMask::CreateDeviceResources()
{
    // This flag adds support for surfaces with a different color channel ordering
    // than the API default. It is required for compatibility with Direct2D.
    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT; 

#if defined(_DEBUG)    
    // If the project is in a debug build, enable debugging via SDK Layers.
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    // This array defines the set of DirectX hardware feature levels this app will support.
    // Note the ordering should be preserved.
    // Don't forget to declare your application's minimum required feature level in its
    // description.  All applications are assumed to support 9.1 unless otherwise stated.
    const D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    };

    // Create the Direct3D 11 API device object.
    DX::ThrowIfFailed(
        D3D11CreateDevice(
            nullptr,                        // Specify nullptr to use the default adapter.
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            creationFlags,                  // Set debug and Direct2D compatibility flags.
            featureLevels,                  // List of feature levels this app can support.
            ARRAYSIZE(featureLevels),
            D3D11_SDK_VERSION,              // Always set this to D3D11_SDK_VERSION for Metro style apps.
            &m_d3dDevice,                   // Returns the Direct3D device created.
            nullptr,
            nullptr
            )
        );

    // Get the Direct3D 11.1 API device.
    ComPtr<IDXGIDevice> dxgiDevice;
    DX::ThrowIfFailed(
        m_d3dDevice.As(&dxgiDevice)
        );
	
    // Create the Direct2D device object and a corresponding context.
    DX::ThrowIfFailed(
		m_d2dFactory->CreateDevice(dxgiDevice.Get(),&m_d2dDevice)
        
        );

    DX::ThrowIfFailed(
        m_d2dDevice->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
            &m_d2dContext
            )
        );

    // Set DPI to the display's current DPI.
    SetDpi(defaultDpi);

    // Associate the DXGI device with the SurfaceImageSource.
    DX::ThrowIfFailed(
        m_sisNative->SetDevice(dxgiDevice.Get())
        );


}

// Clears the background with the given color.
void TextMask::Clear(Windows::UI::Color color)
{
    m_d2dContext->Clear(DX::ConvertToColorF(color));
}

void TextMask::BeginDraw(Windows::Foundation::Rect updateRect)
{
	POINT offset;
	ComPtr<IDXGISurface> surface;

	// Express target area as a native RECT type.
	RECT updateRectNative; 
	updateRectNative.left = static_cast<LONG>(updateRect.Left);
	updateRectNative.top = static_cast<LONG>(updateRect.Top);
	updateRectNative.right = static_cast<LONG>(updateRect.Right);
	updateRectNative.bottom = static_cast<LONG>(updateRect.Bottom);

	// Begin drawing - returns a target surface and an offset to use as the top left origin when drawing.
	HRESULT beginDrawHR = m_sisNative->BeginDraw(updateRectNative, &surface, &offset);

	if (beginDrawHR == DXGI_ERROR_DEVICE_REMOVED || beginDrawHR == DXGI_ERROR_DEVICE_RESET)
	{
		// If the device has been removed or reset, attempt to recreate it and continue drawing.
		CreateDeviceResources();
		BeginDraw(updateRect);
	}
	else
	{
		// Notify the caller by throwing an exception if any other error was encountered.
		DX::ThrowIfFailed(beginDrawHR);
	}

	// Create render target.
	ComPtr<ID2D1Bitmap1> bitmap;
	DX::ThrowIfFailed(
		m_d2dContext->CreateBitmapFromDxgiSurface(
		surface.Get(),
		nullptr,
		&bitmap
		)
		);

	// Set context's render target.
	m_d2dContext->SetTarget(bitmap.Get());

	// Begin drawing using D2D context.
	m_d2dContext->BeginDraw();

	Clear(Windows::UI::Colors::Transparent);
}



void TextMask::EndDraw()
{
   
    // Remove the render target and end drawing.
    DX::ThrowIfFailed(
        m_d2dContext->EndDraw()
        );

    m_d2dContext->SetTarget(nullptr);

    DX::ThrowIfFailed(
        m_sisNative->EndDraw()
        );
}

void TextMask::RenderText(TextAttribute^ attri)
{
	Microsoft::WRL::ComPtr<IDWriteTextFormat> m_textFormat;
	Microsoft::WRL::ComPtr<IDWriteTextLayout> m_textLayout;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pBrush;
	DWRITE_TEXT_METRICS m_textMetrics;
	DX::ThrowIfFailed(
		m_dwriteFactory->CreateTextFormat(
		attri->textFamily->Data(),
		nullptr,
		(int)(attri->style&TextDemo::FontStyle::STYLE_BOLD) ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
		(int)(attri->style& TextDemo::FontStyle::STYLE_OBLIQUE) ? DWRITE_FONT_STYLE_OBLIQUE : DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		50,
		L"en-US",
		&m_textFormat
		)
		);
	DX::ThrowIfFailed(
		m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING)
		);

	DX::ThrowIfFailed(
		m_d2dContext->CreateSolidColorBrush(
		D2D1::ColorF(attri->color.R/255.0,attri->color.G/255.0,attri->color.B/255.0,attri->alpha/100.0),
		&m_pBrush
		)
		);
	Platform::String^ text = attri->textContent;
	float width=5000;
	float height=5000;
	

	DX::ThrowIfFailed(
		m_dwriteFactory->CreateTextLayout(
		text->Data(),
		text->Length(),
		m_textFormat.Get(),
		width, // maxWidth¡£
		height, // maxHeight¡£
		&m_textLayout
		)
		);

	DWRITE_TEXT_RANGE textRange;
	textRange.length=text->Length();
	textRange.startPosition=0;


	DX::ThrowIfFailed(
		m_textLayout->SetUnderline((int)(attri->style&TextDemo::FontStyle::STYLE_UNDERLINE)?true:false,
		textRange)
		);

	////×ÖÌåÊÊÅä
	//FontBaseLine^ baseline=getFontBaseLineFromName(attribute->textFamily);
	//if(baseline!=nullptr){
	//	DX::ThrowIfFailed(
	//		m_textLayout->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_UNIFORM,
	//		baseline->lineSpace/orgScale*(attribute->size/40),
	//		baseline->baseLine/orgScale*(attribute->size/40))
	//		);
	//}
	DX::ThrowIfFailed(
		m_textLayout->GetMetrics(&m_textMetrics)
		);

	
	float left=attri->left;
	float top=attri->top;

	float centerX;
	float centerY;
	

	centerX=left+(attri->width/2.0);
	centerY=top+(attri->height/2.0);

	
	Matrix3x2F translation = Matrix3x2F::Rotation(attri->angle,
		D2D1::Point2F(
		centerX,centerY
		)
		);	
	
	translation.SetProduct(
		translation,
		Matrix3x2F::Scale(attri->scale,attri->scale,D2D1::Point2F(
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

void TextMask::Render()
{
	int attriSize = pTextAttributes->Length;
	for(int i = 0;i<attriSize;i++)
	{
		RenderText(pTextAttributes[i]);
	}
}