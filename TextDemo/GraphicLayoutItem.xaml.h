//
// GraphicLayoutItem.xaml.h
// GraphicLayoutItem 类的声明
//

#pragma once

#include "GraphicLayoutItem.g.h"
#include "ILayoutItem.h"

namespace TextDemo
{
	ref class TextControl;
	ref class TextCanvasControl;	
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class GraphicLayoutItem sealed : public ILayoutItem
	{
	public:
		GraphicLayoutItem(TextControl^ textControl,TextCanvasControl^ canvasControl);
		virtual void notifyChanged();
		
		virtual TextAttribute ^				getTextAttribute();		
		virtual void showSelectBorder(bool isShow);
	private:		
		//移动自己在父控件中的位置
		void moveSelf(double disX,double disY);
		//缩放文本
		void scaleSelf(double scaleValue);
		//旋转控件本身
		void rotateSelf(double angle,double centerX,double centerY);

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
		
		void DeletePressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void DeleteTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void SelectGridPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void SelectGridReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void SelectGridMoved(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void UserControl_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
