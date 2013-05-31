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
	/*m_width = pixelWidth;
	m_height = pixelHeight;
	CreateDeviceIndependentResources();
	CreateDeviceResources();*/
}