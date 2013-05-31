#pragma once

#include "pch.h"

namespace Haozhaopian {

	public ref class VignetteMask sealed : Windows::UI::Xaml::Media::Imaging::SurfaceImageSource
	{
	public:
		VignetteMask(int pixelWidth, int pixelHeight, bool isOpaque);

		void BeginDraw(Windows::Foundation::Rect updateRect);
		void BeginDraw()    { BeginDraw(Windows::Foundation::Rect(0, 0, (float)m_width, (float)m_height)); }
		void EndDraw();

		void SetDpi(float dpi);

		void Clear(Windows::UI::Color color);
		void RenderLine();
		void RenderRound();
		void release();

		property double RoundCenterX
		{
			void set(double v)
			{
				m_roundCenterX = v;
			}
			double get()
			{
				return m_roundCenterX;
			}
		}

		property double RoundCenterY
		{
			void set(double v)
			{
				m_roundCenterY = v;
			}
			double get()
			{
				return m_roundCenterY;
			}
		}

		property double RoundInsideRadius
		{
			void set(double v)
			{
				m_roundInsideRadius = v;
			}
			double get()
			{
				return m_roundInsideRadius;
			}
		}

		property double RoundOutsideRadius
		{
			void set(double v)
			{
				m_roundOutsideRadius = v;
			}
			double get()
			{
				return m_roundOutsideRadius;
			}
		}

		property double LineInsideDistance
		{
			void set(double v)
			{
				m_lineInsideDistance = v;
			}
			double get()
			{
				return m_lineInsideDistance;
			}
		}

		property double LineOutsideDistance
		{
			void set(double v)
			{
				m_lineOutsideDistance = v;
			}
			double get()
			{
				return m_lineOutsideDistance;
			}
		}

		property double OutSideLine1X
		{
			void set(double v)
			{
				m_outsideLine1X = v;
			}
			double get()
			{
				return m_outsideLine1X;
			}
		}

		property double OutSideLine1Y
		{
			void set(double v)
			{
				m_outsideLine1Y = v;
			}
			double get()
			{
				return m_outsideLine1Y;
			}
		}

		property double OutSideLine2X
		{
			void set(double v)
			{
				m_outsideLine2X = v;
			}
			double get()
			{
				return m_outsideLine2X;
			}
		}

		property double OutSideLine2Y
		{
			void set(double v)
			{
				m_outsideLine2Y = v;
			}
			double get()
			{
				return m_outsideLine2Y;
			}
		}
	private protected:
		void CreateDeviceIndependentResources();
		void CreateDeviceResources();

		Microsoft::WRL::ComPtr<ISurfaceImageSourceNative>   m_sisNative;

		// Direct3D device
		Microsoft::WRL::ComPtr<ID3D11Device>                m_d3dDevice;

		// Direct2D objects
		Microsoft::WRL::ComPtr<ID2D1Device>                 m_d2dDevice;
		Microsoft::WRL::ComPtr<ID2D1DeviceContext>          m_d2dContext;
		Microsoft::WRL::ComPtr<ID2D1Factory1>				m_d2dFactory;
		
		int                                                 m_width;
		int                                                 m_height;

		//round shift
		FLOAT												m_roundCenterX;
		FLOAT												m_roundCenterY;
		FLOAT												m_roundInsideRadius;
		FLOAT												m_roundOutsideRadius;

		//line shift
		FLOAT												m_lineInsideDistance;
		FLOAT												m_lineOutsideDistance;
		FLOAT												m_outsideLine1X;
		FLOAT												m_outsideLine1Y;
		FLOAT												m_outsideLine2X;
		FLOAT												m_outsideLine2Y;

	};

}