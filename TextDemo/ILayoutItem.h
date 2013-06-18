#pragma once

using namespace Windows::UI::Xaml;

namespace TextDemo
{
	public enum class FontStyle
		{
			STYLE_NORMAL=1,
			STYLE_BOLD=2<<0,
			STYLE_ITALIC=2<<1,
			STYLE_UNDERLINE=2<<2
		};
	public enum class TextItemType
	{
		Type_Text,
		Type_Graphics
	};

	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class TextAttribute sealed{
	public:
		TextAttribute(){}
		TextAttribute^ clone()
		{
			auto tmp = ref new TextAttribute();
			tmp->itemType = itemType;
			tmp->textContent = ref new Platform::String(textContent->Data());
			tmp->textFamily = ref new Platform::String(textFamily->Data());
			tmp->bgimageName = ref new Platform::String(bgimageName->Data());
			tmp->textFamilyIndex = textFamilyIndex;
			tmp->color = color;
			tmp->colorIndex = colorIndex;
			
			tmp->textAlignment = textAlignment;
			tmp->style = style;
			tmp->size = size;
			tmp->angle = angle;
			tmp->scale = scale;
			tmp->alpha = alpha;	
			tmp->charaSpacing = charaSpacing;

			tmp->width = width;
			tmp->height = height;
			tmp->left = left;
			tmp->top = top;			
			tmp->Zindex = Zindex;			
			
			return tmp;
		}

		property TextItemType										itemType;
		property Platform::String^									textContent;
		property Platform::String^									textFamily;
		property int												textFamilyIndex;
		property Platform::String^									bgimageName;	
		property Windows::UI::Color									color;
		property int												colorIndex;
		property Windows::UI::Xaml::TextAlignment					textAlignment;
		
		property FontStyle											style;
		property double												size;
		property double												angle;
		property double												scale;
		property int												alpha;
		property double												charaSpacing;
		
		property double												width;
		property double												height;
		property double												left;
		property double												top;
		
		property int												Zindex;		
	};

	enum ItemAction{
		NONE,MOVE,SCALE
	};
	
	

	[Windows::Foundation::Metadata::WebHostHidden]
	public interface class ILayoutItem
	{
	public:
		void notifyChanged();
		void showSelectBorder(bool isShow);
		TextAttribute ^	getTextAttribute();	
	};
		

	double angleToRadian(double angle);

	double radianToAngle(double radian);

	//移动对象到顶层
	void bringToFront(Windows::UI::Xaml::Controls::Canvas^ parent,Windows::UI::Xaml::UIElement^ child);

	//获取两地的距离
	double getDistance(double x1,double y1,double x2,double y2);

	//获取两点的夹角
	double getAngle(double centerX,double centerY,double x1,double y1,double x2,double y2);



	//获取指定文字的宽高
	void getCharacterSize(Platform::String^ str,
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
		double result[2]);
}
