//
// MainPage.xaml.h
// MainPage 类的声明。
//

#pragma once

#include "MainPage.g.h"

namespace TextDemo
{
	/// <summary>
	/// 可用于自身或导航至 Frame 内部的空白页。
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
	private:
		Concurrency::task<void> loadImage();
		double getFitScale();

		Windows::UI::Xaml::Media::Imaging::WriteableBitmap^ m_previewImg;
		double m_imgNowW;
		double m_imgNowH;
		double m_scale;
		
	};

}
