//
// TextLayoutItem.xaml.h
// TextLayoutItem 类的声明
//

#pragma once

#include "TextLayoutItem.g.h"
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

	ref class TextControl;
	ref class TextCanvasControl;
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class TextAttribute sealed{
	public:
		TextAttribute(){}
		TextAttribute^ clone()
		{
			auto tmp = ref new TextAttribute();
			tmp->textContent = ref new Platform::String(textContent->Data());
			tmp->textFamily = ref new Platform::String(textFamily->Data());
			tmp->bgimageUri = ref new Platform::String(bgimageUri->Data());
			tmp->textFamilyIndex = textFamilyIndex;
			tmp->color = color;
			tmp->colorIndex = colorIndex;
			
			tmp->textAlignment = textAlignment;
			tmp->style = style;
			tmp->size = size;
			tmp->angle = angle;
			tmp->scale = scale;
			tmp->alpha = alpha;
			tmp->orientation = orientation;
			tmp->width = width;
			tmp->height = height;
			tmp->left = left;
			tmp->top = top;
			tmp->lineHight = lineHight;
			tmp->Zindex = Zindex;			
			
			return tmp;
		}
		property Platform::String^									textContent;
		property Platform::String^									textFamily;
		property int												textFamilyIndex;
		property Platform::String^									bgimageUri;	
		property Windows::UI::Color									color;
		property int												colorIndex;
		property TextAlignment										textAlignment;
		
		property FontStyle											style;
		property double												size;
		property double												angle;
		property double												scale;
		property int												alpha;
		property double												orientation;
		property double												width;
		property double												height;
		property double												left;
		property double												top;
		property double												lineHight;
		property int												Zindex;		
	};

	enum ItemAction{
		NONE,MOVE,SCALE
	};

	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class TextLayoutItem sealed
	{
	public:
		TextLayoutItem(TextControl^ textControl,TextCanvasControl^ canvasControl);
		void notifyChanged();
		
		TextAttribute ^				getTextAttribute();		
		void showSelectBorder(bool isShow);

	private:
		
		//移动自己在父控件中的位置
		void moveSelf(double disX,double disY);
		//缩放文本
		void scaleSelf(double scaleValue);
		//旋转控件本身
		void rotateSelf(double angle,double centerX,double centerY);
		//根据宽高计算合适的字号
		float getMatchingsize(float width,float height);

		
		void UserControl_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		bool					m_isInit;
		bool					m_isPressed;
		bool					m_isChanged;
		TextAttribute^			m_textAttribute;
		TextControl^			m_pTextControl;
		TextCanvasControl^		m_pTextCanvasControl;


		//User Action
		ItemAction				m_itemAction;
		double					eventDwon_x;
		double					eventDwon_y;
		double					new_x; //触摸模式下记录缩放后触摸点位置
		double					new_y;
		double					oldScale;
		double					oldDistance;
		double					oldAngle;
		
		void SelectGridPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void SelectGridReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void SelectGridMoved(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void DeleteTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void DeletePressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
	};
}
