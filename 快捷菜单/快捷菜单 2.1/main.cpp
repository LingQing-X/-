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
using namespace std;

// 声明全局变量
const char childwindow[] = "Drive mapping"; // 网络映射辅助工具 子窗口的全局变量
int isExitWindowOpen = 0; // 用于使“退出”子窗口只能打开一次
int isSettingsWindowOpen = 0; // 用于使“设置”子窗口只能打开一次
int isWebsiteWindowOpen = 0; // 用于使“网站”子窗口只能打开一次
int isMappingWindowOpen = 0; // 用于使“映射”子窗口只能打开一次
int isiToolsWindowOpen = 0; // 用于使“工具箱”子窗口只能打开一次

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
HWND htext; // 文本
HWND LockPassword; // 锁定密码 复选框
HWND Saveusername; // 映射成功后保存用户名 复选框
HWND notice; // 须知 文本
HWND ForcedisconnectallButton; // 强制断开全部(重启资源管理器) 按钮
// 控件值
string username; // 账号
string password; // 密码
string ipAddress; // IP地址
string sharedFolder; // 共享目录
string drive; // 盘符
// 倒计时持续时间的全局变量
int countdownMinutes = 0; // 分
int countdownDuration = 0; // 秒
// 布尔变量
bool hasContentExecuted = false;
bool noContentExecuted = false;

/* 这是所有窗口输入的地方 */
HWND A_Top, A_AOU, A_text, A_text2, A_1, A_2, A_3, A_strongTop;
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
				lstrcpy(nid.szInfo, TEXT("我在这哦！qwq")); // 设置消息内容
				Shell_NotifyIcon(NIM_MODIFY, &nid); // 发送消息
			} else  if (found2) {
				// 包含"...2"的处理逻辑
				// 退出程序
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
					hwnd_exit = CreateWindowEx(WS_EX_CLIENTEDGE, "ExitWindowClass", "退出前询问U_U", WS_SYSMENU, windowX, windowY, windowWidth, windowHeight, NULL, NULL, GetModuleHandle(NULL), NULL);
					ShowWindow(hwnd_exit, SW_SHOW);
					isExitWindowOpen = 1; // 将变量设为1，表示“退出”子窗口已打开
				} else {
					// 置顶后取消置顶窗口提醒用户
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

		/* 创建按钮 */
		case WM_CREATE: { // 创建按钮
			A_Top = CreateWindow("button", "置顶此程序的所有窗口", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 48, 5, 170, 15, hwnd, (HMENU)00000, hInst, NULL);
			A_AOU = CreateWindow("button", "自动打开新增的存储设备", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 40, 25, 185, 15, hwnd, (HMENU)857, hInst, NULL); // 自动打开新增的存储设备 = Auto Open Inserted USB
			A_text = CreateWindow("Static", "欢迎使用！", WS_CHILD | WS_VISIBLE, 0, 195, 370, 50, hwnd, NULL, hInst, NULL);
			A_text2 = CreateWindow("Static", "欢迎使用！", WS_CHILD | WS_VISIBLE, 0, 228, 370, 20, hwnd, NULL, hInst, NULL);
			A_1 = CreateWindow("button", "常用网站快捷打开", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 65, 45, 140, 45, hwnd, (HMENU)1, hInst, NULL);
			A_2 = CreateWindow("button", "网络映射辅助工具", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 65, 95, 140, 45, hwnd, (HMENU)2, hInst, NULL);
			A_3 = CreateWindow("button", "实用工具箱", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 65, 145, 140, 45, hwnd, (HMENU)3, hInst, NULL);
			// 启动计时器
			SetTimer(hwnd, 1, 500, NULL); // 定时器ID: 1，间隔时间: 500毫秒（0.5秒）
			SetTimer(hwnd, 2, 10000, NULL); // 定时器ID: 2，间隔时间: 10000毫秒（10秒）
			// 随机选择的文本
			std::string motivationalTexts[] = {
				"人生没有白走的路，每一步都算",
				"花有重开日，人无再少年",
				"失败是成长的机会，勇敢面对",
				"每一次的尝试都是一种进步",
				"不要害怕失败，它是成功的一部分",
				"坚持不懈，你终会收获成果",
				"知足常乐，终身不辱",
				"手就这么大，握不住的太多了",
				"成功需要付出努力和耐心",
				"只有你能决定自己的未来",
				"祸从口出，病从口入",
				"成功的关键在于坚持不懈",
				"成事莫说，覆水难收",
				"勇敢面对挑战，你会变得更强大",
				"每一次的努力都是值得的",
				"会当凌绝顶，一览众山小",
				"不要害怕改变，它是成长的机会",
				"积极思考，你会看到更多的可能性",
				"忍一句，息一怒，绕一着，退一步",
				"你的努力一定会为你带来回报"
			};
			std::srand(static_cast<unsigned int>(std::time(nullptr))); // 设置随机数种子
			int randomIndex = std::rand() % (sizeof(motivationalTexts) / sizeof(motivationalTexts[0]));
			// 将所有文本合并为一个字符串
			std::string usageStr = "欢迎使用！版本：2.1\n" + motivationalTexts[randomIndex];
			// 更新欢迎文本显示的文本
			SetWindowText(A_text, usageStr.c_str());
			break;
		}

		/* 计时器 */
		case WM_TIMER: {
			if (wParam == 2) { // 判断计时器ID
				// 随机选择的文本
				std::string motivationalTexts[] = {
					"人生没有白走的路，每一步都算",
					"花有重开日，人无再少年",
					"失败是成长的机会，勇敢面对",
					"每一次的尝试都是一种进步",
					"不要害怕失败，它是成功的一部分",
					"坚持不懈，你终会收获成果",
					"知足常乐，终身不辱",
					"手就这么大，握不住的太多了",
					"成功需要付出努力和耐心",
					"只有你能决定自己的未来",
					"祸从口出，病从口入",
					"成功的关键在于坚持不懈",
					"成事莫说，覆水难收",
					"勇敢面对挑战，你会变得更强大",
					"每一次的努力都是值得的",
					"会当凌绝顶，一览众山小",
					"不要害怕改变，它是成长的机会",
					"积极思考，你会看到更多的可能性",
					"忍一句，息一怒，绕一着，退一步",
					"你的努力一定会为你带来回报"
				};
				std::srand(static_cast<unsigned int>(std::time(nullptr))); // 设置随机数种子
				int randomIndex = std::rand() % (sizeof(motivationalTexts) / sizeof(motivationalTexts[0]));

				// 将所有文本合并为一个字符串
				std::string usageStr = "欢迎使用！版本：2.1\n" + motivationalTexts[randomIndex];

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

				// 将所有文本合并为一个字符串
				std::string usageStr = "CPU占用：" + std::to_string(static_cast<int>(cpuUsage)) + "%" + "     内存占用：" + std::to_string(static_cast<int>(memoryUsagePercentage)) + "%";

				// 更新欢迎2文本显示的文本
				SetWindowText(A_text2, usageStr.c_str());
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

				case 32912: { // 设置
					if (isSettingsWindowOpen == 0) { // 检查是否已经打开了“设置”子窗口
						/* 使窗口始终出现在屏幕中间 */
						// 获取屏幕宽度和高度
						int screenWidth = GetSystemMetrics(SM_CXSCREEN);
						int screenHeight = GetSystemMetrics(SM_CYSCREEN);
						// 计算窗口左上角坐标
						int windowWidth = 300; // 窗口宽度
						int windowHeight = 325; // 窗口高度
						int windowX = (screenWidth - windowWidth) / 2;
						int windowY = (screenHeight - windowHeight) / 2;

						// 按钮被点击时，创建 常用网站快捷打开 子窗口
						hwnd_Settings = CreateWindowEx(WS_EX_CLIENTEDGE, "SettingsWindowClass", "设置", WS_SYSMENU | WS_MINIMIZEBOX, windowX, windowY, windowWidth, windowHeight, NULL, NULL, GetModuleHandle(NULL), NULL);
						ShowWindow(hwnd_Settings, SW_SHOW);
						isSettingsWindowOpen = 1; // 将变量设为1，表示“映射”子窗口已打开
					} else {
						MessageBox(hwnd, "窗口已存在！=-=", "提示=-=", MB_OK | MB_ICONASTERISK);
						// 置顶后取消置顶窗口提醒用户
						SetWindowPos(hwnd_Settings, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
						SetWindowPos(hwnd_Settings, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
					}
					break;
				}

				case 32913: { // 详细
					MessageBox(hwnd, "常见问题及解决办法：\n在使用的过程中可能会遇到程序无响应的情况，这是正常情况请您耐心等待几秒，如等待时间过长可自行强制关闭程序（任务栏右键-点击启动任务管理器-切换到应用程序选项页-选择此程序-点击结束任务）\n\n关于您宝贵的意见和反馈：\n我们非常愿意采纳您宝贵的意见和反馈并应用到下一个版本中，但是对于比较有难度的功能会开发缓慢，请您谅解。我们会尽量采纳大部分有用处的意见，并且欢迎您在程序使用中发现BUG（程序漏洞）时反馈给我们以便在下一个版本中修复它。对于底层的BUG或者牵连其他代码的的BUG我们通常需要更久的时间来进行修复，请您谅解！\n\n联系我们与获取新版本：\n联系我们请加QQ群：938213028。可在群内获取最新版本程序，下一版本更新内容或信息交流等\n\n程序的开发与源代码：\n此程序由小熊猫Dev-C++编译器使用C++语言开发。程序代码暂不开源，如有学习需要我们可为您提供设计思路\n\n使用说明/免责声明：\n我们不会收集您的任何隐私信息，更不会将信息上传至云端。如果您使用此程序造成了任何的后果，均由您本人承担！\n\n我们的用心才能换来您舒服的体验和对我们的支持，最后，感谢您的使用！——开发者：Ling", "详细^u^", MB_OK );
					break;
				}

				case 32914: { // 更新内容
					MessageBox(hwnd, "——2.0及之前版本——\n2.0版本及之前版本更新内容省略。2.0版本在2023年7月5日正式开始开发，到2023年9月27日完成开发。2126行代码，共89515个字符\n——2.1版本——\n2.1版本在2023年9月28日正式开始开发，到2023年10月12日完成开发。3194行代码，共142734个字符\n更新内容：1.优化了部分窗口的布局。2.更新了主窗口、设置、网络映射辅助工具、实用工具箱和一些消息框。3.因为局域网发送消息不可用了所以就删除了。4.修复了已知的BUG\n———————\n\n我们的用心才能换来您舒服的体验和对我们的支持，最后，感谢您的使用！——开发者：Ling", "更新内容^u^", MB_OK );
					break;
				}

				case 1: { // 常用网站快捷打开
					if (isWebsiteWindowOpen == 0) { // 检查是否已经打开了“网站”子窗口
						/* 使窗口始终出现在屏幕中间 */
						// 获取屏幕宽度和高度
						int screenWidth = GetSystemMetrics(SM_CXSCREEN);
						int screenHeight = GetSystemMetrics(SM_CYSCREEN);
						// 计算窗口左上角坐标
						int windowWidth = 520; // 窗口宽度
						int windowHeight = 365; // 窗口高度
						int windowX = (screenWidth - windowWidth) / 2;
						int windowY = (screenHeight - windowHeight) / 2;

						// 按钮被点击时，创建 常用网站快捷打开 子窗口
						hwnd_website = CreateWindowEx(WS_EX_CLIENTEDGE, "ChildWindowClass", "常用网站快捷打开", WS_SYSMENU | WS_MINIMIZEBOX, windowX, windowY, windowWidth, windowHeight, NULL, NULL, GetModuleHandle(NULL), NULL);
						ShowWindow(hwnd_website, SW_SHOW);
						isWebsiteWindowOpen = 1; // 将变量设为1，表示“网站”子窗口已打开
					} else {
						MessageBox(hwnd, "窗口已存在！=-=", "提示=-=", MB_OK | MB_ICONASTERISK);
						// 置顶后取消置顶窗口提醒用户
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
						int windowHeight = 340; // 窗口高度
						int windowX = (screenWidth - windowWidth) / 2;
						int windowY = (screenHeight - windowHeight) / 2;

						// 按钮被点击时，创建 网络映射辅助工具 子窗口
						hwnd_mapping = CreateWindowEx(WS_EX_CLIENTEDGE, childwindow, "网络映射辅助工具", WS_SYSMENU | WS_MINIMIZEBOX, windowX, windowY, windowWidth, windowHeight, NULL, NULL, GetModuleHandle(NULL), NULL);
						ShowWindow(hwnd_mapping, SW_SHOW);
						isMappingWindowOpen = 1; // 将变量设为1，表示“映射”子窗口已打开
					} else {
						MessageBox(hwnd, "窗口已存在！=-=", "提示=-=", MB_OK | MB_ICONASTERISK);
						// 置顶后取消置顶窗口提醒用户
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
						hwnd_iTools = CreateWindowEx(WS_EX_CLIENTEDGE, "iToolsWindowClass", "实用工具箱", WS_SYSMENU | WS_MINIMIZEBOX, windowX, windowY, windowWidth, windowHeight, NULL, NULL, GetModuleHandle(NULL), NULL);
						ShowWindow(hwnd_iTools, SW_SHOW);
						isiToolsWindowOpen = 1; // 将变量设为1，表示“工具箱”子窗口已打开
					} else {
						MessageBox(hwnd, "窗口已存在！=-=", "提示=-=", MB_OK | MB_ICONASTERISK);
						// 置顶后取消置顶窗口提醒用户
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

		/* 创建按钮 */
		case WM_CREATE: { // 创建按钮
			if (SendMessage(A_Top, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断 置顶 复选框是否被勾选
				// 勾选
				SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 置顶
			}
			HWND htext = CreateWindow("Static", "您刚刚点击了关闭按钮，请问您下一步的操作是？", WS_CHILD | WS_VISIBLE, 3, 0, 230, 40, hwnd, NULL, hInst, NULL);
			HWND hHideToTrayCheckbox = CreateWindow("button", "隐藏到托盘区", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 55, 35, 120, 20, hwnd, (HMENU)1, NULL, NULL);
			HWND hExitProgramCheckbox = CreateWindow("button", "退出程序", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 55, 55, 120, 20, hwnd, (HMENU)2, NULL, NULL);
			HWND hNoReminderCheckbox = CreateWindow("button", "不再提醒,下次退出时自动选择", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 5, 75, 220, 20, hwnd, (HMENU)3, NULL, NULL);
			HWND determinebutton = CreateWindow("button", "确认", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 23, 100, 80, 35, hwnd, (HMENU)4, hInst, NULL);
			HWND cancellationbutton = CreateWindow("button", "取消", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 127, 100, 80, 35, hwnd, (HMENU)5, hInst, NULL);
			// 设置 隐藏到托盘区 单选框的选择状态为选中
			SendMessage(hHideToTrayCheckbox, BM_SETCHECK, BST_CHECKED, 0);
			break;
		}

		case WM_COMMAND: { // 处理按钮点击事件
			switch (wParam) {

				case 3 : { // 不再提醒
					HWND hNoReminderCheckbox = GetDlgItem(hwnd, 3); // 获取 不再提醒... 复选框的句柄
					if (SendMessage(hNoReminderCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断 不再提醒... 复选框是否被勾选
						// 勾选
						MessageBox(hwnd_exit, "下次退出时将自动选择，如不想自动选择可在设置中更改", "提示OwO", MB_OK | MB_ICONINFORMATION);
					}
					break;
				}

				case 4 : { //确认
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
						lstrcpy(nid.szInfo, TEXT("我在这哦！qwq")); // 设置消息内容
						Shell_NotifyIcon(NIM_MODIFY, &nid); // 发送消息
					} else if (SendMessage(hExitProgramCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) {
						// 用户选择了“退出程序”
						// 销毁窗口
						DestroyWindow(hwnd);
						Shell_NotifyIcon(NIM_DELETE, &nid); // 移除托盘图标
						PostQuitMessage(0); // 如果没有这个，退出窗口时，窗口将一直保持为使用状态
					}
					break;
				}

				case 5 : { //取消
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

		/* 创建按钮 */
		case WM_CREATE: { // 创建按钮
			if (SendMessage(A_Top, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断 置顶 复选框是否被勾选
				// 勾选
				SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 置顶
			}
			HWND Nevernotifytext = CreateWindow("BUTTON", "设置-快捷菜单", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 5, 5, 280, 180, hwnd, NULL, NULL, NULL);
			HWND Nevernotifyresetting = CreateWindow("button", "重置不再提醒", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 15, 30, 100, 30, hwnd, (HMENU)1, NULL, NULL);
			HWND testtext = CreateWindow("BUTTON", "实验性功能", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 5, 185, 280, 100, hwnd, NULL, NULL, NULL);
			A_strongTop = CreateWindow("button", "加强置顶", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 15, 205, 80, 15, hwnd, (HMENU)2, hInst, NULL);

			// 根据全局变量的值设置复选框的勾选状态
			SendMessage(A_strongTop, BM_SETCHECK, strongTopChecked, 0);

			std::ifstream file("data.json");
			std::string line;
			bool found = false;

			while (std::getline(file, line)) {
				if (line.find("Never notify") != std::string::npos) {
					found = true;
				}
			}

			file.close();

			if (!found) {
				// 设置 重置 按钮状态为禁用
				EnableWindow(Nevernotifyresetting, FALSE);
			}
			break;
		}

		case WM_COMMAND: { // 处理按钮点击事件
			switch (wParam) {

				case 1 : { // 重置
					//清空文件内容
					std::ofstream file("data.json", std::ios::trunc);
					file.close();
					// 写入
					std::ofstream newFile("data.json");
					newFile << "*此文件存储快捷菜单程序的设置数据";
					newFile.close();
					// 设置 重置 按钮状态为禁用
					HWND Nevernotifyresetting = GetDlgItem(hwnd, 1); // 获取 重置 按钮的句柄
					EnableWindow(Nevernotifyresetting, FALSE);
					MessageBox(hwnd, "已完成操作！OwO", "已重置！OwO", MB_OK | MB_ICONINFORMATION);
					break;
				}

				case 2 : {
					// 获取复选框的勾选状态
					int checked = SendMessage(A_strongTop, BM_GETCHECK, 0, 0);

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

		/* 创建按钮 */
		case WM_CREATE: { // 创建按钮
			if (SendMessage(A_Top, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断 置顶 复选框是否被勾选
				// 勾选
				SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 置顶
			}
			HWND usingopen = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_DROPDOWNLIST, 165, 5, 160, 200, hwnd, (HMENU)0, NULL, NULL);
			SendMessage(usingopen, CB_ADDSTRING, 0, (LPARAM) "使用默认浏览器打开");
			SendMessage(usingopen, CB_ADDSTRING, 0, (LPARAM) "使用谷歌浏览器打开");
			SendMessage(usingopen, CB_SETCURSEL, 0, 0);

			HWND searchEngineGroupBox = CreateWindow("BUTTON", "搜索引擎", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 4, 30, 250, 100, hwnd, NULL, NULL, NULL);
			HWND baiduSearchbutton = CreateWindow("button", "百度搜索", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 15, 50, 70, 30, hwnd, (HMENU)11, NULL, NULL);
			HWND sogouSearchbutton = CreateWindow("button", "搜狗搜索", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 95, 50, 70, 30, hwnd, (HMENU)12, NULL, NULL);
			HWND bingSearchbutton = CreateWindow("button", "必应搜索", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 175, 50, 70, 30, hwnd, (HMENU)13, NULL, NULL);
			HWND sosoSearchbutton = CreateWindow("button", "搜搜搜索", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 15, 90, 70, 30, hwnd, (HMENU)14, NULL, NULL);
			HWND sllSearchbutton = CreateWindow("button", "360搜索", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 95, 90, 70, 30, hwnd, (HMENU)15, NULL, NULL);

			HWND programmingGroupBox = CreateWindow("BUTTON", "编程/学习", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 256, 30, 250, 100, hwnd, NULL, NULL, NULL);
			HWND programmingCatbutton = CreateWindow("button", "编程猫社区", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 265, 50, 80, 30, hwnd, (HMENU)21, NULL, NULL);
			HWND Dingdingbutton = CreateWindow("button", "腾讯扣叮", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 355, 50, 70, 30, hwnd, (HMENU)22, NULL, NULL);
			HWND Giteebutton = CreateWindow("button", "Gitee", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 435, 50, 65, 30, hwnd, (HMENU)23, NULL, NULL);
			HWND CSDNbutton = CreateWindow("button", "CSDN", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 265, 90, 65, 30, hwnd, (HMENU)24, NULL, NULL);

			HWND videoGroupBox = CreateWindow("BUTTON", "视频平台", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 4, 130, 250, 100, hwnd, NULL, NULL, NULL);
			HWND bilibilibutton = CreateWindow("button", "哔哩哔哩", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 15, 150, 70, 30, hwnd, (HMENU)31, NULL, NULL);
			HWND douyinbutton = CreateWindow("button", "抖音", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 95, 150, 70, 30, hwnd, (HMENU)32, NULL, NULL);
			HWND xiguabutton = CreateWindow("button", "西瓜视频", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 175, 150, 70, 30, hwnd, (HMENU)33, NULL, NULL);
			HWND kuaishoubutton = CreateWindow("button", "快手", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 15, 190, 70, 30, hwnd, (HMENU)34, NULL, NULL);

			HWND geshizhuanhGroupBox = CreateWindow("BUTTON", "格式转换", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 256, 130, 250, 100, hwnd, NULL, NULL, NULL);
			HWND convertiobutton = CreateWindow("button", "Convertio", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 265, 150, 80, 30, hwnd, (HMENU)41, NULL, NULL);
			HWND xunjiebutton = CreateWindow("button", "迅捷视频", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 355, 150, 70, 30, hwnd, (HMENU)42, NULL, NULL);

			HWND ziyuanGroupBox = CreateWindow("BUTTON", "资源/素材", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 4, 230, 250, 100, hwnd, NULL, NULL, NULL);
			HWND aigeibutton = CreateWindow("button", "爱给网", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 15, 250, 70, 30, hwnd, (HMENU)51, NULL, NULL);
			HWND pickfreebutton = CreateWindow("button", "资源导航", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 95, 250, 70, 30, hwnd, (HMENU)52, NULL, NULL);
			HWND zitibutton = CreateWindow("button", "字体下载", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 175, 250, 70, 30, hwnd, (HMENU)53, NULL, NULL);

			HWND yuleGroupBox = CreateWindow("BUTTON", "娱乐", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 256, 230, 250, 100, hwnd, NULL, NULL, NULL);
			HWND Factsbutton = CreateWindow("button", "全球实况", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 265, 250, 70, 30, hwnd, (HMENU)61, NULL, NULL);
			HWND chishabutton = CreateWindow("button", "今天吃啥", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 345, 250, 70, 30, hwnd, (HMENU)62, NULL, NULL);
			HWND mcbutton = CreateWindow("button", "经典MC", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 425, 250, 70, 30, hwnd, (HMENU)63, NULL, NULL);
			HWND gcfzbutton = CreateWindow("button", "工厂放置", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 265, 290, 70, 30, hwnd, (HMENU)64, NULL, NULL);
			break;
		}

		case WM_COMMAND: { // 处理按钮点击事件
			switch (wParam) {

				case 11 : { // 百度搜索
					HWND Usingopen = GetDlgItem(hwnd, 0); // 获取下拉选择框的句柄
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // 获取选择的索引
					if (selectedIndex == 0) {
						// 选择使用默认浏览器打开
						const char* url = "https://www.baidu.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// 选择使用谷歌浏览器打开
						const char* url = "https://www.baidu.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 12 : { // 搜狗搜索
					HWND Usingopen = GetDlgItem(hwnd, 0); // 获取下拉选择框的句柄
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // 获取选择的索引
					if (selectedIndex == 0) {
						// 选择使用默认浏览器打开
						const char* url = "https://www.sogou.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// 选择使用谷歌浏览器打开
						const char* url = "https://www.sogou.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 13 : { // 必应搜索
					HWND Usingopen = GetDlgItem(hwnd, 0); // 获取下拉选择框的句柄
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // 获取选择的索引
					if (selectedIndex == 0) {
						// 选择使用默认浏览器打开
						const char* url = "https://cn.bing.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// 选择使用谷歌浏览器打开
						const char* url = "https://cn.bing.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 14 : { // 搜搜搜索
					HWND Usingopen = GetDlgItem(hwnd, 0); // 获取下拉选择框的句柄
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // 获取选择的索引
					if (selectedIndex == 0) {
						// 选择使用默认浏览器打开
						const char* url = "https://www.soso.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// 选择使用谷歌浏览器打开
						const char* url = "https://www.soso.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 15 : { // 360搜索
					HWND Usingopen = GetDlgItem(hwnd, 0); // 获取下拉选择框的句柄
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // 获取选择的索引
					if (selectedIndex == 0) {
						// 选择使用默认浏览器打开
						const char* url = "https://www.so.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// 选择使用谷歌浏览器打开
						const char* url = "https://www.so.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 21 : { // 编程猫社区
					HWND Usingopen = GetDlgItem(hwnd, 0); // 获取下拉选择框的句柄
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // 获取选择的索引
					if (selectedIndex == 0) {
						// 选择使用默认浏览器打开
						const char* url = "https://shequ.codemao.cn/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// 选择使用谷歌浏览器打开
						const char* url = "https://shequ.codemao.cn/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 22 : { // 腾讯扣叮
					HWND Usingopen = GetDlgItem(hwnd, 0); // 获取下拉选择框的句柄
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // 获取选择的索引
					if (selectedIndex == 0) {
						// 选择使用默认浏览器打开
						const char* url = "https://coding.qq.com/home/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// 选择使用谷歌浏览器打开
						const char* url = "https://coding.qq.com/home/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 23 : { // Gitee
					HWND Usingopen = GetDlgItem(hwnd, 0); // 获取下拉选择框的句柄
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // 获取选择的索引
					if (selectedIndex == 0) {
						// 选择使用默认浏览器打开
						const char* url = "https://gitee.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// 选择使用谷歌浏览器打开
						const char* url = "https://gitee.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 24 : { // CSDN
					HWND Usingopen = GetDlgItem(hwnd, 0); // 获取下拉选择框的句柄
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // 获取选择的索引
					if (selectedIndex == 0) {
						// 选择使用默认浏览器打开
						const char* url = "https://www.csdn.net/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// 选择使用谷歌浏览器打开
						const char* url = "https://www.csdn.net/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 31 : { // bilibili
					HWND Usingopen = GetDlgItem(hwnd, 0); // 获取下拉选择框的句柄
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // 获取选择的索引
					if (selectedIndex == 0) {
						// 选择使用默认浏览器打开
						const char* url = "https://www.bilibili.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// 选择使用谷歌浏览器打开
						const char* url = "https://www.bilibili.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 32 : { // 抖音
					HWND Usingopen = GetDlgItem(hwnd, 0); // 获取下拉选择框的句柄
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // 获取选择的索引
					if (selectedIndex == 0) {
						// 选择使用默认浏览器打开
						const char* url = "https://www.douyin.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// 选择使用谷歌浏览器打开
						const char* url = "https://www.douyin.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 33 : { // 西瓜视频
					HWND Usingopen = GetDlgItem(hwnd, 0); // 获取下拉选择框的句柄
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // 获取选择的索引
					if (selectedIndex == 0) {
						// 选择使用默认浏览器打开
						const char* url = "https://www.ixigua.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// 选择使用谷歌浏览器打开
						const char* url = "https://www.ixigua.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 34 : { // 快手
					HWND Usingopen = GetDlgItem(hwnd, 0); // 获取下拉选择框的句柄
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // 获取选择的索引
					if (selectedIndex == 0) {
						// 选择使用默认浏览器打开
						const char* url = "https://www.kuaishou.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// 选择使用谷歌浏览器打开
						const char* url = "https://www.kuaishou.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 41 : { // Convertio
					HWND Usingopen = GetDlgItem(hwnd, 0); // 获取下拉选择框的句柄
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // 获取选择的索引
					if (selectedIndex == 0) {
						// 选择使用默认浏览器打开
						const char* url = "https://convertio.co/zh/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// 选择使用谷歌浏览器打开
						const char* url = "https://convertio.co/zh/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 42 : { // 迅捷视频
					HWND Usingopen = GetDlgItem(hwnd, 0); // 获取下拉选择框的句柄
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // 获取选择的索引
					if (selectedIndex == 0) {
						// 选择使用默认浏览器打开
						const char* url = "https://app.xunjieshipin.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// 选择使用谷歌浏览器打开
						const char* url = "https://app.xunjieshipin.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 51 : { // 爱给网
					HWND Usingopen = GetDlgItem(hwnd, 0); // 获取下拉选择框的句柄
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // 获取选择的索引
					if (selectedIndex == 0) {
						// 选择使用默认浏览器打开
						const char* url = "https://www.aigei.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// 选择使用谷歌浏览器打开
						const char* url = "https://www.aigei.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 52 : { // 资源导航
					HWND Usingopen = GetDlgItem(hwnd, 0); // 获取下拉选择框的句柄
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // 获取选择的索引
					if (selectedIndex == 0) {
						// 选择使用默认浏览器打开
						const char* url = "http://www.pickfree.cn/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// 选择使用谷歌浏览器打开
						const char* url = "http://www.pickfree.cn/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 53 : { // 字体下载
					HWND Usingopen = GetDlgItem(hwnd, 0); // 获取下拉选择框的句柄
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // 获取选择的索引
					if (selectedIndex == 0) {
						// 选择使用默认浏览器打开
						const char* url = "https://font.chinaz.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// 选择使用谷歌浏览器打开
						const char* url = "https://font.chinaz.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 61 : { // 全球实况
					HWND Usingopen = GetDlgItem(hwnd, 0); // 获取下拉选择框的句柄
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // 获取选择的索引
					if (selectedIndex == 0) {
						// 选择使用默认浏览器打开
						const char* url = "https://www.skylinewebcams.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// 选择使用谷歌浏览器打开
						const char* url = "https://www.skylinewebcams.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 62 : { // 今天吃啥
					HWND Usingopen = GetDlgItem(hwnd, 0); // 获取下拉选择框的句柄
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // 获取选择的索引
					if (selectedIndex == 0) {
						// 选择使用默认浏览器打开
						const char* url = "http://guozhivip.com/eat/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// 选择使用谷歌浏览器打开
						const char* url = "http://guozhivip.com/eat/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 63 : { // 经典
					HWND Usingopen = GetDlgItem(hwnd, 0); // 获取下拉选择框的句柄
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // 获取选择的索引
					if (selectedIndex == 0) {
						// 选择使用默认浏览器打开
						const char* url = "https://classic.minecraft.net/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// 选择使用谷歌浏览器打开
						const char* url = "https://classic.minecraft.net/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 64 : { // 工厂放置
					HWND Usingopen = GetDlgItem(hwnd, 0); // 获取下拉选择框的句柄
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // 获取选择的索引
					if (selectedIndex == 0) {
						// 选择使用默认浏览器打开
						const char* url = "https://likexia.gitee.io/idle-factory/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// 选择使用谷歌浏览器打开
						const char* url = "https://likexia.gitee.io/idle-factory/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
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
				int result = MessageBox(hwnd, "正在倒计时，退出将丢失进度，确认退出？\n您可在主窗口点击退出选择隐藏到托盘区", "提示=-=", MB_OKCANCEL | MB_ICONQUESTION);
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
			break;
		}

		/* 创建按钮 */
		case WM_CREATE: { // 创建按钮
			if (SendMessage(A_Top, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断 置顶 复选框是否被勾选
				// 勾选
				SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 置顶
			}
			hGroupBoxMapping = CreateWindow("BUTTON", "映射/断开", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 250, 5, 240, 300, hwnd, NULL, NULL, NULL);
			usernameLabel = CreateWindow("STATIC", "用户名：", WS_VISIBLE | WS_CHILD, 260, 30, 80, 20, hwnd, NULL, NULL, NULL);
			usernameEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 330, 27, 150, 26, hwnd, NULL, NULL, NULL); // 使用WS_EX_CLIENTEDGE样式使输入框变成默认
			passwordLabel = CreateWindow("STATIC", "密码：", WS_VISIBLE | WS_CHILD, 260, 60, 80, 20, hwnd, NULL, NULL, NULL);
			passwordEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD | ES_AUTOHSCROLL, 330, 57, 150, 26, hwnd, NULL, NULL, NULL); // 加上了ES_AUTOHSCROLL以便可以无限输入
			ipLabel = CreateWindow("STATIC", "IP地址：", WS_VISIBLE | WS_CHILD, 260, 90, 80, 20, hwnd, NULL, NULL, NULL);
			ipComboBox = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN, 330, 87, 150, 200, hwnd, NULL, NULL, NULL);
			SendMessage(ipComboBox, CB_ADDSTRING, 0, (LPARAM) "192.168.60.250");
			SendMessage(ipComboBox, CB_ADDSTRING, 0, (LPARAM) "192.168.60.199");
			SendMessage(ipComboBox, CB_SETCURSEL, 0, 0);
			sharedFolderLabel = CreateWindow("STATIC", "共享目录：", WS_VISIBLE | WS_CHILD, 260, 120, 75, 20, hwnd, NULL, NULL, NULL);
			sharedFolderEdit = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN, 330, 117, 150, 200, hwnd, NULL, NULL, NULL);
			SendMessage(sharedFolderEdit, CB_ADDSTRING, 0, (LPARAM) "共享访问");
			SendMessage(sharedFolderEdit, CB_ADDSTRING, 0, (LPARAM) "home");
			SendMessage(sharedFolderEdit, CB_SETCURSEL, 0, 0);
			driveLabel = CreateWindow("STATIC", "盘符：", WS_VISIBLE | WS_CHILD, 260, 150, 80, 20, hwnd, NULL, NULL, NULL);
			driveComboBox = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWNLIST | WS_VSCROLL, 330, 147, 150, 200, hwnd, NULL, NULL, NULL);
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
			SendMessage(driveComboBox, CB_SETCURSEL, 0, 0);
			countdownLabel = CreateWindow("STATIC", "距断开所有还有：", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 260, 180, 180, 20, hwnd, NULL, NULL, NULL);
			connectButton = CreateWindow("BUTTON", "映射", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 260, 202, 103, 25, hwnd, (HMENU) 1, NULL, NULL);
			disconnectButton = CreateWindow("BUTTON", "断开全部", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 375, 202, 103, 25, hwnd, (HMENU) 2, NULL, NULL);
			ForcedisconnectallButton = CreateWindow("BUTTON", "强制断开全部(重启资源管理器)", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 260, 232, 218, 25, hwnd, (HMENU) 333, NULL, NULL);
			htext = CreateWindow("Static", "按下回车键(Enter)后切换到下一个(仅前两个输入框和映射按钮)", WS_CHILD | WS_VISIBLE, 260, 263, 220, 40, hwnd, NULL, hInst, NULL);

			hGroupBoxSettings = CreateWindow("BUTTON", "设置/其他", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 5, 5, 240, 300, hwnd, NULL, NULL, NULL);
			showPasswordCheckbox = CreateWindow("BUTTON", "显示密码", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, 85, 80, 20, hwnd, (HMENU)999, NULL, NULL);
			LockPassword = CreateWindow("BUTTON", "锁定密码", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 100, 85, 80, 20, hwnd, (HMENU)998, NULL, NULL);
			countdownCheckbox = CreateWindow("BUTTON", "首次映射成功后开启倒计时", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, 130, 200, 20, hwnd, (HMENU)3, NULL, NULL);
			hStaticSetCountdownTime = CreateWindow("Static", "设置倒计时时间：", WS_CHILD | WS_VISIBLE, 10, 150, 120, 20, hwnd, NULL, hInst, NULL);
			countdownEdit = CreateWindow("EDIT", "40", WS_VISIBLE | WS_CHILD | WS_BORDER, 130, 150, 40, 20, hwnd, NULL, NULL, NULL);
			countdownUnitComboBox = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_DROPDOWNLIST, 170, 149, 40, 200, hwnd, NULL, NULL, NULL); // 可以使用CBS_DROPDOWNLIST样式使控件无法编辑
			SendMessage(countdownUnitComboBox, CB_ADDSTRING, 0, (LPARAM) "分");
			SendMessage(countdownUnitComboBox, CB_ADDSTRING, 0, (LPARAM) "秒");
			SendMessage(countdownUnitComboBox, CB_SETCURSEL, 0, 0);
			jieshudaojishicheckbox = CreateWindow("BUTTON", "断开后不结束倒计时", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, 170, 160, 20, hwnd, NULL, NULL, NULL);
			detailedButton = CreateWindow("BUTTON", "详细", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 271, 70, 30, hwnd, (HMENU) 4, NULL, NULL);
			hStaticMappingKernel = CreateWindow("Static", "映射内核", WS_CHILD | WS_VISIBLE, 10, 25, 60, 20, hwnd, NULL, hInst, NULL);
			hBtnWNetAddConnection2A = CreateWindow("button", "使用WNetAddConnection2A函数", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 10, 45, 230, 20, hwnd, (HMENU)5, NULL, NULL);
			hBtnNetCommandLine = CreateWindow("button", "使用NET命令行命令", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 10, 65, 150, 20, hwnd, (HMENU)6, NULL, NULL);
			Saveusername = CreateWindow("BUTTON", "映射成功后保存用户名", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, 105, 200, 20, hwnd, NULL, NULL, NULL);
			notice = CreateWindow("Static", "使用须知：保存的用户名存储在D盘，没有D盘将存储在此程序所在位置的文件夹中。推荐将此程序连文件夹放在除C盘外的其他盘中。可创建快捷方式到桌面使用", WS_CHILD | WS_VISIBLE, 10, 190, 220, 80, hwnd, NULL, hInst, NULL);
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
			break;
		}

		/* 计时器 */
		case WM_TIMER: {
			if (wParam == 2) { // 判断计时器ID
				if (GetFocus() == usernameEdit) { // 如果用户名输入框被选中
					if (GetAsyncKeyState(VK_RETURN) & 0x8000) { // 用户点击了回车键
						SetFocus(passwordEdit); // 切换到密码输入框
						Sleep(500);	// 等待一会，防止连续
					}
				} else if (GetFocus() == passwordEdit) { // 如果密码输入框被选中
					if (GetAsyncKeyState(VK_RETURN) & 0x8000) { // 用户点击了回车键
						SendMessage(connectButton, BM_CLICK, 0, 0); // 点击映射按钮
						Sleep(500); // 等待一会，防止连续
					}
				}
				// 判断当前位置是否在C盘
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
							//std::cout << "文件不存在" << std::endl;
							if (!noContentExecuted) {
								DestroyWindow(usernameEdit); // 删除用户名输入框
								// 创建一个没有下拉列表的
								usernameEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 330, 27, 150, 26, hwnd, NULL, NULL, NULL);
								hasContentExecuted = false; // 设置标志变量
								noContentExecuted = true; // 设置标志变量
							}
						} else {
							// 文件存在，获取用户名列表并创建下拉列表
							//std::cout << "文件已存在" << std::endl;
							// 判断存储文件是否有内容
							std::ifstream inputFile(filePath);
							if (inputFile.is_open()) {
								std::string line;
								if (std::getline(inputFile, line)) {
									// 如果文件有内容，创建下拉列表框并将内容添加到下拉列表中
									if (!hasContentExecuted) {
										DestroyWindow(usernameEdit); // 删除用户名输入框
										// 创建一个没有下拉列表的
										usernameEdit = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | WS_VSCROLL, 330, 27, 150, 200, hwnd, NULL, NULL, NULL); // 使用WS_EX_CLIENTEDGE样式使输入框变成默认
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
											// 创建一个没有下拉列表的
											usernameEdit = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | WS_VSCROLL, 330, 27, 150, 200, hwnd, NULL, NULL, NULL); // 使用WS_EX_CLIENTEDGE样式使输入框变成默认
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
										usernameEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 330, 27, 150, 26, hwnd, NULL, NULL, NULL);
										hasContentExecuted = false; // 设置标志变量
										noContentExecuted = true; // 设置标志变量
									}
								}
								inputFile.close();
							}
						}
					} else {
						//std::cout << "D盘不存在或非固定/可移动磁盘" << std::endl;
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
						//std::cout << "文件不存在" << std::endl;
						if (!noContentExecuted) {
							DestroyWindow(usernameEdit); // 删除用户名输入框
							// 创建一个没有下拉列表的
							usernameEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 330, 27, 150, 26, hwnd, NULL, NULL, NULL);
							hasContentExecuted = false; // 设置标志变量
							noContentExecuted = true; // 设置标志变量
						}
					} else {
						// 文件存在，获取用户名列表并创建下拉列表
						//std::cout << "文件已存在" << std::endl;
						// 判断存储文件是否有内容
						std::ifstream inputFile(filePath);
						if (inputFile.is_open()) {
							std::string line;
							if (std::getline(inputFile, line)) {
								// 如果文件有内容，创建下拉列表框并将内容添加到下拉列表中
								if (!hasContentExecuted) {
									DestroyWindow(usernameEdit); // 删除用户名输入框
									// 创建一个没有下拉列表的
									usernameEdit = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | WS_VSCROLL, 330, 27, 150, 200, hwnd, NULL, NULL, NULL); // 使用WS_EX_CLIENTEDGE样式使输入框变成默认
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
										// 创建一个没有下拉列表的
										usernameEdit = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | WS_VSCROLL, 330, 27, 150, 200, hwnd, NULL, NULL, NULL); // 使用WS_EX_CLIENTEDGE样式使输入框变成默认
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
									usernameEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 330, 27, 150, 26, hwnd, NULL, NULL, NULL);
									hasContentExecuted = false; // 设置标志变量
									noContentExecuted = true; // 设置标志变量
								}
							}
							inputFile.close();
						}
					}
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
							countdownLabel = CreateWindow("STATIC", "距断开所有还有：", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 260, 180, 180, 20, hwnd, NULL, NULL, NULL);
							// 显示消息框
							std::string komplettMessage = "已断开部分映射的网络驱动器！\n（代码：" + std::to_string(result) + "）";
							MessageBox(hwnd, komplettMessage.c_str(), "倒计时结束啦！OwO", MB_OK | MB_ICONINFORMATION);
						} else if (SendMessage(hBtnNetCommandLine, BM_GETCHECK, 0, 0) == BST_CHECKED) {
							string command = "net use * /del /Y";
							system(command.c_str());
							//取消禁用倒计时
							EnableWindow(countdownEdit, TRUE);
							EnableWindow(countdownUnitComboBox, TRUE);
							// 删旧倒计时
							DestroyWindow(countdownLabel);
							// 更新倒计时
							countdownLabel = CreateWindow("STATIC", "距断开所有还有：", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 260, 180, 180, 20, hwnd, NULL, NULL, NULL);
							// 显示一个消息框，提醒驱动器已断开映射
							MessageBox(hwnd, "已断开全部映射的驱动器！", "倒计时结束啦！OwO", MB_OK | MB_ICONINFORMATION);
						}
					}
				}
			}
			break;
		}

		case WM_COMMAND: { // 处理按钮点击事件
			switch (wParam) {

				case 999: { // 显示密码
					LONG_PTR style = GetWindowLongPtr(passwordEdit, GWL_STYLE); // 获取输入框的样式
					if (style & ES_PASSWORD) { // 判断输入框样式中是否包含ES_PASSWORD标志
						/* 如果密码输入框的内容为密码字符（包含ES_PASSWORD） */
						// 获取旧密码输入框的内容
						char passwordBuf[256];
						GetWindowText(passwordEdit, passwordBuf, sizeof(passwordBuf));
						// 删除旧的密码字符密码输入框
						DestroyWindow(passwordEdit);
						// 创建新的明文密码输入框
						passwordEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 330, 57, 150, 26, hwnd, NULL, NULL, NULL);
						// 将内容复制到新密码输入框
						SetWindowText(passwordEdit, passwordBuf);
					} else {
						/* 如果密码输入框的内容为明文（不包含ES_PASSWORD） */
						// 获取旧密码输入框的内容
						char passwordBuf[256];
						GetWindowText(passwordEdit, passwordBuf, sizeof(passwordBuf));
						// 删除旧的明文密码输入框
						DestroyWindow(passwordEdit);
						// 创建新的密码字符密码输入框
						passwordEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD | ES_AUTOHSCROLL, 330, 57, 150, 26, hwnd, NULL, NULL, NULL);
						// 将内容复制到新密码输入框
						SetWindowText(passwordEdit, passwordBuf);
					}
					break; // 退出循环，如果没有这个会运行下一行的代码，或者无响应（卡死）
				}

				case 998 : { // 锁定密码
					if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断 锁定密码 复选框是否被勾选
						// 勾选
						MessageBox(hwnd, "您勾选了锁定密码复选框，当映射成功或失败后将不会清空密码\n但请注意，请在完成映射操作后自行清空密码以防您的密码泄露！", "提示OwO", MB_OK | MB_ICONINFORMATION);
					}
					break;
				}

				case 1: {// 映射远程共享文件夹
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
								std::cout << "网络驱动器映射成功！\n（代码：" << result << "）" << std::endl;
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 锁定密码 复选框是否未被勾选
									SetWindowText(passwordEdit, ""); // 将密码输入框的文本设置为空字符串
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
								std::string successMessage = "网络驱动器映射成功！OwO\n（代码：" + std::to_string(result) + "）";
								MessageBox(hwnd, successMessage.c_str(), "成功啦！OwO", MB_OK | MB_ICONINFORMATION);
							} else {
								std::cout << "网络驱动器映射失败！\n（代码：" << result << "）" << std::endl;
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 锁定密码 复选框是否未被勾选
									SetWindowText(passwordEdit, ""); // 将密码输入框的文本设置为空字符串
								}
								std::string errorMessage = "网络驱动器映射失败！X_X\n（代码：" + std::to_string(result) + "）";
								MessageBox(hwnd, errorMessage.c_str(), "出错了！X_X", MB_OK | MB_ICONERROR);
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
								// 连接成功
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 锁定密码 复选框是否未被勾选
									SetWindowText(passwordEdit, ""); // 将密码输入框的文本设置为空字符串
								}
								MessageBox(hwnd, "没有任何问题！OwO", "成功啦！OwO", MB_OK | MB_ICONINFORMATION);
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
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 锁定密码 复选框是否未被勾选
									SetWindowText(passwordEdit, ""); // 将密码输入框的文本设置为空字符串
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
								std::string successMessage = "网络驱动器映射成功！OwO\n（代码：" + std::to_string(result) + "）";
								MessageBox(hwnd, successMessage.c_str(), "成功啦！OwO", MB_OK | MB_ICONINFORMATION);
							} else {
								std::cout << "网络驱动器映射失败！\n（代码：" << result << "）" << std::endl;
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 锁定密码 复选框是否未被勾选
									SetWindowText(passwordEdit, ""); // 将密码输入框的文本设置为空字符串
								}
								std::string errorMessage = "网络驱动器映射失败！X_X\n（代码：" + std::to_string(result) + "）";
								MessageBox(hwnd, errorMessage.c_str(), "出错了！X_X", MB_OK | MB_ICONERROR);
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
								// 连接成功
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 锁定密码 复选框是否未被勾选
									SetWindowText(passwordEdit, ""); // 将密码输入框的文本设置为空字符串
								}
								MessageBox(hwnd, "没有任何问题！OwO", "成功啦！OwO", MB_OK | MB_ICONINFORMATION);
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
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 锁定密码 复选框是否未被勾选
									SetWindowText(passwordEdit, ""); // 将密码输入框的文本设置为空字符串
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
								std::string successMessage = "网络驱动器映射成功！OwO\n（代码：" + std::to_string(result) + "）";
								MessageBox(hwnd, successMessage.c_str(), "成功啦！OwO", MB_OK | MB_ICONINFORMATION);
							} else {
								std::cout << "网络驱动器映射失败！\n（代码：" << result << "）" << std::endl;
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 锁定密码 复选框是否未被勾选
									SetWindowText(passwordEdit, ""); // 将密码输入框的文本设置为空字符串
								}
								std::string errorMessage = "网络驱动器映射失败！X_X\n（代码：" + std::to_string(result) + "）";
								MessageBox(hwnd, errorMessage.c_str(), "出错了！X_X", MB_OK | MB_ICONERROR);
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
								// 连接成功
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 锁定密码 复选框是否未被勾选
									SetWindowText(passwordEdit, ""); // 将密码输入框的文本设置为空字符串
								}
								MessageBox(hwnd, "没有任何问题！OwO", "成功啦！OwO", MB_OK | MB_ICONINFORMATION);
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
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 锁定密码 复选框是否未被勾选
									SetWindowText(passwordEdit, ""); // 将密码输入框的文本设置为空字符串
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
								std::string successMessage = "网络驱动器映射成功！OwO\n（代码：" + std::to_string(result) + "）";
								MessageBox(hwnd, successMessage.c_str(), "成功啦！OwO", MB_OK | MB_ICONINFORMATION);
							} else {
								std::cout << "网络驱动器映射失败！\n（代码：" << result << "）" << std::endl;
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 锁定密码 复选框是否未被勾选
									SetWindowText(passwordEdit, ""); // 将密码输入框的文本设置为空字符串
								}
								std::string errorMessage = "网络驱动器映射失败！X_X\n（代码：" + std::to_string(result) + "）";
								MessageBox(hwnd, errorMessage.c_str(), "出错了！X_X", MB_OK | MB_ICONERROR);
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
								// 连接成功
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // 判断 锁定密码 复选框是否未被勾选
									SetWindowText(passwordEdit, ""); // 将密码输入框的文本设置为空字符串
								}
								MessageBox(hwnd, "没有任何问题！OwO", "成功啦！OwO", MB_OK | MB_ICONINFORMATION);
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
						countdownLabel = CreateWindow("STATIC", "距断开所有还有：", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 260, 180, 180, 20, hwnd, NULL, NULL, NULL);
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
						std::string komplettMessage = "已断开部分映射的网络驱动器！\n（代码：" + std::to_string(result) + "）";
						MessageBox(hwnd, komplettMessage.c_str(), "完成啦！OwO", MB_OK | MB_ICONINFORMATION);
					} else if (SendMessage(hBtnNetCommandLine, BM_GETCHECK, 0, 0) == BST_CHECKED) {
						string command = "net use * /del /Y";
						system(command.c_str());
						MessageBox(hwnd, "已完成操作！OwO", "完成啦！OwO", MB_OK | MB_ICONINFORMATION);
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
						countdownLabel = CreateWindow("STATIC", "距断开所有还有：", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 260, 180, 180, 20, hwnd, NULL, NULL, NULL);
						// 禁用倒计时
						EnableWindow(countdownLabel, FALSE);
						EnableWindow(countdownEdit, FALSE);
						EnableWindow(countdownUnitComboBox, FALSE);
					}
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
						countdownLabel = CreateWindow("STATIC", "距断开所有还有：", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 260, 180, 180, 20, hwnd, NULL, NULL, NULL);
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
						std::string komplettMessage = "已断开部分映射的网络驱动器！\n如果还有问题请尝试重启计算机！X_x\n（代码：" + std::to_string(result) + "）";
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
						MessageBox(hwnd, "已完成操作！OwO\n如果还有问题请尝试重启计算机！X_x", "完成啦！OwO", MB_OK | MB_ICONINFORMATION);
					}
					break;
				}

				case 4: { // 详细
					MessageBox(hwnd, "Q&A问答（Question&Answer）：\nQ：这是什么，有什么用？\nA；这是可以将NAS网络存储器映射到本地网络驱动器（网络驱动器相当于硬盘）的辅助程序，它可以使您更便携地使用您的远程网络存储器设备\nQ；NAS是什么？\nA；NAS（Network Attached Storage：网络附属存储）按字面简单说就是连接在网络上，具备资料存储功能的装置，因此也称为“网络存储器”。它是一种专用数据存储服务器——来源于 百度百科\n\n使用须知：\n保存的用户名将存储在D盘中（如果有），如果没有D盘将存储在此程序所在位置的文件夹中。推荐将此程序连同文件夹放在除C盘外的其他盘中。可创建快捷方式到桌面使用\n\n高级：\n可在主窗口勾选\"自动打开存储设备\"复选框后映射，映射成功后将自动打开映射的文件夹\n\n技术与支持：\n使用到的部分映射代码：net use [盘符]: \\[IP]\[共享文件夹]，使用到的部分断开代码：net use * /del /Y，以上代码均为命令行（命令提示符）代码，可在命令行中直接使用。以上提供的代码并非为程序内用到的所有代码，具体可自行上网查询。此映射辅助程序支持多种不同的映射逻辑判断，您如果是管理员则可以无需账户直接进行映射。支持自定义倒计时时间及单位，且您如果在倒计时时点击退出会有提醒。支持隐藏到后台（系统托盘）继续进行倒计时工作（需在主窗口点击退出-选择隐藏到托盘区-然后确定，如果开启了不再提醒可在设置中重置）\n\n常见问题及解决办法：\n在使用的过程中可能会遇到程序无响应的情况，这是正常情况请您耐心等待几秒，如等待时间过长可自行强制关闭程序（任务栏右键-点击启动任务管理器-切换到应用程序选项页-选择此程序-点击结束任务）\n\n我们的用心才能换来您舒服的体验和对我们的支持，最后，感谢您的使用！——开发者：Ling", "详细^u^", MB_OK );
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

		/* 创建按钮 */
		case WM_CREATE: { // 创建按钮
			if (SendMessage(A_Top, BM_GETCHECK, 0, 0) == BST_CHECKED) { // 判断 置顶 复选框是否被勾选
				// 勾选
				SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 置顶
			}
			HWND SystemToolsGroupBox = CreateWindow("BUTTON", "系统工具", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 5, 5, 210, 305, hwnd, NULL, NULL, NULL);
			HWND restartButton = CreateWindow("BUTTON", "重启资源管理器和组策略", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 15, 30, 190, 30, hwnd, (HMENU)1, NULL, NULL);
			HWND OthertoolsGroupBox = CreateWindow("BUTTON", "其他工具", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 220, 5, 210, 305, hwnd, NULL, NULL, NULL);
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
	wc.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 0);
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

	/* 创建时检测主窗口是否已存在，多余的销毁 */
	// 创建一个互斥体
	HANDLE hMutex = CreateMutex(NULL, TRUE, "MyAppMutex");
	// 检查互斥体是否已经存在
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		// 如果互斥体已经存在，表示已经有一个主窗口进程在运行，因此销毁当前实例
		CloseHandle(hMutex);
		// 在程序退出前释放互斥体
		CloseHandle(hMutex);
		MessageBox(host_hwnd, "此程序进程已在运行，如任务栏没有，请检查系统托盘", "提示O_O", MB_OK | MB_ICONASTERISK); // 因为是新建窗口不能发送消息到旧窗口所以只能提醒用户自行查看
		return 0;
	}

	/* 使窗口始终出现在屏幕中间 */
	// 获取屏幕宽度和高度
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	// 计算窗口左上角坐标
	int windowWidth = 280; // 窗口宽度
	int windowHeight = 300; // 窗口高度
	int windowX = (screenWidth - windowWidth) / 2;
	int windowY = (screenHeight - windowHeight) / 2;

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

	host_hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, "WindowClass", "快捷菜单", WS_VISIBLE | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, windowX, windowY, windowWidth, windowHeight, NULL, NULL, hInstance, NULL);

	if (host_hwnd == NULL) {
		MessageBox(NULL, "窗口创建失败！", "错误", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// 创建菜单栏
	HMENU hMenu = CreateMenu();
	// 创建“设置”菜单项
	HMENU hSubMenu = CreatePopupMenu();
	AppendMenu(hSubMenu, MF_STRING, 32913, "详细");
	AppendMenu(hSubMenu, MF_STRING, 32914, "更新内容");
	AppendMenu(hSubMenu, MF_STRING, 32912, "设置");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSubMenu, "更多");
	// 将菜单栏关联到主窗口
	SetMenu(host_hwnd, hMenu);

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
		MessageBox(NULL, "托盘图标加载失败！X_X", "错误X_X", MB_ICONERROR | MB_OK);
	}
	strcpy(nid.szTip, "快捷菜单");
	// 添加托盘图标
	Shell_NotifyIcon(NIM_ADD, &nid);

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
	} else { // 检测文件中是否包含“*此文件存储快捷菜单程序的设置数据”
		std::ifstream file("data.json");
		if (!file) {
			MessageBox(NULL, "无法打开data.json文件！", "错误", MB_ICONEXCLAMATION | MB_OK);
			return 0;
		}
		// 读取data.json文件并查看是否含有“*此文件存储快捷菜单程序的设置数据”
		std::string line;
		bool found = false;
		while (std::getline(file, line)) {
			if (line.find("*此文件存储快捷菜单程序的设置数据") != std::string::npos) {
				found = true;
				break;
			}
		}

		file.close();
		// 如果不包含那么新建一个新的data.json文件并写入文本
		if (!found) {
			std::ofstream newFile("data.json");
			newFile << "*此文件存储快捷菜单程序的设置数据";
			newFile.close();
		}
	}

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
