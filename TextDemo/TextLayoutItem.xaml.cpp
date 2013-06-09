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

const int selectMargin = 40;

double angleToRadian(double angle)
{
	return angle * 3.1415926 / 180;
}

double radianToAngle(double radian)
{
	return radian * 180 / 3.1415926;
}

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

//获取两地的距离
double getDistance(double x1,double y1,double x2,double y2){
	double temp_w = x1-x2;
	double temp_h = y1-y2;
	return sqrt(temp_w*temp_w + temp_h*temp_h); // 计算
}

//获取两点的夹角
double getAngle(double centerX,double centerY,double x1,double y1,double x2,double y2){
	x1 -= centerX;
	y1 -= centerY;
	double length1 = x1 * x1 + y1 * y1;
	x2 -= centerX;
	y2 -= centerY;
	double length2 = x2 * x2 + y2 * y2;
	double alpha = (double) ((x1 * x2 + y1 * y2) / (sqrt(length1) * sqrt(length2)));
	double ll = x1 * y2 - y1 * x2;
	if (alpha > 0.999999 && alpha < 1.000001) {
		return 0.0;
	}
	double result = (double)acos(alpha);
	if (ll < 0) {
		result = -result;
	}
	
	return radianToAngle(result);
}



//获取指定文字的宽高
void getCharacterSize(Platform::String^ str,
					  Platform::String^ fontName,
					  float fontSize,
					  DWRITE_FONT_WEIGHT weight,
					  DWRITE_FONT_STYLE style,
					  bool hasUnderLine,
					  DWRITE_TEXT_ALIGNMENT align,
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
		m_textFormat->SetTextAlignment(align);
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

	DWRITE_LINE_SPACING_METHOD lineMethod;
	FLOAT lineSpacing,lineBase;
	if(SUCCEEDED(hr))
	{
		m_textLayout->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_UNIFORM,
			(fontSize) / 0.8,
			fontSize);
			
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

TextLayoutItem::TextLayoutItem(TextControl^ textControl,TextCanvasControl^ canvasControl)
{
	InitializeComponent();
	m_pTextControl = textControl;
	m_pTextCanvasControl = canvasControl;
	m_textAttribute = m_pTextControl->createTextAttribute();
	m_itemAction = ItemAction::NONE;
	eventDwon_x = 0.0;
	eventDwon_y = 0.0;
	new_x = 0.0;
	new_y = 0.0;
	oldScale = 1.0;
	oldDistance = 1.0;
	oldAngle = 1.0;
	m_isChanged = false;
	m_isPressed = false;
	m_isInit = true;

}

TextAttribute^ TextLayoutItem::getTextAttribute()
{
	return m_textAttribute;
}

void TextLayoutItem::showSelectBorder(bool isShow)
{
	if(isShow)
	{
		selectBorder->Opacity = 1.0;
		img_delete->Opacity = 1.0;
		img_rotate->Opacity = 1.0;
	}
	else
	{
		selectBorder->Opacity = 0.0;
		img_delete->Opacity = 0.0;
		img_rotate->Opacity = 0.0;
	}
}

float TextLayoutItem::getMatchingsize(float width,float height)
{
	double textSize[2]={0,0};
	DWRITE_TEXT_ALIGNMENT align;
	if(m_textAttribute->textAlignment == TextAlignment::Center)
		align = DWRITE_TEXT_ALIGNMENT_CENTER;
	else if(m_textAttribute->textAlignment == TextAlignment::Left)
		align = DWRITE_TEXT_ALIGNMENT_LEADING;
	else if(m_textAttribute->textAlignment == TextAlignment::Right)
		align = DWRITE_TEXT_ALIGNMENT_TRAILING;
	for (int i=72;i>0;i--){
		getCharacterSize(m_textAttribute->textContent,
					 m_textAttribute->textFamily,
					 i,
					 (int)(m_textAttribute->style&TextDemo::FontStyle::STYLE_BOLD)?DWRITE_FONT_WEIGHT_BOLD:DWRITE_FONT_WEIGHT_NORMAL,
					 (int)(m_textAttribute->style&TextDemo::FontStyle::STYLE_ITALIC)?DWRITE_FONT_STYLE_ITALIC:DWRITE_FONT_STYLE_NORMAL,
					 (int)(m_textAttribute->style&TextDemo::FontStyle::STYLE_UNDERLINE)?true:false,
					 align,
					 DWRITE_FONT_STRETCH_NORMAL,					
					 width,height,
					 textSize);
		if(textSize[0]<width && textSize[1]<height){
			return i;
		}
	}
	return 1;
}


void TextLayoutItem::notifyChanged()
{
	double textSize[2]={0,0};
	DWRITE_TEXT_ALIGNMENT align;
	if(m_textAttribute->textAlignment == TextAlignment::Center)
		align = DWRITE_TEXT_ALIGNMENT_CENTER;
	else if(m_textAttribute->textAlignment == TextAlignment::Left)
		align = DWRITE_TEXT_ALIGNMENT_LEADING;
	else if(m_textAttribute->textAlignment == TextAlignment::Right)
		align = DWRITE_TEXT_ALIGNMENT_TRAILING;
	getCharacterSize(m_textAttribute->textContent,
		m_textAttribute->textFamily,
		m_textAttribute->size,
		(int)(m_textAttribute->style&TextDemo::FontStyle::STYLE_BOLD)?DWRITE_FONT_WEIGHT_BOLD:DWRITE_FONT_WEIGHT_NORMAL,
		(int)(m_textAttribute->style&TextDemo::FontStyle::STYLE_ITALIC)?DWRITE_FONT_STYLE_ITALIC:DWRITE_FONT_STYLE_NORMAL,
		(int)(m_textAttribute->style&TextDemo::FontStyle::STYLE_UNDERLINE)?true:false,
		align,
		DWRITE_FONT_STRETCH_NORMAL,
		5000,5000,
		textSize);
	auto parent = safe_cast<Canvas^>(this->Parent);	
	double preLeft = parent->GetLeft(this);
	double preTop = parent->GetTop(this);
	
	selectGrid->Width = textSize[0] + selectMargin;
	selectGrid->Height = textSize[1] + selectMargin;

	double preSelectWidth = m_textAttribute->width + selectMargin;
	double preSelectHeight = m_textAttribute->height + selectMargin;
	m_textAttribute->width=textSize[0];
	m_textAttribute->height=textSize[1];


	
	if(m_isInit)
	{
		moveSelf((parent->ActualWidth - selectGrid->Width)/2.0,(parent->ActualHeight - selectGrid->Height)/2.0);

		m_isInit = false;
	}
	else
	{
		double newSelectWidth = m_textAttribute->width + selectMargin;
		double newSelectHeight = m_textAttribute->height + selectMargin;

		double maxTxtScaleX = parent->ActualWidth / (newSelectWidth + img_delete->Width);
		double maxTxtScaleY = parent->ActualHeight / (newSelectHeight + img_delete->Height);
		double maxTxtScale = maxTxtScaleX < maxTxtScaleY ? maxTxtScaleX : maxTxtScaleY;	

		double preScale = m_textAttribute->scale;
		if(m_textAttribute->scale > maxTxtScale)
			m_textAttribute->scale = maxTxtScale;		
		selectGrid->Width = newSelectWidth * m_textAttribute->scale;
		selectGrid->Height = newSelectHeight * m_textAttribute->scale;		
	
		
		//重新调整旋转中心点和位移，保证选择框最终左上点保持不变
		RotateTransform^ preRotateTrans = safe_cast<RotateTransform^>(selectGrid->RenderTransform);
		RotateTransform^ curRotateTrans = ref new RotateTransform();
		curRotateTrans->CenterX = selectGrid->Width / 2.0;
		curRotateTrans->CenterY = selectGrid->Height / 2.0;
		curRotateTrans->Angle = m_textAttribute->angle;

		auto preLeftTop = preRotateTrans->TransformPoint(Point(preLeft,preTop));
		auto curLeftTop = curRotateTrans->TransformPoint(Point(preLeft,preTop));

		parent->SetLeft(this,preLeft + (preLeftTop.X - curLeftTop.X));
		parent->SetTop(this,preTop + (preLeftTop.Y - curLeftTop.Y));

		UpdateLayout();
		
		rotateSelf(m_textAttribute->angle,curRotateTrans->CenterX,curRotateTrans->CenterY);	

		double newCenterX = parent->GetLeft(this) + curRotateTrans->CenterX;
		double newCenterY = parent->GetTop(this) + curRotateTrans->CenterY;

		m_textAttribute->left = newCenterX - m_textAttribute->width  / 2.0;
		m_textAttribute->top = newCenterY - m_textAttribute->height / 2.0;

	}
	
	m_pTextCanvasControl->updateTextMask();
	m_pTextControl->EventLock = true;
	m_pTextControl->setCurrentItem(this);
	m_pTextControl->EventLock = false;
}

void TextLayoutItem::scaleSelf(double scaleValue)
{
	double orgWidth = selectGrid->Width;
	double orgHeight = selectGrid->Height;
	selectGrid->Width = (m_textAttribute->width + selectMargin) * scaleValue;
	selectGrid->Height = (m_textAttribute->height + selectMargin) * scaleValue;
	double disX = (orgWidth - selectGrid->Width) / 2.0;
	double disY = (orgHeight - selectGrid->Height) / 2.0;

	auto parent = safe_cast<Canvas^>(this->Parent);
		
	parent->SetLeft(this,parent->GetLeft(this)+disX);
	parent->SetTop(this,parent->GetTop(this)+disY);
	
}

void TextLayoutItem::rotateSelf(double angle,double centerX,double centerY)
{
	
	RotateTransform^ rotateTrans = safe_cast<RotateTransform^>(selectGrid->RenderTransform);
	rotateTrans->Angle = m_textAttribute->angle;
	rotateTrans->CenterX=centerX;
	rotateTrans->CenterY=centerY;
	selectGrid->RenderTransform=rotateTrans;
}

void TextLayoutItem::moveSelf(double disX,double disY)
{
	auto parent = safe_cast<Canvas^>(this->Parent);
	double halfW = selectGrid->Width / 2.0;
	double halfH = selectGrid->Height / 2.0;
	double newX = parent->GetLeft(this) + disX;
	double newY = parent->GetTop(this) + disY;	

	if(disX != 0 || disY != 0)
		m_isChanged = true;
	parent->SetLeft(this,parent->GetLeft(this)+disX);
	parent->SetTop(this,parent->GetTop(this)+disY);

	if(m_isInit)
	{
		m_textAttribute->left = newX + selectMargin / 2;
		m_textAttribute->top = newY + selectMargin / 2;
	}
	else
	{
		m_textAttribute->left += disX;
		m_textAttribute->top += disY;
	}


}

void TextDemo::TextLayoutItem::UserControl_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	UpdateLayout();
	notifyChanged();
}



void TextDemo::TextLayoutItem::SelectGridPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	Canvas^ canvas=safe_cast<Canvas^>(this->Parent);
	bringToFront(canvas,this);
	if(sender->GetType()->ToString()->Equals(selectGrid->GetType()->ToString())){
		m_itemAction=MOVE;
		eventDwon_y = e->GetCurrentPoint(canvas)->Position.Y;
		eventDwon_x = e->GetCurrentPoint(canvas)->Position.X;
	}else if(sender->GetType()->ToString()->Equals(img_rotate->GetType()->ToString())){
		m_itemAction=SCALE;
		eventDwon_y = e->GetCurrentPoint(canvas)->Position.Y;
		eventDwon_x=e->GetCurrentPoint(canvas)->Position.X;
		new_x = eventDwon_x;
		new_y = eventDwon_y;			
	}
	e->Handled=true;
	selectGrid->CapturePointer(e->Pointer);
	oldScale=m_textAttribute->scale;
	oldAngle = 0;
	m_isPressed = true;
	m_pTextControl->EventLock = true;
	m_pTextControl->setCurrentItem(this);
	m_pTextControl->EventLock = false;
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
	Canvas^ canvas=safe_cast<Canvas^>(this->Parent);
	if(m_itemAction==MOVE && e->Pointer->PointerDeviceType != PointerDeviceType::Touch){
		e->Handled=true;
		double disX,disY;
		disX=e->GetCurrentPoint(canvas)->Position.X-eventDwon_x;
		disY=e->GetCurrentPoint(canvas)->Position.Y-eventDwon_y;	
		moveSelf(disX,disY);	
		m_pTextCanvasControl->updateTextMask();
		eventDwon_y = e->GetCurrentPoint(canvas)->Position.Y;
		eventDwon_x = e->GetCurrentPoint(canvas)->Position.X;	
	}else if(m_itemAction==SCALE){
		double x =e->GetCurrentPoint(canvas)->Position.X;
		double y =e->GetCurrentPoint(canvas)->Position.Y;
		double centerX=0;
		double centerY=0;
		centerX=canvas->GetLeft(this) + selectGrid->Width / 2.0;
		centerY=canvas->GetTop(this) + selectGrid->Height / 2.0;
		//起始触摸点和图片中心点的距离
		oldDistance = getDistance(centerX,centerY,eventDwon_x,eventDwon_y);      
		//当前触摸点和图片中心点的距离
		double newDistance = getDistance(centerX,centerY,x,y);
		//得到缩放比例
		double scaleValue = newDistance/oldDistance;

		double newScale=oldScale*scaleValue;

		double orgSelectWidth = m_textAttribute->width + selectMargin;
		double orgSelectHeight = m_textAttribute->height + selectMargin;
		
		double maxTxtScaleX = canvas->ActualWidth / (orgSelectWidth + img_delete->Width);
		double maxTxtScaleY = canvas->ActualHeight / (orgSelectHeight + img_delete->Height);
		double maxTxtScale = maxTxtScaleX < maxTxtScaleY ? maxTxtScaleX : maxTxtScaleY;
		if(newScale > maxTxtScale)
			newScale = maxTxtScale;

		double newFontSize=getMatchingsize(orgSelectWidth*newScale,orgSelectHeight*newScale);

		if(newFontSize>8.0 ){
			if(m_textAttribute->scale != newScale)
				m_isChanged = true;
			//记录到属性
			m_textAttribute->scale=newScale;
			//缩放文本框		
			scaleSelf(m_textAttribute->scale);
		}
		
		//计算旋转角度,并记录到属性
		double angle =  getAngle(centerX,centerY,eventDwon_x,eventDwon_y,x,y) ;		
		double angleOffset = angle - oldAngle;		
		oldAngle = angle;
		if(angleOffset != 0)
			m_isChanged = true;
		m_textAttribute->angle += angleOffset ;
		if(m_textAttribute->angle >= 360) m_textAttribute->angle -= 360;		
		if(m_textAttribute->angle<0) m_textAttribute->angle+=360;
		double gridCenterX = selectGrid->Width / 2.0;
		double gridCenterY = selectGrid->Height / 2.0;
		rotateSelf(m_textAttribute->angle,gridCenterX,gridCenterY);

		////刷新界面控件数据
		m_pTextControl->EventLock = true;
		m_pTextControl->setCurrentItem(this);
		m_pTextControl->EventLock = false;
		m_pTextCanvasControl->updateTextMask();
	}
}


void TextDemo::TextLayoutItem::DeleteTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	e->Handled = true;
}


void TextDemo::TextLayoutItem::DeletePressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	m_pTextControl->deleteTextItem(this);
	//m_pFunctionControl->saveTextData();
}
