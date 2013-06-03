//
// TextLayoutItem.xaml.cpp
// TextLayoutItem 类的实现
//

#include "pch.h"
#include "TextLayoutItem.xaml.h"
#include "TextControl.xaml.h"

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

//移动对象到顶层
void bringToFront(Windows::UI::Xaml::Controls::Canvas^ parent,Windows::UI::Xaml::UIElement^ child){
	unsigned childCount = parent->Children->Size;
	if(childCount<=1)
		return;
	int oldZIndex=parent->GetZIndex(child);
	for(unsigned int i = 0; i<childCount;i++){
		UIElement^ childElement = parent->Children->GetAt(i);
		int iZindex=parent->GetZIndex(childElement);
		if(iZindex>oldZIndex){
			parent->SetZIndex(childElement,iZindex-1);
		}
	}
	parent->SetZIndex(child,childCount);
	
}

//获取指定文字的宽高
void getCharacterSize(Platform::String^ str,
					  Platform::String^ fontName,
					  float fontSize,
					  DWRITE_FONT_WEIGHT weight,
					  DWRITE_FONT_STYLE style,
					  DWRITE_FONT_STRETCH stretch,
					  float width,
					  float height,
					  double result[2]){
	IDWriteFactory* pDWriteFactory = NULL;
	Microsoft::WRL::ComPtr<IDWriteTextLayout> m_textLayout;
	Microsoft::WRL::ComPtr<IDWriteTextFormat> m_textFormat;
	DWRITE_TEXT_METRICS m_textMetrics;
    HRESULT hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(&pDWriteFactory)
            );
	if (SUCCEEDED(hr))
    {
		hr = pDWriteFactory->CreateTextFormat(fontName->ToString()->Data(),
			nullptr,
			weight,
			style,
			stretch,
			fontSize,
			L"en-US",
			&m_textFormat
			);
    }
	if (SUCCEEDED(hr))
    {
		hr = pDWriteFactory->CreateTextLayout(
			str->ToString()->Data(),
			str->Length(),
			m_textFormat.Get(),
			width, // maxWidth。
			height, // maxHeight。
			&m_textLayout
			);
	}
	if (SUCCEEDED(hr))
    {
		hr=m_textLayout->GetMetrics(&m_textMetrics);
	}
	if (SUCCEEDED(hr))
    {
		result[0]=m_textMetrics.width;
		result[1]=m_textMetrics.height;	
	}	
}

TextLayoutItem::TextLayoutItem(TextControl^ textControl)
{
	InitializeComponent();
	m_pTextControl = textControl;
	m_textAttribute = m_pTextControl->createTextAttribute();
}

TextAttribute^ TextLayoutItem::getTextAttribute()
{
	return m_textAttribute;
}


void TextLayoutItem::notifyChanged()
{
	double textSize[2]={0,0};
	getCharacterSize(m_textAttribute->textContent,
		m_textAttribute->textFamily,
		m_textAttribute->size,
		(int)(m_textAttribute->style&TextDemo::FontStyle::STYLE_BOLD)?DWRITE_FONT_WEIGHT_BOLD:DWRITE_FONT_WEIGHT_NORMAL,
		(int)(m_textAttribute->style&TextDemo::FontStyle::STYLE_OBLIQUE)?DWRITE_FONT_STYLE_OBLIQUE:DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		5000,5000,
		textSize);

	selectGrid->Width = textSize[0];
	selectGrid->Height = textSize[1];

	m_textAttribute->width=selectGrid->Width;
	m_textAttribute->height=selectGrid->Height;
}


void TextDemo::TextLayoutItem::UserControl_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	UpdateLayout();
	notifyChanged();
}

