#pragma once

// 协助 Win32 API 处理异常的帮助器实用程序。
namespace DX
{
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			// 在此行中设置断点，以捕获 Win32 API 错误。
			throw Platform::Exception::CreateException(hr);
		}
	}

	 inline D2D1_COLOR_F ConvertToColorF(Windows::UI::Color color)
    {
        return D2D1::ColorF(color.R / 255.0f, color.G / 255.0f, color.B / 255.0f, color.A / 255.0f);            
    }

    inline D2D1_RECT_F ConvertToRectF(Windows::Foundation::Rect source)
    {
        return D2D1::RectF(source.Left, source.Top, source.Right, source.Bottom);
    }
}
