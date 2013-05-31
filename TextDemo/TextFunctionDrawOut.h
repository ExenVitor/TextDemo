#pragma once
#include "DirectXBase.h"
#include "TextFunctionItem.xaml.h"

namespace Haozhaopian
{
	ref class TextAttribute;
	ref class TemplateAttribute;

	public ref class FontBaseLine sealed{
	public:
		FontBaseLine(){};
		FontBaseLine(Platform::String^ name,int baseline,int linespace){
			this->fontName=name;
			this->baseLine=baseline;
			this->lineSpace=linespace;
		}
		property int												baseLine;
		property int												lineSpace;
		property Platform::String^									fontName;
		
	};

	ref class TextFunctionDrawOut sealed : public DirectXBase
	{
	public:
		property int												BitmapWidth;
		property int												BitmapHeight;
		TextFunctionDrawOut(Windows::UI::Core::CoreWindow^ window, float dpi);
		// DirectXBase 方法。
		virtual void CreateDeviceIndependentResources() override;
		virtual void CreateDeviceResources() override;
		virtual void CreateWindowSizeDependentResources() override;
		//onDraw
		virtual void Render() override;
		//void setTextAttributes(Platform::Collections::Vector<Haozhaopian::TextAttribute ^> ^ vector){m_pTextAttribute=vector;};
		Platform::Array<byte>^ getArray() {return pData;}
		property Platform::Array<Haozhaopian::TextAttribute ^> ^ 	m_pTextAttributes;
		//画图片
		void drawBitmap(TextAttribute^ attribute);
		//画文本
		void drawText(TextAttribute^ attribute);
		//设置bitmap参数
		void setBitmapArgus(double width,double height,double orgScale);
	private:
		//初始化字体适配表
		void InitializeAdapter();
		//取出字体适配数据
		FontBaseLine^ getFontBaseLineFromName(Platform::String^ fontname);
	private:		
		Microsoft::WRL::ComPtr<ID2D1Bitmap1>							m_pRenderBitmap;
		Platform::Array<byte>^											pData;
		D2D1_SIZE_F														bitmapSize;
		double															orgScale;
		Platform::Collections::Vector<Haozhaopian::FontBaseLine^>^		m_pFontAdapterArray;
		
	};
}