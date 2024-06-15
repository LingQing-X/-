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

// ����ȫ�ֱ���
const char childwindow[] = "Drive mapping"; // ����ӳ�丨������ �Ӵ��ڵ�ȫ�ֱ���
int isExitWindowOpen = 0; // ����ʹ���˳����Ӵ���ֻ�ܴ�һ��
int isSettingsWindowOpen = 0; // ����ʹ�����á��Ӵ���ֻ�ܴ�һ��
int isWebsiteWindowOpen = 0; // ����ʹ����վ���Ӵ���ֻ�ܴ�һ��
int isMappingWindowOpen = 0; // ����ʹ��ӳ�䡱�Ӵ���ֻ�ܴ�һ��
int isiToolsWindowOpen = 0; // ����ʹ�������䡱�Ӵ���ֻ�ܴ�һ��
int CtrlxhuoCtrlC = 0; // ����ʹ��������һ�ε���ֹͣ

// ���ھ��
HWND host_hwnd; // ���������ڵľ������
HWND hwnd_exit; // �������˳������ڵľ������
HWND hwnd_Settings; // ���������á����ڵľ������
HWND hwnd_website; // ��������վ�����ڵľ������
HWND hwnd_mapping; // ������ӳ�䡱���ڵľ������
HWND hwnd_iTools; // �����������䡱���ڵľ������

/* ����ӳ�丨������ �Ӵ��� */
// �ؼ����
HWND hGroupBoxMapping; // ӳ��/�Ͽ� �����
HWND usernameLabel; // �˺�
HWND usernameEdit; // �˺� �����
HWND passwordLabel; // ����
HWND passwordEdit; // ���� �����
HWND ipLabel; // IP��ַ
HWND ipComboBox; //IP��ַ �����
HWND sharedFolderLabel; //����Ŀ¼
HWND sharedFolderEdit; // ����Ŀ¼ �����
HWND driveLabel; // �̷�
HWND driveComboBox; // �̷� ѡ�������б�
HWND connectButton; // ���� ��ť
HWND disconnectButton; // �Ͽ� ��ť
HWND hGroupBoxSettings; // ����/���� �����
HWND showPasswordCheckbox; // ��ʾ���� ��ѡ��
HWND countdownCheckbox; // ӳ��ɹ���ʼ��ʱ ��ѡ��
HWND hStaticSetCountdownTime; // ���õ���ʱʱ�䣺
HWND countdownLabel; // ʱ������
HWND countdownEdit; // ʱ������ �����
HWND countdownUnitComboBox; // ʱ�䵥λ����
HWND detailedButton; // ��ϸ ��ť
HWND jieshudaojishicheckbox; // �Ͽ��󲻽�������ʱ ��ѡ��
HWND hStaticMappingKernel; // ӳ���ں� �ı�
HWND hBtnWNetAddConnection2A; // ʹ��WNetAddConnection2A���� ��ѡ��
HWND hBtnNetCommandLine; // ʹ��NET���������� ��ѡ��
HWND GenerateBatchProcessing; // ����ר�����ҵ�ӳ���������ļ�
HWND LockPassword; // �������� ��ѡ��
HWND Saveusername; // ӳ��ɹ��󱣴��û��� ��ѡ��
HWND ForcedisconnectallButton; // ǿ�ƶϿ�ȫ��(������Դ������) ��ť
HWND keyongpancheckbox; // ��ʾ��ռ�õ��̷� ��ѡ��
HWND zidongxuanze; // ӳ��ɹ����Զ�ѡ����һ������Ŀ¼���̷� ��ѡ��
HWND daojishi; // ����ʱ �ı�
HWND yingsheshezhi; // ӳ������ �ı�
HWND bujinyongxsmm; // ӳ��ɹ��󲻽�ֹ��ʾ���� ��ѡ��
HWND nextpage; // ��/��һҳ
HWND SaveuserIP; // ����IP��ַ
HWND Saveusershare; // ���湲��Ŀ¼
HWND yingsheshezhi2; // ��ϸ˵��2 �ı�
// �ؼ�ֵ
string username; // �˺�
string password; // ����
string ipAddress; // IP��ַ
string sharedFolder; // ����Ŀ¼
string drive; // �̷�
// ����ʱ����ʱ���ȫ�ֱ���
int countdownMinutes = 0; // ��
int countdownDuration = 0; // ��
// ��������������ı�־��
bool hasContentExecuted = false;
bool noContentExecuted = false;
bool hasContentExecuted2 = false;
bool noContentExecuted2 = false;
bool hasContentExecuted3 = false;
bool noContentExecuted3 = false;
BOOL bUpdateDriveList = TRUE;

/* �������д�������ĵط� */
HWND A_Top, A_AOU, A_text, A_text2, A_text3, A_1, A_2, A_3, A_999, hTab, Settingtext, Nevernotifyresetting, experimentalfeaturestext, strongTop, Startup, button_32913, button_32914, A_Jtb, A_4;
HINSTANCE hInst;
NOTIFYICONDATA nid; // ����

// ����һ��ȫ�ֱ������ڱ�ʾ��ǿ�ö���״̬
int strongTopChecked = 0;

// �����Ҽ��˵���ID
#define IDM_EXIT 1001

HANDLE hThread = NULL; // �Զ��������Ĵ洢�豸 ��ȫ�ֱ���

std::unordered_set<std::string> openedDrives; // �Ѵ򿪵�����������

// ������״̬��ö��
enum DriveStatus {
	DRIVE_STATUS_READY,
	DRIVE_STATUS_NOT_READY,
	DRIVE_STATUS_ERROR
};

// ��ȡ��������״̬
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

// �������������������
// ���������ʹ���ĺ���
DWORD WINAPI DriveDetection(LPVOID lpParam) {
	// �����Ѵ򿪵�����������
	std::unordered_set<std::string> openedDrives;
	// ��ȡ��ǰ�Ѳ����������
	DWORD drives = GetLogicalDrives();
	// ��������������Ƿ����
	while (true) {
		// ��ȡ���µ���������Ϣ
		DWORD newDrives = GetLogicalDrives();
		// ����Ƿ����µ�����������
		DWORD insertedDrives = newDrives & (~drives);
		if (insertedDrives != 0) {
			// �����²����������
			for (int i = 0; i < 26; ++i) {
				if ((insertedDrives >> i) & 1) {
					char driveLetter = 'A' + i;
					char drivePath[4] = { driveLetter, ':', '\\', '\0' };
					// ����������·��
					std::string drivePathStr(drivePath);
					// ��ȡ����������
					UINT driveType = GetDriveType(drivePathStr.c_str());
					// ��������������ִ����Ӧ�Ĳ���
					switch (driveType) {
						case DRIVE_NO_ROOT_DIR: {
							std::cout << "������ " << driveLetter << " �����ڻ���Ч" << std::endl;
							MessageBox(NULL, "��⵽����������������Ϊ�����ڻ���Ч", "����*_*", MB_ICONEXCLAMATION | MB_OK);
							break;
						}
						case DRIVE_UNKNOWN: {
							std::cout << "������ " << driveLetter << " ����δ֪" << std::endl;
							MessageBox(NULL, "��⵽����������������Ϊ����δ֪", "����*_*", MB_ICONEXCLAMATION | MB_OK);
							break;
						}
						case DRIVE_REMOVABLE: {
							std::cout << "������ " << driveLetter << " �ǿ��ƶ���" << std::endl;
							// ��ȡ������״̬
							DriveStatus driveStatus = GetDriveStatus(drivePathStr);
							if (driveStatus == DRIVE_STATUS_READY) {
								std::cout << "������ " << driveLetter << " ����������" << std::endl;
								// ������״̬Ϊ���������Գ��Դ�������
								HINSTANCE result = ShellExecuteA(NULL, "open", drivePath, NULL, NULL, SW_SHOW);
								// ��¼�Ѵ򿪵�������
								openedDrives.insert(drivePathStr);
							} else if (driveStatus == DRIVE_STATUS_NOT_READY) {
								std::cout << "������ " << driveLetter << " ������δ����" << std::endl;
								// ������״̬Ϊδ����������ִ����������
								// ���ϼ��������״̬��ֱ��״̬Ϊ����
								DriveStatus driveStatus = GetDriveStatus(drivePathStr);
								while (driveStatus != DRIVE_STATUS_READY) {
									driveStatus = GetDriveStatus(drivePathStr);
								}
								// ������״̬Ϊ���������Գ��Դ�������
								HINSTANCE result = ShellExecuteA(NULL, "open", drivePath, NULL, NULL, SW_SHOW);
								// ��¼�Ѵ򿪵�������
								openedDrives.insert(drivePathStr);
							} else {
								std::cout << "������ " << driveLetter << " �������쳣" << std::endl;
								// ������״̬Ϊ�쳣������ִ����������
								// ���ϼ��������״̬��ֱ��״̬Ϊ����
								DriveStatus driveStatus = GetDriveStatus(drivePathStr);
								while (driveStatus != DRIVE_STATUS_READY) {
									driveStatus = GetDriveStatus(drivePathStr);
								}
								// ������״̬Ϊ���������Գ��Դ�������
								HINSTANCE result = ShellExecuteA(NULL, "open", drivePath, NULL, NULL, SW_SHOW);
								// ��¼�Ѵ򿪵�������
								openedDrives.insert(drivePathStr);
							}
							break;
						}
						case DRIVE_FIXED: {
							std::cout << "������ " << driveLetter << " �ǹ̶���" << std::endl;
							// ���Դ�������
							HINSTANCE result = ShellExecuteA(NULL, "open", drivePath, NULL, NULL, SW_SHOW);
							// ��¼�Ѵ򿪵�������
							openedDrives.insert(drivePathStr);
							break;
						}
						case DRIVE_REMOTE: {
							std::cout << "������ " << driveLetter << " ������������" << std::endl;
							// ��ȡ������״̬
							DriveStatus driveStatus = GetDriveStatus(drivePathStr);
							if (driveStatus == DRIVE_STATUS_READY) {
								std::cout << "������ " << driveLetter << " ����������" << std::endl;
								// ������״̬Ϊ���������Գ��Դ�������
								HINSTANCE result = ShellExecuteA(NULL, "open", drivePath, NULL, NULL, SW_SHOW);
								// ��¼�Ѵ򿪵�������
								openedDrives.insert(drivePathStr);
							} else if (driveStatus == DRIVE_STATUS_NOT_READY) {
								std::cout << "������ " << driveLetter << " ������δ����" << std::endl;
								// ������״̬Ϊδ����������ִ����������
								// ���ϼ��������״̬��ֱ��״̬Ϊ����
								DriveStatus driveStatus = GetDriveStatus(drivePathStr);
								while (driveStatus != DRIVE_STATUS_READY) {
									driveStatus = GetDriveStatus(drivePathStr);
								}
								// ������״̬Ϊ���������Գ��Դ�������
								HINSTANCE result = ShellExecuteA(NULL, "open", drivePath, NULL, NULL, SW_SHOW);
								// ��¼�Ѵ򿪵�������
								openedDrives.insert(drivePathStr);
							} else {
								std::cout << "������ " << driveLetter << " �������쳣" << std::endl;
								// ������״̬Ϊ�쳣������ִ����������
								// ���ϼ��������״̬��ֱ��״̬Ϊ����
								DriveStatus driveStatus = GetDriveStatus(drivePathStr);
								while (driveStatus != DRIVE_STATUS_READY) {
									driveStatus = GetDriveStatus(drivePathStr);
								}
								// ������״̬Ϊ���������Գ��Դ�������
								HINSTANCE result = ShellExecuteA(NULL, "open", drivePath, NULL, NULL, SW_SHOW);
								// ��¼�Ѵ򿪵�������
								openedDrives.insert(drivePathStr);
							}
							break;
						}
						case DRIVE_CDROM: {
							std::cout << "������ " << driveLetter << " �ǹ���������" << std::endl;
							// ���Դ�������
							HINSTANCE result = ShellExecuteA(NULL, "open", drivePath, NULL, NULL, SW_SHOW);
							// ��¼�Ѵ򿪵�������
							openedDrives.insert(drivePathStr);
							break;
						}
						case DRIVE_RAMDISK: {
							std::cout << "������ " << driveLetter << " ��RAM����" << std::endl;
							// ���Դ�������
							HINSTANCE result = ShellExecuteA(NULL, "open", drivePath, NULL, NULL, SW_SHOW);
							// ��¼�Ѵ򿪵�������
							openedDrives.insert(drivePathStr);
							break;
						}
						default: {
							// Ĭ�������ִ�еĲ���
							break;
						}
					}
				}
			}
		}
		// ������������Ϣ
		drives = newDrives;
	}
	return 0;
}

HANDLE tThread = NULL; // �ö� ��ȫ�ֱ���

// �̺߳���������ִ��ѭ���ö�����
DWORD WINAPI TopMostThread(LPVOID lpParam) {
	while (true) {
		SetWindowPos(host_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // �ö�������
		SetWindowPos(hwnd_exit, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // �ö��˳�����
		SetWindowPos(hwnd_Settings, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // �ö����ô���
		SetWindowPos(hwnd_website, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // �ö���վ����
		SetWindowPos(hwnd_mapping, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // �ö�ӳ�䴰��
		SetWindowPos(hwnd_iTools, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // �ö������䴰��
	}
	return 0;
}

/* �˺������������ڵ���Ϣ */
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch (Message) {

		/* ������ٰ�ťʱ */
		case WM_CLOSE: {
			// ���data.json�ļ��Ƿ���ڣ��������ݣ�
			std::ifstream file("data.json");
			if (!file) {
				// data.json�ļ������ڣ�������
				std::ofstream newFile("data.json");
				if (!newFile) {
					MessageBox(NULL, "�޷�������������JSON�ļ���", "����", MB_ICONEXCLAMATION | MB_OK);
					return 0;
				}
				// ���������д���ʼ��data.json���ݵ��´������ļ���
				newFile << "*���ļ��洢��ݲ˵��������������";
				newFile.close();
				// ����ļ����Ƿ������Ӧ��ֵ
				std::ifstream file("data.json");
				if (!file) {
					MessageBox(NULL, "�޷���data.json�ļ���", "����", MB_ICONEXCLAMATION | MB_OK);
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
				// ����"...1"�Ĵ����߼�
				// ���ص�������
				// ��С��������
				ShowWindow(host_hwnd, SW_MINIMIZE);
				ShowWindow(host_hwnd, SW_HIDE);
				// ���ء����á�����
				ShowWindow(hwnd_Settings, SW_HIDE);
				// ���ء���վ������
				ShowWindow(hwnd_website, SW_HIDE);
				// ���ء�ӳ�䡱����
				ShowWindow(hwnd_mapping, SW_HIDE);
				// ���ء������䡱����
				ShowWindow(hwnd_iTools, SW_HIDE);
				// ����ϵͳ����ͼ������½�С��Ϣ
				// ����һ�� NOTIFYICONDATA �ṹ�����
				NOTIFYICONDATA nid;
				nid.cbSize = sizeof(NOTIFYICONDATA);
				nid.hWnd = host_hwnd;
				nid.uID = 1; // ָ��һ��Ψһ��ID
				nid.uFlags = NIF_INFO; // ���ñ�־��ָ��Ҫ��ʾ��Ϣ
				nid.dwInfoFlags = NIIF_INFO; // ������Ϣ����
				nid.uTimeout = 5000; // ָ����Ϣ��ʾ��ʱ�䣨���룩
				lstrcpy(nid.szInfoTitle, TEXT("��ݲ˵�")); // ������Ϣ����
				lstrcpy(nid.szInfo, TEXT("������Ŷ��qwq\n������ͼ���쳣���ٴδ򿪳�����ʾOvO")); // ������Ϣ����
				Shell_NotifyIcon(NIM_MODIFY, &nid); // ������Ϣ
			} else  if (found2) {
				// ����"...2"�Ĵ����߼�
				// �˳�����
				// �ڴ��ڹر�ʱ��� ������.txt �ļ�������
				std::ofstream file("������.txt", std::ios::trunc); // ʹ�� std::ios::trunc ģʽ���ļ�������ļ�����
				if (file.is_open()) {
					file.close(); // �ر��ļ�
					std::cout << "�ļ����������" << std::endl;
				} else {
					std::cerr << "�޷����ļ�" << std::endl;
				}

				// ���ٴ���
				DestroyWindow(hwnd);
				Shell_NotifyIcon(NIM_DELETE, &nid); // �Ƴ�����ͼ��
				PostQuitMessage(0); // ���û��������˳�����ʱ�����ڽ�һֱ����Ϊʹ��״̬
			} else {
				// �����Ĵ����߼�
				if (isExitWindowOpen == 0) { // ����Ƿ��Ѿ����ˡ��˳����Ӵ���
					/* ʹ����ʼ�ճ�������Ļ�м� */
					// ��ȡ��Ļ��Ⱥ͸߶�
					int screenWidth = GetSystemMetrics(SM_CXSCREEN);
					int screenHeight = GetSystemMetrics(SM_CYSCREEN);
					// ���㴰�����Ͻ�����
					int windowWidth = 240; // ���ڿ��
					int windowHeight = 175; // ���ڸ߶�
					int windowX = (screenWidth - windowWidth) / 2;
					int windowY = (screenHeight - windowHeight) / 2;
					hwnd_exit = CreateWindowEx(WS_EX_APPWINDOW, "ExitWindowClass", "�˳�ǰѯ��o(�i�n�i)o", WS_VISIBLE | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, windowX, windowY, windowWidth, windowHeight, NULL, NULL, GetModuleHandle(NULL), NULL);
					ShowWindow(hwnd_exit, SW_SHOW);
					isExitWindowOpen = 1; // ��������Ϊ1����ʾ���˳����Ӵ����Ѵ�
				} else {
					// �ö���ȡ���ö����������û�
					ShowWindow(hwnd_exit, SW_RESTORE);
					SetWindowPos(hwnd_exit, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
					SetWindowPos(hwnd_exit, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				}
			}
			break;
		}

		/* ������ʱ���������߳�ֹͣ */
		case WM_DESTROY: { // ����ʱ
			Shell_NotifyIcon(NIM_DELETE, &nid); // �Ƴ�����ͼ��
			PostQuitMessage(0); // ���û��������˳�����ʱ�����ڽ�һֱ����Ϊʹ��״̬
			break;
		}

		/* �ǿͻ�����Ҫ������ͻ���ʱ���͸����ڵ���Ϣ */
		case WM_NCCALCSIZE: {
			// ���ر߿�
			break;
		}

		/* �ڴ����Ͻ��л��Ʋ��� */
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			RECT rc;
			GetClientRect(hwnd, &rc);
			// ����һ����ɫ��ˢ����ѡ���豸��������
			HBRUSH hBrush = CreateSolidBrush(RGB(240, 240, 240));
			SelectObject(hdc, hBrush);
			// ���Ʋ˵�������
			RECT menuBarRect = { rc.left, rc.top, rc.right, rc.top + 32 };
			FillRect(hdc, &menuBarRect, hBrush);
			// �������
			HPEN hPen = CreatePen(PS_SOLID, 2, RGB(128, 128, 128));
			SelectObject(hdc, hPen);
			MoveToEx(hdc, rc.left, rc.top, NULL);
			LineTo(hdc, rc.right, rc.top);
			LineTo(hdc, rc.right, rc.bottom);
			LineTo(hdc, rc.left, rc.bottom);
			LineTo(hdc, rc.left, rc.top);
			// ���ơ�X����ť
			HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("System"));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(50, 50, 50));
			SetBkMode(hdc, TRANSPARENT);
			TextOut(hdc, rc.right - 24, rc.top + 10, "X", 1);
			// ���ơ�-����ť
			WCHAR minimizeButton[] = { 0x2013, '\0' }; // ʹ��Unicode�����ơ�-�����Ż����һЩ
			SetTextColor(hdc, RGB(50, 50, 50));
			SetBkMode(hdc, TRANSPARENT);
			TextOutW(hdc, rc.right - 60, rc.top + 10, minimizeButton, lstrlenW(minimizeButton));
			// ��ȡ����ͼ��
			HICON hIcon = (HICON)LoadImage(NULL, "��ݲ˵�.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
			if (hIcon != NULL) {
				// ����ͼ��λ��
				int iconWidth = 24; // ͼ��ĳ�
				int iconHeight = 24; // ͼ��Ŀ�
				int iconX = rc.left + 4;
				int iconY = rc.top + (32 - iconHeight) / 2;
				// ����ͼ��
				DrawIconEx(hdc, iconX, iconY, hIcon, iconWidth, iconHeight, 0, NULL, DI_NORMAL);
			}
			// ��ȡ���ڱ���
			TCHAR title[256];
			GetWindowText(hwnd, title, 256);
			// ���ƴ��ڱ���
			HFONT hTitleFont = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("΢���ź�"));
			SelectObject(hdc, hTitleFont);
			SetTextColor(hdc, RGB(0, 0, 0));
			SetBkMode(hdc, TRANSPARENT);
			TextOut(hdc, rc.left + 31, rc.top + 7, title, lstrlen(title));
			// ɾ����ˢ�����ʺ��������
			DeleteObject(hBrush);
			DeleteObject(hPen);
			DeleteObject(hFont);
			DeleteObject(hTitleFont);
			EndPaint(hwnd, &ps);
			break;
		}

		/* ������ڴ��ڵķǿͻ����ƶ�ʱ������������λ�� */
		case WM_NCHITTEST: {
			POINT pt = { LOWORD(lParam), HIWORD(lParam) };
			ScreenToClient(hwnd, &pt);
			RECT rc;
			GetClientRect(hwnd, &rc);
			// ����Ƿ����ˡ�X����ť
			if (pt.x > rc.right - 37 && pt.x < rc.right - 0 && pt.y > rc.top + 0 && pt.y < rc.top + 32) {
				return HTCLOSE;
			}
			// ����Ƿ����ˡ�-����ť
			if (pt.x > rc.right - 70 && pt.x < rc.right - 35 && pt.y > rc.top + 0 && pt.y < rc.top + 32) {
				return HTMINBUTTON;
			}
			// ����������λ�ã���������Ӧ��ֵ
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

		/* ������ڴ��ڵķǿͻ���������ʱ��ִ����Ӧ���� */
		case WM_NCLBUTTONUP: {
			if (wParam == HTCLOSE) {
				// �������ˡ�X����ť�����͹رմ��ڵ���Ϣ
				SendMessage(hwnd, WM_CLOSE, 0, 0);
				return 0;
			} else if (wParam == HTMINBUTTON) {
				// �������ˡ�-����ť��������С�����ڵ���Ϣ
				SendMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
				return 0;
			}
			break;
		}

		/* ������ڴ��ڵķǿͻ���������������ڱ������ϣ�ִ�����²��� */
		case WM_NCLBUTTONDOWN: {
			if (wParam == HTCAPTION) {
				SetForegroundWindow(hwnd); // ����������Ϊǰ̨����
				SendMessage(hwnd, WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0); // �����ƶ����ڵ���Ϣ
				return 0;
			}
			break;
		}

		/* ֪ͨ���ڵĻ״̬�����仯 */
		case WM_ACTIVATE: {
			if (wParam == WA_INACTIVE) {
				// ���˳���Ľ��㶪ʧʱ�����ر��������Է�Windows7ϵͳ�л���������������ԭ��������ʾ������
				SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_CAPTION); // ���ر�����
				SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER); // ʹ�ı���Ч
			}
			break;
		}

		/* ���þ�̬�ؼ��ı�����ɫ */
		case WM_CTLCOLORSTATIC: {
			HDC hdcStatic = (HDC)wParam; // ��ȡ��̬�ؼ����豸�����ľ��
			SetTextColor(hdcStatic, RGB(0, 0, 0)); // �����ı���ɫΪ��ɫ
			SetBkColor(hdcStatic, RGB(255, 255, 255)); // ���ñ�����ɫΪ����ɫ
			return (INT_PTR)CreateSolidBrush(RGB(255, 255, 255)); // ����һ������ɫ�Ļ�ˢ��Ϊ��̬�ؼ��ı���ɫ
		}

		/* ������ť */
		case WM_CREATE: { // ������ť
			A_Top = CreateWindow("button", "�ö��˳�������д���", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 25, 80, 170, 15, hwnd, (HMENU)00000, hInst, NULL);
			A_AOU = CreateWindow("button", "�Զ��������Ĵ洢�豸", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 215, 80, 185, 15, hwnd, (HMENU)857, hInst, NULL); // �Զ��������Ĵ洢�豸 = Auto Open Inserted USB
			A_text = CreateWindow("Static", "��ӭʹ�ã�", WS_CHILD | WS_VISIBLE, 190, 280, 228, 38, hwnd, NULL, hInst, NULL);
			A_text2 = CreateWindow("Static", "��ӭʹ�ã�", WS_CHILD | WS_VISIBLE, 3, 280, 186, 38, hwnd, NULL, hInst, NULL);
			A_text3 = CreateWindow("Static", "��ӭʹ�ÿ�ݲ˵�������Ч���ڴ�����ʺ��ִ��������˵���������ʹ��ϰ�ߡ���л����ʹ�ã�������ʹ�����¹��ܣ�", WS_CHILD | WS_VISIBLE, 8, 35, 408, 45, hwnd, NULL, hInst, NULL);
			A_1 = CreateWindow("button", "ʵ��վ���ݴ�", WS_CHILD | WS_VISIBLE | BS_FLAT, 10, 105, 140, 45, hwnd, (HMENU)1, hInst, NULL);
			A_2 = CreateWindow("button", "����ӳ�丨������", WS_CHILD | WS_VISIBLE | BS_FLAT, 160, 105, 140, 45, hwnd, (HMENU)2, hInst, NULL);
			A_3 = CreateWindow("button", "ʵ�ù�����", WS_CHILD | WS_VISIBLE | BS_FLAT, 310, 105, 100, 45, hwnd, (HMENU)3, hInst, NULL);
			A_999 = CreateWindow("button", "����", WS_CHILD | WS_VISIBLE | BS_FLAT, 360, 254, 50, 25, hwnd, (HMENU)999, hInst, NULL);
			A_Jtb = CreateWindow("button", "����������", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 10, 260, 90, 15, hwnd, NULL, hInst, NULL);
			A_4 = CreateWindow("button", "�򿪼�����", WS_CHILD | WS_VISIBLE | BS_FLAT, 110, 254, 90, 25, hwnd, (HMENU)1999, hInst, NULL);
			// ����һ��������ı�
			HFONT hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("΢���ź�"));
			// ���´���������Ӧ�����ı��ؼ�
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
			// ������ʱ��
			SetTimer(hwnd, 1, 500, NULL); // ��ʱ��ID: 1�����ʱ��: 500���루0.5�룩
			SetTimer(hwnd, 2, 10000, NULL); // ��ʱ��ID: 2�����ʱ��: 10000���루10�룩
			SetTimer(hwnd, 3, 0, NULL); // ��ʱ��ID: 3�����ʱ��: 0���루0�룩
			SetTimer(hwnd, 4, 0, NULL); // ��ʱ��ID: 4�����ʱ��: 0���루0�룩
			// ���ѡ����ı�
			std::string motivationalTexts[] = {
				"	           ���ӿ��룬���ӿڳ�",      // 1
				"	           ����Ī˵����ˮ����",      // 2
				"	           ֪�㳣�֣�������",      // 3
				"	           �˸����ģ��ĸ��м�",      // 4
				"	           ����Ī˵�����²�֪",      // 5
				"	           �˹��������������",      // 6
				"	           ��˼���У���˼����",      // 7
				"	           �����Ƽ�����������",      // 8
				"	           �������̣���������",      // 9
				"	    �Ƿ������У�������Ȼ��",         // 10
				"	    �����ؿ��գ�����������",         // 11
				"	    �ᵱ�������һ����ɽС",         // 12
				"           ���˲���ò�࣬��ˮ���ɶ���", // 13
				"       ��֮��Ȼ��ʧ̹֮Ȼ��˳����Ȼ",   // 14
				"    ��һ�䣬Ϣһŭ����һ�ţ���һ��",    // 15
				"    �ع��ѹ��عع���ǰ·������Ӳ�",    // 16
				"    �˵��������ƽ���ƫ����ʱ���괺",    // 17
				"    ������ʱ�����У�������ʱĪǿ��",    // 18
				"    �����������ǩ��������˼������",    // 19
				"    ���ư��Ƴ����ڣ�������ˮ�ζ���"     // 20
			};
			std::srand(static_cast<unsigned int>(std::time(nullptr))); // �������������
			int randomIndex = std::rand() % (sizeof(motivationalTexts) / sizeof(motivationalTexts[0]));
			// �������ı��ϲ�Ϊһ���ַ���
			std::string usageStr = "		           �汾��2.52\n" + motivationalTexts[randomIndex];
			// ���»�ӭ�ı���ʾ���ı�
			SetWindowText(A_text, usageStr.c_str());
			break;
		}

		/* ��ʱ�� */
		case WM_TIMER: {
			if (wParam == 2) { // �жϼ�ʱ��ID
				// ���ѡ����ı�
				std::string motivationalTexts[] = {
					"	           ���ӿ��룬���ӿڳ�",      // 1
					"	           ����Ī˵����ˮ����",      // 2
					"	           ֪�㳣�֣�������",      // 3
					"	           �˸����ģ��ĸ��м�",      // 4
					"	           ����Ī˵�����²�֪",      // 5
					"	           �˹��������������",      // 6
					"	           ��˼���У���˼����",      // 7
					"	           �����Ƽ�����������",      // 8
					"	           �������̣���������",      // 9
					"	    �Ƿ������У�������Ȼ��",         // 10
					"	    �����ؿ��գ�����������",         // 11
					"	    �ᵱ�������һ����ɽС",         // 12
					"           ���˲���ò�࣬��ˮ���ɶ���", // 13
					"       ��֮��Ȼ��ʧ̹֮Ȼ��˳����Ȼ",   // 14
					"    ��һ�䣬Ϣһŭ����һ�ţ���һ��",    // 15
					"    �ع��ѹ��عع���ǰ·������Ӳ�",    // 16
					"    �˵��������ƽ���ƫ����ʱ���괺",    // 17
					"    ������ʱ�����У�������ʱĪǿ��",    // 18
					"    �����������ǩ��������˼������",    // 19
					"    ���ư��Ƴ����ڣ�������ˮ�ζ���"     // 20
				};
				std::srand(static_cast<unsigned int>(std::time(nullptr))); // �������������
				int randomIndex = std::rand() % (sizeof(motivationalTexts) / sizeof(motivationalTexts[0]));

				// �������ı��ϲ�Ϊһ���ַ���
				std::string usageStr = "		           �汾��2.52\n" + motivationalTexts[randomIndex];

				// ���»�ӭ�ı���ʾ���ı�
				SetWindowText(A_text, usageStr.c_str());
			}

			if (wParam == 1) { // �жϼ�ʱ��ID
				// ��ȡ��ǰCPUռ��
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

				// ��ȡ��ǰ�ڴ�ռ��
				MEMORYSTATUSEX memStatus;
				memStatus.dwLength = sizeof(memStatus);
				GlobalMemoryStatusEx(&memStatus);
				double memoryUsage = static_cast<double>(memStatus.ullTotalPhys - memStatus.ullAvailPhys);
				double totalMemory = static_cast<double>(memStatus.ullTotalPhys);
				double memoryUsagePercentage = (memoryUsage / totalMemory) * 100;

				// ��ȡ��ǰ�����ڴ�ռ��
				MEMORYSTATUSEX xunimemStatus;
				memStatus.dwLength = sizeof(memStatus);
				GlobalMemoryStatusEx(&memStatus);
				double xunimemoryUsage = static_cast<double>(memStatus.ullTotalPageFile - memStatus.ullAvailPageFile);
				double xunitotalMemory = static_cast<double>(memStatus.ullTotalPageFile);
				double xunimemoryUsagePercentage = (xunimemoryUsage / xunitotalMemory) * 100;

				// �������ı��ϲ�Ϊһ���ַ���
				std::string usageStr = "Ӳ��ʹ���ʣ�CPU��" + std::to_string(static_cast<int>(cpuUsage)) + "%\n�ڴ棺" + std::to_string(static_cast<int>(memoryUsagePercentage)) + "%" + "  �����ڴ棺" + std::to_string(static_cast<int>(xunimemoryUsagePercentage)) + "%";

				// ���»�ӭ2�ı���ʾ���ı�
				SetWindowText(A_text2, usageStr.c_str());
			}

			if (wParam == 3) { // �жϼ�ʱ��ID
				// Ŀ���������
				std::string targetProcessName = "��ݲ˵�.exe";
				int count = 0;

				// ��ȡ���̿��վ��
				HANDLE hProcessSnap;
				PROCESSENTRY32 pe32;
				hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

				// �����̿��վ���Ƿ���Ч
				if (hProcessSnap == INVALID_HANDLE_VALUE) {
					// ���������Ϣ
					std::cout << "CreateToolhelp32Snapshot ʧ��" << std::endl; // ͨ��������Ȩ�޲������ϵͳ��Դ���㵼�µġ�����������ڴ���ϵͳ���գ�����޷���ȡϵͳ���գ����޷����������б�
					return 1;
				}

				// ���ýṹ���С����ȡ��һ��������Ϣ
				pe32.dwSize = sizeof(PROCESSENTRY32);
				if (!Process32First(hProcessSnap, &pe32)) {
					// ���������Ϣ
					std::cout << "Process32First ʧ��" << std::endl; // �����������޷���ȡ��һ�����̵���Ϣ��ͨ��������ϵͳ���ղ���ȷ������Ч���µġ�����������ڻ�ȡϵͳ�����еĵ�һ��������Ϣ
					CloseHandle(hProcessSnap);
					return 1;
				}

				// ���������б�ͳ��Ŀ�����ʵ������
				do {
					if (std::string(pe32.szExeFile) == targetProcessName) {
						count++;
					}
				} while (Process32Next(hProcessSnap, &pe32));

				// �رս��̿��վ��
				CloseHandle(hProcessSnap);

				// ����Ŀ�����ʵ������������
				if (count > 1) {
					std::cout << "�ж��Ŀ����̵�ʵ���������У�Ŀ����̴���1��" << std::endl;
					// ��ʾ���ں��ö���ȡ���ö����������û�
					ShowWindow(host_hwnd, SW_RESTORE); // ��ʾ������
					ShowWindow(hwnd_exit, SW_RESTORE); // ��ʾ�˳�����
					ShowWindow(hwnd_Settings, SW_RESTORE); // ��ʾ���ô���
					ShowWindow(hwnd_website, SW_RESTORE); // ��ʾ��վ����
					ShowWindow(hwnd_mapping, SW_RESTORE); // ��ʾӳ�䴰��
					ShowWindow(hwnd_iTools, SW_RESTORE); // ��ʾ�����䴰��
					SetWindowPos(host_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // �ö�������
					SetWindowPos(hwnd_exit, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // �ö��˳�����
					SetWindowPos(hwnd_Settings, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // �ö����ô���
					SetWindowPos(hwnd_website, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // �ö���վ����
					SetWindowPos(hwnd_mapping, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // �ö�ӳ�䴰��
					SetWindowPos(hwnd_iTools, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // �ö������䴰��

					if (SendMessage(A_Top, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� �ö� ��ѡ���Ƿ�δ����ѡ
						SetWindowPos(host_hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // ȡ���ö�������
						SetWindowPos(hwnd_exit, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // ȡ���ö��˳�����
						SetWindowPos(hwnd_Settings, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // ȡ���ö����ô���
						SetWindowPos(hwnd_website, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // ȡ���ö���վ����
						SetWindowPos(hwnd_mapping, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // ȡ���ö�ӳ�䴰��
						SetWindowPos(hwnd_iTools, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // ȡ���ö������䴰��
					}
				} else {
					std::cout << "û���ظ���Ŀ�����ʵ���������У�Ŀ�����С�ڵ���1��" << std::endl;
				}
			}

			if (wParam == 4) { // �жϼ�ʱ��ID
				if (SendMessage(A_Jtb, BM_GETCHECK, 0, 0) == BST_CHECKED) { // ���������� ��ѡ��ѡ��
					// ���Ctrl���Ƿ񱻰���
					bool ctrlPressed = GetAsyncKeyState(VK_CONTROL) & 0x8000;
					// ���X���Ƿ񱻰���
					bool xPressed = GetAsyncKeyState(0x58) & 0x8000; // 0x58 ��X�����������
					// ���C���Ƿ񱻰���
					bool cPressed = GetAsyncKeyState(0x43) & 0x8000; // 0x43 ��C�����������
					// �ж�Ctrl+X��Ctrl+C�Ƿ�ͬʱ����
					if (ctrlPressed && (xPressed || cPressed)) {
						CtrlxhuoCtrlC++; // ���������ӣ���ʾ�Ѱ���
						// �ж�ȫ�ֱ�����ֵ
						if (CtrlxhuoCtrlC == 1) { // �������Ϊ1��ʾ��һʱ�̰��µĲ���
							std::cout << "Ctrl+X��Ctrl+C�����£�" << std::endl;
							// �򿪼�����
							if (OpenClipboard(nullptr)) {
								// ��ȡ�������е��ı�����
								HANDLE hData = GetClipboardData(CF_TEXT);
								if (hData != nullptr) {
									// �����ڴ�鲢��ȡ�ı�����
									char* pszText = static_cast<char*>(GlobalLock(hData));
									if (pszText != nullptr) {
										// ���ı�����ת��Ϊ�ַ���
										std::string clipboardText(pszText);
										// �ͷ��ڴ�鲢�رռ�����
										GlobalUnlock(hData);
										CloseClipboard();
										// д���ı����ݵ��ļ�
										std::ofstream file("������.txt", std::ios_base::app); // ���ļ�����׷��д��
										if (file.is_open()) {
											file << clipboardText << std::endl; // д���ı�������
											file.close(); // �ر��ļ�
											std::cout << "������д�뵽�ļ� ������.txt ��" << std::endl;
										} else {
											std::cerr << "�޷����ļ�" << std::endl;
										}
									} else {
										std::cerr << "�޷���ȡ����������" << std::endl;
									}
								} else {
									std::cerr << "�޷���ȡ����������" << std::endl;
								}
							} else {
								std::cerr << "�޷��򿪼�����" << std::endl;
							}
						}
					} else {
						CtrlxhuoCtrlC = 0; // ��������Ϊ1����ʾ�Ѱ���
					}
				}
			}
			break;
		}

		case WM_COMMAND: { // ����ť����¼�
			switch (wParam) {

				case 857: { // �Զ��������Ĵ洢�豸
					if (SendMessage(A_AOU, BM_GETCHECK, 0, 0) == BST_CHECKED) { // ��ѡ��ѡ��
						hThread = CreateThread(NULL, 0, DriveDetection, NULL, 0, NULL); // �����߳���ִ��DriveDetection����
						if (hThread == NULL) { // ����߳̾��Ϊ��
							MessageBox(hwnd, "�޷������̣߳�", "����", MB_OK | MB_ICONERROR); // ��ʾ������Ϣ��
						}
					} else if (SendMessage(A_AOU, BM_GETCHECK, 0, 0) != BST_CHECKED) { // ��ѡ��δ��ѡ��
						if (hThread != NULL) { // ����߳̾����Ϊ��
							TerminateThread(hThread, 0); // ��ֹ�߳�
							CloseHandle(hThread); // �ر��߳̾��
							hThread = NULL;
						}
					}
					break;
				}

				case 00000: { // �ö�����
					if (SendMessage(A_Top, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �ж� �ö� ��ѡ���Ƿ񱻹�ѡ
						// ��ѡ
						// �ж�ȫ�ֱ�����ֵ
						if (strongTopChecked == 0) {
							// ȫ�ֱ�����ֵΪ0ʱ�Ĳ���
							SetWindowPos(host_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // �ö�������
							SetWindowPos(hwnd_exit, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // �ö��˳�����
							SetWindowPos(hwnd_Settings, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // �ö����ô���
							SetWindowPos(hwnd_website, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // �ö���վ����
							SetWindowPos(hwnd_mapping, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // �ö�ӳ�䴰��
							SetWindowPos(hwnd_iTools, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // �ö������䴰��
						} else if (strongTopChecked == 1) {
							// ȫ�ֱ�����ֵΪ1ʱ�Ĳ���
							tThread = CreateThread(NULL, 0, TopMostThread, NULL, 0, NULL); // �����߳���ִ��TopMostThread����
							if (tThread == NULL) { // ����߳̾��Ϊ��
								MessageBox(hwnd, "�޷������̣߳�", "����", MB_OK | MB_ICONERROR); // ��ʾ������Ϣ��
							}
						}
					} else {
						// δ��ѡ
						// �ж�ȫ�ֱ�����ֵ
						if (strongTopChecked == 0) {
							// ȫ�ֱ�����ֵΪ0ʱ�Ĳ���
							SetWindowPos(host_hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // ȡ���ö�������
							SetWindowPos(hwnd_exit, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // ȡ���ö��˳�����
							SetWindowPos(hwnd_Settings, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // ȡ���ö����ô���
							SetWindowPos(hwnd_website, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // ȡ���ö���վ����
							SetWindowPos(hwnd_mapping, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // ȡ���ö�ӳ�䴰��
							SetWindowPos(hwnd_iTools, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // ȡ���ö������䴰��
						} else if (strongTopChecked == 1) {
							// ȫ�ֱ�����ֵΪ1ʱ�Ĳ���
							if (tThread != NULL) { // ����߳̾����Ϊ��
								TerminateThread(tThread, 0); // ��ֹ�߳�
								CloseHandle(tThread); // �ر��߳̾��
								tThread = NULL;
								SetWindowPos(host_hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // ȡ���ö�������
								SetWindowPos(hwnd_exit, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // ȡ���ö��˳�����
								SetWindowPos(hwnd_Settings, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // ȡ���ö����ô���
								SetWindowPos(hwnd_website, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // ȡ���ö���վ����
								SetWindowPos(hwnd_mapping, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // ȡ���ö�ӳ�䴰��
								SetWindowPos(hwnd_iTools, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // ȡ���ö������䴰��
							}
						}
					}
					break;
				}

				case 999: { // ���ࣨ���ã�
					if (isSettingsWindowOpen == 0) { // ����Ƿ��Ѿ����ˡ����ࡱ�Ӵ���
						/* ʹ����ʼ�ճ�������Ļ�м� */
						// ��ȡ��Ļ��Ⱥ͸߶�
						int screenWidth = GetSystemMetrics(SM_CXSCREEN);
						int screenHeight = GetSystemMetrics(SM_CYSCREEN);
						// ���㴰�����Ͻ�����
						int windowWidth = 350; // ���ڿ��
						int windowHeight = 375; // ���ڸ߶�
						int windowX = (screenWidth - windowWidth) / 2;
						int windowY = (screenHeight - windowHeight) / 2;

						// ��ť�����ʱ������ ���� �Ӵ���
						hwnd_Settings = CreateWindowEx(WS_EX_APPWINDOW, "SettingsWindowClass", "����|��^*)", WS_VISIBLE | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, windowX, windowY, windowWidth, windowHeight, NULL, NULL, GetModuleHandle(NULL), NULL);
						ShowWindow(hwnd_Settings, SW_SHOW);
						isSettingsWindowOpen = 1; // ��������Ϊ1����ʾ�����ࡱ�Ӵ����Ѵ�
					} else {
						//MessageBox(hwnd, "�����Ѿ�������Ŷ��(*'��'*)\n�رյ�������ʾ", "��ʾ=-=", MB_OK | MB_ICONASTERISK);
						// ��ʾ���ں��ö���ȡ���ö����������û�
						ShowWindow(hwnd_Settings, SW_RESTORE);
						SetWindowPos(hwnd_Settings, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
						SetWindowPos(hwnd_Settings, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
					}
					break;
				}

				case 1999: { // �򿪼�����
					HINSTANCE result = ShellExecute(NULL, "open", "notepad.exe", "������.txt", NULL, SW_SHOWNORMAL);
					break;
				}

				case 1: { // ������վ��ݴ�
					if (isWebsiteWindowOpen == 0) { // ����Ƿ��Ѿ����ˡ�վ�㡱�Ӵ���
						/* ʹ����ʼ�ճ�������Ļ�м� */
						// ��ȡ��Ļ��Ⱥ͸߶�
						int screenWidth = GetSystemMetrics(SM_CXSCREEN);
						int screenHeight = GetSystemMetrics(SM_CYSCREEN);
						// ���㴰�����Ͻ�����
						int windowWidth = 480; // ���ڿ��
						int windowHeight = 365; // ���ڸ߶�
						int windowX = (screenWidth - windowWidth) / 2;
						int windowY = (screenHeight - windowHeight) / 2;

						// ��ť�����ʱ������ ������վ��ݴ� �Ӵ���
						hwnd_website = CreateWindowEx(WS_EX_APPWINDOW, "ChildWindowClass", "ʵ��վ���ݴ�(^_-)��", WS_VISIBLE | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, windowX, windowY, windowWidth, windowHeight, NULL, NULL, GetModuleHandle(NULL), NULL);
						ShowWindow(hwnd_website, SW_SHOW);
						isWebsiteWindowOpen = 1; // ��������Ϊ1����ʾ��վ�㡱�Ӵ����Ѵ�
					} else {
						//MessageBox(hwnd, "�����Ѿ�������Ŷ��(*'��'*)\n�رյ�������ʾ", "��ʾ=-=", MB_OK | MB_ICONASTERISK);
						// ��ʾ���ں��ö���ȡ���ö����������û�
						ShowWindow(hwnd_website, SW_RESTORE);
						SetWindowPos(hwnd_website, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
						SetWindowPos(hwnd_website, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
					}
					break;
				}

				case 2: { // ����ӳ�丨������
					if (isMappingWindowOpen == 0) { // ����Ƿ��Ѿ����ˡ�ӳ�䡱�Ӵ���
						/* ʹ����ʼ�ճ�������Ļ�м� */
						// ��ȡ��Ļ��Ⱥ͸߶�
						int screenWidth = GetSystemMetrics(SM_CXSCREEN);
						int screenHeight = GetSystemMetrics(SM_CYSCREEN);
						// ���㴰�����Ͻ�����
						int windowWidth = 505; // ���ڿ��
						int windowHeight = 345; // ���ڸ߶�
						int windowX = (screenWidth - windowWidth) / 2;
						int windowY = (screenHeight - windowHeight) / 2;

						// ��ť�����ʱ������ ����ӳ�丨������ �Ӵ���
						hwnd_mapping = CreateWindowEx(WS_EX_APPWINDOW, childwindow, "����ӳ�丨������(��^��^��) ", WS_VISIBLE | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, windowX, windowY, windowWidth, windowHeight, NULL, NULL, GetModuleHandle(NULL), NULL);
						ShowWindow(hwnd_mapping, SW_SHOW);
						isMappingWindowOpen = 1; // ��������Ϊ1����ʾ��ӳ�䡱�Ӵ����Ѵ�
					} else {
						//MessageBox(hwnd, "�����Ѿ�������Ŷ��(*'��'*)\n�رյ�������ʾ", "��ʾ=-=", MB_OK | MB_ICONASTERISK);
						// ��ʾ���ں��ö���ȡ���ö����������û�
						ShowWindow(hwnd_mapping, SW_RESTORE);
						SetWindowPos(hwnd_mapping, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
						SetWindowPos(hwnd_mapping, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
					}
					break;
				}

				case 3: { // ʵ�ù�����
					if (isiToolsWindowOpen == 0) { // ����Ƿ��Ѿ����ˡ������䡱�Ӵ���
						/* ʹ����ʼ�ճ�������Ļ�м� */
						// ��ȡ��Ļ��Ⱥ͸߶�
						int screenWidth = GetSystemMetrics(SM_CXSCREEN);
						int screenHeight = GetSystemMetrics(SM_CYSCREEN);
						// ���㴰�����Ͻ�����
						int windowWidth = 445; // ���ڿ��
						int windowHeight = 350; // ���ڸ߶�
						int windowX = (screenWidth - windowWidth) / 2;
						int windowY = (screenHeight - windowHeight) / 2;

						// ��ť�����ʱ������ ʵ�ù����� �Ӵ���
						hwnd_iTools = CreateWindowEx(WS_EX_APPWINDOW, "iToolsWindowClass", "ʵ�ù�����(��^��^) ", WS_VISIBLE | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, windowX, windowY, windowWidth, windowHeight, NULL, NULL, GetModuleHandle(NULL), NULL);
						ShowWindow(hwnd_iTools, SW_SHOW);
						isiToolsWindowOpen = 1; // ��������Ϊ1����ʾ�������䡱�Ӵ����Ѵ�
					} else {
						//MessageBox(hwnd, "�����Ѿ�������Ŷ��(*'��'*)\n�رյ�������ʾ", "��ʾ=-=", MB_OK | MB_ICONASTERISK);
						// ��ʾ���ں��ö���ȡ���ö����������û�
						ShowWindow(hwnd_iTools, SW_RESTORE);
						SetWindowPos(hwnd_iTools, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
						SetWindowPos(hwnd_iTools, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
					}
					break;
				}

				case IDM_EXIT: {
					// �����˳���ť
					PostQuitMessage(0); // ���û��������˳�����ʱ�����ڽ�һֱ����Ϊʹ��״̬
					DestroyWindow(hwnd);
					break;
				}

				/* ����������Ϣ�����кࣩܶ��ʹ��Ĭ�Ϲ��̴��� */
				default: {
					DefWindowProc(hwnd, Message, wParam, lParam);
					break;
				}
			}
			break;
		}

		/* ����ͼ����Ϣ���� */
		case WM_USER + 1: {
			switch (lParam) {
				// ��������������ͼ��
				case WM_LBUTTONDOWN: {
					// ��ʾ����
					ShowWindow(hwnd, SW_RESTORE);
					SetForegroundWindow(hwnd);
					// ��ʾ�����á�����
					ShowWindow(hwnd_Settings, SW_SHOW);
					// ��ʾ����վ������
					ShowWindow(hwnd_website, SW_SHOW);
					// ��ʾ��ӳ�䡱����
					ShowWindow(hwnd_mapping, SW_SHOW);
					// ���ء������䡱����
					ShowWindow(hwnd_iTools, SW_SHOW);
					break;
				}
				// ����Ҽ���������ͼ��
				case WM_RBUTTONDOWN: {
					POINT pt;
					GetCursorPos(&pt);
					// �����Ҽ��˵�
					HMENU hMenu = CreatePopupMenu();
					InsertMenu(hMenu, -1, MF_BYPOSITION, IDM_EXIT, "�˳�");
					// ��ʾ�Ҽ��˵�
					SetForegroundWindow(hwnd);
					TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
					DestroyMenu(hMenu);
					break;
				}
			}
			break;
		}

		/* ����������Ϣ���ࣩܶ��ʹ��Ĭ�ϳ����� */
		default: {
			return DefWindowProc(hwnd, Message, wParam, lParam);
		}
	}
	return 0;
}

/* �˺������� �˳� �Ӵ��ڵ���Ϣ */
LRESULT CALLBACK exitWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {

		// �����Ӵ��ڵĹر���Ϣ
		case WM_CLOSE: {
			// �رմ���
			DestroyWindow(hwnd);
			break;
		}

		/* ������ʱ���������߳�ֹͣ */
		case WM_DESTROY: { // ����ʱ
			isExitWindowOpen = 0; // ��������Ϊ0����ʾ���˳����Ӵ����ѹر�
			break;
		}

		/* �ǿͻ�����Ҫ������ͻ���ʱ���͸����ڵ���Ϣ */
		case WM_NCCALCSIZE: {
			// ���ر߿�
			break;
		}

		/* �ڴ����Ͻ��л��Ʋ��� */
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			RECT rc;
			GetClientRect(hwnd, &rc);
			// ����һ����ɫ��ˢ����ѡ���豸��������
			HBRUSH hBrush = CreateSolidBrush(RGB(240, 240, 240));
			SelectObject(hdc, hBrush);
			// ���Ʋ˵�������
			RECT menuBarRect = { rc.left, rc.top, rc.right, rc.top + 32 };
			FillRect(hdc, &menuBarRect, hBrush);
			// �������
			HPEN hPen = CreatePen(PS_SOLID, 2, RGB(128, 128, 128));
			SelectObject(hdc, hPen);
			MoveToEx(hdc, rc.left, rc.top, NULL);
			LineTo(hdc, rc.right, rc.top);
			LineTo(hdc, rc.right, rc.bottom);
			LineTo(hdc, rc.left, rc.bottom);
			LineTo(hdc, rc.left, rc.top);
			// ���ơ�X����ť
			HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("System"));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(50, 50, 50));
			SetBkMode(hdc, TRANSPARENT);
			TextOut(hdc, rc.right - 24, rc.top + 10, "X", 1);
			// ��ȡ����ͼ��
			HICON hIcon = (HICON)LoadImage(NULL, "��ݲ˵�.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
			if (hIcon != NULL) {
				// ����ͼ��λ��
				int iconWidth = 24; // ͼ��ĳ�
				int iconHeight = 24; // ͼ��Ŀ�
				int iconX = rc.left + 4;
				int iconY = rc.top + (32 - iconHeight) / 2;
				// ����ͼ��
				DrawIconEx(hdc, iconX, iconY, hIcon, iconWidth, iconHeight, 0, NULL, DI_NORMAL);
			}
			// ��ȡ���ڱ���
			TCHAR title[256];
			GetWindowText(hwnd, title, 256);
			// ���ƴ��ڱ���
			HFONT hTitleFont = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("΢���ź�"));
			SelectObject(hdc, hTitleFont);
			SetTextColor(hdc, RGB(0, 0, 0));
			SetBkMode(hdc, TRANSPARENT);
			TextOut(hdc, rc.left + 31, rc.top + 7, title, lstrlen(title));
			// ɾ����ˢ�����ʺ��������
			DeleteObject(hBrush);
			DeleteObject(hPen);
			DeleteObject(hFont);
			DeleteObject(hTitleFont);
			EndPaint(hwnd, &ps);
			break;
		}

		/* ������ڴ��ڵķǿͻ����ƶ�ʱ������������λ�� */
		case WM_NCHITTEST: {
			POINT pt = { LOWORD(lParam), HIWORD(lParam) };
			ScreenToClient(hwnd, &pt);
			RECT rc;
			GetClientRect(hwnd, &rc);
			// ����Ƿ����ˡ�X����ť
			if (pt.x > rc.right - 37 && pt.x < rc.right - 0 && pt.y > rc.top + 0 && pt.y < rc.top + 32) {
				return HTCLOSE;
			}
			// ����������λ�ã���������Ӧ��ֵ
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

		/* ������ڴ��ڵķǿͻ���������ʱ��ִ����Ӧ���� */
		case WM_NCLBUTTONUP: {
			if (wParam == HTCLOSE) {
				// �������ˡ�X����ť�����͹رմ��ڵ���Ϣ
				SendMessage(hwnd, WM_CLOSE, 0, 0);
				return 0;
			}
			break;
		}

		/* ������ڴ��ڵķǿͻ���������������ڱ������ϣ�ִ�����²��� */
		case WM_NCLBUTTONDOWN: {
			if (wParam == HTCAPTION) {
				SetForegroundWindow(hwnd); // ����������Ϊǰ̨����
				SendMessage(hwnd, WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0); // �����ƶ����ڵ���Ϣ
				return 0;
			}
			break;
		}

		/* ֪ͨ���ڵĻ״̬�����仯 */
		case WM_ACTIVATE: {
			if (wParam == WA_INACTIVE) {
				// ���˳���Ľ��㶪ʧʱ�����ر��������Է�Windows7ϵͳ�л���������������ԭ��������ʾ������
				SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_CAPTION); // ���ر�����
				SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER); // ʹ�ı���Ч
			}
			break;
		}

		/* ���þ�̬�ؼ��ı�����ɫ */
		case WM_CTLCOLORSTATIC: {
			HDC hdcStatic = (HDC)wParam; // ��ȡ��̬�ؼ����豸�����ľ��
			SetTextColor(hdcStatic, RGB(0, 0, 0)); // �����ı���ɫΪ��ɫ
			SetBkColor(hdcStatic, RGB(255, 255, 255)); // ���ñ�����ɫΪ����ɫ
			return (INT_PTR)CreateSolidBrush(RGB(255, 255, 255)); // ����һ������ɫ�Ļ�ˢ��Ϊ��̬�ؼ��ı���ɫ
		}

		/* ������ť */
		case WM_CREATE: { // ������ť
			if (SendMessage(A_Top, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �ж� �ö� ��ѡ���Ƿ񱻹�ѡ
				// ��ѡ
				SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // �ö�
			}
			HWND htext = CreateWindow("Static", "���ոյ���˹رհ�ť����������һ���Ĳ����ǣ�", WS_CHILD | WS_VISIBLE, 8, 32, 230, 40, hwnd, NULL, hInst, NULL);
			HWND hHideToTrayCheckbox = CreateWindow("button", "���ص�������", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 68, 72, 120, 20, hwnd, (HMENU)1, NULL, NULL);
			HWND hExitProgramCheckbox = CreateWindow("button", "�˳�����", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 68, 92, 120, 20, hwnd, (HMENU)2, NULL, NULL);
			HWND hNoReminderCheckbox = CreateWindow("button", "��������,�´��˳�ʱ�Զ�ѡ��", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 18, 110, 210, 20, hwnd, (HMENU)3, NULL, NULL);
			HWND determinebutton = CreateWindow("button", "ȷ��", WS_CHILD | WS_VISIBLE | BS_FLAT, 28, 132, 80, 35, hwnd, (HMENU)4, hInst, NULL);
			HWND cancellationbutton = CreateWindow("button", "ȡ��", WS_CHILD | WS_VISIBLE | BS_FLAT, 132, 132, 80, 35, hwnd, (HMENU)5, hInst, NULL);
			// ����һ��������ı�
			HFONT hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("΢���ź�"));
			// ���´���������Ӧ�����ı��ؼ�
			SendMessage(htext, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(hHideToTrayCheckbox, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(hExitProgramCheckbox, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(hNoReminderCheckbox, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(determinebutton, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(cancellationbutton, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			// ���data.json�ļ��Ƿ���ڣ��������ݣ�
			std::ifstream file("data.json");
			if (!file) {
				// data.json�ļ������ڣ�������
				std::ofstream newFile("data.json");
				if (!newFile) {
					MessageBox(NULL, "�޷�������������JSON�ļ���", "����", MB_ICONEXCLAMATION | MB_OK);
					return 0;
				}
				// ���������д���ʼ��data.json���ݵ��´������ļ���
				newFile << "*���ļ��洢��ݲ˵��������������";
				newFile.close();
				// ����ļ����Ƿ������Ӧ��ֵ
				std::ifstream file("data.json");
				if (!file) {
					MessageBox(NULL, "�޷���data.json�ļ���", "����", MB_ICONEXCLAMATION | MB_OK);
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
				// ����"...3"�Ĵ����߼�
				// ���� ���ص������� ��ѡ���ѡ��״̬Ϊѡ��
				SendMessage(hHideToTrayCheckbox, BM_SETCHECK, BST_CHECKED, 0);
			} else  if (found4) {
				// ����"...4"�Ĵ����߼�
				// ���� �˳����� ��ѡ���ѡ��״̬Ϊѡ��
				SendMessage(hExitProgramCheckbox, BM_SETCHECK, BST_CHECKED, 0);
			} else {
				// ���� ���ص������� ��ѡ���ѡ��״̬Ϊѡ��
				SendMessage(hHideToTrayCheckbox, BM_SETCHECK, BST_CHECKED, 0);
			}
			break;
		}

		case WM_COMMAND: { // ����ť����¼�
			switch (wParam) {

				case 1 : { // ���ص�������
					// ���data.json�ļ��Ƿ���ڣ��������ݣ�
					std::ifstream file("data.json");
					if (!file) {
						// data.json�ļ������ڣ�������
						std::ofstream newFile("data.json");
						if (!newFile) {
							MessageBox(NULL, "�޷�������������JSON�ļ���", "����", MB_ICONEXCLAMATION | MB_OK);
							return 0;
						}
						// ���������д���ʼ��data.json���ݵ��´������ļ���
						newFile << "*���ļ��洢��ݲ˵��������������\nNever notify = 3";
						newFile.close();
						// ����ļ����Ƿ������Ӧ��ֵ
						std::ifstream file("data.json");
						if (!file) {
							MessageBox(NULL, "�޷���data.json�ļ���", "����", MB_ICONEXCLAMATION | MB_OK);
							return 0;
						}
					} else {
						//����ļ�����
						std::ofstream file("data.json", std::ios::trunc);
						file.close();
						// д��
						std::ofstream newFile("data.json");
						newFile << "*���ļ��洢��ݲ˵��������������\nNever notify = 3";
						newFile.close();
					}
					break;
				}

				case 2 : { // �˳�����
					// ���data.json�ļ��Ƿ���ڣ��������ݣ�
					std::ifstream file("data.json");
					if (!file) {
						// data.json�ļ������ڣ�������
						std::ofstream newFile("data.json");
						if (!newFile) {
							MessageBox(NULL, "�޷�������������JSON�ļ���", "����", MB_ICONEXCLAMATION | MB_OK);
							return 0;
						}
						// ���������д���ʼ��data.json���ݵ��´������ļ���
						newFile << "*���ļ��洢��ݲ˵��������������\nNever notify = 4";
						newFile.close();
						// ����ļ����Ƿ������Ӧ��ֵ
						std::ifstream file("data.json");
						if (!file) {
							MessageBox(NULL, "�޷���data.json�ļ���", "����", MB_ICONEXCLAMATION | MB_OK);
							return 0;
						}
					} else {
						//����ļ�����
						std::ofstream file("data.json", std::ios::trunc);
						file.close();
						// д��
						std::ofstream newFile("data.json");
						newFile << "*���ļ��洢��ݲ˵��������������\nNever notify = 4";
						newFile.close();
					}
					break;
				}

				case 3 : { // ��������...
					HWND hNoReminderCheckbox = GetDlgItem(hwnd, 3); // ��ȡ ��������... ��ѡ��ľ��
					if (SendMessage(hNoReminderCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �ж� ��������... ��ѡ���Ƿ񱻹�ѡ
						// ��ѡ
						MessageBox(hwnd_exit, "�´��˳�ʱ���Զ�ѡ���粻���Զ�ѡ����������и���", "��ʾ(*^��^*)", MB_OK | MB_ICONINFORMATION);
					}
					break;
				}

				case 4 : { // ȷ��
					HWND hNoReminderCheckbox = GetDlgItem(hwnd, 3); // ��ȡ ��������... ��ѡ��ľ��
					if (SendMessage(hNoReminderCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �ж� ��������... ��ѡ���Ƿ񱻹�ѡ
						// ��ѡ
						// ���data.json�ļ��Ƿ���ڣ��������ݣ�
						std::ifstream file("data.json");
						if (!file) {
							// data.json�ļ������ڣ�������
							std::ofstream newFile("data.json");
							if (!newFile) {
								MessageBox(NULL, "�޷�������������JSON�ļ���", "����", MB_ICONEXCLAMATION | MB_OK);
								return 0;
							}
							// ���������д���ʼ��data.json���ݵ��´������ļ���
							newFile << "*���ļ��洢��ݲ˵��������������";
							newFile.close();
							// ����ļ����Ƿ������Ӧ��ֵ
							std::ifstream file("data.json");
							if (!file) {
								MessageBox(NULL, "�޷���data.json�ļ���", "����", MB_ICONEXCLAMATION | MB_OK);
								return 0;
							}
						}
						HWND hHideToTrayCheckbox = GetDlgItem(hwnd, 1); // ��ȡ�����ص�����������ѡ��ľ��
						HWND hExitProgramCheckbox = GetDlgItem(hwnd, 2); // ��ȡ���˳����򡱵�ѡ��ľ��
						if (SendMessage(hHideToTrayCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) {
							// �û�ѡ���ˡ����ص���������
							//����ļ�����
							std::ofstream file("data.json", std::ios::trunc);
							file.close();
							// д��
							std::ofstream newFile("data.json");
							newFile << "*���ļ��洢��ݲ˵��������������\nNever notify = 1";
							newFile.close();
						} else if (SendMessage(hExitProgramCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) {
							// �û�ѡ���ˡ��˳�����
							//����ļ�����
							std::ofstream file("data.json", std::ios::trunc);
							file.close();
							// д��
							std::ofstream newFile("data.json");
							newFile << "*���ļ��洢��ݲ˵��������������\nNever notify = 2";
							newFile.close();
						}
					}

					HWND hHideToTrayCheckbox = GetDlgItem(hwnd, 1); // ��ȡ�����ص�����������ѡ��ľ��
					HWND hExitProgramCheckbox = GetDlgItem(hwnd, 2); // ��ȡ���˳����򡱵�ѡ��ľ��
					if (SendMessage(hHideToTrayCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) {
						// �û�ѡ���ˡ����ص���������
						// ��С��������
						ShowWindow(host_hwnd, SW_MINIMIZE);
						ShowWindow(host_hwnd, SW_HIDE);
						// ���ء����á�����
						ShowWindow(hwnd_Settings, SW_HIDE);
						// ���ء���վ������
						ShowWindow(hwnd_website, SW_HIDE);
						// ���ء�ӳ�䡱����
						ShowWindow(hwnd_mapping, SW_HIDE);
						// ���ء������䡱����
						ShowWindow(hwnd_iTools, SW_HIDE);
						// ���ٴ���
						DestroyWindow(hwnd);
						// ����ϵͳ����ͼ������½�С��Ϣ
						// ����һ�� NOTIFYICONDATA �ṹ�����
						NOTIFYICONDATA nid;
						nid.cbSize = sizeof(NOTIFYICONDATA);
						nid.hWnd = host_hwnd;
						nid.uID = 1; // ָ��һ��Ψһ��ID
						nid.uFlags = NIF_INFO; // ���ñ�־��ָ��Ҫ��ʾ��Ϣ
						nid.dwInfoFlags = NIIF_INFO; // ������Ϣ����
						nid.uTimeout = 5000; // ָ����Ϣ��ʾ��ʱ�䣨���룩
						lstrcpy(nid.szInfoTitle, TEXT("��ݲ˵�")); // ������Ϣ����
						lstrcpy(nid.szInfo, TEXT("������Ŷ��qwq\n������ͼ���쳣���ٴδ򿪳�����ʾOvO")); // ������Ϣ����
						Shell_NotifyIcon(NIM_MODIFY, &nid); // ������Ϣ
					} else if (SendMessage(hExitProgramCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) {
						// �û�ѡ���ˡ��˳�����
						// �ڴ��ڹر�ʱ��� ������.txt �ļ�������
						std::ofstream file("������.txt", std::ios::trunc); // ʹ�� std::ios::trunc ģʽ���ļ�������ļ�����
						if (file.is_open()) {
							file.close(); // �ر��ļ�
							std::cout << "�ļ����������" << std::endl;
						} else {
							std::cerr << "�޷����ļ�" << std::endl;
						}
						// ���ٴ���
						DestroyWindow(hwnd);
						Shell_NotifyIcon(NIM_DELETE, &nid); // �Ƴ�����ͼ��
						PostQuitMessage(0); // ���û��������˳�����ʱ�����ڽ�һֱ����Ϊʹ��״̬
					}
					break;
				}

				case 5 : { // ȡ��
					// ���ٴ���
					DestroyWindow(hwnd);
					break;
				}

				/* ����������Ϣ�����кࣩܶ��ʹ��Ĭ�Ϲ��̴��� */
				default: {
					DefWindowProc(hwnd, msg, wParam, lParam);
					break;
				}
			}
			break;
		}

		/* ����������Ϣ���ࣩܶ��ʹ��Ĭ�ϳ����� */
		default: {
			DefWindowProc(hwnd, msg, wParam, lParam);
			break;
		}
	}
}

/* �˺������� ���� �Ӵ��ڵ���Ϣ */
LRESULT CALLBACK SettingsWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	// �����Ӵ�����Ϣ
	switch (msg) {

		// �����Ӵ��ڵĹر���Ϣ
		case WM_CLOSE: {
			// �ر��Ӵ���
			DestroyWindow(hwnd);
			break;
		}

		/* ������ʱ���������߳�ֹͣ */
		case WM_DESTROY: { // ����ʱ
			isSettingsWindowOpen = 0; // ��������Ϊ0����ʾ����վ���Ӵ����ѹر�
			break;
		}

		/* �ǿͻ�����Ҫ������ͻ���ʱ���͸����ڵ���Ϣ */
		case WM_NCCALCSIZE: {
			// ���ر߿�
			break;
		}

		/* �ڴ����Ͻ��л��Ʋ��� */
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			RECT rc;
			GetClientRect(hwnd, &rc);
			// ����һ����ɫ��ˢ����ѡ���豸��������
			HBRUSH hBrush = CreateSolidBrush(RGB(240, 240, 240));
			SelectObject(hdc, hBrush);
			// ���Ʋ˵�������
			RECT menuBarRect = { rc.left, rc.top, rc.right, rc.top + 32 };
			FillRect(hdc, &menuBarRect, hBrush);
			// �������
			HPEN hPen = CreatePen(PS_SOLID, 2, RGB(128, 128, 128));
			SelectObject(hdc, hPen);
			MoveToEx(hdc, rc.left, rc.top, NULL);
			LineTo(hdc, rc.right, rc.top);
			LineTo(hdc, rc.right, rc.bottom);
			LineTo(hdc, rc.left, rc.bottom);
			LineTo(hdc, rc.left, rc.top);
			// ���ơ�X����ť
			HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("System"));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(50, 50, 50));
			SetBkMode(hdc, TRANSPARENT);
			TextOut(hdc, rc.right - 24, rc.top + 10, "X", 1);
			// ���ơ�-����ť
			WCHAR minimizeButton[] = { 0x2013, '\0' }; // ʹ��Unicode�����ơ�-�����Ż����һЩ
			SetTextColor(hdc, RGB(50, 50, 50));
			SetBkMode(hdc, TRANSPARENT);
			TextOutW(hdc, rc.right - 60, rc.top + 10, minimizeButton, lstrlenW(minimizeButton));
			// ��ȡ����ͼ��
			HICON hIcon = (HICON)LoadImage(NULL, "��ݲ˵�.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
			if (hIcon != NULL) {
				// ����ͼ��λ��
				int iconWidth = 24; // ͼ��ĳ�
				int iconHeight = 24; // ͼ��Ŀ�
				int iconX = rc.left + 4;
				int iconY = rc.top + (32 - iconHeight) / 2;
				// ����ͼ��
				DrawIconEx(hdc, iconX, iconY, hIcon, iconWidth, iconHeight, 0, NULL, DI_NORMAL);
			}
			// ��ȡ���ڱ���
			TCHAR title[256];
			GetWindowText(hwnd, title, 256);
			// ���ƴ��ڱ���
			HFONT hTitleFont = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("΢���ź�"));
			SelectObject(hdc, hTitleFont);
			SetTextColor(hdc, RGB(0, 0, 0));
			SetBkMode(hdc, TRANSPARENT);
			TextOut(hdc, rc.left + 31, rc.top + 7, title, lstrlen(title));
			// ɾ����ˢ�����ʺ��������
			DeleteObject(hBrush);
			DeleteObject(hPen);
			DeleteObject(hFont);
			DeleteObject(hTitleFont);
			EndPaint(hwnd, &ps);
			break;
		}

		/* ������ڴ��ڵķǿͻ����ƶ�ʱ������������λ�� */
		case WM_NCHITTEST: {
			POINT pt = { LOWORD(lParam), HIWORD(lParam) };
			ScreenToClient(hwnd, &pt);
			RECT rc;
			GetClientRect(hwnd, &rc);
			// ����Ƿ����ˡ�X����ť
			if (pt.x > rc.right - 37 && pt.x < rc.right - 0 && pt.y > rc.top + 0 && pt.y < rc.top + 32) {
				return HTCLOSE;
			}
			// ����Ƿ����ˡ�-����ť
			if (pt.x > rc.right - 70 && pt.x < rc.right - 35 && pt.y > rc.top + 0 && pt.y < rc.top + 32) {
				return HTMINBUTTON;
			}
			// ����������λ�ã���������Ӧ��ֵ
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

		/* ������ڴ��ڵķǿͻ���������ʱ��ִ����Ӧ���� */
		case WM_NCLBUTTONUP: {
			if (wParam == HTCLOSE) {
				// �������ˡ�X����ť�����͹رմ��ڵ���Ϣ
				SendMessage(hwnd, WM_CLOSE, 0, 0);
				return 0;
			} else if (wParam == HTMINBUTTON) {
				// �������ˡ�-����ť��������С�����ڵ���Ϣ
				SendMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
				return 0;
			}
			break;
		}

		/* ������ڴ��ڵķǿͻ���������������ڱ������ϣ�ִ�����²��� */
		case WM_NCLBUTTONDOWN: {
			if (wParam == HTCAPTION) {
				SetForegroundWindow(hwnd); // ����������Ϊǰ̨����
				SendMessage(hwnd, WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0); // �����ƶ����ڵ���Ϣ
				return 0;
			}
			break;
		}

		/* ֪ͨ���ڵĻ״̬�����仯 */
		case WM_ACTIVATE: {
			if (wParam == WA_INACTIVE) {
				// ���˳���Ľ��㶪ʧʱ�����ر��������Է�Windows7ϵͳ�л���������������ԭ��������ʾ������
				SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_CAPTION); // ���ر�����
				SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER); // ʹ�ı���Ч
			}
			break;
		}

		/* ���þ�̬�ؼ��ı�����ɫ */
		//case WM_CTLCOLORSTATIC: {
		//	HDC hdcStatic = (HDC)wParam; // ��ȡ��̬�ؼ����豸�����ľ��
		//	SetTextColor(hdcStatic, RGB(0, 0, 0)); // �����ı���ɫΪ��ɫ
		//	SetBkColor(hdcStatic, RGB(255, 255, 255)); // ���ñ�����ɫΪ����ɫ
		//	return (INT_PTR)CreateSolidBrush(RGB(255, 255, 255)); // ����һ������ɫ�Ļ�ˢ��Ϊ��̬�ؼ��ı���ɫ
		//}

		/* ������ť */
		case WM_CREATE: { // ������ť
			// ����TabControl�ؼ�
			hTab = CreateWindow(WC_TABCONTROL, "", WS_CHILD | WS_VISIBLE, 1, 30, 348, 344, hwnd, NULL, NULL, NULL);
			// ����ѡ�
			TCITEM tabItem1 = {0};
			tabItem1.mask = TCIF_TEXT;
			tabItem1.pszText = "����";
			TabCtrl_InsertItem(hTab, 0, &tabItem1);
			TCITEM tabItem2 = {0};
			tabItem2.mask = TCIF_TEXT;
			tabItem2.pszText = "��ϸ";
			TabCtrl_InsertItem(hTab, 1, &tabItem2);
			if (SendMessage(A_Top, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �ж� �ö� ��ѡ���Ƿ񱻹�ѡ
				// ��ѡ
				SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // �ö�
			}
			// ѡ�ҳ��1
			Settingtext = CreateWindow("BUTTON", "����-��ݲ˵�", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 10, 60, 330, 175, hwnd, NULL, NULL, NULL);
			Nevernotifyresetting = CreateWindow("button", "���ò���ѯ��", WS_CHILD | WS_VISIBLE | BS_FLAT, 20, 80, 100, 30, hwnd, (HMENU)1, NULL, NULL);
			Startup = CreateWindow("button", "����������", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 130, 88, 95, 15, hwnd, (HMENU)3, hInst, NULL);

			experimentalfeaturestext = CreateWindow("BUTTON", "ʵ���Թ��ܣ����ã�������BUG��", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 10, 235, 330, 130, hwnd, NULL, NULL, NULL);
			strongTop = CreateWindow("button", "��ǿ�ö��������ڿ����ö���Ч��", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 20, 255, 230, 15, hwnd, (HMENU)2, hInst, NULL);

			// ѡ�ҳ��2
			button_32913 = CreateWindow("button", "��ϸ˵��", WS_CHILD | WS_VISIBLE | BS_FLAT, 20, 80, 80, 30, hwnd, (HMENU)32913, hInst, NULL);
			ShowWindow(button_32913, SW_HIDE);
			button_32914 = CreateWindow("button", "��������", WS_CHILD | WS_VISIBLE | BS_FLAT, 120, 80, 80, 30, hwnd, (HMENU)32914, hInst, NULL);
			ShowWindow(button_32914, SW_HIDE);

			// ������ʱ��
			SetTimer(hwnd, 1, 0, NULL); // ������ʱ����ID��1

			// ����һ��������ı�
			HFONT hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("΢���ź�"));
			// ���´���������Ӧ�����ı��ؼ�
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
				// ��ȡ��ǰѡ�е�ѡ�����
				hTab = pnmh->hwndFrom;
				int tabIndex = TabCtrl_GetCurSel(hTab);

				// ����ѡ�������ʾ��Ӧ��ҳ��
				switch (tabIndex) {
					case 0: {
						// ѡ�ҳ��1
						ShowWindow(Settingtext, SW_SHOW);
						ShowWindow(Nevernotifyresetting, SW_SHOW);
						ShowWindow(experimentalfeaturestext, SW_SHOW);
						ShowWindow(strongTop, SW_SHOW);
						ShowWindow(Startup, SW_SHOW);

						// ѡ�ҳ��2
						ShowWindow(button_32913, SW_HIDE);
						ShowWindow(button_32914, SW_HIDE);
						break;
					}
					case 1: {
						// ѡ�ҳ��1
						ShowWindow(Settingtext, SW_HIDE);
						ShowWindow(Nevernotifyresetting, SW_HIDE);
						ShowWindow(experimentalfeaturestext, SW_HIDE);
						ShowWindow(strongTop, SW_HIDE);
						ShowWindow(Startup, SW_HIDE);

						// ѡ�ҳ��2
						ShowWindow(button_32913, SW_SHOW);
						ShowWindow(button_32914, SW_SHOW);
						break;
					}
				}
			}
			return 0; // ����0�Ա�ʾ��Ϣ�ѱ���ȷ����
		}

		/* ��ʱ�� */
		case WM_TIMER: {
			// ����ȫ�ֱ�����ֵ���ø�ѡ��Ĺ�ѡ״̬
			SendMessage(strongTop, BM_SETCHECK, strongTopChecked, 0);

			std::ifstream file("data.json");
			std::string line;
			bool found = false;

			while (std::getline(file, line)) {
				if (line.find("Never notify = 1") != std::string::npos || line.find("Never notify = 2") != std::string::npos) {
					// ����������������һ������ʱִ�еĴ����
					found = true;
				}
			}

			file.close();

			if (!found) {
				// ���� ���� ��ť״̬Ϊ����
				EnableWindow(Nevernotifyresetting, FALSE);
			} else {
				EnableWindow(Nevernotifyresetting, TRUE);
			}

			HKEY hKey;
			LPCTSTR lpValueName = "��ݲ˵�";
			// ��ע�����
			if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) {
				// ���ע����ֵ�Ƿ����
				DWORD dataType;
				DWORD dataSize = 0;
				if (RegQueryValueEx(hKey, lpValueName, NULL, &dataType, NULL, &dataSize) == ERROR_SUCCESS) {
					std::cout << "��������ӵ����������" << std::endl;
					// ���� �������� ��ѡ���ѡ��״̬Ϊѡ��
					SendMessage(Startup, BM_SETCHECK, BST_CHECKED, 0);
				} else {
					std::cout << "����δ��ӵ����������" << std::endl;
					// ���� �������� ��ѡ���ѡ��״̬Ϊδѡ��
					SendMessage(Startup, BM_SETCHECK, BST_UNCHECKED, 0);
				}
				// �ر�ע�����
				RegCloseKey(hKey);
			} else {
				std::cout << "��ע�����ʧ�ܣ�" << std::endl;
			}
			break;
		}

		case WM_COMMAND: { // ����ť����¼�
			switch (wParam) {

				case 1 : { // ����...
					//����ļ�����
					std::ofstream file("data.json", std::ios::trunc);
					file.close();
					// д��
					std::ofstream newFile("data.json");
					newFile << "*���ļ��洢��ݲ˵��������������";
					newFile.close();
					MessageBox(hwnd, "����ɲ�����\\���������/", "�����ã�(��'��'��)", MB_OK | MB_ICONINFORMATION);
					break;
				}

				case 2 : {
					// ��ȡ��ѡ��Ĺ�ѡ״̬
					int checked = SendMessage(strongTop, BM_GETCHECK, 0, 0);

					// ���ݹ�ѡ״̬����ȫ�ֱ���
					strongTopChecked = (checked == BST_CHECKED) ? 1 : 0;

					// ��������Ը��ݹ�ѡ״ִ̬����Ӧ�Ĳ���
					if (strongTopChecked == 1) {
						// ��ѡ�򱻹�ѡʱ�Ĳ���
						int strongTopChecked = 1;
					} else {
						// ��ѡ��ȡ����ѡʱ�Ĳ���
						int strongTopChecked = 0;
					}

					if (SendMessage(A_Top, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �ж� �ö� ��ѡ���Ƿ񱻹�ѡ
						// ��ѡ
						// �ж�ȫ�ֱ�����ֵ
						if (strongTopChecked == 0) {
							// ȫ�ֱ�����ֵΪ0ʱ�Ĳ���
							if (tThread != NULL) { // ����߳̾����Ϊ��
								TerminateThread(tThread, 0); // ��ֹ�߳�
								CloseHandle(tThread); // �ر��߳̾��
								tThread = NULL;
							}
						} else if (strongTopChecked == 1) {
							// ȫ�ֱ�����ֵΪ1ʱ�Ĳ���
							tThread = CreateThread(NULL, 0, TopMostThread, NULL, 0, NULL); // �����߳���ִ��TopMostThread����
							if (tThread == NULL) { // ����߳̾��Ϊ��
								MessageBox(hwnd, "�޷������̣߳�", "����", MB_OK | MB_ICONERROR); // ��ʾ������Ϣ��
							}
						}
					}
					break;
				}

				case 3 : {
					HKEY hKey;
					LPCTSTR lpValueName = "��ݲ˵�";
					// ��ע�����
					if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) {
						// ���ע����ֵ�Ƿ����
						DWORD dataType;
						DWORD dataSize = 0;
						if (RegQueryValueEx(hKey, lpValueName, NULL, &dataType, NULL, &dataSize) == ERROR_SUCCESS) {
							std::cout << "��������ӵ����������" << std::endl;
							HKEY hKey;
							LPCTSTR lpValueName = "��ݲ˵�";
							// ��ע�����
							if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
								// ɾ��ע����ֵ
								if (RegDeleteValue(hKey, lpValueName) == ERROR_SUCCESS) {
									std::cout << "�����ѳɹ��ӿ������������Ƴ���" << std::endl;
								} else {
									std::cout << "�ӿ������������Ƴ�����ʧ�ܣ�" << std::endl;
								}
								// �ر�ע�����
								RegCloseKey(hKey);
							} else {
								std::cout << "��ע�����ʧ�ܣ�" << std::endl;
							}
						} else {
							std::cout << "����δ��ӵ����������" << std::endl;
							HKEY hKey;
							LPCTSTR lpValueName = "��ݲ˵�";
							char buffer[MAX_PATH];
							GetModuleFileName(NULL, buffer, MAX_PATH);
							std::string programPath(buffer);
							LPCTSTR lpPath = programPath.c_str();
							// ��ע�����
							if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
								// ����ע����ֵ
								if (RegSetValueEx(hKey, lpValueName, 0, REG_SZ, (LPBYTE)lpPath, strlen(lpPath) + 1) == ERROR_SUCCESS) {
									std::cout << "�����ѳɹ���ӵ����������" << std::endl;
								} else {
									std::cout << "��ӵ�����������ʧ�ܣ�" << std::endl;
								}
								// �ر�ע�����
								RegCloseKey(hKey);
							} else {
								std::cout << "��ע�����ʧ�ܣ�" << std::endl;
							}
						}
						// �ر�ע�����
						RegCloseKey(hKey);
					} else {
						std::cout << "��ע�����ʧ�ܣ�" << std::endl;
					}
					break;
				}

				case 32913: { // ��ϸ˵��
					MessageBox(hwnd, "�������⼰����취��\n��ʹ�õĹ����п��ܻ�������������Ӧ�������������������������ĵȴ����룬��ȴ�ʱ�����������ǿ�ƹرճ����������Ҽ�-����������������-�л���Ӧ�ó���ѡ��ҳ-ѡ��˳���-�����������\n\n���������������ͷ�����\n���Ƿǳ�Ը����������������ͷ�����Ӧ�õ���һ���汾�У����Ƕ��ڱȽ����ѶȵĹ��ܻῪ�������������½⡣���ǻᾡ�����ɴ󲿷����ô�����������һ�ӭ���ڳ���ʹ���з���BUG������©����ʱ�����������Ա�����һ���汾���޸��������ڵײ��BUG����ǣ����������ĵ�BUG����ͨ����Ҫ���õ�ʱ���������޸��������½⣡\n\n����Ŀ�����Դ���룺\n�˳�����С��èDev-C++���������汾��6.7.5����Ͳ���ϵͳҪ��Windows XP 32λ����װ������Dev-Cpp.6.7.5.GCC.9.2.Setup��ʹ��C++���Կ�������������ݲ���Դ������ѧϰ��Ҫ���ǿ�Ϊ���ṩѧϰ˼·\n\nʹ��˵��/����������\n���ǲ����ռ������κ���˽��Ϣ�������Ὣ��Ϣ�ϴ����ƶˡ������ʹ�ô˳���������κεĺ�������������˳е���\n\n���ǵ����Ĳ��ܻ��������������Ͷ����ǵ�֧�֣���󣬸�л����ʹ�ã����������ߣ�Ling", "��ϸ˵��(��������)�� ", MB_OK );
					break;
				}

				case 32914: { // ��������
					MessageBox(hwnd, "����2.0��֮ǰ�汾����\n2.0�汾��֮ǰ�汾��������ʡ�ԡ�2.0�汾��2023��7��5����ʽ��ʼ��������2023��9��27����ɿ�����2126�д��룬��89515���ַ�\n����2.1�汾����\n2.1�汾��2023��9��28����ʽ��ʼ��������2023��10��12����ɿ�����3194�д��룬��142734���ַ�\n�������ݣ�1.�Ż��˲��ִ��ڵĲ��֡�2.�����������ڡ����á�����ӳ�丨�����ߡ�ʵ�ù������һЩ��Ϣ��3.��Ϊ������������Ϣ�����������Ծ�ɾ���ˡ�4.�޸�����֪��BUG\n����2.52�汾����\n2.52�汾��2023��10��31����ʽ��ʼ��������2024��4��24����ɿ�������;��ͣ�����ο�����7128�д��룬��326090���ַ�\n2.52�汾��������������£�\n1.��������ʱ�е���ۣ����з�win10���ڣ�\n2.���޸ġ����ÿ��İ�ť����ťƯ���ˣ�\n3.���޸ġ����������½����������ࡹ��ť�����º�ɴ򿪡����ࡹ����\n4.��ɾ����ɾ���������ڵġ��˵������������ࡹ�͡����á��������������ࡹ������\n5.����������ӳ�乤�ߡ�����������ר�����ҵ�ӳ���������ļ�������\n6.�����������õ��˳�ʱ���ѣ�ѡ������һ��ѡ����´��Զ�ѡ����һ�\n7.���޸ġ���������վ��ݴ򿪡����ڸ���Ϊ��ʵ��վ���ݴ򿪡�\n8.���޸ġ������á�����Ϊʵʱ���»���\n9.�������������á�����������������������\n10.��������������һЩ�ɰ��������ֺ��޸Ĳ��ֵ������ݣ���Ȼûʲô�ã���������������qwq��\n11.���޸ġ��޸�һ���ɰ汾�ʹ��ڵĻ��ƣ�������������ʱ���ٴ�����ʱ���ͼ����ز��������������ʾͼ���쳣���ѣ�֮����ʾ�������������е����ѡ��޸ĺ��ǣ�������ʾ�����Ѿ������������ѣ��������ͼ���쳣������\n12.���޸ġ���ʵ��վ�㡹�������ࡸ��ҳ����ť��ɾ����һЩ��̫ʵ�õġ���ҳ����ť\n13.��ɾ����ɾȥ��ӳ�乤�ߡ��ġ�ʹ����֪���ı�����\n14.����������ӳ�乤�ߡ���������ʾ��ռ�õ��̷�������\n15.����������ӳ�乤�ߡ�������ӳ��ɹ����Զ�ѡ����һ������Ŀ¼���̷�������\n16.���޸ġ���ӳ�乤�ߡ����·��ࣨ�޸�λ�ã�[����/����]�ڵĿؼ��������޸ġ�ӳ��/�Ͽ����ڵĿؼ���λ�úʹ�С\n17.����������ӳ�乤�ߡ�������ӳ��ɹ��󲻽�����ʾ���롹����\n18.���޸ġ�[����]���ڵ�[��ϸ]ҳ�����ˡ���ϸ˵�����͡��������ݡ�������\n19.���޸ġ����������½ǵ��������Բ����޸ģ����Ӻ�ɾ����������Ϊ20��\n20.���޸ġ���ӳ�乤�ߡ���ӳ��ɹ���ʧ����������Ϊ����ӳ��ɹ���������룬��֮�෴\n21.���޸ġ���ӳ�乤�ߡ���Ĭ��ӳ�䵹��ʱʱ���Ϊ45����\n22.��ɾ������ӳ�乤�ߡ����������롹ɾȥ��ʾ��Ϣ\n23.���޸ġ����д��ڽ����������Ϊ΢���źڣ����ڿ��Ų����ˣ�\n24.���޸ġ���ӳ�乤�ߡ��޸�ӳ���ں�-��ʹ��WNetAddConnection2A�������������С\n25.����������ʵ��վ�㡹���沼������ʽ���£�������һЩ�ؼ���������ˡ�����ַ��ѡ����������Ϊ10��\n26.���޸ġ������ࡹ-����-�����Թ���-����ǿ�ö�������Ϊ����ǿ�ö��������ڿ����ö���Ч����\n27.���޸ġ���ӳ�乤�ߡ��������£�������ʱʱ����̷�����������򶼿�����������\n28.����������ʵ��վ�㡹��ʹ��������򿪡�ѡ���������ʹ��Edge������򿪡�\n29.�������������°�ť��δ�������ʱ�������ĵ����ı����ģ��رյ�������ʾ����С����������Ч����ǰ�汾���У�����\n30.��ɾ����ɾ���࿪���ں����ʾ�����������˳���࿪ʱ������Ϊֱ����ʾ�䴰��\n31.���޸ġ���ӳ�乤�ߡ�΢���䴰�ڴ�С���仯����\n32.���޸ġ�������˵���ı�����\n33.���޸ġ���ӳ�乤�ߡ������䡰ʹ��NET�������������Ϊ��ʹ��NET������ʾ������\n34.����������ʵ�ù����䡹������������Դ����������ť���书��\n35.����������ʵ�ù����䡹������ϵͳ�������ļ�����ť���书��\n36.����������ʵ�ù����䡹������ϵͳ�������ļ��С���ť���书��\n37.������������������Ӳ��ʹ���ʣ��������ڴ桱ռ����ʾ\n38.����������ӳ�乤�ߡ���������/��һ����ť���������л�ҳ��\n39.����������ӳ�乤�ߡ�����������IP��ַ���͡����湲��Ŀ¼������\n40.����������ӳ�乤�ߡ�������Ĭ��IP��ַ/����Ŀ¼�洢�����ܣ��ڿ�ݲ˵�Ŀ¼�£�\n41.�����������������������������塱���ܣ����Է���Ĳ鿴���ƹ������ݣ�Ŀǰֻ�ܼ�Ⲣд��Ctrl+X��Ctrl+C����ϼ��������ݣ�\n42.����������ӳ�乤�ߡ�����ϸ˵������ť������������ӳ�䷵��ֵ��Ϣ\n��������������\n\n���ǵ����Ĳ��ܻ��������������Ͷ����ǵ�֧�֣���󣬸�л����ʹ�ã����������ߣ�Ling", "��������(o������)o  ", MB_OK );
					break;
				}

				/* ����������Ϣ�����кࣩܶ��ʹ��Ĭ�Ϲ��̴��� */
				default: {
					DefWindowProc(hwnd, msg, wParam, lParam);
					break;
				}
			}
			break;
		}

		/* ����������Ϣ���ࣩܶ��ʹ��Ĭ�ϳ����� */
		default: {
			DefWindowProc(hwnd, msg, wParam, lParam);
			break;
		}
	}
}

/* �˺������� ������վ��ݴ� �Ӵ��ڵ���Ϣ */
LRESULT CALLBACK ChildWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	// �����Ӵ�����Ϣ
	switch (msg) {

		// �����Ӵ��ڵĹر���Ϣ
		case WM_CLOSE: {
			// �ر��Ӵ���
			DestroyWindow(hwnd);
			break;
		}

		/* ������ʱ���������߳�ֹͣ */
		case WM_DESTROY: { // ����ʱ
			isWebsiteWindowOpen = 0; // ��������Ϊ0����ʾ����վ���Ӵ����ѹر�
			break;
		}

		/* �ǿͻ�����Ҫ������ͻ���ʱ���͸����ڵ���Ϣ */
		case WM_NCCALCSIZE: {
			// ���ر߿�
			break;
		}

		/* �ڴ����Ͻ��л��Ʋ��� */
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			RECT rc;
			GetClientRect(hwnd, &rc);
			// ����һ����ɫ��ˢ����ѡ���豸��������
			HBRUSH hBrush = CreateSolidBrush(RGB(240, 240, 240));
			SelectObject(hdc, hBrush);
			// ���Ʋ˵�������
			RECT menuBarRect = { rc.left, rc.top, rc.right, rc.top + 32 };
			FillRect(hdc, &menuBarRect, hBrush);
			// �������
			HPEN hPen = CreatePen(PS_SOLID, 2, RGB(128, 128, 128));
			SelectObject(hdc, hPen);
			MoveToEx(hdc, rc.left, rc.top, NULL);
			LineTo(hdc, rc.right, rc.top);
			LineTo(hdc, rc.right, rc.bottom);
			LineTo(hdc, rc.left, rc.bottom);
			LineTo(hdc, rc.left, rc.top);
			// ���ơ�X����ť
			HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("System"));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(50, 50, 50));
			SetBkMode(hdc, TRANSPARENT);
			TextOut(hdc, rc.right - 24, rc.top + 10, "X", 1);
			// ���ơ�-����ť
			WCHAR minimizeButton[] = { 0x2013, '\0' }; // ʹ��Unicode�����ơ�-�����Ż����һЩ
			SetTextColor(hdc, RGB(50, 50, 50));
			SetBkMode(hdc, TRANSPARENT);
			TextOutW(hdc, rc.right - 60, rc.top + 10, minimizeButton, lstrlenW(minimizeButton));
			// ��ȡ����ͼ��
			HICON hIcon = (HICON)LoadImage(NULL, "��ݲ˵�.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
			if (hIcon != NULL) {
				// ����ͼ��λ��
				int iconWidth = 24; // ͼ��ĳ�
				int iconHeight = 24; // ͼ��Ŀ�
				int iconX = rc.left + 4;
				int iconY = rc.top + (32 - iconHeight) / 2;
				// ����ͼ��
				DrawIconEx(hdc, iconX, iconY, hIcon, iconWidth, iconHeight, 0, NULL, DI_NORMAL);
			}
			// ��ȡ���ڱ���
			TCHAR title[256];
			GetWindowText(hwnd, title, 256);
			// ���ƴ��ڱ���
			HFONT hTitleFont = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("΢���ź�"));
			SelectObject(hdc, hTitleFont);
			SetTextColor(hdc, RGB(0, 0, 0));
			SetBkMode(hdc, TRANSPARENT);
			TextOut(hdc, rc.left + 31, rc.top + 7, title, lstrlen(title));
			// ɾ����ˢ�����ʺ��������
			DeleteObject(hBrush);
			DeleteObject(hPen);
			DeleteObject(hFont);
			DeleteObject(hTitleFont);
			EndPaint(hwnd, &ps);
			break;
		}

		/* ������ڴ��ڵķǿͻ����ƶ�ʱ������������λ�� */
		case WM_NCHITTEST: {
			POINT pt = { LOWORD(lParam), HIWORD(lParam) };
			ScreenToClient(hwnd, &pt);
			RECT rc;
			GetClientRect(hwnd, &rc);
			// ����Ƿ����ˡ�X����ť
			if (pt.x > rc.right - 37 && pt.x < rc.right - 0 && pt.y > rc.top + 0 && pt.y < rc.top + 32) {
				return HTCLOSE;
			}
			// ����Ƿ����ˡ�-����ť
			if (pt.x > rc.right - 70 && pt.x < rc.right - 35 && pt.y > rc.top + 0 && pt.y < rc.top + 32) {
				return HTMINBUTTON;
			}
			// ����������λ�ã���������Ӧ��ֵ
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

		/* ������ڴ��ڵķǿͻ���������ʱ��ִ����Ӧ���� */
		case WM_NCLBUTTONUP: {
			if (wParam == HTCLOSE) {
				// �������ˡ�X����ť�����͹رմ��ڵ���Ϣ
				SendMessage(hwnd, WM_CLOSE, 0, 0);
				return 0;
			} else if (wParam == HTMINBUTTON) {
				// �������ˡ�-����ť��������С�����ڵ���Ϣ
				SendMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
				return 0;
			}
			break;
		}

		/* ������ڴ��ڵķǿͻ���������������ڱ������ϣ�ִ�����²��� */
		case WM_NCLBUTTONDOWN: {
			if (wParam == HTCAPTION) {
				SetForegroundWindow(hwnd); // ����������Ϊǰ̨����
				SendMessage(hwnd, WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0); // �����ƶ����ڵ���Ϣ
				return 0;
			}
			break;
		}

		/* ֪ͨ���ڵĻ״̬�����仯 */
		case WM_ACTIVATE: {
			if (wParam == WA_INACTIVE) {
				// ���˳���Ľ��㶪ʧʱ�����ر��������Է�Windows7ϵͳ�л���������������ԭ��������ʾ������
				SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_CAPTION); // ���ر�����
				SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER); // ʹ�ı���Ч
			}
			break;
		}

		/* ���þ�̬�ؼ��ı�����ɫ */
		case WM_CTLCOLORSTATIC: {
			HDC hdcStatic = (HDC)wParam; // ��ȡ��̬�ؼ����豸�����ľ��
			SetTextColor(hdcStatic, RGB(0, 0, 0)); // �����ı���ɫΪ��ɫ
			SetBkColor(hdcStatic, RGB(255, 255, 255)); // ���ñ�����ɫΪ����ɫ
			return (INT_PTR)CreateSolidBrush(RGB(255, 255, 255)); // ����һ������ɫ�Ļ�ˢ��Ϊ��̬�ؼ��ı���ɫ
		}

		/* ������ť */
		case WM_CREATE: { // ������ť
			if (SendMessage(A_Top, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �ж� �ö� ��ѡ���Ƿ񱻹�ѡ
				// ��ѡ
				SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // �ö�
			}
			HWND xuanzefenzukuang = CreateWindow("BUTTON", "ѡ����ַ��", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 10, 35, 460, 55, hwnd, NULL, NULL, NULL);
			HWND Usingopen = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_DROPDOWNLIST | WS_VSCROLL, 40, 54, 160, 200, hwnd, (HMENU)1, NULL, NULL);
			SendMessage(Usingopen, CB_ADDSTRING, 0, (LPARAM) "ʹ��Ĭ���������");
			SendMessage(Usingopen, CB_ADDSTRING, 0, (LPARAM) "ʹ�ùȸ��������");
			SendMessage(Usingopen, CB_ADDSTRING, 0, (LPARAM) "ʹ��Edge�������");
			SendMessage(Usingopen, CB_SETCURSEL, 0, 0);
			HWND WebsiteURL = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_DROPDOWNLIST | WS_VSCROLL, 200, 54, 180, 200, hwnd, (HMENU)2, NULL, NULL);
			SendMessage(WebsiteURL, CB_ADDSTRING, 0, (LPARAM) "��Сæ-���߹�����");
			SendMessage(WebsiteURL, CB_ADDSTRING, 0, (LPARAM) "��ʱ����-���߹�����");
			SendMessage(WebsiteURL, CB_ADDSTRING, 0, (LPARAM) "PICK FREE�����Դ����");
			SendMessage(WebsiteURL, CB_ADDSTRING, 0, (LPARAM) "Vector Magicתʸ��ͼ");
			SendMessage(WebsiteURL, CB_ADDSTRING, 0, (LPARAM) "�����Ա����Ϸ�����ģ�");
			SendMessage(WebsiteURL, CB_ADDSTRING, 0, (LPARAM) "����ͼ�в���Ҫ������");
			SendMessage(WebsiteURL, CB_ADDSTRING, 0, (LPARAM) "��������-Scratch���");
			SendMessage(WebsiteURL, CB_ADDSTRING, 0, (LPARAM) "������-�ȸ�������");
			SendMessage(WebsiteURL, CB_ADDSTRING, 0, (LPARAM) "�����Ӿ���Ӧ�ٶ�");
			SendMessage(WebsiteURL, CB_ADDSTRING, 0, (LPARAM) "��ҳ��Windows11");
			SendMessage(WebsiteURL, CB_SETCURSEL, 0, 0);
			HWND ConfirmOpen = CreateWindow("BUTTON", "��", WS_VISIBLE | WS_CHILD | BS_FLAT, 380, 54, 60, 27, hwnd, (HMENU)3, NULL, NULL);

			HWND kuaijiefenzukuang = CreateWindow("BUTTON", "���ÿ�ݴ�", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 10, 90, 460, 255, hwnd, NULL, NULL, NULL);
			HWND baiduSearchbutton = CreateWindow("button", "�ٶ�����", WS_CHILD | WS_VISIBLE | BS_FLAT, 20, 110, 75, 30, hwnd, (HMENU)11, NULL, NULL);
			HWND bingSearchbutton = CreateWindow("button", "��Ӧ", WS_CHILD | WS_VISIBLE | BS_FLAT, 110, 110, 70, 30, hwnd, (HMENU)13, NULL, NULL);
			HWND programmingCatbutton = CreateWindow("button", "���è����", WS_CHILD | WS_VISIBLE | BS_FLAT, 195, 110, 90, 30, hwnd, (HMENU)21, NULL, NULL);
			HWND Dingdingbutton = CreateWindow("button", "��Ѷ�۶�", WS_CHILD | WS_VISIBLE | BS_FLAT, 300, 110, 75, 30, hwnd, (HMENU)22, NULL, NULL);
			HWND bilibilibutton = CreateWindow("button", "��������bilibili", WS_CHILD | WS_VISIBLE | BS_FLAT, 20, 150, 120, 30, hwnd, (HMENU)31, NULL, NULL);
			HWND douyinbutton = CreateWindow("button", "����", WS_CHILD | WS_VISIBLE | BS_FLAT, 390, 110, 70, 30, hwnd, (HMENU)32, NULL, NULL);
			HWND convertiobutton = CreateWindow("button", "Convertio�ļ�ת����", WS_CHILD | WS_VISIBLE | BS_FLAT, 160, 150, 160, 30, hwnd, (HMENU)41, NULL, NULL);
			HWND xunjiebutton = CreateWindow("button", "Ѹ����Ƶת����", WS_CHILD | WS_VISIBLE | BS_FLAT, 340, 150, 120, 30, hwnd, (HMENU)42, NULL, NULL);

			HWND wenbentext = CreateWindow("STATIC", "*��ʹ��������򿪡��Դ˴������пؼ���Ч", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 5, 344, 275, 20, hwnd, NULL, NULL, NULL);

			// ����һ��������ı�
			HFONT hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("΢���ź�"));
			// ���´���������Ӧ�����ı��ؼ�
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

		case WM_COMMAND: { // ����ť����¼�
			switch (wParam) {

				case 3: { // ��
					HWND Usingopen = GetDlgItem(hwnd, 1); // ��ȡ�������������ѡ���ľ��
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // ��ȡѡ�������
					HWND WebsiteURL = GetDlgItem(hwnd, 2); // ��ȡ����ַ������ѡ���ľ��
					int urlIndex = SendMessage(WebsiteURL, CB_GETCURSEL, 0, 0); // ��ȡѡ�����ַ����

					const char* url = "";
					if (urlIndex == 0) {
						url = "https://tool.browser.qq.com/"; // ��Сæ-���߹�����
					} else if (urlIndex == 1) {
						url = "https://www.67tool.com/"; // ��ʱ����-���߹�����
					} else if (urlIndex == 2) {
						url = "http://www.pickfree.cn/"; // PICK FREE�����Դ����
					} else if (urlIndex == 3) {
						url = "https://zh.vectormagic.com/"; //  Vector Magicתʸ��ͼ
					} else if (urlIndex == 4) {
						url = "https://playground.17coding.net/"; // �����Ա����Ϸ
					} else if (urlIndex == 5) {
						url = "https://magicstudio.com/zh/magiceraser/"; // ����ͼ�в���Ҫ������
					} else if (urlIndex == 6) {
						url = "https://www.ccw.site/"; // ��������
					} else if (urlIndex == 7) {
						url = "https://chrome.zzzmh.cn/"; // ������
					} else if (urlIndex == 8) {
						url = "https://humanbenchmark.com/"; // ��Ӧ�ٶȲ���
					} else if (urlIndex == 9) {
						url = "https://wintest.andrewstech.me/"; // ��ҳ��Windows11
					}

					if (selectedIndex == 0) {
						// ѡ��ʹ��Ĭ���������
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// ѡ��ʹ�ùȸ��������
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 2) {
						// ѡ��ʹ��Edge�������
						ShellExecute(NULL, "open", "msedge.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 11 : { // �ٶ�����
					HWND Usingopen = GetDlgItem(hwnd, 1); // ��ȡ����ѡ���ľ��
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // ��ȡѡ�������
					if (selectedIndex == 0) {
						// ѡ��ʹ��Ĭ���������
						const char* url = "https://www.baidu.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// ѡ��ʹ�ùȸ��������
						const char* url = "https://www.baidu.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 2) {
						// ѡ��ʹ��Edge�������
						const char* url = "https://www.baidu.com/";
						ShellExecute(NULL, "open", "msedge.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 13 : { // ��Ӧ
					HWND Usingopen = GetDlgItem(hwnd, 1); // ��ȡ����ѡ���ľ��
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // ��ȡѡ�������
					if (selectedIndex == 0) {
						// ѡ��ʹ��Ĭ���������
						const char* url = "https://cn.bing.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// ѡ��ʹ�ùȸ��������
						const char* url = "https://cn.bing.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 2) {
						// ѡ��ʹ��Edge�������
						const char* url = "https://cn.bing.com/";
						ShellExecute(NULL, "open", "msedge.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 21 : { // ���è����
					HWND Usingopen = GetDlgItem(hwnd, 1); // ��ȡ����ѡ���ľ��
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // ��ȡѡ�������
					if (selectedIndex == 0) {
						// ѡ��ʹ��Ĭ���������
						const char* url = "https://shequ.codemao.cn/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// ѡ��ʹ�ùȸ��������
						const char* url = "https://shequ.codemao.cn/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 2) {
						// ѡ��ʹ��Edge�������
						const char* url = "https://shequ.codemao.cn/";
						ShellExecute(NULL, "open", "msedge.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 22 : { // ��Ѷ�۶�
					HWND Usingopen = GetDlgItem(hwnd, 1); // ��ȡ����ѡ���ľ��
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // ��ȡѡ�������
					if (selectedIndex == 0) {
						// ѡ��ʹ��Ĭ���������
						const char* url = "https://coding.qq.com/home/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// ѡ��ʹ�ùȸ��������
						const char* url = "https://coding.qq.com/home/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 2) {
						// ѡ��ʹ��Edge�������
						const char* url = "https://coding.qq.com/home/";
						ShellExecute(NULL, "open", "msedge.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 31 : { // ��������bilibili
					HWND Usingopen = GetDlgItem(hwnd, 1); // ��ȡ����ѡ���ľ��
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // ��ȡѡ�������
					if (selectedIndex == 0) {
						// ѡ��ʹ��Ĭ���������
						const char* url = "https://www.bilibili.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// ѡ��ʹ�ùȸ��������
						const char* url = "https://www.bilibili.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 2) {
						// ѡ��ʹ��Edge�������
						const char* url = "https://www.bilibili.com/";
						ShellExecute(NULL, "open", "msedge.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 32 : { // ����
					HWND Usingopen = GetDlgItem(hwnd, 1); // ��ȡ����ѡ���ľ��
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // ��ȡѡ�������
					if (selectedIndex == 0) {
						// ѡ��ʹ��Ĭ���������
						const char* url = "https://www.douyin.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// ѡ��ʹ�ùȸ��������
						const char* url = "https://www.douyin.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 2) {
						// ѡ��ʹ��Edge�������
						const char* url = "https://www.douyin.com/";
						ShellExecute(NULL, "open", "msedge.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 41 : { // Convertio�ļ�ת����
					HWND Usingopen = GetDlgItem(hwnd, 1); // ��ȡ����ѡ���ľ��
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // ��ȡѡ�������
					if (selectedIndex == 0) {
						// ѡ��ʹ��Ĭ���������
						const char* url = "https://convertio.co/zh/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// ѡ��ʹ�ùȸ��������
						const char* url = "https://convertio.co/zh/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 2) {
						// ѡ��ʹ��Edge�������
						const char* url = "https://convertio.co/zh/";
						ShellExecute(NULL, "open", "msedge.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 42 : { // Ѹ����Ƶת����
					HWND Usingopen = GetDlgItem(hwnd, 1); // ��ȡ����ѡ���ľ��
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // ��ȡѡ�������
					if (selectedIndex == 0) {
						// ѡ��ʹ��Ĭ���������
						const char* url = "https://app.xunjieshipin.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// ѡ��ʹ�ùȸ��������
						const char* url = "https://app.xunjieshipin.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 2) {
						// ѡ��ʹ��Edge�������
						const char* url = "https://app.xunjieshipin.com/";
						ShellExecute(NULL, "open", "msedge.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				/* ����������Ϣ�����кࣩܶ��ʹ��Ĭ�Ϲ��̴��� */
				default: {
					DefWindowProc(hwnd, msg, wParam, lParam);
					break;
				}
			}
			break;
		}

		/* ����������Ϣ���ࣩܶ��ʹ��Ĭ�ϳ����� */
		default: {
			DefWindowProc(hwnd, msg, wParam, lParam);
			break;
		}
	}
}

/* �˺������� ����ӳ�丨������ �Ӵ��ڵ���Ϣ */
LRESULT CALLBACK ChildWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {

		// �����Ӵ��ڵĹر���Ϣ
		case WM_CLOSE: {
			if (countdownMinutes == 0 && countdownDuration == 0) { // �������countdownMinutes���֣���countdownDuration���룩������0ʱ
				// �ر��Ӵ���
				DestroyWindow(hwnd);
			} else {
				int result = MessageBox(hwnd, "���ڵ���ʱ���˳�����ʧ���ȣ�ȷ���˳���(#^.^#)\n�����������ڵ���˳�ѡ�����ص�������", "��ʾ(��_��;)", MB_OKCANCEL | MB_ICONQUESTION);
				if (result == IDOK) { // �û������ȷ�ϰ�ť
					// �ر��Ӵ���
					DestroyWindow(hwnd);
				}
			}
			break;
		}

		/* ������ʱ���������߳�ֹͣ */
		case WM_DESTROY: { // ����ʱ
			isMappingWindowOpen = 0; // ��������Ϊ0����ʾ��ӳ�䡱�Ӵ����ѹر�
			hasContentExecuted = false; // ���ñ�־����
			noContentExecuted = false; // ���ñ�־����
			hasContentExecuted2 = false; // ���ñ�־����
			noContentExecuted2 = false; // ���ñ�־����
			hasContentExecuted3 = false; // ���ñ�־����
			noContentExecuted3 = false; // ���ñ�־����
			break;
		}

		/* �ǿͻ�����Ҫ������ͻ���ʱ���͸����ڵ���Ϣ */
		case WM_NCCALCSIZE: {
			// ���ر߿�
			break;
		}

		/* �ڴ����Ͻ��л��Ʋ��� */
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			RECT rc;
			GetClientRect(hwnd, &rc);
			// ����һ����ɫ��ˢ����ѡ���豸��������
			HBRUSH hBrush = CreateSolidBrush(RGB(240, 240, 240));
			SelectObject(hdc, hBrush);
			// ���Ʋ˵�������
			RECT menuBarRect = { rc.left, rc.top, rc.right, rc.top + 32 };
			FillRect(hdc, &menuBarRect, hBrush);
			// �������
			HPEN hPen = CreatePen(PS_SOLID, 2, RGB(128, 128, 128));
			SelectObject(hdc, hPen);
			MoveToEx(hdc, rc.left, rc.top, NULL);
			LineTo(hdc, rc.right, rc.top);
			LineTo(hdc, rc.right, rc.bottom);
			LineTo(hdc, rc.left, rc.bottom);
			LineTo(hdc, rc.left, rc.top);
			// ���ơ�X����ť
			HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("System"));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(50, 50, 50));
			SetBkMode(hdc, TRANSPARENT);
			TextOut(hdc, rc.right - 24, rc.top + 10, "X", 1);
			// ���ơ�-����ť
			WCHAR minimizeButton[] = { 0x2013, '\0' }; // ʹ��Unicode�����ơ�-�����Ż����һЩ
			SetTextColor(hdc, RGB(50, 50, 50));
			SetBkMode(hdc, TRANSPARENT);
			TextOutW(hdc, rc.right - 60, rc.top + 10, minimizeButton, lstrlenW(minimizeButton));
			// ��ȡ����ͼ��
			HICON hIcon = (HICON)LoadImage(NULL, "��ݲ˵�.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
			if (hIcon != NULL) {
				// ����ͼ��λ��
				int iconWidth = 24; // ͼ��ĳ�
				int iconHeight = 24; // ͼ��Ŀ�
				int iconX = rc.left + 4;
				int iconY = rc.top + (32 - iconHeight) / 2;
				// ����ͼ��
				DrawIconEx(hdc, iconX, iconY, hIcon, iconWidth, iconHeight, 0, NULL, DI_NORMAL);
			}
			// ��ȡ���ڱ���
			TCHAR title[256];
			GetWindowText(hwnd, title, 256);
			// ���ƴ��ڱ���
			HFONT hTitleFont = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("΢���ź�"));
			SelectObject(hdc, hTitleFont);
			SetTextColor(hdc, RGB(0, 0, 0));
			SetBkMode(hdc, TRANSPARENT);
			TextOut(hdc, rc.left + 31, rc.top + 7, title, lstrlen(title));
			// ɾ����ˢ�����ʺ��������
			DeleteObject(hBrush);
			DeleteObject(hPen);
			DeleteObject(hFont);
			DeleteObject(hTitleFont);
			EndPaint(hwnd, &ps);
			break;
		}

		/* ������ڴ��ڵķǿͻ����ƶ�ʱ������������λ�� */
		case WM_NCHITTEST: {
			POINT pt = { LOWORD(lParam), HIWORD(lParam) };
			ScreenToClient(hwnd, &pt);
			RECT rc;
			GetClientRect(hwnd, &rc);
			// ����Ƿ����ˡ�X����ť
			if (pt.x > rc.right - 37 && pt.x < rc.right - 0 && pt.y > rc.top + 0 && pt.y < rc.top + 32) {
				return HTCLOSE;
			}
			// ����Ƿ����ˡ�-����ť
			if (pt.x > rc.right - 70 && pt.x < rc.right - 35 && pt.y > rc.top + 0 && pt.y < rc.top + 32) {
				return HTMINBUTTON;
			}
			// ����������λ�ã���������Ӧ��ֵ
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

		/* ������ڴ��ڵķǿͻ���������ʱ��ִ����Ӧ���� */
		case WM_NCLBUTTONUP: {
			if (wParam == HTCLOSE) {
				// �������ˡ�X����ť�����͹رմ��ڵ���Ϣ
				SendMessage(hwnd, WM_CLOSE, 0, 0);
				return 0;
			} else if (wParam == HTMINBUTTON) {
				// �������ˡ�-����ť��������С�����ڵ���Ϣ
				SendMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
				return 0;
			}
			break;
		}

		/* ������ڴ��ڵķǿͻ���������������ڱ������ϣ�ִ�����²��� */
		case WM_NCLBUTTONDOWN: {
			if (wParam == HTCAPTION) {
				SetForegroundWindow(hwnd); // ����������Ϊǰ̨����
				SendMessage(hwnd, WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0); // �����ƶ����ڵ���Ϣ
				return 0;
			}
			break;
		}

		/* ֪ͨ���ڵĻ״̬�����仯 */
		case WM_ACTIVATE: {
			if (wParam == WA_INACTIVE) {
				// ���˳���Ľ��㶪ʧʱ�����ر��������Է�Windows7ϵͳ�л���������������ԭ��������ʾ������
				SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_CAPTION); // ���ر�����
				SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER); // ʹ�ı���Ч
			}
			break;
		}

		/* ���þ�̬�ؼ��ı�����ɫ */
		case WM_CTLCOLORSTATIC: {
			HDC hdcStatic = (HDC)wParam; // ��ȡ��̬�ؼ����豸�����ľ��
			SetTextColor(hdcStatic, RGB(0, 0, 0)); // �����ı���ɫΪ��ɫ
			SetBkColor(hdcStatic, RGB(255, 255, 255)); // ���ñ�����ɫΪ����ɫ
			return (INT_PTR)CreateSolidBrush(RGB(255, 255, 255)); // ����һ������ɫ�Ļ�ˢ��Ϊ��̬�ؼ��ı���ɫ
		}

		/* ������ť */
		case WM_CREATE: { // ������ť
			if (SendMessage(A_Top, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �ж� �ö� ��ѡ���Ƿ񱻹�ѡ
				// ��ѡ
				SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // �ö�
			}
			// ����һ��������ı�
			HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("΢���ź�"));
			hGroupBoxMapping = CreateWindow("BUTTON", "ӳ��/�Ͽ�", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 255, 35, 240, 300, hwnd, NULL, NULL, NULL);
			usernameLabel = CreateWindow("STATIC", "�û�����", WS_VISIBLE | WS_CHILD, 265, 60, 80, 20, hwnd, NULL, NULL, NULL);
			usernameEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 335, 57, 150, 26, hwnd, NULL, NULL, NULL); // ʹ��WS_EX_CLIENTEDGE��ʽʹ�������Ĭ��
			passwordLabel = CreateWindow("STATIC", "���룺", WS_VISIBLE | WS_CHILD, 265, 90, 80, 20, hwnd, NULL, NULL, NULL);
			passwordEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD | ES_AUTOHSCROLL, 335, 87, 150, 26, hwnd, NULL, NULL, NULL); // ������ES_AUTOHSCROLL�Ա������������
			ipLabel = CreateWindow("STATIC", "IP��ַ��", WS_VISIBLE | WS_CHILD, 265, 120, 80, 20, hwnd, NULL, NULL, NULL);
			ipComboBox = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_AUTOHSCROLL, 335, 117, 150, 200, hwnd, NULL, NULL, NULL); // ������CBS_AUTOHSCROLL�Ա������������
			SendMessage(ipComboBox, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0)); // ���´���������Ӧ�����ı��ؼ�
			//SendMessage(ipComboBox, CB_ADDSTRING, 0, (LPARAM) "192.168.60.250");
			//SendMessage(ipComboBox, CB_ADDSTRING, 0, (LPARAM) "192.168.60.199");
			//SendMessage(ipComboBox, CB_SETCURSEL, 0, 0);
			sharedFolderLabel = CreateWindow("STATIC", "����Ŀ¼��", WS_VISIBLE | WS_CHILD, 265, 150, 75, 20, hwnd, NULL, NULL, NULL);
			sharedFolderEdit = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_AUTOHSCROLL, 335, 147, 150, 200, hwnd, NULL, NULL, NULL); // ������CBS_AUTOHSCROLL�Ա������������
			SendMessage(sharedFolderEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0)); // ���´���������Ӧ�����ı��ؼ�
			//SendMessage(sharedFolderEdit, CB_ADDSTRING, 0, (LPARAM) "�������");
			//SendMessage(sharedFolderEdit, CB_ADDSTRING, 0, (LPARAM) "home");
			//SendMessage(sharedFolderEdit, CB_SETCURSEL, 0, 0);
			driveLabel = CreateWindow("STATIC", "�̷���", WS_VISIBLE | WS_CHILD, 265, 180, 80, 20, hwnd, NULL, NULL, NULL);
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
			SendMessage(driveComboBox, CB_SETCURSEL, 0, 0); // �����б��Ĭ��ѡ��Ϊ�б�ĵ�һ��
			countdownLabel = CreateWindow("STATIC", "��Ͽ����л��У�", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 265, 210, 180, 20, hwnd, NULL, NULL, NULL);
			connectButton = CreateWindow("BUTTON", "ӳ��", WS_VISIBLE | WS_CHILD | BS_FLAT, 265, 234, 105, 28, hwnd, (HMENU) 1, NULL, NULL);
			disconnectButton = CreateWindow("BUTTON", "�Ͽ�ȫ��", WS_VISIBLE | WS_CHILD | BS_FLAT, 380, 234, 105, 28, hwnd, (HMENU) 2, NULL, NULL);
			ForcedisconnectallButton = CreateWindow("BUTTON", "ǿ�ƶϿ�ȫ��(������Դ������)", WS_VISIBLE | WS_CHILD | BS_FLAT, 265, 267, 220, 28, hwnd, (HMENU) 333, NULL, NULL);
			GenerateBatchProcessing = CreateWindow("BUTTON", "����ר�����ҵ�ӳ���������ļ�", WS_VISIBLE | WS_CHILD | BS_FLAT, 265, 300, 220, 28, hwnd, (HMENU) 3339, NULL, NULL);

			hGroupBoxSettings = CreateWindow("BUTTON", "����/����", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 10, 35, 240, 300, hwnd, NULL, NULL, NULL);
			yingsheshezhi = CreateWindow("Static", "ӳ������", WS_CHILD | WS_VISIBLE, 15, 195, 60, 20, hwnd, NULL, hInst, NULL);
			showPasswordCheckbox = CreateWindow("BUTTON", "��ʾ����", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 15, 215, 80, 20, hwnd, (HMENU)999, NULL, NULL);
			LockPassword = CreateWindow("BUTTON", "��������", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 105, 215, 80, 20, hwnd, NULL, NULL, NULL);
			daojishi = CreateWindow("Static", "����ʱ��ʱ�䵽��Ͽ�ȫ����", WS_CHILD | WS_VISIBLE, 15, 115, 200, 20, hwnd, NULL, hInst, NULL);
			countdownCheckbox = CreateWindow("BUTTON", "�״�ӳ��ɹ���������ʱ", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 15, 135, 200, 20, hwnd, (HMENU)3, NULL, NULL);
			hStaticSetCountdownTime = CreateWindow("Static", "���õ���ʱʱ�䣺", WS_CHILD | WS_VISIBLE, 15, 155, 120, 20, hwnd, NULL, hInst, NULL);
			countdownEdit = CreateWindow("EDIT", "45", WS_VISIBLE | WS_CHILD | WS_BORDER, 135, 155, 40, 20, hwnd, NULL, NULL, NULL);
			countdownUnitComboBox = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_DROPDOWNLIST, 175, 155, 40, 200, hwnd, NULL, NULL, NULL); // ����ʹ��CBS_DROPDOWNLIST��ʽʹ�ؼ��޷��༭
			SendMessage(countdownUnitComboBox, CB_ADDSTRING, 0, (LPARAM) "��");
			SendMessage(countdownUnitComboBox, CB_ADDSTRING, 0, (LPARAM) "��");
			SendMessage(countdownUnitComboBox, CB_SETCURSEL, 0, 0);
			jieshudaojishicheckbox = CreateWindow("BUTTON", "�Ͽ��󲻽�������ʱ", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 15, 175, 155, 20, hwnd, NULL, NULL, NULL);
			hStaticMappingKernel = CreateWindow("Static", "ӳ���ں�", WS_CHILD | WS_VISIBLE, 15, 55, 60, 20, hwnd, NULL, hInst, NULL);
			hBtnWNetAddConnection2A = CreateWindow("button", "ʹ��WNetAddConnection2A����", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 15, 75, 230, 20, hwnd, (HMENU)5, NULL, NULL);
			hBtnNetCommandLine = CreateWindow("button", "ʹ��NET������ʾ������", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 15, 95, 200, 20, hwnd, (HMENU)6, NULL, NULL);
			Saveusername = CreateWindow("BUTTON", "ӳ��ɹ��󱣴��û���", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 15, 235, 170, 20, hwnd, NULL, NULL, NULL);
			keyongpancheckbox = CreateWindow("BUTTON", "��ʾ��ռ�õ��̷�", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 15, 255, 140, 20, hwnd, (HMENU)198, NULL, NULL);
			zidongxuanze = CreateWindow("BUTTON", "ӳ��ɹ����Զ�ѡ����һ������Ŀ¼���̷�", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | BS_MULTILINE, 15, 275, 231, 35, hwnd, NULL, NULL, NULL); // ����ʹ��BS_MULTILINE��ʽʹ�ؼ��Զ�����
			detailedButton = CreateWindow("BUTTON", "��ϸ˵��", WS_VISIBLE | WS_CHILD | BS_FLAT, 175, 50, 70, 25, hwnd, (HMENU) 4, NULL, NULL);
			bujinyongxsmm = CreateWindow("BUTTON", "ӳ��ɹ��󲻽�����ʾ����", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 15, 310, 200, 20, hwnd, (HMENU)998, NULL, NULL);
			nextpage = CreateWindow("BUTTON", "��/��һҳ", WS_VISIBLE | WS_CHILD | BS_FLAT, 95, 50, 70, 25, hwnd, (HMENU) 7, NULL, NULL);
			SaveuserIP = CreateWindow("BUTTON", "ӳ��ɹ��󱣴�IP��ַ", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 15, 75, 170, 20, hwnd, NULL, NULL, NULL);
			Saveusershare = CreateWindow("BUTTON", "ӳ��ɹ��󱣴湲��Ŀ¼", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 15, 95, 170, 20, hwnd, NULL, NULL, NULL);
			yingsheshezhi2 = CreateWindow("Static", "ӳ������", WS_CHILD | WS_VISIBLE, 15, 55, 60, 20, hwnd, NULL, hInst, NULL);
			ShowWindow(SaveuserIP, SW_HIDE);
			ShowWindow(Saveusershare, SW_HIDE);
			ShowWindow(yingsheshezhi2, SW_HIDE);
			// ���� ʹ��WNetAddConnection2A���� ��ѡ���ѡ��״̬Ϊѡ��
			SendMessage(hBtnWNetAddConnection2A, BM_SETCHECK, BST_CHECKED, 0);
			//�Զ����û��л����û��������
			SetFocus(usernameEdit);
			// ���õ���ʱ
			EnableWindow(countdownLabel, FALSE);
			EnableWindow(countdownEdit, FALSE);
			EnableWindow(countdownUnitComboBox, FALSE);
			// ������ʱ��
			SetTimer(hwnd, 2, 0, NULL); // ������ʱ����ID��2
			// ���´���������Ӧ�����ı��ؼ�
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
			HFONT hFont2 = CreateFont(19.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("΢���ź�"));
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

		/* ��ʱ�� */
		case WM_TIMER: {

			if (wParam == 2) { // �жϼ�ʱ��ID

				// �ڼ�ʱ����Ϣ�������ж���һ������������һ�ε��̷�״̬
				static DWORD previousDriveMask = 0;
				if (SendMessage(keyongpancheckbox, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� ��ʾ��ռ�õ��̷� ��ѡ���Ƿ�δ����ѡ
					DWORD currentDriveMask = GetLogicalDrives();
					// ����̷�״̬�Ƿ����仯
					if (currentDriveMask != previousDriveMask) {
						bUpdateDriveList = TRUE;
						previousDriveMask = currentDriveMask;
					}
				}

				if (bUpdateDriveList == TRUE) {
					if (SendMessage(keyongpancheckbox, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� ��ʾ��ռ�õ��̷� ��ѡ���Ƿ�δ����ѡ
						SendMessage(driveComboBox, CB_RESETCONTENT, 0, 0); // ����б�ؼ�������
						// ������ã�δ��ռ�ã����̷����б�ؼ���
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
							char drive[2] = { drives[i], '\0' }; // ����ַ�����ֹ�� '\0'
							SendMessage(driveComboBox, CB_ADDSTRING, 0, (LPARAM)drive);
						}
						SendMessage(driveComboBox, CB_SETCURSEL, 0, 0); // �����б��Ĭ��ѡ��Ϊ�б�ĵ�һ��
					} else if (SendMessage(keyongpancheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) {
						SendMessage(driveComboBox, CB_RESETCONTENT, 0, 0); // ����б�ؼ�������
						// �����̷����б�ؼ���
						char driveLetter = 'Z';
						for (int i = 0; i < 26; i++) {
							char drive[2] = { driveLetter, '\0' };
							SendMessage(driveComboBox, CB_ADDSTRING, 0, (LPARAM)drive);
							driveLetter--;
						}
						SendMessage(driveComboBox, CB_SETCURSEL, 0, 0); // �����б��Ĭ��ѡ��Ϊ�б�ĵ�һ��
					}
					bUpdateDriveList = FALSE; // ����־����ΪFALSE�������ظ������б�����
				}

				// ��ȡ�����롱������е��ı�����
				int length = GetWindowTextLength(passwordEdit);
				if (length == 0) { // ��������Ϊ��
					EnableWindow(showPasswordCheckbox, TRUE); // ���� ��ʾ���� ��ѡ��
				}

				// �жϵ�ǰλ���Ƿ���C�̣��û����洢.txt��
				char currentPath[MAX_PATH];
				GetCurrentDirectoryA(MAX_PATH, currentPath);
				std::string currentDrive = std::string(currentPath).substr(0, 2);
				if (currentDrive == "C:") {
					// C���У��ж�D���Ƿ����
					UINT driveType = GetDriveTypeA("D:\\");
					if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
						std::string folderPath = "D:\\��ݲ˵��洢";
						std::string filePath = folderPath + "\\�û����洢.txt";
						// �ж��ļ��Ƿ����
						if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
							// �ļ������ڣ�ɾ���û����������б�
							std::cout << "�ļ�������" << std::endl;
							if (!noContentExecuted) {
								DestroyWindow(usernameEdit); // ɾ���û��������
								// ����һ��û�������б��
								usernameEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 335, 57, 150, 26, hwnd, NULL, NULL, NULL); // ʹ��WS_EX_CLIENTEDGE��ʽʹ�������Ĭ��
								hasContentExecuted = false; // ���ñ�־����
								noContentExecuted = true; // ���ñ�־����
							}
						} else {
							// �ļ����ڣ���ȡ�û����б����������б�
							std::cout << "�ļ��Ѵ���" << std::endl;
							// �жϴ洢�ļ��Ƿ�������
							std::ifstream inputFile(filePath);
							if (inputFile.is_open()) {
								std::string line;
								if (std::getline(inputFile, line)) {
									// ����ļ������ݣ����������б�򲢽�������ӵ������б���
									if (!hasContentExecuted) {
										DestroyWindow(usernameEdit); // ɾ���û��������
										// ����һ���������б��
										usernameEdit = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | WS_VSCROLL | CBS_AUTOHSCROLL, 335, 57, 150, 200, hwnd, NULL, NULL, NULL); // ������CBS_AUTOHSCROLL�Ա������������
										SendMessage(usernameEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str()); //��ȡ�ļ��е�������ӵ������б���
										while (std::getline(inputFile, line)) {
											SendMessage(usernameEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str());
										}
										hasContentExecuted = true; // ���ñ�־����
										noContentExecuted = false; // ���ñ�־����
									} else {
										// ����ļ��Ƿ����仯
										std::ifstream file(filePath, std::ios::binary | std::ios::ate);
										static std::streampos previousSize = file.tellg();
										file.seekg(0, std::ios::end);
										std::streampos currentSize = file.tellg();
										if (currentSize != previousSize) {
											previousSize = currentSize;
											DestroyWindow(usernameEdit); // ɾ���û��������
											// ����һ���������б��
											usernameEdit = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | WS_VSCROLL | CBS_AUTOHSCROLL, 335, 57, 150, 200, hwnd, NULL, NULL, NULL); // ������CBS_AUTOHSCROLL�Ա������������
											SendMessage(usernameEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str()); //��ȡ�ļ��е�������ӵ������б���
											while (std::getline(inputFile, line)) {
												SendMessage(usernameEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str());
											}
											std::cout << "�ļ������仯" << std::endl;
											return true; // �ļ������仯
										}
										return false; // �ļ�δ�����仯
									}
								} else {
									// ����ļ�û�����ݣ�ɾ�������б��
									if (!noContentExecuted) {
										DestroyWindow(usernameEdit); // ɾ���û��������
										// ����һ��û�������б��
										usernameEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 335, 57, 150, 26, hwnd, NULL, NULL, NULL); // ʹ��WS_EX_CLIENTEDGE��ʽʹ�������Ĭ��
										hasContentExecuted = false; // ���ñ�־����
										noContentExecuted = true; // ���ñ�־����
									}
								}
								inputFile.close();
							}
						}
					} else {
						std::cout << "D�̲����ڻ�ǹ̶�/���ƶ�����" << std::endl;
					}
				} else {
					// ����C���У���ȡ��ǰ·��
					char currentPath[MAX_PATH];
					GetCurrentDirectoryA(MAX_PATH, currentPath);
					std::string folderPath = std::string(currentPath);
					std::string filePath = folderPath + "\\�û����洢.txt";
					// �ж��ļ��Ƿ����
					if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
						// �ļ������ڣ�ɾ���û����������б�
						std::cout << "�ļ�������" << std::endl;
						if (!noContentExecuted) {
							DestroyWindow(usernameEdit); // ɾ���û��������
							// ����һ��û�������б��
							usernameEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 335, 57, 150, 26, hwnd, NULL, NULL, NULL); // ʹ��WS_EX_CLIENTEDGE��ʽʹ�������Ĭ��
							hasContentExecuted = false; // ���ñ�־����
							noContentExecuted = true; // ���ñ�־����
						}
					} else {
						// �ļ����ڣ���ȡ�û����б����������б�
						std::cout << "�ļ��Ѵ���" << std::endl;
						// �жϴ洢�ļ��Ƿ�������
						std::ifstream inputFile(filePath);
						if (inputFile.is_open()) {
							std::string line;
							if (std::getline(inputFile, line)) {
								// ����ļ������ݣ����������б�򲢽�������ӵ������б���
								if (!hasContentExecuted) {
									DestroyWindow(usernameEdit); // ɾ���û��������
									// ����һ���������б��
									usernameEdit = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | WS_VSCROLL | CBS_AUTOHSCROLL, 335, 57, 150, 200, hwnd, NULL, NULL, NULL); // ������CBS_AUTOHSCROLL�Ա������������
									SendMessage(usernameEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str()); //��ȡ�ļ��е�������ӵ������б���
									while (std::getline(inputFile, line)) {
										SendMessage(usernameEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str());
									}
									hasContentExecuted = true; // ���ñ�־����
									noContentExecuted = false; // ���ñ�־����
								} else {
									// ����ļ��Ƿ����仯
									std::ifstream file(filePath, std::ios::binary | std::ios::ate);
									static std::streampos previousSize = file.tellg();
									file.seekg(0, std::ios::end);
									std::streampos currentSize = file.tellg();
									if (currentSize != previousSize) {
										previousSize = currentSize;
										DestroyWindow(usernameEdit); // ɾ���û��������
										// ����һ���������б��
										usernameEdit = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | WS_VSCROLL | CBS_AUTOHSCROLL, 335, 57, 150, 200, hwnd, NULL, NULL, NULL); // ������CBS_AUTOHSCROLL�Ա������������
										SendMessage(usernameEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str()); //��ȡ�ļ��е�������ӵ������б���
										while (std::getline(inputFile, line)) {
											SendMessage(usernameEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str());
										}
										std::cout << "�ļ������仯" << std::endl;
										return true; // �ļ������仯
									}
									return false; // �ļ�δ�����仯
								}
							} else {
								// ����ļ�û�����ݣ�ɾ�������б��
								if (!noContentExecuted) {
									DestroyWindow(usernameEdit); // ɾ���û��������
									// ����һ��û�������б��
									usernameEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 335, 57, 150, 26, hwnd, NULL, NULL, NULL); // ʹ��WS_EX_CLIENTEDGE��ʽʹ�������Ĭ��
									hasContentExecuted = false; // ���ñ�־����
									noContentExecuted = true; // ���ñ�־����
								}
							}
							inputFile.close();
						}
					}
					// ����һ��������ı�
					HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("΢���ź�"));
					// ���´���������Ӧ�����ı��ؼ�
					SendMessage(usernameEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
				}

				// �жϵ�ǰλ���Ƿ���C�̣�IP��ַ�洢.txt��
				char currentPath2[MAX_PATH];
				GetCurrentDirectoryA(MAX_PATH, currentPath2);
				std::string currentDrive2 = std::string(currentPath2).substr(0, 2);
				if (currentDrive2 == "C:") {
					// C���У��ж�D���Ƿ����
					UINT driveType2 = GetDriveTypeA("D:\\");
					if (driveType2 == DRIVE_FIXED || driveType2 == DRIVE_REMOVABLE) {
						std::string folderPath = "D:\\��ݲ˵��洢";
						std::string filePath = folderPath + "\\IP��ַ�洢.txt";
						// �ж��ļ��Ƿ����
						if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
							// �ļ������ڣ�ɾ��IP��ַ�������б�
							std::cout << "�ļ�������" << std::endl;
							if (!noContentExecuted2) {
								DestroyWindow(ipComboBox); // ɾ��IP��ַ�����
								// ����һ��û�������б��
								ipComboBox = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 335, 117, 150, 26, hwnd, NULL, NULL, NULL); // ��WS_EX_CLIENTEDGE��ʽʹ�������Ĭ��
								hasContentExecuted2 = false; // ���ñ�־����
								noContentExecuted2 = true; // ���ñ�־����
							}
						} else {
							// �ļ����ڣ���ȡIP��ַ�б����������б�
							std::cout << "�ļ��Ѵ���" << std::endl;
							// �жϴ洢�ļ��Ƿ�������
							std::ifstream inputFile(filePath);
							if (inputFile.is_open()) {
								std::string line;
								if (std::getline(inputFile, line)) {
									// ����ļ������ݣ����������б�򲢽�������ӵ������б���
									if (!hasContentExecuted2) {
										DestroyWindow(ipComboBox); // ɾ��IP��ַ�����
										// ����һ���������б��
										ipComboBox = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_AUTOHSCROLL, 335, 117, 150, 200, hwnd, NULL, NULL, NULL); // ������CBS_AUTOHSCROLL�Ա������������
										SendMessage(ipComboBox, CB_ADDSTRING, 0, (LPARAM)line.c_str()); //��ȡ�ļ��е�������ӵ������б���
										while (std::getline(inputFile, line)) {
											SendMessage(ipComboBox, CB_ADDSTRING, 0, (LPARAM)line.c_str());
										}
										hasContentExecuted2 = true; // ���ñ�־����
										noContentExecuted2 = false; // ���ñ�־����
									} else {
										// ����ļ��Ƿ����仯
										std::ifstream file(filePath, std::ios::binary | std::ios::ate);
										static std::streampos previousSize = file.tellg();
										file.seekg(0, std::ios::end);
										std::streampos currentSize = file.tellg();
										if (currentSize != previousSize) {
											previousSize = currentSize;
											DestroyWindow(ipComboBox); // ɾ��IP��ַ�����
											// ����һ���������б��
											ipComboBox = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_AUTOHSCROLL, 335, 117, 150, 200, hwnd, NULL, NULL, NULL); // ������CBS_AUTOHSCROLL�Ա������������
											SendMessage(ipComboBox, CB_ADDSTRING, 0, (LPARAM)line.c_str()); //��ȡ�ļ��е�������ӵ������б���
											while (std::getline(inputFile, line)) {
												SendMessage(ipComboBox, CB_ADDSTRING, 0, (LPARAM)line.c_str());
											}
											std::cout << "�ļ������仯" << std::endl;
											return true; // �ļ������仯
										}
										return false; // �ļ�δ�����仯
									}
								} else {
									// ����ļ�û�����ݣ�ɾ�������б��
									if (!noContentExecuted2) {
										DestroyWindow(ipComboBox); // ɾ��IP��ַ�����
										// ����һ��û�������б��
										ipComboBox = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 335, 117, 150, 26, hwnd, NULL, NULL, NULL); // ��WS_EX_CLIENTEDGE��ʽʹ�������Ĭ��
										hasContentExecuted2 = false; // ���ñ�־����
										noContentExecuted2 = true; // ���ñ�־����
									}
								}
								inputFile.close();
							}
						}
					} else {
						std::cout << "D�̲����ڻ�ǹ̶�/���ƶ�����" << std::endl;
					}
				} else {
					// ����C���У���ȡ��ǰ·��
					char currentPath[MAX_PATH];
					GetCurrentDirectoryA(MAX_PATH, currentPath);
					std::string folderPath = std::string(currentPath);
					std::string filePath = folderPath + "\\IP��ַ�洢.txt";
					// �ж��ļ��Ƿ����
					if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
						// �ļ������ڣ�ɾ��IP��ַ�������б�
						std::cout << "�ļ�������" << std::endl;
						if (!noContentExecuted2) {
							DestroyWindow(ipComboBox); // ɾ��IP��ַ�����
							// ����һ��û�������б��
							ipComboBox = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 335, 117, 150, 26, hwnd, NULL, NULL, NULL); // ��WS_EX_CLIENTEDGE��ʽʹ�������Ĭ��
							hasContentExecuted2 = false; // ���ñ�־����
							noContentExecuted2 = true; // ���ñ�־����
						}
					} else {
						// �ļ����ڣ���ȡIP��ַ�б����������б�
						std::cout << "�ļ��Ѵ���" << std::endl;
						// �жϴ洢�ļ��Ƿ�������
						std::ifstream inputFile(filePath);
						if (inputFile.is_open()) {
							std::string line;
							if (std::getline(inputFile, line)) {
								// ����ļ������ݣ����������б�򲢽�������ӵ������б���
								if (!hasContentExecuted2) {
									DestroyWindow(ipComboBox); // ɾ��IP��ַ�����
									// ����һ���������б��
									ipComboBox = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_AUTOHSCROLL, 335, 117, 150, 200, hwnd, NULL, NULL, NULL); // ������CBS_AUTOHSCROLL�Ա������������
									SendMessage(ipComboBox, CB_ADDSTRING, 0, (LPARAM)line.c_str()); //��ȡ�ļ��е�������ӵ������б���
									while (std::getline(inputFile, line)) {
										SendMessage(ipComboBox, CB_ADDSTRING, 0, (LPARAM)line.c_str());
									}
									hasContentExecuted2 = true; // ���ñ�־����
									noContentExecuted2 = false; // ���ñ�־����
								} else {
									// ����ļ��Ƿ����仯
									std::ifstream file(filePath, std::ios::binary | std::ios::ate);
									static std::streampos previousSize = file.tellg();
									file.seekg(0, std::ios::end);
									std::streampos currentSize = file.tellg();
									if (currentSize != previousSize) {
										previousSize = currentSize;
										DestroyWindow(ipComboBox); // ɾ��IP��ַ�����
										// ����һ���������б��
										ipComboBox = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_AUTOHSCROLL, 335, 117, 150, 200, hwnd, NULL, NULL, NULL); // ������CBS_AUTOHSCROLL�Ա������������
										SendMessage(ipComboBox, CB_ADDSTRING, 0, (LPARAM)line.c_str()); //��ȡ�ļ��е�������ӵ������б���
										while (std::getline(inputFile, line)) {
											SendMessage(ipComboBox, CB_ADDSTRING, 0, (LPARAM)line.c_str());
										}
										std::cout << "�ļ������仯" << std::endl;
										return true; // �ļ������仯
									}
									return false; // �ļ�δ�����仯
								}
							} else {
								// ����ļ�û�����ݣ�ɾ�������б��
								if (!noContentExecuted2) {
									DestroyWindow(ipComboBox); // ɾ��IP��ַ�����
									// ����һ��û�������б��
									ipComboBox = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 335, 117, 150, 26, hwnd, NULL, NULL, NULL); // ��WS_EX_CLIENTEDGE��ʽʹ�������Ĭ��
									hasContentExecuted2 = false; // ���ñ�־����
									noContentExecuted2 = true; // ���ñ�־����
								}
							}
							inputFile.close();
						}
					}
					// ����һ��������ı�
					HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("΢���ź�"));
					// ���´���������Ӧ�����ı��ؼ�
					SendMessage(ipComboBox, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
				}

				// ��ȡĬ��IP��ַ�洢�ļ�
				std::string defaultIPFilePath = "Ĭ��IP��ַ�洢.txt";
				// �жϴ洢�ļ��Ƿ�������
				std::ifstream defaultFile(defaultIPFilePath);
				if (defaultFile.is_open()) {
					std::string line;
					bool isFirstLine = true;
					std::vector<std::string> defaultIPList; // �洢Ĭ��IP��ַ���б�
					while (std::getline(defaultFile, line)) {
						if (isFirstLine) {
							if (ipComboBox == NULL) { // �ж������б��Ƿ񲻴���
								// ���������б��
								DestroyWindow(ipComboBox); // ɾ��IP��ַ�����
								ipComboBox = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_AUTOHSCROLL, 335, 117, 150, 200, hwnd, NULL, NULL, NULL); // ������CBS_AUTOHSCROLL�Ա������������
							}
							isFirstLine = false;
						}
						defaultIPList.push_back(line); // �����ݴ洢��defaultIPList��
					}
					defaultFile.close();
					// ��Ĭ��IP��ַ������ӵ�IP��ַ������У�ȷ��Ĭ�������ϲ���ʾ
					for (int i = defaultIPList.size() - 1; i >= 0; --i) {
						SendMessage(ipComboBox, CB_INSERTSTRING, 0, (LPARAM)defaultIPList[i].c_str());
					}
					// ����Ĭ��ѡ����Ϊ���һ����ȡ��Ĭ��IP��ַ
					SendMessage(ipComboBox, CB_SETCURSEL, 0, 0);
					// ����һ��������ı�
					HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("΢���ź�"));
					// ���´���������Ӧ�����ı��ؼ�
					SendMessage(ipComboBox, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
				}

				// �жϵ�ǰλ���Ƿ���C�̣�����Ŀ¼�洢.txt��
				char currentPath3[MAX_PATH];
				GetCurrentDirectoryA(MAX_PATH, currentPath3);
				std::string currentDrive3 = std::string(currentPath3).substr(0, 2);
				if (currentDrive3 == "C:") {
					// C���У��ж�D���Ƿ����
					UINT driveType2 = GetDriveTypeA("D:\\");
					if (driveType2 == DRIVE_FIXED || driveType2 == DRIVE_REMOVABLE) {
						std::string folderPath = "D:\\��ݲ˵��洢";
						std::string filePath = folderPath + "\\����Ŀ¼�洢.txt";
						// �ж��ļ��Ƿ����
						if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
							// �ļ������ڣ�ɾ������Ŀ¼�������б�
							std::cout << "�ļ�������" << std::endl;
							if (!noContentExecuted3) {
								DestroyWindow(sharedFolderEdit); // ɾ������Ŀ¼�����
								// ����һ��û�������б��
								sharedFolderEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 335, 147, 150, 26, hwnd, NULL, NULL, NULL); // ��WS_EX_CLIENTEDGE��ʽʹ�������Ĭ��
								hasContentExecuted3 = false; // ���ñ�־����
								noContentExecuted3 = true; // ���ñ�־����
							}
						} else {
							// �ļ����ڣ���ȡ����Ŀ¼�б����������б�
							std::cout << "�ļ��Ѵ���" << std::endl;
							// �жϴ洢�ļ��Ƿ�������
							std::ifstream inputFile(filePath);
							if (inputFile.is_open()) {
								std::string line;
								if (std::getline(inputFile, line)) {
									// ����ļ������ݣ����������б�򲢽�������ӵ������б���
									if (!hasContentExecuted3) {
										DestroyWindow(sharedFolderEdit); // ɾ������Ŀ¼�����
										// ����һ���������б��
										sharedFolderEdit = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_AUTOHSCROLL, 335, 147, 150, 200, hwnd, NULL, NULL, NULL); // ������CBS_AUTOHSCROLL�Ա������������
										SendMessage(sharedFolderEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str()); //��ȡ�ļ��е�������ӵ������б���
										while (std::getline(inputFile, line)) {
											SendMessage(sharedFolderEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str());
										}
										hasContentExecuted3 = true; // ���ñ�־����
										noContentExecuted3 = false; // ���ñ�־����
									} else {
										// ����ļ��Ƿ����仯
										std::ifstream file(filePath, std::ios::binary | std::ios::ate);
										static std::streampos previousSize = file.tellg();
										file.seekg(0, std::ios::end);
										std::streampos currentSize = file.tellg();
										if (currentSize != previousSize) {
											previousSize = currentSize;
											DestroyWindow(sharedFolderEdit); // ɾ������Ŀ¼�����
											// ����һ���������б��
											sharedFolderEdit = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_AUTOHSCROLL, 335, 147, 150, 200, hwnd, NULL, NULL, NULL); // ������CBS_AUTOHSCROLL�Ա������������
											SendMessage(sharedFolderEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str()); //��ȡ�ļ��е�������ӵ������б���
											while (std::getline(inputFile, line)) {
												SendMessage(sharedFolderEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str());
											}
											std::cout << "�ļ������仯" << std::endl;
											return true; // �ļ������仯
										}
										return false; // �ļ�δ�����仯
									}
								} else {
									// ����ļ�û�����ݣ�ɾ�������б��
									if (!noContentExecuted3) {
										DestroyWindow(sharedFolderEdit); // ɾ������Ŀ¼�����
										// ����һ��û�������б��
										sharedFolderEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 335, 147, 150, 26, hwnd, NULL, NULL, NULL); // ��WS_EX_CLIENTEDGE��ʽʹ�������Ĭ��
										hasContentExecuted3 = false; // ���ñ�־����
										noContentExecuted3 = true; // ���ñ�־����
									}
								}
								inputFile.close();
							}
						}
					} else {
						std::cout << "D�̲����ڻ�ǹ̶�/���ƶ�����" << std::endl;
					}
				} else {
					// ����C���У���ȡ��ǰ·��
					char currentPath[MAX_PATH];
					GetCurrentDirectoryA(MAX_PATH, currentPath);
					std::string folderPath = std::string(currentPath);
					std::string filePath = folderPath + "\\����Ŀ¼�洢.txt";
					// �ж��ļ��Ƿ����
					if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
						// �ļ������ڣ�ɾ������Ŀ¼�������б�
						std::cout << "�ļ�������" << std::endl;
						if (!noContentExecuted3) {
							DestroyWindow(sharedFolderEdit); // ɾ������Ŀ¼�����
							// ����һ��û�������б��
							sharedFolderEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 335, 147, 150, 26, hwnd, NULL, NULL, NULL); // ��WS_EX_CLIENTEDGE��ʽʹ�������Ĭ��
							hasContentExecuted3 = false; // ���ñ�־����
							noContentExecuted3 = true; // ���ñ�־����
						}
					} else {
						// �ļ����ڣ���ȡ����Ŀ¼�б����������б�
						std::cout << "�ļ��Ѵ���" << std::endl;
						// �жϴ洢�ļ��Ƿ�������
						std::ifstream inputFile(filePath);
						if (inputFile.is_open()) {
							std::string line;
							if (std::getline(inputFile, line)) {
								// ����ļ������ݣ����������б�򲢽�������ӵ������б���
								if (!hasContentExecuted3) {
									DestroyWindow(sharedFolderEdit); // ɾ������Ŀ¼�����
									// ����һ���������б��
									sharedFolderEdit = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_AUTOHSCROLL, 335, 147, 150, 200, hwnd, NULL, NULL, NULL); // ������CBS_AUTOHSCROLL�Ա������������
									SendMessage(sharedFolderEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str()); //��ȡ�ļ��е�������ӵ������б���
									while (std::getline(inputFile, line)) {
										SendMessage(sharedFolderEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str());
									}
									hasContentExecuted3 = true; // ���ñ�־����
									noContentExecuted3 = false; // ���ñ�־����
								} else {
									// ����ļ��Ƿ����仯
									std::ifstream file(filePath, std::ios::binary | std::ios::ate);
									static std::streampos previousSize = file.tellg();
									file.seekg(0, std::ios::end);
									std::streampos currentSize = file.tellg();
									if (currentSize != previousSize) {
										previousSize = currentSize;
										DestroyWindow(sharedFolderEdit); // ɾ������Ŀ¼�����
										// ����һ���������б��
										sharedFolderEdit = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_AUTOHSCROLL, 335, 147, 150, 200, hwnd, NULL, NULL, NULL); // ������CBS_AUTOHSCROLL�Ա������������
										SendMessage(sharedFolderEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str()); //��ȡ�ļ��е�������ӵ������б���
										while (std::getline(inputFile, line)) {
											SendMessage(sharedFolderEdit, CB_ADDSTRING, 0, (LPARAM)line.c_str());
										}
										std::cout << "�ļ������仯" << std::endl;
										return true; // �ļ������仯
									}
									return false; // �ļ�δ�����仯
								}
							} else {
								// ����ļ�û�����ݣ�ɾ�������б��
								if (!noContentExecuted3) {
									DestroyWindow(sharedFolderEdit); // ɾ������Ŀ¼�����
									// ����һ��û�������б��
									sharedFolderEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 335, 147, 150, 26, hwnd, NULL, NULL, NULL); // ��WS_EX_CLIENTEDGE��ʽʹ�������Ĭ��
									hasContentExecuted3 = false; // ���ñ�־����
									noContentExecuted3 = true; // ���ñ�־����
								}
							}
							inputFile.close();
						}
					}
					// ����һ��������ı�
					HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("΢���ź�"));
					// ���´���������Ӧ�����ı��ؼ�
					SendMessage(sharedFolderEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
				}

				// ��ȡĬ�Ϲ���Ŀ¼�洢�ļ����Ƽ��ı�ʹ��ANSI��ʽ��������ܲ�֧�����ģ�
				std::string defaultIPFilePath2 = "Ĭ�Ϲ���Ŀ¼�洢.txt";
				// �жϴ洢�ļ��Ƿ�������
				std::ifstream defaultFile2(defaultIPFilePath2);
				if (defaultFile2.is_open()) {
					std::string line;
					bool isFirstLine = true;
					std::vector<std::string> defaultIPList; // �洢Ĭ�Ϲ���Ŀ¼���б�
					while (std::getline(defaultFile2, line)) {
						if (isFirstLine) {
							if (sharedFolderEdit == NULL) { // �ж������б��Ƿ񲻴���
								// ���������б��
								DestroyWindow(sharedFolderEdit); // ɾ������Ŀ¼�����
								sharedFolderEdit = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_AUTOHSCROLL, 335, 147, 150, 200, hwnd, NULL, NULL, NULL); // ������CBS_AUTOHSCROLL�Ա������������
							}
							isFirstLine = false;
						}
						defaultIPList.push_back(line); // �����ݴ洢��defaultIPList��
					}
					defaultFile2.close();
					// ��Ĭ�Ϲ���Ŀ¼������ӵ�����Ŀ¼������У�ȷ��Ĭ�������ϲ���ʾ
					for (int i = defaultIPList.size() - 1; i >= 0; --i) {
						SendMessage(sharedFolderEdit, CB_INSERTSTRING, 0, (LPARAM)defaultIPList[i].c_str());
					}
					// ����Ĭ��ѡ����Ϊ���һ����ȡ��Ĭ�Ϲ���Ŀ¼
					SendMessage(sharedFolderEdit, CB_SETCURSEL, 0, 0);
					// ����һ��������ı�
					HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("΢���ź�"));
					// ���´���������Ӧ�����ı��ؼ�
					SendMessage(sharedFolderEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
				}

			}

			if (wParam == 1) { // �жϼ�ʱ��ID
				if (countdownDuration > 0 || countdownMinutes > 0) {
					if (SendMessage(countdownUnitComboBox, CB_GETCURSEL, 0, 0) == 0) { // ѡ�����"��"
						if (countdownDuration == 0 && countdownMinutes > 0) {
							countdownMinutes--; // ���ٵ���ʱ�ķ���
							countdownDuration = 59; // ������ʱ��������Ϊ59
						} else {
							countdownDuration--; // ���ٵ���ʱ����
						}
					} else if (SendMessage(countdownUnitComboBox, CB_GETCURSEL, 0, 0) == 1) { // ѡ�����"��"
						if (countdownMinutes > 0) {
							countdownMinutes--; // ���ٵ���ʱ�ķ��ӣ��룩
						}
					}
					// ���µ���ʱ��ǩ
					char countdownLabelBuf[256];
					if (SendMessage(countdownUnitComboBox, CB_GETCURSEL, 0, 0) == 0) { // ѡ�����"��"
						sprintf(countdownLabelBuf, "��Ͽ����л��У�%02d:%02d", countdownMinutes, countdownDuration);
					} else if (SendMessage(countdownUnitComboBox, CB_GETCURSEL, 0, 0) == 1) { // ѡ�����"��"
						sprintf(countdownLabelBuf, "��Ͽ����л��У�%02d��", countdownMinutes);
					}
					SetWindowText(countdownLabel, countdownLabelBuf);

					if (countdownMinutes == 0 && countdownDuration == 0) {
						// ������ʱ�ﵽ��ʱ�Ͽ�ӳ���������
						if (SendMessage(hBtnWNetAddConnection2A, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �жϵ�ѡ���ĸ���ѡ��
							typedef DWORD(WINAPI * LPWNETCANCELCONNECTION2A)(LPCSTR, DWORD, BOOL);
							// ���ؿ��ļ�
							HMODULE hMpr = LoadLibrary("mpr.dll");
							if (hMpr == NULL) {
								std::cout << "�޷�����mpr.dll���ļ�" << std::endl;
								return 1;
							}
							// ��ȡ������ַ
							LPWNETCANCELCONNECTION2A pWNetCancelConnection2A = (LPWNETCANCELCONNECTION2A)GetProcAddress(hMpr, "WNetCancelConnection2A");
							if (pWNetCancelConnection2A == NULL) {
								std::cout << "�޷���ȡWNetCancelConnection2A������ַ" << std::endl;
								FreeLibrary(hMpr);
								return 1;
							}
							DWORD result = NO_ERROR; // ����һ��result����
							// ѭ���Ͽ���������������
							for (char driveLetter = 'Z'; driveLetter >= 'A'; --driveLetter) {
								std::string drive = std::string(1, driveLetter) + ":";
								result = pWNetCancelConnection2A(drive.c_str(), CONNECT_UPDATE_PROFILE, TRUE);
								if (result == NO_ERROR) {
									std::cout << "���������� " << drive << " �Ͽ��ɹ���OwO�����룺" << result << "��" << std::endl;
								} else {
									std::cout << "���������� " << drive << " �Ͽ�ʧ�ܣ�X_X�����룺" << result << "��" << std::endl;
								}
							}
							// �ͷſ��ļ�
							FreeLibrary(hMpr);
							//ȡ�����õ���ʱ
							EnableWindow(countdownEdit, TRUE);
							EnableWindow(countdownUnitComboBox, TRUE);
							// ɾ�ɵ���ʱ
							DestroyWindow(countdownLabel);
							// ���µ���ʱ
							countdownLabel = CreateWindow("STATIC", "��Ͽ����л��У�", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 265, 210, 180, 20, hwnd, NULL, NULL, NULL);
							// ��ʾ��Ϣ��
							std::string komplettMessage = "�ѶϿ�����ӳ���������������������\n�����룺" + std::to_string(result) + "��";
							MessageBox(hwnd, komplettMessage.c_str(), "����ʱ��������(>��-*)/", MB_OK | MB_ICONINFORMATION);
						} else if (SendMessage(hBtnNetCommandLine, BM_GETCHECK, 0, 0) == BST_CHECKED) {
							string command = "net use * /del /Y";
							system(command.c_str());
							//ȡ�����õ���ʱ
							EnableWindow(countdownEdit, TRUE);
							EnableWindow(countdownUnitComboBox, TRUE);
							// ɾ�ɵ���ʱ
							DestroyWindow(countdownLabel);
							// ���µ���ʱ
							countdownLabel = CreateWindow("STATIC", "��Ͽ����л��У�", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 265, 210, 180, 20, hwnd, NULL, NULL, NULL);
							// ��ʾһ����Ϣ�������������ѶϿ�ӳ��
							MessageBox(hwnd, "�ѶϿ�ȫ��ӳ�����������������", "����ʱ��������(>��-*)/", MB_OK | MB_ICONINFORMATION);
						}
					}
					// ����һ��������ı�
					HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("΢���ź�"));
					// ���´���������Ӧ�����ı��ؼ�
					SendMessage(jieshudaojishicheckbox, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
				}
			}
			break;
		}

		case WM_COMMAND: { // ����ť����¼�
			switch (wParam) {

				case 198: { // ��ʾ��ռ�õ��̷�
					bUpdateDriveList = TRUE;
					break; // �˳�ѭ�������û�������������һ�еĴ��룬��������Ӧ��������
				}

				case 999: { // ��ʾ����
					if (SendMessage(showPasswordCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �ж� ��ʾ���� ��ѡ���Ƿ񱻹�ѡ
						// ��ȡ����������������
						char passwordBuf[256];
						GetWindowText(passwordEdit, passwordBuf, sizeof(passwordBuf));
						// ɾ���ɵ������ַ����������
						DestroyWindow(passwordEdit);
						// �����µ��������������
						passwordEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 335, 87, 150, 26, hwnd, NULL, NULL, NULL);
						// �����ݸ��Ƶ������������
						SetWindowText(passwordEdit, passwordBuf);
					} else if (SendMessage(showPasswordCheckbox, BM_GETCHECK, 0, 0) == BST_UNCHECKED) {
						// ��ȡ����������������
						char passwordBuf[256];
						GetWindowText(passwordEdit, passwordBuf, sizeof(passwordBuf));
						// ɾ���ɵ��������������
						DestroyWindow(passwordEdit);
						// �����µ������ַ����������
						passwordEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD | ES_AUTOHSCROLL, 335, 87, 150, 26, hwnd, NULL, NULL, NULL);
						// �����ݸ��Ƶ������������
						SetWindowText(passwordEdit, passwordBuf);
					}
					// ����һ��������ı�
					HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("΢���ź�"));
					// ���´���������Ӧ�����ı��ؼ�
					SendMessage(passwordEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
					break; // �˳�ѭ�������û�������������һ�еĴ��룬��������Ӧ��������
				}

				case 998 : { // ӳ��ɹ��󲻽�����ʾ����
					if (SendMessage(bujinyongxsmm, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �ж� ӳ��ɹ��󲻽�����ʾ���� ��ѡ���Ƿ񱻹�ѡ
						// ��ѡ
						MessageBox(hwnd, "����ѡ��ӳ��ɹ��󲻽�����ʾ���븴ѡ�򣬵�ӳ��ɹ���\n�����������ʾ���븴ѡ�򣬲������벻���л�Ϊ�����ַ�\n����ӳ�����������������Է���������й¶��", "��ʾ(�ޣߣ�)", MB_OK | MB_ICONINFORMATION);
					}
					break;
				}

				case 1: {  // ӳ��Զ�̹����ļ���
					// ��ȡ�ؼ�ֵ
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

					// �ж��Ƿ����û���������
					if (username.empty() && password.empty()) {
						// û���û��������룬�л���û���û�������������ӷ�ʽ
						if (SendMessage(hBtnWNetAddConnection2A, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �жϵ�ѡ���ĸ���ѡ��
							typedef DWORD(WINAPI * LPWNETADDCONNECTION2A)(LPNETRESOURCEA, LPCSTR, LPCSTR, DWORD);
							// ���ؿ��ļ�
							HMODULE hMpr = LoadLibrary("mpr.dll");
							if (hMpr == NULL) {
								std::cout << "�޷�����mpr.dll���ļ�" << std::endl;
								return 1;
							}
							// ��ȡ������ַ
							LPWNETADDCONNECTION2A pWNetAddConnection2A = (LPWNETADDCONNECTION2A)GetProcAddress(hMpr, "WNetAddConnection2A");
							if (pWNetAddConnection2A == NULL) {
								std::cout << "�޷���ȡWNetAddConnection2A������ַ" << std::endl;
								FreeLibrary(hMpr);
								return 1;
							}
							// ����Ҫӳ��������������ı���·����Զ��·��
							std::string localDrive = drive + ":";
							std::string remotePath = "\\\\" + ipAddress + "\\" + sharedFolder;
							const char* localDrivePtr = localDrive.c_str();
							const char* remotePathPtr = remotePath.c_str();
							// ����NETRESOURCEA�ṹ�壬����ʼ��
							NETRESOURCEA resource = {
								.dwType = RESOURCETYPE_DISK,
								.lpLocalName = const_cast<char*>(localDrivePtr),
								.lpRemoteName = const_cast<char*>(remotePathPtr)
							};
							// ʹ��WNetAddConnection2A����ӳ������������
							DWORD result = pWNetAddConnection2A(&resource, nullptr, nullptr, CONNECT_UPDATE_PROFILE);
							// ���ӳ����
							if (result == NO_ERROR) {
								if (countdownMinutes == 0 && countdownDuration == 0) { // �������countdownMinutes���֣���countdownDuration���룩������0ʱ
									if (SendMessage(countdownCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �ж� ӳ��ɹ���������ʱ ��ѡ���Ƿ񱻹�ѡ
										// ��ѡ
										// ���õ���ʱ����
										countdownMinutes = 0;
										countdownDuration = 0;
										// ���ü�ʱ���ļ��ʱ��
										int selectedIndex = SendMessage(countdownUnitComboBox, CB_GETCURSEL, 0, 0);
										// ��������ʱ��ʱ��
										SetTimer(hwnd, 1, 1000, NULL); // ��ʱ��ID: 1�����ʱ��Ϊ1000���루1�룩
										// ��������ʱ
										char countdownBuf[256];
										GetWindowText(countdownEdit, countdownBuf, sizeof(countdownBuf));
										countdownMinutes = atoi(countdownBuf); // ���ַ���ת��Ϊ����
										//���õ���ʱ
										EnableWindow(countdownEdit, FALSE);
										EnableWindow(countdownUnitComboBox, FALSE);
									}
								}
								if (SendMessage(SaveuserIP, BM_GETCHECK, 0, 0) == BST_CHECKED) { // ��ѡ��ѡ��
									// �жϵ�ǰλ���Ƿ���C��
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C���У��ж�D���Ƿ����
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D�̴��ڣ��ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
											std::string folderPath = "D:\\��ݲ˵��洢";
											std::string filePath = folderPath + "\\IP��ַ�洢.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "�ɹ������ļ���" << folderPath << std::endl;
												} else {
													std::cout << "�����ļ���ʧ��" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "�ɹ������ļ�" << filePath << std::endl;
												} else {
													std::cout << "�����ļ�ʧ��" << std::endl;
													return 0;
												}
											}
											// ��ȡIP��ַ
											char usernameBuf[256];
											GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// ����ļ����Ƿ������ͬ���û���
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
											// �����������ͬ���û�������洢�û������ļ���
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "�ɹ��洢�û���" << std::endl;
												} else {
													std::cout << "�洢�û���ʧ��" << std::endl;
												}
											} else {
												std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
											}
										} else {
											std::cout << "û��D��" << std::endl;
										}
									} else {
										// ����C���У���ȡ��ǰ·�����ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\IP��ַ�洢.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "�ɹ������ļ���" << folderPath << std::endl;
											} else {
												std::cout << "�����ļ���ʧ��" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "�ɹ������ļ�" << filePath << std::endl;
											} else {
												std::cout << "�����ļ�ʧ��" << std::endl;
												return 0;
											}
										}
										// ��ȡIP��ַ
										char usernameBuf[256];
										GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// ����ļ����Ƿ������ͬ���û���
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
										// �����������ͬ���û�������洢�û������ļ���
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "�ɹ��洢�û���" << std::endl;
											} else {
												std::cout << "�洢�û���ʧ��" << std::endl;
											}
										} else {
											std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
										}
									}
								}
								if (SendMessage(Saveusershare, BM_GETCHECK, 0, 0) == BST_CHECKED) { // ��ѡ��ѡ��
									// �жϵ�ǰλ���Ƿ���C��
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C���У��ж�D���Ƿ����
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D�̴��ڣ��ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
											std::string folderPath = "D:\\��ݲ˵��洢";
											std::string filePath = folderPath + "\\����Ŀ¼�洢.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "�ɹ������ļ���" << folderPath << std::endl;
												} else {
													std::cout << "�����ļ���ʧ��" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "�ɹ������ļ�" << filePath << std::endl;
												} else {
													std::cout << "�����ļ�ʧ��" << std::endl;
													return 0;
												}
											}
											// ��ȡ����Ŀ¼
											char usernameBuf[256];
											GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// ����ļ����Ƿ������ͬ���û���
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
											// �����������ͬ���û�������洢�û������ļ���
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "�ɹ��洢�û���" << std::endl;
												} else {
													std::cout << "�洢�û���ʧ��" << std::endl;
												}
											} else {
												std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
											}
										} else {
											std::cout << "û��D��" << std::endl;
										}
									} else {
										// ����C���У���ȡ��ǰ·�����ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\����Ŀ¼�洢.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "�ɹ������ļ���" << folderPath << std::endl;
											} else {
												std::cout << "�����ļ���ʧ��" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "�ɹ������ļ�" << filePath << std::endl;
											} else {
												std::cout << "�����ļ�ʧ��" << std::endl;
												return 0;
											}
										}
										// ��ȡ����Ŀ¼
										char usernameBuf[256];
										GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// ����ļ����Ƿ������ͬ���û���
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
										// �����������ͬ���û�������洢�û������ļ���
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "�ɹ��洢�û���" << std::endl;
											} else {
												std::cout << "�洢�û���ʧ��" << std::endl;
											}
										} else {
											std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
										}
									}
								}
								if (SendMessage(bujinyongxsmm, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� ӳ��ɹ��󲻽�ֹ��ʾ���� ��ѡ���Ƿ�δ����ѡ
									EnableWindow(showPasswordCheckbox, FALSE); // ���� ��ʾ���� ��ѡ��
									SendMessage(showPasswordCheckbox, BM_SETCHECK, BST_UNCHECKED, 0); // ���� showPasswordCheckbox ��ѡ���ѡ��״̬Ϊδѡ��
									LONG_PTR style = GetWindowLongPtr(passwordEdit, GWL_STYLE); // ��ȡ��������ʽ
									if (!(style & ES_PASSWORD)) { // �ж��������ʽ���Ƿ񲻰���ES_PASSWORD��־
										/* �����������������Ϊ���ģ�������ES_PASSWORD�� */
										// ��ȡ����������������
										char passwordBuf[256];
										GetWindowText(passwordEdit, passwordBuf, sizeof(passwordBuf));
										// ɾ���ɵ��������������
										DestroyWindow(passwordEdit);
										// �����µ������ַ����������
										passwordEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD | ES_AUTOHSCROLL, 335, 87, 150, 26, hwnd, NULL, NULL, NULL);
										// �����ݸ��Ƶ������������
										SetWindowText(passwordEdit, passwordBuf);
									}
								}
								std::string successMessage = "����������ӳ��ɹ����t(������)�q\n�����룺" + std::to_string(result) + "��";
								MessageBox(hwnd, successMessage.c_str(), "�ɹ�����(>��-*)/", MB_OK | MB_ICONINFORMATION);
							} else {
								std::cout << "����������ӳ��ʧ�ܣ�\n�����룺" << result << "��" << std::endl;
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� �������� ��ѡ���Ƿ�δ����ѡ
									SetWindowText(passwordEdit, ""); // �������������ı�����Ϊ���ַ���
								}
								std::string errorMessage = "����������ӳ��ʧ�ܣ�(�i�s^�t�i)\n�����룺" + std::to_string(result) + "��";
								MessageBox(hwnd, errorMessage.c_str(), "�����ˣ��r(�s�n�t���q", MB_OK | MB_ICONERROR);
							}
							// �ͷſ��ļ�
							FreeLibrary(hMpr);
						} else if (SendMessage(hBtnNetCommandLine, BM_GETCHECK, 0, 0) == BST_CHECKED) {
							string command = "net use ";
							command += drive;
							command += ": \\\\";
							command += ipAddress;
							command += "\\" + sharedFolder;
							int result = system(command.c_str());
							if (result == 0) {
								if (countdownMinutes == 0 && countdownDuration == 0) { // �������countdownMinutes���֣���countdownDuration���룩������0ʱ
									if (SendMessage(countdownCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �ж� ӳ��ɹ���������ʱ ��ѡ���Ƿ񱻹�ѡ
										// ��ѡ
										// ���õ���ʱ����
										countdownMinutes = 0;
										countdownDuration = 0;
										// ���ü�ʱ���ļ��ʱ��
										int selectedIndex = SendMessage(countdownUnitComboBox, CB_GETCURSEL, 0, 0);
										// ��������ʱ��ʱ��
										SetTimer(hwnd, 1, 1000, NULL); // ��ʱ��ID: 1�����ʱ��Ϊ1000���루1�룩
										// ��������ʱ
										char countdownBuf[256];
										GetWindowText(countdownEdit, countdownBuf, sizeof(countdownBuf));
										countdownMinutes = atoi(countdownBuf); // ���ַ���ת��Ϊ����
										//���õ���ʱ
										EnableWindow(countdownEdit, FALSE);
										EnableWindow(countdownUnitComboBox, FALSE);
									}
								}
								if (SendMessage(SaveuserIP, BM_GETCHECK, 0, 0) == BST_CHECKED) { // ��ѡ��ѡ��
									// �жϵ�ǰλ���Ƿ���C��
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C���У��ж�D���Ƿ����
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D�̴��ڣ��ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
											std::string folderPath = "D:\\��ݲ˵��洢";
											std::string filePath = folderPath + "\\IP��ַ�洢.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "�ɹ������ļ���" << folderPath << std::endl;
												} else {
													std::cout << "�����ļ���ʧ��" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "�ɹ������ļ�" << filePath << std::endl;
												} else {
													std::cout << "�����ļ�ʧ��" << std::endl;
													return 0;
												}
											}
											// ��ȡIP��ַ
											char usernameBuf[256];
											GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// ����ļ����Ƿ������ͬ���û���
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
											// �����������ͬ���û�������洢�û������ļ���
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "�ɹ��洢�û���" << std::endl;
												} else {
													std::cout << "�洢�û���ʧ��" << std::endl;
												}
											} else {
												std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
											}
										} else {
											std::cout << "û��D��" << std::endl;
										}
									} else {
										// ����C���У���ȡ��ǰ·�����ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\IP��ַ�洢.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "�ɹ������ļ���" << folderPath << std::endl;
											} else {
												std::cout << "�����ļ���ʧ��" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "�ɹ������ļ�" << filePath << std::endl;
											} else {
												std::cout << "�����ļ�ʧ��" << std::endl;
												return 0;
											}
										}
										// ��ȡIP��ַ
										char usernameBuf[256];
										GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// ����ļ����Ƿ������ͬ���û���
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
										// �����������ͬ���û�������洢�û������ļ���
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "�ɹ��洢�û���" << std::endl;
											} else {
												std::cout << "�洢�û���ʧ��" << std::endl;
											}
										} else {
											std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
										}
									}
								}
								if (SendMessage(Saveusershare, BM_GETCHECK, 0, 0) == BST_CHECKED) { // ��ѡ��ѡ��
									// �жϵ�ǰλ���Ƿ���C��
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C���У��ж�D���Ƿ����
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D�̴��ڣ��ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
											std::string folderPath = "D:\\��ݲ˵��洢";
											std::string filePath = folderPath + "\\����Ŀ¼�洢.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "�ɹ������ļ���" << folderPath << std::endl;
												} else {
													std::cout << "�����ļ���ʧ��" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "�ɹ������ļ�" << filePath << std::endl;
												} else {
													std::cout << "�����ļ�ʧ��" << std::endl;
													return 0;
												}
											}
											// ��ȡ����Ŀ¼
											char usernameBuf[256];
											GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// ����ļ����Ƿ������ͬ���û���
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
											// �����������ͬ���û�������洢�û������ļ���
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "�ɹ��洢�û���" << std::endl;
												} else {
													std::cout << "�洢�û���ʧ��" << std::endl;
												}
											} else {
												std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
											}
										} else {
											std::cout << "û��D��" << std::endl;
										}
									} else {
										// ����C���У���ȡ��ǰ·�����ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\����Ŀ¼�洢.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "�ɹ������ļ���" << folderPath << std::endl;
											} else {
												std::cout << "�����ļ���ʧ��" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "�ɹ������ļ�" << filePath << std::endl;
											} else {
												std::cout << "�����ļ�ʧ��" << std::endl;
												return 0;
											}
										}
										// ��ȡ����Ŀ¼
										char usernameBuf[256];
										GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// ����ļ����Ƿ������ͬ���û���
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
										// �����������ͬ���û�������洢�û������ļ���
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "�ɹ��洢�û���" << std::endl;
											} else {
												std::cout << "�洢�û���ʧ��" << std::endl;
											}
										} else {
											std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
										}
									}
								}
								// ���ӳɹ�
								if (SendMessage(zidongxuanze, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� ӳ��ɹ����Զ�ѡ����һ������Ŀ¼���̷� ��ѡ���Ƿ�δ����ѡ
									// ����"����Ŀ¼"����һ��ѡ��Ϊѡ��״̬�����Ѿ������һ���򷵻ص�һ��
									int currentSelection = SendMessage(sharedFolderEdit, CB_GETCURSEL, 0, 0);
									int itemCount = SendMessage(sharedFolderEdit, CB_GETCOUNT, 0, 0);
									int nextSelection = (currentSelection + 1) % itemCount; // ��ȡ��һ��ѡ������������Ѿ������һ���򷵻ص�һ��
									SendMessage(sharedFolderEdit, CB_SETCURSEL, nextSelection, 0);
									if (SendMessage(keyongpancheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �ж� ��ʾ��ռ�õ��̷� ��ѡ���Ƿ񱻹�ѡ
										// ����"�̷�"����һ��ѡ��Ϊѡ��״̬�����Ѿ������һ���򷵻ص�һ��
										currentSelection = SendMessage(driveComboBox, CB_GETCURSEL, 0, 0);
										itemCount = SendMessage(driveComboBox, CB_GETCOUNT, 0, 0);
										nextSelection = (currentSelection + 1) % itemCount; // ��ȡ��һ��ѡ������������Ѿ������һ���򷵻ص�һ��
										SendMessage(driveComboBox, CB_SETCURSEL, nextSelection, 0);
									}
								}
								MessageBox(hwnd, "û���κ����⣡qwq", "�ɹ�����qwq", MB_OK | MB_ICONINFORMATION);
							} else {
								// ����ʧ��
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� �������� ��ѡ���Ƿ�δ����ѡ
									SetWindowText(passwordEdit, ""); // �������������ı�����Ϊ���ַ���
								}
								MessageBox(hwnd, "���˵�С���⣡X_X", "�����ˣ�X_X", MB_OK | MB_ICONERROR);
							}
						}
					} else if (username.empty()) {
						// û���û������л���û���û��������ӷ�ʽ
						if (SendMessage(hBtnWNetAddConnection2A, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �жϵ�ѡ���ĸ���ѡ��
							typedef DWORD(WINAPI * LPWNETADDCONNECTION2A)(LPNETRESOURCEA, LPCSTR, LPCSTR, DWORD);
							// ���ؿ��ļ�
							HMODULE hMpr = LoadLibrary("mpr.dll");
							if (hMpr == NULL) {
								std::cout << "�޷�����mpr.dll���ļ�" << std::endl;
								return 1;
							}
							// ��ȡ������ַ
							LPWNETADDCONNECTION2A pWNetAddConnection2A = (LPWNETADDCONNECTION2A)GetProcAddress(hMpr, "WNetAddConnection2A");
							if (pWNetAddConnection2A == NULL) {
								std::cout << "�޷���ȡWNetAddConnection2A������ַ" << std::endl;
								FreeLibrary(hMpr);
								return 1;
							}
							// ����Ҫӳ��������������ı���·����Զ��·��
							std::string localDrive = drive + ":";
							std::string remotePath = "\\\\" + ipAddress + "\\" + sharedFolder;
							const char* localDrivePtr = localDrive.c_str();
							const char* remotePathPtr = remotePath.c_str();
							// ���� NETRESOURCEA �ṹ�壬����ʼ��
							NETRESOURCEA resource = {
								.dwType = RESOURCETYPE_DISK,
								.lpLocalName = const_cast<char*>(localDrivePtr),
								.lpRemoteName = const_cast<char*>(remotePathPtr),
							};
							// ����Ҫӳ�������������������
							std::string passwordStr = password;
							const char* passwordPtr = passwordStr.c_str();
							// ʹ��WNetAddConnection2A����ӳ������������
							DWORD result = pWNetAddConnection2A(&resource, passwordPtr, NULL, CONNECT_UPDATE_PROFILE);
							// ���ӳ����
							if (result == NO_ERROR) {
								std::cout << "����������ӳ��ɹ���\n�����룺" << result << "��" << std::endl;
								if (SendMessage(zidongxuanze, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� ӳ��ɹ����Զ�ѡ����һ������Ŀ¼���̷� ��ѡ���Ƿ�δ����ѡ
									// ����"����Ŀ¼"����һ��ѡ��Ϊѡ��״̬�����Ѿ������һ���򷵻ص�һ��
									int currentSelection = SendMessage(sharedFolderEdit, CB_GETCURSEL, 0, 0);
									int itemCount = SendMessage(sharedFolderEdit, CB_GETCOUNT, 0, 0);
									int nextSelection = (currentSelection + 1) % itemCount; // ��ȡ��һ��ѡ������������Ѿ������һ���򷵻ص�һ��
									SendMessage(sharedFolderEdit, CB_SETCURSEL, nextSelection, 0);
									if (SendMessage(keyongpancheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �ж� ��ʾ��ռ�õ��̷� ��ѡ���Ƿ񱻹�ѡ
										// ����"�̷�"����һ��ѡ��Ϊѡ��״̬�����Ѿ������һ���򷵻ص�һ��
										currentSelection = SendMessage(driveComboBox, CB_GETCURSEL, 0, 0);
										itemCount = SendMessage(driveComboBox, CB_GETCOUNT, 0, 0);
										nextSelection = (currentSelection + 1) % itemCount; // ��ȡ��һ��ѡ������������Ѿ������һ���򷵻ص�һ��
										SendMessage(driveComboBox, CB_SETCURSEL, nextSelection, 0);
									}
								}
								if (countdownMinutes == 0 && countdownDuration == 0) { // �������countdownMinutes���֣���countdownDuration���룩������0ʱ
									if (SendMessage(countdownCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �ж� ӳ��ɹ���������ʱ ��ѡ���Ƿ񱻹�ѡ
										// ��ѡ
										// ���õ���ʱ����
										countdownMinutes = 0;
										countdownDuration = 0;
										// ���ü�ʱ���ļ��ʱ��
										int selectedIndex = SendMessage(countdownUnitComboBox, CB_GETCURSEL, 0, 0);
										// ��������ʱ��ʱ��
										SetTimer(hwnd, 1, 1000, NULL); // ��ʱ��ID: 1�����ʱ��Ϊ1000���루1�룩
										// ��������ʱ
										char countdownBuf[256];
										GetWindowText(countdownEdit, countdownBuf, sizeof(countdownBuf));
										countdownMinutes = atoi(countdownBuf); // ���ַ���ת��Ϊ����
										//���õ���ʱ
										EnableWindow(countdownEdit, FALSE);
										EnableWindow(countdownUnitComboBox, FALSE);
									}
								}
								if (SendMessage(SaveuserIP, BM_GETCHECK, 0, 0) == BST_CHECKED) { // ��ѡ��ѡ��
									// �жϵ�ǰλ���Ƿ���C��
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C���У��ж�D���Ƿ����
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D�̴��ڣ��ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
											std::string folderPath = "D:\\��ݲ˵��洢";
											std::string filePath = folderPath + "\\IP��ַ�洢.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "�ɹ������ļ���" << folderPath << std::endl;
												} else {
													std::cout << "�����ļ���ʧ��" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "�ɹ������ļ�" << filePath << std::endl;
												} else {
													std::cout << "�����ļ�ʧ��" << std::endl;
													return 0;
												}
											}
											// ��ȡIP��ַ
											char usernameBuf[256];
											GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// ����ļ����Ƿ������ͬ���û���
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
											// �����������ͬ���û�������洢�û������ļ���
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "�ɹ��洢�û���" << std::endl;
												} else {
													std::cout << "�洢�û���ʧ��" << std::endl;
												}
											} else {
												std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
											}
										} else {
											std::cout << "û��D��" << std::endl;
										}
									} else {
										// ����C���У���ȡ��ǰ·�����ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\IP��ַ�洢.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "�ɹ������ļ���" << folderPath << std::endl;
											} else {
												std::cout << "�����ļ���ʧ��" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "�ɹ������ļ�" << filePath << std::endl;
											} else {
												std::cout << "�����ļ�ʧ��" << std::endl;
												return 0;
											}
										}
										// ��ȡIP��ַ
										char usernameBuf[256];
										GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// ����ļ����Ƿ������ͬ���û���
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
										// �����������ͬ���û�������洢�û������ļ���
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "�ɹ��洢�û���" << std::endl;
											} else {
												std::cout << "�洢�û���ʧ��" << std::endl;
											}
										} else {
											std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
										}
									}
								}
								if (SendMessage(Saveusershare, BM_GETCHECK, 0, 0) == BST_CHECKED) { // ��ѡ��ѡ��
									// �жϵ�ǰλ���Ƿ���C��
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C���У��ж�D���Ƿ����
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D�̴��ڣ��ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
											std::string folderPath = "D:\\��ݲ˵��洢";
											std::string filePath = folderPath + "\\����Ŀ¼�洢.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "�ɹ������ļ���" << folderPath << std::endl;
												} else {
													std::cout << "�����ļ���ʧ��" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "�ɹ������ļ�" << filePath << std::endl;
												} else {
													std::cout << "�����ļ�ʧ��" << std::endl;
													return 0;
												}
											}
											// ��ȡ����Ŀ¼
											char usernameBuf[256];
											GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// ����ļ����Ƿ������ͬ���û���
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
											// �����������ͬ���û�������洢�û������ļ���
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "�ɹ��洢�û���" << std::endl;
												} else {
													std::cout << "�洢�û���ʧ��" << std::endl;
												}
											} else {
												std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
											}
										} else {
											std::cout << "û��D��" << std::endl;
										}
									} else {
										// ����C���У���ȡ��ǰ·�����ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\����Ŀ¼�洢.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "�ɹ������ļ���" << folderPath << std::endl;
											} else {
												std::cout << "�����ļ���ʧ��" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "�ɹ������ļ�" << filePath << std::endl;
											} else {
												std::cout << "�����ļ�ʧ��" << std::endl;
												return 0;
											}
										}
										// ��ȡ����Ŀ¼
										char usernameBuf[256];
										GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// ����ļ����Ƿ������ͬ���û���
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
										// �����������ͬ���û�������洢�û������ļ���
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "�ɹ��洢�û���" << std::endl;
											} else {
												std::cout << "�洢�û���ʧ��" << std::endl;
											}
										} else {
											std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
										}
									}
								}
								if (SendMessage(bujinyongxsmm, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� ӳ��ɹ��󲻽�ֹ��ʾ���� ��ѡ���Ƿ�δ����ѡ
									EnableWindow(showPasswordCheckbox, FALSE); // ���� ��ʾ���� ��ѡ��
									SendMessage(showPasswordCheckbox, BM_SETCHECK, BST_UNCHECKED, 0); // ���� showPasswordCheckbox ��ѡ���ѡ��״̬Ϊδѡ��
									LONG_PTR style = GetWindowLongPtr(passwordEdit, GWL_STYLE); // ��ȡ��������ʽ
									if (!(style & ES_PASSWORD)) { // �ж��������ʽ���Ƿ񲻰���ES_PASSWORD��־
										/* �����������������Ϊ���ģ�������ES_PASSWORD�� */
										// ��ȡ����������������
										char passwordBuf[256];
										GetWindowText(passwordEdit, passwordBuf, sizeof(passwordBuf));
										// ɾ���ɵ��������������
										DestroyWindow(passwordEdit);
										// �����µ������ַ����������
										passwordEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD | ES_AUTOHSCROLL, 335, 87, 150, 26, hwnd, NULL, NULL, NULL);
										// �����ݸ��Ƶ������������
										SetWindowText(passwordEdit, passwordBuf);
									}
								}
								// ����һ��������ı�
								HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("΢���ź�"));
								// ���´���������Ӧ�����ı��ؼ�
								SendMessage(passwordEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
								std::string successMessage = "����������ӳ��ɹ����t(������)�q\n�����룺" + std::to_string(result) + "��";
								MessageBox(hwnd, successMessage.c_str(), "�ɹ�����(>��-*)/", MB_OK | MB_ICONINFORMATION);
							} else {
								std::cout << "����������ӳ��ʧ�ܣ�\n�����룺" << result << "��" << std::endl;
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� �������� ��ѡ���Ƿ�δ����ѡ
									SetWindowText(passwordEdit, ""); // �������������ı�����Ϊ���ַ���
								}
								std::string errorMessage = "����������ӳ��ʧ�ܣ�(�i�s^�t�i)\n�����룺" + std::to_string(result) + "��";
								MessageBox(hwnd, errorMessage.c_str(), "�����ˣ��r(�s�n�t���q", MB_OK | MB_ICONERROR);
							}
							// �ͷſ��ļ�
							FreeLibrary(hMpr);
						} else if (SendMessage(hBtnNetCommandLine, BM_GETCHECK, 0, 0) == BST_CHECKED) {
							string command = "net use ";
							command += drive;
							command += ": \\\\";
							command += ipAddress;
							command += "\\" + sharedFolder + " /user:" + password;
							int result = system(command.c_str());
							if (result == 0) {
								if (countdownMinutes == 0 && countdownDuration == 0) { // �������countdownMinutes���֣���countdownDuration���룩������0ʱ
									if (SendMessage(countdownCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �ж� ӳ��ɹ���������ʱ ��ѡ���Ƿ񱻹�ѡ
										// ��ѡ
										// ���õ���ʱ����
										countdownMinutes = 0;
										countdownDuration = 0;
										// ���ü�ʱ���ļ��ʱ��
										int selectedIndex = SendMessage(countdownUnitComboBox, CB_GETCURSEL, 0, 0);
										// ��������ʱ��ʱ��
										SetTimer(hwnd, 1, 1000, NULL); // ��ʱ��ID: 1�����ʱ��Ϊ1000���루1�룩
										// ��������ʱ
										char countdownBuf[256];
										GetWindowText(countdownEdit, countdownBuf, sizeof(countdownBuf));
										countdownMinutes = atoi(countdownBuf); // ���ַ���ת��Ϊ����
										//���õ���ʱ
										EnableWindow(countdownEdit, FALSE);
										EnableWindow(countdownUnitComboBox, FALSE);
									}
								}
								if (SendMessage(SaveuserIP, BM_GETCHECK, 0, 0) == BST_CHECKED) { // ��ѡ��ѡ��
									// �жϵ�ǰλ���Ƿ���C��
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C���У��ж�D���Ƿ����
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D�̴��ڣ��ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
											std::string folderPath = "D:\\��ݲ˵��洢";
											std::string filePath = folderPath + "\\IP��ַ�洢.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "�ɹ������ļ���" << folderPath << std::endl;
												} else {
													std::cout << "�����ļ���ʧ��" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "�ɹ������ļ�" << filePath << std::endl;
												} else {
													std::cout << "�����ļ�ʧ��" << std::endl;
													return 0;
												}
											}
											// ��ȡIP��ַ
											char usernameBuf[256];
											GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// ����ļ����Ƿ������ͬ���û���
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
											// �����������ͬ���û�������洢�û������ļ���
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "�ɹ��洢�û���" << std::endl;
												} else {
													std::cout << "�洢�û���ʧ��" << std::endl;
												}
											} else {
												std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
											}
										} else {
											std::cout << "û��D��" << std::endl;
										}
									} else {
										// ����C���У���ȡ��ǰ·�����ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\IP��ַ�洢.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "�ɹ������ļ���" << folderPath << std::endl;
											} else {
												std::cout << "�����ļ���ʧ��" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "�ɹ������ļ�" << filePath << std::endl;
											} else {
												std::cout << "�����ļ�ʧ��" << std::endl;
												return 0;
											}
										}
										// ��ȡIP��ַ
										char usernameBuf[256];
										GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// ����ļ����Ƿ������ͬ���û���
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
										// �����������ͬ���û�������洢�û������ļ���
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "�ɹ��洢�û���" << std::endl;
											} else {
												std::cout << "�洢�û���ʧ��" << std::endl;
											}
										} else {
											std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
										}
									}
								}
								if (SendMessage(Saveusershare, BM_GETCHECK, 0, 0) == BST_CHECKED) { // ��ѡ��ѡ��
									// �жϵ�ǰλ���Ƿ���C��
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C���У��ж�D���Ƿ����
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D�̴��ڣ��ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
											std::string folderPath = "D:\\��ݲ˵��洢";
											std::string filePath = folderPath + "\\����Ŀ¼�洢.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "�ɹ������ļ���" << folderPath << std::endl;
												} else {
													std::cout << "�����ļ���ʧ��" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "�ɹ������ļ�" << filePath << std::endl;
												} else {
													std::cout << "�����ļ�ʧ��" << std::endl;
													return 0;
												}
											}
											// ��ȡ����Ŀ¼
											char usernameBuf[256];
											GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// ����ļ����Ƿ������ͬ���û���
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
											// �����������ͬ���û�������洢�û������ļ���
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "�ɹ��洢�û���" << std::endl;
												} else {
													std::cout << "�洢�û���ʧ��" << std::endl;
												}
											} else {
												std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
											}
										} else {
											std::cout << "û��D��" << std::endl;
										}
									} else {
										// ����C���У���ȡ��ǰ·�����ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\����Ŀ¼�洢.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "�ɹ������ļ���" << folderPath << std::endl;
											} else {
												std::cout << "�����ļ���ʧ��" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "�ɹ������ļ�" << filePath << std::endl;
											} else {
												std::cout << "�����ļ�ʧ��" << std::endl;
												return 0;
											}
										}
										// ��ȡ����Ŀ¼
										char usernameBuf[256];
										GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// ����ļ����Ƿ������ͬ���û���
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
										// �����������ͬ���û�������洢�û������ļ���
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "�ɹ��洢�û���" << std::endl;
											} else {
												std::cout << "�洢�û���ʧ��" << std::endl;
											}
										} else {
											std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
										}
									}
								}
								// ���ӳɹ�
								if (SendMessage(bujinyongxsmm, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� ӳ��ɹ��󲻽�ֹ��ʾ���� ��ѡ���Ƿ�δ����ѡ
									EnableWindow(showPasswordCheckbox, FALSE); // ���� ��ʾ���� ��ѡ��
									SendMessage(showPasswordCheckbox, BM_SETCHECK, BST_UNCHECKED, 0); // ���� showPasswordCheckbox ��ѡ���ѡ��״̬Ϊδѡ��
									LONG_PTR style = GetWindowLongPtr(passwordEdit, GWL_STYLE); // ��ȡ��������ʽ
									if (!(style & ES_PASSWORD)) { // �ж��������ʽ���Ƿ񲻰���ES_PASSWORD��־
										/* �����������������Ϊ���ģ�������ES_PASSWORD�� */
										// ��ȡ����������������
										char passwordBuf[256];
										GetWindowText(passwordEdit, passwordBuf, sizeof(passwordBuf));
										// ɾ���ɵ��������������
										DestroyWindow(passwordEdit);
										// �����µ������ַ����������
										passwordEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD | ES_AUTOHSCROLL, 335, 87, 150, 26, hwnd, NULL, NULL, NULL);
										// �����ݸ��Ƶ������������
										SetWindowText(passwordEdit, passwordBuf);
									}
								}
								// ����һ��������ı�
								HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("΢���ź�"));
								// ���´���������Ӧ�����ı��ؼ�
								SendMessage(passwordEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
								if (SendMessage(zidongxuanze, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� ӳ��ɹ����Զ�ѡ����һ������Ŀ¼���̷� ��ѡ���Ƿ�δ����ѡ
									// ����"����Ŀ¼"����һ��ѡ��Ϊѡ��״̬�����Ѿ������һ���򷵻ص�һ��
									int currentSelection = SendMessage(sharedFolderEdit, CB_GETCURSEL, 0, 0);
									int itemCount = SendMessage(sharedFolderEdit, CB_GETCOUNT, 0, 0);
									int nextSelection = (currentSelection + 1) % itemCount; // ��ȡ��һ��ѡ������������Ѿ������һ���򷵻ص�һ��
									SendMessage(sharedFolderEdit, CB_SETCURSEL, nextSelection, 0);
									if (SendMessage(keyongpancheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �ж� ��ʾ��ռ�õ��̷� ��ѡ���Ƿ񱻹�ѡ
										// ����"�̷�"����һ��ѡ��Ϊѡ��״̬�����Ѿ������һ���򷵻ص�һ��
										currentSelection = SendMessage(driveComboBox, CB_GETCURSEL, 0, 0);
										itemCount = SendMessage(driveComboBox, CB_GETCOUNT, 0, 0);
										nextSelection = (currentSelection + 1) % itemCount; // ��ȡ��һ��ѡ������������Ѿ������һ���򷵻ص�һ��
										SendMessage(driveComboBox, CB_SETCURSEL, nextSelection, 0);
									}
								}
								MessageBox(hwnd, "û���κ����⣡qwq", "�ɹ�����qwq", MB_OK | MB_ICONINFORMATION);
							} else {
								// ����ʧ��
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� �������� ��ѡ���Ƿ�δ����ѡ
									SetWindowText(passwordEdit, ""); // �������������ı�����Ϊ���ַ���
								}
								MessageBox(hwnd, "���˵�С���⣡X_X", "�����ˣ�X_X", MB_OK | MB_ICONERROR);
							}
						}
					} else if (password.empty()) {
						// û�����룬�л���û����������ӷ�ʽ
						if (SendMessage(hBtnWNetAddConnection2A, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �жϵ�ѡ���ĸ���ѡ��
							typedef DWORD(WINAPI * LPWNETADDCONNECTION2A)(LPNETRESOURCEA, LPCSTR, LPCSTR, DWORD);
							// ���ؿ��ļ�
							HMODULE hMpr = LoadLibrary("mpr.dll");
							if (hMpr == NULL) {
								std::cout << "�޷�����mpr.dll���ļ�" << std::endl;
								return 1;
							}
							// ��ȡ������ַ
							LPWNETADDCONNECTION2A pWNetAddConnection2A = (LPWNETADDCONNECTION2A)GetProcAddress(hMpr, "WNetAddConnection2A");
							if (pWNetAddConnection2A == NULL) {
								std::cout << "�޷���ȡWNetAddConnection2A������ַ" << std::endl;
								FreeLibrary(hMpr);
								return 1;
							}
							// ����Ҫӳ��������������ı���·����Զ��·��
							std::string localDrive = drive + ":";
							std::string remotePath = "\\\\" + ipAddress + "\\" + sharedFolder;
							const char* localDrivePtr = localDrive.c_str();
							const char* remotePathPtr = remotePath.c_str();
							// ���� NETRESOURCEA �ṹ�壬����ʼ��
							NETRESOURCEA resource = {
								.dwType = RESOURCETYPE_DISK,
								.lpLocalName = const_cast<char*>(localDrivePtr),
								.lpRemoteName = const_cast<char*>(remotePathPtr),
							};
							// ����Ҫӳ����������������û���
							std::string usernameStr = username;
							const char* usernamePtr = usernameStr.c_str();
							// ʹ��WNetAddConnection2A����ӳ������������
							DWORD result = pWNetAddConnection2A(&resource, NULL, usernamePtr, CONNECT_UPDATE_PROFILE);
							// ���ӳ����
							if (result == NO_ERROR) {
								std::cout << "����������ӳ��ɹ���\n�����룺" << result << "��" << std::endl;
								if (countdownMinutes == 0 && countdownDuration == 0) { // �������countdownMinutes���֣���countdownDuration���룩������0ʱ
									if (SendMessage(countdownCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �ж� ӳ��ɹ���������ʱ ��ѡ���Ƿ񱻹�ѡ
										// ��ѡ
										// ���õ���ʱ����
										countdownMinutes = 0;
										countdownDuration = 0;
										// ���ü�ʱ���ļ��ʱ��
										int selectedIndex = SendMessage(countdownUnitComboBox, CB_GETCURSEL, 0, 0);
										// ��������ʱ��ʱ��
										SetTimer(hwnd, 1, 1000, NULL); // ��ʱ��ID: 1�����ʱ��Ϊ1000���루1�룩
										// ��������ʱ
										char countdownBuf[256];
										GetWindowText(countdownEdit, countdownBuf, sizeof(countdownBuf));
										countdownMinutes = atoi(countdownBuf); // ���ַ���ת��Ϊ����
										//���õ���ʱ
										EnableWindow(countdownEdit, FALSE);
										EnableWindow(countdownUnitComboBox, FALSE);
									}
								}
								if (SendMessage(Saveusername, BM_GETCHECK, 0, 0) == BST_CHECKED) { // ��ѡ��ѡ��
									// �жϵ�ǰλ���Ƿ���C��
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C���У��ж�D���Ƿ����
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D�̴��ڣ��ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
											std::string folderPath = "D:\\��ݲ˵��洢";
											std::string filePath = folderPath + "\\�û����洢.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "�ɹ������ļ���" << folderPath << std::endl;
												} else {
													std::cout << "�����ļ���ʧ��" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "�ɹ������ļ�" << filePath << std::endl;
												} else {
													std::cout << "�����ļ�ʧ��" << std::endl;
													return 0;
												}
											}
											// ��ȡ�û���
											char usernameBuf[256];
											GetWindowText(usernameEdit, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// ����ļ����Ƿ������ͬ���û���
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
											// �����������ͬ���û�������洢�û������ļ���
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "�ɹ��洢�û���" << std::endl;
												} else {
													std::cout << "�洢�û���ʧ��" << std::endl;
												}
											} else {
												std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
											}
										} else {
											std::cout << "û��D��" << std::endl;
										}
									} else {
										// ����C���У���ȡ��ǰ·�����ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\�û����洢.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "�ɹ������ļ���" << folderPath << std::endl;
											} else {
												std::cout << "�����ļ���ʧ��" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "�ɹ������ļ�" << filePath << std::endl;
											} else {
												std::cout << "�����ļ�ʧ��" << std::endl;
												return 0;
											}
										}
										// ��ȡ�û���
										char usernameBuf[256];
										GetWindowText(usernameEdit, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// ����ļ����Ƿ������ͬ���û���
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
										// �����������ͬ���û�������洢�û������ļ���
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "�ɹ��洢�û���" << std::endl;
											} else {
												std::cout << "�洢�û���ʧ��" << std::endl;
											}
										} else {
											std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
										}
									}
								}
								if (SendMessage(SaveuserIP, BM_GETCHECK, 0, 0) == BST_CHECKED) { // ��ѡ��ѡ��
									// �жϵ�ǰλ���Ƿ���C��
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C���У��ж�D���Ƿ����
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D�̴��ڣ��ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
											std::string folderPath = "D:\\��ݲ˵��洢";
											std::string filePath = folderPath + "\\IP��ַ�洢.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "�ɹ������ļ���" << folderPath << std::endl;
												} else {
													std::cout << "�����ļ���ʧ��" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "�ɹ������ļ�" << filePath << std::endl;
												} else {
													std::cout << "�����ļ�ʧ��" << std::endl;
													return 0;
												}
											}
											// ��ȡIP��ַ
											char usernameBuf[256];
											GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// ����ļ����Ƿ������ͬ���û���
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
											// �����������ͬ���û�������洢�û������ļ���
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "�ɹ��洢�û���" << std::endl;
												} else {
													std::cout << "�洢�û���ʧ��" << std::endl;
												}
											} else {
												std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
											}
										} else {
											std::cout << "û��D��" << std::endl;
										}
									} else {
										// ����C���У���ȡ��ǰ·�����ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\IP��ַ�洢.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "�ɹ������ļ���" << folderPath << std::endl;
											} else {
												std::cout << "�����ļ���ʧ��" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "�ɹ������ļ�" << filePath << std::endl;
											} else {
												std::cout << "�����ļ�ʧ��" << std::endl;
												return 0;
											}
										}
										// ��ȡIP��ַ
										char usernameBuf[256];
										GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// ����ļ����Ƿ������ͬ���û���
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
										// �����������ͬ���û�������洢�û������ļ���
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "�ɹ��洢�û���" << std::endl;
											} else {
												std::cout << "�洢�û���ʧ��" << std::endl;
											}
										} else {
											std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
										}
									}
								}
								if (SendMessage(Saveusershare, BM_GETCHECK, 0, 0) == BST_CHECKED) { // ��ѡ��ѡ��
									// �жϵ�ǰλ���Ƿ���C��
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C���У��ж�D���Ƿ����
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D�̴��ڣ��ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
											std::string folderPath = "D:\\��ݲ˵��洢";
											std::string filePath = folderPath + "\\����Ŀ¼�洢.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "�ɹ������ļ���" << folderPath << std::endl;
												} else {
													std::cout << "�����ļ���ʧ��" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "�ɹ������ļ�" << filePath << std::endl;
												} else {
													std::cout << "�����ļ�ʧ��" << std::endl;
													return 0;
												}
											}
											// ��ȡ����Ŀ¼
											char usernameBuf[256];
											GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// ����ļ����Ƿ������ͬ���û���
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
											// �����������ͬ���û�������洢�û������ļ���
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "�ɹ��洢�û���" << std::endl;
												} else {
													std::cout << "�洢�û���ʧ��" << std::endl;
												}
											} else {
												std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
											}
										} else {
											std::cout << "û��D��" << std::endl;
										}
									} else {
										// ����C���У���ȡ��ǰ·�����ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\����Ŀ¼�洢.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "�ɹ������ļ���" << folderPath << std::endl;
											} else {
												std::cout << "�����ļ���ʧ��" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "�ɹ������ļ�" << filePath << std::endl;
											} else {
												std::cout << "�����ļ�ʧ��" << std::endl;
												return 0;
											}
										}
										// ��ȡ����Ŀ¼
										char usernameBuf[256];
										GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// ����ļ����Ƿ������ͬ���û���
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
										// �����������ͬ���û�������洢�û������ļ���
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "�ɹ��洢�û���" << std::endl;
											} else {
												std::cout << "�洢�û���ʧ��" << std::endl;
											}
										} else {
											std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
										}
									}
								}
								if (SendMessage(bujinyongxsmm, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� ӳ��ɹ��󲻽�ֹ��ʾ���� ��ѡ���Ƿ�δ����ѡ
									EnableWindow(showPasswordCheckbox, FALSE); // ���� ��ʾ���� ��ѡ��
									SendMessage(showPasswordCheckbox, BM_SETCHECK, BST_UNCHECKED, 0); // ���� showPasswordCheckbox ��ѡ���ѡ��״̬Ϊδѡ��
									LONG_PTR style = GetWindowLongPtr(passwordEdit, GWL_STYLE); // ��ȡ��������ʽ
									if (!(style & ES_PASSWORD)) { // �ж��������ʽ���Ƿ񲻰���ES_PASSWORD��־
										/* �����������������Ϊ���ģ�������ES_PASSWORD�� */
										// ��ȡ����������������
										char passwordBuf[256];
										GetWindowText(passwordEdit, passwordBuf, sizeof(passwordBuf));
										// ɾ���ɵ��������������
										DestroyWindow(passwordEdit);
										// �����µ������ַ����������
										passwordEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD | ES_AUTOHSCROLL, 335, 87, 150, 26, hwnd, NULL, NULL, NULL);
										// �����ݸ��Ƶ������������
										SetWindowText(passwordEdit, passwordBuf);
									}
								}
								std::string successMessage = "����������ӳ��ɹ����t(������)�q\n�����룺" + std::to_string(result) + "��";
								MessageBox(hwnd, successMessage.c_str(), "�ɹ�����(>��-*)/", MB_OK | MB_ICONINFORMATION);
							} else {
								std::cout << "����������ӳ��ʧ�ܣ�\n�����룺" << result << "��" << std::endl;
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� �������� ��ѡ���Ƿ�δ����ѡ
									SetWindowText(passwordEdit, ""); // �������������ı�����Ϊ���ַ���
								}
								std::string errorMessage = "����������ӳ��ʧ�ܣ�(�i�s^�t�i)\n�����룺" + std::to_string(result) + "��";
								MessageBox(hwnd, errorMessage.c_str(), "�����ˣ��r(�s�n�t���q", MB_OK | MB_ICONERROR);
							}
							// �ͷſ��ļ�
							FreeLibrary(hMpr);
						} else if (SendMessage(hBtnNetCommandLine, BM_GETCHECK, 0, 0) == BST_CHECKED) {
							string command = "net use ";
							command += drive;
							command += ": \\\\";
							command += ipAddress;
							command += "\\" + sharedFolder + " /user:" + username;
							int result = system(command.c_str());
							if (result == 0) {
								if (countdownMinutes == 0 && countdownDuration == 0) { // �������countdownMinutes���֣���countdownDuration���룩������0ʱ
									if (SendMessage(countdownCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �ж� ӳ��ɹ���������ʱ ��ѡ���Ƿ񱻹�ѡ
										// ��ѡ
										// ���õ���ʱ����
										countdownMinutes = 0;
										countdownDuration = 0;
										// ���ü�ʱ���ļ��ʱ��
										int selectedIndex = SendMessage(countdownUnitComboBox, CB_GETCURSEL, 0, 0);
										// ��������ʱ��ʱ��
										SetTimer(hwnd, 1, 1000, NULL); // ��ʱ��ID: 1�����ʱ��Ϊ1000���루1�룩
										// ��������ʱ
										char countdownBuf[256];
										GetWindowText(countdownEdit, countdownBuf, sizeof(countdownBuf));
										countdownMinutes = atoi(countdownBuf); // ���ַ���ת��Ϊ����
										//���õ���ʱ
										EnableWindow(countdownEdit, FALSE);
										EnableWindow(countdownUnitComboBox, FALSE);
									}
								}
								if (SendMessage(Saveusername, BM_GETCHECK, 0, 0) == BST_CHECKED) { // ��ѡ��ѡ��
									// �жϵ�ǰλ���Ƿ���C��
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C���У��ж�D���Ƿ����
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D�̴��ڣ��ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
											std::string folderPath = "D:\\��ݲ˵��洢";
											std::string filePath = folderPath + "\\�û����洢.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "�ɹ������ļ���" << folderPath << std::endl;
												} else {
													std::cout << "�����ļ���ʧ��" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "�ɹ������ļ�" << filePath << std::endl;
												} else {
													std::cout << "�����ļ�ʧ��" << std::endl;
													return 0;
												}
											}
											// ��ȡ�û���
											char usernameBuf[256];
											GetWindowText(usernameEdit, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// ����ļ����Ƿ������ͬ���û���
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
											// �����������ͬ���û�������洢�û������ļ���
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "�ɹ��洢�û���" << std::endl;
												} else {
													std::cout << "�洢�û���ʧ��" << std::endl;
												}
											} else {
												std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
											}
										} else {
											std::cout << "û��D��" << std::endl;
										}
									} else {
										// ����C���У���ȡ��ǰ·�����ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\�û����洢.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "�ɹ������ļ���" << folderPath << std::endl;
											} else {
												std::cout << "�����ļ���ʧ��" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "�ɹ������ļ�" << filePath << std::endl;
											} else {
												std::cout << "�����ļ�ʧ��" << std::endl;
												return 0;
											}
										}
										// ��ȡ�û���
										char usernameBuf[256];
										GetWindowText(usernameEdit, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// ����ļ����Ƿ������ͬ���û���
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
										// �����������ͬ���û�������洢�û������ļ���
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "�ɹ��洢�û���" << std::endl;
											} else {
												std::cout << "�洢�û���ʧ��" << std::endl;
											}
										} else {
											std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
										}
									}
								}
								if (SendMessage(SaveuserIP, BM_GETCHECK, 0, 0) == BST_CHECKED) { // ��ѡ��ѡ��
									// �жϵ�ǰλ���Ƿ���C��
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C���У��ж�D���Ƿ����
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D�̴��ڣ��ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
											std::string folderPath = "D:\\��ݲ˵��洢";
											std::string filePath = folderPath + "\\IP��ַ�洢.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "�ɹ������ļ���" << folderPath << std::endl;
												} else {
													std::cout << "�����ļ���ʧ��" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "�ɹ������ļ�" << filePath << std::endl;
												} else {
													std::cout << "�����ļ�ʧ��" << std::endl;
													return 0;
												}
											}
											// ��ȡIP��ַ
											char usernameBuf[256];
											GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// ����ļ����Ƿ������ͬ���û���
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
											// �����������ͬ���û�������洢�û������ļ���
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "�ɹ��洢�û���" << std::endl;
												} else {
													std::cout << "�洢�û���ʧ��" << std::endl;
												}
											} else {
												std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
											}
										} else {
											std::cout << "û��D��" << std::endl;
										}
									} else {
										// ����C���У���ȡ��ǰ·�����ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\IP��ַ�洢.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "�ɹ������ļ���" << folderPath << std::endl;
											} else {
												std::cout << "�����ļ���ʧ��" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "�ɹ������ļ�" << filePath << std::endl;
											} else {
												std::cout << "�����ļ�ʧ��" << std::endl;
												return 0;
											}
										}
										// ��ȡIP��ַ
										char usernameBuf[256];
										GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// ����ļ����Ƿ������ͬ���û���
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
										// �����������ͬ���û�������洢�û������ļ���
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "�ɹ��洢�û���" << std::endl;
											} else {
												std::cout << "�洢�û���ʧ��" << std::endl;
											}
										} else {
											std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
										}
									}
								}
								if (SendMessage(Saveusershare, BM_GETCHECK, 0, 0) == BST_CHECKED) { // ��ѡ��ѡ��
									// �жϵ�ǰλ���Ƿ���C��
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C���У��ж�D���Ƿ����
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D�̴��ڣ��ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
											std::string folderPath = "D:\\��ݲ˵��洢";
											std::string filePath = folderPath + "\\����Ŀ¼�洢.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "�ɹ������ļ���" << folderPath << std::endl;
												} else {
													std::cout << "�����ļ���ʧ��" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "�ɹ������ļ�" << filePath << std::endl;
												} else {
													std::cout << "�����ļ�ʧ��" << std::endl;
													return 0;
												}
											}
											// ��ȡ����Ŀ¼
											char usernameBuf[256];
											GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// ����ļ����Ƿ������ͬ���û���
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
											// �����������ͬ���û�������洢�û������ļ���
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "�ɹ��洢�û���" << std::endl;
												} else {
													std::cout << "�洢�û���ʧ��" << std::endl;
												}
											} else {
												std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
											}
										} else {
											std::cout << "û��D��" << std::endl;
										}
									} else {
										// ����C���У���ȡ��ǰ·�����ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\����Ŀ¼�洢.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "�ɹ������ļ���" << folderPath << std::endl;
											} else {
												std::cout << "�����ļ���ʧ��" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "�ɹ������ļ�" << filePath << std::endl;
											} else {
												std::cout << "�����ļ�ʧ��" << std::endl;
												return 0;
											}
										}
										// ��ȡ����Ŀ¼
										char usernameBuf[256];
										GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// ����ļ����Ƿ������ͬ���û���
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
										// �����������ͬ���û�������洢�û������ļ���
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "�ɹ��洢�û���" << std::endl;
											} else {
												std::cout << "�洢�û���ʧ��" << std::endl;
											}
										} else {
											std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
										}
									}
								}
								// ���ӳɹ�
								if (SendMessage(zidongxuanze, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� ӳ��ɹ����Զ�ѡ����һ������Ŀ¼���̷� ��ѡ���Ƿ�δ����ѡ
									// ����"����Ŀ¼"����һ��ѡ��Ϊѡ��״̬�����Ѿ������һ���򷵻ص�һ��
									int currentSelection = SendMessage(sharedFolderEdit, CB_GETCURSEL, 0, 0);
									int itemCount = SendMessage(sharedFolderEdit, CB_GETCOUNT, 0, 0);
									int nextSelection = (currentSelection + 1) % itemCount; // ��ȡ��һ��ѡ������������Ѿ������һ���򷵻ص�һ��
									SendMessage(sharedFolderEdit, CB_SETCURSEL, nextSelection, 0);
									if (SendMessage(keyongpancheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �ж� ��ʾ��ռ�õ��̷� ��ѡ���Ƿ񱻹�ѡ
										// ����"�̷�"����һ��ѡ��Ϊѡ��״̬�����Ѿ������һ���򷵻ص�һ��
										currentSelection = SendMessage(driveComboBox, CB_GETCURSEL, 0, 0);
										itemCount = SendMessage(driveComboBox, CB_GETCOUNT, 0, 0);
										nextSelection = (currentSelection + 1) % itemCount; // ��ȡ��һ��ѡ������������Ѿ������һ���򷵻ص�һ��
										SendMessage(driveComboBox, CB_SETCURSEL, nextSelection, 0);
									}
								}
								MessageBox(hwnd, "û���κ����⣡qwq", "�ɹ�����qwq", MB_OK | MB_ICONINFORMATION);
							} else {
								// ����ʧ��
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� �������� ��ѡ���Ƿ�δ����ѡ
									SetWindowText(passwordEdit, ""); // �������������ı�����Ϊ���ַ���
								}
								MessageBox(hwnd, "���˵�С���⣡X_X", "�����ˣ�X_X", MB_OK | MB_ICONERROR);
							}
						}
					} else {
						// ���û��������룬�л������û�������������ӷ�ʽ
						if (SendMessage(hBtnWNetAddConnection2A, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �жϵ�ѡ���ĸ���ѡ��
							typedef DWORD(WINAPI * LPWNETADDCONNECTION2A)(LPNETRESOURCEA, LPCSTR, LPCSTR, DWORD);
							// ���ؿ��ļ�
							HMODULE hMpr = LoadLibrary("mpr.dll");
							if (hMpr == NULL) {
								std::cout << "�޷�����mpr.dll���ļ�" << std::endl;
								return 1;
							}
							// ��ȡ������ַ
							LPWNETADDCONNECTION2A pWNetAddConnection2A = (LPWNETADDCONNECTION2A)GetProcAddress(hMpr, "WNetAddConnection2A");
							if (pWNetAddConnection2A == NULL) {
								std::cout << "�޷���ȡWNetAddConnection2A������ַ" << std::endl;
								FreeLibrary(hMpr);
								return 1;
							}
							// ����Ҫӳ��������������ı���·����Զ��·��
							std::string localDrive = drive + ":";
							std::string remotePath = "\\\\" + ipAddress + "\\" + sharedFolder;
							const char* localDrivePtr = localDrive.c_str();
							const char* remotePathPtr = remotePath.c_str();
							// ���� NETRESOURCEA �ṹ�壬����ʼ��
							NETRESOURCEA resource = {
								.dwType = RESOURCETYPE_DISK,
								.lpLocalName = const_cast<char*>(localDrivePtr),
								.lpRemoteName = const_cast<char*>(remotePathPtr),
							};
							// ����Ҫӳ����������������û���������
							std::string usernameStr = username;
							std::string passwordStr = password;
							const char* usernamePtr = usernameStr.c_str();
							const char* passwordPtr = passwordStr.c_str();
							// ʹ��WNetAddConnection2A����ӳ������������
							DWORD result = pWNetAddConnection2A(&resource, passwordPtr, usernamePtr, CONNECT_UPDATE_PROFILE);
							// ���ӳ����
							if (result == NO_ERROR) {
								std::cout << "����������ӳ��ɹ���\n�����룺" << result << "��" << std::endl;
								if (countdownMinutes == 0 && countdownDuration == 0) { // �������countdownMinutes���֣���countdownDuration���룩������0ʱ
									if (SendMessage(countdownCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �ж� ӳ��ɹ���������ʱ ��ѡ���Ƿ񱻹�ѡ
										// ��ѡ
										// ���õ���ʱ����
										countdownMinutes = 0;
										countdownDuration = 0;
										// ���ü�ʱ���ļ��ʱ��
										int selectedIndex = SendMessage(countdownUnitComboBox, CB_GETCURSEL, 0, 0);
										// ��������ʱ��ʱ��
										SetTimer(hwnd, 1, 1000, NULL); // ��ʱ��ID: 1�����ʱ��Ϊ1000���루1�룩
										// ��������ʱ
										char countdownBuf[256];
										GetWindowText(countdownEdit, countdownBuf, sizeof(countdownBuf));
										countdownMinutes = atoi(countdownBuf); // ���ַ���ת��Ϊ����
										//���õ���ʱ
										EnableWindow(countdownEdit, FALSE);
										EnableWindow(countdownUnitComboBox, FALSE);
									}
								}
								if (SendMessage(Saveusername, BM_GETCHECK, 0, 0) == BST_CHECKED) { // ��ѡ��ѡ��
									// �жϵ�ǰλ���Ƿ���C��
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C���У��ж�D���Ƿ����
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D�̴��ڣ��ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
											std::string folderPath = "D:\\��ݲ˵��洢";
											std::string filePath = folderPath + "\\�û����洢.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "�ɹ������ļ���" << folderPath << std::endl;
												} else {
													std::cout << "�����ļ���ʧ��" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "�ɹ������ļ�" << filePath << std::endl;
												} else {
													std::cout << "�����ļ�ʧ��" << std::endl;
													return 0;
												}
											}
											// ��ȡ�û���
											char usernameBuf[256];
											GetWindowText(usernameEdit, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// ����ļ����Ƿ������ͬ���û���
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
											// �����������ͬ���û�������洢�û������ļ���
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "�ɹ��洢�û���" << std::endl;
												} else {
													std::cout << "�洢�û���ʧ��" << std::endl;
												}
											} else {
												std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
											}
										} else {
											std::cout << "û��D��" << std::endl;
										}
									} else {
										// ����C���У���ȡ��ǰ·�����ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\�û����洢.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "�ɹ������ļ���" << folderPath << std::endl;
											} else {
												std::cout << "�����ļ���ʧ��" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "�ɹ������ļ�" << filePath << std::endl;
											} else {
												std::cout << "�����ļ�ʧ��" << std::endl;
												return 0;
											}
										}
										// ��ȡ�û���
										char usernameBuf[256];
										GetWindowText(usernameEdit, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// ����ļ����Ƿ������ͬ���û���
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
										// �����������ͬ���û�������洢�û������ļ���
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "�ɹ��洢�û���" << std::endl;
											} else {
												std::cout << "�洢�û���ʧ��" << std::endl;
											}
										} else {
											std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
										}
									}
								}
								if (SendMessage(SaveuserIP, BM_GETCHECK, 0, 0) == BST_CHECKED) { // ��ѡ��ѡ��
									// �жϵ�ǰλ���Ƿ���C��
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C���У��ж�D���Ƿ����
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D�̴��ڣ��ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
											std::string folderPath = "D:\\��ݲ˵��洢";
											std::string filePath = folderPath + "\\IP��ַ�洢.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "�ɹ������ļ���" << folderPath << std::endl;
												} else {
													std::cout << "�����ļ���ʧ��" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "�ɹ������ļ�" << filePath << std::endl;
												} else {
													std::cout << "�����ļ�ʧ��" << std::endl;
													return 0;
												}
											}
											// ��ȡIP��ַ
											char usernameBuf[256];
											GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// ����ļ����Ƿ������ͬ���û���
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
											// �����������ͬ���û�������洢�û������ļ���
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "�ɹ��洢�û���" << std::endl;
												} else {
													std::cout << "�洢�û���ʧ��" << std::endl;
												}
											} else {
												std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
											}
										} else {
											std::cout << "û��D��" << std::endl;
										}
									} else {
										// ����C���У���ȡ��ǰ·�����ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\IP��ַ�洢.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "�ɹ������ļ���" << folderPath << std::endl;
											} else {
												std::cout << "�����ļ���ʧ��" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "�ɹ������ļ�" << filePath << std::endl;
											} else {
												std::cout << "�����ļ�ʧ��" << std::endl;
												return 0;
											}
										}
										// ��ȡIP��ַ
										char usernameBuf[256];
										GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// ����ļ����Ƿ������ͬ���û���
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
										// �����������ͬ���û�������洢�û������ļ���
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "�ɹ��洢�û���" << std::endl;
											} else {
												std::cout << "�洢�û���ʧ��" << std::endl;
											}
										} else {
											std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
										}
									}
								}
								if (SendMessage(Saveusershare, BM_GETCHECK, 0, 0) == BST_CHECKED) { // ��ѡ��ѡ��
									// �жϵ�ǰλ���Ƿ���C��
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C���У��ж�D���Ƿ����
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D�̴��ڣ��ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
											std::string folderPath = "D:\\��ݲ˵��洢";
											std::string filePath = folderPath + "\\����Ŀ¼�洢.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "�ɹ������ļ���" << folderPath << std::endl;
												} else {
													std::cout << "�����ļ���ʧ��" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "�ɹ������ļ�" << filePath << std::endl;
												} else {
													std::cout << "�����ļ�ʧ��" << std::endl;
													return 0;
												}
											}
											// ��ȡ����Ŀ¼
											char usernameBuf[256];
											GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// ����ļ����Ƿ������ͬ���û���
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
											// �����������ͬ���û�������洢�û������ļ���
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "�ɹ��洢�û���" << std::endl;
												} else {
													std::cout << "�洢�û���ʧ��" << std::endl;
												}
											} else {
												std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
											}
										} else {
											std::cout << "û��D��" << std::endl;
										}
									} else {
										// ����C���У���ȡ��ǰ·�����ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\����Ŀ¼�洢.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "�ɹ������ļ���" << folderPath << std::endl;
											} else {
												std::cout << "�����ļ���ʧ��" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "�ɹ������ļ�" << filePath << std::endl;
											} else {
												std::cout << "�����ļ�ʧ��" << std::endl;
												return 0;
											}
										}
										// ��ȡ����Ŀ¼
										char usernameBuf[256];
										GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// ����ļ����Ƿ������ͬ���û���
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
										// �����������ͬ���û�������洢�û������ļ���
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "�ɹ��洢�û���" << std::endl;
											} else {
												std::cout << "�洢�û���ʧ��" << std::endl;
											}
										} else {
											std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
										}
									}
								}
								if (SendMessage(bujinyongxsmm, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� ӳ��ɹ��󲻽�ֹ��ʾ���� ��ѡ���Ƿ�δ����ѡ
									EnableWindow(showPasswordCheckbox, FALSE); // ���� ��ʾ���� ��ѡ��
									SendMessage(showPasswordCheckbox, BM_SETCHECK, BST_UNCHECKED, 0); // ���� showPasswordCheckbox ��ѡ���ѡ��״̬Ϊδѡ��
									LONG_PTR style = GetWindowLongPtr(passwordEdit, GWL_STYLE); // ��ȡ��������ʽ
									if (!(style & ES_PASSWORD)) { // �ж��������ʽ���Ƿ񲻰���ES_PASSWORD��־
										/* �����������������Ϊ���ģ�������ES_PASSWORD�� */
										// ��ȡ����������������
										char passwordBuf[256];
										GetWindowText(passwordEdit, passwordBuf, sizeof(passwordBuf));
										// ɾ���ɵ��������������
										DestroyWindow(passwordEdit);
										// �����µ������ַ����������
										passwordEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD | ES_AUTOHSCROLL, 335, 87, 150, 26, hwnd, NULL, NULL, NULL);
										// �����ݸ��Ƶ������������
										SetWindowText(passwordEdit, passwordBuf);
									}
								}
								// ����һ��������ı�
								HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("΢���ź�"));
								// ���´���������Ӧ�����ı��ؼ�
								SendMessage(passwordEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
								std::string successMessage = "����������ӳ��ɹ����t(������)�q\n�����룺" + std::to_string(result) + "��";
								MessageBox(hwnd, successMessage.c_str(), "�ɹ�����(>��-*)/", MB_OK | MB_ICONINFORMATION);
							} else {
								std::cout << "����������ӳ��ʧ�ܣ�\n�����룺" << result << "��" << std::endl;
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� �������� ��ѡ���Ƿ�δ����ѡ
									SetWindowText(passwordEdit, ""); // �������������ı�����Ϊ���ַ���
								}
								std::string errorMessage = "����������ӳ��ʧ�ܣ�(�i�s^�t�i)\n�����룺" + std::to_string(result) + "��";
								MessageBox(hwnd, errorMessage.c_str(), "�����ˣ��r(�s�n�t���q", MB_OK | MB_ICONERROR);
							}
							// �ͷſ��ļ�
							FreeLibrary(hMpr);
						} else if (SendMessage(hBtnNetCommandLine, BM_GETCHECK, 0, 0) == BST_CHECKED) {
							string command = "net use ";
							command += drive;
							command += ": \\\\";
							command += ipAddress;
							command += "\\" + sharedFolder + " /user:" + username + " " + password;
							int result = system(command.c_str());
							if (result == 0) {
								if (countdownMinutes == 0 && countdownDuration == 0) { // �������countdownMinutes���֣���countdownDuration���룩������0ʱ
									if (SendMessage(countdownCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �ж� ӳ��ɹ���������ʱ ��ѡ���Ƿ񱻹�ѡ
										// ��ѡ
										// ���õ���ʱ����
										countdownMinutes = 0;
										countdownDuration = 0;
										// ���ü�ʱ���ļ��ʱ��
										int selectedIndex = SendMessage(countdownUnitComboBox, CB_GETCURSEL, 0, 0);
										// ��������ʱ��ʱ��
										SetTimer(hwnd, 1, 1000, NULL); // ��ʱ��ID: 1�����ʱ��Ϊ1000���루1�룩
										// ��������ʱ
										char countdownBuf[256];
										GetWindowText(countdownEdit, countdownBuf, sizeof(countdownBuf));
										countdownMinutes = atoi(countdownBuf); // ���ַ���ת��Ϊ����
										//���õ���ʱ
										EnableWindow(countdownEdit, FALSE);
										EnableWindow(countdownUnitComboBox, FALSE);
									}
								}
								if (SendMessage(Saveusername, BM_GETCHECK, 0, 0) == BST_CHECKED) { // ��ѡ��ѡ��
									// �жϵ�ǰλ���Ƿ���C��
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C���У��ж�D���Ƿ����
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D�̴��ڣ��ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
											std::string folderPath = "D:\\��ݲ˵��洢";
											std::string filePath = folderPath + "\\�û����洢.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "�ɹ������ļ���" << folderPath << std::endl;
												} else {
													std::cout << "�����ļ���ʧ��" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "�ɹ������ļ�" << filePath << std::endl;
												} else {
													std::cout << "�����ļ�ʧ��" << std::endl;
													return 0;
												}
											}
											// ��ȡ�û���
											char usernameBuf[256];
											GetWindowText(usernameEdit, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// ����ļ����Ƿ������ͬ���û���
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
											// �����������ͬ���û�������洢�û������ļ���
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "�ɹ��洢�û���" << std::endl;
												} else {
													std::cout << "�洢�û���ʧ��" << std::endl;
												}
											} else {
												std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
											}
										} else {
											std::cout << "û��D��" << std::endl;
										}
									} else {
										// ����C���У���ȡ��ǰ·�����ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\�û����洢.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "�ɹ������ļ���" << folderPath << std::endl;
											} else {
												std::cout << "�����ļ���ʧ��" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "�ɹ������ļ�" << filePath << std::endl;
											} else {
												std::cout << "�����ļ�ʧ��" << std::endl;
												return 0;
											}
										}
										// ��ȡ�û���
										char usernameBuf[256];
										GetWindowText(usernameEdit, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// ����ļ����Ƿ������ͬ���û���
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
										// �����������ͬ���û�������洢�û������ļ���
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "�ɹ��洢�û���" << std::endl;
											} else {
												std::cout << "�洢�û���ʧ��" << std::endl;
											}
										} else {
											std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
										}
									}
								}
								if (SendMessage(SaveuserIP, BM_GETCHECK, 0, 0) == BST_CHECKED) { // ��ѡ��ѡ��
									// �жϵ�ǰλ���Ƿ���C��
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C���У��ж�D���Ƿ����
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D�̴��ڣ��ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
											std::string folderPath = "D:\\��ݲ˵��洢";
											std::string filePath = folderPath + "\\IP��ַ�洢.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "�ɹ������ļ���" << folderPath << std::endl;
												} else {
													std::cout << "�����ļ���ʧ��" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "�ɹ������ļ�" << filePath << std::endl;
												} else {
													std::cout << "�����ļ�ʧ��" << std::endl;
													return 0;
												}
											}
											// ��ȡIP��ַ
											char usernameBuf[256];
											GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// ����ļ����Ƿ������ͬ���û���
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
											// �����������ͬ���û�������洢�û������ļ���
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "�ɹ��洢�û���" << std::endl;
												} else {
													std::cout << "�洢�û���ʧ��" << std::endl;
												}
											} else {
												std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
											}
										} else {
											std::cout << "û��D��" << std::endl;
										}
									} else {
										// ����C���У���ȡ��ǰ·�����ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\IP��ַ�洢.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "�ɹ������ļ���" << folderPath << std::endl;
											} else {
												std::cout << "�����ļ���ʧ��" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "�ɹ������ļ�" << filePath << std::endl;
											} else {
												std::cout << "�����ļ�ʧ��" << std::endl;
												return 0;
											}
										}
										// ��ȡIP��ַ
										char usernameBuf[256];
										GetWindowText(ipComboBox, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// ����ļ����Ƿ������ͬ���û���
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
										// �����������ͬ���û�������洢�û������ļ���
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "�ɹ��洢�û���" << std::endl;
											} else {
												std::cout << "�洢�û���ʧ��" << std::endl;
											}
										} else {
											std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
										}
									}
								}
								if (SendMessage(Saveusershare, BM_GETCHECK, 0, 0) == BST_CHECKED) { // ��ѡ��ѡ��
									// �жϵ�ǰλ���Ƿ���C��
									char currentPath[MAX_PATH];
									GetCurrentDirectoryA(MAX_PATH, currentPath);
									std::string currentDrive = std::string(currentPath).substr(0, 2);
									if (currentDrive == "C:") {
										// C���У��ж�D���Ƿ����
										UINT driveType = GetDriveTypeA("D:\\");
										if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
											// D�̴��ڣ��ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
											std::string folderPath = "D:\\��ݲ˵��洢";
											std::string filePath = folderPath + "\\����Ŀ¼�洢.txt";
											if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												if (CreateDirectoryA(folderPath.c_str(), NULL)) {
													std::cout << "�ɹ������ļ���" << folderPath << std::endl;
												} else {
													std::cout << "�����ļ���ʧ��" << std::endl;
													return 0;
												}
											}
											if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
												std::ofstream file(filePath);
												if (file.good()) {
													std::cout << "�ɹ������ļ�" << filePath << std::endl;
												} else {
													std::cout << "�����ļ�ʧ��" << std::endl;
													return 0;
												}
											}
											// ��ȡ����Ŀ¼
											char usernameBuf[256];
											GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
											std::string username = usernameBuf;
											// ����ļ����Ƿ������ͬ���û���
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
											// �����������ͬ���û�������洢�û������ļ���
											if (!usernameExists) {
												std::ofstream outputFile(filePath, std::ios::app);
												if (outputFile.is_open()) {
													outputFile << username << std::endl;
													outputFile.close();
													std::cout << "�ɹ��洢�û���" << std::endl;
												} else {
													std::cout << "�洢�û���ʧ��" << std::endl;
												}
											} else {
												std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
											}
										} else {
											std::cout << "û��D��" << std::endl;
										}
									} else {
										// ����C���У���ȡ��ǰ·�����ж��ļ��к��ļ��Ƿ���ڣ�����������򴴽�
										char currentPath[MAX_PATH];
										GetCurrentDirectoryA(MAX_PATH, currentPath);
										std::string folderPath = std::string(currentPath);
										std::string filePath = folderPath + "\\����Ŀ¼�洢.txt";
										if (GetFileAttributesA(folderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											if (CreateDirectoryA(folderPath.c_str(), NULL)) {
												std::cout << "�ɹ������ļ���" << folderPath << std::endl;
											} else {
												std::cout << "�����ļ���ʧ��" << std::endl;
												return 0;
											}
										}
										if (GetFileAttributesA(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
											std::ofstream file(filePath);
											if (file.good()) {
												std::cout << "�ɹ������ļ�" << filePath << std::endl;
											} else {
												std::cout << "�����ļ�ʧ��" << std::endl;
												return 0;
											}
										}
										// ��ȡ����Ŀ¼
										char usernameBuf[256];
										GetWindowText(sharedFolderEdit, usernameBuf, sizeof(usernameBuf));
										std::string username = usernameBuf;
										// ����ļ����Ƿ������ͬ���û���
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
										// �����������ͬ���û�������洢�û������ļ���
										if (!usernameExists) {
											std::ofstream outputFile(filePath, std::ios::app);
											if (outputFile.is_open()) {
												outputFile << username << std::endl;
												outputFile.close();
												std::cout << "�ɹ��洢�û���" << std::endl;
											} else {
												std::cout << "�洢�û���ʧ��" << std::endl;
											}
										} else {
											std::cout << "�ļ����Ѵ�����ͬ���û����������д洢" << std::endl;
										}
									}
								}
								// ���ӳɹ�
								if (SendMessage(bujinyongxsmm, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� ӳ��ɹ��󲻽�ֹ��ʾ���� ��ѡ���Ƿ�δ����ѡ
									EnableWindow(showPasswordCheckbox, FALSE); // ���� ��ʾ���� ��ѡ��
									SendMessage(showPasswordCheckbox, BM_SETCHECK, BST_UNCHECKED, 0); // ���� showPasswordCheckbox ��ѡ���ѡ��״̬Ϊδѡ��
									LONG_PTR style = GetWindowLongPtr(passwordEdit, GWL_STYLE); // ��ȡ��������ʽ
									if (!(style & ES_PASSWORD)) { // �ж��������ʽ���Ƿ񲻰���ES_PASSWORD��־
										/* �����������������Ϊ���ģ�������ES_PASSWORD�� */
										// ��ȡ����������������
										char passwordBuf[256];
										GetWindowText(passwordEdit, passwordBuf, sizeof(passwordBuf));
										// ɾ���ɵ��������������
										DestroyWindow(passwordEdit);
										// �����µ������ַ����������
										passwordEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD | ES_AUTOHSCROLL, 335, 87, 150, 26, hwnd, NULL, NULL, NULL);
										// �����ݸ��Ƶ������������
										SetWindowText(passwordEdit, passwordBuf);
									}
								}
								// ����һ��������ı�
								HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("΢���ź�"));
								// ���´���������Ӧ�����ı��ؼ�
								SendMessage(passwordEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
								if (SendMessage(zidongxuanze, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� ӳ��ɹ����Զ�ѡ����һ������Ŀ¼���̷� ��ѡ���Ƿ�δ����ѡ
									// ����"����Ŀ¼"����һ��ѡ��Ϊѡ��״̬�����Ѿ������һ���򷵻ص�һ��
									int currentSelection = SendMessage(sharedFolderEdit, CB_GETCURSEL, 0, 0);
									int itemCount = SendMessage(sharedFolderEdit, CB_GETCOUNT, 0, 0);
									int nextSelection = (currentSelection + 1) % itemCount; // ��ȡ��һ��ѡ������������Ѿ������һ���򷵻ص�һ��
									SendMessage(sharedFolderEdit, CB_SETCURSEL, nextSelection, 0);
									if (SendMessage(keyongpancheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �ж� ��ʾ��ռ�õ��̷� ��ѡ���Ƿ񱻹�ѡ
										// ����"�̷�"����һ��ѡ��Ϊѡ��״̬�����Ѿ������һ���򷵻ص�һ��
										currentSelection = SendMessage(driveComboBox, CB_GETCURSEL, 0, 0);
										itemCount = SendMessage(driveComboBox, CB_GETCOUNT, 0, 0);
										nextSelection = (currentSelection + 1) % itemCount; // ��ȡ��һ��ѡ������������Ѿ������һ���򷵻ص�һ��
										SendMessage(driveComboBox, CB_SETCURSEL, nextSelection, 0);
									}
								}
								MessageBox(hwnd, "û���κ����⣡qwq", "�ɹ�����qwq", MB_OK | MB_ICONINFORMATION);
							} else {
								// ����ʧ��
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� �������� ��ѡ���Ƿ�δ����ѡ
									SetWindowText(passwordEdit, ""); // �������������ı�����Ϊ���ַ���
								}
								MessageBox(hwnd, "���˵�С���⣡X_X", "�����ˣ�X_X", MB_OK | MB_ICONERROR);
							}
						}
					}
					break;
				}

				case 2: { // �Ͽ�ȫ��
					// �Ͽ�ȫ��Զ�̹����ļ���
					if (SendMessage(jieshudaojishicheckbox, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� �Ͽ��󲻽�������ʱ ��ѡ���Ƿ�δ����ѡ
						// δ��ѡ
						// ���õ���ʱ����
						countdownMinutes = 0;
						countdownDuration = 0;
						// ɾ�ɵ���ʱ
						DestroyWindow(countdownLabel);
						// ���µ���ʱ
						countdownLabel = CreateWindow("STATIC", "��Ͽ����л��У�", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 265, 210, 180, 20, hwnd, NULL, NULL, NULL);
					}
					if (SendMessage(countdownCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �жϸ�ѡ���Ƿ񱻹�ѡ
						// ��ѡ
						if (SendMessage(jieshudaojishicheckbox, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� �Ͽ��󲻽�������ʱ ��ѡ���Ƿ�δ����ѡ
							// δ��ѡ
							//ȡ�����õ���ʱ
							EnableWindow(countdownLabel, TRUE);
							EnableWindow(countdownEdit, TRUE);
							EnableWindow(countdownUnitComboBox, TRUE);
						}
					} else {
						// ȡ����ѡ
						// ���õ���ʱ
						EnableWindow(countdownLabel, FALSE);
						EnableWindow(countdownEdit, FALSE);
						EnableWindow(countdownUnitComboBox, FALSE);
					}
					if (SendMessage(hBtnWNetAddConnection2A, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �жϵ�ѡ���ĸ���ѡ��
						typedef DWORD(WINAPI * LPWNETCANCELCONNECTION2A)(LPCSTR, DWORD, BOOL);
						// ���ؿ��ļ�
						HMODULE hMpr = LoadLibrary("mpr.dll");
						if (hMpr == NULL) {
							std::cout << "�޷�����mpr.dll���ļ�" << std::endl;
							return 1;
						}
						// ��ȡ������ַ
						LPWNETCANCELCONNECTION2A pWNetCancelConnection2A = (LPWNETCANCELCONNECTION2A)GetProcAddress(hMpr, "WNetCancelConnection2A");
						if (pWNetCancelConnection2A == NULL) {
							std::cout << "�޷���ȡWNetCancelConnection2A������ַ" << std::endl;
							FreeLibrary(hMpr);
							return 1;
						}
						DWORD result = NO_ERROR; // ����һ��result����
						// ѭ���Ͽ���������������
						for (char driveLetter = 'Z'; driveLetter >= 'A'; --driveLetter) {
							std::string drive = std::string(1, driveLetter) + ":";
							result = pWNetCancelConnection2A(drive.c_str(), CONNECT_UPDATE_PROFILE, TRUE);
							if (result == NO_ERROR) {
								std::cout << "���������� " << drive << " �Ͽ��ɹ���OwO�����룺" << result << "��" << std::endl;
							} else {
								std::cout << "���������� " << drive << " �Ͽ�ʧ�ܣ�X_X�����룺" << result << "��" << std::endl;
							}
						}
						// �ͷſ��ļ�
						FreeLibrary(hMpr);
						// ��ʾ��Ϣ��
						std::string komplettMessage = "�ѶϿ�����ӳ���������������(*������*) \n�����룺" + std::to_string(result) + "��";
						// ����һ��������ı�
						HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("΢���ź�"));
						// ���´���������Ӧ�����ı��ؼ�
						SendMessage(countdownLabel, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
						MessageBox(hwnd, komplettMessage.c_str(), "�������(>��-*)/", MB_OK | MB_ICONINFORMATION);
					} else if (SendMessage(hBtnNetCommandLine, BM_GETCHECK, 0, 0) == BST_CHECKED) {
						string command = "net use * /del /Y";
						system(command.c_str());
						// ����һ��������ı�
						HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("΢���ź�"));
						// ���´���������Ӧ�����ı��ؼ�
						SendMessage(countdownLabel, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
						MessageBox(hwnd, "����ɲ�����qwq", "�������qwq", MB_OK | MB_ICONINFORMATION);
					}
					break;
				}

				case 3: { // ӳ��ɹ���������ʱ
					if (SendMessage(countdownCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �жϸ�ѡ���Ƿ񱻹�ѡ
						// ��ѡ
						//ȡ�����õ���ʱ
						EnableWindow(countdownLabel, TRUE);
						EnableWindow(countdownEdit, TRUE);
						EnableWindow(countdownUnitComboBox, TRUE);
					} else {
						// ȡ����ѡ
						// ���õ���ʱ����
						countdownMinutes = 0;
						countdownDuration = 0;
						// ɾ�ɵ���ʱ
						DestroyWindow(countdownLabel);
						// ���µ���ʱ
						countdownLabel = CreateWindow("STATIC", "��Ͽ����л��У�", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 265, 210, 180, 20, hwnd, NULL, NULL, NULL);
						// ���õ���ʱ
						EnableWindow(countdownLabel, FALSE);
						EnableWindow(countdownEdit, FALSE);
						EnableWindow(countdownUnitComboBox, FALSE);
					}
					// ����һ��������ı�
					HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("΢���ź�"));
					// ���´���������Ӧ�����ı��ؼ�
					SendMessage(countdownLabel, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
					break;
				}

				case 333 : { // ǿ�ƶϿ�ȫ��(������Դ������)
					// �ر���Դ������
					system("taskkill /f /im explorer.exe");
					// ��ȡ Windows Ŀ¼��·��
					TCHAR windowsDir[MAX_PATH];
					if (GetWindowsDirectory(windowsDir, MAX_PATH) > 0) {
						// ������Դ������������·��
						TCHAR explorerPath[MAX_PATH];
						wsprintf(explorerPath, TEXT("%s\\explorer.exe"), windowsDir);

						// ������Դ������
						ShellExecute(NULL, TEXT("open"), explorerPath, NULL, NULL, SW_SHOWNORMAL);
					}
					// �Ͽ�ȫ��Զ�̹����ļ���
					if (SendMessage(jieshudaojishicheckbox, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� �Ͽ��󲻽�������ʱ ��ѡ���Ƿ�δ����ѡ
						// δ��ѡ
						// ���õ���ʱ����
						countdownMinutes = 0;
						countdownDuration = 0;
						// ɾ�ɵ���ʱ
						DestroyWindow(countdownLabel);
						// ���µ���ʱ
						countdownLabel = CreateWindow("STATIC", "��Ͽ����л��У�", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 265, 210, 180, 20, hwnd, NULL, NULL, NULL);
					}
					if (SendMessage(countdownCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �жϸ�ѡ���Ƿ񱻹�ѡ
						// ��ѡ
						if (SendMessage(jieshudaojishicheckbox, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� �Ͽ��󲻽�������ʱ ��ѡ���Ƿ�δ����ѡ
							// δ��ѡ
							//ȡ�����õ���ʱ
							EnableWindow(countdownLabel, TRUE);
							EnableWindow(countdownEdit, TRUE);
							EnableWindow(countdownUnitComboBox, TRUE);
						}
					} else {
						// ȡ����ѡ
						// ���õ���ʱ
						EnableWindow(countdownLabel, FALSE);
						EnableWindow(countdownEdit, FALSE);
						EnableWindow(countdownUnitComboBox, FALSE);
					}
					if (SendMessage(hBtnWNetAddConnection2A, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �жϵ�ѡ���ĸ���ѡ��
						typedef DWORD(WINAPI * LPWNETCANCELCONNECTION2A)(LPCSTR, DWORD, BOOL);
						// ���ؿ��ļ�
						HMODULE hMpr = LoadLibrary("mpr.dll");
						if (hMpr == NULL) {
							std::cout << "�޷�����mpr.dll���ļ�" << std::endl;
							return 1;
						}
						// ��ȡ������ַ
						LPWNETCANCELCONNECTION2A pWNetCancelConnection2A = (LPWNETCANCELCONNECTION2A)GetProcAddress(hMpr, "WNetCancelConnection2A");
						if (pWNetCancelConnection2A == NULL) {
							std::cout << "�޷���ȡWNetCancelConnection2A������ַ" << std::endl;
							FreeLibrary(hMpr);
							return 1;
						}
						DWORD result = NO_ERROR; // ����һ��result����
						// ѭ���Ͽ���������������
						for (char driveLetter = 'Z'; driveLetter >= 'A'; --driveLetter) {
							std::string drive = std::string(1, driveLetter) + ":";
							result = pWNetCancelConnection2A(drive.c_str(), CONNECT_UPDATE_PROFILE, TRUE);
							if (result == NO_ERROR) {
								std::cout << "���������� " << drive << " �Ͽ��ɹ���OwO�����룺" << result << "��" << std::endl;
							} else {
								std::cout << "���������� " << drive << " �Ͽ�ʧ�ܣ�X_X�����룺" << result << "��" << std::endl;
							}
						}
						// �ͷſ��ļ�
						FreeLibrary(hMpr);
						// �ر���Դ������
						system("taskkill /f /im explorer.exe");
						// ��ȡ Windows Ŀ¼��·��
						TCHAR windowsDir[MAX_PATH];
						if (GetWindowsDirectory(windowsDir, MAX_PATH) > 0) {
							// ������Դ������������·��
							TCHAR explorerPath[MAX_PATH];
							wsprintf(explorerPath, TEXT("%s\\explorer.exe"), windowsDir);

							// ������Դ������
							ShellExecute(NULL, TEXT("open"), explorerPath, NULL, NULL, SW_SHOWNORMAL);
						}
						// ��ʾ��Ϣ��
						std::string komplettMessage = "�ѶϿ�����ӳ���������������(*�����)\n�绹�������볢����������������ԣ���(T^T)\n�����룺" + std::to_string(result) + "��";
						// ����һ��������ı�
						HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("΢���ź�"));
						// ���´���������Ӧ�����ı��ؼ�
						SendMessage(countdownLabel, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
						MessageBox(hwnd, komplettMessage.c_str(), "�������OwO", MB_OK | MB_ICONINFORMATION);
					} else if (SendMessage(hBtnNetCommandLine, BM_GETCHECK, 0, 0) == BST_CHECKED) {
						string command = "net use * /del /Y";
						system(command.c_str());
						// �ر���Դ������
						system("taskkill /f /im explorer.exe");
						// ��ȡ Windows Ŀ¼��·��
						TCHAR windowsDir[MAX_PATH];
						if (GetWindowsDirectory(windowsDir, MAX_PATH) > 0) {
							// ������Դ������������·��
							TCHAR explorerPath[MAX_PATH];
							wsprintf(explorerPath, TEXT("%s\\explorer.exe"), windowsDir);

							// ������Դ������
							ShellExecute(NULL, TEXT("open"), explorerPath, NULL, NULL, SW_SHOWNORMAL);
						}
						// ����һ��������ı�
						HFONT hFont = CreateFont(20.9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("΢���ź�"));
						// ���´���������Ӧ�����ı��ؼ�
						SendMessage(countdownLabel, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
						MessageBox(hwnd, "����ɲ�����qwq\n������������볢����������������ԣ���X_X", "�������qwq", MB_OK | MB_ICONINFORMATION);
					}
					break;
				}

				case 3339: { // ����ר�����ҵ�ӳ���������ļ�
					int result = MessageBox(hwnd, "ȷ�ϴ���bat�������ļ���[ �� _ �� ?]\nȷ�Ϻ�������ѡ�񴴽�λ�ú�Ϊ�ļ�����\n�ڴ���ǰ��ȷ��ӳ����Ϣ����Ŷ��\n��ע�⣺���ļ���һ��й¶�˺�����ķ�\n�գ�������ý��ļ��������ĸ�������", "ѯ��[ �� _ �� ?]", MB_OKCANCEL | MB_ICONQUESTION);
					if (result == IDOK) {
						// ��ȡ�ؼ�ֵ
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
						// �������Ϊ����ѡ�����Ϊ�ļ�·��
						OPENFILENAME ofn; // ����һ��OPENFILENAME�ṹ��������������Ϊ�ļ��Ի������Ϣ
						char szFile[MAX_PATH] = {0}; // �������Ϊѡ����ļ�·��
						strcpy(szFile, ("ӳ��(" + username + " " + sharedFolder + " " + drive + ")").c_str()); // ���û����͹̶��ַ�����������������������Ƶ�szFile��
						ZeroMemory(&ofn, sizeof(ofn)); // ��ofn�ṹ��������ڴ����㣬��ȷ�����г�Ա�����ĳ�ʼֵΪ0
						ofn.lStructSize = sizeof(ofn); // ����ofn�ṹ������Ĵ�С���Ա�Ի����ܹ���ȷʶ��ṹ��İ汾
						ofn.hwndOwner = hwnd; // ���öԻ���ĸ����ھ����ָ���ĸ������ǶԻ����������
						ofn.lpstrFilter = "Windows�������ļ� (*.bat)\0*.bat\0";  // �����ļ��������������û�ֻ��ѡ����չ��Ϊ.bat���ļ�
						ofn.lpstrFile = szFile; // �����������Ϊѡ����ļ�·���Ļ�����
						ofn.nMaxFile = MAX_PATH; // ���û������Ĵ�С����ȷ�����ܹ��������·������
						ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;  // ���öԻ���ı�־�������Ƿ���ʾ�����ļ���ʾ��·����������Լ�����ֻ���ļ���ѡ���
						/*
						�����ǳ��õĶԻ����־��
						OFN_ALLOWMULTISELECT �������û�ѡ�����ļ�
						OFN_CREATEPROMPT �����ָ�����ļ������ڣ���ʾ�û��Ƿ񴴽����ļ�
						OFN_EXPLORER ��ʹ���°���ļ�ѡ��Ի�����ʽ��Windows XP�����߰汾��
						OFN_FILEMUSTEXIST ��Ҫ���û�ѡ����ļ��������
						OFN_HIDEREADONLY ������ֻ���ļ���ѡ��
						OFN_NOCHANGEDIR ���Ի���رպ󣬲��ı䵱ǰĿ¼
						OFN_NODEREFERENCELINKS ����������ݷ�ʽ��Ŀ���ļ�
						OFN_OVERWRITEPROMPT �����ָ�����ļ��Ѵ��ڣ���ʾ�û��Ƿ񸲸Ǹ��ļ�
						OFN_PATHMUSTEXIST ��Ҫ���û�ѡ���·���������
						OFN_READONLY ����ѡ����ļ�����Ϊֻ��ģʽ
						OFN_SHOWHELP ����ʾ������ť
						*/
						ofn.lpstrDefExt = "bat"; // ����Ĭ�ϵ��ļ���չ��Ϊ.bat���Ա����û�û��������չ��ʱ�Զ����
						if (GetSaveFileName(&ofn)) {
							// �û�ѡ�����ļ�·����������ʹ��szFile���к�������
							// ����bat�ļ�
							std::string filePath = ofn.lpstrFile;
							std::ofstream file(filePath.c_str()); // ��std::stringת��Ϊconst char*
							if (file) {
								// д��bat�ļ�
								file << "::*��Windows�������ļ��ɿ�ݲ˵�����" << std::endl;
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
								MessageBox(hwnd, "bat�������ļ������ɹ���", "�����ɹ�(>��-*)/", MB_OK | MB_ICONINFORMATION);
							} else {
								MessageBox(hwnd, "�޷�����bat�������ļ���", "����(�i�s^�t�i)", MB_OK | MB_ICONERROR);
							}
						}
					}
					break;
				}

				case 4: { // ��ϸ˵��
					MessageBox(hwnd, "Q&A�ʴ�Question&Answer����\nQ������ʲô����ʲô�ã�\nA�����ǿ��Խ�NAS����洢��ӳ�䵽���������������������������൱��Ӳ�̣��ĸ�������������ʹ������Я��ʹ������Զ������洢���豸\nQ��NAS��ʲô��\nA��NAS��Network Attached Storage�����總���洢���������˵���������������ϣ��߱����ϴ洢���ܵ�װ�ã����Ҳ��Ϊ������洢����������һ��ר�����ݴ洢������������Դ�� �ٶȰٿ�\n\nʹ����֪��\n������û������洢��D���У�����У������û��D�̽��洢�ڴ˳�������λ�õ��ļ����С��Ƽ����˳�����ͬ�ļ��з��ڳ�C������������С��ɴ�����ݷ�ʽ������ʹ��\n\n�߼��÷���\n���������ڹ�ѡ\"�Զ��򿪴洢�豸\"��ѡ���ӳ�䣬ӳ��ɹ����Զ���ӳ����ļ���\n\n������֧�֣�\nʹ�õ��Ĳ���ӳ����룺net use [�̷�]: \\[IP]\[�����ļ���]��ʹ�õ��Ĳ��ֶϿ����룺net use * /del /Y�����ϴ����Ϊ�����У�������ʾ�������룬������������ֱ��ʹ�á������ṩ�Ĵ��벢��Ϊ�������õ������д��룬���������������ѯ����ӳ�丨������֧�ֶ��ֲ�ͬ��ӳ���߼��жϣ�������ǹ���Ա����������˻�ֱ�ӽ���ӳ�䡣֧���Զ��嵹��ʱʱ�估��λ����������ڵ���ʱʱ����˳��������ѡ�֧�����ص���̨��ϵͳ���̣��������е���ʱ���������������ڵ���˳�-ѡ�����ص�������-Ȼ��ȷ������������˲������ѿ������������ã�\n\n�������⼰����취��\n��ʹ�õĹ����п��ܻ�������������Ӧ�������������������������ĵȴ����룬��ȴ�ʱ�����������ǿ�ƹرճ����������Ҽ�-����������������-�л���Ӧ�ó���ѡ��ҳ-ѡ��˳���-�����������\n\n����ӳ�䷵��ֵ��\n1. ERROR_SUCCESS (0): �����ɹ���ɡ�2. ERROR_ALREADY_ASSIGNED (85): ָ���ı����豸�Ѿ������䡣3. ERROR_BAD_DEVICE (1200): ָ�����豸����Ч��4. ERROR_BAD_NET_NAME (67): ָ����������Դ������Ч��5. ERROR_BAD_PROVIDER (1204): ָ�����ṩ����������Ч��6. ERROR_CANCELLED (1223): �������û�ȡ����7. ERROR_EXTENDED_ERROR (1208): �й���ϸ������Ϣ�������WNetGetLastError������8. ERROR_INVALID_PARAMETER (87): �ṩ�Ĳ�����Ч��9. ERROR_NO_NET_OR_BAD_PATH (1203): ָ���������ṩ����������Ч��10. ERROR_NO_NETWORK (1222): �޷��������硣11. ERROR_NOT_CONNECTED (2250): ���������Ѿ����ڡ�12. ERROR_OPEN_FILES (2401): �д򿪵��ļ����豸��13. ERROR_SESSION_CREDENTIAL_CONFLICT (1219): �Ựƾ�������лỰ��ͻ��14. ERROR_BAD_PROFILE (1206): ָ���������ļ���Ч��15. ERROR_CANNOT_OPEN_PROFILE (1205): �޷���ָ���������ļ���16. ERROR_DEVICE_IN_USE (2404): �豸���ڱ�ʹ���С�17. ERROR_NOT_SUPPORTED (50): ��������֧�֡�18. ERROR_INVALID_PASSWORD (86): ָ�����������벻��ȷ��19. ERROR_REDIR_UNFULLFILLED (2403): �ض�������δ���㡣20. ERROR_UNEXP_NET_ERR (59): ������������\n\n���ǵ����Ĳ��ܻ��������������Ͷ����ǵ�֧�֣���󣬸�л����ʹ�ã����������ߣ�Ling", "��ϸ˵��(>��<)", MB_OK );
					break;
				}

				case 7: { // ��/��һҳ
					// �жϿؼ��Ƿ�����
					LONG style = GetWindowLong(yingsheshezhi, GWL_STYLE);
					if (!(style & WS_VISIBLE)) {
						// �ؼ�������
						// ��ʾ��һҳ
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
						// ���صڶ�ҳ
						ShowWindow(SaveuserIP, SW_HIDE);
						ShowWindow(Saveusershare, SW_HIDE);
						ShowWindow(yingsheshezhi2, SW_HIDE);
					} else {
						// �ؼ�δ����
						// ���ص�һҳ
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
						// ��ʾ�ڶ�ҳ
						ShowWindow(SaveuserIP, SW_SHOW);
						ShowWindow(Saveusershare, SW_SHOW);
						ShowWindow(yingsheshezhi2, SW_SHOW);
					}
					break;
				}

				/* ����������Ϣ�����кࣩܶ��ʹ��Ĭ�Ϲ��̴��� */
				default: {
					DefWindowProc(hwnd, msg, wParam, lParam);
					break;
				}
			}
			break;
		}

		/* ����������Ϣ���ࣩܶ��ʹ��Ĭ�ϳ����� */
		default: {
			DefWindowProc(hwnd, msg, wParam, lParam);
			break;
		}
	}
}

/* �˺������� ʵ�ù����� �Ӵ��ڵ���Ϣ */
LRESULT CALLBACK iToolsWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	// �����Ӵ�����Ϣ
	switch (msg) {

		// �����Ӵ��ڵĹر���Ϣ
		case WM_CLOSE: {
			// �ر��Ӵ���
			DestroyWindow(hwnd);
			break;
		}

		/* ������ʱ���������߳�ֹͣ */
		case WM_DESTROY: { // ����ʱ
			isiToolsWindowOpen = 0; // ��������Ϊ0����ʾ�������䡱�Ӵ����ѹر�
			break;
		}

		/* �ǿͻ�����Ҫ������ͻ���ʱ���͸����ڵ���Ϣ */
		case WM_NCCALCSIZE: {
			// ���ر߿�
			break;
		}

		/* �ڴ����Ͻ��л��Ʋ��� */
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			RECT rc;
			GetClientRect(hwnd, &rc);
			// ����һ����ɫ��ˢ����ѡ���豸��������
			HBRUSH hBrush = CreateSolidBrush(RGB(240, 240, 240));
			SelectObject(hdc, hBrush);
			// ���Ʋ˵�������
			RECT menuBarRect = { rc.left, rc.top, rc.right, rc.top + 32 };
			FillRect(hdc, &menuBarRect, hBrush);
			// �������
			HPEN hPen = CreatePen(PS_SOLID, 2, RGB(128, 128, 128));
			SelectObject(hdc, hPen);
			MoveToEx(hdc, rc.left, rc.top, NULL);
			LineTo(hdc, rc.right, rc.top);
			LineTo(hdc, rc.right, rc.bottom);
			LineTo(hdc, rc.left, rc.bottom);
			LineTo(hdc, rc.left, rc.top);
			// ���ơ�X����ť
			HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("System"));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(50, 50, 50));
			SetBkMode(hdc, TRANSPARENT);
			TextOut(hdc, rc.right - 24, rc.top + 10, "X", 1);
			// ���ơ�-����ť
			WCHAR minimizeButton[] = { 0x2013, '\0' }; // ʹ��Unicode�����ơ�-�����Ż����һЩ
			SetTextColor(hdc, RGB(50, 50, 50));
			SetBkMode(hdc, TRANSPARENT);
			TextOutW(hdc, rc.right - 60, rc.top + 10, minimizeButton, lstrlenW(minimizeButton));
			// ��ȡ����ͼ��
			HICON hIcon = (HICON)LoadImage(NULL, "��ݲ˵�.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
			if (hIcon != NULL) {
				// ����ͼ��λ��
				int iconWidth = 24; // ͼ��ĳ�
				int iconHeight = 24; // ͼ��Ŀ�
				int iconX = rc.left + 4;
				int iconY = rc.top + (32 - iconHeight) / 2;
				// ����ͼ��
				DrawIconEx(hdc, iconX, iconY, hIcon, iconWidth, iconHeight, 0, NULL, DI_NORMAL);
			}
			// ��ȡ���ڱ���
			TCHAR title[256];
			GetWindowText(hwnd, title, 256);
			// ���ƴ��ڱ���
			HFONT hTitleFont = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("΢���ź�"));
			SelectObject(hdc, hTitleFont);
			SetTextColor(hdc, RGB(0, 0, 0));
			SetBkMode(hdc, TRANSPARENT);
			TextOut(hdc, rc.left + 31, rc.top + 7, title, lstrlen(title));
			// ɾ����ˢ�����ʺ��������
			DeleteObject(hBrush);
			DeleteObject(hPen);
			DeleteObject(hFont);
			DeleteObject(hTitleFont);
			EndPaint(hwnd, &ps);
			break;
		}

		/* ������ڴ��ڵķǿͻ����ƶ�ʱ������������λ�� */
		case WM_NCHITTEST: {
			POINT pt = { LOWORD(lParam), HIWORD(lParam) };
			ScreenToClient(hwnd, &pt);
			RECT rc;
			GetClientRect(hwnd, &rc);
			// ����Ƿ����ˡ�X����ť
			if (pt.x > rc.right - 37 && pt.x < rc.right - 0 && pt.y > rc.top + 0 && pt.y < rc.top + 32) {
				return HTCLOSE;
			}
			// ����Ƿ����ˡ�-����ť
			if (pt.x > rc.right - 70 && pt.x < rc.right - 35 && pt.y > rc.top + 0 && pt.y < rc.top + 32) {
				return HTMINBUTTON;
			}
			// ����������λ�ã���������Ӧ��ֵ
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

		/* ������ڴ��ڵķǿͻ���������ʱ��ִ����Ӧ���� */
		case WM_NCLBUTTONUP: {
			if (wParam == HTCLOSE) {
				// �������ˡ�X����ť�����͹رմ��ڵ���Ϣ
				SendMessage(hwnd, WM_CLOSE, 0, 0);
				return 0;
			} else if (wParam == HTMINBUTTON) {
				// �������ˡ�-����ť��������С�����ڵ���Ϣ
				SendMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
				return 0;
			}
			break;
		}

		/* ������ڴ��ڵķǿͻ���������������ڱ������ϣ�ִ�����²��� */
		case WM_NCLBUTTONDOWN: {
			if (wParam == HTCAPTION) {
				SetForegroundWindow(hwnd); // ����������Ϊǰ̨����
				SendMessage(hwnd, WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0); // �����ƶ����ڵ���Ϣ
				return 0;
			}
			break;
		}

		/* ֪ͨ���ڵĻ״̬�����仯 */
		case WM_ACTIVATE: {
			if (wParam == WA_INACTIVE) {
				// ���˳���Ľ��㶪ʧʱ�����ر��������Է�Windows7ϵͳ�л���������������ԭ��������ʾ������
				SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_CAPTION); // ���ر�����
				SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER); // ʹ�ı���Ч
			}
			break;
		}

		/* ���þ�̬�ؼ��ı�����ɫ */
		case WM_CTLCOLORSTATIC: {
			HDC hdcStatic = (HDC)wParam; // ��ȡ��̬�ؼ����豸�����ľ��
			SetTextColor(hdcStatic, RGB(0, 0, 0)); // �����ı���ɫΪ��ɫ
			SetBkColor(hdcStatic, RGB(255, 255, 255)); // ���ñ�����ɫΪ����ɫ
			return (INT_PTR)CreateSolidBrush(RGB(255, 255, 255)); // ����һ������ɫ�Ļ�ˢ��Ϊ��̬�ؼ��ı���ɫ
		}

		/* ������ť */
		case WM_CREATE: { // ������ť
			if (SendMessage(A_Top, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �ж� �ö� ��ѡ���Ƿ񱻹�ѡ
				// ��ѡ
				SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // �ö�
			}
			HWND SystemToolsGroupBox = CreateWindow("BUTTON", "ϵͳ����", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 10, 35, 210, 305, hwnd, NULL, NULL, NULL);
			HWND restartButton = CreateWindow("BUTTON", "������Դ�������������", WS_VISIBLE | WS_CHILD | BS_FLAT, 20, 60, 190, 30, hwnd, (HMENU)1, NULL, NULL);
			HWND RestartResourceManager = CreateWindow("BUTTON", "������Դ������", WS_VISIBLE | WS_CHILD | BS_FLAT, 20, 100, 190, 30, hwnd, (HMENU)2, NULL, NULL);
			HWND Systemlevelhiddenfiles = CreateWindow("BUTTON", "ϵͳ������/��ʾ�ļ�", WS_VISIBLE | WS_CHILD | BS_FLAT, 20, 140, 190, 30, hwnd, (HMENU)3, NULL, NULL);
			HWND Systemlevelhiddenfolder = CreateWindow("BUTTON", "ϵͳ������/��ʾ�ļ���", WS_VISIBLE | WS_CHILD | BS_FLAT, 20, 180, 190, 30, hwnd, (HMENU)4, NULL, NULL);
			HWND OthertoolsGroupBox = CreateWindow("BUTTON", "��������", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 225, 35, 210, 305, hwnd, NULL, NULL, NULL);

			// ����һ��������ı�
			HFONT hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("΢���ź�"));
			// ���´���������Ӧ�����ı��ؼ�
			SendMessage(SystemToolsGroupBox, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(restartButton, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(OthertoolsGroupBox, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(RestartResourceManager, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(Systemlevelhiddenfiles, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(Systemlevelhiddenfolder, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			break;
		}

		case WM_COMMAND: { // ����ť����¼�
			switch (wParam) {

				case 1: { // ��������Ժ���Դ������
					// �ر���Դ������
					system("taskkill /f /im explorer.exe");
					// ��ȡ Windows Ŀ¼��·��
					TCHAR windowsDir[MAX_PATH];
					if (GetWindowsDirectory(windowsDir, MAX_PATH) > 0) {
						// ������Դ������������·��
						TCHAR explorerPath[MAX_PATH];
						wsprintf(explorerPath, TEXT("%s\\explorer.exe"), windowsDir);

						// ������Դ������
						ShellExecute(NULL, TEXT("open"), explorerPath, NULL, NULL, SW_SHOWNORMAL);
					}
					//���������
					system("gpupdate /force");

					//�����û�����ɲ���
					MessageBox(hwnd, "����ɲ�����OwO", "OK����OwO", MB_OK | MB_ICONINFORMATION);
					break;
				}

				case 2: { // ������Դ������
					// �ر���Դ������
					system("taskkill /f /im explorer.exe");
					// ��ȡ Windows Ŀ¼��·��
					TCHAR windowsDir[MAX_PATH];
					if (GetWindowsDirectory(windowsDir, MAX_PATH) > 0) {
						// ������Դ������������·��
						TCHAR explorerPath[MAX_PATH];
						wsprintf(explorerPath, TEXT("%s\\explorer.exe"), windowsDir);

						// ������Դ������
						ShellExecute(NULL, TEXT("open"), explorerPath, NULL, NULL, SW_SHOWNORMAL);
					}

					//�����û�����ɲ���
					MessageBox(hwnd, "����ɲ�����OwO", "OK����OwO", MB_OK | MB_ICONINFORMATION);
					break;
				}

				case 3: { // ϵͳ������/��ʾ�ļ�
					// �����򿪴���ѡ����ļ�·��
					OPENFILENAME ofn; // ����һ��OPENFILENAME�ṹ����������ڴ��ļ��Ի������Ϣ
					char szFile[MAX_PATH] = {0}; // ���ڴ洢�û�ѡ����ļ�·��
					ZeroMemory(&ofn, sizeof(ofn)); // ��ofn�ṹ��������ڴ����㣬��ȷ�����г�Ա�����ĳ�ʼֵΪ0
					ofn.lStructSize = sizeof(ofn); // ����ofn�ṹ������Ĵ�С���Ա�Ի����ܹ���ȷʶ��ṹ��İ汾
					ofn.hwndOwner = hwnd; // ���öԻ���ĸ����ھ����ָ���ĸ������ǶԻ����������
					ofn.lpstrFilter = "�����ļ� (*.*)\0*.*\0";  // �����ļ��������������û�ѡ���������͵��ļ�
					ofn.lpstrFile = szFile; // �������ڴ洢�û�ѡ����ļ�·���Ļ�����
					ofn.nMaxFile = MAX_PATH; // ���û������Ĵ�С����ȷ�����ܹ��������·������
					ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_FORCESHOWHIDDEN; // ���öԻ���ı�־�������ļ�������ڡ�·����������Լ�����ֻ���ļ���ѡ��ȡ����OFN_FORCESHOWHIDDEN��־�������ļ��Ի����п���ϵͳ�����ص��ļ�
					/*
					�����ǳ��õĶԻ����־��
					OFN_ALLOWMULTISELECT �������û�ѡ�����ļ�
					OFN_CREATEPROMPT �����ָ�����ļ������ڣ���ʾ�û��Ƿ񴴽����ļ�
					OFN_EXPLORER ��ʹ���°���ļ�ѡ��Ի�����ʽ��Windows XP�����߰汾��
					OFN_FILEMUSTEXIST ��Ҫ���û�ѡ����ļ��������
					OFN_HIDEREADONLY ������ֻ���ļ���ѡ��
					OFN_NOCHANGEDIR ���Ի���رպ󣬲��ı䵱ǰĿ¼
					OFN_NODEREFERENCELINKS ����������ݷ�ʽ��Ŀ���ļ�
					OFN_OVERWRITEPROMPT �����ָ�����ļ��Ѵ��ڣ���ʾ�û��Ƿ񸲸Ǹ��ļ�
					OFN_PATHMUSTEXIST ��Ҫ���û�ѡ���·���������
					OFN_READONLY ����ѡ����ļ�����Ϊֻ��ģʽ
					OFN_SHOWHELP ����ʾ������ť
					*/
					// ��ȡ�û�ѡ����ļ�·��
					if (GetOpenFileName(&ofn)) {
						// ʹ�� Windows API ���� GetFileAttributes() ����ȡָ���ļ�������
						DWORD attributes = GetFileAttributes(szFile);
						if (attributes != INVALID_FILE_ATTRIBUTES) {
							// ����ļ����ԣ��ж��Ƿ���ϵͳ�������ļ�
							if ((attributes & FILE_ATTRIBUTE_SYSTEM) && (attributes & FILE_ATTRIBUTE_HIDDEN)) {
								// �ļ���ϵͳ�������ļ���ȡ������
								// �û��Ѿ�ѡ����һ���ļ������������ﴦ��ѡ����ļ�·��
								// szFile �����д洢���û�ѡ����ļ�·��
								string command = "attrib -s -h ";
								// �ڵ��� system ����ִ��ȡ�������ļ�������ʱ�����ļ�·��������������
								command += "\"";
								command += szFile;
								command += "\"";
								int result = system(command.c_str());
								if (result == 0) {
									// ȡ����������ִ�гɹ�
									MessageBox(hwnd, "��ʾ����ִ�гɹ���OvO\n�����δȡ�����������볢��ˢ��", "�ɹ�����", MB_OK | MB_ICONINFORMATION);
								} else {
									// ȡ����������ִ��ʧ��
									MessageBox(hwnd, "��ʾ����ִ��ʧ�ܣ�QAQ", "ʧ�������أ�", MB_OK | MB_ICONERROR);
								}
							} else {
								// �ļ�����ϵͳ�������ļ��������ļ�
								// �û��Ѿ�ѡ����һ���ļ������������ﴦ��ѡ����ļ�·��
								// szFile �����д洢���û�ѡ����ļ�·��
								string command = "attrib +s +h ";
								// �ڵ��� system ����ִ�������ļ�������ʱ�����ļ�·������������������������ȷ��ϵͳ��ȷʶ������ո���ļ�·���������ͻὫ�ļ�·����˫������������ȷ����ȷ��������ո���ļ�·����ʹ�����ļ��������ܹ���ȷִ��
								command += "\"";
								command += szFile;
								command += "\"";
								int result = system(command.c_str());
								if (result == 0) {
									// ����ִ�гɹ�
									MessageBox(hwnd, "��������ִ�гɹ���OvO\n�����δ���������볢��ˢ��", "�ɹ�����", MB_OK | MB_ICONINFORMATION);
								} else {
									// ����ִ��ʧ��
									MessageBox(hwnd, "��������ִ��ʧ�ܣ�QAQ", "ʧ�������أ�", MB_OK | MB_ICONERROR);
								}
							}
						} else {
							// ��ȡ�ļ�����ʧ��
							MessageBox(hwnd, "��ȡ�ļ�����ʧ�ܣ�", "����", MB_OK | MB_ICONERROR);
						}
					}
					break;
				}

				case 4: { // ϵͳ������/��ʾ�ļ���
					// ����ѡ���ļ��еĴ���
					OPENFILENAME ofn; // ����һ��OPENFILENAME�ṹ�����������ѡ���ļ��жԻ������Ϣ
					char szDir[MAX_PATH] = {0}; // ���ڴ洢�û�ѡ����ļ���·��
					ZeroMemory(&ofn, sizeof(ofn)); // ��ofn�ṹ��������ڴ����㣬��ȷ�����г�Ա�����ĳ�ʼֵΪ0
					ofn.lStructSize = sizeof(ofn); // ����ofn�ṹ������Ĵ�С���Ա�Ի����ܹ���ȷʶ��ṹ��İ汾
					ofn.hwndOwner = hwnd; // ���öԻ���ĸ����ھ����ָ���ĸ������ǶԻ����������
					ofn.lpstrTitle = "��ѡ���ļ��У��뽫�ļ������ƻ�·�����ƽ��ļ���������������ƣ���Ҫ����������ļ����ڻ�ǰһ��Ŀ¼��"; // ����ѡ���ļ��жԻ���ı���
					ofn.lpstrFilter = "��ϸ�����ڱ���\0*.*\0";  // �����ļ��������������û�ѡ���������͵��ļ�
					ofn.lpstrFile = szDir; // �������ڴ洢�û�ѡ����ļ���·���Ļ�����
					ofn.nMaxFile = MAX_PATH; // ���û������Ĵ�С����ȷ�����ܹ��������·������
					ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_FORCESHOWHIDDEN | OFN_NOCHANGEDIR | OFN_NOVALIDATE | OFN_DONTADDTORECENT; // ���öԻ���ı�־����������ֻ���ļ����ļ�������ڡ�·��������ڡ�����֤�ļ����Ͳ���ѡ����ļ�����ӵ����ʹ���ļ����б��
					/*
					�����ǳ��õĶԻ����־��
					OFN_ALLOWMULTISELECT �������û�ѡ�����ļ�
					OFN_CREATEPROMPT �����ָ�����ļ������ڣ���ʾ�û��Ƿ񴴽����ļ�
					OFN_EXPLORER ��ʹ���°���ļ�ѡ��Ի�����ʽ��Windows XP�����߰汾��
					OFN_FILEMUSTEXIST ��Ҫ���û�ѡ����ļ��������
					OFN_HIDEREADONLY ������ֻ���ļ���ѡ��
					OFN_NOCHANGEDIR ���Ի���رպ󣬲��ı䵱ǰĿ¼
					OFN_NODEREFERENCELINKS ����������ݷ�ʽ��Ŀ���ļ�
					OFN_OVERWRITEPROMPT �����ָ�����ļ��Ѵ��ڣ���ʾ�û��Ƿ񸲸Ǹ��ļ�
					OFN_PATHMUSTEXIST ��Ҫ���û�ѡ���·���������
					OFN_READONLY ����ѡ����ļ�����Ϊֻ��ģʽ
					OFN_SHOWHELP ����ʾ������ť
					*/
					// ��ʾѡ���ļ��жԻ���
					if (GetOpenFileName(&ofn)) {
						// ʹ�� Windows API ���� GetFileAttributes() ����ȡָ���ļ�������
						DWORD attributes = GetFileAttributes(szDir);
						if (attributes != INVALID_FILE_ATTRIBUTES) {
							// ����ļ����ԣ��ж��Ƿ���ϵͳ�������ļ�
							if ((attributes & FILE_ATTRIBUTE_SYSTEM) && (attributes & FILE_ATTRIBUTE_HIDDEN)) {
								// �ļ���ϵͳ�������ļ���ȡ������
								// �û��Ѿ�ѡ����һ���ļ������������ﴦ��ѡ����ļ�·��
								// szFile �����д洢���û�ѡ����ļ�·��
								string command = "attrib -s -h ";
								// �ڵ��� system ����ִ��ȡ�������ļ�������ʱ�����ļ�·��������������
								command += "\"";
								command += szDir;
								command += "\"";
								int result = system(command.c_str());
								if (result == 0) {
									// ȡ����������ִ�гɹ�
									MessageBox(hwnd, "��ʾ����ִ�гɹ���OvO\n�����δȡ�����������볢��ˢ��\n����Ҫ������Ŀ¼�������ļ�", "�ɹ�����", MB_OK | MB_ICONINFORMATION);
								} else {
									// ȡ����������ִ��ʧ��
									MessageBox(hwnd, "��ʾ����ִ��ʧ�ܣ�QAQ", "ʧ�������أ�", MB_OK | MB_ICONERROR);
								}
							} else {
								// �ļ�����ϵͳ�������ļ��������ļ�
								// �û��Ѿ�ѡ����һ���ļ������������ﴦ��ѡ����ļ�·��
								// szFile �����д洢���û�ѡ����ļ�·��
								string command = "attrib +s +h ";
								// �ڵ��� system ����ִ�������ļ�������ʱ�����ļ�·������������������������ȷ��ϵͳ��ȷʶ������ո���ļ�·���������ͻὫ�ļ�·����˫������������ȷ����ȷ��������ո���ļ�·����ʹ�����ļ��������ܹ���ȷִ��
								command += "\"";
								command += szDir;
								command += "\"";
								int result = system(command.c_str());
								if (result == 0) {
									// ����ִ�гɹ�
									MessageBox(hwnd, "��������ִ�гɹ���OvO\n�����δ���������볢��ˢ��\n����Ҫ������Ŀ¼�������ļ�", "�ɹ�����", MB_OK | MB_ICONINFORMATION);
								} else {
									// ����ִ��ʧ��
									MessageBox(hwnd, "��������ִ��ʧ�ܣ�QAQ", "ʧ�������أ�", MB_OK | MB_ICONERROR);
								}
							}
						} else {
							// ��ȡ�ļ�����ʧ��
							MessageBox(hwnd, "��ȡ�ļ�����ʧ�ܣ�", "����", MB_OK | MB_ICONERROR);
						}
					}
					break;
				}

				/* ����������Ϣ�����кࣩܶ��ʹ��Ĭ�Ϲ��̴��� */
				default: {
					DefWindowProc(hwnd, msg, wParam, lParam);
					break;
				}
			}
			break;
		}

		/* ����������Ϣ���ࣩܶ��ʹ��Ĭ�ϳ����� */
		default: {
			DefWindowProc(hwnd, msg, wParam, lParam);
			break;
		}
	}
}

/* Win32 GUI����ġ�main������������ִ�п�ʼ�ĵط� */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	/* ����ʱ����������Ƿ��Ѵ��ڣ���������ٲ�ʹ�����ȴ��ڵ� */
	// ����һ��������
	HANDLE hMutex = CreateMutex(NULL, TRUE, "MyAppMutex");
	// ��黥�����Ƿ��Ѿ�����
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		// ����������Ѿ����ڣ���ʾ�Ѿ���һ�������ڽ��������У�������ٵ�ǰʵ��
		CloseHandle(hMutex);
		// �ڳ����˳�ǰ�ͷŻ�����
		CloseHandle(hMutex);
		//MessageBox(host_hwnd, "�˳�������������У���������û�У�����ϵͳ����", "��ʾO_O", MB_OK | MB_ICONASTERISK); // ��Ϊ���½����ڲ��ܷ�����Ϣ���ɴ�������ֻ�������û����в鿴 // �����õ���������ѽ��
		return 0;
	}

	WNDCLASSEX wc; // ���ڵ����Խṹ
	HWND hwnd; // һ����HANDLE���������H��������ָ�����ǵĴ��ڵ�ָ��
	MSG msg; // ������Ϣ����ʱλ��

	/* ���ṹ�����㲢����������Ҫ�޸ĵ����� */
	memset(&wc, 0, sizeof(wc));
	wc.cbSize         = sizeof(WNDCLASSEX);
	wc.lpfnWndProc    = WndProc; // �������ǽ�Ҫ������Ϣ�ĵط�
	wc.hInstance      = hInstance;
	wc.hCursor        = LoadCursor(NULL, IDC_ARROW);

	/* ��ɫ��COLOR_WINDOWֻ��һ��ϵͳ��ɫ��#define�����԰�Ctrl+������ */
	wc.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1); // 0ΪӦ�ó�����棨�ͻ�����Ĭ����ɫ������ƫһ���ң�;1Ϊ����ɫ���˳���Ĵ˲���Ĭ��Ϊ1��
	wc.lpszClassName  = "WindowClass";
	wc.hIcon          = LoadIcon(NULL, IDC_ICON); // ���ر�׼ͼ��
	wc.hIconSm        = LoadIcon(NULL, IDC_ICON); //  ʹ�����ơ�A����ʹ����Ŀͼ��
	HANDLE hIcon1 = LoadImage(NULL, "��ݲ˵�.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
	if (hIcon1) {
		// ���سɹ����� hIcon1 ��ֵ��������ṹ�� hIcon �� hIconSm ��Ա
		wc.hIcon = (HICON)hIcon1;
		wc.hIconSm = (HICON)hIcon1;
	} else {
		// ����ʧ��
		MessageBox(NULL, "ͼ�����ʧ�ܣ�X_X", "����X_X", MB_ICONERROR | MB_OK);
	}

	// ע�������ڵ���
	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "����ע��ʧ�ܣ�", "����", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// ע�� �˳� �Ӵ��ڵ���
	wc.lpfnWndProc   = exitWindowProc;
	wc.lpszClassName = "ExitWindowClass";
	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "����ע��ʧ�ܣ�", "����", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// ע�� ���� �Ӵ��ڵ���
	wc.lpfnWndProc = SettingsWndProc;
	wc.lpszClassName = "SettingsWindowClass";
	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "�Ӵ���ע��ʧ�ܣ�", "����", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// ע�� ���ô��ڿ�ݴ� �Ӵ��ڵ���
	wc.lpfnWndProc	 = ChildWindowProc;
	wc.lpszClassName = "ChildWindowClass";
	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "�Ӵ���ע��ʧ�ܣ�", "����", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// ע�� ����ӳ�丨������ �Ӵ��ڵ���
	wc.lpfnWndProc   = ChildWndProc;
	wc.lpszClassName = childwindow;
	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "�Ӵ���ע��ʧ�ܣ�", "����", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// ע�� ʵ�ù����� �Ӵ��ڵ���
	wc.lpfnWndProc	 = iToolsWindowProc;
	wc.lpszClassName = "iToolsWindowClass";
	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "�Ӵ���ע��ʧ�ܣ�", "����", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	/* ʹ����ʼ�ճ�������Ļ�м� */
	// ��ȡ��Ļ��Ⱥ͸߶�
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	// ���㴰�����Ͻ�����
	int windowWidth = 420; // ���ڿ��
	int windowHeight = 320; // ���ڸ߶�
	int windowX = (screenWidth - windowWidth) / 2;
	int windowY = (screenHeight - windowHeight) / 2;

	host_hwnd = CreateWindowEx(WS_EX_APPWINDOW, "WindowClass", "��ݲ˵�(��'��'��)", WS_VISIBLE | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, windowX, windowY, windowWidth, windowHeight, NULL, NULL, hInstance, NULL);

	if (host_hwnd == NULL) {
		MessageBox(NULL, "���ڴ���ʧ�ܣ�", "����", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// ��ʼ������ͼ��
	memset(&nid, 0, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.hWnd = host_hwnd;
	nid.uID = 1;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_USER + 1;
	HANDLE hIcon2 = LoadImage(NULL, "��ݲ˵�.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
	if (hIcon2) {
		// ���سɹ����� hIcon2 ��ֵ������ͼ��� hIcon ��Ա
		nid.hIcon = (HICON)hIcon2;
	} else {
		// ����ʧ�ܣ�ʹ��Ĭ��ͼ��
		HICON hIcon2 = LoadIcon(NULL, IDI_APPLICATION);
		nid.hIcon = (HICON)hIcon2;
	}
	strcpy(nid.szTip, "��ݲ˵�");
	// �������ͼ��
	Shell_NotifyIcon(NIM_ADD, &nid);

	/*
	    �������ǳ���ĺ��ģ��������붼�ᱻ�������͵�WndProc����ע�⣬
	    GetMessage��������������ֱ�������յ���Ϣ����˴�ѭ���������������ĸ�CPUʹ����
	*/
	while (GetMessage(&msg, NULL, 0, 0) > 0) { // ���û���յ�����...
		TranslateMessage(&msg); // ����У�������ת��Ϊ�ַ�
		DispatchMessage(&msg); // ���䷢�͵�WndProc
	}
	return msg.wParam;
}
