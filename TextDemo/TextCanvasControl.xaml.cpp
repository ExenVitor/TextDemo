//
// TextCanvasControl.xaml.cpp
// TextCanvasControl 类的实现
//

#include "pch.h"
#include "TextCanvasControl.xaml.h"

using namespace TextDemo;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace concurrency;

// “用户控件”项模板在 http://go.microsoft.com/fwlink/?LinkId=234236 上提供

TextCanvasControl::TextCanvasControl()
{
	InitializeComponent();
}

void TextCanvasControl::initTextCanvas(double width,double height,double scale,Windows::UI::Xaml::Media::Imaging::WriteableBitmap^ previewImg)
{
	maskView->Width = width;
	maskView->Height = height;

	textCanvas->Width = width;
	textCanvas->Height = height;

	textCanvas->Children->Clear();
	auto clipRect = ref new RectangleGeometry();
	clipRect->Rect = Rect(0,0,width,height);
	textCanvas->Clip = clipRect;

	m_scale = scale;
	m_previewImg = previewImg;
	UpdateLayout();
}