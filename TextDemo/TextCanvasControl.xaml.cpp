//
// TextCanvasControl.xaml.cpp
// TextCanvasControl 类的实现
//

#include "pch.h"
#include "TextCanvasControl.xaml.h"
#include "TextLayoutItem.xaml.h"
#include "TextMask.h"
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
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace concurrency;

// “用户控件”项模板在 http://go.microsoft.com/fwlink/?LinkId=234236 上提供

extern void bringToFront(Windows::UI::Xaml::Controls::Canvas^ parent,Windows::UI::Xaml::UIElement^ child);

TextCanvasControl::TextCanvasControl(MainPage^ page)
{
	InitializeComponent();
	m_pAdjustPage = page;
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

	m_textMask = nullptr;
	m_textMask = ref new TextMask(width,height,false);
	updateTextMask();
	maskView->Source = m_textMask;

	UpdateLayout();
}

void TextCanvasControl::updateTextMask()
{
	m_textMask->pTextAttributes = getItemAttributes();
	m_textMask->BeginDraw();
	m_textMask->Render();
	m_textMask->EndDraw();
}

void TextCanvasControl::addTextLayoutItem(TextLayoutItem^ item)
{
	item->setCanvasControl(this);
	textCanvas->Children->Append(item);
	textCanvas->UpdateLayout();
	item->UpdateLayout();
	bringToFront(textCanvas,item);
	
}



Platform::Array<TextAttribute^>^ TextCanvasControl::getItemAttributes()
{
	auto result=ref new Platform::Array<TextAttribute^>(textCanvas->Children->Size);
	for(int i=0;i<textCanvas->Children->Size;i++){
		result[i]=((TextLayoutItem^)textCanvas->Children->GetAt(i))->getTextAttribute();
		result[i]->Zindex=textCanvas->GetZIndex(textCanvas->Children->GetAt(i));
	}
	//按zindex排序
	int k;
	int j;
	TextAttribute^ tmp;
	for(int i=0;i<result->Length;i++){
		k=i;
		for(j=i+1;j<result->Length;j++){
			if(result[k]->Zindex>result[j]->Zindex){
				k=j;
			}
		}
		if(k!=i){
			tmp=result[k];
			result[k]=result[i];
			result[i]=tmp;
		}
	}

	return result;
}