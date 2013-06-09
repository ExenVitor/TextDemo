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

	[Windows::UI::Xaml::Data::Bindable]
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class TextColors sealed
	{
	public:
		TextColors(){}	    
		property Windows::UI::Color Color;			
	};

	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class TextControl sealed
	{
	public:
		TextControl(MainPage^ page);
		void initTextControl();

		TextAttribute^ createTextAttribute();

		void setCurrentItem(TextLayoutItem^ item);
		void deleteTextItem(TextLayoutItem^ item);

		property bool EventLock;


	private:
		void onAddClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void onSaveClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void onSelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		Platform::Collections::Vector<TextColors^>^ getColors();
		Platform::Collections::Vector<Windows::UI::Xaml::Media::FontFamily^>^ getFontFamily();
		TextDemo::FontStyle getCurrentFontStyle();
		void setCurrentFontStyle(TextDemo::FontStyle style);

		MainPage^			m_pAdjustPage;
		TextLayoutItem^		m_currentTextItem;
		bool				m_isChinese;
		
		void OnInputTextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e);
		void OnAlignChecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnTextStyle_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
