#pragma once

#include "pch.h"
#include "TextLayoutItem.xaml.h"

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

		property Platform::Array<TextAttribute ^> ^ 	pTextAttributes;
	private protected:
		void CreateDeviceIndependentResources();
		void CreateDeviceResources();
		void Clear(Windows::UI::Color color);

		void RenderText(TextAttribute^ attri);

		Microsoft::WRL::ComPtr<ISurfaceImageSourceNative>   m_sisNative;

		// Direct3D device
		Microsoft::WRL::ComPtr<ID3D11Device>                m_d3dDevice;

		// Direct2D objects
		Microsoft::WRL::ComPtr<ID2D1Device>                 m_d2dDevice;
		Microsoft::WRL::ComPtr<ID2D1DeviceContext>          m_d2dContext;
		Microsoft::WRL::ComPtr<ID2D1Factory1>				m_d2dFactory;

		Microsoft::WRL::ComPtr<IDWriteFactory1>				m_dwriteFactory;

		int                                                 m_width;
		int                                                 m_height;
	};
}