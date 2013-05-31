﻿//
// MainPage.xaml.cpp
// MainPage 类的实现。
//

#include "pch.h"
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
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace concurrency;

// “空白页”项模板在 http://go.microsoft.com/fwlink/?LinkId=234238 上有介绍

const int drawSpace = 60;


MainPage::MainPage()
{
	InitializeComponent();
}

/// <summary>
/// 在此页将要在 Frame 中显示时进行调用。
/// </summary>
/// <param name="e">描述如何访问此页的事件数据。Parameter
/// 属性通常用于配置页。</param>
void MainPage::OnNavigatedTo(NavigationEventArgs^ e)
{
	(void) e;	// 未使用的参数

	loadImage();
}


task<void> MainPage::loadImage()
{
	String ^path = "Assets\\sample.jpg";
	m_currentImg = nullptr;
	m_currentImg = ref new WriteableBitmap(10,10);
	auto packageFolder = Windows::ApplicationModel::Package::Current->InstalledLocation;
	return create_task(packageFolder->GetFileAsync(path)).then([this](StorageFile^ file){
		return file->OpenAsync(FileAccessMode::Read);
	}).then([this](IRandomAccessStream^ stream){
		m_currentImg->SetSource(stream);
		auto callback = ref new Windows::UI::Core:: DispatchedHandler([this]()
		{
			m_scale = getFitScale();
			double imgPixW = m_currentImg->PixelWidth;
			double imgPixH = m_currentImg->PixelHeight;
			m_imgNowW = imgPixW * m_scale;
			m_imgNowH = imgPixH * m_scale;

			imageView->Width = m_imgNowW;
			imageView->Height = m_imgNowH;

			maskView->Width = m_imgNowW;
			maskView->Height = m_imgNowH;

			textCanvas->Width = m_imgNowW;
			textCanvas->Height = m_imgNowH;

			textCanvas->Children->Clear();
			auto clipRect = ref new RectangleGeometry();
			clipRect->Rect = Rect(0,0,m_imgNowW,m_imgNowH);
			textCanvas->Clip = clipRect;

			imageView->Source = m_currentImg;

		});
		return Dispatcher->RunAsync(Windows::UI::Core:: CoreDispatcherPriority ::Normal, callback);
	});

}

double MainPage::getFitScale()
{
	double fitScale = 1.0;
	
	double w = m_currentImg->PixelWidth;
	double h = m_currentImg->PixelHeight;

	double canvasWidth = displayGrid->ActualWidth - drawSpace;
	double canvasHeight = displayGrid->ActualHeight - drawSpace;

	fitScale = canvasWidth / w < canvasHeight / h ? canvasWidth / w : canvasHeight / h;

	if(fitScale > 1.0)
		fitScale = 1.0;
	return fitScale;
}