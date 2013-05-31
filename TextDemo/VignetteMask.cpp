#include "pch.h"
#include "DirectXHelper.h"
#include "VignetteMask.h"

using namespace D2D1;
using namespace Platform;
using namespace Haozhaopian;
using namespace Microsoft::WRL;
using namespace Windows::Storage;
using namespace Windows::ApplicationModel;

const float defaultDpi = 96.0;

VignetteMask::VignetteMask(int pixelWidth, int pixelHeight, bool isOpaque):
	SurfaceImageSource(pixelWidth,pixelHeight,isOpaque)
{
	m_width = pixelWidth;
	m_height = pixelHeight;
	CreateDeviceIndependentResources();
	CreateDeviceResources();
}

void VignetteMask::release()
{
	m_sisNative = nullptr;
	m_d3dDevice = nullptr;
	m_d2dContext = nullptr;
	m_d2dDevice = nullptr;
	m_d2dFactory = nullptr;
}

// Initialize resources that are independent of hardware.
void VignetteMask::CreateDeviceIndependentResources()
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
}

// Initialize hardware-dependent resources.
void VignetteMask::CreateDeviceResources()
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

// Sets the current DPI.
void VignetteMask::SetDpi(float dpi)
{
    // Update Direct2D's stored DPI.
    m_d2dContext->SetDpi(dpi, dpi);
}

// Begins drawing, allowing updates to content in the specified area.
void VignetteMask::BeginDraw(Windows::Foundation::Rect updateRect)
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

}

void VignetteMask::RenderRound()
{
	m_d2dContext->Clear(ColorF(0.0f,0.0f,0.0f,0.0f));

	m_d2dContext->SetTransform(Matrix3x2F::Identity());

	ComPtr<ID2D1GeometryGroup> pGeoGroup;
	ComPtr<ID2D1EllipseGeometry> pInsideGeo;
	ComPtr<ID2D1RectangleGeometry> pViewRectGeo;
	ComPtr<ID2D1GradientStopCollection> pGradientStops;
	ComPtr<ID2D1RadialGradientBrush> pRadialGradientBrush;


	

	const D2D1_ELLIPSE ellipseInside = D2D1::Ellipse(
		D2D1::Point2F(m_roundCenterX,m_roundCenterY),
		m_roundInsideRadius,
		m_roundInsideRadius);

	const D2D1_ELLIPSE ellipseOutside = D2D1::Ellipse(
		D2D1::Point2F(m_roundCenterX,m_roundCenterY),
		m_roundOutsideRadius,
		m_roundOutsideRadius);

	const D2D1_RECT_F viewRect = D2D1::RectF(
		0,
		0,
		(FLOAT)m_width,
		(FLOAT)m_height);	

	DX::ThrowIfFailed(
		m_d2dFactory->CreateRectangleGeometry(
		&viewRect,
		pViewRectGeo.GetAddressOf()
		)		
		);	

	DX::ThrowIfFailed(
		m_d2dFactory->CreateEllipseGeometry(
		&ellipseInside,
		pInsideGeo.GetAddressOf()
		)
		);

	ID2D1Geometry *ppGeometries[] = 
	{
		pInsideGeo.Get(),
		pViewRectGeo.Get()
	};

	DX::ThrowIfFailed(
		m_d2dFactory->CreateGeometryGroup(
		D2D1_FILL_MODE_ALTERNATE,
		ppGeometries,
		ARRAYSIZE(ppGeometries),
		pGeoGroup.GetAddressOf()
		)
		);

	FLOAT gradientStartPoint = m_roundInsideRadius / m_roundOutsideRadius;

	D2D1_GRADIENT_STOP gradientStop[6];	
	gradientStop[0].color = D2D1::ColorF(D2D1::ColorF::White,0.0f);
	gradientStop[0].position = 0.0f;
	gradientStop[1].color = D2D1::ColorF(D2D1::ColorF::White,0.0f);
	gradientStop[1].position = gradientStartPoint;
	gradientStop[2].color = D2D1::ColorF(D2D1::ColorF::White,0.5f);
	gradientStop[2].position = gradientStartPoint + (1.0f - gradientStartPoint) / 2.0f * 1.2;
	gradientStop[3].color = D2D1::ColorF(D2D1::ColorF::White,0.65f);
	gradientStop[3].position = gradientStartPoint + (1.0f - gradientStartPoint) / 2.0f * 1.6;
	gradientStop[4].color = D2D1::ColorF(D2D1::ColorF::White,0.75f);
	gradientStop[4].position = gradientStartPoint + (1.0f - gradientStartPoint) / 2.0f * 1.8;
	/*gradientStop[1].color = D2D1::ColorF(D2D1::ColorF::White,0.8f - 0.8 * 0.6f);
	gradientStop[1].position = gradientStartPoint + (1.0f - gradientStartPoint) * 0.3;
	gradientStop[2].color = D2D1::ColorF(D2D1::ColorF::White,0.8f - 0.8 * 0.3f);
	gradientStop[2].position = gradientStartPoint + (1.0f - gradientStartPoint) * 0.6;
	gradientStop[3].color = D2D1::ColorF(D2D1::ColorF::White,0.8f - 0.8f * 0.05f);
	gradientStop[3].position = gradientStartPoint + (1.0f - gradientStartPoint) * 0.8;*/
	gradientStop[5].color = D2D1::ColorF(D2D1::ColorF::White,0.8f);
	gradientStop[5].position = 1.0f;

	DX::ThrowIfFailed(
		m_d2dContext->CreateGradientStopCollection(
		gradientStop,
		ARRAYSIZE(gradientStop),
		D2D1_GAMMA_2_2,
		D2D1_EXTEND_MODE_CLAMP,
		pGradientStops.GetAddressOf()
		)
		);

	DX::ThrowIfFailed(
		m_d2dContext->CreateRadialGradientBrush(
		D2D1::RadialGradientBrushProperties(
		D2D1::Point2F(m_roundCenterX,m_roundCenterY),
		D2D1::Point2F(0.f,0.f),
		m_roundOutsideRadius,
		m_roundOutsideRadius),
		pGradientStops.Get(),
		pRadialGradientBrush.GetAddressOf()
		)
		);



	m_d2dContext->FillGeometry(pViewRectGeo.Get(),pRadialGradientBrush.Get());


}

void VignetteMask::RenderLine()
{
	m_d2dContext->Clear(ColorF(0.0f,0.0f,0.0f,0.0f));
	
	m_d2dContext->SetTransform(Matrix3x2F::Identity());

	ComPtr<ID2D1GeometryGroup> pGeoGroup;	
	ComPtr<ID2D1RectangleGeometry> pViewRectGeo;
	ComPtr<ID2D1GradientStopCollection> pGradientStops;	
	ComPtr<ID2D1LinearGradientBrush> pLinearGradientBrush;
	
	const D2D1_RECT_F viewRect = D2D1::RectF(
		0,
		0,
		(FLOAT)m_width,
		(FLOAT)m_height);	

	DX::ThrowIfFailed(
		m_d2dFactory->CreateRectangleGeometry(
		&viewRect,
		pViewRectGeo.GetAddressOf()
		)		
		);	
	
	FLOAT sumDistance = 2.0 * m_lineOutsideDistance;
	FLOAT gradientPoint1 = (m_lineOutsideDistance - m_lineInsideDistance) / sumDistance;
	//FLOAT gradientPoint2 = gradientPoint1 + (sumDistance - 2.0 * gradientPoint1) / sumDistance;
	

	D2D1_GRADIENT_STOP gradientStop[4];	
	gradientStop[0].color = D2D1::ColorF(D2D1::ColorF::White,0.8f);
	gradientStop[0].position = 0.0f;
	gradientStop[1].color = D2D1::ColorF(D2D1::ColorF::White,0.0f);
	gradientStop[1].position = gradientPoint1;
	gradientStop[2].color = D2D1::ColorF(D2D1::ColorF::White,0.0f);
	gradientStop[2].position = 1.0f - gradientPoint1;
	gradientStop[3].color = D2D1::ColorF(D2D1::ColorF::White,0.8f);
	gradientStop[3].position = 1.0f;
	
	DX::ThrowIfFailed(
		m_d2dContext->CreateGradientStopCollection(
		gradientStop,
		ARRAYSIZE(gradientStop),
		D2D1_GAMMA_2_2,
		D2D1_EXTEND_MODE_CLAMP,
		pGradientStops.GetAddressOf()
		)
		);

	DX::ThrowIfFailed(
		m_d2dContext->CreateLinearGradientBrush(
		D2D1::LinearGradientBrushProperties(
		D2D1::Point2F(m_outsideLine1X,m_outsideLine1Y),
		D2D1::Point2F(m_outsideLine2X,m_outsideLine2Y)),
		pGradientStops.Get(),
		pLinearGradientBrush.GetAddressOf()
		)
		);
	
	
	m_d2dContext->FillGeometry(pViewRectGeo.Get(),pLinearGradientBrush.Get());

	pGeoGroup = nullptr;
	pViewRectGeo = nullptr;
	pGradientStops = nullptr;
	pLinearGradientBrush = nullptr;
}


void VignetteMask::EndDraw()
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

// Clears the background with the given color.
void VignetteMask::Clear(Windows::UI::Color color)
{
    m_d2dContext->Clear(DX::ConvertToColorF(color));
}

