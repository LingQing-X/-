#include <windows.h>

/* 这是所有输入到窗口的位置 */
HWND WB, ZD, A1, B2, C3, D4, E5, F6;
HINSTANCE hInst;
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch (Message) {

		case WM_CREATE: { //创建时
			MessageBox(NULL, "\n                                              告示及更新内容\n告示：\n快捷菜单更新啦！现（2022-11-6）最新版本为1.1，作者正在开发1.1.5版本啦！如有开发建议或疑问请加扣群：864229402\n\n更新内容：\n1.1更新内容：\n新增了一个置顶窗口的复选框按钮，禁用了窗口最大化按钮和调整窗口大小。\n\n1.0.5更新内容：\n新增了两个按钮，第一个是“打开命令提示符”第二个是“一键打开百度网页”。\n\n然后就是修改了“强制退出极域”这个按钮的名字，旧名字是“一键退出极域学生端”，因为有点长，所以就改了。还有就是当点击“强制退出极域”时，确定完，还有重新连接上极域课堂管理系统的操作步骤的提示，非常滴nice。\n\n最后就是修复了两个bug，第一个bug是点击“打开任务管理器”按钮的bug，就是点击打开后，会有一个黑窗口伴随着出来，现在就不会有这种情况发生了。第二个bug是退出了此应用，此应用还会一直保持使用状态，一直保持使用状态就会导致无法删除此应用的bug，现已修复。", "信息", MB_OK);

			WB = CreateWindow("static", "此应用由XGACY开发，如有开发建议或疑问请加扣群：864229402", WS_CHILD | WS_VISIBLE,
			                  20,   10, 340, 40, hwnd, (HMENU)23333, hInst, NULL);
			ZD = CreateWindow("button", "置顶窗口", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			                  150,  58, 80,  15, hwnd, (HMENU)54188, hInst, NULL);
			A1 = CreateWindow("button", "一键关闭萝卜圈", WS_CHILD | WS_VISIBLE,
			                  20,   80, 160, 60, hwnd, (HMENU)1, hInst, NULL);
			B2 = CreateWindow("button", "强制退出极域", WS_CHILD | WS_VISIBLE,
			                  200,  80, 160, 60, hwnd, (HMENU)2, hInst, NULL);
			C3 = CreateWindow("button", "打开命令提示符", WS_CHILD | WS_VISIBLE,
			                  20,  160, 160, 60, hwnd, (HMENU)3, hInst, NULL);
			D4 = CreateWindow("button", "一键打开百度网页", WS_CHILD | WS_VISIBLE,
			                  200, 160, 160, 60, hwnd, (HMENU)4, hInst, NULL);
			E5 = CreateWindow("button", "打开任务管理器", WS_CHILD | WS_VISIBLE,
			                  20,  240, 160, 60, hwnd, (HMENU)5, hInst, NULL);
			F6 = CreateWindow("button", "关机", WS_CHILD | WS_VISIBLE,
			                  200, 240, 160, 60, hwnd, (HMENU)6, hInst, NULL);
			break;
		}

		/* 销毁后，告诉主线程停止 */
		case WM_DESTROY: { //销毁时
			PostQuitMessage(0); //如果没有这个，退出窗口时，窗口将一直保持为使用状态
			break;
		}

		case WM_COMMAND: { //命令
			switch (wParam) {

				case 54188: { //置顶窗口
					HWND hWnd = GetForegroundWindow(); //句柄：获取前景窗口
					if (GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST) { //检查当前窗口是否为置顶状态
						SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); //权限置顶
					} else { //置顶 SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); （在"HWND_|>（这里）<|TOPMOST"加上"NO"就是取消置顶）
						SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); //置顶
					}
					break;
				}

				case 1: { //一键关闭萝卜圈
					int x = MessageBox(NULL, "确认要执行此操作吗？\n确认后您的萝卜圈将直接关闭！\n请确认您的机器人和程序是否保存！", "询问", MB_OKCANCEL | MB_ICONQUESTION);
					if (x == IDOK) {
						system("taskkill /f /t /im IRobotQApp.exe"); //关闭萝卜圈的进程
					}
					break;
				}

				case 2: { //强制退出极域
					int x = MessageBox(NULL, "确认要执行此操作吗？\n确认后您将退出极域课堂管理系统的管\n控，退出后您将不再受教师端的控制！\n--------------------------------------\n请注意：使用此应用造成的所有后果\n，请您自己承担！与应用作者无关！", "询问", MB_OKCANCEL | MB_ICONQUESTION);
					if (x == IDOK) {
						system("taskkill /f /t /im StudentMain.exe"); //关闭极域电子教室的进程
						MessageBox(NULL, "如果您想要重新连接上极域课堂管理\n系统，请您双击电脑桌面上的极域课\n堂管理系统的快捷方式等待连接即可\n--------------------------------------\n请注意：使用此应用造成的所有后果\n，请您自己承担！与应用作者无关！", "提示", MB_OK | MB_ICONASTERISK);
					}
					break;
				}

				case 3: { //打开命令提示符
					system("start cmd.exe"); //打开命令提示符
					break;
				}

				case 4: { //一键打开百度网页
					system("explorer https://www.baidu.com/");
					break;
				}

				case 5: { //打开任务管理器
					system("start taskmgr.exe"); //打开任务管理器，且不会有黑窗口（直接用system("taskmgr")打开就会有黑窗口，而用system("start taskmgr.exe")打开就不会有黑窗口）
					break;
				}

				case 6: { //关机
					int x = MessageBox(NULL, "确认要执行此操作吗？\n确认后您的电脑将在10秒后自动关闭！", "询问", MB_OKCANCEL | MB_ICONQUESTION);
					if (x == IDOK) {
						system("shutdown -s -t 10"); //关机（-s和-t是参数 10是关机时间 单位：秒）
					}
					break;
				}

				/* 所有其他消息（其中很多）都使用默认过程处理 */
				default: {
					DefWindowProc(hwnd, Message, wParam, lParam);
					break;
				}

			}
			break;
		}
		/* 所有其他消息（其中很多）都使用默认过程处理 */
		default: {
			DefWindowProc(hwnd, Message, wParam, lParam);
			break;
		}
	}
}

/* Win32 GUI程序的“main”函数：这是执行开始的地方 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc; /* 窗口的属性结构 */
	HWND hwnd; /* 一个“句柄”，因此是H，或指向窗口的指针 */
	MSG msg; /* 所有邮件的临时位置 */

	/* 清除结构并设置要修改的内容 */
	memset(&wc, 0, sizeof(wc));
	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.lpfnWndProc	 = WndProc; /* 这是我们发送消息的地方 */
	wc.hInstance	 = hInstance;
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);

	/* 白色，COLOR_WINDOW只是系统颜色的#定义，请尝试Ctrl+单击它 */
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = "WindowClass";
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION); /* 加载标准图标 */
	wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION); /* 使用名称“A”来使用项目图标 */

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "窗口注册失败！", "错误", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, "WindowClass", "快捷菜单 1.1_此应用由XGACY开发", WS_VISIBLE | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
	                      CW_USEDEFAULT, /* x */
	                      CW_USEDEFAULT, /* y */
	                      400, /* 窗口长 */
	                      360, /* 窗口宽 */
	                      NULL, NULL, hInstance, NULL);

	if (hwnd == NULL) {
		MessageBox(NULL, "窗口创建失败！", "错误", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	/*
		这是我们程序的核心，在这里处理所有输入
		发送至WndProc。请注意，GetMessage会阻止代码流，直到它接收到一些东西，所以
		此循环不会产生不合理的高CPU使用率
	*/
	while (GetMessage(&msg, NULL, 0, 0) > 0) { /* 如果没有收到错误... */
		TranslateMessage(&msg); /* 将密钥代码转换为字符（如果存在） */
		DispatchMessage(&msg); /* 发送给WndProc */
	}

	return msg.wParam;
}
