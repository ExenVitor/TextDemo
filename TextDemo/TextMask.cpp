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
}

