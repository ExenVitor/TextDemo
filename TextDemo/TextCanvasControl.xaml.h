//
// TextCanvasControl.xaml.h
// TextCanvasControl 类的声明
//

#pragma once

#include "TextCanvasControl.g.h"

using namespace Windows::UI::Xaml::Media::Imaging;

namespace TextDemo
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class TextCanvasControl sealed
	{
	public:
		TextCanvasControl();
		void initTextCanvas(double width,double height,double scale,WriteableBitmap^ previewImg);

	private:
		double				m_scale;
		WriteableBitmap^	m_previewImg;
	};
}
