#include "pch.h"
#include "ILayoutItem.h"
using namespace TextDemo;
using namespace Windows::UI::Xaml;

extern const int selectMargin = 40;

double TextDemo::angleToRadian(double angle)
{
	return angle * 3.1415926 / 180;
}

double TextDemo::radianToAngle(double radian)
{
	return radian * 180 / 3.1415926;
}

//移动对象到顶层
void TextDemo::bringToFront(Windows::UI::Xaml::Controls::Canvas^ parent,Windows::UI::Xaml::UIElement^ child){
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
double TextDemo::getDistance(double x1,double y1,double x2,double y2){
	double temp_w = x1-x2;
	double temp_h = y1-y2;
	return sqrt(temp_w*temp_w + temp_h*temp_h); // 计算
}

//获取两点的夹角
double TextDemo::getAngle(double centerX,double centerY,double x1,double y1,double x2,double y2){
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

	return TextDemo::radianToAngle(result);
}



//获取指定文字的宽高
void TextDemo::getCharacterSize(Platform::String^ str,
					  Platform::String^ fontName,
					  float fontSize,
					  DWRITE_FONT_WEIGHT weight,
					  DWRITE_FONT_STYLE style,
					  bool hasUnderLine,
					  DWRITE_TEXT_ALIGNMENT align,
					  DWRITE_FONT_STRETCH stretch,
					  float charaSpacing,
					  float width,
					  float height,
					  double result[2]){
						  IDWriteFactory* pDWriteFactory = NULL;
						  Microsoft::WRL::ComPtr<IDWriteTextLayout> m_textLayout;
						  Microsoft::WRL::ComPtr<IDWriteTextFormat> m_textFormat;
						  Microsoft::WRL::ComPtr<IDWriteTextLayout1> m_textLayout1;
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
							  hr=m_textFormat->SetTextAlignment(align);
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

						  m_textLayout.As(&m_textLayout1);
						  DWRITE_TEXT_RANGE textRange;
						  textRange.length=str->Length();
						  textRange.startPosition=0;


						  if (SUCCEEDED(hr))
						  {
							  hr=m_textLayout1->SetUnderline(hasUnderLine,
								  textRange);
						  }

						  DWRITE_LINE_SPACING_METHOD lineMethod;
						  FLOAT lineSpacing,lineBase;
						  if(SUCCEEDED(hr))
						  {
							  hr=m_textLayout1->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_UNIFORM,
								  (fontSize) / 0.8,
								  fontSize);

						  }

						  if(SUCCEEDED(hr))
						  {
							  hr=m_textLayout1->SetCharacterSpacing(charaSpacing,charaSpacing,0,textRange);
						  }

						  if (SUCCEEDED(hr))
						  {
							  hr=m_textLayout1->GetMetrics(&m_textMetrics);
						  }
						  if (SUCCEEDED(hr))
						  {
							  result[0]=m_textMetrics.width;
							  result[1]=m_textMetrics.height;	
						  }	
}