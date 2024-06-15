#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <direct.h>
#include <commctrl.h>
#include <vector>
#include <filesystem>
#include <tlhelp32.h>
using namespace std;

// 声明全局变量
const char childwindow[] = "Drive mapping"; // 网络映射辅助工具 子窗口的全局变量
int isExitWindowOpen = 0; // 用于使“退出”子窗口只能打开一次
int isSettingsWindowOpen = 0; // 用于使“设置”子窗口只能打开一次
int isWebsiteWindowOpen = 0; // 用于使“网站”子窗口只能打开一次
int isMappingWindowOpen = 0; // 用于使“映射”子窗口只能打开一次
int isiToolsWindowOpen = 0; // 用于使“工具箱”子窗口只能打开一次
int CtrlxhuoCtrlC = 0; // 用于使剪贴板检测一次到后停止

// 窗口句柄
HWND host_hwnd; // 声明主窗口的句柄变量
HWND hwnd_exit; // 声明“退出”窗口的句柄变量
HWND hwnd_Settings; // 声明“设置”窗口的句柄变量
HWND hwnd_website; // 声明“网站”窗口的句柄变量
HWND hwnd_mapping; // 声明“映射”窗口的句柄变量
HWND hwnd_iTools; // 声明“工具箱”窗口的句柄变量

/* 网络映射辅助工具 子窗口 */
// 控件句柄
HWND hGroupBoxMapping; // 映射/断开 分组框
HWND usernameLabel; // 账号
HWND usernameEdit; // 账号 输入框
HWND passwordLabel; // 密码
HWND passwordEdit; // 密码 输入框
HWND ipLabel; // IP地址
HWND ipComboBox; //IP地址 输入框
HWND sharedFolderLabel; //共享目录
HWND sharedFolderEdit; // 共享目录 输入框
HWND driveLabel; // 盘符
HWND driveComboBox; // 盘符 选择下拉列表
HWND connectButton; // 连接 按钮
HWND disconnectButton; // 断开 按钮
HWND hGroupBoxSettings; // 设置/其他 分组框
HWND showPasswordCheckbox; // 显示密码 复选框
HWND countdownCheckbox; // 映射成功后开始计时 复选框
HWND hStaticSetCountdownTime; // 设置倒计时时间：
HWND countdownLabel; // 时间设置
HWND countdownEdit; // 时间设置 输入框
HWND countdownUnitComboBox; // 时间单位设置
HWND detailedButton; // 详细 按钮
HWND jieshudaojishicheckbox; // 断开后不结束倒计时 复选框
HWND hStaticMappingKernel; // 映射内核 文本
HWND hBtnWNetAddConnection2A; // 使用WNetAddConnection2A函数 单选框
HWND hBtnNetCommandLine; // 使用NET命令行命令 单选框
HWND GenerateBatchProcessing; // 生成专属于我的映射批处理文件
HWND LockPassword; // 锁定密码 复选框
HWND Saveusername; // 映射成功后保存用户名 复选框
HWND ForcedisconnectallButton; // 强制断开全部(重启资源管理器) 按钮
HWND keyongpancheckbox; // 显示已占用的盘符 复选框
HWND zidongxuanze; // 映射成功后不自动选择下一个共享目录和盘符 复选框
HWND daojishi; // 倒计时 文本
HWND yingsheshezhi; // 映射设置 文本
HWND bujinyongxsmm; // 映射成功后不禁止显示密码 复选框
HWND nextpage; // 下/上一页
HWND SaveuserIP; // 保存IP地址
HWND Saveusershare; // 保存共享目录
HWND yingsheshezhi2; // 详细说明2 文本
// 控件值
string username; // 账号
string password; // 密码
string ipAddress; // IP地址
string sharedFolder; // 共享目录
string drive; // 盘符
// 倒计时持续时间的全局变量
int countdownMinutes = 0; // 分
int countdownDuration = 0; // 秒
// 布尔变量（定义的标志）
bool hasContentExecuted = false;
bool noContentExecuted = false;
bool hasContentExecuted2 = false;
bool noContentExecuted2 = false;
bool hasContentExecuted3 = false;
bool noContentExecuted3 = false;
BOOL bUpdateDriveList = TRUE;

/* 这是所有窗口输入的地方 */
HWND A_Top, A_AOU, A_text, A_text2, A_text3, A_1, A_2, A_3, A_999, hTab, Settingtext, Nevernotifyresetting, experimentalfeaturestext, strongTop, Startup, button_32913, button_32914, A_Jtb, A_4;
HINSTANCE hInst;
NOTIFYICONDATA nid; // 托盘

// 声明一个全局变量用于表示加强置顶的状态
int strongTopChecked = 0;

// 声明右键菜单的ID
#define IDM_EXIT 1001

HANDLE hThread = NULL; // 自动打开新增的存储设备 的全局变量

std::unordered_set<std::string> openedDrives; // 已打开的驱动器集合

// 驱动器状态的枚举
enum DriveStatus {
	DRIVE_STATUS_READY,
	DRIVE_STATUS_NOT_READY,
	DRIVE_STATUS_ERROR
};

// 获取驱动器的状态
DriveStatus GetDriveStatus(const std::string& drivePath) {
	DWORD driveAttributes = GetFileAttributesA(drivePath.c_str());
	if (driveAttributes == INVALID_FILE_ATTRIBUTES) {
		return DRIVE_STATUS_ERROR;
	} else if (driveAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		return DRIVE_STATUS_READY;
	} else {
		return DRIVE_STATUS_NOT_READY;
	}
}

// 检测新增的驱动器并打开
// 驱动器检测和处理的函数
DWORD WINAPI DriveDetection(LPVOID lpParam) {
	// 定义已打开的驱动器集合
	std::unordered_set<std::string> openedDrives;
	// 获取当前已插入的驱动器
	DWORD drives = GetLogicalDrives();
	// 持续监测驱动器是否插入
	while (true) {
		// 获取最新的驱动器信息
		DWORD newDrives = GetLogicalDrives();
		// 检查是否有新的驱动器插入
		DWORD insertedDrives = newDrives & (~drives);
		if (insertedDrives != 0) {
			// 遍历新插入的驱动器
			for (int i = 0; i < 26; ++i) {
				if ((insertedDrives >> i) & 1) {
					char driveLetter = 'A' + i;
					char drivePath[4] = { driveLetter, ':', '\\', '\0' };
					// 构建驱动器路径
					std::string drivePathStr(drivePath);
					// 获取驱动器类型
					UINT driveType = GetDriveType(drivePathStr.c_str());
					// 根据驱动器类型执行相应的操作
					switch (driveType) {
						case DRIVE_NO_ROOT_DIR: {
							std::cout << "驱动器 " << driveLetter << " 不存在或无效" << std::endl;
							MessageBox(NULL, "检测到新增的驱动器类型为不存在或无效", "错误*_*", MB_ICONEXCLAMATION | MB_OK);
							break;
						}
						case DRIVE_UNKNOWN: {
							std::cout << "驱动器 " << driveLetter << " 类型未知" << std::endl;
							MessageBox(NULL, "检测到新增的驱动器类型为类型未知", "错误*_*", MB_ICONEXCLAMATION | MB_OK);
							break;
						}
						case DRIVE_REMOVABLE: {
							std::cout << "驱动器 " << driveLetter << " 是可移动的" << std::endl;
							// 获取驱动器状态
							DriveStatus driveStatus = GetDriveStatus(drivePathStr);
							if (driveStatus == DRIVE_STATUS_READY) {
								std::cout << "驱动器 " << driveLetter << " 驱动器就绪" << std::endl;
								// 驱动器状态为就绪，可以尝试打开驱动器
								HINSTANCE result = ShellExecuteA(NULL, "open", drivePath, NULL, NULL, SW_SHOW);
								// 记录已打开的驱动器
								openedDrives.insert(drivePathStr);
							} else if (driveStatus == DRIVE_STATUS_NOT_READY) {
								std::cout << "驱动器 " << driveLetter << " 驱动器未就绪" << std::endl;
								// 驱动器状态为未就绪，可以执行其他操作
								// 不断检测驱动器状态，直到状态为就绪
								DriveStatus driveStatus = GetDriveStatus(drivePathStr);
								while (driveStatus != DRIVE_STATUS_READY) {
									driveStatus = GetDriveStatus(drivePathStr);
								}
								// 驱动器状态为就绪，可以尝试打开驱动器
								HINSTANCE result = ShellExecuteA(NULL, "open", drivePath, NULL, NULL, SW_SHOW);
								// 记录已打开的驱动器
								openedDrives.insert(drivePathStr);
							} else {
								std::cout << "驱动器 " << driveLetter << " 驱动器异常" << std::endl;
								// 驱动器状态为异常，可以执行其他操作
								// 不断检测驱动器状态，直到状态为就绪
								DriveStatus driveStatus = GetDriveStatus(drivePathStr);
								while (driveStatus != DRIVE_STATUS_READY) {
									driveStatus = GetDriveStatus(drivePathStr);
								}
								// 驱动器状态为就绪，可以尝试打开驱动器
								HINSTANCE result = ShellExecuteA(NULL, "open", drivePath, NULL, NULL, SW_SHOW);
								// 记录已打开的驱动器
								openedDrives.insert(drivePathStr);
							}
							break;
						}
						case DRIVE_FIXED: {
							std::cout << "驱动器 " << driveLetter << " 是固定的" << std::endl;
							// 尝试打开驱动器
							HINSTANCE result = ShellExecuteA(NULL, "open", drivePath, NULL, NULL, SW_SHOW);
							// 记录已打开的驱动器
							openedDrives.insert(drivePathStr);
							break;
						}
						case DRIVE_REMOTE: {
							std::cout << "驱动器 " << driveLetter << " 是网络驱动器" << std::endl;
							// 获取驱动器状态
							DriveStatus driveStatus = GetDriveStatus(drivePathStr);
							if (driveStatus == DRIVE_STATUS_READY) {
								std::cout << "驱动器 " << driveLetter << " 驱动器就绪" << std::endl;
								// 驱动器状态为就绪，可以尝试打开驱动器
								HINSTANCE result = ShellExecuteA(NULL, "open", drivePath, NULL, NULL, SW_SHOW);
								// 记录已打开的驱动器
								openedDrives.insert(drivePathStr);
							} else if (driveStatus == DRIVE_STATUS_NOT_READY) {
								std::cout << "驱动器 " << driveLetter << " 驱动器未就绪" << std::endl;
								// 驱动器状态为未就绪，可以执行其他操作
								// 不断检测驱动器状态，直到状态为就绪
								DriveStatus driveStatus = GetDriveStatus(drivePathStr);
								while (driveStatus != DRIVE_STATUS_READY) {
									driveStatus = GetDriveStatus(drivePathStr);
								}
								// 驱动器状态为就绪，可以尝试打开驱动器
								HINSTANCE result = ShellExecuteA(NULL, "open", drivePath, NULL, NULL, SW_SHOW);
								// 记录已打开的驱动器
								openedDrives.insert(drivePathStr);
							} else {
								std::cout << "驱动器 " << driveLetter << " 驱动器异常" << std::endl;
								// 驱动器状态为异常，可以执行其他操作
								// 不断检测驱动器状态，直到状态为就绪
								DriveStatus driveStatus = GetDriveStatus(drivePathStr);
								while (driveStatus != DRIVE_STATUS_READY) {
									driveStatus = GetDriveStatus(drivePathStr);
								}
								// 驱动器状态为就绪，可以尝试打开驱动器
								HINSTANCE result = ShellExecuteA(NULL, "open", drivePath, NULL, NULL, SW_SHOW);
								// 记录已打开的驱动器
								openedDrives.insert(drivePathStr);
							}
							break;
						}
						case DRIVE_CDROM: {
							std::cout << "驱动器 " << driveLetter << " 是光盘驱动器" << std::endl;
							// 尝试打开驱动器
							HINSTANCE result = ShellExecuteA(NULL, "open", drivePath, NULL, NULL, SW_SHOW);
							// 记录已打开的驱动器
							openedDrives.insert(drivePathStr);
							break;
						}
						case DRIVE_RAMDISK: {
							std::cout << "驱动器 " << driveLetter << " 是RAM磁盘" << std::endl;
							// 尝试打开驱动器
							HINSTANCE result = ShellExecuteA(NULL, "open", drivePath, NULL, NULL, SW_SHOW);
							// 记录已打开的驱动器
							openedDrives.insert(drivePathStr);
							break;
						}
						default: {
							// 默认情况下执行的操作
							break;
						}
					}
				}
			}
		}
		// 更新驱动器信息
		drives = newDrives;
	}
	return 0;
}

HANDLE tThread = NULL; // 置顶 的全局变量

// 线程函数，用于执行循环置顶操作
DWORD WINAPI TopMostThread(LPVOID lpParam) {
	while (true) {
		SetWindowPos(host_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 置顶主窗口
		SetWindowPos(hwnd_exit, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 置顶退出窗口
		SetWindowPos(hwnd_Settings, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 置顶设置窗口
		SetWindowPos(hwnd_website, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 置顶网站窗口
		SetWindowPos(hwnd_mapping, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 置顶映射窗口
		SetWindowPos(hwnd_iTools, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 置顶工具箱窗口
	}
	return 0;
}

/* 此函数处理主窗口的消息 */
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch (Message) {

		/* 点击销毁按钮时 */
		case WM_CLOSE: {
			// 检查data.json文件是否存在（设置数据）
			std::ifstream file("data.json");
			if (!file) {
				// data.json文件不存在，创建它
				std::ofstream newFile("data.json");
				if (!newFile) {
					MessageBox(NULL, "无法创建设置数据JSON文件！", "错误", MB_ICONEXCLAMATION | MB_OK);
					return 0;
				}
				// 在这里可以写入初始的data.json数据到新创建的文件中
				newFile << "*此文件存储快捷菜单程序的设置数据";
				newFile.close();
				// 检测文件中是否包含对应的值
				std::ifstream file("data.json");
				if (!file) {
					MessageBox(NULL, "无法打开data.json文件！", "错误", MB_ICONEXCLAMATION | MB_OK);
					return 0;
				}
			}

			std::string line;
			bool found1 = false;
			bool found2 = false;

			while (std::getline(file, line)) {
				if (line.find("Never notify = 1") != std::string::npos) {
					found1 = true;
				}
				if (line.find("Never notify = 2") != std::string::npos) {
					found2 = true;
				}
			}

			file.close();

			if (found1) {
				// 包含"...1"的处理逻辑
				// 隐藏到托盘区
				// 最小化到托盘
				ShowWindow(host_hwnd, SW_MINIMIZE);
				ShowWindow(host_hwnd, SW_HIDE);
				// 隐藏“设置”窗口
				ShowWindow(hwnd_Settings, SW_HIDE);
				// 隐藏“网站”窗口
				ShowWindow(hwnd_website, SW_HIDE);
				// 隐藏“映射”窗口
				ShowWindow(hwnd_mapping, SW_HIDE);
				// 隐藏“工具箱”窗口
				ShowWindow(hwnd_iTools, SW_HIDE);
				// 设置系统托盘图标的右下角小消息
				// 创建一个 NOTIFYICONDATA 结构体对象
				NOTIFYICONDATA nid;
				nid.cbSize = sizeof(NOTIFYICONDATA);
				nid.hWnd = host_hwnd;
				nid.uID = 1; // 指定一个唯一的ID
				nid.uFlags = NIF_INFO; // 设置标志以指定要显示信息
				nid.dwInfoFlags = NIIF_INFO; // 设置信息类型
				nid.uTimeout = 5000; // 指定信息显示的时间（毫秒）
				lstrcpy(nid.szInfoTitle, TEXT("快捷菜单")); // 设置信息标题
				lstrcpy(nid.szInfo, TEXT("我在这哦！qwq\n如托盘图标异常可再次打开程序显示OvO")); // 设置消息内容
				Shell_NotifyIcon(NIM_MODIFY, &nid); // 发送消息
			} else  if (found2) {
				// 包含"...2"的处理逻辑
				// 退出程序
				// 在窗口关闭时清空 剪贴板.txt 文件的内容
				std::ofstream file("剪贴板.txt", std::ios::trunc); // 使用 std::ios::trunc 模式打开文件，清空文件内容
				if (file.is_open()) {
					file.close(); // 关闭文件
					std::cout << "文件内容已清空" << std::endl;
				} else {
					std::cerr << "无法打开文件" << std::endl;
				}

				// 销毁窗口
				DestroyWindow(hwnd);
				Shell_NotifyIcon(NIM_DELETE, &nid); // 移除托盘图标
				PostQuitMessage(0); // 如果没有这个，退出窗口时，窗口将一直保持为使用状态
			} else {
				// 其他的处理逻辑
				if (isExitWindowOpen == 0) { // 检查是否已经打开了“退出”子窗口
					/* 使窗口始终出现在屏幕中间 */
					// 获取屏幕宽度和高度
					int screenWidth = GetSystemMetrics(SM_CXSCREEN);
					int screenHeight = GetSystemMetrics(SM_CYSCREEN);
					// 计算窗口左上角坐标
					int windowWidth = 240; // 窗口宽度
					int windowHeight = 175; // 窗口高度
					int windowX = (screenWidth - windowWidth) / 2;
					int windowY = (screenHeight - windowHeight) / 2;
					hwnd_exit = CreateWindowEx(WS_EX_APPWINDOW, "ExitWindowClass", "退出前询问o(╥﹏╥)o", WS_VISIBLE | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, windowX, windowY, windowWidth, windowHeight, NULL, NULL, GetModuleHandle(NULL), NULL);
					ShowWindow(hwnd_exit, SW_SHOW);
					isExitWindowOpen = 1; // 将变量设为1，表示“退出”子窗口已打开
				} else {
					// 置顶后取消置顶窗口提醒用户
					ShowWindow(hwnd_exit, SW_RESTORE);
					SetWindowPos(hwnd_exit, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
					SetWindowPos(hwnd_exit, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				}
			}
			break;
		}

		/* 在销毁时，告诉主线程停止 */
		case WM_DESTROY: { // 销毁时
			Shell_NotifyIcon(NIM_DELETE, &nid); // 移除托盘图标
			PostQuitMessage(0); // 如果没有这个，退出窗口时，窗口将一直保持为使用状态
			break;
		}

		/* 非客户区需要被计算和绘制时发送给窗口的消息 */
		case WM_NCCALCSIZE: {
			// 隐藏边框
			break;
		}

		/* 在窗口上进行绘制操作 */
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			RECT rc;
			GetClientRect(hwnd, &rc);
			// 创建一个白色画刷，并选择到设备上下文中
			HBRUSH hBrush = CreateSolidBrush(RGB(240, 240, 240));
			SelectObject(hdc, hBrush);
			// 绘制菜单栏矩形
			RECT menuBarRect = { rc.left, rc.top, rc.right, rc.top + 32 };
			FillRect(hdc, &menuBarRect, hBrush);
			// 绘制描边
			HPEN hPen = CreatePen(PS_SOLID, 2, RGB(128, 128, 128));
			SelectObject(hdc, hPen);
			MoveToEx(hdc, rc.left, rc.top, NULL);
			LineTo(hdc, rc.right, rc.top);
			LineTo(hdc, rc.right, rc.bottom);
			LineTo(hdc, rc.left, rc.bottom);
			LineTo(hdc, rc.left, rc.top);
			// 绘制“X”按钮
			HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("System"));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(50, 50, 50));
			SetBkMode(hdc, TRANSPARENT);
			TextOut(hdc, rc.right - 24, rc.top + 10, "X", 1);
			// 绘制“-”按钮
			WCHAR minimizeButton[] = { 0x2013, '\0' }; // 使用Unicode码点绘制“-”符号会更长一些
			SetTextColor(hdc, RGB(50, 50, 50));
			SetBkMode(hdc, TRANSPARENT);
			TextOutW(hdc, rc.right - 60, rc.top + 10, minimizeButton, lstrlenW(minimizeButton));
			// 获取窗口图标
			HICON hIcon = (HICON)LoadImage(NULL, "快捷菜单.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
			if (hIcon != NULL) {
				// 计算图标位置
				int iconWidth = 24; // 图标的长
				int iconHeight = 24; // 图标的宽
				int iconX = rc.left + 4;
				int iconY = rc.top + (32 - iconHeight) / 2;
				// 绘制图标
				DrawIconEx(hdc, iconX, iconY, hIcon, iconWidth, iconHeight, 0, NULL, DI_NORMAL);
			}
			// 获取窗口标题
			TCHAR title[256];
			GetWindowText(hwnd, title, 256);
			// 绘制窗口标题
			HFONT hTitleFont = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("微软雅黑"));
			SelectObject(hdc, hTitleFont);
			SetTextColor(hdc, RGB(0, 0, 0));
			SetBkMode(hdc, TRANSPARENT);
			TextOut(hdc, rc.left + 31, rc.top + 7, title, lstrlen(title));
			// 删除画刷、画笔和字体对象
			DeleteObject(hBrush);
			DeleteObject(hPen);
			DeleteObject(hFont);
			DeleteObject(hTitleFont);
			EndPaint(hwnd, &ps);
			break;
		}

		/* 当鼠标在窗口的非客户区移动时，检测鼠标所在位置 */
		case WM_NCHITTEST: {
			POINT pt = { LOWORD(lParam), HIWORD(lParam) };
			ScreenToClient(hwnd, &pt);
			RECT rc;
			GetClientRect(hwnd, &rc);
			// 检测是否点击了“X”按钮
			if (pt.x > rc.right - 37 && pt.x < rc.right - 0 && pt.y > rc.top + 0 && pt.y < rc.top + 32) {
				return HTCLOSE;
			}
			// 检测是否点击了“-”按钮
			if (pt.x > rc.right - 70 && pt.x < rc.right - 35 && pt.y > rc.top + 0 && pt.y < rc.top + 32) {
				return HTMINBUTTON;
			}
			// 检测鼠标所在位置，并返回相应的值
			if (pt.y < rc.top + 32) {
				return HTCAPTION;
			} else if (pt.x < rc.left + 8 && pt.y < rc.top + 8) {
				return HTTOPLEFT;
			} else if (pt.x > rc.right - 8 && pt.y < rc.top + 8) {
				return HTTOPRIGHT;
			} else if (pt.x < rc.left + 8 && pt.y > rc.bottom - 8) {
				return HTBOTTOMLEFT;
			} else if (pt.x > rc.right - 8 && pt.y > rc.bottom - 8) {
				return HTBOTTOMRIGHT;
			} else if (pt.x < rc.left + 8) {
				return HTLEFT;
			} else if (pt.x > rc.right - 8) {
				return HTRIGHT;
			} else if (pt.y < rc.top + 8) {
				return HTTOP;
			} else if (pt.y > rc.bottom - 8) {
				return HTBOTTOM;
			} else {
				return HTCLIENT;
			}
			break;
		}

		/* 当鼠标在窗口的非客户区点击左键时，执行相应操作 */
		case WM_NCLBUTTONUP: {
			if (wParam == HTCLOSE) {
				// 如果点击了“X”按钮，发送关闭窗口的消息
				SendMessage(hwnd, WM_CLOSE, 0, 0);
				return 0;
			} else if (wParam == HTMINBUTTON) {
				// 如果点击了“-”按钮，发送最小化窗口的消息
				SendMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
				return 0;
			}
			break;
		}

		/* 当鼠标在窗口的非客户区按下左键并且在标题栏上，执行以下操作 */
		case WM_NCLBUTTONDOWN: {
			if (wParam == HTCAPTION) {
				SetForegroundWindow(hwnd); // 将窗口设置为前台窗口
				SendMessage(hwnd, WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0); // 发送移动窗口的消息
				return 0;
			}
			break;
		}

		/* 通知窗口的活动状态发生变化 */
		case WM_ACTIVATE: {
			if (wParam == WA_INACTIVE) {
				// 当此程序的焦点丢失时，隐藏标题栏，以防Windows7系统切换到经典主题后出现原标题栏显示的问题
				SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_CAPTION); // 隐藏标题栏
				SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER); // 使改变生效
			}
			break;
		}

		/* 设置静态控件的背景颜色 */
		case WM_CTLCOLORSTATIC: {
			HDC hdcStatic = (HDC)wParam; // 获取静态控件的设备上下文句柄
			SetTextColor(hdcStatic, RGB(0, 0, 0)); // 设置文本颜色为黑色
			SetBkColor(hdcStatic, RGB(255, 255, 255)); // 设置背景颜色为纯白色
			return (INT_PTR)CreateSolidBrush(RGB(255, 255, 255)); // 返回一个纯白色的画刷作为静态控件的背景色
		}

		/* 创建按钮 */
		case WM_CREATE: { // 创建按钮
			A_Top = CreateWindow("button", "置顶此程序的所有窗口", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 25, 80, 170, 15, hwnd, (HMENU)00000, hInst, NULL);
			A_AOU = CreateWindow("button", "自动打开新增的存储设备", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 215, 80, 185, 15, hwnd, (HMENU)857, hInst, NULL); // 自动打开新增的存储设备 = Auto Open Inserted USB
			A_text = CreateWindow("Static", "欢迎使用！", WS_CHILD | WS_VISIBLE, 190, 280, 228, 38, hwnd, NULL, hInst, NULL);
			A_text2 = CreateWindow("Static", "欢迎使用！", WS_CHILD | WS_VISIBLE, 3, 280, 186, 38, hwnd, NULL, hInst, NULL);
			A_text3 = CreateWindow("Static", "欢迎使用快捷菜单！我们效力于打造更适合现代化年轻人的软件界面和使用习惯。感谢您的使用，您可以使用以下功能：", WS_CHILD | WS_VISIBLE, 8, 35, 408, 45, hwnd, NULL, hInst, NULL);
			A_1 = CreateWindow("button", "实用站点快捷打开", WS_CHILD | WS_VISIBLE | BS_FLAT, 10, 105, 140, 45, hwnd, (HMENU)1, hInst, NULL);
			A_2 = CreateWindow("button", "网络映射辅助工具", WS_CHILD | WS_VISIBLE | BS_FLAT, 160, 105, 140, 45, hwnd, (HMENU)2, hInst, NULL);
			A_3 = CreateWindow("button", "实用工具箱", WS_CHILD | WS_VISIBLE | BS_FLAT, 310, 105, 100, 45, hwnd, (HMENU)3, hInst, NULL);
			A_999 = CreateWindow("button", "更多", WS_CHILD | WS_VISIBLE | BS_FLAT, 360, 254, 50, 25, hwnd, (HMENU)999, hInst, NULL);
			A_Jtb = CreateWindow("button", "开启剪贴板", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 10, 260, 90, 15, hwnd, NULL, hInst, NULL);
			A_4 = CreateWindow("button", "打开剪贴板", WS_CHILD | WS_VISIBLE | BS_FLAT, 110, 254, 90, 25, hwnd, (HMENU)1999, hInst, NULL);
			// 创建一个字体的文本
			HFONT hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("微软雅黑"));
			// 将新创建的字体应用于文本控件
			SendMessage(A_Top, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(A_AOU, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(A_text, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(A_text2, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(A_text3, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(A_1, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(A_2, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(A_3, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(A_999, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(A_Jtb, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(A_4, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			// 启动计时器
			SetTimer(hwnd, 1, 500, NULL); // 定时器ID: 1，间隔时间: 500毫秒（0.5秒）
			SetTimer(hwnd, 2, 10000, NULL); // 定时器ID: 2，间隔时间: 10000毫秒（10秒）
			SetTimer(hwnd, 3, 0, NULL); // 定时器ID: 3，间隔时间: 0毫秒（0秒）
			SetTimer(hwnd, 4, 0, NULL); // 定时器ID: 4，间隔时间: 0毫秒（0秒）
			// 随机选择的文本
			std::string motivationalTexts[] = {
				"	           病从口入，祸从口出",      // 1
				"	           成事莫说，覆水难收",      // 2
				"	           知足常乐，终身不辱",      // 3
				"	           人各有心，心各有见",      // 4
				"	           见事莫说，问事不知",      // 5
				"	           人过留名，雁过留声",      // 6
				"	           三思而行，再思可矣",      // 7
				"	           光阴似箭，日月如梭",      // 8
				"	           得忍且忍，得耐且耐",      // 9
				"	    是非终日有，不听自然无",         // 10
				"	    花有重开日，人无再少年",         // 11
				"	    会当凌绝顶，一览众山小",         // 12
				"           凡人不可貌相，海水不可斗量", // 13
				"       得之淡然，失之坦然，顺其自然",   // 14
				"    忍一句，息一怒，饶一着，退一步",    // 15
				"    关关难过关关过，前路漫漫亦灿灿",    // 16
				"    人道洛阳花似锦，偏我来时不逢春",    // 17
				"    命里有时终须有，命里无时莫强求",    // 18
				"    相逢已是上上签，何须相思煮余年",    // 19
				"    心似白云常自在，意如流水任东西"     // 20
			};
			std::srand(static_cast<unsigned int>(std::time(nullptr))); // 设置随机数种子
			int randomIndex = std::rand() % (sizeof(motivationalTexts) / sizeof(motivationalTexts[0]));
			// 将所有文本合并为一个字符串
			std::string usageStr = "		           版本：2.52\n" + motivationalTexts[randomIndex];
			// 更新欢迎文本显示的文本
			SetWindowText(A_text, usageStr.c_str());
			break;
		}

		/* 计时器 */
		case WM_TIMER: {
			if (wParam == 2) { // 判断计时器ID
				// 随机选择的文本
				std::string motivationalTexts[] = {
					"	           病从口入，祸从口出",      // 1
					"	           成事莫说，覆水难收",      // 2
					"	           知足常乐，终身不辱",      // 3
					"	           人各有心，心各有见",      // 4
					"	           见事莫说，问事不知",      // 5
					"	           人过留名，雁过留声",      // 6
					"	           三思而行，再思可矣",      // 7
					"	           光阴似箭，日月如梭",      // 8
					"	           得忍且忍，得耐且耐",      // 9
					"	    是非终日有，不听自然无",         // 10
					"	    花有重开日，人无再少年",         // 11
					"	    会当凌绝顶，一览众山小",         // 12
					"           凡人不可貌相，海水不可斗量", // 13
					"       得之淡然，失之坦然，顺其自然",   // 14
					"    忍一句，息一怒，饶一着，退一步",    // 15
					"    关关难过关关过，前路漫漫亦灿灿",    // 16
					"    人道洛阳花似锦，偏我来时不逢春",    // 17
					"    命里有时终须有，命里无时莫强求",    // 18
					"    相逢已是上上签，何须相思煮余年",    // 19
					"    心似白云常自在，意如流水任东西"     // 20
				};
				std::srand(static_cast<unsigned int>(std::time(nullptr))); // 设置随机数种子
				int randomIndex = std::rand() % (sizeof(motivationalTexts) / sizeof(motivationalTexts[0]));

				// 将所有文本合并为一个字符串
				std::string usageStr = "		           版本：2.52\n" + motivationalTexts[randomIndex];

				// 更新欢迎文本显示的文本
				SetWindowText(A_text, usageStr.c_str());
			}

			if (wParam == 1) { // 判断计时器ID
				// 获取当前CPU占用
				FILETIME idleTime, kernelTime, userTime;
				GetSystemTimes(&idleTime, &kernelTime, &userTime);
				static ULONGLONG lastIdleTime = 0;
				static ULONGLONG lastKernelTime = 0;
				static ULONGLONG lastUserTime = 0;
				ULONGLONG currentIdleTime = (idleTime.dwHighDateTime << 32) | idleTime.dwLowDateTime;
				ULONGLONG currentKernelTime = (kernelTime.dwHighDateTime << 32) | kernelTime.dwLowDateTime;
				ULONGLONG currentUserTime = (userTime.dwHighDateTime << 32) | userTime.dwLowDateTime;
				ULONGLONG idleTimeDiff = currentIdleTime - lastIdleTime;
				ULONGLONG kernelTimeDiff = currentKernelTime - lastKernelTime;
				ULONGLONG userTimeDiff = currentUserTime - lastUserTime;
				ULONGLONG systemTimeDiff = kernelTimeDiff + userTimeDiff;
				double cpuUsage = (1.0 - static_cast<double>(idleTimeDiff) / systemTimeDiff) * 100;
				lastIdleTime = currentIdleTime;
				lastKernelTime = currentKernelTime;
				lastUserTime = currentUserTime;

				// 获取当前内存占用
				MEMORYSTATUSEX memStatus;
				memStatus.dwLength = sizeof(memStatus);
				GlobalMemoryStatusEx(&memStatus);
				double memoryUsage = static_cast<double>(memStatus.ullTotalPhys - memStatus.ullAvailPhys);
				double totalMemory = static_cast<double>(memStatus.ullTotalPhys);
				double memoryUsagePercentage = (memoryUsage / totalMemory) * 100;

				// 获取当前虚拟内存占用
				MEMORYSTATUSEX xunimemStatus;
				memStatus.dwLength = sizeof(memStatus);
				GlobalMemoryStatusEx(&memStatus);
				double xunimemoryUsage = static_cast<double>(memStatus.ullTotalPageFile - memStatus.ullAvailPageFile);
				double xunitotalMemory = static_cast<double>(memStatus.ullTotalPageFile);
				double xunimemoryUsagePercentage = (xunimemoryUsage / xunitotalMemory) * 100;

				// 将所有文本合并为一个字符串
				std::string usageStr = "硬件使用率：CPU：" + std::to_string(static_cast<int>(cpuUsage)) + "%\n内存：" + std::to_string(static_cast<int>(memoryUsagePercentage)) + "%" + "  虚拟内存：" + std::to_string(static_cast<int>(xunimemoryUsagePercentage)) + "%";

				// 更新欢迎2文本显示的文本
				SetWindowText(A_text2, usageStr.c_str());
			}

			if (wParam == 3) { // 判断计时器ID
				// 目标进程名称
				std::string targetProcessName = "快捷菜单.exe";
				int count = 0;

				// 获取进程快照句柄
				HANDLE hProcessSnap;
				PROCESSENTRY32 pe32;
				hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

				// 检查进程快照句柄是否有效
				if (hProcessSnap == INVALID_HANDLE_VALUE) {
					// 输出错误信息
					std::cout << "CreateToolhelp32Snapshot 失败" << std::endl; // 通常是由于权限不足或者系统资源不足导致的。这个函数用于创建系统快照，如果无法获取系统快照，就无法遍历进程列表
					return 1;
				}

				// 设置结构体大小并获取第一个进程信息
				pe32.dwSize = sizeof(PROCESSENTRY32);
				if (!Process32First(hProcessSnap, &pe32)) {
					// 输出错误信息
					std::cout << "Process32First 失败" << std::endl; // 可能是由于无法获取第一个进程的信息，通常是由于系统快照不正确或者无效导致的。这个函数用于获取系统快照中的第一个进程信息
					CloseHandle(hProcessSnap);
					return 1;
				}

				// 遍历进程列表，统计目标进程实例数量
				do {
					if (std::string(pe32.szExeFile) == targetProcessName) {
						count++;
					}
				} while (Process32Next(hProcessSnap, &pe32));

				// 关闭进程快照句柄
				CloseHandle(hProcessSnap);

				// 根据目标进程实例数量输出结果
				if (count > 1) {
					std::cout << "有多个目标进程的实例正在运行（目标进程大于1）" << std::endl;
					// 显示窗口后置顶再取消置顶窗口提醒用户
					ShowWindow(host_hwnd, SW_RESTORE); // 显示主窗口
					ShowWindow(hwnd_exit, SW_RESTORE); // 显示退出窗口
					ShowWindow(hwnd_Settings, SW_RESTORE); // 显示设置窗口
					ShowWindow(hwnd_website, SW_RESTORE); // 显示网站窗口
					ShowWindow(hwnd_mapping, SW_RESTORE); // 显示映射窗口
					ShowWindow(hwnd_iTools, SW_RESTORE); // 显示工具箱窗口
					SetWindowPos(host_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 置顶主窗口
					SetWindowPos(hwnd_exit, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 置顶退出窗口
					SetWindowPos(hwnd_Settings, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 置顶设置窗口
					SetWindowPos(hwnd_website, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 置顶网站窗口
					SetWindowPos(hwnd_mapping, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 置顶映射窗口
					SetWindowPos(hwnd_iTools, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 置顶工具箱窗口

					if (SendMessage(A_Top, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 置顶 复选框是否未被勾选
						SetWindowPos(host_hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 取消置顶主窗口
						SetWindowPos(hwnd_exit, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 取消置顶退出窗口
						SetWindowPos(hwnd_Settings, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 取消置顶设置窗口
						SetWindowPos(hwnd_website, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 取消置顶网站窗口
						SetWindowPos(hwnd_mapping, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 取消置顶映射窗口
						SetWindowPos(hwnd_iTools, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 取消置顶工具箱窗口
					}
				} else {
					std::cout << "没有重复的目标进程实例正在运行（目标进程小于等于1）" << std::endl;
				}
			}

			if (wParam == 4) { // 判断计时器ID
				if (SendMessage(A_Jtb, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 开启剪贴板 复选框被选中
					// 检测Ctrl键是否被按下
					bool ctrlPressed = GetAsyncKeyState(VK_CONTROL) & 0x8000;
					// 检测X键是否被按下
					bool xPressed = GetAsyncKeyState(0x58) & 0x8000; // 0x58 是X键的虚拟键码
					// 检测C键是否被按下
					bool cPressed = GetAsyncKeyState(0x43) & 0x8000; // 0x43 是C键的虚拟键码
					// 判断Ctrl+X或Ctrl+C是否同时按下
					if (ctrlPressed && (xPressed || cPressed)) {
						CtrlxhuoCtrlC++; // 将变量增加，表示已按下
						// 判断全局变量的值
						if (CtrlxhuoCtrlC == 1) { // 如果变量为1表示第一时刻按下的操作
							std::cout << "Ctrl+X或Ctrl+C被按下！" << std::endl;
							// 打开剪贴板
							if (OpenClipboard(nullptr)) {
								// 获取剪贴板中的文本数据
								HANDLE hData = GetClipboardData(CF_TEXT);
								if (hData != nullptr) {
									// 锁定内存块并获取文本数据
									char* pszText = static_cast<char*>(GlobalLock(hData));
									if (pszText != nullptr) {
										// 将文本数据转换为字符串
										std::string clipboardText(pszText);
										// 释放内存块并关闭剪贴板
										GlobalUnlock(hData);
										CloseClipboard();
										// 写入文本数据到文件
										std::ofstream file("剪贴板.txt", std::ios_base::app); // 打开文件进行追加写入
										if (file.is_open()) {
											file << clipboardText << std::endl; // 写入文本并换行
											file.close(); // 关闭文件
											std::cout << "内容已写入到文件 剪贴板.txt 中" << std::endl;
										} else {
											std::cerr << "无法打开文件" << std::endl;
										}
									} else {
										std::cerr << "无法获取剪贴板内容" << std::endl;
									}
								} else {
									std::cerr << "无法获取剪贴板数据" << std::endl;
								}
							} else {
								std::cerr << "无法打开剪贴板" << std::endl;
							}
						}
					} else {
						CtrlxhuoCtrlC = 0; // 将变量设为1，表示已按下
					}
				}
			}
			break;
		}

		case WM_COMMAND: { // 处理按钮点击事件
			switch (wParam) {

				case 857: { // 自动打开新增的存储设备
					if (SendMessage(A_AOU, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 复选框被选中
						hThread = CreateThread(NULL, 0, DriveDetection, NULL, 0, NULL); // 创建线程以执行DriveDetection函数
						if (hThread == NULL) { // 如果线程句柄为空
							MessageBox(hwnd, "无法创建线程！", "错误", MB_OK | MB_ICONERROR); // 显示错误消息框
						}
					} else if (SendMessage(A_AOU, BM_GETCHECK, 0, 0) != BST_CHECKED) { // 复选框未被选中
						if (hThread != NULL) { // 如果线程句柄不为空
							TerminateThread(hThread, 0); // 终止线程
							CloseHandle(hThread); // 关闭线程句柄
							hThread = NULL;
						}
					}
					break;
				}

				case 00000: { // 置顶窗口
					if (SendMessage(A_Top, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断 置顶 复选框是否被勾选
						// 勾选
						// 判断全局变量的值
						if (strongTopChecked == 0) {
							// 全局变量的值为0时的操作
							SetWindowPos(host_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 置顶主窗口
							SetWindowPos(hwnd_exit, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 置顶退出窗口
							SetWindowPos(hwnd_Settings, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 置顶设置窗口
							SetWindowPos(hwnd_website, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 置顶网站窗口
							SetWindowPos(hwnd_mapping, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 置顶映射窗口
							SetWindowPos(hwnd_iTools, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 置顶工具箱窗口
						} else if (strongTopChecked == 1) {
							// 全局变量的值为1时的操作
							tThread = CreateThread(NULL, 0, TopMostThread, NULL, 0, NULL); // 创建线程以执行TopMostThread函数
							if (tThread == NULL) { // 如果线程句柄为空
								MessageBox(hwnd, "无法创建线程！", "错误", MB_OK | MB_ICONERROR); // 显示错误消息框
							}
						}
					} else {
						// 未勾选
						// 判断全局变量的值
						if (strongTopChecked == 0) {
							// 全局变量的值为0时的操作
							SetWindowPos(host_hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 取消置顶主窗口
							SetWindowPos(hwnd_exit, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 取消置顶退出窗口
							SetWindowPos(hwnd_Settings, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 取消置顶设置窗口
							SetWindowPos(hwnd_website, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 取消置顶网站窗口
							SetWindowPos(hwnd_mapping, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 取消置顶映射窗口
							SetWindowPos(hwnd_iTools, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 取消置顶工具箱窗口
						} else if (strongTopChecked == 1) {
							// 全局变量的值为1时的操作
							if (tThread != NULL) { // 如果线程句柄不为空
								TerminateThread(tThread, 0); // 终止线程
								CloseHandle(tThread); // 关闭线程句柄
								tThread = NULL;
								SetWindowPos(host_hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 取消置顶主窗口
								SetWindowPos(hwnd_exit, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 取消置顶退出窗口
								SetWindowPos(hwnd_Settings, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 取消置顶设置窗口
								SetWindowPos(hwnd_website, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 取消置顶网站窗口
								SetWindowPos(hwnd_mapping, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 取消置顶映射窗口
								SetWindowPos(hwnd_iTools, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 取消置顶工具箱窗口
							}
						}
					}
					break;
				}

				case 999: { // 更多（设置）
					if (isSettingsWindowOpen == 0) { // 检查是否已经打开了“更多”子窗口
						/* 使窗口始终出现在屏幕中间 */
						// 获取屏幕宽度和高度
						int screenWidth = GetSystemMetrics(SM_CXSCREEN);
						int screenHeight = GetSystemMetrics(SM_CYSCREEN);
						// 计算窗口左上角坐标
						int windowWidth = 350; // 窗口宽度
						int windowHeight = 375; // 窗口高度
						int windowX = (screenWidth - windowWidth) / 2;
						int windowY = (screenHeight - windowHeight) / 2;

						// 按钮被点击时，创建 更多 子窗口
						hwnd_Settings = CreateWindowEx(WS_EX_APPWINDOW, "SettingsWindowClass", "更多|ω^*)", WS_VISIBLE | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, windowX, windowY, windowWidth, windowHeight, NULL, NULL, GetModuleHandle(NULL), NULL);
						ShowWindow(hwnd_Settings, SW_SHOW);
						isSettingsWindowOpen = 1; // 将变量设为1，表示“更多”子窗口已打开
					} else {
						//MessageBox(hwnd, "窗口已经存在了哦！(*'▽'*)\n关闭弹窗后将显示", "提示=-=", MB_OK | MB_ICONASTERISK);
						// 显示窗口后置顶再取消置顶窗口提醒用户
						ShowWindow(hwnd_Settings, SW_RESTORE);
						SetWindowPos(hwnd_Settings, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
						SetWindowPos(hwnd_Settings, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
					}
					break;
				}

				case 1999: { // 打开剪贴板
					HINSTANCE result = ShellExecute(NULL, "open", "notepad.exe", "剪贴板.txt", NULL, SW_SHOWNORMAL);
					break;
				}

				case 1: { // 常用网站快捷打开
					if (isWebsiteWindowOpen == 0) { // 检查是否已经打开了“站点”子窗口
						/* 使窗口始终出现在屏幕中间 */
						// 获取屏幕宽度和高度
						int screenWidth = GetSystemMetrics(SM_CXSCREEN);
						int screenHeight = GetSystemMetrics(SM_CYSCREEN);
						// 计算窗口左上角坐标
						int windowWidth = 480; // 窗口宽度
						int windowHeight = 365; // 窗口高度
						int windowX = (screenWidth - windowWidth) / 2;
						int windowY = (screenHeight - windowHeight) / 2;

						// 按钮被点击时，创建 常用网站快捷打开 子窗口
						hwnd_website = CreateWindowEx(WS_EX_APPWINDOW, "ChildWindowClass", "实用站点快捷打开(^_-)☆", WS_VISIBLE | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, windowX, windowY, windowWidth, windowHeight, NULL, NULL, GetModuleHandle(NULL), NULL);
						ShowWindow(hwnd_website, SW_SHOW);
						isWebsiteWindowOpen = 1; // 将变量设为1，表示“站点”子窗口已打开
					} else {
						//MessageBox(hwnd, "窗口已经存在了哦！(*'▽'*)\n关闭弹窗后将显示", "提示=-=", MB_OK | MB_ICONASTERISK);
						// 显示窗口后置顶再取消置顶窗口提醒用户
						ShowWindow(hwnd_website, SW_RESTORE);
						SetWindowPos(hwnd_website, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
						SetWindowPos(hwnd_website, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
					}
					break;
				}

				case 2: { // 网络映射辅助工具
					if (isMappingWindowOpen == 0) { // 检查是否已经打开了“映射”子窗口
						/* 使窗口始终出现在屏幕中间 */
						// 获取屏幕宽度和高度
						int screenWidth = GetSystemMetrics(SM_CXSCREEN);
						int screenHeight = GetSystemMetrics(SM_CYSCREEN);
						// 计算窗口左上角坐标
						int windowWidth = 505; // 窗口宽度
						int windowHeight = 345; // 窗口高度
						int windowX = (screenWidth - windowWidth) / 2;
						int windowY = (screenHeight - windowHeight) / 2;

						// 按钮被点击时，创建 网络映射辅助工具 子窗口
						hwnd_mapping = CreateWindowEx(WS_EX_APPWINDOW, childwindow, "网络映射辅助工具(〃^ヮ^〃) ", WS_VISIBLE | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, windowX, windowY, windowWidth, windowHeight, NULL, NULL, GetModuleHandle(NULL), NULL);
						ShowWindow(hwnd_mapping, SW_SHOW);
						isMappingWindowOpen = 1; // 将变量设为1，表示“映射”子窗口已打开
					} else {
						//MessageBox(hwnd, "窗口已经存在了哦！(*'▽'*)\n关闭弹窗后将显示", "提示=-=", MB_OK | MB_ICONASTERISK);
						// 显示窗口后置顶再取消置顶窗口提醒用户
						ShowWindow(hwnd_mapping, SW_RESTORE);
						SetWindowPos(hwnd_mapping, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
						SetWindowPos(hwnd_mapping, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
					}
					break;
				}

				case 3: { // 实用工具箱
					if (isiToolsWindowOpen == 0) { // 检查是否已经打开了“工具箱”子窗口
						/* 使窗口始终出现在屏幕中间 */
						// 获取屏幕宽度和高度
						int screenWidth = GetSystemMetrics(SM_CXSCREEN);
						int screenHeight = GetSystemMetrics(SM_CYSCREEN);
						// 计算窗口左上角坐标
						int windowWidth = 445; // 窗口宽度
						int windowHeight = 350; // 窗口高度
						int windowX = (screenWidth - windowWidth) / 2;
						int windowY = (screenHeight - windowHeight) / 2;

						// 按钮被点击时，创建 实用工具箱 子窗口
						hwnd_iTools = CreateWindowEx(WS_EX_APPWINDOW, "iToolsWindowClass", "实用工具箱(〃^ω^) ", WS_VISIBLE | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, windowX, windowY, windowWidth, windowHeight, NULL, NULL, GetModuleHandle(NULL), NULL);
						ShowWindow(hwnd_iTools, SW_SHOW);
						isiToolsWindowOpen = 1; // 将变量设为1，表示“工具箱”子窗口已打开
					} else {
						//MessageBox(hwnd, "窗口已经存在了哦！(*'▽'*)\n关闭弹窗后将显示", "提示=-=", MB_OK | MB_ICONASTERISK);
						// 显示窗口后置顶再取消置顶窗口提醒用户
						ShowWindow(hwnd_iTools, SW_RESTORE);
						SetWindowPos(hwnd_iTools, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
						SetWindowPos(hwnd_iTools, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
					}
					break;
				}

				case IDM_EXIT: {
					// 托盘退出按钮
					PostQuitMessage(0); // 如果没有这个，退出窗口时，窗口将一直保持为使用状态
					DestroyWindow(hwnd);
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

		/* 托盘图标消息处理 */
		case WM_USER + 1: {
			switch (lParam) {
				// 鼠标左键单击托盘图标
				case WM_LBUTTONDOWN: {
					// 显示窗口
					ShowWindow(hwnd, SW_RESTORE);
					SetForegroundWindow(hwnd);
					// 显示“设置”窗口
					ShowWindow(hwnd_Settings, SW_SHOW);
					// 显示“网站”窗口
					ShowWindow(hwnd_website, SW_SHOW);
					// 显示“映射”窗口
					ShowWindow(hwnd_mapping, SW_SHOW);
					// 隐藏“工具箱”窗口
					ShowWindow(hwnd_iTools, SW_SHOW);
					break;
				}
				// 鼠标右键单击托盘图标
				case WM_RBUTTONDOWN: {
					POINT pt;
					GetCursorPos(&pt);
					// 创建右键菜单
					HMENU hMenu = CreatePopupMenu();
					InsertMenu(hMenu, -1, MF_BYPOSITION, IDM_EXIT, "退出");
					// 显示右键菜单
					SetForegroundWindow(hwnd);
					TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
					DestroyMenu(hMenu);
					break;
				}
			}
			break;
		}

		/* 所有其他消息（很多）都使用默认程序处理 */
		default: {
			return DefWindowProc(hwnd, Message, wParam, lParam);
		}
	}
	return 0;
}

/* 此函数处理 退出 子窗口的消息 */
LRESULT CALLBACK exitWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {

		// 处理子窗口的关闭消息
		case WM_CLOSE: {
			// 关闭窗口
			DestroyWindow(hwnd);
			break;
		}

		/* 在销毁时，告诉主线程停止 */
		case WM_DESTROY: { // 销毁时
			isExitWindowOpen = 0; // 将变量设为0，表示“退出”子窗口已关闭
			break;
		}

		/* 非客户区需要被计算和绘制时发送给窗口的消息 */
		case WM_NCCALCSIZE: {
			// 隐藏边框
			break;
		}

		/* 在窗口上进行绘制操作 */
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			RECT rc;
			GetClientRect(hwnd, &rc);
			// 创建一个白色画刷，并选择到设备上下文中
			HBRUSH hBrush = CreateSolidBrush(RGB(240, 240, 240));
			SelectObject(hdc, hBrush);
			// 绘制菜单栏矩形
			RECT menuBarRect = { rc.left, rc.top, rc.right, rc.top + 32 };
			FillRect(hdc, &menuBarRect, hBrush);
			// 绘制描边
			HPEN hPen = CreatePen(PS_SOLID, 2, RGB(128, 128, 128));
			SelectObject(hdc, hPen);
			MoveToEx(hdc, rc.left, rc.top, NULL);
			LineTo(hdc, rc.right, rc.top);
			LineTo(hdc, rc.right, rc.bottom);
			LineTo(hdc, rc.left, rc.bottom);
			LineTo(hdc, rc.left, rc.top);
			// 绘制“X”按钮
			HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("System"));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(50, 50, 50));
			SetBkMode(hdc, TRANSPARENT);
			TextOut(hdc, rc.right - 24, rc.top + 10, "X", 1);
			// 获取窗口图标
			HICON hIcon = (HICON)LoadImage(NULL, "快捷菜单.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
			if (hIcon != NULL) {
				// 计算图标位置
				int iconWidth = 24; // 图标的长
				int iconHeight = 24; // 图标的宽
				int iconX = rc.left + 4;
				int iconY = rc.top + (32 - iconHeight) / 2;
				// 绘制图标
				DrawIconEx(hdc, iconX, iconY, hIcon, iconWidth, iconHeight, 0, NULL, DI_NORMAL);
			}
			// 获取窗口标题
			TCHAR title[256];
			GetWindowText(hwnd, title, 256);
			// 绘制窗口标题
			HFONT hTitleFont = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("微软雅黑"));
			SelectObject(hdc, hTitleFont);
			SetTextColor(hdc, RGB(0, 0, 0));
			SetBkMode(hdc, TRANSPARENT);
			TextOut(hdc, rc.left + 31, rc.top + 7, title, lstrlen(title));
			// 删除画刷、画笔和字体对象
			DeleteObject(hBrush);
			DeleteObject(hPen);
			DeleteObject(hFont);
			DeleteObject(hTitleFont);
			EndPaint(hwnd, &ps);
			break;
		}

		/* 当鼠标在窗口的非客户区移动时，检测鼠标所在位置 */
		case WM_NCHITTEST: {
			POINT pt = { LOWORD(lParam), HIWORD(lParam) };
			ScreenToClient(hwnd, &pt);
			RECT rc;
			GetClientRect(hwnd, &rc);
			// 检测是否点击了“X”按钮
			if (pt.x > rc.right - 37 && pt.x < rc.right - 0 && pt.y > rc.top + 0 && pt.y < rc.top + 32) {
				return HTCLOSE;
			}
			// 检测鼠标所在位置，并返回相应的值
			if (pt.y < rc.top + 32) {
				return HTCAPTION;
			} else if (pt.x < rc.left + 8 && pt.y < rc.top + 8) {
				return HTTOPLEFT;
			} else if (pt.x > rc.right - 8 && pt.y < rc.top + 8) {
				return HTTOPRIGHT;
			} else if (pt.x < rc.left + 8 && pt.y > rc.bottom - 8) {
				return HTBOTTOMLEFT;
			} else if (pt.x > rc.right - 8 && pt.y > rc.bottom - 8) {
				return HTBOTTOMRIGHT;
			} else if (pt.x < rc.left + 8) {
				return HTLEFT;
			} else if (pt.x > rc.right - 8) {
				return HTRIGHT;
			} else if (pt.y < rc.top + 8) {
				return HTTOP;
			} else if (pt.y > rc.bottom - 8) {
				return HTBOTTOM;
			} else {
				return HTCLIENT;
			}
			break;
		}

		/* 当鼠标在窗口的非客户区点击左键时，执行相应操作 */
		case WM_NCLBUTTONUP: {
			if (wParam == HTCLOSE) {
				// 如果点击了“X”按钮，发送关闭窗口的消息
				SendMessage(hwnd, WM_CLOSE, 0, 0);
				return 0;
			}
			break;
		}

		/* 当鼠标在窗口的非客户区按下左键并且在标题栏上，执行以下操作 */
		case WM_NCLBUTTONDOWN: {
			if (wParam == HTCAPTION) {
				SetForegroundWindow(hwnd); // 将窗口设置为前台窗口
				SendMessage(hwnd, WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0); // 发送移动窗口的消息
				return 0;
			}
			break;
		}

		/* 通知窗口的活动状态发生变化 */
		case WM_ACTIVATE: {
			if (wParam == WA_INACTIVE) {
				// 当此程序的焦点丢失时，隐藏标题栏，以防Windows7系统切换到经典主题后出现原标题栏显示的问题
				SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_CAPTION); // 隐藏标题栏
				SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER); // 使改变生效
			}
			break;
		}

		/* 设置静态控件的背景颜色 */
		case WM_CTLCOLORSTATIC: {
			HDC hdcStatic = (HDC)wParam; // 获取静态控件的设备上下文句柄
			SetTextColor(hdcStatic, RGB(0, 0, 0)); // 设置文本颜色为黑色
			SetBkColor(hdcStatic, RGB(255, 255, 255)); // 设置背景颜色为纯白色
			return (INT_PTR)CreateSolidBrush(RGB(255, 255, 255)); // 返回一个纯白色的画刷作为静态控件的背景色
		}

		/* 创建按钮 */
		case WM_CREATE: { // 创建按钮
			if (SendMessage(A_Top, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断 置顶 复选框是否被勾选
				// 勾选
				SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 置顶
			}
			HWND htext = CreateWindow("Static", "您刚刚点击了关闭按钮，请问您下一步的操作是？", WS_CHILD | WS_VISIBLE, 8, 32, 230, 40, hwnd, NULL, hInst, NULL);
			HWND hHideToTrayCheckbox = CreateWindow("button", "隐藏到托盘区", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 68, 72, 120, 20, hwnd, (HMENU)1, NULL, NULL);
			HWND hExitProgramCheckbox = CreateWindow("button", "退出程序", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 68, 92, 120, 20, hwnd, (HMENU)2, NULL, NULL);
			HWND hNoReminderCheckbox = CreateWindow("button", "不再提醒,下次退出时自动选择", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 18, 110, 210, 20, hwnd, (HMENU)3, NULL, NULL);
			HWND determinebutton = CreateWindow("button", "确认", WS_CHILD | WS_VISIBLE | BS_FLAT, 28, 132, 80, 35, hwnd, (HMENU)4, hInst, NULL);
			HWND cancellationbutton = CreateWindow("button", "取消", WS_CHILD | WS_VISIBLE | BS_FLAT, 132, 132, 80, 35, hwnd, (HMENU)5, hInst, NULL);
			// 创建一个字体的文本
			HFONT hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("微软雅黑"));
			// 将新创建的字体应用于文本控件
			SendMessage(htext, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(hHideToTrayCheckbox, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(hExitProgramCheckbox, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(hNoReminderCheckbox, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(determinebutton, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(cancellationbutton, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			// 检查data.json文件是否存在（设置数据）
			std::ifstream file("data.json");
			if (!file) {
				// data.json文件不存在，创建它
				std::ofstream newFile("data.json");
				if (!newFile) {
					MessageBox(NULL, "无法创建设置数据JSON文件！", "错误", MB_ICONEXCLAMATION | MB_OK);
					return 0;
				}
				// 在这里可以写入初始的data.json数据到新创建的文件中
				newFile << "*此文件存储快捷菜单程序的设置数据";
				newFile.close();
				// 检测文件中是否包含对应的值
				std::ifstream file("data.json");
				if (!file) {
					MessageBox(NULL, "无法打开data.json文件！", "错误", MB_ICONEXCLAMATION | MB_OK);
					return 0;
				}
			}

			std::string line;
			bool found4 = false;
			bool found3 = false;

			while (std::getline(file, line)) {
				if (line.find("Never notify = 3") != std::string::npos) {
					found3 = true;
				}
				if (line.find("Never notify = 4") != std::string::npos) {
					found4 = true;
				}
			}

			file.close();

			if (found3) {
				// 包含"...3"的处理逻辑
				// 设置 隐藏到托盘区 单选框的选择状态为选中
				SendMessage(hHideToTrayCheckbox, BM_SETCHECK, BST_CHECKED, 0);
			} else  if (found4) {
				// 包含"...4"的处理逻辑
				// 设置 退出程序 单选框的选择状态为选中
				SendMessage(hExitProgramCheckbox, BM_SETCHECK, BST_CHECKED, 0);
			} else {
				// 设置 隐藏到托盘区 单选框的选择状态为选中
				SendMessage(hHideToTrayCheckbox, BM_SETCHECK, BST_CHECKED, 0);
			}
			break;
		}

		case WM_COMMAND: { // 处理按钮点击事件
			switch (wParam) {

				case 1 : { // 隐藏到托盘区
					// 检查data.json文件是否存在（设置数据）
					std::ifstream file("data.json");
					if (!file) {
						// data.json文件不存在，创建它
						std::ofstream newFile("data.json");
						if (!newFile) {
							MessageBox(NULL, "无法创建设置数据JSON文件！", "错误", MB_ICONEXCLAMATION | MB_OK);
							return 0;
						}
						// 在这里可以写入初始的data.json数据到新创建的文件中
						newFile << "*此文件存储快捷菜单程序的设置数据\nNever notify = 3";
						newFile.close();
						// 检测文件中是否包含对应的值
						std::ifstream file("data.json");
						if (!file) {
							MessageBox(NULL, "无法打开data.json文件！", "错误", MB_ICONEXCLAMATION | MB_OK);
							return 0;
						}
					} else {
						//清空文件内容
						std::ofstream file("data.json", std::ios::trunc);
						file.close();
						// 写入
						std::ofstream newFile("data.json");
						newFile << "*此文件存储快捷菜单程序的设置数据\nNever notify = 3";
						newFile.close();
					}
					break;
				}

				case 2 : { // 退出程序
					// 检查data.json文件是否存在（设置数据）
					std::ifstream file("data.json");
					if (!file) {
						// data.json文件不存在，创建它
						std::ofstream newFile("data.json");
						if (!newFile) {
							MessageBox(NULL, "无法创建设置数据JSON文件！", "错误", MB_ICONEXCLAMATION | MB_OK);
							return 0;
						}
						// 在这里可以写入初始的data.json数据到新创建的文件中
						newFile << "*此文件存储快捷菜单程序的设置数据\nNever notify = 4";
						newFile.close();
						// 检测文件中是否包含对应的值
						std::ifstream file("data.json");
						if (!file) {
							MessageBox(NULL, "无法打开data.json文件！", "错误", MB_ICONEXCLAMATION | MB_OK);
							return 0;
						}
					} else {
						//清空文件内容
						std::ofstream file("data.json", std::ios::trunc);
						file.close();
						// 写入
						std::ofstream newFile("data.json");
						newFile << "*此文件存储快捷菜单程序的设置数据\nNever notify = 4";
						newFile.close();
					}
					break;
				}

				case 3 : { // 不再提醒...
					HWND hNoReminderCheckbox = GetDlgItem(hwnd, 3); // 获取 不再提醒... 复选框的句柄
					if (SendMessage(hNoReminderCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断 不再提醒... 复选框是否被勾选
						// 勾选
						MessageBox(hwnd_exit, "下次退出时将自动选择，如不想自动选择可在设置中更改", "提示(*^▽^*)", MB_OK | MB_ICONINFORMATION);
					}
					break;
				}

				case 4 : { // 确认
					HWND hNoReminderCheckbox = GetDlgItem(hwnd, 3); // 获取 不再提醒... 复选框的句柄
					if (SendMessage(hNoReminderCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断 不再提醒... 复选框是否被勾选
						// 勾选
						// 检查data.json文件是否存在（设置数据）
						std::ifstream file("data.json");
						if (!file) {
							// data.json文件不存在，创建它
							std::ofstream newFile("data.json");
							if (!newFile) {
								MessageBox(NULL, "无法创建设置数据JSON文件！", "错误", MB_ICONEXCLAMATION | MB_OK);
								return 0;
							}
							// 在这里可以写入初始的data.json数据到新创建的文件中
							newFile << "*此文件存储快捷菜单程序的设置数据";
							newFile.close();
							// 检测文件中是否包含对应的值
							std::ifstream file("data.json");
							if (!file) {
								MessageBox(NULL, "无法打开data.json文件！", "错误", MB_ICONEXCLAMATION | MB_OK);
								return 0;
							}
						}
						HWND hHideToTrayCheckbox = GetDlgItem(hwnd, 1); // 获取“隐藏到托盘区”单选框的句柄
						HWND hExitProgramCheckbox = GetDlgItem(hwnd, 2); // 获取“退出程序”单选框的句柄
						if (SendMessage(hHideToTrayCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) {
							// 用户选择了“隐藏到托盘区”
							//清空文件内容
							std::ofstream file("data.json", std::ios::trunc);
							file.close();
							// 写入
							std::ofstream newFile("data.json");
							newFile << "*此文件存储快捷菜单程序的设置数据\nNever notify = 1";
							newFile.close();
						} else if (SendMessage(hExitProgramCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) {
							// 用户选择了“退出程序”
							//清空文件内容
							std::ofstream file("data.json", std::ios::trunc);
							file.close();
							// 写入
							std::ofstream newFile("data.json");
							newFile << "*此文件存储快捷菜单程序的设置数据\nNever notify = 2";
							newFile.close();
						}
					}

					HWND hHideToTrayCheckbox = GetDlgItem(hwnd, 1); // 获取“隐藏到托盘区”单选框的句柄
					HWND hExitProgramCheckbox = GetDlgItem(hwnd, 2); // 获取“退出程序”单选框的句柄
					if (SendMessage(hHideToTrayCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) {
						// 用户选择了“隐藏到托盘区”
						// 最小化到托盘
						ShowWindow(host_hwnd, SW_MINIMIZE);
						ShowWindow(host_hwnd, SW_HIDE);
						// 隐藏“设置”窗口
						ShowWindow(hwnd_Settings, SW_HIDE);
						// 隐藏“网站”窗口
						ShowWindow(hwnd_website, SW_HIDE);
						// 隐藏“映射”窗口
						ShowWindow(hwnd_mapping, SW_HIDE);
						// 隐藏“工具箱”窗口
						ShowWindow(hwnd_iTools, SW_HIDE);
						// 销毁窗口
						DestroyWindow(hwnd);
						// 设置系统托盘图标的右下角小消息
						// 创建一个 NOTIFYICONDATA 结构体对象
						NOTIFYICONDATA nid;
						nid.cbSize = sizeof(NOTIFYICONDATA);
						nid.hWnd = host_hwnd;
						nid.uID = 1; // 指定一个唯一的ID
						nid.uFlags = NIF_INFO; // 设置标志以指定要显示信息
						nid.dwInfoFlags = NIIF_INFO; // 设置信息类型
						nid.uTimeout = 5000; // 指定信息显示的时间（毫秒）
						lstrcpy(nid.szInfoTitle, TEXT("快捷菜单")); // 设置信息标题
						lstrcpy(nid.szInfo, TEXT("我在这哦！qwq\n如托盘图标异常可再次打开程序显示OvO")); // 设置消息内容
						Shell_NotifyIcon(NIM_MODIFY, &nid); // 发送消息
					} else if (SendMessage(hExitProgramCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) {
						// 用户选择了“退出程序”
						// 在窗口关闭时清空 剪贴板.txt 文件的内容
						std::ofstream file("剪贴板.txt", std::ios::trunc); // 使用 std::ios::trunc 模式打开文件，清空文件内容
						if (file.is_open()) {
							file.close(); // 关闭文件
							std::cout << "文件内容已清空" << std::endl;
						} else {
							std::cerr << "无法打开文件" << std::endl;
						}
						// 销毁窗口
						DestroyWindow(hwnd);
						Shell_NotifyIcon(NIM_DELETE, &nid); // 移除托盘图标
						PostQuitMessage(0); // 如果没有这个，退出窗口时，窗口将一直保持为使用状态
					}
					break;
				}

				case 5 : { // 取消
					// 销毁窗口
					DestroyWindow(hwnd);
					break;
				}

				/* 所有其他消息（其中很多）都使用默认过程处理 */
				default: {
					DefWindowProc(hwnd, msg, wParam, lParam);
					break;
				}
			}
			break;
		}

		/* 所有其他消息（很多）都使用默认程序处理 */
		default: {
			DefWindowProc(hwnd, msg, wParam, lParam);
			break;
		}
	}
}

/* 此函数处理 设置 子窗口的消息 */
LRESULT CALLBACK SettingsWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	// 处理子窗口消息
	switch (msg) {

		// 处理子窗口的关闭消息
		case WM_CLOSE: {
			// 关闭子窗口
			DestroyWindow(hwnd);
			break;
		}

		/* 在销毁时，告诉主线程停止 */
		case WM_DESTROY: { // 销毁时
			isSettingsWindowOpen = 0; // 将变量设为0，表示“网站”子窗口已关闭
			break;
		}

		/* 非客户区需要被计算和绘制时发送给窗口的消息 */
		case WM_NCCALCSIZE: {
			// 隐藏边框
			break;
		}

		/* 在窗口上进行绘制操作 */
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			RECT rc;
			GetClientRect(hwnd, &rc);
			// 创建一个白色画刷，并选择到设备上下文中
			HBRUSH hBrush = CreateSolidBrush(RGB(240, 240, 240));
			SelectObject(hdc, hBrush);
			// 绘制菜单栏矩形
			RECT menuBarRect = { rc.left, rc.top, rc.right, rc.top + 32 };
			FillRect(hdc, &menuBarRect, hBrush);
			// 绘制描边
			HPEN hPen = CreatePen(PS_SOLID, 2, RGB(128, 128, 128));
			SelectObject(hdc, hPen);
			MoveToEx(hdc, rc.left, rc.top, NULL);
			LineTo(hdc, rc.right, rc.top);
			LineTo(hdc, rc.right, rc.bottom);
			LineTo(hdc, rc.left, rc.bottom);
			LineTo(hdc, rc.left, rc.top);
			// 绘制“X”按钮
			HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("System"));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(50, 50, 50));
			SetBkMode(hdc, TRANSPARENT);
			TextOut(hdc, rc.right - 24, rc.top + 10, "X", 1);
			// 绘制“-”按钮
			WCHAR minimizeButton[] = { 0x2013, '\0' }; // 使用Unicode码点绘制“-”符号会更长一些
			SetTextColor(hdc, RGB(50, 50, 50));
			SetBkMode(hdc, TRANSPARENT);
			TextOutW(hdc, rc.right - 60, rc.top + 10, minimizeButton, lstrlenW(minimizeButton));
			// 获取窗口图标
			HICON hIcon = (HICON)LoadImage(NULL, "快捷菜单.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
			if (hIcon != NULL) {
				// 计算图标位置
				int iconWidth = 24; // 图标的长
				int iconHeight = 24; // 图标的宽
				int iconX = rc.left + 4;
				int iconY = rc.top + (32 - iconHeight) / 2;
				// 绘制图标
				DrawIconEx(hdc, iconX, iconY, hIcon, iconWidth, iconHeight, 0, NULL, DI_NORMAL);
			}
			// 获取窗口标题
			TCHAR title[256];
			GetWindowText(hwnd, title, 256);
			// 绘制窗口标题
			HFONT hTitleFont = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("微软雅黑"));
			SelectObject(hdc, hTitleFont);
			SetTextColor(hdc, RGB(0, 0, 0));
			SetBkMode(hdc, TRANSPARENT);
			TextOut(hdc, rc.left + 31, rc.top + 7, title, lstrlen(title));
			// 删除画刷、画笔和字体对象
			DeleteObject(hBrush);
			DeleteObject(hPen);
			DeleteObject(hFont);
			DeleteObject(hTitleFont);
			EndPaint(hwnd, &ps);
			break;
		}

		/* 当鼠标在窗口的非客户区移动时，检测鼠标所在位置 */
		case WM_NCHITTEST: {
			POINT pt = { LOWORD(lParam), HIWORD(lParam) };
			ScreenToClient(hwnd, &pt);
			RECT rc;
			GetClientRect(hwnd, &rc);
			// 检测是否点击了“X”按钮
			if (pt.x > rc.right - 37 && pt.x < rc.right - 0 && pt.y > rc.top + 0 && pt.y < rc.top + 32) {
				return HTCLOSE;
			}
			// 检测是否点击了“-”按钮
			if (pt.x > rc.right - 70 && pt.x < rc.right - 35 && pt.y > rc.top + 0 && pt.y < rc.top + 32) {
				return HTMINBUTTON;
			}
			// 检测鼠标所在位置，并返回相应的值
			if (pt.y < rc.top + 32) {
				return HTCAPTION;
			} else if (pt.x < rc.left + 8 && pt.y < rc.top + 8) {
				return HTTOPLEFT;
			} else if (pt.x > rc.right - 8 && pt.y < rc.top + 8) {
				return HTTOPRIGHT;
			} else if (pt.x < rc.left + 8 && pt.y > rc.bottom - 8) {
				return HTBOTTOMLEFT;
			} else if (pt.x > rc.right - 8 && pt.y > rc.bottom - 8) {
				return HTBOTTOMRIGHT;
			} else if (pt.x < rc.left + 8) {
				return HTLEFT;
			} else if (pt.x > rc.right - 8) {
				return HTRIGHT;
			} else if (pt.y < rc.top + 8) {
				return HTTOP;
			} else if (pt.y > rc.bottom - 8) {
				return HTBOTTOM;
			} else {
				return HTCLIENT;
			}
			break;
		}

		/* 当鼠标在窗口的非客户区点击左键时，执行相应操作 */
		case WM_NCLBUTTONUP: {
			if (wParam == HTCLOSE) {
				// 如果点击了“X”按钮，发送关闭窗口的消息
				SendMessage(hwnd, WM_CLOSE, 0, 0);
				return 0;
			} else if (wParam == HTMINBUTTON) {
				// 如果点击了“-”按钮，发送最小化窗口的消息
				SendMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
				return 0;
			}
			break;
		}

		/* 当鼠标在窗口的非客户区按下左键并且在标题栏上，执行以下操作 */
		case WM_NCLBUTTONDOWN: {
			if (wParam == HTCAPTION) {
				SetForegroundWindow(hwnd); // 将窗口设置为前台窗口
				SendMessage(hwnd, WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0); // 发送移动窗口的消息
				return 0;
			}
			break;
		}

		/* 通知窗口的活动状态发生变化 */
		case WM_ACTIVATE: {
			if (wParam == WA_INACTIVE) {
				// 当此程序的焦点丢失时，隐藏标题栏，以防Windows7系统切换到经典主题后出现原标题栏显示的问题
				SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_CAPTION); // 隐藏标题栏
				SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER); // 使改变生效
			}
			break;
		}

		/* 设置静态控件的背景颜色 */
		//case WM_CTLCOLORSTATIC: {
		//	HDC hdcStatic = (HDC)wParam; // 获取静态控件的设备上下文句柄
		//	SetTextColor(hdcStatic, RGB(0, 0, 0)); // 设置文本颜色为黑色
		//	SetBkColor(hdcStatic, RGB(255, 255, 255)); // 设置背景颜色为纯白色
		//	return (INT_PTR)CreateSolidBrush(RGB(255, 255, 255)); // 返回一个纯白色的画刷作为静态控件的背景色
		//}

		/* 创建按钮 */
		case WM_CREATE: { // 创建按钮
			// 创建TabControl控件
			hTab = CreateWindow(WC_TABCONTROL, "", WS_CHILD | WS_VISIBLE, 1, 30, 348, 344, hwnd, NULL, NULL, NULL);
			// 插入选项卡
			TCITEM tabItem1 = {0};
			tabItem1.mask = TCIF_TEXT;
			tabItem1.pszText = "设置";
			TabCtrl_InsertItem(hTab, 0, &tabItem1);
			TCITEM tabItem2 = {0};
			tabItem2.mask = TCIF_TEXT;
			tabItem2.pszText = "详细";
			TabCtrl_InsertItem(hTab, 1, &tabItem2);
			if (SendMessage(A_Top, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断 置顶 复选框是否被勾选
				// 勾选
				SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 置顶
			}
			// 选项卡页面1
			Settingtext = CreateWindow("BUTTON", "设置-快捷菜单", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 10, 60, 330, 175, hwnd, NULL, NULL, NULL);
			Nevernotifyresetting = CreateWindow("button", "重置不再询问", WS_CHILD | WS_VISIBLE | BS_FLAT, 20, 80, 100, 30, hwnd, (HMENU)1, NULL, NULL);
			Startup = CreateWindow("button", "开机自启动", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 130, 88, 95, 15, hwnd, (HMENU)3, hInst, NULL);

			experimentalfeaturestext = CreateWindow("BUTTON", "实验性功能（慎用，可能有BUG）", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 10, 235, 330, 130, hwnd, NULL, NULL, NULL);
			strongTop = CreateWindow("button", "加强置顶（主窗口开启置顶生效）", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 20, 255, 230, 15, hwnd, (HMENU)2, hInst, NULL);

			// 选项卡页面2
			button_32913 = CreateWindow("button", "详细说明", WS_CHILD | WS_VISIBLE | BS_FLAT, 20, 80, 80, 30, hwnd, (HMENU)32913, hInst, NULL);
			ShowWindow(button_32913, SW_HIDE);
			button_32914 = CreateWindow("button", "更新内容", WS_CHILD | WS_VISIBLE | BS_FLAT, 120, 80, 80, 30, hwnd, (HMENU)32914, hInst, NULL);
			ShowWindow(button_32914, SW_HIDE);

			// 启动计时器
			SetTimer(hwnd, 1, 0, NULL); // 创建计时器，ID：1

			// 创建一个字体的文本
			HFONT hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("微软雅黑"));
			// 将新创建的字体应用于文本控件
			SendMessage(hTab, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(Settingtext, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(Nevernotifyresetting, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(Startup, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(experimentalfeaturestext, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(strongTop, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(button_32913, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(button_32914, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			break;
		}

		case WM_NOTIFY: {
			NMHDR* pnmh = (NMHDR*)lParam;
			if (pnmh->code == TCN_SELCHANGE) {
				// 获取当前选中的选项卡索引
				hTab = pnmh->hwndFrom;
				int tabIndex = TabCtrl_GetCurSel(hTab);

				// 根据选项卡索引显示对应的页面
				switch (tabIndex) {
					case 0: {
						// 选项卡页面1
						ShowWindow(Settingtext, SW_SHOW);
						ShowWindow(Nevernotifyresetting, SW_SHOW);
						ShowWindow(experimentalfeaturestext, SW_SHOW);
						ShowWindow(strongTop, SW_SHOW);
						ShowWindow(Startup, SW_SHOW);

						// 选项卡页面2
						ShowWindow(button_32913, SW_HIDE);
						ShowWindow(button_32914, SW_HIDE);
						break;
					}
					case 1: {
						// 选项卡页面1
						ShowWindow(Settingtext, SW_HIDE);
						ShowWindow(Nevernotifyresetting, SW_HIDE);
						ShowWindow(experimentalfeaturestext, SW_HIDE);
						ShowWindow(strongTop, SW_HIDE);
						ShowWindow(Startup, SW_HIDE);

						// 选项卡页面2
						ShowWindow(button_32913, SW_SHOW);
						ShowWindow(button_32914, SW_SHOW);
						break;
					}
				}
			}
			return 0; // 返回0以表示消息已被正确处理
		}

		/* 计时器 */
		case WM_TIMER: {
			// 根据全局变量的值设置复选框的勾选状态
			SendMessage(strongTop, BM_SETCHECK, strongTopChecked, 0);

			std::ifstream file("data.json");
			std::string line;
			bool found = false;

			while (std::getline(file, line)) {
				if (line.find("Never notify = 1") != std::string::npos || line.find("Never notify = 2") != std::string::npos) {
					// 当两个条件中任意一个满足时执行的代码块
					found = true;
				}
			}

			file.close();

			if (!found) {
				// 设置 重置 按钮状态为禁用
				EnableWindow(Nevernotifyresetting, FALSE);
			} else {
				EnableWindow(Nevernotifyresetting, TRUE);
			}

			HKEY hKey;
			LPCTSTR lpValueName = "快捷菜单";
			// 打开注册表项
			if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) {
				// 检查注册表键值是否存在
				DWORD dataType;
				DWORD dataSize = 0;
				if (RegQueryValueEx(hKey, lpValueName, NULL, &dataType, NULL, &dataSize) == ERROR_SUCCESS) {
					std::cout << "程序已添加到开机启动项！" << std::endl;
					// 设置 开机启动 单选框的选择状态为选中
					SendMessage(Startup, BM_SETCHECK, BST_CHECKED, 0);
				} else {
					std::cout << "程序未添加到开机启动项！" << std::endl;
					// 设置 开机启动 单选框的选择状态为未选中
					SendMessage(Startup, BM_SETCHECK, BST_UNCHECKED, 0);
				}
				// 关闭注册表项
				RegCloseKey(hKey);
			} else {
				std::cout << "打开注册表项失败！" << std::endl;
			}
			break;
		}

		case WM_COMMAND: { // 处理按钮点击事件
			switch (wParam) {

				case 1 : { // 重置...
					//清空文件内容
					std::ofstream file("data.json", std::ios::trunc);
					file.close();
					// 写入
					std::ofstream newFile("data.json");
					newFile << "*此文件存储快捷菜单程序的设置数据";
					newFile.close();
					MessageBox(hwnd, "已完成操作！\\（￣︶￣）/", "已重置！(〃'▽'〃)", MB_OK | MB_ICONINFORMATION);
					break;
				}

				case 2 : {
					// 获取复选框的勾选状态
					int checked = SendMessage(strongTop, BM_GETCHECK, 0, 0);

					// 根据勾选状态更新全局变量
					strongTopChecked = (checked == BST_CHECKED) ? 1 : 0;

					// 在这里可以根据勾选状态执行相应的操作
					if (strongTopChecked == 1) {
						// 复选框被勾选时的操作
						int strongTopChecked = 1;
					} else {
						// 复选框取消勾选时的操作
						int strongTopChecked = 0;
					}

					if (SendMessage(A_Top, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断 置顶 复选框是否被勾选
						// 勾选
						// 判断全局变量的值
						if (strongTopChecked == 0) {
							// 全局变量的值为0时的操作
							if (tThread != NULL) { // 如果线程句柄不为空
								TerminateThread(tThread, 0); // 终止线程
								CloseHandle(tThread); // 关闭线程句柄
								tThread = NULL;
							}
						} else if (strongTopChecked == 1) {
							// 全局变量的值为1时的操作
							tThread = CreateThread(NULL, 0, TopMostThread, NULL, 0, NULL); // 创建线程以执行TopMostThread函数
							if (tThread == NULL) { // 如果线程句柄为空
								MessageBox(hwnd, "无法创建线程！", "错误", MB_OK | MB_ICONERROR); // 显示错误消息框
							}
						}
					}
					break;
				}

				case 3 : {
					HKEY hKey;
					LPCTSTR lpValueName = "快捷菜单";
					// 打开注册表项
					if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) {
						// 检查注册表键值是否存在
						DWORD dataType;
						DWORD dataSize = 0;
						if (RegQueryValueEx(hKey, lpValueName, NULL, &dataType, NULL, &dataSize) == ERROR_SUCCESS) {
							std::cout << "程序已添加到开机启动项！" << std::endl;
							HKEY hKey;
							LPCTSTR lpValueName = "快捷菜单";
							// 打开注册表项
							if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
								// 删除注册表键值
								if (RegDeleteValue(hKey, lpValueName) == ERROR_SUCCESS) {
									std::cout << "程序已成功从开机启动项中移除！" << std::endl;
								} else {
									std::cout << "从开机启动项中移除程序失败！" << std::endl;
								}
								// 关闭注册表项
								RegCloseKey(hKey);
							} else {
								std::cout << "打开注册表项失败！" << std::endl;
							}
						} else {
							std::cout << "程序未添加到开机启动项！" << std::endl;
							HKEY hKey;
							LPCTSTR lpValueName = "快捷菜单";
							char buffer[MAX_PATH];
							GetModuleFileName(NULL, buffer, MAX_PATH);
							std::string programPath(buffer);
							LPCTSTR lpPath = programPath.c_str();
							// 打开注册表项
							if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
								// 设置注册表键值
								if (RegSetValueEx(hKey, lpValueName, 0, REG_SZ, (LPBYTE)lpPath, strlen(lpPath) + 1) == ERROR_SUCCESS) {
									std::cout << "程序已成功添加到开机启动项！" << std::endl;
								} else {
									std::cout << "添加到开机启动项失败！" << std::endl;
								}
								// 关闭注册表项
								RegCloseKey(hKey);
							} else {
								std::cout << "打开注册表项失败！" << std::endl;
							}
						}
						// 关闭注册表项
						RegCloseKey(hKey);
					} else {
						std::cout << "打开注册表项失败！" << std::endl;
					}
					break;
				}

				case 32913: { // 详细说明
					MessageBox(hwnd, "常见问题及解决办法：\n在使用的过程中可能会遇到程序无响应的情况，这是正常情况请您耐心等待几秒，如等待时间过长可自行强制关闭程序（任务栏右键-点击启动任务管理器-切换到应用程序选项页-选择此程序-点击结束任务）\n\n关于您宝贵的意见和反馈：\n我们非常愿意采纳您宝贵的意见和反馈并应用到下一个版本中，但是对于比较有难度的功能会开发缓慢，请您谅解。我们会尽量采纳大部分有用处的意见，并且欢迎您在程序使用中发现BUG（程序漏洞）时反馈给我们以便在下一个版本中修复它。对于底层的BUG或者牵连其他代码的的BUG我们通常需要更久的时间来进行修复，请您谅解！\n\n程序的开发与源代码：\n此程序由小熊猫Dev-C++编译器（版本：6.7.5，最低操作系统要求：Windows XP 32位，安装包名：Dev-Cpp.6.7.5.GCC.9.2.Setup）使用C++语言开发。程序代码暂不开源，如有学习需要我们可为您提供学习思路\n\n使用说明/免责声明：\n我们不会收集您的任何隐私信息，更不会将信息上传至云端。如果您使用此程序造成了任何的后果，均由您本人承担！\n\n我们的用心才能换来您舒服的体验和对我们的支持，最后，感谢您的使用！——开发者：Ling", "详细说明(～￣▽￣)～ ", MB_OK );
					break;
				}

				case 32914: { // 更新内容
					MessageBox(hwnd, "——2.0及之前版本——\n2.0版本及之前版本更新内容省略。2.0版本在2023年7月5日正式开始开发，到2023年9月27日完成开发。2126行代码，共89515个字符\n——2.1版本——\n2.1版本在2023年9月28日正式开始开发，到2023年10月12日完成开发。3194行代码，共142734个字符\n更新内容：1.优化了部分窗口的布局。2.更新了主窗口、设置、网络映射辅助工具、实用工具箱和一些消息框。3.因为局域网发送消息不可用了所以就删除了。4.修复了已知的BUG\n——2.52版本——\n2.52版本在2023年10月31日正式开始开发，到2024年4月24日完成开发，中途暂停过几次开发。7128行代码，共326090个字符\n2.52版本具体更新内容如下：\n1.【新增】时尚的外观（自研仿win10窗口）\n2.【修改】更好看的按钮（按钮漂亮了）\n3.【修改】主窗口右下角新增「更多」按钮，按下后可打开「更多」窗口\n4.【删除】删除了主窗口的「菜单栏」，「更多」和「设置」功能移至「更多」窗口中\n5.【新增】「映射工具」新增「生成专属于我的映射批处理文件」功能\n6.【新增】更好的退出时提醒（选择其中一项选项后，下次自动选择那一项）\n7.【修改】「常用网站快捷打开」窗口改名为「实用站点快捷打开」\n8.【修改】「设置」更新为实时更新机制\n9.【新增】「设置」新增「开机自启动」功能\n10.【新增】增加了一些可爱的颜文字和修改部分弹窗内容（虽然没什么用，但是美观性有了qwq）\n11.【修改】修改一个旧版本就存在的机制：当程序已运行时，再次运行时如果图标加载不正常则会优先显示图标异常提醒，之后显示程序已正在运行的提醒。修改后是：优先显示程序已经正在运行提醒，并且如果图标异常不提醒\n12.【修改】「实用站点」新增更多「网页」按钮，删除了一些不太实用的「网页」按钮\n13.【删除】删去「映射工具」的「使用须知」文本提醒\n14.【新增】「映射工具」新增「显示已占用的盘符」功能\n15.【新增】「映射工具」新增「映射成功后不自动选择下一个共享目录和盘符」功能\n16.【修改】「映射工具」重新分类（修改位置）[设置/其他]内的控件和重新修改「映射/断开」内的控件的位置和大小\n17.【新增】「映射工具」新增「映射成功后不禁用显示密码」功能\n18.【修改】[更多]窗口的[详细]页更新了「详细说明」和「更新内容」的内容\n19.【修改】主窗口右下角的名人名言部分修改（增加和删除），总数为20句\n20.【修改】「映射工具」将映射成功或失败清空密码改为：当映射成功不清空密码，反之相反\n21.【修改】「映射工具」将默认映射倒计时时间改为45分钟\n22.【删除】「映射工具」「锁定密码」删去提示消息\n23.【修改】所有窗口界面字体更换为微软雅黑（终于看着不丑了）\n24.【修改】「映射工具」修改映射内核-“使用WNetAddConnection2A函数”的字体大小\n25.【新增】「实用站点」界面布局与样式更新，新增了一些控件，更简洁了。“网址”选择框的项总数为10项\n26.【修改】「更多」-设置-试验性功能-“加强置顶”改名为“加强置顶（主窗口开启置顶生效）”\n27.【修改】「映射工具」输入框更新：除倒计时时间和盘符外所有输入框都可以无限输入\n28.【新增】「实用站点」“使用浏览器打开”选择框新增“使用Edge浏览器打开”\n29.【新增】当按下按钮多次创建窗口时，弹窗的弹窗文本更改，关闭弹窗后将显示（最小化下依旧生效，以前版本不行）窗口\n30.【删除】删除多开窗口后的提示弹窗（包括此程序多开时），改为直接显示其窗口\n31.【修改】「映射工具」微调其窗口大小，变化不大\n32.【修改】主窗口说明文本更新\n33.【修改】「映射工具」调整其“使用NET命令行命令”名称为“使用NET命令提示符命令\n34.【新增】「实用工具箱」新增“重启资源管理器”按钮及其功能\n35.【新增】「实用工具箱」新增“系统级隐藏文件”按钮及其功能\n36.【新增】「实用工具箱」新增“系统级隐藏文件夹”按钮及其功能\n37.【新增】主窗口新增硬件使用率：“虚拟内存”占用显示\n38.【新增】「映射工具」新增“下/上一个按钮”，用于切换页面\n39.【新增】「映射工具」新增“保存IP地址”和“保存共享目录”功能\n40.【新增】「映射工具」新增“默认IP地址/共享目录存储”功能（在快捷菜单目录下）\n41.【新增】主窗口新增“开启剪贴板”功能，可以方便的查看复制过的内容（目前只能检测并写入Ctrl+X或Ctrl+C的组合键复制内容）\n42.【新增】「映射工具」“详细说明”按钮弹窗新增常见映射返回值信息\n———————\n\n我们的用心才能换来您舒服的体验和对我们的支持，最后，感谢您的使用！——开发者：Ling", "更新内容(o￣▽￣)o  ", MB_OK );
					break;
				}

				/* 所有其他消息（其中很多）都使用默认过程处理 */
				default: {
					DefWindowProc(hwnd, msg, wParam, lParam);
					break;
				}
			}
			break;
		}

		/* 所有其他消息（很多）都使用默认程序处理 */
		default: {
			DefWindowProc(hwnd, msg, wParam, lParam);
			break;
		}
	}
}

/* 此函数处理 常用网站快捷打开 子窗口的消息 */
LRESULT CALLBACK ChildWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	// 处理子窗口消息
	switch (msg) {

		// 处理子窗口的关闭消息
		case WM_CLOSE: {
			// 关闭子窗口
			DestroyWindow(hwnd);
			break;
		}

		/* 在销毁时，告诉主线程停止 */
		case WM_DESTROY: { // 销毁时
			isWebsiteWindowOpen = 0; // 将变量设为0，表示“网站”子窗口已关闭
			break;
		}

		/* 非客户区需要被计算和绘制时发送给窗口的消息 */
		case WM_NCCALCSIZE: {
			// 隐藏边框
			break;
		}

		/* 在窗口上进行绘制操作 */
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			RECT rc;
			GetClientRect(hwnd, &rc);
			// 创建一个白色画刷，并选择到设备上下文中
			HBRUSH hBrush = CreateSolidBrush(RGB(240, 240, 240));
			SelectObject(hdc, hBrush);
			// 绘制菜单栏矩形
			RECT menuBarRect = { rc.left, rc.top, rc.right, rc.top + 32 };
			FillRect(hdc, &menuBarRect, hBrush);
			// 绘制描边
			HPEN hPen = CreatePen(PS_SOLID, 2, RGB(128, 128, 128));
			SelectObject(hdc, hPen);
			MoveToEx(hdc, rc.left, rc.top, NULL);
			LineTo(hdc, rc.right, rc.top);
			LineTo(hdc, rc.right, rc.bottom);
			LineTo(hdc, rc.left, rc.bottom);
			LineTo(hdc, rc.left, rc.top);
			// 绘制“X”按钮
			HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("System"));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(50, 50, 50));
			SetBkMode(hdc, TRANSPARENT);
			TextOut(hdc, rc.right - 24, rc.top + 10, "X", 1);
			// 绘制“-”按钮
			WCHAR minimizeButton[] = { 0x2013, '\0' }; // 使用Unicode码点绘制“-”符号会更长一些
			SetTextColor(hdc, RGB(50, 50, 50));
			SetBkMode(hdc, TRANSPARENT);
			TextOutW(hdc, rc.right - 60, rc.top + 10, minimizeButton, lstrlenW(minimizeButton));
			// 获取窗口图标
			HICON hIcon = (HICON)LoadImage(NULL, "快捷菜单.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
			if (hIcon != NULL) {
				// 计算图标位置
				int iconWidth = 24; // 图标的长
				int iconHeight = 24; // 图标的宽
				int iconX = rc.left + 4;
				int iconY = rc.top + (32 - iconHeight) / 2;
				// 绘制图标
				DrawIconEx(hdc, iconX, iconY, hIcon, iconWidth, iconHeight, 0, NULL, DI_NORMAL);
			}
			// 获取窗口标题
			TCHAR title[256];
			GetWindowText(hwnd, title, 256);
			// 绘制窗口标题
			HFONT hTitleFont = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("微软雅黑"));
			SelectObject(hdc, hTitleFont);
			SetTextColor(hdc, RGB(0, 0, 0));
			SetBkMode(hdc, TRANSPARENT);
			TextOut(hdc, rc.left + 31, rc.top + 7, title, lstrlen(title));
			// 删除画刷、画笔和字体对象
			DeleteObject(hBrush);
			DeleteObject(hPen);
			DeleteObject(hFont);
			DeleteObject(hTitleFont);
			EndPaint(hwnd, &ps);
			break;
		}

		/* 当鼠标在窗口的非客户区移动时，检测鼠标所在位置 */
		case WM_NCHITTEST: {
			POINT pt = { LOWORD(lParam), HIWORD(lParam) };
			ScreenToClient(hwnd, &pt);
			RECT rc;
			GetClientRect(hwnd, &rc);
			// 检测是否点击了“X”按钮
			if (pt.x > rc.right - 37 && pt.x < rc.right - 0 && pt.y > rc.top + 0 && pt.y < rc.top + 32) {
				return HTCLOSE;
			}
			// 检测是否点击了“-”按钮
			if (pt.x > rc.right - 70 && pt.x < rc.right - 35 && pt.y > rc.top + 0 && pt.y < rc.top + 32) {
				return HTMINBUTTON;
			}
			// 检测鼠标所在位置，并返回相应的值
			if (pt.y < rc.top + 32) {
				return HTCAPTION;
			} else if (pt.x < rc.left + 8 && pt.y < rc.top + 8) {
				return HTTOPLEFT;
			} else if (pt.x > rc.right - 8 && pt.y < rc.top + 8) {
				return HTTOPRIGHT;
			} else if (pt.x < rc.left + 8 && pt.y > rc.bottom - 8) {
				return HTBOTTOMLEFT;
			} else if (pt.x > rc.right - 8 && pt.y > rc.bottom - 8) {
				return HTBOTTOMRIGHT;
			} else if (pt.x < rc.left + 8) {
				return HTLEFT;
			} else if (pt.x > rc.right - 8) {
				return HTRIGHT;
			} else if (pt.y < rc.top + 8) {
				return HTTOP;
			} else if (pt.y > rc.bottom - 8) {
				return HTBOTTOM;
			} else {
				return HTCLIENT;
			}
			break;
		}

		/* 当鼠标在窗口的非客户区点击左键时，执行相应操作 */
		case WM_NCLBUTTONUP: {
			if (wParam == HTCLOSE) {
				// 如果点击了“X”按钮，发送关闭窗口的消息
				SendMessage(hwnd, WM_CLOSE, 0, 0);
				return 0;
			} else if (wParam == HTMINBUTTON) {
				// 如果点击了“-”按钮，发送最小化窗口的消息
				SendMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
				return 0;
			}
			break;
		}

		/* 当鼠标在窗口的非客户区按下左键并且在标题栏上，执行以下操作 */
		case WM_NCLBUTTONDOWN: {
			if (wParam == HTCAPTION) {
				SetForegroundWindow(hwnd); // 将窗口设置为前台窗口
				SendMessage(hwnd, WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0); // 发送移动窗口的消息
				return 0;
			}
			break;
		}

		/* 通知窗口的活动状态发生变化 */
		case WM_ACTIVATE: {
			if (wParam == WA_INACTIVE) {
				// 当此程序的焦点丢失时，隐藏标题栏，以防Windows7系统切换到经典主题后出现原标题栏显示的问题
				SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_CAPTION); // 隐藏标题栏
				SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER); // 使改变生效
			}
			break;
		}

		/* 设置静态控件的背景颜色 */
		case WM_CTLCOLORSTATIC: {
			HDC hdcStatic = (HDC)wParam; // 获取静态控件的设备上下文句柄
			SetTextColor(hdcStatic, RGB(0, 0, 0)); // 设置文本颜色为黑色
			SetBkColor(hdcStatic, RGB(255, 255, 255)); // 设置背景颜色为纯白色
			return (INT_PTR)CreateSolidBrush(RGB(255, 255, 255)); // 返回一个纯白色的画刷作为静态控件的背景色
		}

		/* 创建按钮 */
		case WM_CREATE: { // 创建按钮
			if (SendMessage(A_Top, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断 置顶 复选框是否被勾选
				// 勾选
				SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 置顶
			}
			HWND xuanzefenzukuang = CreateWindow("BUTTON", "选择网址打开", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 10, 35, 460, 55, hwnd, NULL, NULL, NULL);
			HWND Usingopen = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_DROPDOWNLIST | WS_VSCROLL, 40, 54, 160, 200, hwnd, (HMENU)1, NULL, NULL);
			SendMessage(Usingopen, CB_ADDSTRING, 0, (LPARAM) "使用默认浏览器打开");
			SendMessage(Usingopen, CB_ADDSTRING, 0, (LPARAM) "使用谷歌浏览器打开");
			SendMessage(Usingopen, CB_ADDSTRING, 0, (LPARAM) "使用Edge浏览器打开");
			SendMessage(Usingopen, CB_SETCURSEL, 0, 0);
			HWND WebsiteURL = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_DROPDOWNLIST | WS_VSCROLL, 200, 54, 180, 200, hwnd, (HMENU)2, NULL, NULL);
			SendMessage(WebsiteURL, CB_ADDSTRING, 0, (LPARAM) "帮小忙-在线工具箱");
			SendMessage(WebsiteURL, CB_ADDSTRING, 0, (LPARAM) "即时工具-在线工具箱");
			SendMessage(WebsiteURL, CB_ADDSTRING, 0, (LPARAM) "PICK FREE免费资源导航");
			SendMessage(WebsiteURL, CB_ADDSTRING, 0, (LPARAM) "Vector Magic转矢量图");
			SendMessage(WebsiteURL, CB_ADDSTRING, 0, (LPARAM) "块语言编程游戏（中文）");
			SendMessage(WebsiteURL, CB_ADDSTRING, 0, (LPARAM) "擦除图中不需要的物体");
			SendMessage(WebsiteURL, CB_ADDSTRING, 0, (LPARAM) "共创世界-Scratch编程");
			SendMessage(WebsiteURL, CB_ADDSTRING, 0, (LPARAM) "极简插件-谷歌插件下载");
			SendMessage(WebsiteURL, CB_ADDSTRING, 0, (LPARAM) "测试视觉反应速度");
			SendMessage(WebsiteURL, CB_ADDSTRING, 0, (LPARAM) "网页版Windows11");
			SendMessage(WebsiteURL, CB_SETCURSEL, 0, 0);
			HWND ConfirmOpen = CreateWindow("BUTTON", "打开", WS_VISIBLE | WS_CHILD | BS_FLAT, 380, 54, 60, 27, hwnd, (HMENU)3, NULL, NULL);

			HWND kuaijiefenzukuang = CreateWindow("BUTTON", "常用快捷打开", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 10, 90, 460, 255, hwnd, NULL, NULL, NULL);
			HWND baiduSearchbutton = CreateWindow("button", "百度搜索", WS_CHILD | WS_VISIBLE | BS_FLAT, 20, 110, 75, 30, hwnd, (HMENU)11, NULL, NULL);
			HWND bingSearchbutton = CreateWindow("button", "必应", WS_CHILD | WS_VISIBLE | BS_FLAT, 110, 110, 70, 30, hwnd, (HMENU)13, NULL, NULL);
			HWND programmingCatbutton = CreateWindow("button", "编程猫社区", WS_CHILD | WS_VISIBLE | BS_FLAT, 195, 110, 90, 30, hwnd, (HMENU)21, NULL, NULL);
			HWND Dingdingbutton = CreateWindow("button", "腾讯扣叮", WS_CHILD | WS_VISIBLE | BS_FLAT, 300, 110, 75, 30, hwnd, (HMENU)22, NULL, NULL);
			HWND bilibilibutton = CreateWindow("button", "哔哩哔哩bilibili", WS_CHILD | WS_VISIBLE | BS_FLAT, 20, 150, 120, 30, hwnd, (HMENU)31, NULL, NULL);
			HWND douyinbutton = CreateWindow("button", "抖音", WS_CHILD | WS_VISIBLE | BS_FLAT, 390, 110, 70, 30, hwnd, (HMENU)32, NULL, NULL);
			HWND convertiobutton = CreateWindow("button", "Convertio文件转换器", WS_CHILD | WS_VISIBLE | BS_FLAT, 160, 150, 160, 30, hwnd, (HMENU)41, NULL, NULL);
			HWND xunjiebutton = CreateWindow("button", "迅捷视频转换器", WS_CHILD | WS_VISIBLE | BS_FLAT, 340, 150, 120, 30, hwnd, (HMENU)42, NULL, NULL);

			HWND wenbentext = CreateWindow("STATIC", "*“使用浏览器打开”对此窗口所有控件生效", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 5, 344, 275, 20, hwnd, NULL, NULL, NULL);

			// 创建一个字体的文本
			HFONT hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("微软雅黑"));
			// 将新创建的字体应用于文本控件
			SendMessage(xuanzefenzukuang, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(Usingopen, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(WebsiteURL, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(ConfirmOpen, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(kuaijiefenzukuang, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(baiduSearchbutton, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(bingSearchbutton, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(programmingCatbutton, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(Dingdingbutton, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(bilibilibutton, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(douyinbutton, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(convertiobutton, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(xunjiebutton, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(wenbentext, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			break;
		}

		case WM_COMMAND: { // 处理按钮点击事件
			switch (wParam) {

				case 3: { // 打开
					HWND Usingopen = GetDlgItem(hwnd, 1); // 获取“浏览器”下拉选择框的句柄
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // 获取选择的索引
					HWND WebsiteURL = GetDlgItem(hwnd, 2); // 获取“网址”下拉选择框的句柄
					int urlIndex = SendMessage(WebsiteURL, CB_GETCURSEL, 0, 0); // 获取选择的网址索引

					const char* url = "";
					if (urlIndex == 0) {
						url = "https://tool.browser.qq.com/"; // 帮小忙-在线工具箱
					} else if (urlIndex == 1) {
						url = "https://www.67tool.com/"; // 即时工具-在线工具箱
					} else if (urlIndex == 2) {
						url = "http://www.pickfree.cn/"; // PICK FREE免费资源导航
					} else if (urlIndex == 3) {
						url = "https://zh.vectormagic.com/"; //  Vector Magic转矢量图
					} else if (urlIndex == 4) {
						url = "https://playground.17coding.net/"; // 块语言编程游戏
					} else if (urlIndex == 5) {
						url = "https://magicstudio.com/zh/magiceraser/"; // 擦除图中不需要的物体
					} else if (urlIndex == 6) {
						url = "https://www.ccw.site/"; // 共创世界
					} else if (urlIndex == 7) {
						url = "https://chrome.zzzmh.cn/"; // 极简插件
					} else if (urlIndex == 8) {
						url = "https://humanbenchmark.com/"; // 反应速度测试
					} else if (urlIndex == 9) {
						url = "https://wintest.andrewstech.me/"; // 网页版Windows11
					}

					if (selectedIndex == 0) {
						// 选择使用默认浏览器打开
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// 选择使用谷歌浏览器打开
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 2) {
						// 选择使用Edge浏览器打开
						ShellExecute(NULL, "open", "msedge.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 11 : { // 百度搜索
					HWND Usingopen = GetDlgItem(hwnd, 1); // 获取下拉选择框的句柄
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // 获取选择的索引
					if (selectedIndex == 0) {
						// 选择使用默认浏览器打开
						const char* url = "https://www.baidu.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// 选择使用谷歌浏览器打开
						const char* url = "https://www.baidu.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 2) {
						// 选择使用Edge浏览器打开
						const char* url = "https://www.baidu.com/";
						ShellExecute(NULL, "open", "msedge.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 13 : { // 必应
					HWND Usingopen = GetDlgItem(hwnd, 1); // 获取下拉选择框的句柄
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // 获取选择的索引
					if (selectedIndex == 0) {
						// 选择使用默认浏览器打开
						const char* url = "https://cn.bing.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// 选择使用谷歌浏览器打开
						const char* url = "https://cn.bing.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 2) {
						// 选择使用Edge浏览器打开
						const char* url = "https://cn.bing.com/";
						ShellExecute(NULL, "open", "msedge.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 21 : { // 编程猫社区
					HWND Usingopen = GetDlgItem(hwnd, 1); // 获取下拉选择框的句柄
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // 获取选择的索引
					if (selectedIndex == 0) {
						// 选择使用默认浏览器打开
						const char* url = "https://shequ.codemao.cn/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// 选择使用谷歌浏览器打开
						const char* url = "https://shequ.codemao.cn/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 2) {
						// 选择使用Edge浏览器打开
						const char* url = "https://shequ.codemao.cn/";
						ShellExecute(NULL, "open", "msedge.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 22 : { // 腾讯扣叮
					HWND Usingopen = GetDlgItem(hwnd, 1); // 获取下拉选择框的句柄
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // 获取选择的索引
					if (selectedIndex == 0) {
						// 选择使用默认浏览器打开
						const char* url = "https://coding.qq.com/home/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// 选择使用谷歌浏览器打开
						const char* url = "https://coding.qq.com/home/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 2) {
						// 选择使用Edge浏览器打开
						const char* url = "https://coding.qq.com/home/";
						ShellExecute(NULL, "open", "msedge.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 31 : { // 哔哩哔哩bilibili
					HWND Usingopen = GetDlgItem(hwnd, 1); // 获取下拉选择框的句柄
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // 获取选择的索引
					if (selectedIndex == 0) {
						// 选择使用默认浏览器打开
						const char* url = "https://www.bilibili.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// 选择使用谷歌浏览器打开
						const char* url = "https://www.bilibili.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 2) {
						// 选择使用Edge浏览器打开
						const char* url = "https://www.bilibili.com/";
						ShellExecute(NULL, "open", "msedge.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 32 : { // 抖音
					HWND Usingopen = GetDlgItem(hwnd, 1); // 获取下拉选择框的句柄
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // 获取选择的索引
					if (selectedIndex == 0) {
						// 选择使用默认浏览器打开
						const char* url = "https://www.douyin.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// 选择使用谷歌浏览器打开
						const char* url = "https://www.douyin.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 2) {
						// 选择使用Edge浏览器打开
						const char* url = "https://www.douyin.com/";
						ShellExecute(NULL, "open", "msedge.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 41 : { // Convertio文件转换器
					HWND Usingopen = GetDlgItem(hwnd, 1); // 获取下拉选择框的句柄
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // 获取选择的索引
					if (selectedIndex == 0) {
						// 选择使用默认浏览器打开
						const char* url = "https://convertio.co/zh/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// 选择使用谷歌浏览器打开
						const char* url = "https://convertio.co/zh/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 2) {
						// 选择使用Edge浏览器打开
						const char* url = "https://convertio.co/zh/";
						ShellExecute(NULL, "open", "msedge.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 42 : { // 迅捷视频转换器
					HWND Usingopen = GetDlgItem(hwnd, 1); // 获取下拉选择框的句柄
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // 获取选择的索引
					if (selectedIndex == 0) {
						// 选择使用默认浏览器打开
						const char* url = "https://app.xunjieshipin.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// 选择使用谷歌浏览器打开
						const char* url = "https://app.xunjieshipin.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 2) {
						// 选择使用Edge浏览器打开
						const char* url = "https://app.xunjieshipin.com/";
						ShellExecute(NULL, "open", "msedge.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				/* 所有其他消息（其中很多）都使用默认过程处理 */
				default: {
					DefWindowProc(hwnd, msg, wParam, lParam);
					break;
				}
			}
			break;
		}

		/* 所有其他消息（很多）都使用默认程序处理 */
		default: {
			DefWindowProc(hwnd, msg, wParam, lParam);
			break;
		}
	}
}

/* 此函数处理 网络映射辅助工具 子窗口的消息 */
LRESULT CALLBACK ChildWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {

		// 处理子窗口的关闭消息
		case WM_CLOSE: {
			if (countdownMinutes == 0 && countdownDuration == 0) { // 如果变量countdownMinutes（分）和countdownDuration（秒）都等于0时
				// 关闭子窗口
				DestroyWindow(hwnd);
			} else {
				int result = MessageBox(hwnd, "正在倒计时，退出将丢失进度，确认退出？(#^.^#)\n您可在主窗口点击退出选择隐藏到托盘区", "提示(⌒_⌒;)", MB_OKCANCEL | MB_ICONQUESTION);
				if (result == IDOK) { // 用户点击了确认按钮
					// 关闭子窗口
					DestroyWindow(hwnd);
				}
			}
			break;
		}

		/* 在销毁时，告诉主线程停止 */
		case WM_DESTROY: { // 销毁时
			isMappingWindowOpen = 0; // 将变量设为0，表示“映射”子窗口已关闭
			hasContentExecuted = false; // 设置标志变量
			noContentExecuted = false; // 设置标志变量
			hasContentExecuted2 = false; // 设置标志变量
			noContentExecuted2 = false; // 设置标志变量
			hasContentExecuted3 = false; // 设置标志变量
			noContentExecuted3 = false; // 设置标志变量
			break;
		}

		/* 非客户区需要被计算和绘制时发送给窗口的消息 */
		case WM_NCCALCSIZE: {
			// 隐藏边框
			break;
		}

		/* 在窗口上进行绘制操作 */
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			RECT rc;
			GetClientRect(hwnd, &rc);
			// 创建一个白色画刷，并选择到设备上下文中
			HBRUSH hBrush = CreateSolidBrush(RGB(240, 240, 240));
			SelectObject(hdc, hBrush);
			// 绘制菜单栏矩形
			RECT menuBarRect = { rc.left, rc.top, rc.right, rc.top + 32 };
			FillRect(hdc, &menuBarRect, hBrush);
			// 绘制描边
			HPEN hPen = CreatePen(PS_SOLID, 2, RGB(128, 128, 128));
			SelectObject(hdc, hPen);
			MoveToEx(hdc, rc.left, rc.top, NULL);
			LineTo(hdc, rc.right, rc.top);
			LineTo(hdc, rc.right, rc.bottom);
			LineTo(hdc, rc.left, rc.bottom);
			LineTo(hdc, rc.left, rc.top);
			// 绘制“X”按钮
			HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("System"));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(50, 50, 50));
			SetBkMode(hdc, TRANSPARENT);
			TextOut(hdc, rc.right - 24, rc.top + 10, "X", 1);
			// 绘制“-”按钮
			WCHAR minimizeButton[] = { 0x2013, '\0' }; // 使用Unicode码点绘制“-”符号会更长一些
			SetTextColor(hdc, RGB(50, 50, 50));
			SetBkMode(hdc, TRANSPARENT);
			TextOutW(hdc, rc.right - 60, rc.top + 10, minimizeButton, lstrlenW(minimizeButton));
			// 获取窗口图标
			HICON hIcon = (HICON)LoadImage(NULL, "快捷菜单.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
			if (hIcon != NULL) {
				// 计算图标位置
				int iconWidth = 24; // 图标的长
				int iconHeight = 24; // 图标的宽
				int iconX = rc.left + 4;
				int iconY = rc.top + (32 - iconHeight) / 2;
				// 绘制图标
				DrawIconEx(hdc, iconX, iconY, hIcon, iconWidth, iconHeight, 0, NULL, DI_NORMAL);
			}
			// 获取窗口标题
			TCHAR title[256];
			GetWindowText(hwnd, title, 256);
			// 绘制窗口标题
			HFONT hTitleFont = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("微软雅黑"));
			SelectObject(hdc, hTitleFont);
			SetTextColor(hdc, RGB(0, 0, 0));
			SetBkMode(hdc, TRANSPARENT);
			TextOut(hdc, rc.left + 31, rc.top + 7, title, lstrlen(title));
			// 删除画刷、画笔和字体对象
			DeleteObject(hBrush);
			DeleteObject(hPen);
			DeleteObject(hFont);
			DeleteObject(hTitleFont);
			EndPaint(hwnd, &ps);
			break;
		}

		/* 当鼠标在窗口的非客户区移动时，检测鼠标所在位置 */
		case WM_NCHITTEST: {
			POINT pt = { LOWORD(lParam), HIWORD(lParam) };
			ScreenToClient(hwnd, &pt);
			RECT rc;
			GetClientRect(hwnd, &rc);
			// 检测是否点击了“X”按钮
			if (pt.x > rc.right - 37 && pt.x < rc.right - 0 && pt.y > rc.top + 0 && pt.y < rc.top + 32) {
				return HTCLOSE;
			}
			// 检测是否点击了“-”按钮
			if (pt.x > rc.right - 70 && pt.x < rc.right - 35 && pt.y > rc.top + 0 && pt.y < rc.top + 32) {
				return HTMINBUTTON;
			}
			// 检测鼠标所在位置，并返回相应的值
			if (pt.y < rc.top + 32) {
				return HTCAPTION;
			} else if (pt.x < rc.left + 8 && pt.y < rc.top + 8) {
				return HTTOPLEFT;
			} else if (pt.x > rc.right - 8 && pt.y < rc.top + 8) {
				return HTTOPRIGHT;
			} else if (pt.x < rc.left + 8 && pt.y > rc.bottom - 8) {
				return HTBOTTOMLEFT;
			} else if (pt.x > rc.right - 8 && pt.y > rc.bottom - 8) {
				return HTBOTTOMRIGHT;
			} else if (pt.x < rc.left + 8) {
				return HTLEFT;
			} else if (pt.x > rc.right - 8) {
				return HTRIGHT;
			} else if (pt.y < rc.top + 8) {
				return HTTOP;
			} else if (pt.y > rc.bottom - 8) {
				return HTBOTTOM;
			} else {
				return HTCLIENT;
			}
			break;
		}

		/* 当鼠标在窗口的非客户区点击左键时，执行相应操作 */
		case WM_NCLBUTTONUP: {
			if (wParam == HTCLOSE) {
				// 如果点击了“X”按钮，发送关闭窗口的消息
				SendMessage(hwnd, WM_CLOSE, 0, 0);
				return 0;
			} else if (wParam == HTMINBUTTON) {
				// 如果点击了“-”按钮，发送最小化窗口的消息
				SendMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
				return 0;
			}
			break;
		}

		/* 当鼠标在窗口的非客户区按下左键并且在标题栏上，执行以下操作 */
		case WM_NCLBUTTONDOWN: {
			if (wParam == HTCAPTION) {
				SetForegroundWindow(hwnd); // 将窗口设置为前台窗口
				SendMessage(hwnd, WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0); // 发送移动窗口的消息
				return 0;
			}
			break;
		}

		/* 通知窗口的活动状态发生变化 */
		case WM_ACTIVATE: {
			if (wParam == WA_INACTIVE) {
				// 当此程序的焦点丢失时，隐藏标题栏，以防Windows7系统切换到经典主题后出现原标题栏显示的问题
				SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_CAPTION); // 隐藏标题栏
				SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER); // 使改变生效
			}
			break;
		}

		/* 设置静态控件的背景颜色 */
		case WM_CTLCOLORSTATIC: {
			HDC hdcStatic = (HDC)wParam; // 获取静态控件的设备上下文句柄
			SetTextColor(hdcStatic, RGB(0, 0, 0)); // 设置文本颜色为黑色
			SetBkColor(hdcStatic, RGB(255, 255, 255)); // 设置背景颜色为纯白色
			return (INT_PTR)CreateSolidBrush(RGB(255, 255, 255)); // 返回一个纯白色的画刷作为静态控件的背景色
		}

		/* 创建按钮 */
		case WM_CREATE: { // 创建按钮
			if (SendMessage(A_Top, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断 置顶 复选框是否被勾选
				// 勾选
				SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 置顶
			}
			// 创建一个字体的文本
			HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("微软雅黑"));
			hGroupBoxMapping = CreateWindow("BUTTON", "映射/断开", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 255, 35, 240, 300, hwnd, NULL, NULL, NULL);
			usernameLabel = CreateWindow("STATIC", "用户名：", WS_VISIBLE | WS_CHILD, 265, 60, 80, 20, hwnd, NULL, NULL, NULL);
			usernameEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 335, 57, 150, 26, hwnd, NULL, NULL, NULL); // 使用WS_EX_CLIENTEDGE样式使输入框变成默认
			passwordLabel = CreateWindow("STATIC", "密码：", WS_VISIBLE | WS_CHILD, 265, 90, 80, 20, hwnd, NULL, NULL, NULL);
			passwordEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD | ES_AUTOHSCROLL, 335, 87, 150, 26, hwnd, NULL, NULL, NULL); // 加上了ES_AUTOHSCROLL以便可以无限输入
			ipLabel = CreateWindow("STATIC", "IP地址：", WS_VISIBLE | WS_CHILD, 265, 120, 80, 20, hwnd, NULL, NULL, NULL);
			ipComboBox = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_AUTOHSCROLL, 335, 117, 150, 200, hwnd, NULL, NULL, NULL); // 加上了CBS_AUTOHSCROLL以便可以无限输入
			SendMessage(ipComboBox, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0)); // 将新创建的字体应用于文本控件
			//SendMessage(ipComboBox, CB_ADDSTRING, 0, (LPARAM) "192.168.60.250");
			//SendMessage(ipComboBox, CB_ADDSTRING, 0, (LPARAM) "192.168.60.199");
			//SendMessage(ipComboBox, CB_SETCURSEL, 0, 0);
			sharedFolderLabel = CreateWindow("STATIC", "共享目录：", WS_VISIBLE | WS_CHILD, 265, 150, 75, 20, hwnd, NULL, NULL, NULL);
			sharedFolderEdit = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_AUTOHSCROLL, 335, 147, 150, 200, hwnd, NULL, NULL, NULL); // 加上了CBS_AUTOHSCROLL以便可以无限输入
			SendMessage(sharedFolderEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0)); // 将新创建的字体应用于文本控件
			//SendMessage(sharedFolderEdit, CB_ADDSTRING, 0, (LPARAM) "共享访问");
			//SendMessage(sharedFolderEdit, CB_ADDSTRING, 0, (LPARAM) "home");
			//SendMessage(sharedFolderEdit, CB_SETCURSEL, 0, 0);
			driveLabel = CreateWindow("STATIC", "盘符：", WS_VISIBLE | WS_CHILD, 265, 180, 80, 20, hwnd, NULL, NULL, NULL);
			driveComboBox = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWNLIST | WS_VSCROLL, 335, 177, 150, 200, hwnd, NULL, NULL, NULL);
			SendMessage(driveComboBox, CB_ADDSTRING, 0, (LPARAM) "Z");
			SendMessage(driveComboBox, CB_ADDSTRING, 0, (LPARAM) "Y");
			SendMessage(driveComboBox, CB_ADDSTRING, 0, (LPARAM) "X");
			SendMessage(driveComboBox, CB_ADDSTRING, 0, (LPARAM) "W");
			SendMessage(driveComboBox, CB_ADDSTRING, 0, (LPARAM) "V");
			SendMessage(driveComboBox, CB_ADDSTRING, 0, (LPARAM) "U");
			SendMessage(driveComboBox, CB_ADDSTRING, 0, (LPARAM) "T");
			SendMessage(driveComboBox, CB_ADDSTRING, 0, (LPARAM) "S");
			SendMessage(driveComboBox, CB_ADDSTRING, 0, (LPARAM) "R");
			SendMessage(driveComboBox, CB_ADDSTRING, 0, (LPARAM) "Q");
			SendMessage(driveComboBox, CB_ADDSTRING, 0, (LPARAM) "P");
			SendMessage(driveComboBox, CB_ADDSTRING, 0, (LPARAM) "O");
			SendMessage(driveComboBox, CB_ADDSTRING, 0, (LPARAM) "N");
			SendMessage(driveComboBox, CB_ADDSTRING, 0, (LPARAM) "M");
			SendMessage(driveComboBox, CB_ADDSTRING, 0, (LPARAM) "L");
			SendMessage(driveComboBox, CB_ADDSTRING, 0, (LPARAM) "K");
			SendMessage(driveComboBox, CB_ADDSTRING, 0, (LPARAM) "J");
			SendMessage(driveComboBox, CB_ADDSTRING, 0, (LPARAM) "I");
			SendMessage(driveComboBox, CB_ADDSTRING, 0, (LPARAM) "H");
			SendMessage(driveComboBox, CB_ADDSTRING, 0, (LPARAM) "G");
			SendMessage(driveComboBox, CB_ADDSTRING, 0, (LPARAM) "F");
			SendMessage(driveComboBox, CB_ADDSTRING, 0, (LPARAM) "E");
			SendMessage(driveComboBox, CB_ADDSTRING, 0, (LPARAM) "D");
			SendMessage(driveComboBox, CB_ADDSTRING, 0, (LPARAM) "C");
			SendMessage(driveComboBox, CB_ADDSTRING, 0, (LPARAM) "B");
			SendMessage(driveComboBox, CB_ADDSTRING, 0, (LPARAM) "A");
			SendMessage(driveComboBox, CB_SETCURSEL, 0, 0); // 设置列表的默认选择为列表的第一项
			countdownLabel = CreateWindow("STATIC", "距断开所有还有：", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 265, 210, 180, 20, hwnd, NULL, NULL, NULL);
			connectButton = CreateWindow("BUTTON", "映射", WS_VISIBLE | WS_CHILD | BS_FLAT, 265, 234, 105, 28, hwnd, (HMENU) 1, NULL, NULL);
			disconnectButton = CreateWindow("BUTTON", "断开全部", WS_VISIBLE | WS_CHILD | BS_FLAT, 380, 234, 105, 28, hwnd, (HMENU) 2, NULL, NULL);
			ForcedisconnectallButton = CreateWindow("BUTTON", "强制断开全部(重启资源管理器)", WS_VISIBLE | WS_CHILD | BS_FLAT, 265, 267, 220, 28, hwnd, (HMENU) 333, NULL, NULL);
			GenerateBatchProcessing = CreateWindow("BUTTON", "生成专属于我的映射批处理文件", WS_VISIBLE | WS_CHILD | BS_FLAT, 265, 300, 220, 28, hwnd, (HMENU) 3339, NULL, NULL);

			hGroupBoxSettings = CreateWindow("BUTTON", "设置/其他", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 10, 35, 240, 300, hwnd, NULL, NULL, NULL);
			yingsheshezhi = CreateWindow("Static", "映射设置", WS_CHILD | WS_VISIBLE, 15, 195, 60, 20, hwnd, NULL, hInst, NULL);
			showPasswordCheckbox = CreateWindow("BUTTON", "显示密码", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 15, 215, 80, 20, hwnd, (HMENU)999, NULL, NULL);
			LockPassword = CreateWindow("BUTTON", "锁定密码", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 105, 215, 80, 20, hwnd, NULL, NULL, NULL);
			daojishi = CreateWindow("Static", "倒计时（时间到后断开全部）", WS_CHILD | WS_VISIBLE, 15, 115, 200, 20, hwnd, NULL, hInst, NULL);
			countdownCheckbox = CreateWindow("BUTTON", "首次映射成功后开启倒计时", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 15, 135, 200, 20, hwnd, (HMENU)3, NULL, NULL);
			hStaticSetCountdownTime = CreateWindow("Static", "设置倒计时时间：", WS_CHILD | WS_VISIBLE, 15, 155, 120, 20, hwnd, NULL, hInst, NULL);
			countdownEdit = CreateWindow("EDIT", "45", WS_VISIBLE | WS_CHILD | WS_BORDER, 135, 155, 40, 20, hwnd, NULL, NULL, NULL);
			countdownUnitComboBox = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_DROPDOWNLIST, 175, 155, 40, 200, hwnd, NULL, NULL, NULL); // 可以使用CBS_DROPDOWNLIST样式使控件无法编辑
			SendMessage(countdownUnitComboBox, CB_ADDSTRING, 0, (LPARAM) "分");
			SendMessage(countdownUnitComboBox, CB_ADDSTRING, 0, (LPARAM) "秒");
			SendMessage(countdownUnitComboBox, CB_SETCURSEL, 0, 0);
			jieshudaojishicheckbox = CreateWindow("BUTTON", "断开后不结束倒计时", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 15, 175, 155, 20, hwnd, NULL, NULL, NULL);
			hStaticMappingKernel = CreateWindow("Static", "映射内核", WS_CHILD | WS_VISIBLE, 15, 55, 60, 20, hwnd, NULL, hInst, NULL);
			hBtnWNetAddConnection2A = CreateWindow("button", "使用WNetAddConnection2A函数", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 15, 75, 230, 20, hwnd, (HMENU)5, NULL, NULL);
			hBtnNetCommandLine = CreateWindow("button", "使用NET命令提示符命令", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 15, 95, 200, 20, hwnd, (HMENU)6, NULL, NULL);
			Saveusername = CreateWindow("BUTTON", "映射成功后保存用户名", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 15, 235, 170, 20, hwnd, NULL, NULL, NULL);
			keyongpancheckbox = CreateWindow("BUTTON", "显示已占用的盘符", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 15, 255, 140, 20, hwnd, (HMENU)198, NULL, NULL);
			zidongxuanze = CreateWindow("BUTTON", "映射成功后不自动选择下一个共享目录和盘符", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | BS_MULTILINE, 15, 275, 231, 35, hwnd, NULL, NULL, NULL); // 可以使用BS_MULTILINE样式使控件自动换行
			detailedButton = CreateWindow("BUTTON", "详细说明", WS_VISIBLE | WS_CHILD | BS_FLAT, 175, 50, 70, 25, hwnd, (HMENU) 4, NULL, NULL);
			bujinyongxsmm = CreateWindow("BUTTON", "映射成功后不禁用显示密码", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 15, 310, 200, 20, hwnd, (HMENU)998, NULL, NULL);
			nextpage = CreateWindow("BUTTON", "下/上一页", WS_VISIBLE | WS_CHILD | BS_FLAT, 95, 50, 70, 25, hwnd, (HMENU) 7, NULL, NULL);
			SaveuserIP = CreateWindow("BUTTON", "映射成功后保存IP地址", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 15, 75, 170, 20, hwnd, NULL, NULL, NULL);
			Saveusershare = CreateWindow("BUTTON", "映射成功后保存共享目录", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 15, 95, 170, 20, hwnd, NULL, NULL, NULL);
			yingsheshezhi2 = CreateWindow("Static", "映射设置", WS_CHILD | WS_VISIBLE, 15, 55, 60, 20, hwnd, NULL, hInst, NULL);
			ShowWindow(SaveuserIP, SW_HIDE);
			ShowWindow(Saveusershare, SW_HIDE);
			ShowWindow(yingsheshezhi2, SW_HIDE);
			// 设置 使用WNetAddConnection2A函数 单选框的选择状态为选中
			SendMessage(hBtnWNetAddConnection2A, BM_SETCHECK, BST_CHECKED, 0);
			//自动帮用户切换到用户名输入框
			SetFocus(usernameEdit);
			// 禁用倒计时
			EnableWindow(countdownLabel, FALSE);
			EnableWindow(countdownEdit, FALSE);
			EnableWindow(countdownUnitComboBox, FALSE);
			// 启动计时器
			SetTimer(hwnd, 2, 0, NULL); // 创建计时器，ID：2
			// 将新创建的字体应用于文本控件
			SendMessage(hGroupBoxMapping, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(usernameLabel, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(usernameEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(passwordLabel, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(passwordEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(ipLabel, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(sharedFolderLabel, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(driveLabel, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(driveComboBox, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(countdownLabel, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(connectButton, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(disconnectButton, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(ForcedisconnectallButton, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(GenerateBatchProcessing, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(hGroupBoxSettings, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(yingsheshezhi, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(showPasswordCheckbox, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(LockPassword, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(daojishi, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(countdownCheckbox, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(hStaticSetCountdownTime, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(countdownEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(countdownUnitComboBox, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(jieshudaojishicheckbox, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(hStaticMappingKernel, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			HFONT hFont2 = CreateFont(19.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("微软雅黑"));
			SendMessage(hBtnWNetAddConnection2A, WM_SETFONT, (WPARAM)hFont2, MAKELPARAM(TRUE, 0));
			SendMessage(hBtnNetCommandLine, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(Saveusername, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(keyongpancheckbox, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(zidongxuanze, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(detailedButton, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(bujinyongxsmm, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(nextpage, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(SaveuserIP, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(Saveusershare, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(yingsheshezhi2, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			break;
		}

		/* 计时器 */
		case WM_TIMER: {

			if (wParam == 2) { // 判断计时器ID

				// 在计时器消息处理函数中定义一个变量保存上一次的盘符状态
				static DWORD previousDriveMask = 0;
				if (SendMessage(keyongpancheckbox, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 显示已占用的盘符 复选框是否未被勾选
					DWORD currentDriveMask = GetLogicalDrives();
					// 检查盘符状态是否发生变化
					if (currentDriveMask != previousDriveMask) {
						bUpdateDriveList = TRUE;
						previousDriveMask = currentDriveMask;
					}
				}

				if (bUpdateDriveList == TRUE) {
					if (SendMessage(keyongpancheckbox, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 显示已占用的盘符 复选框是否未被勾选
						SendMessage(driveComboBox, CB_RESETCONTENT, 0, 0); // 清空列表控件的内容
						// 插入可用（未被占用）的盘符到列表控件中
						char drives[26];
						DWORD drivesMask = GetLogicalDrives();
						int index = 0;
						for (int i = 0; i < 26; i++) {
							if (!(drivesMask & 1)) {
								drives[index] = 'A' + i;
								index++;
							}
							drivesMask >>= 1;
						}
						SendMessage(driveComboBox, CB_RESETCONTENT, 0, 0);
						for (int i = index - 1; i >= 0; i--) {
							char drive[2] = { drives[i], '\0' }; // 添加字符串终止符 '\0'
							SendMessage(driveComboBox, CB_ADDSTRING, 0, (LPARAM)drive);
						}
						SendMessage(driveComboBox, CB_SETCURSEL, 0, 0); // 设置列表的默认选择为列表的第一项
					} else if (SendMessage(keyongpancheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) {
						SendMessage(driveComboBox, CB_RESETCONTENT, 0, 0); // 清空列表控件的内容
						// 插入盘符到列表控件中
						char driveLetter = 'Z';
						for (int i = 0; i < 26; i++) {
							char drive[2] = { driveLetter, '\0' };
							SendMessage(driveComboBox, CB_ADDSTRING, 0, (LPARAM)drive);
							driveLetter--;
						}
						SendMessage(driveComboBox, CB_SETCURSEL, 0, 0); // 设置列表的默认选择为列表的第一项
					}
					bUpdateDriveList = FALSE; // 将标志设置为FALSE，避免重复更新列表内容
				}

				// 获取“密码”输入框中的文本长度
				int length = GetWindowTextLength(passwordEdit);
				if (length == 0) { // 如果输入框为空
					EnableWindow(showPasswordCheckbox, TRUE); // 启用 显示密码 复选框
				}

				// 判断当前位置是否在C盘（用户名存储.txt）
				char currentPath[MAX_PATH];
				GetCurrentDirectoryA(MAX_PATH, currentPath);
				std::string currentDrive = std::string(currentPath).substr(0, 2);
				if (currentDrive == "C:") {
					// C盘中，判断D盘是否存在
					UINT driveType = GetDriveTypeA("D:\\");
					if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
						std::string folderPath = "D:\\快捷菜单存储";
						std::string filePath = folderPath + "\\用户名存储.txt";
						// 判断文件是否存在
						if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
							// 文件不存在，删除用户名的下拉列表
							std::cout << "文件不存在" << std::endl;
							if (!noContentExecuted) {
								DestroyWindow(usernameEdit); // 删除用户名输入框
								// 创建一个没有下拉列表的
								usernameEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 335, 57, 150, 26, hwnd, NULL, NULL, NULL); // 使用WS_EX_CLIENTEDGE样式使输入框变成默认
								hasContentExecuted = false; // 设置标志变量
								noContentExecuted = true; // 设置标志变量
							}
						} else {
							// 文件存在，获取用户名列表并创建下拉列表
							std::cout << "文件已存在" << std::endl;
							// 判断存储文件是否有内容
							std::ifstream inputFile(filePath);
							if (inputFile.is_open()) {
								std::string line;
								if (std::getline(inputFile, line)) {
									// 如果文件有内容，创建下拉列表框并将内容添加到下拉列表中
									if (!hasContentExecuted) {
										DestroyWindow(usernameEdit); // 删除用户名输入框
										// 创建一个有下拉列表的
										usernameEdit = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | WS_VSCROLL | CBS_AUTOHSCROLL, 335, 57, 150, 200, hwnd, NULL, NULL, NULL); // 加上了CBS_AUTOHSCROLL以便可以无限输入
										SendMessage(usernameEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str()); //获取文件中的内容添加到下拉列表中
										while (std::getline(inputFile, line)) {
											SendMessage(usernameEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str());
										}
										hasContentExecuted = true; // 设置标志变量
										noContentExecuted = false; // 设置标志变量
									} else {
										// 检测文件是否发生变化
										std::ifstream file(filePath, std::ios::binary | std::ios::ate);
										static std::streampos previousSize = file.tellg();
										file.seekg(0, std::ios::end);
										std::streampos currentSize = file.tellg();
										if (currentSize != previousSize) {
											previousSize = currentSize;
											DestroyWindow(usernameEdit); // 删除用户名输入框
											// 创建一个有下拉列表的
											usernameEdit = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | WS_VSCROLL | CBS_AUTOHSCROLL, 335, 57, 150, 200, hwnd, NULL, NULL, NULL); // 加上了CBS_AUTOHSCROLL以便可以无限输入
											SendMessage(usernameEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str()); //获取文件中的内容添加到下拉列表中
											while (std::getline(inputFile, line)) {
												SendMessage(usernameEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str());
											}
											std::cout << "文件发生变化" << std::endl;
											return true; // 文件发生变化
										}
										return false; // 文件未发生变化
									}
								} else {
									// 如果文件没有内容，删除下拉列表框
									if (!noContentExecuted) {
										DestroyWindow(usernameEdit); // 删除用户名输入框
										// 创建一个没有下拉列表的
										usernameEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 335, 57, 150, 26, hwnd, NULL, NULL, NULL); // 使用WS_EX_CLIENTEDGE样式使输入框变成默认
										hasContentExecuted = false; // 设置标志变量
										noContentExecuted = true; // 设置标志变量
									}
								}
								inputFile.close();
							}
						}
					} else {
						std::cout << "D盘不存在或非固定/可移动磁盘" << std::endl;
					}
				} else {
					// 不在C盘中，获取当前路径
					char currentPath[MAX_PATH];
					GetCurrentDirectoryA(MAX_PATH, currentPath);
					std::string folderPath = std::string(currentPath);
					std::string filePath = folderPath + "\\用户名存储.txt";
					// 判断文件是否存在
					if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
						// 文件不存在，删除用户名的下拉列表
						std::cout << "文件不存在" << std::endl;
						if (!noContentExecuted) {
							DestroyWindow(usernameEdit); // 删除用户名输入框
							// 创建一个没有下拉列表的
							usernameEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 335, 57, 150, 26, hwnd, NULL, NULL, NULL); // 使用WS_EX_CLIENTEDGE样式使输入框变成默认
							hasContentExecuted = false; // 设置标志变量
							noContentExecuted = true; // 设置标志变量
						}
					} else {
						// 文件存在，获取用户名列表并创建下拉列表
						std::cout << "文件已存在" << std::endl;
						// 判断存储文件是否有内容
						std::ifstream inputFile(filePath);
						if (inputFile.is_open()) {
							std::string line;
							if (std::getline(inputFile, line)) {
								// 如果文件有内容，创建下拉列表框并将内容添加到下拉列表中
								if (!hasContentExecuted) {
									DestroyWindow(usernameEdit); // 删除用户名输入框
									// 创建一个有下拉列表的
									usernameEdit = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | WS_VSCROLL | CBS_AUTOHSCROLL, 335, 57, 150, 200, hwnd, NULL, NULL, NULL); // 加上了CBS_AUTOHSCROLL以便可以无限输入
									SendMessage(usernameEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str()); //获取文件中的内容添加到下拉列表中
									while (std::getline(inputFile, line)) {
										SendMessage(usernameEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str());
									}
									hasContentExecuted = true; // 设置标志变量
									noContentExecuted = false; // 设置标志变量
								} else {
									// 检测文件是否发生变化
									std::ifstream file(filePath, std::ios::binary | std::ios::ate);
									static std::streampos previousSize = file.tellg();
									file.seekg(0, std::ios::end);
									std::streampos currentSize = file.tellg();
									if (currentSize != previousSize) {
										previousSize = currentSize;
										DestroyWindow(usernameEdit); // 删除用户名输入框
										// 创建一个有下拉列表的
										usernameEdit = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | WS_VSCROLL | CBS_AUTOHSCROLL, 335, 57, 150, 200, hwnd, NULL, NULL, NULL); // 加上了CBS_AUTOHSCROLL以便可以无限输入
										SendMessage(usernameEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str()); //获取文件中的内容添加到下拉列表中
										while (std::getline(inputFile, line)) {
											SendMessage(usernameEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str());
										}
										std::cout << "文件发生变化" << std::endl;
										return true; // 文件发生变化
									}
									return false; // 文件未发生变化
								}
							} else {
								// 如果文件没有内容，删除下拉列表框
								if (!noContentExecuted) {
									DestroyWindow(usernameEdit); // 删除用户名输入框
									// 创建一个没有下拉列表的
									usernameEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 335, 57, 150, 26, hwnd, NULL, NULL, NULL); // 使用WS_EX_CLIENTEDGE样式使输入框变成默认
									hasContentExecuted = false; // 设置标志变量
									noContentExecuted = true; // 设置标志变量
								}
							}
							inputFile.close();
						}
					}
					// 创建一个字体的文本
					HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("微软雅黑"));
					// 将新创建的字体应用于文本控件
					SendMessage(usernameEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
				}

				// 判断当前位置是否在C盘（IP地址存储.txt）
				char currentPath2[MAX_PATH];
				GetCurrentDirectoryA(MAX_PATH, currentPath2);
				std::string currentDrive2 = std::string(currentPath2).substr(0, 2);
				if (currentDrive2 == "C:") {
					// C盘中，判断D盘是否存在
					UINT driveType2 = GetDriveTypeA("D:\\");
					if (driveType2 == DRIVE_FIXED || driveType2 == DRIVE_REMOVABLE) {
						std::string folderPath = "D:\\快捷菜单存储";
						std::string filePath = folderPath + "\\IP地址存储.txt";
						// 判断文件是否存在
						if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
							// 文件不存在，删除IP地址的下拉列表
							std::cout << "文件不存在" << std::endl;
							if (!noContentExecuted2) {
								DestroyWindow(ipComboBox); // 删除IP地址输入框
								// 创建一个没有下拉列表的
								ipComboBox = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 335, 117, 150, 26, hwnd, NULL, NULL, NULL); // 用WS_EX_CLIENTEDGE样式使输入框变成默认
								hasContentExecuted2 = false; // 设置标志变量
								noContentExecuted2 = true; // 设置标志变量
							}
						} else {
							// 文件存在，获取IP地址列表并创建下拉列表
							std::cout << "文件已存在" << std::endl;
							// 判断存储文件是否有内容
							std::ifstream inputFile(filePath);
							if (inputFile.is_open()) {
								std::string line;
								if (std::getline(inputFile, line)) {
									// 如果文件有内容，创建下拉列表框并将内容添加到下拉列表中
									if (!hasContentExecuted2) {
										DestroyWindow(ipComboBox); // 删除IP地址输入框
										// 创建一个有下拉列表的
										ipComboBox = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_AUTOHSCROLL, 335, 117, 150, 200, hwnd, NULL, NULL, NULL); // 加上了CBS_AUTOHSCROLL以便可以无限输入
										SendMessage(ipComboBox, CB_ADDSTRING, 0, (LPARAM)line.c_str()); //获取文件中的内容添加到下拉列表中
										while (std::getline(inputFile, line)) {
											SendMessage(ipComboBox, CB_ADDSTRING, 0, (LPARAM)line.c_str());
										}
										hasContentExecuted2 = true; // 设置标志变量
										noContentExecuted2 = false; // 设置标志变量
									} else {
										// 检测文件是否发生变化
										std::ifstream file(filePath, std::ios::binary | std::ios::ate);
										static std::streampos previousSize = file.tellg();
										file.seekg(0, std::ios::end);
										std::streampos currentSize = file.tellg();
										if (currentSize != previousSize) {
											previousSize = currentSize;
											DestroyWindow(ipComboBox); // 删除IP地址输入框
											// 创建一个有下拉列表的
											ipComboBox = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_AUTOHSCROLL, 335, 117, 150, 200, hwnd, NULL, NULL, NULL); // 加上了CBS_AUTOHSCROLL以便可以无限输入
											SendMessage(ipComboBox, CB_ADDSTRING, 0, (LPARAM)line.c_str()); //获取文件中的内容添加到下拉列表中
											while (std::getline(inputFile, line)) {
												SendMessage(ipComboBox, CB_ADDSTRING, 0, (LPARAM)line.c_str());
											}
											std::cout << "文件发生变化" << std::endl;
											return true; // 文件发生变化
										}
										return false; // 文件未发生变化
									}
								} else {
									// 如果文件没有内容，删除下拉列表框
									if (!noContentExecuted2) {
										DestroyWindow(ipComboBox); // 删除IP地址输入框
										// 创建一个没有下拉列表的
										ipComboBox = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 335, 117, 150, 26, hwnd, NULL, NULL, NULL); // 用WS_EX_CLIENTEDGE样式使输入框变成默认
										hasContentExecuted2 = false; // 设置标志变量
										noContentExecuted2 = true; // 设置标志变量
									}
								}
								inputFile.close();
							}
						}
					} else {
						std::cout << "D盘不存在或非固定/可移动磁盘" << std::endl;
					}
				} else {
					// 不在C盘中，获取当前路径
					char currentPath[MAX_PATH];
					GetCurrentDirectoryA(MAX_PATH, currentPath);
					std::string folderPath = std::string(currentPath);
					std::string filePath = folderPath + "\\IP地址存储.txt";
					// 判断文件是否存在
					if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
						// 文件不存在，删除IP地址的下拉列表
						std::cout << "文件不存在" << std::endl;
						if (!noContentExecuted2) {
							DestroyWindow(ipComboBox); // 删除IP地址输入框
							// 创建一个没有下拉列表的
							ipComboBox = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 335, 117, 150, 26, hwnd, NULL, NULL, NULL); // 用WS_EX_CLIENTEDGE样式使输入框变成默认
							hasContentExecuted2 = false; // 设置标志变量
							noContentExecuted2 = true; // 设置标志变量
						}
					} else {
						// 文件存在，获取IP地址列表并创建下拉列表
						std::cout << "文件已存在" << std::endl;
						// 判断存储文件是否有内容
						std::ifstream inputFile(filePath);
						if (inputFile.is_open()) {
							std::string line;
							if (std::getline(inputFile, line)) {
								// 如果文件有内容，创建下拉列表框并将内容添加到下拉列表中
								if (!hasContentExecuted2) {
									DestroyWindow(ipComboBox); // 删除IP地址输入框
									// 创建一个有下拉列表的
									ipComboBox = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_AUTOHSCROLL, 335, 117, 150, 200, hwnd, NULL, NULL, NULL); // 加上了CBS_AUTOHSCROLL以便可以无限输入
									SendMessage(ipComboBox, CB_ADDSTRING, 0, (LPARAM)line.c_str()); //获取文件中的内容添加到下拉列表中
									while (std::getline(inputFile, line)) {
										SendMessage(ipComboBox, CB_ADDSTRING, 0, (LPARAM)line.c_str());
									}
									hasContentExecuted2 = true; // 设置标志变量
									noContentExecuted2 = false; // 设置标志变量
								} else {
									// 检测文件是否发生变化
									std::ifstream file(filePath, std::ios::binary | std::ios::ate);
									static std::streampos previousSize = file.tellg();
									file.seekg(0, std::ios::end);
									std::streampos currentSize = file.tellg();
									if (currentSize != previousSize) {
										previousSize = currentSize;
										DestroyWindow(ipComboBox); // 删除IP地址输入框
										// 创建一个有下拉列表的
										ipComboBox = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_AUTOHSCROLL, 335, 117, 150, 200, hwnd, NULL, NULL, NULL); // 加上了CBS_AUTOHSCROLL以便可以无限输入
										SendMessage(ipComboBox, CB_ADDSTRING, 0, (LPARAM)line.c_str()); //获取文件中的内容添加到下拉列表中
										while (std::getline(inputFile, line)) {
											SendMessage(ipComboBox, CB_ADDSTRING, 0, (LPARAM)line.c_str());
										}
										std::cout << "文件发生变化" << std::endl;
										return true; // 文件发生变化
									}
									return false; // 文件未发生变化
								}
							} else {
								// 如果文件没有内容，删除下拉列表框
								if (!noContentExecuted2) {
									DestroyWindow(ipComboBox); // 删除IP地址输入框
									// 创建一个没有下拉列表的
									ipComboBox = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 335, 117, 150, 26, hwnd, NULL, NULL, NULL); // 用WS_EX_CLIENTEDGE样式使输入框变成默认
									hasContentExecuted2 = false; // 设置标志变量
									noContentExecuted2 = true; // 设置标志变量
								}
							}
							inputFile.close();
						}
					}
					// 创建一个字体的文本
					HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("微软雅黑"));
					// 将新创建的字体应用于文本控件
					SendMessage(ipComboBox, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
				}

				// 读取默认IP地址存储文件
				std::string defaultIPFilePath = "默认IP地址存储.txt";
				// 判断存储文件是否有内容
				std::ifstream defaultFile(defaultIPFilePath);
				if (defaultFile.is_open()) {
					std::string line;
					bool isFirstLine = true;
					std::vector<std::string> defaultIPList; // 存储默认IP地址的列表
					while (std::getline(defaultFile, line)) {
						if (isFirstLine) {
							if (ipComboBox == NULL) { // 判断下拉列表是否不存在
								// 创建下拉列表框
								DestroyWindow(ipComboBox); // 删除IP地址输入框
								ipComboBox = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_AUTOHSCROLL, 335, 117, 150, 200, hwnd, NULL, NULL, NULL); // 加上了CBS_AUTOHSCROLL以便可以无限输入
							}
							isFirstLine = false;
						}
						defaultIPList.push_back(line); // 将内容存储到defaultIPList中
					}
					defaultFile.close();
					// 将默认IP地址逆序添加到IP地址输入框中，确保默认项在上层显示
					for (int i = defaultIPList.size() - 1; i >= 0; --i) {
						SendMessage(ipComboBox, CB_INSERTSTRING, 0, (LPARAM)defaultIPList[i].c_str());
					}
					// 设置默认选择项为最后一个读取的默认IP地址
					SendMessage(ipComboBox, CB_SETCURSEL, 0, 0);
					// 创建一个字体的文本
					HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("微软雅黑"));
					// 将新创建的字体应用于文本控件
					SendMessage(ipComboBox, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
				}

				// 判断当前位置是否在C盘（共享目录存储.txt）
				char currentPath3[MAX_PATH];
				GetCurrentDirectoryA(MAX_PATH, currentPath3);
				std::string currentDrive3 = std::string(currentPath3).substr(0, 2);
				if (currentDrive3 == "C:") {
					// C盘中，判断D盘是否存在
					UINT driveType2 = GetDriveTypeA("D:\\");
					if (driveType2 == DRIVE_FIXED || driveType2 == DRIVE_REMOVABLE) {
						std::string folderPath = "D:\\快捷菜单存储";
						std::string filePath = folderPath + "\\共享目录存储.txt";
						// 判断文件是否存在
						if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
							// 文件不存在，删除共享目录的下拉列表
							std::cout << "文件不存在" << std::endl;
							if (!noContentExecuted3) {
								DestroyWindow(sharedFolderEdit); // 删除共享目录输入框
								// 创建一个没有下拉列表的
								sharedFolderEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 335, 147, 150, 26, hwnd, NULL, NULL, NULL); // 用WS_EX_CLIENTEDGE样式使输入框变成默认
								hasContentExecuted3 = false; // 设置标志变量
								noContentExecuted3 = true; // 设置标志变量
							}
						} else {
							// 文件存在，获取共享目录列表并创建下拉列表
							std::cout << "文件已存在" << std::endl;
							// 判断存储文件是否有内容
							std::ifstream inputFile(filePath);
							if (inputFile.is_open()) {
								std::string line;
								if (std::getline(inputFile, line)) {
									// 如果文件有内容，创建下拉列表框并将内容添加到下拉列表中
									if (!hasContentExecuted3) {
										DestroyWindow(sharedFolderEdit); // 删除共享目录输入框
										// 创建一个有下拉列表的
										sharedFolderEdit = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_AUTOHSCROLL, 335, 147, 150, 200, hwnd, NULL, NULL, NULL); // 加上了CBS_AUTOHSCROLL以便可以无限输入
										SendMessage(sharedFolderEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str()); //获取文件中的内容添加到下拉列表中
										while (std::getline(inputFile, line)) {
											SendMessage(sharedFolderEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str());
										}
										hasContentExecuted3 = true; // 设置标志变量
										noContentExecuted3 = false; // 设置标志变量
									} else {
										// 检测文件是否发生变化
										std::ifstream file(filePath, std::ios::binary | std::ios::ate);
										static std::streampos previousSize = file.tellg();
										file.seekg(0, std::ios::end);
										std::streampos currentSize = file.tellg();
										if (currentSize != previousSize) {
											previousSize = currentSize;
											DestroyWindow(sharedFolderEdit); // 删除共享目录输入框
											// 创建一个有下拉列表的
											sharedFolderEdit = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_AUTOHSCROLL, 335, 147, 150, 200, hwnd, NULL, NULL, NULL); // 加上了CBS_AUTOHSCROLL以便可以无限输入
											SendMessage(sharedFolderEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str()); //获取文件中的内容添加到下拉列表中
											while (std::getline(inputFile, line)) {
												SendMessage(sharedFolderEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str());
											}
											std::cout << "文件发生变化" << std::endl;
											return true; // 文件发生变化
										}
										return false; // 文件未发生变化
									}
								} else {
									// 如果文件没有内容，删除下拉列表框
									if (!noContentExecuted3) {
										DestroyWindow(sharedFolderEdit); // 删除共享目录输入框
										// 创建一个没有下拉列表的
										sharedFolderEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 335, 147, 150, 26, hwnd, NULL, NULL, NULL); // 用WS_EX_CLIENTEDGE样式使输入框变成默认
										hasContentExecuted3 = false; // 设置标志变量
										noContentExecuted3 = true; // 设置标志变量
									}
								}
								inputFile.close();
							}
						}
					} else {
						std::cout << "D盘不存在或非固定/可移动磁盘" << std::endl;
					}
				} else {
					// 不在C盘中，获取当前路径
					char currentPath[MAX_PATH];
					GetCurrentDirectoryA(MAX_PATH, currentPath);
					std::string folderPath = std::string(currentPath);
					std::string filePath = folderPath + "\\共享目录存储.txt";
					// 判断文件是否存在
					if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
						// 文件不存在，删除共享目录的下拉列表
						std::cout << "文件不存在" << std::endl;
						if (!noContentExecuted3) {
							DestroyWindow(sharedFolderEdit); // 删除共享目录输入框
							// 创建一个没有下拉列表的
							sharedFolderEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 335, 147, 150, 26, hwnd, NULL, NULL, NULL); // 用WS_EX_CLIENTEDGE样式使输入框变成默认
							hasContentExecuted3 = false; // 设置标志变量
							noContentExecuted3 = true; // 设置标志变量
						}
					} else {
						// 文件存在，获取共享目录列表并创建下拉列表
						std::cout << "文件已存在" << std::endl;
						// 判断存储文件是否有内容
						std::ifstream inputFile(filePath);
						if (inputFile.is_open()) {
							std::string line;
							if (std::getline(inputFile, line)) {
								// 如果文件有内容，创建下拉列表框并将内容添加到下拉列表中
								if (!hasContentExecuted3) {
									DestroyWindow(sharedFolderEdit); // 删除共享目录输入框
									// 创建一个有下拉列表的
									sharedFolderEdit = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_AUTOHSCROLL, 335, 147, 150, 200, hwnd, NULL, NULL, NULL); // 加上了CBS_AUTOHSCROLL以便可以无限输入
									SendMessage(sharedFolderEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str()); //获取文件中的内容添加到下拉列表中
									while (std::getline(inputFile, line)) {
										SendMessage(sharedFolderEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str());
									}
									hasContentExecuted3 = true; // 设置标志变量
									noContentExecuted3 = false; // 设置标志变量
								} else {
									// 检测文件是否发生变化
									std::ifstream file(filePath, std::ios::binary | std::ios::ate);
									static std::streampos previousSize = file.tellg();
									file.seekg(0, std::ios::end);
									std::streampos currentSize = file.tellg();
									if (currentSize != previousSize) {
										previousSize = currentSize;
										DestroyWindow(sharedFolderEdit); // 删除共享目录输入框
										// 创建一个有下拉列表的
										sharedFolderEdit = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_AUTOHSCROLL, 335, 147, 150, 200, hwnd, NULL, NULL, NULL); // 加上了CBS_AUTOHSCROLL以便可以无限输入
										SendMessage(sharedFolderEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str()); //获取文件中的内容添加到下拉列表中
										while (std::getline(inputFile, line)) {
											SendMessage(sharedFolderEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str());
										}
										std::cout << "文件发生变化" << std::endl;
										return true; // 文件发生变化
									}
									return false; // 文件未发生变化
								}
							} else {
								// 如果文件没有内容，删除下拉列表框
								if (!noContentExecuted3) {
									DestroyWindow(sharedFolderEdit); // 删除共享目录输入框
									// 创建一个没有下拉列表的
									sharedFolderEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 335, 147, 150, 26, hwnd, NULL, NULL, NULL); // 用WS_EX_CLIENTEDGE样式使输入框变成默认
									hasContentExecuted3 = false; // 设置标志变量
									noContentExecuted3 = true; // 设置标志变量
								}
							}
							inputFile.close();
						}
					}
					// 创建一个字体的文本
					HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("微软雅黑"));
					// 将新创建的字体应用于文本控件
					SendMessage(sharedFolderEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
				}

				// 读取默认共享目录存储文件（推荐文本使用ANSI格式，否则可能不支持中文）
				std::string defaultIPFilePath2 = "默认共享目录存储.txt";
				// 判断存储文件是否有内容
				std::ifstream defaultFile2(defaultIPFilePath2);
				if (defaultFile2.is_open()) {
					std::string line;
					bool isFirstLine = true;
					std::vector<std::string> defaultIPList; // 存储默认共享目录的列表
					while (std::getline(defaultFile2, line)) {
						if (isFirstLine) {
							if (sharedFolderEdit == NULL) { // 判断下拉列表是否不存在
								// 创建下拉列表框
								DestroyWindow(sharedFolderEdit); // 删除共享目录输入框
								sharedFolderEdit = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_AUTOHSCROLL, 335, 147, 150, 200, hwnd, NULL, NULL, NULL); // 加上了CBS_AUTOHSCROLL以便可以无限输入
							}
							isFirstLine = false;
						}
						defaultIPList.push_back(line); // 将内容存储到defaultIPList中
					}
					defaultFile2.close();
					// 将默认共享目录逆序添加到共享目录输入框中，确保默认项在上层显示
					for (int i = defaultIPList.size() - 1; i >= 0; --i) {
						SendMessage(sharedFolderEdit, CB_INSERTSTRING, 0, (LPARAM)defaultIPList[i].c_str());
					}
					// 设置默认选择项为最后一个读取的默认共享目录
					SendMessage(sharedFolderEdit, CB_SETCURSEL, 0, 0);
					// 创建一个字体的文本
					HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("微软雅黑"));
					// 将新创建的字体应用于文本控件
					SendMessage(sharedFolderEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
				}

			}

			if (wParam == 1) { // 判断计时器ID
				if (countdownDuration > 0 || countdownMinutes > 0) {
					if (SendMessage(countdownUnitComboBox, CB_GETCURSEL, 0, 0) == 0) { // 选择的是"分"
						if (countdownDuration == 0 && countdownMinutes > 0) {
							countdownMinutes--; // 减少倒计时的分钟
							countdownDuration = 59; // 将倒计时的秒重置为59
						} else {
							countdownDuration--; // 减少倒计时的秒
						}
					} else if (SendMessage(countdownUnitComboBox, CB_GETCURSEL, 0, 0) == 1) { // 选择的是"秒"
						if (countdownMinutes > 0) {
							countdownMinutes--; // 减少倒计时的分钟（秒）
						}
					}
					// 更新倒计时标签
					char countdownLabelBuf[256];
					if (SendMessage(countdownUnitComboBox, CB_GETCURSEL, 0, 0) == 0) { // 选择的是"分"
						sprintf(countdownLabelBuf, "距断开所有还有：%02d:%02d", countdownMinutes, countdownDuration);
					} else if (SendMessage(countdownUnitComboBox, CB_GETCURSEL, 0, 0) == 1) { // 选择的是"秒"
						sprintf(countdownLabelBuf, "距断开所有还有：%02d秒", countdownMinutes);
					}
					SetWindowText(countdownLabel, countdownLabelBuf);

					if (countdownMinutes == 0 && countdownDuration == 0) {
						// 当倒计时达到零时断开映射的驱动器
						if (SendMessage(hBtnWNetAddConnection2A, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断单选框哪个被选择
							typedef DWORD(WINAPI * LPWNETCANCELCONNECTION2A)(LPCSTR, DWORD, BOOL);
							// 加载库文件
							HMODULE hMpr = LoadLibrary("mpr.dll");
							if (hMpr == NULL) {
								std::cout << "无法加载mpr.dll库文件" << std::endl;
								return 1;
							}
							// 获取函数地址
							LPWNETCANCELCONNECTION2A pWNetCancelConnection2A = (LPWNETCANCELCONNECTION2A)GetProcAddress(hMpr, "WNetCancelConnection2A");
							if (pWNetCancelConnection2A == NULL) {
								std::cout << "无法获取WNetCancelConnection2A函数地址" << std::endl;
								FreeLibrary(hMpr);
								return 1;
							}
							DWORD result = NO_ERROR; // 声明一个result变量
							// 循环断开网络驱动器连接
							for (char driveLetter = 'Z'; driveLetter >= 'A'; --driveLetter) {
								std::string drive = std::string(1, driveLetter) + ":";
								result = pWNetCancelConnection2A(drive.c_str(), CONNECT_UPDATE_PROFILE, TRUE);
								if (result == NO_ERROR) {
									std::cout << "网络驱动器 " << drive << " 断开成功！OwO（代码：" << result << "）" << std::endl;
								} else {
									std::cout << "网络驱动器 " << drive << " 断开失败！X_X（代码：" << result << "）" << std::endl;
								}
							}
							// 释放库文件
							FreeLibrary(hMpr);
							//取消禁用倒计时
							EnableWindow(countdownEdit, TRUE);
							EnableWindow(countdownUnitComboBox, TRUE);
							// 删旧倒计时
							DestroyWindow(countdownLabel);
							// 更新倒计时
							countdownLabel = CreateWindow("STATIC", "距断开所有还有：", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 265, 210, 180, 20, hwnd, NULL, NULL, NULL);
							// 显示消息框
							std::string komplettMessage = "已断开部分映射的网络驱动器！￣▽￣\n（代码：" + std::to_string(result) + "）";
							MessageBox(hwnd, komplettMessage.c_str(), "倒计时结束啦！(>ω-*)/", MB_OK | MB_ICONINFORMATION);
						} else if (SendMessage(hBtnNetCommandLine, BM_GETCHECK, 0, 0) == BST_CHECKED) {
							string command = "net use * /del /Y";
							system(command.c_str());
							//取消禁用倒计时
							EnableWindow(countdownEdit, TRUE);
							EnableWindow(countdownUnitComboBox, TRUE);
							// 删旧倒计时
							DestroyWindow(countdownLabel);
							// 更新倒计时
							countdownLabel = CreateWindow("STATIC", "距断开所有还有：", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 265, 210, 180, 20, hwnd, NULL, NULL, NULL);
							// 显示一个消息框，提醒驱动器已断开映射
							MessageBox(hwnd, "已断开全部映射的驱动器！￣▽￣", "倒计时结束啦！(>ω-*)/", MB_OK | MB_ICONINFORMATION);
						}
					}
					// 创建一个字体的文本
					HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("微软雅黑"));
					// 将新创建的字体应用于文本控件
					SendMessage(jieshudaojishicheckbox, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
				}
			}
			break;
		}

		case WM_COMMAND: { // 处理按钮点击事件
			switch (wParam) {

				case 198: { // 显示已占用的盘符
					bUpdateDriveList = TRUE;
					break; // 退出循环，如果没有这个会运行下一行的代码，或者无响应（卡死）
				}

				case 999: { // 显示密码
					if (SendMessage(showPasswordCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断 显示密码 复选框是否被勾选
						// 获取旧密码输入框的内容
						char passwordBuf[256];
						GetWindowText(passwordEdit, passwordBuf, sizeof(passwordBuf));
						// 删除旧的密码字符密码输入框
						DestroyWindow(passwordEdit);
						// 创建新的明文密码输入框
						passwordEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 335, 87, 150, 26, hwnd, NULL, NULL, NULL);
						// 将内容复制到新密码输入框
						SetWindowText(passwordEdit, passwordBuf);
					} else if (SendMessage(showPasswordCheckbox, BM_GETCHECK, 0, 0) == BST_UNCHECKED) {
						// 获取旧密码输入框的内容
						char passwordBuf[256];
						GetWindowText(passwordEdit, passwordBuf, sizeof(passwordBuf));
						// 删除旧的明文密码输入框
						DestroyWindow(passwordEdit);
						// 创建新的密码字符密码输入框
						passwordEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD | ES_AUTOHSCROLL, 335, 87, 150, 26, hwnd, NULL, NULL, NULL);
						// 将内容复制到新密码输入框
						SetWindowText(passwordEdit, passwordBuf);
					}
					// 创建一个字体的文本
					HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("微软雅黑"));
					// 将新创建的字体应用于文本控件
					SendMessage(passwordEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
					break; // 退出循环，如果没有这个会运行下一行的代码，或者无响应（卡死）
				}

				case 998 : { // 映射成功后不禁用显示密码
					if (SendMessage(bujinyongxsmm, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断 映射成功后不禁用显示密码 复选框是否被勾选
						// 勾选
						MessageBox(hwnd, "您勾选了映射成功后不禁用显示密码复选框，当映射成功后\n将不会禁用显示密码复选框，并且密码不会切换为密码字符\n，请映射完后自行清空密码以防您的密码泄露！", "提示(＾＿－)", MB_OK | MB_ICONINFORMATION);
					}
					break;
				}

				case 1: {  // 映射远程共享文件夹
					// 获取控件值
					char usernameBuf[256], passwordBuf[256], ipAddressBuf[256], sharedFolderBuf[256], driveBuf[256];
					GetWindowText(usernameEdit, usernameBuf, sizeof(usernameBuf));
					GetWindowText(passwordEdit, passwordBuf, sizeof(passwordBuf));
					GetWindowText(ipComboBox, ipAddressBuf, sizeof(ipAddressBuf));
					GetWindowText(sharedFolderEdit, sharedFolderBuf, sizeof(sharedFolderBuf));
					GetWindowText(driveComboBox, driveBuf, sizeof(driveBuf));
					username = usernameBuf;
					password = passwordBuf;
					ipAddress = ipAddressBuf;
					sharedFolder = sharedFolderBuf;
					drive = driveBuf;

					// 判断是否有用户名和密码
					if (username.empty() && password.empty()) {
						// 没有用户名和密码，切换到没有用户名和密码的连接方式
						if (SendMessage(hBtnWNetAddConnection2A, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断单选框哪个被选择
							typedef DWORD(WINAPI * LPWNETADDCONNECTION2A)(LPNETRESOURCEA, LPCSTR, LPCSTR, DWORD);
							// 加载库文件
							HMODULE hMpr = LoadLibrary("mpr.dll");
							if (hMpr == NULL) {
								std::cout << "无法加载mpr.dll库文件" << std::endl;
								return 1;
							}
							// 获取函数地址
							LPWNETADDCONNECTION2A pWNetAddConnection2A = (LPWNETADDCONNECTION2A)GetProcAddress(hMpr, "WNetAddConnection2A");
							if (pWNetAddConnection2A == NULL) {
								std::cout << "无法获取WNetAddConnection2A函数地址" << std::endl;
								FreeLibrary(hMpr);
								return 1;
							}
							// 设置要映射的网络驱动器的本地路径和远程路径
							std::string localDrive = drive + ":";
							std::string remotePath = "\\\\" + ipAddress + "\\" + sharedFolder;
							const char* localDrivePtr = localDrive.c_str();
							const char* remotePathPtr = remotePath.c_str();
							// 创建NETRESOURCEA结构体，并初始化
							NETRESOURCEA resource = {
								.dwType = RESOURCETYPE_DISK,
								.lpLocalName = const_cast<char*>(localDrivePtr),
								.lpRemoteName = const_cast<char*>(remotePathPtr)
							};
							// 使用WNetAddConnection2A函数映射网络驱动器
							DWORD result = pWNetAddConnection2A(&resource, nullptr, nullptr, CONNECT_UPDATE_PROFILE);
							// 检查映射结果
							if (result == NO_ERROR) {
								if (countdownMinutes == 0 && countdownDuration == 0) { // 如果变量countdownMinutes（分）和countdownDuration（秒）都等于0时
									if (SendMessage(countdownCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断 映射成功后开启倒计时 复选框是否被勾选
										// 勾选
										// 重置倒计时变量
										countdownMinutes = 0;
										countdownDuration = 0;
										// 设置计时器的间隔时间
										int selectedIndex = SendMessage(countdownUnitComboBox, CB_GETCURSEL, 0, 0);
										// 启动倒计时计时器
										SetTimer(hwnd, 1, 1000, NULL); // 定时器ID: 1，间隔时间为1000毫秒（1秒）
										// 开启倒计时
										char countdownBuf[256];
										GetWindowText(countdownEdit, countdownBuf, sizeof(countdownBuf));
										countdownMinutes = atoi(countdownBuf); // 将字符串转换为整数
										//禁用倒计时
										EnableWindow(countdownEdit, FALSE);
										EnableWindow(countdownUnitComboBox, FALSE);
									}
								}
								if (SendMessage(SaveuserIP, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 复选框被选中
									// 判断当前位置是否在C盘
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C盘中，判断D盘是否存在
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D盘存在，判断文件夹和文件是否存在，如果不存在则创建
											std::string folderPath = "D:\\快捷菜单存储";
											std::string filePath = folderPath + "\\IP地址存储.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "成功创建文件夹" << folderPath << std::endl;
												} else {
													std::cout << "创建文件夹失败" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "成功创建文件" << filePath << std::endl;
												} else {
													std::cout << "创建文件失败" << std::endl;
													return 0;
												}
											}
											// 获取IP地址
											char usernameBuf[256];
											GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// 检查文件中是否存在相同的用户名
											std::ifstream checkFile(filePath);
											std::string line;
											bool usernameExists = false;
											while (std::getline(checkFile, line)) {
												if (line == username) {
													usernameExists = true;
													break;
												}
											}
											checkFile.close();
											// 如果不存在相同的用户名，则存储用户名到文件中
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "成功存储用户名" << std::endl;
												} else {
													std::cout << "存储用户名失败" << std::endl;
												}
											} else {
												std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
											}
										} else {
											std::cout << "没有D盘" << std::endl;
										}
									} else {
										// 不在C盘中，获取当前路径，判断文件夹和文件是否存在，如果不存在则创建
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\IP地址存储.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "成功创建文件夹" << folderPath << std::endl;
											} else {
												std::cout << "创建文件夹失败" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "成功创建文件" << filePath << std::endl;
											} else {
												std::cout << "创建文件失败" << std::endl;
												return 0;
											}
										}
										// 获取IP地址
										char usernameBuf[256];
										GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// 检查文件中是否存在相同的用户名
										std::ifstream checkFile(filePath);
										std::string line;
										bool usernameExists = false;
										while (std::getline(checkFile, line)) {
											if (line == username) {
												usernameExists = true;
												break;
											}
										}
										checkFile.close();
										// 如果不存在相同的用户名，则存储用户名到文件中
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "成功存储用户名" << std::endl;
											} else {
												std::cout << "存储用户名失败" << std::endl;
											}
										} else {
											std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
										}
									}
								}
								if (SendMessage(Saveusershare, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 复选框被选中
									// 判断当前位置是否在C盘
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C盘中，判断D盘是否存在
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D盘存在，判断文件夹和文件是否存在，如果不存在则创建
											std::string folderPath = "D:\\快捷菜单存储";
											std::string filePath = folderPath + "\\共享目录存储.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "成功创建文件夹" << folderPath << std::endl;
												} else {
													std::cout << "创建文件夹失败" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "成功创建文件" << filePath << std::endl;
												} else {
													std::cout << "创建文件失败" << std::endl;
													return 0;
												}
											}
											// 获取共享目录
											char usernameBuf[256];
											GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// 检查文件中是否存在相同的用户名
											std::ifstream checkFile(filePath);
											std::string line;
											bool usernameExists = false;
											while (std::getline(checkFile, line)) {
												if (line == username) {
													usernameExists = true;
													break;
												}
											}
											checkFile.close();
											// 如果不存在相同的用户名，则存储用户名到文件中
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "成功存储用户名" << std::endl;
												} else {
													std::cout << "存储用户名失败" << std::endl;
												}
											} else {
												std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
											}
										} else {
											std::cout << "没有D盘" << std::endl;
										}
									} else {
										// 不在C盘中，获取当前路径，判断文件夹和文件是否存在，如果不存在则创建
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\共享目录存储.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "成功创建文件夹" << folderPath << std::endl;
											} else {
												std::cout << "创建文件夹失败" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "成功创建文件" << filePath << std::endl;
											} else {
												std::cout << "创建文件失败" << std::endl;
												return 0;
											}
										}
										// 获取共享目录
										char usernameBuf[256];
										GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// 检查文件中是否存在相同的用户名
										std::ifstream checkFile(filePath);
										std::string line;
										bool usernameExists = false;
										while (std::getline(checkFile, line)) {
											if (line == username) {
												usernameExists = true;
												break;
											}
										}
										checkFile.close();
										// 如果不存在相同的用户名，则存储用户名到文件中
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "成功存储用户名" << std::endl;
											} else {
												std::cout << "存储用户名失败" << std::endl;
											}
										} else {
											std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
										}
									}
								}
								if (SendMessage(bujinyongxsmm, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 映射成功后不禁止显示密码 复选框是否未被勾选
									EnableWindow(showPasswordCheckbox, FALSE); // 禁用 显示密码 复选框
									SendMessage(showPasswordCheckbox, BM_SETCHECK, BST_UNCHECKED, 0); // 设置 showPasswordCheckbox 复选框的选择状态为未选中
									LONG_PTR style = GetWindowLongPtr(passwordEdit, GWL_STYLE); // 获取输入框的样式
									if (!(style & ES_PASSWORD)) { // 判断输入框样式中是否不包含ES_PASSWORD标志
										/* 如果密码输入框的内容为明文（不包含ES_PASSWORD） */
										// 获取旧密码输入框的内容
										char passwordBuf[256];
										GetWindowText(passwordEdit, passwordBuf, sizeof(passwordBuf));
										// 删除旧的明文密码输入框
										DestroyWindow(passwordEdit);
										// 创建新的密码字符密码输入框
										passwordEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD | ES_AUTOHSCROLL, 335, 87, 150, 26, hwnd, NULL, NULL, NULL);
										// 将内容复制到新密码输入框
										SetWindowText(passwordEdit, passwordBuf);
									}
								}
								std::string successMessage = "网络驱动器映射成功！╰(￣▽￣)╭\n（代码：" + std::to_string(result) + "）";
								MessageBox(hwnd, successMessage.c_str(), "成功啦！(>ω-*)/", MB_OK | MB_ICONINFORMATION);
							} else {
								std::cout << "网络驱动器映射失败！\n（代码：" << result << "）" << std::endl;
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 锁定密码 复选框是否未被勾选
									SetWindowText(passwordEdit, ""); // 将密码输入框的文本设置为空字符串
								}
								std::string errorMessage = "网络驱动器映射失败！(╥╯^╰╥)\n（代码：" + std::to_string(result) + "）";
								MessageBox(hwnd, errorMessage.c_str(), "出错了！╮(╯﹏╰）╭", MB_OK | MB_ICONERROR);
							}
							// 释放库文件
							FreeLibrary(hMpr);
						} else if (SendMessage(hBtnNetCommandLine, BM_GETCHECK, 0, 0) == BST_CHECKED) {
							string command = "net use ";
							command += drive;
							command += ": \\\\";
							command += ipAddress;
							command += "\\" + sharedFolder;
							int result = system(command.c_str());
							if (result == 0) {
								if (countdownMinutes == 0 && countdownDuration == 0) { // 如果变量countdownMinutes（分）和countdownDuration（秒）都等于0时
									if (SendMessage(countdownCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断 映射成功后开启倒计时 复选框是否被勾选
										// 勾选
										// 重置倒计时变量
										countdownMinutes = 0;
										countdownDuration = 0;
										// 设置计时器的间隔时间
										int selectedIndex = SendMessage(countdownUnitComboBox, CB_GETCURSEL, 0, 0);
										// 启动倒计时计时器
										SetTimer(hwnd, 1, 1000, NULL); // 定时器ID: 1，间隔时间为1000毫秒（1秒）
										// 开启倒计时
										char countdownBuf[256];
										GetWindowText(countdownEdit, countdownBuf, sizeof(countdownBuf));
										countdownMinutes = atoi(countdownBuf); // 将字符串转换为整数
										//禁用倒计时
										EnableWindow(countdownEdit, FALSE);
										EnableWindow(countdownUnitComboBox, FALSE);
									}
								}
								if (SendMessage(SaveuserIP, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 复选框被选中
									// 判断当前位置是否在C盘
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C盘中，判断D盘是否存在
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D盘存在，判断文件夹和文件是否存在，如果不存在则创建
											std::string folderPath = "D:\\快捷菜单存储";
											std::string filePath = folderPath + "\\IP地址存储.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "成功创建文件夹" << folderPath << std::endl;
												} else {
													std::cout << "创建文件夹失败" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "成功创建文件" << filePath << std::endl;
												} else {
													std::cout << "创建文件失败" << std::endl;
													return 0;
												}
											}
											// 获取IP地址
											char usernameBuf[256];
											GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// 检查文件中是否存在相同的用户名
											std::ifstream checkFile(filePath);
											std::string line;
											bool usernameExists = false;
											while (std::getline(checkFile, line)) {
												if (line == username) {
													usernameExists = true;
													break;
												}
											}
											checkFile.close();
											// 如果不存在相同的用户名，则存储用户名到文件中
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "成功存储用户名" << std::endl;
												} else {
													std::cout << "存储用户名失败" << std::endl;
												}
											} else {
												std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
											}
										} else {
											std::cout << "没有D盘" << std::endl;
										}
									} else {
										// 不在C盘中，获取当前路径，判断文件夹和文件是否存在，如果不存在则创建
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\IP地址存储.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "成功创建文件夹" << folderPath << std::endl;
											} else {
												std::cout << "创建文件夹失败" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "成功创建文件" << filePath << std::endl;
											} else {
												std::cout << "创建文件失败" << std::endl;
												return 0;
											}
										}
										// 获取IP地址
										char usernameBuf[256];
										GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// 检查文件中是否存在相同的用户名
										std::ifstream checkFile(filePath);
										std::string line;
										bool usernameExists = false;
										while (std::getline(checkFile, line)) {
											if (line == username) {
												usernameExists = true;
												break;
											}
										}
										checkFile.close();
										// 如果不存在相同的用户名，则存储用户名到文件中
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "成功存储用户名" << std::endl;
											} else {
												std::cout << "存储用户名失败" << std::endl;
											}
										} else {
											std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
										}
									}
								}
								if (SendMessage(Saveusershare, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 复选框被选中
									// 判断当前位置是否在C盘
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C盘中，判断D盘是否存在
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D盘存在，判断文件夹和文件是否存在，如果不存在则创建
											std::string folderPath = "D:\\快捷菜单存储";
											std::string filePath = folderPath + "\\共享目录存储.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "成功创建文件夹" << folderPath << std::endl;
												} else {
													std::cout << "创建文件夹失败" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "成功创建文件" << filePath << std::endl;
												} else {
													std::cout << "创建文件失败" << std::endl;
													return 0;
												}
											}
											// 获取共享目录
											char usernameBuf[256];
											GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// 检查文件中是否存在相同的用户名
											std::ifstream checkFile(filePath);
											std::string line;
											bool usernameExists = false;
											while (std::getline(checkFile, line)) {
												if (line == username) {
													usernameExists = true;
													break;
												}
											}
											checkFile.close();
											// 如果不存在相同的用户名，则存储用户名到文件中
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "成功存储用户名" << std::endl;
												} else {
													std::cout << "存储用户名失败" << std::endl;
												}
											} else {
												std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
											}
										} else {
											std::cout << "没有D盘" << std::endl;
										}
									} else {
										// 不在C盘中，获取当前路径，判断文件夹和文件是否存在，如果不存在则创建
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\共享目录存储.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "成功创建文件夹" << folderPath << std::endl;
											} else {
												std::cout << "创建文件夹失败" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "成功创建文件" << filePath << std::endl;
											} else {
												std::cout << "创建文件失败" << std::endl;
												return 0;
											}
										}
										// 获取共享目录
										char usernameBuf[256];
										GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// 检查文件中是否存在相同的用户名
										std::ifstream checkFile(filePath);
										std::string line;
										bool usernameExists = false;
										while (std::getline(checkFile, line)) {
											if (line == username) {
												usernameExists = true;
												break;
											}
										}
										checkFile.close();
										// 如果不存在相同的用户名，则存储用户名到文件中
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "成功存储用户名" << std::endl;
											} else {
												std::cout << "存储用户名失败" << std::endl;
											}
										} else {
											std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
										}
									}
								}
								// 连接成功
								if (SendMessage(zidongxuanze, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 映射成功后不自动选择下一个共享目录和盘符 复选框是否未被勾选
									// 设置"共享目录"的下一个选项为选中状态，若已经是最后一项则返回第一项
									int currentSelection = SendMessage(sharedFolderEdit, CB_GETCURSEL, 0, 0);
									int itemCount = SendMessage(sharedFolderEdit, CB_GETCOUNT, 0, 0);
									int nextSelection = (currentSelection + 1) % itemCount; // 获取下一个选项的索引，若已经是最后一项则返回第一项
									SendMessage(sharedFolderEdit, CB_SETCURSEL, nextSelection, 0);
									if (SendMessage(keyongpancheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断 显示已占用的盘符 复选框是否被勾选
										// 设置"盘符"的下一个选项为选中状态，若已经是最后一项则返回第一项
										currentSelection = SendMessage(driveComboBox, CB_GETCURSEL, 0, 0);
										itemCount = SendMessage(driveComboBox, CB_GETCOUNT, 0, 0);
										nextSelection = (currentSelection + 1) % itemCount; // 获取下一个选项的索引，若已经是最后一项则返回第一项
										SendMessage(driveComboBox, CB_SETCURSEL, nextSelection, 0);
									}
								}
								MessageBox(hwnd, "没有任何问题！qwq", "成功啦！qwq", MB_OK | MB_ICONINFORMATION);
							} else {
								// 连接失败
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 锁定密码 复选框是否未被勾选
									SetWindowText(passwordEdit, ""); // 将密码输入框的文本设置为空字符串
								}
								MessageBox(hwnd, "出了点小问题！X_X", "出错了！X_X", MB_OK | MB_ICONERROR);
							}
						}
					} else if (username.empty()) {
						// 没有用户名，切换到没有用户名的连接方式
						if (SendMessage(hBtnWNetAddConnection2A, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断单选框哪个被选择
							typedef DWORD(WINAPI * LPWNETADDCONNECTION2A)(LPNETRESOURCEA, LPCSTR, LPCSTR, DWORD);
							// 加载库文件
							HMODULE hMpr = LoadLibrary("mpr.dll");
							if (hMpr == NULL) {
								std::cout << "无法加载mpr.dll库文件" << std::endl;
								return 1;
							}
							// 获取函数地址
							LPWNETADDCONNECTION2A pWNetAddConnection2A = (LPWNETADDCONNECTION2A)GetProcAddress(hMpr, "WNetAddConnection2A");
							if (pWNetAddConnection2A == NULL) {
								std::cout << "无法获取WNetAddConnection2A函数地址" << std::endl;
								FreeLibrary(hMpr);
								return 1;
							}
							// 设置要映射的网络驱动器的本地路径和远程路径
							std::string localDrive = drive + ":";
							std::string remotePath = "\\\\" + ipAddress + "\\" + sharedFolder;
							const char* localDrivePtr = localDrive.c_str();
							const char* remotePathPtr = remotePath.c_str();
							// 创建 NETRESOURCEA 结构体，并初始化
							NETRESOURCEA resource = {
								.dwType = RESOURCETYPE_DISK,
								.lpLocalName = const_cast<char*>(localDrivePtr),
								.lpRemoteName = const_cast<char*>(remotePathPtr),
							};
							// 设置要映射的网络驱动器的密码
							std::string passwordStr = password;
							const char* passwordPtr = passwordStr.c_str();
							// 使用WNetAddConnection2A函数映射网络驱动器
							DWORD result = pWNetAddConnection2A(&resource, passwordPtr, NULL, CONNECT_UPDATE_PROFILE);
							// 检查映射结果
							if (result == NO_ERROR) {
								std::cout << "网络驱动器映射成功！\n（代码：" << result << "）" << std::endl;
								if (SendMessage(zidongxuanze, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 映射成功后不自动选择下一个共享目录和盘符 复选框是否未被勾选
									// 设置"共享目录"的下一个选项为选中状态，若已经是最后一项则返回第一项
									int currentSelection = SendMessage(sharedFolderEdit, CB_GETCURSEL, 0, 0);
									int itemCount = SendMessage(sharedFolderEdit, CB_GETCOUNT, 0, 0);
									int nextSelection = (currentSelection + 1) % itemCount; // 获取下一个选项的索引，若已经是最后一项则返回第一项
									SendMessage(sharedFolderEdit, CB_SETCURSEL, nextSelection, 0);
									if (SendMessage(keyongpancheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断 显示已占用的盘符 复选框是否被勾选
										// 设置"盘符"的下一个选项为选中状态，若已经是最后一项则返回第一项
										currentSelection = SendMessage(driveComboBox, CB_GETCURSEL, 0, 0);
										itemCount = SendMessage(driveComboBox, CB_GETCOUNT, 0, 0);
										nextSelection = (currentSelection + 1) % itemCount; // 获取下一个选项的索引，若已经是最后一项则返回第一项
										SendMessage(driveComboBox, CB_SETCURSEL, nextSelection, 0);
									}
								}
								if (countdownMinutes == 0 && countdownDuration == 0) { // 如果变量countdownMinutes（分）和countdownDuration（秒）都等于0时
									if (SendMessage(countdownCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断 映射成功后开启倒计时 复选框是否被勾选
										// 勾选
										// 重置倒计时变量
										countdownMinutes = 0;
										countdownDuration = 0;
										// 设置计时器的间隔时间
										int selectedIndex = SendMessage(countdownUnitComboBox, CB_GETCURSEL, 0, 0);
										// 启动倒计时计时器
										SetTimer(hwnd, 1, 1000, NULL); // 定时器ID: 1，间隔时间为1000毫秒（1秒）
										// 开启倒计时
										char countdownBuf[256];
										GetWindowText(countdownEdit, countdownBuf, sizeof(countdownBuf));
										countdownMinutes = atoi(countdownBuf); // 将字符串转换为整数
										//禁用倒计时
										EnableWindow(countdownEdit, FALSE);
										EnableWindow(countdownUnitComboBox, FALSE);
									}
								}
								if (SendMessage(SaveuserIP, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 复选框被选中
									// 判断当前位置是否在C盘
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C盘中，判断D盘是否存在
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D盘存在，判断文件夹和文件是否存在，如果不存在则创建
											std::string folderPath = "D:\\快捷菜单存储";
											std::string filePath = folderPath + "\\IP地址存储.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "成功创建文件夹" << folderPath << std::endl;
												} else {
													std::cout << "创建文件夹失败" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "成功创建文件" << filePath << std::endl;
												} else {
													std::cout << "创建文件失败" << std::endl;
													return 0;
												}
											}
											// 获取IP地址
											char usernameBuf[256];
											GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// 检查文件中是否存在相同的用户名
											std::ifstream checkFile(filePath);
											std::string line;
											bool usernameExists = false;
											while (std::getline(checkFile, line)) {
												if (line == username) {
													usernameExists = true;
													break;
												}
											}
											checkFile.close();
											// 如果不存在相同的用户名，则存储用户名到文件中
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "成功存储用户名" << std::endl;
												} else {
													std::cout << "存储用户名失败" << std::endl;
												}
											} else {
												std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
											}
										} else {
											std::cout << "没有D盘" << std::endl;
										}
									} else {
										// 不在C盘中，获取当前路径，判断文件夹和文件是否存在，如果不存在则创建
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\IP地址存储.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "成功创建文件夹" << folderPath << std::endl;
											} else {
												std::cout << "创建文件夹失败" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "成功创建文件" << filePath << std::endl;
											} else {
												std::cout << "创建文件失败" << std::endl;
												return 0;
											}
										}
										// 获取IP地址
										char usernameBuf[256];
										GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// 检查文件中是否存在相同的用户名
										std::ifstream checkFile(filePath);
										std::string line;
										bool usernameExists = false;
										while (std::getline(checkFile, line)) {
											if (line == username) {
												usernameExists = true;
												break;
											}
										}
										checkFile.close();
										// 如果不存在相同的用户名，则存储用户名到文件中
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "成功存储用户名" << std::endl;
											} else {
												std::cout << "存储用户名失败" << std::endl;
											}
										} else {
											std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
										}
									}
								}
								if (SendMessage(Saveusershare, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 复选框被选中
									// 判断当前位置是否在C盘
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C盘中，判断D盘是否存在
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D盘存在，判断文件夹和文件是否存在，如果不存在则创建
											std::string folderPath = "D:\\快捷菜单存储";
											std::string filePath = folderPath + "\\共享目录存储.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "成功创建文件夹" << folderPath << std::endl;
												} else {
													std::cout << "创建文件夹失败" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "成功创建文件" << filePath << std::endl;
												} else {
													std::cout << "创建文件失败" << std::endl;
													return 0;
												}
											}
											// 获取共享目录
											char usernameBuf[256];
											GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// 检查文件中是否存在相同的用户名
											std::ifstream checkFile(filePath);
											std::string line;
											bool usernameExists = false;
											while (std::getline(checkFile, line)) {
												if (line == username) {
													usernameExists = true;
													break;
												}
											}
											checkFile.close();
											// 如果不存在相同的用户名，则存储用户名到文件中
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "成功存储用户名" << std::endl;
												} else {
													std::cout << "存储用户名失败" << std::endl;
												}
											} else {
												std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
											}
										} else {
											std::cout << "没有D盘" << std::endl;
										}
									} else {
										// 不在C盘中，获取当前路径，判断文件夹和文件是否存在，如果不存在则创建
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\共享目录存储.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "成功创建文件夹" << folderPath << std::endl;
											} else {
												std::cout << "创建文件夹失败" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "成功创建文件" << filePath << std::endl;
											} else {
												std::cout << "创建文件失败" << std::endl;
												return 0;
											}
										}
										// 获取共享目录
										char usernameBuf[256];
										GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// 检查文件中是否存在相同的用户名
										std::ifstream checkFile(filePath);
										std::string line;
										bool usernameExists = false;
										while (std::getline(checkFile, line)) {
											if (line == username) {
												usernameExists = true;
												break;
											}
										}
										checkFile.close();
										// 如果不存在相同的用户名，则存储用户名到文件中
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "成功存储用户名" << std::endl;
											} else {
												std::cout << "存储用户名失败" << std::endl;
											}
										} else {
											std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
										}
									}
								}
								if (SendMessage(bujinyongxsmm, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 映射成功后不禁止显示密码 复选框是否未被勾选
									EnableWindow(showPasswordCheckbox, FALSE); // 禁用 显示密码 复选框
									SendMessage(showPasswordCheckbox, BM_SETCHECK, BST_UNCHECKED, 0); // 设置 showPasswordCheckbox 复选框的选择状态为未选中
									LONG_PTR style = GetWindowLongPtr(passwordEdit, GWL_STYLE); // 获取输入框的样式
									if (!(style & ES_PASSWORD)) { // 判断输入框样式中是否不包含ES_PASSWORD标志
										/* 如果密码输入框的内容为明文（不包含ES_PASSWORD） */
										// 获取旧密码输入框的内容
										char passwordBuf[256];
										GetWindowText(passwordEdit, passwordBuf, sizeof(passwordBuf));
										// 删除旧的明文密码输入框
										DestroyWindow(passwordEdit);
										// 创建新的密码字符密码输入框
										passwordEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD | ES_AUTOHSCROLL, 335, 87, 150, 26, hwnd, NULL, NULL, NULL);
										// 将内容复制到新密码输入框
										SetWindowText(passwordEdit, passwordBuf);
									}
								}
								// 创建一个字体的文本
								HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("微软雅黑"));
								// 将新创建的字体应用于文本控件
								SendMessage(passwordEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
								std::string successMessage = "网络驱动器映射成功！╰(￣▽￣)╭\n（代码：" + std::to_string(result) + "）";
								MessageBox(hwnd, successMessage.c_str(), "成功啦！(>ω-*)/", MB_OK | MB_ICONINFORMATION);
							} else {
								std::cout << "网络驱动器映射失败！\n（代码：" << result << "）" << std::endl;
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 锁定密码 复选框是否未被勾选
									SetWindowText(passwordEdit, ""); // 将密码输入框的文本设置为空字符串
								}
								std::string errorMessage = "网络驱动器映射失败！(╥╯^╰╥)\n（代码：" + std::to_string(result) + "）";
								MessageBox(hwnd, errorMessage.c_str(), "出错了！╮(╯﹏╰）╭", MB_OK | MB_ICONERROR);
							}
							// 释放库文件
							FreeLibrary(hMpr);
						} else if (SendMessage(hBtnNetCommandLine, BM_GETCHECK, 0, 0) == BST_CHECKED) {
							string command = "net use ";
							command += drive;
							command += ": \\\\";
							command += ipAddress;
							command += "\\" + sharedFolder + " /user:" + password;
							int result = system(command.c_str());
							if (result == 0) {
								if (countdownMinutes == 0 && countdownDuration == 0) { // 如果变量countdownMinutes（分）和countdownDuration（秒）都等于0时
									if (SendMessage(countdownCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断 映射成功后开启倒计时 复选框是否被勾选
										// 勾选
										// 重置倒计时变量
										countdownMinutes = 0;
										countdownDuration = 0;
										// 设置计时器的间隔时间
										int selectedIndex = SendMessage(countdownUnitComboBox, CB_GETCURSEL, 0, 0);
										// 启动倒计时计时器
										SetTimer(hwnd, 1, 1000, NULL); // 定时器ID: 1，间隔时间为1000毫秒（1秒）
										// 开启倒计时
										char countdownBuf[256];
										GetWindowText(countdownEdit, countdownBuf, sizeof(countdownBuf));
										countdownMinutes = atoi(countdownBuf); // 将字符串转换为整数
										//禁用倒计时
										EnableWindow(countdownEdit, FALSE);
										EnableWindow(countdownUnitComboBox, FALSE);
									}
								}
								if (SendMessage(SaveuserIP, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 复选框被选中
									// 判断当前位置是否在C盘
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C盘中，判断D盘是否存在
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D盘存在，判断文件夹和文件是否存在，如果不存在则创建
											std::string folderPath = "D:\\快捷菜单存储";
											std::string filePath = folderPath + "\\IP地址存储.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "成功创建文件夹" << folderPath << std::endl;
												} else {
													std::cout << "创建文件夹失败" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "成功创建文件" << filePath << std::endl;
												} else {
													std::cout << "创建文件失败" << std::endl;
													return 0;
												}
											}
											// 获取IP地址
											char usernameBuf[256];
											GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// 检查文件中是否存在相同的用户名
											std::ifstream checkFile(filePath);
											std::string line;
											bool usernameExists = false;
											while (std::getline(checkFile, line)) {
												if (line == username) {
													usernameExists = true;
													break;
												}
											}
											checkFile.close();
											// 如果不存在相同的用户名，则存储用户名到文件中
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "成功存储用户名" << std::endl;
												} else {
													std::cout << "存储用户名失败" << std::endl;
												}
											} else {
												std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
											}
										} else {
											std::cout << "没有D盘" << std::endl;
										}
									} else {
										// 不在C盘中，获取当前路径，判断文件夹和文件是否存在，如果不存在则创建
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\IP地址存储.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "成功创建文件夹" << folderPath << std::endl;
											} else {
												std::cout << "创建文件夹失败" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "成功创建文件" << filePath << std::endl;
											} else {
												std::cout << "创建文件失败" << std::endl;
												return 0;
											}
										}
										// 获取IP地址
										char usernameBuf[256];
										GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// 检查文件中是否存在相同的用户名
										std::ifstream checkFile(filePath);
										std::string line;
										bool usernameExists = false;
										while (std::getline(checkFile, line)) {
											if (line == username) {
												usernameExists = true;
												break;
											}
										}
										checkFile.close();
										// 如果不存在相同的用户名，则存储用户名到文件中
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "成功存储用户名" << std::endl;
											} else {
												std::cout << "存储用户名失败" << std::endl;
											}
										} else {
											std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
										}
									}
								}
								if (SendMessage(Saveusershare, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 复选框被选中
									// 判断当前位置是否在C盘
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C盘中，判断D盘是否存在
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D盘存在，判断文件夹和文件是否存在，如果不存在则创建
											std::string folderPath = "D:\\快捷菜单存储";
											std::string filePath = folderPath + "\\共享目录存储.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "成功创建文件夹" << folderPath << std::endl;
												} else {
													std::cout << "创建文件夹失败" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "成功创建文件" << filePath << std::endl;
												} else {
													std::cout << "创建文件失败" << std::endl;
													return 0;
												}
											}
											// 获取共享目录
											char usernameBuf[256];
											GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// 检查文件中是否存在相同的用户名
											std::ifstream checkFile(filePath);
											std::string line;
											bool usernameExists = false;
											while (std::getline(checkFile, line)) {
												if (line == username) {
													usernameExists = true;
													break;
												}
											}
											checkFile.close();
											// 如果不存在相同的用户名，则存储用户名到文件中
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "成功存储用户名" << std::endl;
												} else {
													std::cout << "存储用户名失败" << std::endl;
												}
											} else {
												std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
											}
										} else {
											std::cout << "没有D盘" << std::endl;
										}
									} else {
										// 不在C盘中，获取当前路径，判断文件夹和文件是否存在，如果不存在则创建
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\共享目录存储.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "成功创建文件夹" << folderPath << std::endl;
											} else {
												std::cout << "创建文件夹失败" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "成功创建文件" << filePath << std::endl;
											} else {
												std::cout << "创建文件失败" << std::endl;
												return 0;
											}
										}
										// 获取共享目录
										char usernameBuf[256];
										GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// 检查文件中是否存在相同的用户名
										std::ifstream checkFile(filePath);
										std::string line;
										bool usernameExists = false;
										while (std::getline(checkFile, line)) {
											if (line == username) {
												usernameExists = true;
												break;
											}
										}
										checkFile.close();
										// 如果不存在相同的用户名，则存储用户名到文件中
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "成功存储用户名" << std::endl;
											} else {
												std::cout << "存储用户名失败" << std::endl;
											}
										} else {
											std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
										}
									}
								}
								// 连接成功
								if (SendMessage(bujinyongxsmm, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 映射成功后不禁止显示密码 复选框是否未被勾选
									EnableWindow(showPasswordCheckbox, FALSE); // 禁用 显示密码 复选框
									SendMessage(showPasswordCheckbox, BM_SETCHECK, BST_UNCHECKED, 0); // 设置 showPasswordCheckbox 复选框的选择状态为未选中
									LONG_PTR style = GetWindowLongPtr(passwordEdit, GWL_STYLE); // 获取输入框的样式
									if (!(style & ES_PASSWORD)) { // 判断输入框样式中是否不包含ES_PASSWORD标志
										/* 如果密码输入框的内容为明文（不包含ES_PASSWORD） */
										// 获取旧密码输入框的内容
										char passwordBuf[256];
										GetWindowText(passwordEdit, passwordBuf, sizeof(passwordBuf));
										// 删除旧的明文密码输入框
										DestroyWindow(passwordEdit);
										// 创建新的密码字符密码输入框
										passwordEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD | ES_AUTOHSCROLL, 335, 87, 150, 26, hwnd, NULL, NULL, NULL);
										// 将内容复制到新密码输入框
										SetWindowText(passwordEdit, passwordBuf);
									}
								}
								// 创建一个字体的文本
								HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("微软雅黑"));
								// 将新创建的字体应用于文本控件
								SendMessage(passwordEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
								if (SendMessage(zidongxuanze, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 映射成功后不自动选择下一个共享目录和盘符 复选框是否未被勾选
									// 设置"共享目录"的下一个选项为选中状态，若已经是最后一项则返回第一项
									int currentSelection = SendMessage(sharedFolderEdit, CB_GETCURSEL, 0, 0);
									int itemCount = SendMessage(sharedFolderEdit, CB_GETCOUNT, 0, 0);
									int nextSelection = (currentSelection + 1) % itemCount; // 获取下一个选项的索引，若已经是最后一项则返回第一项
									SendMessage(sharedFolderEdit, CB_SETCURSEL, nextSelection, 0);
									if (SendMessage(keyongpancheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断 显示已占用的盘符 复选框是否被勾选
										// 设置"盘符"的下一个选项为选中状态，若已经是最后一项则返回第一项
										currentSelection = SendMessage(driveComboBox, CB_GETCURSEL, 0, 0);
										itemCount = SendMessage(driveComboBox, CB_GETCOUNT, 0, 0);
										nextSelection = (currentSelection + 1) % itemCount; // 获取下一个选项的索引，若已经是最后一项则返回第一项
										SendMessage(driveComboBox, CB_SETCURSEL, nextSelection, 0);
									}
								}
								MessageBox(hwnd, "没有任何问题！qwq", "成功啦！qwq", MB_OK | MB_ICONINFORMATION);
							} else {
								// 连接失败
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 锁定密码 复选框是否未被勾选
									SetWindowText(passwordEdit, ""); // 将密码输入框的文本设置为空字符串
								}
								MessageBox(hwnd, "出了点小问题！X_X", "出错了！X_X", MB_OK | MB_ICONERROR);
							}
						}
					} else if (password.empty()) {
						// 没有密码，切换到没有密码的连接方式
						if (SendMessage(hBtnWNetAddConnection2A, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断单选框哪个被选择
							typedef DWORD(WINAPI * LPWNETADDCONNECTION2A)(LPNETRESOURCEA, LPCSTR, LPCSTR, DWORD);
							// 加载库文件
							HMODULE hMpr = LoadLibrary("mpr.dll");
							if (hMpr == NULL) {
								std::cout << "无法加载mpr.dll库文件" << std::endl;
								return 1;
							}
							// 获取函数地址
							LPWNETADDCONNECTION2A pWNetAddConnection2A = (LPWNETADDCONNECTION2A)GetProcAddress(hMpr, "WNetAddConnection2A");
							if (pWNetAddConnection2A == NULL) {
								std::cout << "无法获取WNetAddConnection2A函数地址" << std::endl;
								FreeLibrary(hMpr);
								return 1;
							}
							// 设置要映射的网络驱动器的本地路径和远程路径
							std::string localDrive = drive + ":";
							std::string remotePath = "\\\\" + ipAddress + "\\" + sharedFolder;
							const char* localDrivePtr = localDrive.c_str();
							const char* remotePathPtr = remotePath.c_str();
							// 创建 NETRESOURCEA 结构体，并初始化
							NETRESOURCEA resource = {
								.dwType = RESOURCETYPE_DISK,
								.lpLocalName = const_cast<char*>(localDrivePtr),
								.lpRemoteName = const_cast<char*>(remotePathPtr),
							};
							// 设置要映射的网络驱动器的用户名
							std::string usernameStr = username;
							const char* usernamePtr = usernameStr.c_str();
							// 使用WNetAddConnection2A函数映射网络驱动器
							DWORD result = pWNetAddConnection2A(&resource, NULL, usernamePtr, CONNECT_UPDATE_PROFILE);
							// 检查映射结果
							if (result == NO_ERROR) {
								std::cout << "网络驱动器映射成功！\n（代码：" << result << "）" << std::endl;
								if (countdownMinutes == 0 && countdownDuration == 0) { // 如果变量countdownMinutes（分）和countdownDuration（秒）都等于0时
									if (SendMessage(countdownCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断 映射成功后开启倒计时 复选框是否被勾选
										// 勾选
										// 重置倒计时变量
										countdownMinutes = 0;
										countdownDuration = 0;
										// 设置计时器的间隔时间
										int selectedIndex = SendMessage(countdownUnitComboBox, CB_GETCURSEL, 0, 0);
										// 启动倒计时计时器
										SetTimer(hwnd, 1, 1000, NULL); // 定时器ID: 1，间隔时间为1000毫秒（1秒）
										// 开启倒计时
										char countdownBuf[256];
										GetWindowText(countdownEdit, countdownBuf, sizeof(countdownBuf));
										countdownMinutes = atoi(countdownBuf); // 将字符串转换为整数
										//禁用倒计时
										EnableWindow(countdownEdit, FALSE);
										EnableWindow(countdownUnitComboBox, FALSE);
									}
								}
								if (SendMessage(Saveusername, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 复选框被选中
									// 判断当前位置是否在C盘
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C盘中，判断D盘是否存在
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D盘存在，判断文件夹和文件是否存在，如果不存在则创建
											std::string folderPath = "D:\\快捷菜单存储";
											std::string filePath = folderPath + "\\用户名存储.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "成功创建文件夹" << folderPath << std::endl;
												} else {
													std::cout << "创建文件夹失败" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "成功创建文件" << filePath << std::endl;
												} else {
													std::cout << "创建文件失败" << std::endl;
													return 0;
												}
											}
											// 获取用户名
											char usernameBuf[256];
											GetWindowText(usernameEdit, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// 检查文件中是否存在相同的用户名
											std::ifstream checkFile(filePath);
											std::string line;
											bool usernameExists = false;
											while (std::getline(checkFile, line)) {
												if (line == username) {
													usernameExists = true;
													break;
												}
											}
											checkFile.close();
											// 如果不存在相同的用户名，则存储用户名到文件中
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "成功存储用户名" << std::endl;
												} else {
													std::cout << "存储用户名失败" << std::endl;
												}
											} else {
												std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
											}
										} else {
											std::cout << "没有D盘" << std::endl;
										}
									} else {
										// 不在C盘中，获取当前路径，判断文件夹和文件是否存在，如果不存在则创建
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\用户名存储.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "成功创建文件夹" << folderPath << std::endl;
											} else {
												std::cout << "创建文件夹失败" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "成功创建文件" << filePath << std::endl;
											} else {
												std::cout << "创建文件失败" << std::endl;
												return 0;
											}
										}
										// 获取用户名
										char usernameBuf[256];
										GetWindowText(usernameEdit, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// 检查文件中是否存在相同的用户名
										std::ifstream checkFile(filePath);
										std::string line;
										bool usernameExists = false;
										while (std::getline(checkFile, line)) {
											if (line == username) {
												usernameExists = true;
												break;
											}
										}
										checkFile.close();
										// 如果不存在相同的用户名，则存储用户名到文件中
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "成功存储用户名" << std::endl;
											} else {
												std::cout << "存储用户名失败" << std::endl;
											}
										} else {
											std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
										}
									}
								}
								if (SendMessage(SaveuserIP, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 复选框被选中
									// 判断当前位置是否在C盘
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C盘中，判断D盘是否存在
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D盘存在，判断文件夹和文件是否存在，如果不存在则创建
											std::string folderPath = "D:\\快捷菜单存储";
											std::string filePath = folderPath + "\\IP地址存储.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "成功创建文件夹" << folderPath << std::endl;
												} else {
													std::cout << "创建文件夹失败" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "成功创建文件" << filePath << std::endl;
												} else {
													std::cout << "创建文件失败" << std::endl;
													return 0;
												}
											}
											// 获取IP地址
											char usernameBuf[256];
											GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// 检查文件中是否存在相同的用户名
											std::ifstream checkFile(filePath);
											std::string line;
											bool usernameExists = false;
											while (std::getline(checkFile, line)) {
												if (line == username) {
													usernameExists = true;
													break;
												}
											}
											checkFile.close();
											// 如果不存在相同的用户名，则存储用户名到文件中
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "成功存储用户名" << std::endl;
												} else {
													std::cout << "存储用户名失败" << std::endl;
												}
											} else {
												std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
											}
										} else {
											std::cout << "没有D盘" << std::endl;
										}
									} else {
										// 不在C盘中，获取当前路径，判断文件夹和文件是否存在，如果不存在则创建
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\IP地址存储.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "成功创建文件夹" << folderPath << std::endl;
											} else {
												std::cout << "创建文件夹失败" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "成功创建文件" << filePath << std::endl;
											} else {
												std::cout << "创建文件失败" << std::endl;
												return 0;
											}
										}
										// 获取IP地址
										char usernameBuf[256];
										GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// 检查文件中是否存在相同的用户名
										std::ifstream checkFile(filePath);
										std::string line;
										bool usernameExists = false;
										while (std::getline(checkFile, line)) {
											if (line == username) {
												usernameExists = true;
												break;
											}
										}
										checkFile.close();
										// 如果不存在相同的用户名，则存储用户名到文件中
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "成功存储用户名" << std::endl;
											} else {
												std::cout << "存储用户名失败" << std::endl;
											}
										} else {
											std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
										}
									}
								}
								if (SendMessage(Saveusershare, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 复选框被选中
									// 判断当前位置是否在C盘
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C盘中，判断D盘是否存在
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D盘存在，判断文件夹和文件是否存在，如果不存在则创建
											std::string folderPath = "D:\\快捷菜单存储";
											std::string filePath = folderPath + "\\共享目录存储.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "成功创建文件夹" << folderPath << std::endl;
												} else {
													std::cout << "创建文件夹失败" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "成功创建文件" << filePath << std::endl;
												} else {
													std::cout << "创建文件失败" << std::endl;
													return 0;
												}
											}
											// 获取共享目录
											char usernameBuf[256];
											GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// 检查文件中是否存在相同的用户名
											std::ifstream checkFile(filePath);
											std::string line;
											bool usernameExists = false;
											while (std::getline(checkFile, line)) {
												if (line == username) {
													usernameExists = true;
													break;
												}
											}
											checkFile.close();
											// 如果不存在相同的用户名，则存储用户名到文件中
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "成功存储用户名" << std::endl;
												} else {
													std::cout << "存储用户名失败" << std::endl;
												}
											} else {
												std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
											}
										} else {
											std::cout << "没有D盘" << std::endl;
										}
									} else {
										// 不在C盘中，获取当前路径，判断文件夹和文件是否存在，如果不存在则创建
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\共享目录存储.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "成功创建文件夹" << folderPath << std::endl;
											} else {
												std::cout << "创建文件夹失败" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "成功创建文件" << filePath << std::endl;
											} else {
												std::cout << "创建文件失败" << std::endl;
												return 0;
											}
										}
										// 获取共享目录
										char usernameBuf[256];
										GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// 检查文件中是否存在相同的用户名
										std::ifstream checkFile(filePath);
										std::string line;
										bool usernameExists = false;
										while (std::getline(checkFile, line)) {
											if (line == username) {
												usernameExists = true;
												break;
											}
										}
										checkFile.close();
										// 如果不存在相同的用户名，则存储用户名到文件中
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "成功存储用户名" << std::endl;
											} else {
												std::cout << "存储用户名失败" << std::endl;
											}
										} else {
											std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
										}
									}
								}
								if (SendMessage(bujinyongxsmm, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 映射成功后不禁止显示密码 复选框是否未被勾选
									EnableWindow(showPasswordCheckbox, FALSE); // 禁用 显示密码 复选框
									SendMessage(showPasswordCheckbox, BM_SETCHECK, BST_UNCHECKED, 0); // 设置 showPasswordCheckbox 复选框的选择状态为未选中
									LONG_PTR style = GetWindowLongPtr(passwordEdit, GWL_STYLE); // 获取输入框的样式
									if (!(style & ES_PASSWORD)) { // 判断输入框样式中是否不包含ES_PASSWORD标志
										/* 如果密码输入框的内容为明文（不包含ES_PASSWORD） */
										// 获取旧密码输入框的内容
										char passwordBuf[256];
										GetWindowText(passwordEdit, passwordBuf, sizeof(passwordBuf));
										// 删除旧的明文密码输入框
										DestroyWindow(passwordEdit);
										// 创建新的密码字符密码输入框
										passwordEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD | ES_AUTOHSCROLL, 335, 87, 150, 26, hwnd, NULL, NULL, NULL);
										// 将内容复制到新密码输入框
										SetWindowText(passwordEdit, passwordBuf);
									}
								}
								std::string successMessage = "网络驱动器映射成功！╰(￣▽￣)╭\n（代码：" + std::to_string(result) + "）";
								MessageBox(hwnd, successMessage.c_str(), "成功啦！(>ω-*)/", MB_OK | MB_ICONINFORMATION);
							} else {
								std::cout << "网络驱动器映射失败！\n（代码：" << result << "）" << std::endl;
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 锁定密码 复选框是否未被勾选
									SetWindowText(passwordEdit, ""); // 将密码输入框的文本设置为空字符串
								}
								std::string errorMessage = "网络驱动器映射失败！(╥╯^╰╥)\n（代码：" + std::to_string(result) + "）";
								MessageBox(hwnd, errorMessage.c_str(), "出错了！╮(╯﹏╰）╭", MB_OK | MB_ICONERROR);
							}
							// 释放库文件
							FreeLibrary(hMpr);
						} else if (SendMessage(hBtnNetCommandLine, BM_GETCHECK, 0, 0) == BST_CHECKED) {
							string command = "net use ";
							command += drive;
							command += ": \\\\";
							command += ipAddress;
							command += "\\" + sharedFolder + " /user:" + username;
							int result = system(command.c_str());
							if (result == 0) {
								if (countdownMinutes == 0 && countdownDuration == 0) { // 如果变量countdownMinutes（分）和countdownDuration（秒）都等于0时
									if (SendMessage(countdownCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断 映射成功后开启倒计时 复选框是否被勾选
										// 勾选
										// 重置倒计时变量
										countdownMinutes = 0;
										countdownDuration = 0;
										// 设置计时器的间隔时间
										int selectedIndex = SendMessage(countdownUnitComboBox, CB_GETCURSEL, 0, 0);
										// 启动倒计时计时器
										SetTimer(hwnd, 1, 1000, NULL); // 定时器ID: 1，间隔时间为1000毫秒（1秒）
										// 开启倒计时
										char countdownBuf[256];
										GetWindowText(countdownEdit, countdownBuf, sizeof(countdownBuf));
										countdownMinutes = atoi(countdownBuf); // 将字符串转换为整数
										//禁用倒计时
										EnableWindow(countdownEdit, FALSE);
										EnableWindow(countdownUnitComboBox, FALSE);
									}
								}
								if (SendMessage(Saveusername, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 复选框被选中
									// 判断当前位置是否在C盘
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C盘中，判断D盘是否存在
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D盘存在，判断文件夹和文件是否存在，如果不存在则创建
											std::string folderPath = "D:\\快捷菜单存储";
											std::string filePath = folderPath + "\\用户名存储.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "成功创建文件夹" << folderPath << std::endl;
												} else {
													std::cout << "创建文件夹失败" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "成功创建文件" << filePath << std::endl;
												} else {
													std::cout << "创建文件失败" << std::endl;
													return 0;
												}
											}
											// 获取用户名
											char usernameBuf[256];
											GetWindowText(usernameEdit, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// 检查文件中是否存在相同的用户名
											std::ifstream checkFile(filePath);
											std::string line;
											bool usernameExists = false;
											while (std::getline(checkFile, line)) {
												if (line == username) {
													usernameExists = true;
													break;
												}
											}
											checkFile.close();
											// 如果不存在相同的用户名，则存储用户名到文件中
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "成功存储用户名" << std::endl;
												} else {
													std::cout << "存储用户名失败" << std::endl;
												}
											} else {
												std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
											}
										} else {
											std::cout << "没有D盘" << std::endl;
										}
									} else {
										// 不在C盘中，获取当前路径，判断文件夹和文件是否存在，如果不存在则创建
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\用户名存储.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "成功创建文件夹" << folderPath << std::endl;
											} else {
												std::cout << "创建文件夹失败" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "成功创建文件" << filePath << std::endl;
											} else {
												std::cout << "创建文件失败" << std::endl;
												return 0;
											}
										}
										// 获取用户名
										char usernameBuf[256];
										GetWindowText(usernameEdit, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// 检查文件中是否存在相同的用户名
										std::ifstream checkFile(filePath);
										std::string line;
										bool usernameExists = false;
										while (std::getline(checkFile, line)) {
											if (line == username) {
												usernameExists = true;
												break;
											}
										}
										checkFile.close();
										// 如果不存在相同的用户名，则存储用户名到文件中
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "成功存储用户名" << std::endl;
											} else {
												std::cout << "存储用户名失败" << std::endl;
											}
										} else {
											std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
										}
									}
								}
								if (SendMessage(SaveuserIP, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 复选框被选中
									// 判断当前位置是否在C盘
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C盘中，判断D盘是否存在
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D盘存在，判断文件夹和文件是否存在，如果不存在则创建
											std::string folderPath = "D:\\快捷菜单存储";
											std::string filePath = folderPath + "\\IP地址存储.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "成功创建文件夹" << folderPath << std::endl;
												} else {
													std::cout << "创建文件夹失败" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "成功创建文件" << filePath << std::endl;
												} else {
													std::cout << "创建文件失败" << std::endl;
													return 0;
												}
											}
											// 获取IP地址
											char usernameBuf[256];
											GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// 检查文件中是否存在相同的用户名
											std::ifstream checkFile(filePath);
											std::string line;
											bool usernameExists = false;
											while (std::getline(checkFile, line)) {
												if (line == username) {
													usernameExists = true;
													break;
												}
											}
											checkFile.close();
											// 如果不存在相同的用户名，则存储用户名到文件中
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "成功存储用户名" << std::endl;
												} else {
													std::cout << "存储用户名失败" << std::endl;
												}
											} else {
												std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
											}
										} else {
											std::cout << "没有D盘" << std::endl;
										}
									} else {
										// 不在C盘中，获取当前路径，判断文件夹和文件是否存在，如果不存在则创建
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\IP地址存储.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "成功创建文件夹" << folderPath << std::endl;
											} else {
												std::cout << "创建文件夹失败" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "成功创建文件" << filePath << std::endl;
											} else {
												std::cout << "创建文件失败" << std::endl;
												return 0;
											}
										}
										// 获取IP地址
										char usernameBuf[256];
										GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// 检查文件中是否存在相同的用户名
										std::ifstream checkFile(filePath);
										std::string line;
										bool usernameExists = false;
										while (std::getline(checkFile, line)) {
											if (line == username) {
												usernameExists = true;
												break;
											}
										}
										checkFile.close();
										// 如果不存在相同的用户名，则存储用户名到文件中
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "成功存储用户名" << std::endl;
											} else {
												std::cout << "存储用户名失败" << std::endl;
											}
										} else {
											std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
										}
									}
								}
								if (SendMessage(Saveusershare, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 复选框被选中
									// 判断当前位置是否在C盘
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C盘中，判断D盘是否存在
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D盘存在，判断文件夹和文件是否存在，如果不存在则创建
											std::string folderPath = "D:\\快捷菜单存储";
											std::string filePath = folderPath + "\\共享目录存储.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "成功创建文件夹" << folderPath << std::endl;
												} else {
													std::cout << "创建文件夹失败" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "成功创建文件" << filePath << std::endl;
												} else {
													std::cout << "创建文件失败" << std::endl;
													return 0;
												}
											}
											// 获取共享目录
											char usernameBuf[256];
											GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// 检查文件中是否存在相同的用户名
											std::ifstream checkFile(filePath);
											std::string line;
											bool usernameExists = false;
											while (std::getline(checkFile, line)) {
												if (line == username) {
													usernameExists = true;
													break;
												}
											}
											checkFile.close();
											// 如果不存在相同的用户名，则存储用户名到文件中
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "成功存储用户名" << std::endl;
												} else {
													std::cout << "存储用户名失败" << std::endl;
												}
											} else {
												std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
											}
										} else {
											std::cout << "没有D盘" << std::endl;
										}
									} else {
										// 不在C盘中，获取当前路径，判断文件夹和文件是否存在，如果不存在则创建
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\共享目录存储.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "成功创建文件夹" << folderPath << std::endl;
											} else {
												std::cout << "创建文件夹失败" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "成功创建文件" << filePath << std::endl;
											} else {
												std::cout << "创建文件失败" << std::endl;
												return 0;
											}
										}
										// 获取共享目录
										char usernameBuf[256];
										GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// 检查文件中是否存在相同的用户名
										std::ifstream checkFile(filePath);
										std::string line;
										bool usernameExists = false;
										while (std::getline(checkFile, line)) {
											if (line == username) {
												usernameExists = true;
												break;
											}
										}
										checkFile.close();
										// 如果不存在相同的用户名，则存储用户名到文件中
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "成功存储用户名" << std::endl;
											} else {
												std::cout << "存储用户名失败" << std::endl;
											}
										} else {
											std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
										}
									}
								}
								// 连接成功
								if (SendMessage(zidongxuanze, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 映射成功后不自动选择下一个共享目录和盘符 复选框是否未被勾选
									// 设置"共享目录"的下一个选项为选中状态，若已经是最后一项则返回第一项
									int currentSelection = SendMessage(sharedFolderEdit, CB_GETCURSEL, 0, 0);
									int itemCount = SendMessage(sharedFolderEdit, CB_GETCOUNT, 0, 0);
									int nextSelection = (currentSelection + 1) % itemCount; // 获取下一个选项的索引，若已经是最后一项则返回第一项
									SendMessage(sharedFolderEdit, CB_SETCURSEL, nextSelection, 0);
									if (SendMessage(keyongpancheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断 显示已占用的盘符 复选框是否被勾选
										// 设置"盘符"的下一个选项为选中状态，若已经是最后一项则返回第一项
										currentSelection = SendMessage(driveComboBox, CB_GETCURSEL, 0, 0);
										itemCount = SendMessage(driveComboBox, CB_GETCOUNT, 0, 0);
										nextSelection = (currentSelection + 1) % itemCount; // 获取下一个选项的索引，若已经是最后一项则返回第一项
										SendMessage(driveComboBox, CB_SETCURSEL, nextSelection, 0);
									}
								}
								MessageBox(hwnd, "没有任何问题！qwq", "成功啦！qwq", MB_OK | MB_ICONINFORMATION);
							} else {
								// 连接失败
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 锁定密码 复选框是否未被勾选
									SetWindowText(passwordEdit, ""); // 将密码输入框的文本设置为空字符串
								}
								MessageBox(hwnd, "出了点小问题！X_X", "出错了！X_X", MB_OK | MB_ICONERROR);
							}
						}
					} else {
						// 有用户名和密码，切换到有用户名和密码的连接方式
						if (SendMessage(hBtnWNetAddConnection2A, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断单选框哪个被选择
							typedef DWORD(WINAPI * LPWNETADDCONNECTION2A)(LPNETRESOURCEA, LPCSTR, LPCSTR, DWORD);
							// 加载库文件
							HMODULE hMpr = LoadLibrary("mpr.dll");
							if (hMpr == NULL) {
								std::cout << "无法加载mpr.dll库文件" << std::endl;
								return 1;
							}
							// 获取函数地址
							LPWNETADDCONNECTION2A pWNetAddConnection2A = (LPWNETADDCONNECTION2A)GetProcAddress(hMpr, "WNetAddConnection2A");
							if (pWNetAddConnection2A == NULL) {
								std::cout << "无法获取WNetAddConnection2A函数地址" << std::endl;
								FreeLibrary(hMpr);
								return 1;
							}
							// 设置要映射的网络驱动器的本地路径和远程路径
							std::string localDrive = drive + ":";
							std::string remotePath = "\\\\" + ipAddress + "\\" + sharedFolder;
							const char* localDrivePtr = localDrive.c_str();
							const char* remotePathPtr = remotePath.c_str();
							// 创建 NETRESOURCEA 结构体，并初始化
							NETRESOURCEA resource = {
								.dwType = RESOURCETYPE_DISK,
								.lpLocalName = const_cast<char*>(localDrivePtr),
								.lpRemoteName = const_cast<char*>(remotePathPtr),
							};
							// 设置要映射的网络驱动器的用户名和密码
							std::string usernameStr = username;
							std::string passwordStr = password;
							const char* usernamePtr = usernameStr.c_str();
							const char* passwordPtr = passwordStr.c_str();
							// 使用WNetAddConnection2A函数映射网络驱动器
							DWORD result = pWNetAddConnection2A(&resource, passwordPtr, usernamePtr, CONNECT_UPDATE_PROFILE);
							// 检查映射结果
							if (result == NO_ERROR) {
								std::cout << "网络驱动器映射成功！\n（代码：" << result << "）" << std::endl;
								if (countdownMinutes == 0 && countdownDuration == 0) { // 如果变量countdownMinutes（分）和countdownDuration（秒）都等于0时
									if (SendMessage(countdownCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断 映射成功后开启倒计时 复选框是否被勾选
										// 勾选
										// 重置倒计时变量
										countdownMinutes = 0;
										countdownDuration = 0;
										// 设置计时器的间隔时间
										int selectedIndex = SendMessage(countdownUnitComboBox, CB_GETCURSEL, 0, 0);
										// 启动倒计时计时器
										SetTimer(hwnd, 1, 1000, NULL); // 定时器ID: 1，间隔时间为1000毫秒（1秒）
										// 开启倒计时
										char countdownBuf[256];
										GetWindowText(countdownEdit, countdownBuf, sizeof(countdownBuf));
										countdownMinutes = atoi(countdownBuf); // 将字符串转换为整数
										//禁用倒计时
										EnableWindow(countdownEdit, FALSE);
										EnableWindow(countdownUnitComboBox, FALSE);
									}
								}
								if (SendMessage(Saveusername, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 复选框被选中
									// 判断当前位置是否在C盘
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C盘中，判断D盘是否存在
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D盘存在，判断文件夹和文件是否存在，如果不存在则创建
											std::string folderPath = "D:\\快捷菜单存储";
											std::string filePath = folderPath + "\\用户名存储.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "成功创建文件夹" << folderPath << std::endl;
												} else {
													std::cout << "创建文件夹失败" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "成功创建文件" << filePath << std::endl;
												} else {
													std::cout << "创建文件失败" << std::endl;
													return 0;
												}
											}
											// 获取用户名
											char usernameBuf[256];
											GetWindowText(usernameEdit, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// 检查文件中是否存在相同的用户名
											std::ifstream checkFile(filePath);
											std::string line;
											bool usernameExists = false;
											while (std::getline(checkFile, line)) {
												if (line == username) {
													usernameExists = true;
													break;
												}
											}
											checkFile.close();
											// 如果不存在相同的用户名，则存储用户名到文件中
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "成功存储用户名" << std::endl;
												} else {
													std::cout << "存储用户名失败" << std::endl;
												}
											} else {
												std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
											}
										} else {
											std::cout << "没有D盘" << std::endl;
										}
									} else {
										// 不在C盘中，获取当前路径，判断文件夹和文件是否存在，如果不存在则创建
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\用户名存储.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "成功创建文件夹" << folderPath << std::endl;
											} else {
												std::cout << "创建文件夹失败" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "成功创建文件" << filePath << std::endl;
											} else {
												std::cout << "创建文件失败" << std::endl;
												return 0;
											}
										}
										// 获取用户名
										char usernameBuf[256];
										GetWindowText(usernameEdit, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// 检查文件中是否存在相同的用户名
										std::ifstream checkFile(filePath);
										std::string line;
										bool usernameExists = false;
										while (std::getline(checkFile, line)) {
											if (line == username) {
												usernameExists = true;
												break;
											}
										}
										checkFile.close();
										// 如果不存在相同的用户名，则存储用户名到文件中
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "成功存储用户名" << std::endl;
											} else {
												std::cout << "存储用户名失败" << std::endl;
											}
										} else {
											std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
										}
									}
								}
								if (SendMessage(SaveuserIP, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 复选框被选中
									// 判断当前位置是否在C盘
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C盘中，判断D盘是否存在
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D盘存在，判断文件夹和文件是否存在，如果不存在则创建
											std::string folderPath = "D:\\快捷菜单存储";
											std::string filePath = folderPath + "\\IP地址存储.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "成功创建文件夹" << folderPath << std::endl;
												} else {
													std::cout << "创建文件夹失败" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "成功创建文件" << filePath << std::endl;
												} else {
													std::cout << "创建文件失败" << std::endl;
													return 0;
												}
											}
											// 获取IP地址
											char usernameBuf[256];
											GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// 检查文件中是否存在相同的用户名
											std::ifstream checkFile(filePath);
											std::string line;
											bool usernameExists = false;
											while (std::getline(checkFile, line)) {
												if (line == username) {
													usernameExists = true;
													break;
												}
											}
											checkFile.close();
											// 如果不存在相同的用户名，则存储用户名到文件中
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "成功存储用户名" << std::endl;
												} else {
													std::cout << "存储用户名失败" << std::endl;
												}
											} else {
												std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
											}
										} else {
											std::cout << "没有D盘" << std::endl;
										}
									} else {
										// 不在C盘中，获取当前路径，判断文件夹和文件是否存在，如果不存在则创建
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\IP地址存储.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "成功创建文件夹" << folderPath << std::endl;
											} else {
												std::cout << "创建文件夹失败" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "成功创建文件" << filePath << std::endl;
											} else {
												std::cout << "创建文件失败" << std::endl;
												return 0;
											}
										}
										// 获取IP地址
										char usernameBuf[256];
										GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// 检查文件中是否存在相同的用户名
										std::ifstream checkFile(filePath);
										std::string line;
										bool usernameExists = false;
										while (std::getline(checkFile, line)) {
											if (line == username) {
												usernameExists = true;
												break;
											}
										}
										checkFile.close();
										// 如果不存在相同的用户名，则存储用户名到文件中
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "成功存储用户名" << std::endl;
											} else {
												std::cout << "存储用户名失败" << std::endl;
											}
										} else {
											std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
										}
									}
								}
								if (SendMessage(Saveusershare, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 复选框被选中
									// 判断当前位置是否在C盘
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C盘中，判断D盘是否存在
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D盘存在，判断文件夹和文件是否存在，如果不存在则创建
											std::string folderPath = "D:\\快捷菜单存储";
											std::string filePath = folderPath + "\\共享目录存储.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "成功创建文件夹" << folderPath << std::endl;
												} else {
													std::cout << "创建文件夹失败" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "成功创建文件" << filePath << std::endl;
												} else {
													std::cout << "创建文件失败" << std::endl;
													return 0;
												}
											}
											// 获取共享目录
											char usernameBuf[256];
											GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// 检查文件中是否存在相同的用户名
											std::ifstream checkFile(filePath);
											std::string line;
											bool usernameExists = false;
											while (std::getline(checkFile, line)) {
												if (line == username) {
													usernameExists = true;
													break;
												}
											}
											checkFile.close();
											// 如果不存在相同的用户名，则存储用户名到文件中
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "成功存储用户名" << std::endl;
												} else {
													std::cout << "存储用户名失败" << std::endl;
												}
											} else {
												std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
											}
										} else {
											std::cout << "没有D盘" << std::endl;
										}
									} else {
										// 不在C盘中，获取当前路径，判断文件夹和文件是否存在，如果不存在则创建
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\共享目录存储.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "成功创建文件夹" << folderPath << std::endl;
											} else {
												std::cout << "创建文件夹失败" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "成功创建文件" << filePath << std::endl;
											} else {
												std::cout << "创建文件失败" << std::endl;
												return 0;
											}
										}
										// 获取共享目录
										char usernameBuf[256];
										GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// 检查文件中是否存在相同的用户名
										std::ifstream checkFile(filePath);
										std::string line;
										bool usernameExists = false;
										while (std::getline(checkFile, line)) {
											if (line == username) {
												usernameExists = true;
												break;
											}
										}
										checkFile.close();
										// 如果不存在相同的用户名，则存储用户名到文件中
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "成功存储用户名" << std::endl;
											} else {
												std::cout << "存储用户名失败" << std::endl;
											}
										} else {
											std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
										}
									}
								}
								if (SendMessage(bujinyongxsmm, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 映射成功后不禁止显示密码 复选框是否未被勾选
									EnableWindow(showPasswordCheckbox, FALSE); // 禁用 显示密码 复选框
									SendMessage(showPasswordCheckbox, BM_SETCHECK, BST_UNCHECKED, 0); // 设置 showPasswordCheckbox 复选框的选择状态为未选中
									LONG_PTR style = GetWindowLongPtr(passwordEdit, GWL_STYLE); // 获取输入框的样式
									if (!(style & ES_PASSWORD)) { // 判断输入框样式中是否不包含ES_PASSWORD标志
										/* 如果密码输入框的内容为明文（不包含ES_PASSWORD） */
										// 获取旧密码输入框的内容
										char passwordBuf[256];
										GetWindowText(passwordEdit, passwordBuf, sizeof(passwordBuf));
										// 删除旧的明文密码输入框
										DestroyWindow(passwordEdit);
										// 创建新的密码字符密码输入框
										passwordEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD | ES_AUTOHSCROLL, 335, 87, 150, 26, hwnd, NULL, NULL, NULL);
										// 将内容复制到新密码输入框
										SetWindowText(passwordEdit, passwordBuf);
									}
								}
								// 创建一个字体的文本
								HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("微软雅黑"));
								// 将新创建的字体应用于文本控件
								SendMessage(passwordEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
								std::string successMessage = "网络驱动器映射成功！╰(￣▽￣)╭\n（代码：" + std::to_string(result) + "）";
								MessageBox(hwnd, successMessage.c_str(), "成功啦！(>ω-*)/", MB_OK | MB_ICONINFORMATION);
							} else {
								std::cout << "网络驱动器映射失败！\n（代码：" << result << "）" << std::endl;
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 锁定密码 复选框是否未被勾选
									SetWindowText(passwordEdit, ""); // 将密码输入框的文本设置为空字符串
								}
								std::string errorMessage = "网络驱动器映射失败！(╥╯^╰╥)\n（代码：" + std::to_string(result) + "）";
								MessageBox(hwnd, errorMessage.c_str(), "出错了！╮(╯﹏╰）╭", MB_OK | MB_ICONERROR);
							}
							// 释放库文件
							FreeLibrary(hMpr);
						} else if (SendMessage(hBtnNetCommandLine, BM_GETCHECK, 0, 0) == BST_CHECKED) {
							string command = "net use ";
							command += drive;
							command += ": \\\\";
							command += ipAddress;
							command += "\\" + sharedFolder + " /user:" + username + " " + password;
							int result = system(command.c_str());
							if (result == 0) {
								if (countdownMinutes == 0 && countdownDuration == 0) { // 如果变量countdownMinutes（分）和countdownDuration（秒）都等于0时
									if (SendMessage(countdownCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断 映射成功后开启倒计时 复选框是否被勾选
										// 勾选
										// 重置倒计时变量
										countdownMinutes = 0;
										countdownDuration = 0;
										// 设置计时器的间隔时间
										int selectedIndex = SendMessage(countdownUnitComboBox, CB_GETCURSEL, 0, 0);
										// 启动倒计时计时器
										SetTimer(hwnd, 1, 1000, NULL); // 定时器ID: 1，间隔时间为1000毫秒（1秒）
										// 开启倒计时
										char countdownBuf[256];
										GetWindowText(countdownEdit, countdownBuf, sizeof(countdownBuf));
										countdownMinutes = atoi(countdownBuf); // 将字符串转换为整数
										//禁用倒计时
										EnableWindow(countdownEdit, FALSE);
										EnableWindow(countdownUnitComboBox, FALSE);
									}
								}
								if (SendMessage(Saveusername, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 复选框被选中
									// 判断当前位置是否在C盘
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C盘中，判断D盘是否存在
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D盘存在，判断文件夹和文件是否存在，如果不存在则创建
											std::string folderPath = "D:\\快捷菜单存储";
											std::string filePath = folderPath + "\\用户名存储.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "成功创建文件夹" << folderPath << std::endl;
												} else {
													std::cout << "创建文件夹失败" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "成功创建文件" << filePath << std::endl;
												} else {
													std::cout << "创建文件失败" << std::endl;
													return 0;
												}
											}
											// 获取用户名
											char usernameBuf[256];
											GetWindowText(usernameEdit, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// 检查文件中是否存在相同的用户名
											std::ifstream checkFile(filePath);
											std::string line;
											bool usernameExists = false;
											while (std::getline(checkFile, line)) {
												if (line == username) {
													usernameExists = true;
													break;
												}
											}
											checkFile.close();
											// 如果不存在相同的用户名，则存储用户名到文件中
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "成功存储用户名" << std::endl;
												} else {
													std::cout << "存储用户名失败" << std::endl;
												}
											} else {
												std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
											}
										} else {
											std::cout << "没有D盘" << std::endl;
										}
									} else {
										// 不在C盘中，获取当前路径，判断文件夹和文件是否存在，如果不存在则创建
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\用户名存储.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "成功创建文件夹" << folderPath << std::endl;
											} else {
												std::cout << "创建文件夹失败" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "成功创建文件" << filePath << std::endl;
											} else {
												std::cout << "创建文件失败" << std::endl;
												return 0;
											}
										}
										// 获取用户名
										char usernameBuf[256];
										GetWindowText(usernameEdit, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// 检查文件中是否存在相同的用户名
										std::ifstream checkFile(filePath);
										std::string line;
										bool usernameExists = false;
										while (std::getline(checkFile, line)) {
											if (line == username) {
												usernameExists = true;
												break;
											}
										}
										checkFile.close();
										// 如果不存在相同的用户名，则存储用户名到文件中
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "成功存储用户名" << std::endl;
											} else {
												std::cout << "存储用户名失败" << std::endl;
											}
										} else {
											std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
										}
									}
								}
								if (SendMessage(SaveuserIP, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 复选框被选中
									// 判断当前位置是否在C盘
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C盘中，判断D盘是否存在
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D盘存在，判断文件夹和文件是否存在，如果不存在则创建
											std::string folderPath = "D:\\快捷菜单存储";
											std::string filePath = folderPath + "\\IP地址存储.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "成功创建文件夹" << folderPath << std::endl;
												} else {
													std::cout << "创建文件夹失败" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "成功创建文件" << filePath << std::endl;
												} else {
													std::cout << "创建文件失败" << std::endl;
													return 0;
												}
											}
											// 获取IP地址
											char usernameBuf[256];
											GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// 检查文件中是否存在相同的用户名
											std::ifstream checkFile(filePath);
											std::string line;
											bool usernameExists = false;
											while (std::getline(checkFile, line)) {
												if (line == username) {
													usernameExists = true;
													break;
												}
											}
											checkFile.close();
											// 如果不存在相同的用户名，则存储用户名到文件中
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "成功存储用户名" << std::endl;
												} else {
													std::cout << "存储用户名失败" << std::endl;
												}
											} else {
												std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
											}
										} else {
											std::cout << "没有D盘" << std::endl;
										}
									} else {
										// 不在C盘中，获取当前路径，判断文件夹和文件是否存在，如果不存在则创建
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\IP地址存储.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "成功创建文件夹" << folderPath << std::endl;
											} else {
												std::cout << "创建文件夹失败" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "成功创建文件" << filePath << std::endl;
											} else {
												std::cout << "创建文件失败" << std::endl;
												return 0;
											}
										}
										// 获取IP地址
										char usernameBuf[256];
										GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// 检查文件中是否存在相同的用户名
										std::ifstream checkFile(filePath);
										std::string line;
										bool usernameExists = false;
										while (std::getline(checkFile, line)) {
											if (line == username) {
												usernameExists = true;
												break;
											}
										}
										checkFile.close();
										// 如果不存在相同的用户名，则存储用户名到文件中
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "成功存储用户名" << std::endl;
											} else {
												std::cout << "存储用户名失败" << std::endl;
											}
										} else {
											std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
										}
									}
								}
								if (SendMessage(Saveusershare, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 复选框被选中
									// 判断当前位置是否在C盘
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C盘中，判断D盘是否存在
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D盘存在，判断文件夹和文件是否存在，如果不存在则创建
											std::string folderPath = "D:\\快捷菜单存储";
											std::string filePath = folderPath + "\\共享目录存储.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "成功创建文件夹" << folderPath << std::endl;
												} else {
													std::cout << "创建文件夹失败" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "成功创建文件" << filePath << std::endl;
												} else {
													std::cout << "创建文件失败" << std::endl;
													return 0;
												}
											}
											// 获取共享目录
											char usernameBuf[256];
											GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// 检查文件中是否存在相同的用户名
											std::ifstream checkFile(filePath);
											std::string line;
											bool usernameExists = false;
											while (std::getline(checkFile, line)) {
												if (line == username) {
													usernameExists = true;
													break;
												}
											}
											checkFile.close();
											// 如果不存在相同的用户名，则存储用户名到文件中
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "成功存储用户名" << std::endl;
												} else {
													std::cout << "存储用户名失败" << std::endl;
												}
											} else {
												std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
											}
										} else {
											std::cout << "没有D盘" << std::endl;
										}
									} else {
										// 不在C盘中，获取当前路径，判断文件夹和文件是否存在，如果不存在则创建
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\共享目录存储.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "成功创建文件夹" << folderPath << std::endl;
											} else {
												std::cout << "创建文件夹失败" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "成功创建文件" << filePath << std::endl;
											} else {
												std::cout << "创建文件失败" << std::endl;
												return 0;
											}
										}
										// 获取共享目录
										char usernameBuf[256];
										GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// 检查文件中是否存在相同的用户名
										std::ifstream checkFile(filePath);
										std::string line;
										bool usernameExists = false;
										while (std::getline(checkFile, line)) {
											if (line == username) {
												usernameExists = true;
												break;
											}
										}
										checkFile.close();
										// 如果不存在相同的用户名，则存储用户名到文件中
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "成功存储用户名" << std::endl;
											} else {
												std::cout << "存储用户名失败" << std::endl;
											}
										} else {
											std::cout << "文件中已存在相同的用户名，不进行存储" << std::endl;
										}
									}
								}
								// 连接成功
								if (SendMessage(bujinyongxsmm, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 映射成功后不禁止显示密码 复选框是否未被勾选
									EnableWindow(showPasswordCheckbox, FALSE); // 禁用 显示密码 复选框
									SendMessage(showPasswordCheckbox, BM_SETCHECK, BST_UNCHECKED, 0); // 设置 showPasswordCheckbox 复选框的选择状态为未选中
									LONG_PTR style = GetWindowLongPtr(passwordEdit, GWL_STYLE); // 获取输入框的样式
									if (!(style & ES_PASSWORD)) { // 判断输入框样式中是否不包含ES_PASSWORD标志
										/* 如果密码输入框的内容为明文（不包含ES_PASSWORD） */
										// 获取旧密码输入框的内容
										char passwordBuf[256];
										GetWindowText(passwordEdit, passwordBuf, sizeof(passwordBuf));
										// 删除旧的明文密码输入框
										DestroyWindow(passwordEdit);
										// 创建新的密码字符密码输入框
										passwordEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD | ES_AUTOHSCROLL, 335, 87, 150, 26, hwnd, NULL, NULL, NULL);
										// 将内容复制到新密码输入框
										SetWindowText(passwordEdit, passwordBuf);
									}
								}
								// 创建一个字体的文本
								HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("微软雅黑"));
								// 将新创建的字体应用于文本控件
								SendMessage(passwordEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
								if (SendMessage(zidongxuanze, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 映射成功后不自动选择下一个共享目录和盘符 复选框是否未被勾选
									// 设置"共享目录"的下一个选项为选中状态，若已经是最后一项则返回第一项
									int currentSelection = SendMessage(sharedFolderEdit, CB_GETCURSEL, 0, 0);
									int itemCount = SendMessage(sharedFolderEdit, CB_GETCOUNT, 0, 0);
									int nextSelection = (currentSelection + 1) % itemCount; // 获取下一个选项的索引，若已经是最后一项则返回第一项
									SendMessage(sharedFolderEdit, CB_SETCURSEL, nextSelection, 0);
									if (SendMessage(keyongpancheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断 显示已占用的盘符 复选框是否被勾选
										// 设置"盘符"的下一个选项为选中状态，若已经是最后一项则返回第一项
										currentSelection = SendMessage(driveComboBox, CB_GETCURSEL, 0, 0);
										itemCount = SendMessage(driveComboBox, CB_GETCOUNT, 0, 0);
										nextSelection = (currentSelection + 1) % itemCount; // 获取下一个选项的索引，若已经是最后一项则返回第一项
										SendMessage(driveComboBox, CB_SETCURSEL, nextSelection, 0);
									}
								}
								MessageBox(hwnd, "没有任何问题！qwq", "成功啦！qwq", MB_OK | MB_ICONINFORMATION);
							} else {
								// 连接失败
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 锁定密码 复选框是否未被勾选
									SetWindowText(passwordEdit, ""); // 将密码输入框的文本设置为空字符串
								}
								MessageBox(hwnd, "出了点小问题！X_X", "出错了！X_X", MB_OK | MB_ICONERROR);
							}
						}
					}
					break;
				}

				case 2: { // 断开全部
					// 断开全部远程共享文件夹
					if (SendMessage(jieshudaojishicheckbox, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 断开后不结束倒计时 复选框是否未被勾选
						// 未勾选
						// 重置倒计时变量
						countdownMinutes = 0;
						countdownDuration = 0;
						// 删旧倒计时
						DestroyWindow(countdownLabel);
						// 更新倒计时
						countdownLabel = CreateWindow("STATIC", "距断开所有还有：", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 265, 210, 180, 20, hwnd, NULL, NULL, NULL);
					}
					if (SendMessage(countdownCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断复选框是否被勾选
						// 勾选
						if (SendMessage(jieshudaojishicheckbox, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 断开后不结束倒计时 复选框是否未被勾选
							// 未勾选
							//取消禁用倒计时
							EnableWindow(countdownLabel, TRUE);
							EnableWindow(countdownEdit, TRUE);
							EnableWindow(countdownUnitComboBox, TRUE);
						}
					} else {
						// 取消勾选
						// 禁用倒计时
						EnableWindow(countdownLabel, FALSE);
						EnableWindow(countdownEdit, FALSE);
						EnableWindow(countdownUnitComboBox, FALSE);
					}
					if (SendMessage(hBtnWNetAddConnection2A, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断单选框哪个被选择
						typedef DWORD(WINAPI * LPWNETCANCELCONNECTION2A)(LPCSTR, DWORD, BOOL);
						// 加载库文件
						HMODULE hMpr = LoadLibrary("mpr.dll");
						if (hMpr == NULL) {
							std::cout << "无法加载mpr.dll库文件" << std::endl;
							return 1;
						}
						// 获取函数地址
						LPWNETCANCELCONNECTION2A pWNetCancelConnection2A = (LPWNETCANCELCONNECTION2A)GetProcAddress(hMpr, "WNetCancelConnection2A");
						if (pWNetCancelConnection2A == NULL) {
							std::cout << "无法获取WNetCancelConnection2A函数地址" << std::endl;
							FreeLibrary(hMpr);
							return 1;
						}
						DWORD result = NO_ERROR; // 声明一个result变量
						// 循环断开网络驱动器连接
						for (char driveLetter = 'Z'; driveLetter >= 'A'; --driveLetter) {
							std::string drive = std::string(1, driveLetter) + ":";
							result = pWNetCancelConnection2A(drive.c_str(), CONNECT_UPDATE_PROFILE, TRUE);
							if (result == NO_ERROR) {
								std::cout << "网络驱动器 " << drive << " 断开成功！OwO（代码：" << result << "）" << std::endl;
							} else {
								std::cout << "网络驱动器 " << drive << " 断开失败！X_X（代码：" << result << "）" << std::endl;
							}
						}
						// 释放库文件
						FreeLibrary(hMpr);
						// 显示消息框
						std::string komplettMessage = "已断开部分映射的网络驱动器！(*￣▽￣*) \n（代码：" + std::to_string(result) + "）";
						// 创建一个字体的文本
						HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("微软雅黑"));
						// 将新创建的字体应用于文本控件
						SendMessage(countdownLabel, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
						MessageBox(hwnd, komplettMessage.c_str(), "完成啦！(>ω-*)/", MB_OK | MB_ICONINFORMATION);
					} else if (SendMessage(hBtnNetCommandLine, BM_GETCHECK, 0, 0) == BST_CHECKED) {
						string command = "net use * /del /Y";
						system(command.c_str());
						// 创建一个字体的文本
						HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("微软雅黑"));
						// 将新创建的字体应用于文本控件
						SendMessage(countdownLabel, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
						MessageBox(hwnd, "已完成操作！qwq", "完成啦！qwq", MB_OK | MB_ICONINFORMATION);
					}
					break;
				}

				case 3: { // 映射成功后开启倒计时
					if (SendMessage(countdownCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断复选框是否被勾选
						// 勾选
						//取消禁用倒计时
						EnableWindow(countdownLabel, TRUE);
						EnableWindow(countdownEdit, TRUE);
						EnableWindow(countdownUnitComboBox, TRUE);
					} else {
						// 取消勾选
						// 重置倒计时变量
						countdownMinutes = 0;
						countdownDuration = 0;
						// 删旧倒计时
						DestroyWindow(countdownLabel);
						// 更新倒计时
						countdownLabel = CreateWindow("STATIC", "距断开所有还有：", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 265, 210, 180, 20, hwnd, NULL, NULL, NULL);
						// 禁用倒计时
						EnableWindow(countdownLabel, FALSE);
						EnableWindow(countdownEdit, FALSE);
						EnableWindow(countdownUnitComboBox, FALSE);
					}
					// 创建一个字体的文本
					HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("微软雅黑"));
					// 将新创建的字体应用于文本控件
					SendMessage(countdownLabel, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
					break;
				}

				case 333 : { // 强制断开全部(重启资源管理器)
					// 关闭资源管理器
					system("taskkill /f /im explorer.exe");
					// 获取 Windows 目录的路径
					TCHAR windowsDir[MAX_PATH];
					if (GetWindowsDirectory(windowsDir, MAX_PATH) > 0) {
						// 构建资源管理器的完整路径
						TCHAR explorerPath[MAX_PATH];
						wsprintf(explorerPath, TEXT("%s\\explorer.exe"), windowsDir);

						// 启动资源管理器
						ShellExecute(NULL, TEXT("open"), explorerPath, NULL, NULL, SW_SHOWNORMAL);
					}
					// 断开全部远程共享文件夹
					if (SendMessage(jieshudaojishicheckbox, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 断开后不结束倒计时 复选框是否未被勾选
						// 未勾选
						// 重置倒计时变量
						countdownMinutes = 0;
						countdownDuration = 0;
						// 删旧倒计时
						DestroyWindow(countdownLabel);
						// 更新倒计时
						countdownLabel = CreateWindow("STATIC", "距断开所有还有：", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 265, 210, 180, 20, hwnd, NULL, NULL, NULL);
					}
					if (SendMessage(countdownCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断复选框是否被勾选
						// 勾选
						if (SendMessage(jieshudaojishicheckbox, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 断开后不结束倒计时 复选框是否未被勾选
							// 未勾选
							//取消禁用倒计时
							EnableWindow(countdownLabel, TRUE);
							EnableWindow(countdownEdit, TRUE);
							EnableWindow(countdownUnitComboBox, TRUE);
						}
					} else {
						// 取消勾选
						// 禁用倒计时
						EnableWindow(countdownLabel, FALSE);
						EnableWindow(countdownEdit, FALSE);
						EnableWindow(countdownUnitComboBox, FALSE);
					}
					if (SendMessage(hBtnWNetAddConnection2A, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断单选框哪个被选择
						typedef DWORD(WINAPI * LPWNETCANCELCONNECTION2A)(LPCSTR, DWORD, BOOL);
						// 加载库文件
						HMODULE hMpr = LoadLibrary("mpr.dll");
						if (hMpr == NULL) {
							std::cout << "无法加载mpr.dll库文件" << std::endl;
							return 1;
						}
						// 获取函数地址
						LPWNETCANCELCONNECTION2A pWNetCancelConnection2A = (LPWNETCANCELCONNECTION2A)GetProcAddress(hMpr, "WNetCancelConnection2A");
						if (pWNetCancelConnection2A == NULL) {
							std::cout << "无法获取WNetCancelConnection2A函数地址" << std::endl;
							FreeLibrary(hMpr);
							return 1;
						}
						DWORD result = NO_ERROR; // 声明一个result变量
						// 循环断开网络驱动器连接
						for (char driveLetter = 'Z'; driveLetter >= 'A'; --driveLetter) {
							std::string drive = std::string(1, driveLetter) + ":";
							result = pWNetCancelConnection2A(drive.c_str(), CONNECT_UPDATE_PROFILE, TRUE);
							if (result == NO_ERROR) {
								std::cout << "网络驱动器 " << drive << " 断开成功！OwO（代码：" << result << "）" << std::endl;
							} else {
								std::cout << "网络驱动器 " << drive << " 断开失败！X_X（代码：" << result << "）" << std::endl;
							}
						}
						// 释放库文件
						FreeLibrary(hMpr);
						// 关闭资源管理器
						system("taskkill /f /im explorer.exe");
						// 获取 Windows 目录的路径
						TCHAR windowsDir[MAX_PATH];
						if (GetWindowsDirectory(windowsDir, MAX_PATH) > 0) {
							// 构建资源管理器的完整路径
							TCHAR explorerPath[MAX_PATH];
							wsprintf(explorerPath, TEXT("%s\\explorer.exe"), windowsDir);

							// 启动资源管理器
							ShellExecute(NULL, TEXT("open"), explorerPath, NULL, NULL, SW_SHOWNORMAL);
						}
						// 显示消息框
						std::string komplettMessage = "已断开部分映射的网络驱动器！(*￣︶￣)\n如还有问题请尝试重启计算机（电脑）！(T^T)\n（代码：" + std::to_string(result) + "）";
						// 创建一个字体的文本
						HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("微软雅黑"));
						// 将新创建的字体应用于文本控件
						SendMessage(countdownLabel, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
						MessageBox(hwnd, komplettMessage.c_str(), "完成啦！OwO", MB_OK | MB_ICONINFORMATION);
					} else if (SendMessage(hBtnNetCommandLine, BM_GETCHECK, 0, 0) == BST_CHECKED) {
						string command = "net use * /del /Y";
						system(command.c_str());
						// 关闭资源管理器
						system("taskkill /f /im explorer.exe");
						// 获取 Windows 目录的路径
						TCHAR windowsDir[MAX_PATH];
						if (GetWindowsDirectory(windowsDir, MAX_PATH) > 0) {
							// 构建资源管理器的完整路径
							TCHAR explorerPath[MAX_PATH];
							wsprintf(explorerPath, TEXT("%s\\explorer.exe"), windowsDir);

							// 启动资源管理器
							ShellExecute(NULL, TEXT("open"), explorerPath, NULL, NULL, SW_SHOWNORMAL);
						}
						// 创建一个字体的文本
						HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("微软雅黑"));
						// 将新创建的字体应用于文本控件
						SendMessage(countdownLabel, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
						MessageBox(hwnd, "已完成操作！qwq\n如果还有问题请尝试重启计算机（电脑）！X_X", "完成啦！qwq", MB_OK | MB_ICONINFORMATION);
					}
					break;
				}

				case 3339: { // 生成专属于我的映射批处理文件
					int result = MessageBox(hwnd, "确认创建bat批处理文件吗？[ · _ · ?]\n确认后请自行选择创建位置和为文件命名\n在创建前请确认映射信息无误哦！\n请注意：此文件有一定泄露账号密码的风\n险，请您最好将文件放在您的个人区域", "询问[ · _ · ?]", MB_OKCANCEL | MB_ICONQUESTION);
					if (result == IDOK) {
						// 获取控件值
						char usernameBuf[256], passwordBuf[256], ipAddressBuf[256], sharedFolderBuf[256], driveBuf[256];
						GetWindowText(usernameEdit, usernameBuf, sizeof(usernameBuf));
						GetWindowText(passwordEdit, passwordBuf, sizeof(passwordBuf));
						GetWindowText(ipComboBox, ipAddressBuf, sizeof(ipAddressBuf));
						GetWindowText(sharedFolderEdit, sharedFolderBuf, sizeof(sharedFolderBuf));
						GetWindowText(driveComboBox, driveBuf, sizeof(driveBuf));
						std::string username = usernameBuf;
						std::string password = passwordBuf;
						std::string ipAddress = ipAddressBuf;
						std::string sharedFolder = sharedFolderBuf;
						std::string drive = driveBuf;
						// 弹出另存为窗口选择另存为文件路径
						OPENFILENAME ofn; // 声明一个OPENFILENAME结构体变量，用于另存为文件对话框的信息
						char szFile[MAX_PATH] = {0}; // 用于另存为选择的文件路径
						strcpy(szFile, ("映射(" + username + " " + sharedFolder + " " + drive + ")").c_str()); // 将用户名和固定字符串连接起来，并将结果复制到szFile中
						ZeroMemory(&ofn, sizeof(ofn)); // 将ofn结构体变量的内存清零，以确保所有成员变量的初始值为0
						ofn.lStructSize = sizeof(ofn); // 设置ofn结构体变量的大小，以便对话框能够正确识别结构体的版本
						ofn.hwndOwner = hwnd; // 设置对话框的父窗口句柄，指定哪个窗口是对话框的所有者
						ofn.lpstrFilter = "Windows批处理文件 (*.bat)\0*.bat\0";  // 设置文件过滤器，限制用户只能选择扩展名为.bat的文件
						ofn.lpstrFile = szFile; // 设置用于另存为选择的文件路径的缓冲区
						ofn.nMaxFile = MAX_PATH; // 设置缓冲区的大小，以确保它能够容纳最大路径长度
						ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;  // 设置对话框的标志，包括是否显示覆盖文件提示、路径必须存在以及隐藏只读文件的选项等
						/*
						以下是常用的对话框标志：
						OFN_ALLOWMULTISELECT ：允许用户选择多个文件
						OFN_CREATEPROMPT ：如果指定的文件不存在，提示用户是否创建该文件
						OFN_EXPLORER ：使用新版的文件选择对话框样式（Windows XP及更高版本）
						OFN_FILEMUSTEXIST ：要求用户选择的文件必须存在
						OFN_HIDEREADONLY ：隐藏只读文件的选项
						OFN_NOCHANGEDIR ：对话框关闭后，不改变当前目录
						OFN_NODEREFERENCELINKS ：不解析快捷方式的目标文件
						OFN_OVERWRITEPROMPT ：如果指定的文件已存在，提示用户是否覆盖该文件
						OFN_PATHMUSTEXIST ：要求用户选择的路径必须存在
						OFN_READONLY ：将选择的文件设置为只读模式
						OFN_SHOWHELP ：显示帮助按钮
						*/
						ofn.lpstrDefExt = "bat"; // 设置默认的文件扩展名为.bat，以便在用户没有输入扩展名时自动添加
						if (GetSaveFileName(&ofn)) {
							// 用户选择了文件路径，您可以使用szFile进行后续操作
							// 创建bat文件
							std::string filePath = ofn.lpstrFile;
							std::ofstream file(filePath.c_str()); // 将std::string转换为const char*
							if (file) {
								// 写入bat文件
								file << "::*此Windows批处理文件由快捷菜单生成" << std::endl;
								if (username.empty() && password.empty()) {
									file << "net use " << drive << ": \\\\" << ipAddress << "\\" << sharedFolder << std::endl;
								} else if (username.empty()) {
									file << "net use " << drive << ": \\\\" << ipAddress << "\\" << sharedFolder << " /user:" << password << std::endl;
								} else if (password.empty()) {
									file << "net use " << drive << ": \\\\" << ipAddress << "\\" << sharedFolder << " /user:" << username << std::endl;
								} else {
									file << "net use " << drive << ": \\\\" << ipAddress << "\\" << sharedFolder << " /user:" << username << " " << password << std::endl;
								}
								file.close();
								MessageBox(hwnd, "bat批处理文件创建成功！", "创建成功(>ω-*)/", MB_OK | MB_ICONINFORMATION);
							} else {
								MessageBox(hwnd, "无法创建bat批处理文件！", "错误(╥╯^╰╥)", MB_OK | MB_ICONERROR);
							}
						}
					}
					break;
				}

				case 4: { // 详细说明
					MessageBox(hwnd, "Q&A问答（Question&Answer）：\nQ：这是什么，有什么用？\nA；这是可以将NAS网络存储器映射到本地网络驱动器（网络驱动器相当于硬盘）的辅助程序，它可以使您更便携地使用您的远程网络存储器设备\nQ；NAS是什么？\nA；NAS（Network Attached Storage：网络附属存储）按字面简单说就是连接在网络上，具备资料存储功能的装置，因此也称为“网络存储器”。它是一种专用数据存储服务器——来源于 百度百科\n\n使用须知：\n保存的用户名将存储在D盘中（如果有），如果没有D盘将存储在此程序所在位置的文件夹中。推荐将此程序连同文件夹放在除C盘外的其他盘中。可创建快捷方式到桌面使用\n\n高级用法：\n可在主窗口勾选\"自动打开存储设备\"复选框后映射，映射成功后将自动打开映射的文件夹\n\n技术与支持：\n使用到的部分映射代码：net use [盘符]: \\[IP]\[共享文件夹]，使用到的部分断开代码：net use * /del /Y，以上代码均为命令行（命令提示符）代码，可在命令行中直接使用。以上提供的代码并非为程序内用到的所有代码，具体可自行上网查询。此映射辅助程序支持多种不同的映射逻辑判断，您如果是管理员则可以无需账户直接进行映射。支持自定义倒计时时间及单位，且您如果在倒计时时点击退出会有提醒。支持隐藏到后台（系统托盘）继续进行倒计时工作（需在主窗口点击退出-选择隐藏到托盘区-然后确定，如果开启了不再提醒可在设置中重置）\n\n常见问题及解决办法：\n在使用的过程中可能会遇到程序无响应的情况，这是正常情况请您耐心等待几秒，如等待时间过长可自行强制关闭程序（任务栏右键-点击启动任务管理器-切换到应用程序选项页-选择此程序-点击结束任务）\n\n常见映射返回值：\n1. ERROR_SUCCESS (0): 操作成功完成。2. ERROR_ALREADY_ASSIGNED (85): 指定的本地设备已经被分配。3. ERROR_BAD_DEVICE (1200): 指定的设备名无效。4. ERROR_BAD_NET_NAME (67): 指定的网络资源名称无效。5. ERROR_BAD_PROVIDER (1204): 指定的提供程序名称无效。6. ERROR_CANCELLED (1223): 操作被用户取消。7. ERROR_EXTENDED_ERROR (1208): 有关详细错误信息，请调用WNetGetLastError函数。8. ERROR_INVALID_PARAMETER (87): 提供的参数无效。9. ERROR_NO_NET_OR_BAD_PATH (1203): 指定的网络提供程序名称无效。10. ERROR_NO_NETWORK (1222): 无法访问网络。11. ERROR_NOT_CONNECTED (2250): 网络连接已经存在。12. ERROR_OPEN_FILES (2401): 有打开的文件或设备。13. ERROR_SESSION_CREDENTIAL_CONFLICT (1219): 会话凭据与现有会话冲突。14. ERROR_BAD_PROFILE (1206): 指定的配置文件无效。15. ERROR_CANNOT_OPEN_PROFILE (1205): 无法打开指定的配置文件。16. ERROR_DEVICE_IN_USE (2404): 设备正在被使用中。17. ERROR_NOT_SUPPORTED (50): 操作不受支持。18. ERROR_INVALID_PASSWORD (86): 指定的网络密码不正确。19. ERROR_REDIR_UNFULLFILLED (2403): 重定向请求未满足。20. ERROR_UNEXP_NET_ERR (59): 意外的网络错误。\n\n我们的用心才能换来您舒服的体验和对我们的支持，最后，感谢您的使用！——开发者：Ling", "详细说明(>ω<)", MB_OK );
					break;
				}

				case 7: { // 下/上一页
					// 判断控件是否隐藏
					LONG style = GetWindowLong(yingsheshezhi, GWL_STYLE);
					if (!(style & WS_VISIBLE)) {
						// 控件已隐藏
						// 显示第一页
						ShowWindow(yingsheshezhi, SW_SHOW);
						ShowWindow(showPasswordCheckbox, SW_SHOW);
						ShowWindow(LockPassword, SW_SHOW);
						ShowWindow(daojishi, SW_SHOW);
						ShowWindow(countdownCheckbox, SW_SHOW);
						ShowWindow(hStaticSetCountdownTime, SW_SHOW);
						ShowWindow(countdownEdit, SW_SHOW);
						ShowWindow(countdownUnitComboBox, SW_SHOW);
						ShowWindow(jieshudaojishicheckbox, SW_SHOW);
						ShowWindow(hStaticMappingKernel, SW_SHOW);
						ShowWindow(hBtnWNetAddConnection2A, SW_SHOW);
						ShowWindow(hBtnNetCommandLine, SW_SHOW);
						ShowWindow(Saveusername, SW_SHOW);
						ShowWindow(keyongpancheckbox, SW_SHOW);
						ShowWindow(zidongxuanze, SW_SHOW);
						ShowWindow(bujinyongxsmm, SW_SHOW);
						// 隐藏第二页
						ShowWindow(SaveuserIP, SW_HIDE);
						ShowWindow(Saveusershare, SW_HIDE);
						ShowWindow(yingsheshezhi2, SW_HIDE);
					} else {
						// 控件未隐藏
						// 隐藏第一页
						ShowWindow(yingsheshezhi, SW_HIDE);
						ShowWindow(showPasswordCheckbox, SW_HIDE);
						ShowWindow(LockPassword, SW_HIDE);
						ShowWindow(daojishi, SW_HIDE);
						ShowWindow(countdownCheckbox, SW_HIDE);
						ShowWindow(hStaticSetCountdownTime, SW_HIDE);
						ShowWindow(countdownEdit, SW_HIDE);
						ShowWindow(countdownUnitComboBox, SW_HIDE);
						ShowWindow(jieshudaojishicheckbox, SW_HIDE);
						ShowWindow(hStaticMappingKernel, SW_HIDE);
						ShowWindow(hBtnWNetAddConnection2A, SW_HIDE);
						ShowWindow(hBtnNetCommandLine, SW_HIDE);
						ShowWindow(Saveusername, SW_HIDE);
						ShowWindow(keyongpancheckbox, SW_HIDE);
						ShowWindow(zidongxuanze, SW_HIDE);
						ShowWindow(bujinyongxsmm, SW_HIDE);
						// 显示第二页
						ShowWindow(SaveuserIP, SW_SHOW);
						ShowWindow(Saveusershare, SW_SHOW);
						ShowWindow(yingsheshezhi2, SW_SHOW);
					}
					break;
				}

				/* 所有其他消息（其中很多）都使用默认过程处理 */
				default: {
					DefWindowProc(hwnd, msg, wParam, lParam);
					break;
				}
			}
			break;
		}

		/* 所有其他消息（很多）都使用默认程序处理 */
		default: {
			DefWindowProc(hwnd, msg, wParam, lParam);
			break;
		}
	}
}

/* 此函数处理 实用工具箱 子窗口的消息 */
LRESULT CALLBACK iToolsWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	// 处理子窗口消息
	switch (msg) {

		// 处理子窗口的关闭消息
		case WM_CLOSE: {
			// 关闭子窗口
			DestroyWindow(hwnd);
			break;
		}

		/* 在销毁时，告诉主线程停止 */
		case WM_DESTROY: { // 销毁时
			isiToolsWindowOpen = 0; // 将变量设为0，表示“工具箱”子窗口已关闭
			break;
		}

		/* 非客户区需要被计算和绘制时发送给窗口的消息 */
		case WM_NCCALCSIZE: {
			// 隐藏边框
			break;
		}

		/* 在窗口上进行绘制操作 */
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			RECT rc;
			GetClientRect(hwnd, &rc);
			// 创建一个白色画刷，并选择到设备上下文中
			HBRUSH hBrush = CreateSolidBrush(RGB(240, 240, 240));
			SelectObject(hdc, hBrush);
			// 绘制菜单栏矩形
			RECT menuBarRect = { rc.left, rc.top, rc.right, rc.top + 32 };
			FillRect(hdc, &menuBarRect, hBrush);
			// 绘制描边
			HPEN hPen = CreatePen(PS_SOLID, 2, RGB(128, 128, 128));
			SelectObject(hdc, hPen);
			MoveToEx(hdc, rc.left, rc.top, NULL);
			LineTo(hdc, rc.right, rc.top);
			LineTo(hdc, rc.right, rc.bottom);
			LineTo(hdc, rc.left, rc.bottom);
			LineTo(hdc, rc.left, rc.top);
			// 绘制“X”按钮
			HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("System"));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(50, 50, 50));
			SetBkMode(hdc, TRANSPARENT);
			TextOut(hdc, rc.right - 24, rc.top + 10, "X", 1);
			// 绘制“-”按钮
			WCHAR minimizeButton[] = { 0x2013, '\0' }; // 使用Unicode码点绘制“-”符号会更长一些
			SetTextColor(hdc, RGB(50, 50, 50));
			SetBkMode(hdc, TRANSPARENT);
			TextOutW(hdc, rc.right - 60, rc.top + 10, minimizeButton, lstrlenW(minimizeButton));
			// 获取窗口图标
			HICON hIcon = (HICON)LoadImage(NULL, "快捷菜单.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
			if (hIcon != NULL) {
				// 计算图标位置
				int iconWidth = 24; // 图标的长
				int iconHeight = 24; // 图标的宽
				int iconX = rc.left + 4;
				int iconY = rc.top + (32 - iconHeight) / 2;
				// 绘制图标
				DrawIconEx(hdc, iconX, iconY, hIcon, iconWidth, iconHeight, 0, NULL, DI_NORMAL);
			}
			// 获取窗口标题
			TCHAR title[256];
			GetWindowText(hwnd, title, 256);
			// 绘制窗口标题
			HFONT hTitleFont = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("微软雅黑"));
			SelectObject(hdc, hTitleFont);
			SetTextColor(hdc, RGB(0, 0, 0));
			SetBkMode(hdc, TRANSPARENT);
			TextOut(hdc, rc.left + 31, rc.top + 7, title, lstrlen(title));
			// 删除画刷、画笔和字体对象
			DeleteObject(hBrush);
			DeleteObject(hPen);
			DeleteObject(hFont);
			DeleteObject(hTitleFont);
			EndPaint(hwnd, &ps);
			break;
		}

		/* 当鼠标在窗口的非客户区移动时，检测鼠标所在位置 */
		case WM_NCHITTEST: {
			POINT pt = { LOWORD(lParam), HIWORD(lParam) };
			ScreenToClient(hwnd, &pt);
			RECT rc;
			GetClientRect(hwnd, &rc);
			// 检测是否点击了“X”按钮
			if (pt.x > rc.right - 37 && pt.x < rc.right - 0 && pt.y > rc.top + 0 && pt.y < rc.top + 32) {
				return HTCLOSE;
			}
			// 检测是否点击了“-”按钮
			if (pt.x > rc.right - 70 && pt.x < rc.right - 35 && pt.y > rc.top + 0 && pt.y < rc.top + 32) {
				return HTMINBUTTON;
			}
			// 检测鼠标所在位置，并返回相应的值
			if (pt.y < rc.top + 32) {
				return HTCAPTION;
			} else if (pt.x < rc.left + 8 && pt.y < rc.top + 8) {
				return HTTOPLEFT;
			} else if (pt.x > rc.right - 8 && pt.y < rc.top + 8) {
				return HTTOPRIGHT;
			} else if (pt.x < rc.left + 8 && pt.y > rc.bottom - 8) {
				return HTBOTTOMLEFT;
			} else if (pt.x > rc.right - 8 && pt.y > rc.bottom - 8) {
				return HTBOTTOMRIGHT;
			} else if (pt.x < rc.left + 8) {
				return HTLEFT;
			} else if (pt.x > rc.right - 8) {
				return HTRIGHT;
			} else if (pt.y < rc.top + 8) {
				return HTTOP;
			} else if (pt.y > rc.bottom - 8) {
				return HTBOTTOM;
			} else {
				return HTCLIENT;
			}
			break;
		}

		/* 当鼠标在窗口的非客户区点击左键时，执行相应操作 */
		case WM_NCLBUTTONUP: {
			if (wParam == HTCLOSE) {
				// 如果点击了“X”按钮，发送关闭窗口的消息
				SendMessage(hwnd, WM_CLOSE, 0, 0);
				return 0;
			} else if (wParam == HTMINBUTTON) {
				// 如果点击了“-”按钮，发送最小化窗口的消息
				SendMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
				return 0;
			}
			break;
		}

		/* 当鼠标在窗口的非客户区按下左键并且在标题栏上，执行以下操作 */
		case WM_NCLBUTTONDOWN: {
			if (wParam == HTCAPTION) {
				SetForegroundWindow(hwnd); // 将窗口设置为前台窗口
				SendMessage(hwnd, WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0); // 发送移动窗口的消息
				return 0;
			}
			break;
		}

		/* 通知窗口的活动状态发生变化 */
		case WM_ACTIVATE: {
			if (wParam == WA_INACTIVE) {
				// 当此程序的焦点丢失时，隐藏标题栏，以防Windows7系统切换到经典主题后出现原标题栏显示的问题
				SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_CAPTION); // 隐藏标题栏
				SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER); // 使改变生效
			}
			break;
		}

		/* 设置静态控件的背景颜色 */
		case WM_CTLCOLORSTATIC: {
			HDC hdcStatic = (HDC)wParam; // 获取静态控件的设备上下文句柄
			SetTextColor(hdcStatic, RGB(0, 0, 0)); // 设置文本颜色为黑色
			SetBkColor(hdcStatic, RGB(255, 255, 255)); // 设置背景颜色为纯白色
			return (INT_PTR)CreateSolidBrush(RGB(255, 255, 255)); // 返回一个纯白色的画刷作为静态控件的背景色
		}

		/* 创建按钮 */
		case WM_CREATE: { // 创建按钮
			if (SendMessage(A_Top, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断 置顶 复选框是否被勾选
				// 勾选
				SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 置顶
			}
			HWND SystemToolsGroupBox = CreateWindow("BUTTON", "系统工具", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 10, 35, 210, 305, hwnd, NULL, NULL, NULL);
			HWND restartButton = CreateWindow("BUTTON", "重启资源管理器和组策略", WS_VISIBLE | WS_CHILD | BS_FLAT, 20, 60, 190, 30, hwnd, (HMENU)1, NULL, NULL);
			HWND RestartResourceManager = CreateWindow("BUTTON", "重启资源管理器", WS_VISIBLE | WS_CHILD | BS_FLAT, 20, 100, 190, 30, hwnd, (HMENU)2, NULL, NULL);
			HWND Systemlevelhiddenfiles = CreateWindow("BUTTON", "系统级隐藏/显示文件", WS_VISIBLE | WS_CHILD | BS_FLAT, 20, 140, 190, 30, hwnd, (HMENU)3, NULL, NULL);
			HWND Systemlevelhiddenfolder = CreateWindow("BUTTON", "系统级隐藏/显示文件夹", WS_VISIBLE | WS_CHILD | BS_FLAT, 20, 180, 190, 30, hwnd, (HMENU)4, NULL, NULL);
			HWND OthertoolsGroupBox = CreateWindow("BUTTON", "其他工具", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 225, 35, 210, 305, hwnd, NULL, NULL, NULL);

			// 创建一个字体的文本
			HFONT hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("微软雅黑"));
			// 将新创建的字体应用于文本控件
			SendMessage(SystemToolsGroupBox, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(restartButton, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(OthertoolsGroupBox, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(RestartResourceManager, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(Systemlevelhiddenfiles, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(Systemlevelhiddenfolder, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			break;
		}

		case WM_COMMAND: { // 处理按钮点击事件
			switch (wParam) {

				case 1: { // 重启组策略和资源管理器
					// 关闭资源管理器
					system("taskkill /f /im explorer.exe");
					// 获取 Windows 目录的路径
					TCHAR windowsDir[MAX_PATH];
					if (GetWindowsDirectory(windowsDir, MAX_PATH) > 0) {
						// 构建资源管理器的完整路径
						TCHAR explorerPath[MAX_PATH];
						wsprintf(explorerPath, TEXT("%s\\explorer.exe"), windowsDir);

						// 启动资源管理器
						ShellExecute(NULL, TEXT("open"), explorerPath, NULL, NULL, SW_SHOWNORMAL);
					}
					//更新组策略
					system("gpupdate /force");

					//提醒用户已完成操作
					MessageBox(hwnd, "已完成操作！OwO", "OK啦！OwO", MB_OK | MB_ICONINFORMATION);
					break;
				}

				case 2: { // 重启资源管理器
					// 关闭资源管理器
					system("taskkill /f /im explorer.exe");
					// 获取 Windows 目录的路径
					TCHAR windowsDir[MAX_PATH];
					if (GetWindowsDirectory(windowsDir, MAX_PATH) > 0) {
						// 构建资源管理器的完整路径
						TCHAR explorerPath[MAX_PATH];
						wsprintf(explorerPath, TEXT("%s\\explorer.exe"), windowsDir);

						// 启动资源管理器
						ShellExecute(NULL, TEXT("open"), explorerPath, NULL, NULL, SW_SHOWNORMAL);
					}

					//提醒用户已完成操作
					MessageBox(hwnd, "已完成操作！OwO", "OK啦！OwO", MB_OK | MB_ICONINFORMATION);
					break;
				}

				case 3: { // 系统级隐藏/显示文件
					// 弹出打开窗口选择打开文件路径
					OPENFILENAME ofn; // 声明一个OPENFILENAME结构体变量，用于打开文件对话框的信息
					char szFile[MAX_PATH] = {0}; // 用于存储用户选择的文件路径
					ZeroMemory(&ofn, sizeof(ofn)); // 将ofn结构体变量的内存清零，以确保所有成员变量的初始值为0
					ofn.lStructSize = sizeof(ofn); // 设置ofn结构体变量的大小，以便对话框能够正确识别结构体的版本
					ofn.hwndOwner = hwnd; // 设置对话框的父窗口句柄，指定哪个窗口是对话框的所有者
					ofn.lpstrFilter = "所有文件 (*.*)\0*.*\0";  // 设置文件过滤器，允许用户选择所有类型的文件
					ofn.lpstrFile = szFile; // 设置用于存储用户选择的文件路径的缓冲区
					ofn.nMaxFile = MAX_PATH; // 设置缓冲区的大小，以确保它能够容纳最大路径长度
					ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_FORCESHOWHIDDEN; // 设置对话框的标志，包括文件必须存在、路径必须存在以及隐藏只读文件的选项等。添加OFN_FORCESHOWHIDDEN标志可以在文件对话框中看到系统级隐藏的文件
					/*
					以下是常用的对话框标志：
					OFN_ALLOWMULTISELECT ：允许用户选择多个文件
					OFN_CREATEPROMPT ：如果指定的文件不存在，提示用户是否创建该文件
					OFN_EXPLORER ：使用新版的文件选择对话框样式（Windows XP及更高版本）
					OFN_FILEMUSTEXIST ：要求用户选择的文件必须存在
					OFN_HIDEREADONLY ：隐藏只读文件的选项
					OFN_NOCHANGEDIR ：对话框关闭后，不改变当前目录
					OFN_NODEREFERENCELINKS ：不解析快捷方式的目标文件
					OFN_OVERWRITEPROMPT ：如果指定的文件已存在，提示用户是否覆盖该文件
					OFN_PATHMUSTEXIST ：要求用户选择的路径必须存在
					OFN_READONLY ：将选择的文件设置为只读模式
					OFN_SHOWHELP ：显示帮助按钮
					*/
					// 获取用户选择的文件路径
					if (GetOpenFileName(&ofn)) {
						// 使用 Windows API 函数 GetFileAttributes() 来获取指定文件的属性
						DWORD attributes = GetFileAttributes(szFile);
						if (attributes != INVALID_FILE_ATTRIBUTES) {
							// 检查文件属性，判断是否是系统级隐藏文件
							if ((attributes & FILE_ATTRIBUTE_SYSTEM) && (attributes & FILE_ATTRIBUTE_HIDDEN)) {
								// 文件是系统级隐藏文件，取消隐藏
								// 用户已经选择了一个文件，可以在这里处理选择的文件路径
								// szFile 变量中存储了用户选择的文件路径
								string command = "attrib -s -h ";
								// 在调用 system 函数执行取消隐藏文件的命令时，将文件路径用引号括起来
								command += "\"";
								command += szFile;
								command += "\"";
								int result = system(command.c_str());
								if (result == 0) {
									// 取消隐藏命令执行成功
									MessageBox(hwnd, "显示命令执行成功！OvO\n如出现未取消隐藏问题请尝试刷新", "成功啦！", MB_OK | MB_ICONINFORMATION);
								} else {
									// 取消隐藏命令执行失败
									MessageBox(hwnd, "显示命令执行失败！QAQ", "失败了呜呜！", MB_OK | MB_ICONERROR);
								}
							} else {
								// 文件不是系统级隐藏文件，隐藏文件
								// 用户已经选择了一个文件，可以在这里处理选择的文件路径
								// szFile 变量中存储了用户选择的文件路径
								string command = "attrib +s +h ";
								// 在调用 system 函数执行隐藏文件的命令时，将文件路径用引号括起来。这样可以确保系统正确识别包含空格的文件路径。这样就会将文件路径用双引号括起来，确保正确处理包含空格的文件路径，使隐藏文件的命令能够正确执行
								command += "\"";
								command += szFile;
								command += "\"";
								int result = system(command.c_str());
								if (result == 0) {
									// 命令执行成功
									MessageBox(hwnd, "隐藏命令执行成功！OvO\n如出现未隐藏问题请尝试刷新", "成功啦！", MB_OK | MB_ICONINFORMATION);
								} else {
									// 命令执行失败
									MessageBox(hwnd, "隐藏命令执行失败！QAQ", "失败了呜呜！", MB_OK | MB_ICONERROR);
								}
							}
						} else {
							// 获取文件属性失败
							MessageBox(hwnd, "获取文件属性失败！", "错误", MB_OK | MB_ICONERROR);
						}
					}
					break;
				}

				case 4: { // 系统级隐藏/显示文件夹
					// 弹出选择文件夹的窗口
					OPENFILENAME ofn; // 声明一个OPENFILENAME结构体变量，用于选择文件夹对话框的信息
					char szDir[MAX_PATH] = {0}; // 用于存储用户选择的文件夹路径
					ZeroMemory(&ofn, sizeof(ofn)); // 将ofn结构体变量的内存清零，以确保所有成员变量的初始值为0
					ofn.lStructSize = sizeof(ofn); // 设置ofn结构体变量的大小，以便对话框能够正确识别结构体的版本
					ofn.hwndOwner = hwnd; // 设置对话框的父窗口句柄，指定哪个窗口是对话框的所有者
					ofn.lpstrTitle = "请选择文件夹（请将文件夹名称或路径复制进文件名，（如果是名称）需要浏览窗口在文件夹内或前一个目录）"; // 设置选择文件夹对话框的标题
					ofn.lpstrFilter = "详细看窗口标题\0*.*\0";  // 设置文件过滤器，允许用户选择所有类型的文件
					ofn.lpstrFile = szDir; // 设置用于存储用户选择的文件夹路径的缓冲区
					ofn.nMaxFile = MAX_PATH; // 设置缓冲区的大小，以确保它能够容纳最大路径长度
					ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_FORCESHOWHIDDEN | OFN_NOCHANGEDIR | OFN_NOVALIDATE | OFN_DONTADDTORECENT; // 设置对话框的标志，包括隐藏只读文件、文件必须存在、路径必须存在、不验证文件名和不将选择的文件夹添加到最近使用文件夹列表等
					/*
					以下是常用的对话框标志：
					OFN_ALLOWMULTISELECT ：允许用户选择多个文件
					OFN_CREATEPROMPT ：如果指定的文件不存在，提示用户是否创建该文件
					OFN_EXPLORER ：使用新版的文件选择对话框样式（Windows XP及更高版本）
					OFN_FILEMUSTEXIST ：要求用户选择的文件必须存在
					OFN_HIDEREADONLY ：隐藏只读文件的选项
					OFN_NOCHANGEDIR ：对话框关闭后，不改变当前目录
					OFN_NODEREFERENCELINKS ：不解析快捷方式的目标文件
					OFN_OVERWRITEPROMPT ：如果指定的文件已存在，提示用户是否覆盖该文件
					OFN_PATHMUSTEXIST ：要求用户选择的路径必须存在
					OFN_READONLY ：将选择的文件设置为只读模式
					OFN_SHOWHELP ：显示帮助按钮
					*/
					// 显示选择文件夹对话框
					if (GetOpenFileName(&ofn)) {
						// 使用 Windows API 函数 GetFileAttributes() 来获取指定文件的属性
						DWORD attributes = GetFileAttributes(szDir);
						if (attributes != INVALID_FILE_ATTRIBUTES) {
							// 检查文件属性，判断是否是系统级隐藏文件
							if ((attributes & FILE_ATTRIBUTE_SYSTEM) && (attributes & FILE_ATTRIBUTE_HIDDEN)) {
								// 文件是系统级隐藏文件，取消隐藏
								// 用户已经选择了一个文件，可以在这里处理选择的文件路径
								// szFile 变量中存储了用户选择的文件路径
								string command = "attrib -s -h ";
								// 在调用 system 函数执行取消隐藏文件的命令时，将文件路径用引号括起来
								command += "\"";
								command += szDir;
								command += "\"";
								int result = system(command.c_str());
								if (result == 0) {
									// 取消隐藏命令执行成功
									MessageBox(hwnd, "显示命令执行成功！OvO\n如出现未取消隐藏问题请尝试刷新\n如需要可隐藏目录下所有文件", "成功啦！", MB_OK | MB_ICONINFORMATION);
								} else {
									// 取消隐藏命令执行失败
									MessageBox(hwnd, "显示命令执行失败！QAQ", "失败了呜呜！", MB_OK | MB_ICONERROR);
								}
							} else {
								// 文件不是系统级隐藏文件，隐藏文件
								// 用户已经选择了一个文件，可以在这里处理选择的文件路径
								// szFile 变量中存储了用户选择的文件路径
								string command = "attrib +s +h ";
								// 在调用 system 函数执行隐藏文件的命令时，将文件路径用引号括起来。这样可以确保系统正确识别包含空格的文件路径。这样就会将文件路径用双引号括起来，确保正确处理包含空格的文件路径，使隐藏文件的命令能够正确执行
								command += "\"";
								command += szDir;
								command += "\"";
								int result = system(command.c_str());
								if (result == 0) {
									// 命令执行成功
									MessageBox(hwnd, "隐藏命令执行成功！OvO\n如出现未隐藏问题请尝试刷新\n如需要可隐藏目录下所有文件", "成功啦！", MB_OK | MB_ICONINFORMATION);
								} else {
									// 命令执行失败
									MessageBox(hwnd, "隐藏命令执行失败！QAQ", "失败了呜呜！", MB_OK | MB_ICONERROR);
								}
							}
						} else {
							// 获取文件属性失败
							MessageBox(hwnd, "获取文件属性失败！", "错误", MB_OK | MB_ICONERROR);
						}
					}
					break;
				}

				/* 所有其他消息（其中很多）都使用默认过程处理 */
				default: {
					DefWindowProc(hwnd, msg, wParam, lParam);
					break;
				}
			}
			break;
		}

		/* 所有其他消息（很多）都使用默认程序处理 */
		default: {
			DefWindowProc(hwnd, msg, wParam, lParam);
			break;
		}
	}
}

/* Win32 GUI程序的“main”函数：这是执行开始的地方 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	/* 创建时检测主窗口是否已存在，多余的销毁并使用最先存在的 */
	// 创建一个互斥体
	HANDLE hMutex = CreateMutex(NULL, TRUE, "MyAppMutex");
	// 检查互斥体是否已经存在
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		// 如果互斥体已经存在，表示已经有一个主窗口进程在运行，因此销毁当前实例
		CloseHandle(hMutex);
		// 在程序退出前释放互斥体
		CloseHandle(hMutex);
		//MessageBox(host_hwnd, "此程序进程已在运行，如任务栏没有，请检查系统托盘", "提示O_O", MB_OK | MB_ICONASTERISK); // 因为是新建窗口不能发送消息到旧窗口所以只能提醒用户自行查看 // 技术得到解决，现已解决
		return 0;
	}

	WNDCLASSEX wc; // 窗口的属性结构
	HWND hwnd; // 一个“HANDLE”，因此是H，或者是指向我们的窗口的指针
	MSG msg; // 所有消息的临时位置

	/* 将结构体清零并设置我们想要修改的内容 */
	memset(&wc, 0, sizeof(wc));
	wc.cbSize         = sizeof(WNDCLASSEX);
	wc.lpfnWndProc    = WndProc; // 这是我们将要发送消息的地方
	wc.hInstance      = hInstance;
	wc.hCursor        = LoadCursor(NULL, IDC_ARROW);

	/* 白色，COLOR_WINDOW只是一个系统颜色的#define，尝试按Ctrl+单击它 */
	wc.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1); // 0为应用程序界面（客户区）默认颜色（纯白偏一点点灰）;1为纯白色（此程序的此参数默认为1）
	wc.lpszClassName  = "WindowClass";
	wc.hIcon          = LoadIcon(NULL, IDC_ICON); // 加载标准图标
	wc.hIconSm        = LoadIcon(NULL, IDC_ICON); //  使用名称“A”以使用项目图标
	HANDLE hIcon1 = LoadImage(NULL, "快捷菜单.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
	if (hIcon1) {
		// 加载成功，将 hIcon1 赋值给窗口类结构的 hIcon 和 hIconSm 成员
		wc.hIcon = (HICON)hIcon1;
		wc.hIconSm = (HICON)hIcon1;
	} else {
		// 加载失败
		MessageBox(NULL, "图标加载失败！X_X", "错误X_X", MB_ICONERROR | MB_OK);
	}

	// 注册主窗口的类
	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "窗口注册失败！", "错误", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// 注册 退出 子窗口的类
	wc.lpfnWndProc   = exitWindowProc;
	wc.lpszClassName = "ExitWindowClass";
	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "窗口注册失败！", "错误", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// 注册 设置 子窗口的类
	wc.lpfnWndProc = SettingsWndProc;
	wc.lpszClassName = "SettingsWindowClass";
	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "子窗口注册失败！", "错误", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// 注册 常用窗口快捷打开 子窗口的类
	wc.lpfnWndProc	 = ChildWindowProc;
	wc.lpszClassName = "ChildWindowClass";
	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "子窗口注册失败！", "错误", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// 注册 网络映射辅助工具 子窗口的类
	wc.lpfnWndProc   = ChildWndProc;
	wc.lpszClassName = childwindow;
	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "子窗口注册失败！", "错误", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// 注册 实用工具箱 子窗口的类
	wc.lpfnWndProc	 = iToolsWindowProc;
	wc.lpszClassName = "iToolsWindowClass";
	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "子窗口注册失败！", "错误", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	/* 使窗口始终出现在屏幕中间 */
	// 获取屏幕宽度和高度
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	// 计算窗口左上角坐标
	int windowWidth = 420; // 窗口宽度
	int windowHeight = 320; // 窗口高度
	int windowX = (screenWidth - windowWidth) / 2;
	int windowY = (screenHeight - windowHeight) / 2;

	host_hwnd = CreateWindowEx(WS_EX_APPWINDOW, "WindowClass", "快捷菜单(〃'▽'〃)", WS_VISIBLE | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, windowX, windowY, windowWidth, windowHeight, NULL, NULL, hInstance, NULL);

	if (host_hwnd == NULL) {
		MessageBox(NULL, "窗口创建失败！", "错误", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// 初始化托盘图标
	memset(&nid, 0, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.hWnd = host_hwnd;
	nid.uID = 1;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_USER + 1;
	HANDLE hIcon2 = LoadImage(NULL, "快捷菜单.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
	if (hIcon2) {
		// 加载成功，将 hIcon2 赋值给托盘图标的 hIcon 成员
		nid.hIcon = (HICON)hIcon2;
	} else {
		// 加载失败，使用默认图标
		HICON hIcon2 = LoadIcon(NULL, IDI_APPLICATION);
		nid.hIcon = (HICON)hIcon2;
	}
	strcpy(nid.szTip, "快捷菜单");
	// 添加托盘图标
	Shell_NotifyIcon(NIM_ADD, &nid);

	/*
	    这是我们程序的核心，所有输入都会被处理并发送到WndProc。请注意，
	    GetMessage会阻塞代码流，直到它接收到消息，因此此循环不会产生不合理的高CPU使用率
	*/
	while (GetMessage(&msg, NULL, 0, 0) > 0) { // 如果没有收到错误...
		TranslateMessage(&msg); // 如果有，将键码转换为字符
		DispatchMessage(&msg); // 将其发送到WndProc
	}
	return msg.wParam;
}
