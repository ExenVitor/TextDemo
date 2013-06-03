//
// TextControl.xaml.cpp
// TextControl 类的实现
//

#include "pch.h"
#include "TextControl.xaml.h"
#include "TextCanvasControl.xaml.h"
#include "TextLayoutItem.xaml.h"
#include "MainPage.xaml.h"

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

// “用户控件”项模板在 http://go.microsoft.com/fwlink/?LinkId=234236 上提供

TextControl::TextControl(MainPage^ page)
{
	InitializeComponent();
	m_pAdjustPage = page;
}

TextAttribute^ TextControl::createTextAttribute()
{
	auto attri = ref new TextAttribute();
	attri->textContent = L"Hello World!";
	attri->size = 56;
	attri->textFamily=L"Gabriola";
	attri->style = TextDemo::FontStyle::STYLE_NORMAL;

	attri->color = Windows::UI::Colors::White;
	attri->colorIndex = 0;
	attri->angle = 0;

	attri->alpha = 100;
	attri->scale = 1.0;

	return attri;
}

void TextDemo::TextControl::onAddClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_currentTextItem = ref new TextLayoutItem(this);
	m_pAdjustPage->getTextCanvasControl()->addTextLayoutItem(m_currentTextItem);
}
