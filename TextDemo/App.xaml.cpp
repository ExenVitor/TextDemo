//
// App.xaml.cpp
// App 类的实现。
//

#include "pch.h"
#include "MainPage.xaml.h"

using namespace TextDemo;

using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// “空白应用程序”模板在 http://go.microsoft.com/fwlink/?LinkId=234227 上提供

/// <summary>
/// 初始化单一实例应用程序对象。这是执行的创作代码的第一行，
/// 逻辑上等同于 main() 或 WinMain()。
/// </summary>
App::App()
{
	InitializeComponent();
	Suspending += ref new SuspendingEventHandler(this, &App::OnSuspending);
}

/// <summary>
/// 在应用程序由最终用户正常启动时进行调用。
/// 当启动应用程序以执行打开特定的文件或显示搜索结果等操作时
/// 将使用其他入口点。
/// </summary>
/// <param name="args">有关启动请求和过程的详细信息。</param>
void App::OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ args)
{
	auto rootFrame = dynamic_cast<Frame^>(Window::Current->Content);

	// 不要在窗口已包含内容时重复应用程序初始化，
	// 只需确保窗口处于活动状态
	if (rootFrame == nullptr)
	{
		// 创建一个 Frame 以用作导航上下文并将其与
		// SuspensionManager 键关联
		rootFrame = ref new Frame();

		if (args->PreviousExecutionState == ApplicationExecutionState::Terminated)
		{
			// TODO: 仅当适用时还原保存的会话状态，并安排
			// 还原完成后的最终启动步骤

		}

		if (rootFrame->Content == nullptr)
		{
			// 当未还原导航堆栈时，导航到第一页，
			// 并通过将所需信息作为导航参数传入来配置
			// 参数
			if (!rootFrame->Navigate(TypeName(MainPage::typeid), args->Arguments))
			{
				throw ref new FailureException("Failed to create initial page");
			}
		}
		// 将框架放在当前窗口中
		Window::Current->Content = rootFrame;
		// 确保当前窗口处于活动状态
		Window::Current->Activate();
	}
	else
	{
		if (rootFrame->Content == nullptr)
		{
			// 当未还原导航堆栈时，导航到第一页，
			// 并通过将所需信息作为导航参数传入来配置
			// 参数
			if (!rootFrame->Navigate(TypeName(MainPage::typeid), args->Arguments))
			{
				throw ref new FailureException("Failed to create initial page");
			}
		}
		// 确保当前窗口处于活动状态
		Window::Current->Activate();
	}
}

/// <summary>
/// 在将要挂起应用程序执行时调用。在不知道应用程序
/// 将被终止还是恢复的情况下保存应用程序状态，
/// 并让内存内容保持不变。
/// </summary>
/// <param name="sender">挂起的请求的源。</param>
/// <param name="e">有关挂起的请求的详细信息。</param>
void App::OnSuspending(Object^ sender, SuspendingEventArgs^ e)
{
	(void) sender;	// 未使用的参数
	(void) e;	// 未使用的参数

	//TODO: 保存应用程序状态并停止任何后台活动
}
