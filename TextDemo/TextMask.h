#pragma once

#include "pch.h"

namespace TextDemo{

	public ref class TextMask sealed : Windows::UI::Xaml::Media::Imaging::SurfaceImageSource
	{
		TextMask(int pixelWidth, int pixelHeight, bool isOpaque);
	};
}