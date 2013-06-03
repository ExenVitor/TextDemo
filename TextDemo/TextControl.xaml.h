//
// TextControl.xaml.h
// TextControl 类的声明
//

#pragma once

#include "TextControl.g.h"
#include "TextLayoutItem.xaml.h"

namespace TextDemo
{
	ref class MainPage;
	ref class TextLayoutItem;
	ref class TextAttribute;
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class TextControl sealed
	{
	public:
		TextControl(MainPage^ page);
		TextAttribute^ createTextAttribute();

	private:
		void onAddClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		MainPage^			m_pAdjustPage;
		TextLayoutItem^		m_currentTextItem;
	};
}
