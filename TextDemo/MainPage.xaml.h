//
// MainPage.xaml.h
// MainPage 类的声明。
//

#pragma once

#include "MainPage.g.h"

namespace TextDemo
{
	ref class TextCanvasControl;
	ref class TextControl;
	/// <summary>
	/// 可用于自身或导航至 Frame 内部的空白页。
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

		TextControl^ getTextControl() {return m_pTextControl;}
		TextCanvasControl^ getTextCanvasControl() {return m_pTextCanvasControl;}

	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
	private:
		Concurrency::task<void> loadImage();
		void initControls();
		double getFitScale();

		TextControl^			m_pTextControl;
		TextCanvasControl^		m_pTextCanvasControl;
		Windows::UI::Xaml::Media::Imaging::WriteableBitmap^ m_previewImg;
		double m_imgNowW;
		double m_imgNowH;
		double m_scale;

		
	};

}
