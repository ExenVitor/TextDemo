//
// TextLayoutItem.xaml.h
// TextLayoutItem 类的声明
//

#pragma once

#include "TextLayoutItem.g.h"

namespace TextDemo
{

	public enum class FontStyle
		{
			STYLE_NORMAL=1,
			STYLE_BOLD=2<<0,
			STYLE_OBLIQUE=2<<1,
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
			tmp->color = color;
			tmp->colorIndex = colorIndex;
			
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
		property Platform::String^									bgimageUri;	
		property Windows::UI::Color									color;
		property int												colorIndex;
		
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
		TextLayoutItem(TextControl^ textControl);

		void setCanvasControl(TextCanvasControl^ pControl) {m_pTextCanvasControl = pControl;}
		TextAttribute ^				getTextAttribute();		

	private:
		void notifyChanged();
		//移动自己在父控件中的位置
		void moveSelf(double disX,double disY);

		void UserControl_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

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
		
		void SelectGridPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void SelectGridReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void SelectGridMoved(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
	};
}
