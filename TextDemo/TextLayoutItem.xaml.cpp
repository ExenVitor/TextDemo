//
// TextLayoutItem.xaml.cpp
// TextLayoutItem 类的实现
//

#include "pch.h"
#include "TextLayoutItem.xaml.h"
#include "TextControl.xaml.h"
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
using namespace Windows::Devices::Input;

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
					  bool hasUnderLine,
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
			fontSize + 5,
			L"en-US",
			&m_textFormat
			);
    }
	if (SUCCEEDED(hr))
	{
		m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
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
	DWRITE_TEXT_RANGE textRange;
	textRange.length=str->Length();
	textRange.startPosition=0;


	if (SUCCEEDED(hr))
    {
		m_textLayout->SetUnderline(hasUnderLine,
		textRange);
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
	m_itemAction = ItemAction::NONE;
	eventDwon_x = 0.0;
	eventDwon_y = 0.0;
	new_x = 0.0;
	new_y = 0.0;
	oldScale = 1.0;
	oldDistance = 1.0;
	m_isChanged = false;
	m_isPressed = false;

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
		(int)(m_textAttribute->style&TextDemo::FontStyle::STYLE_UNDERLINE)?true:false,
		DWRITE_FONT_STRETCH_NORMAL,
		5000,5000,
		textSize);

	selectGrid->Width = textSize[0];
	selectGrid->Height = textSize[1];

	m_textAttribute->width=selectGrid->Width;
	m_textAttribute->height=selectGrid->Height;

	auto parent = safe_cast<Canvas^>(this->Parent);
	moveSelf((parent->ActualWidth - selectGrid->Width)/2.0,(parent->ActualHeight - selectGrid->Height)/2.0);
	m_pTextCanvasControl->updateTextMask();
}

void TextLayoutItem::moveSelf(double disX,double disY)
{
	auto parent = safe_cast<Canvas^>(this->Parent);
	double halfW = selectGrid->ActualWidth / 2.0;
	double halfH = selectGrid->ActualHeight / 2.0;
	double newX = parent->GetLeft(this) + disX;
	double newY = parent->GetTop(this) + disY;	

	if(disX != 0 || disY != 0)
		m_isChanged = true;
	parent->SetLeft(this,parent->GetLeft(this)+disX);
	parent->SetTop(this,parent->GetTop(this)+disY);
	m_textAttribute->left=parent->GetLeft(this);
	m_textAttribute->top=parent->GetTop(this);
}

void TextDemo::TextLayoutItem::UserControl_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	UpdateLayout();
	notifyChanged();
}



void TextDemo::TextLayoutItem::SelectGridPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	bringToFront((Windows::UI::Xaml::Controls::Canvas^)this->Parent,this);
	if(sender->GetType()->ToString()->Equals(selectGrid->GetType()->ToString())){
		m_itemAction=MOVE;
		eventDwon_y = e->GetCurrentPoint(selectGrid)->Position.Y;
		eventDwon_x = e->GetCurrentPoint(selectGrid)->Position.X;
	}else if(sender->GetType()->ToString()->Equals(img_rotate->GetType()->ToString())){
		m_itemAction=SCALE;
		eventDwon_y = e->GetCurrentPoint(selectGrid)->Position.Y;
		eventDwon_x=e->GetCurrentPoint(selectGrid)->Position.X;
		new_x = eventDwon_x;
		new_y = eventDwon_y;			
	}
	e->Handled=true;
	selectGrid->CapturePointer(e->Pointer);
	oldScale=m_textAttribute->scale;
	m_isPressed = true;
}


void TextDemo::TextLayoutItem::SelectGridReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	m_itemAction=NONE;
	//拦截事件，禁止再传递
	e->Handled=true;
	selectGrid->ReleasePointerCapture(e->Pointer);
	m_isPressed = false;
	/*if(m_isChanged)
	{
		m_isChanged = false;
		m_pFunctionControl->saveTextData();
		m_pFunctionControl->getAdjustPage()->setNeedSave(true);
	}*/

}


void TextDemo::TextLayoutItem::SelectGridMoved(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	if(m_itemAction==MOVE && e->Pointer->PointerDeviceType != PointerDeviceType::Touch){
		e->Handled=true;
		double disX,disY;
		disX=e->GetCurrentPoint(selectGrid)->Position.X-eventDwon_x;
		disY=e->GetCurrentPoint(selectGrid)->Position.Y-eventDwon_y;	
		moveSelf(disX,disY);	
		m_pTextCanvasControl->updateTextMask();
		eventDwon_y = e->GetCurrentPoint(selectGrid)->Position.Y;
		eventDwon_x = e->GetCurrentPoint(selectGrid)->Position.X;	
	}
}
