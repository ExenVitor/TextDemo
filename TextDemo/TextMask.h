#pragma once

#include "pch.h"
#include "ILayoutItem.h"
#include "CustomTextRenderer.h"

namespace TextDemo{

	ref class TextAttribute;
	public ref class TextMask sealed : Windows::UI::Xaml::Media::Imaging::SurfaceImageSource
	{
	public:
		TextMask(int pixelWidth, int pixelHeight, bool isOpaque);
		void SetDpi(float dpi);

		void BeginDraw(Windows::Foundation::Rect updateRect);
		void BeginDraw()    { BeginDraw(Windows::Foundation::Rect(0, 0, (float)m_width, (float)m_height)); }
		void EndDraw();

		void Render();

		Platform::Array<BYTE>^ getSaveData(double width,double height,double scale);

		property Platform::Array<TextAttribute ^> ^ 	pTextAttributes;
		
		property int BitmapWidth
		{
			int get() {return _bitmapWidth;}
		}
		property int BitmapHeight
		{
			int get() {return _bitmapHeight;}
		}
	private protected:
		void CreateDeviceIndependentResources();
		void CreateDeviceResources();
		void Clear(Windows::UI::Color color);

		void RenderText(TextAttribute^ attri);
		void RenderGraphics(TextAttribute^ attri);
		void initBgimageNameArray();
		void initBgimage(int index);
		Microsoft::WRL::ComPtr<ISurfaceImageSourceNative>   m_sisNative;

		// Direct3D device
		Microsoft::WRL::ComPtr<ID3D11Device>                m_d3dDevice;

		// Direct2D objects
		Microsoft::WRL::ComPtr<ID2D1Device>                 m_d2dDevice;
		Microsoft::WRL::ComPtr<ID2D1DeviceContext>          m_d2dContext;
		Microsoft::WRL::ComPtr<ID2D1Factory1>				m_d2dFactory;

		Microsoft::WRL::ComPtr<IDWriteFactory1>				m_dwriteFactory;
		Microsoft::WRL::ComPtr<IWICImagingFactory2>			m_wicFactory;

		Microsoft::WRL::ComPtr<ID2D1Bitmap1>				m_pTargetBitmap;
		Microsoft::WRL::ComPtr<ID2D1Bitmap1>				m_pRenderBitmap;

		std::vector<Microsoft::WRL::ComPtr<ID2D1Bitmap>>    m_bgimageList;
		Platform::Collections::Vector<Platform::String^>^	m_bgimageNames;
		int                                                 m_width;
		int                                                 m_height;
		int													_bitmapWidth;
		int													_bitmapHeight;
		double												m_scale;
	};
}