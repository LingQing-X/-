#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_set>
using namespace std;

// ����ȫ�ֱ���
const char childwindow[] = "Drive mapping"; // ����ӳ�丨������ �Ӵ��ڵ�ȫ�ֱ���
int isExitWindowOpen = 0; // ����ʹ���˳����Ӵ���ֻ�ܴ�һ��
int isSettingsWindowOpen = 0; // ����ʹ�����á��Ӵ���ֻ�ܴ�һ��
int isWebsiteWindowOpen = 0; // ����ʹ����վ���Ӵ���ֻ�ܴ�һ��
int isMappingWindowOpen = 0; // ����ʹ��ӳ�䡱�Ӵ���ֻ�ܴ�һ��
int isLANchatWindowOpen = 0; // ����ʹ�����족�Ӵ���ֻ�ܴ�һ��
int isiToolsWindowOpen = 0; // ����ʹ�������䡱�Ӵ���ֻ�ܴ�һ��

// ���ھ��
HWND host_hwnd; // ���������ڵľ������
HWND hwnd_exit; // �������˳������ڵľ������
HWND hwnd_Settings; // ���������á����ڵľ������
HWND hwnd_website; // ��������վ�����ڵľ������
HWND hwnd_mapping; // ������ӳ�䡱���ڵľ������
HWND hwnd_LANchat; // ���������족���ڵľ������
HWND hwnd_iTools; // �����������䡱���ڵľ������

/* ����ӳ�丨������ �Ӵ��� */
// �ؼ����
HWND usernameLabel; // �˺�
HWND usernameEdit; // �˺� �����
HWND passwordLabel; // ����
HWND passwordEdit; // ���� �����
HWND ipLabel; // IP��ַ
HWND ipComboBox; //IP��ַ �����
HWND sharedFolderLabel; //����Ŀ¼
HWND sharedFolderEdit; // ����Ŀ¼ �����
HWND driveLabel; // �̷�
HWND driveComboBox; // �̷� ѡ�������˵�
HWND showPasswordCheckbox; // ��ʾ���� ��ѡ��
HWND connectButton; // ���� ��ť
HWND disconnectButton; // �Ͽ� ��ť
HWND countdownCheckbox; // ӳ��ɹ���ʼ��ʱ ��ѡ��
HWND countdownLabel; // ʱ������
HWND countdownEdit; // ʱ������ �����
HWND countdownUnitComboBox; // ʱ�䵥λ����
HWND detailedButton; // ��ϸ ��ť
HWND jieshudaojishicheckbox; // �Ͽ��󲻽�������ʱ ��ѡ��
// �ؼ�ֵ
string username; // �˺�
string password; // ����
string ipAddress; // IP��ַ
string sharedFolder; // ����Ŀ¼
string drive; // �̷�
// ����ʱ����ʱ���ȫ�ֱ���
int countdownMinutes = 0; // ��
int countdownDuration = 0; // ��

/* �������д�������ĵط� */
HWND A_AOU, A_Top, A_text, A_1, A_2, A_3, A_4, A_5, A_6, A_7, A_8, A_strongTop;
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
		SetWindowPos(hwnd_LANchat, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // �ö����촰��
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
				// ���ء����족����
				ShowWindow(hwnd_LANchat, SW_HIDE);
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
				lstrcpy(nid.szInfo, TEXT("������Ŷ��qwq")); // ������Ϣ����
				Shell_NotifyIcon(NIM_MODIFY, &nid); // ������Ϣ
			} else  if (found2) {
				// ����"...2"�Ĵ����߼�
				// �˳�����
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
					int windowHeight = 180; // ���ڸ߶�
					int windowX = (screenWidth - windowWidth) / 2;
					int windowY = (screenHeight - windowHeight) / 2;
					hwnd_exit = CreateWindowEx(WS_EX_CLIENTEDGE, "ExitWindowClass", "ѯ��U_U", WS_SYSMENU, windowX, windowY, windowWidth, windowHeight, NULL, NULL, GetModuleHandle(NULL), NULL);
					ShowWindow(hwnd_exit, SW_SHOW);
					isExitWindowOpen = 1; // ��������Ϊ1����ʾ���˳����Ӵ����Ѵ�
				} else {
					// �ö���ȡ���ö����������û�
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

		/* ������ť */
		case WM_CREATE: { // ������ť
			A_AOU = CreateWindow("button", "�Զ��������Ĵ洢�豸", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 40, 15, 185, 15, hwnd, (HMENU)857, hInst, NULL); // �Զ��������Ĵ洢�豸 = Auto Open Inserted USB
			A_Top = CreateWindow("button", "�ö�����", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 250, 15, 80, 15, hwnd, (HMENU)00000, hInst, NULL);
			A_text = CreateWindow("Static", "��ӭʹ�ã�", WS_CHILD | WS_VISIBLE, 0, 195, 370, 35, hwnd, NULL, hInst, NULL);
			A_1 = CreateWindow("button", "������վ��ݴ�", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 35, 50, 135, 50, hwnd, (HMENU)1, hInst, NULL);
			A_2 = CreateWindow("button", "����ӳ�丨������", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 200, 50, 135, 50, hwnd, (HMENU)2, hInst, NULL);
			A_3 = CreateWindow("button", "������������Ϣ", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 35, 120, 135, 50, hwnd, (HMENU)3, hInst, NULL);
			A_4 = CreateWindow("button", "ʵ�ù�����", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 200, 120, 135, 50, hwnd, (HMENU)4, hInst, NULL);
			// ������ʱ��
			SetTimer(hwnd, 1, 500, NULL); // ��ʱ��ID: 1�����ʱ��: 1000���루1�룩
			break;
		}

		/* ��ʱ�� */
		case WM_TIMER: {
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

			// ��CPUռ�ú��ڴ�ռ�úϲ�Ϊһ���ַ���
			std::string usageStr = "��ӭʹ�ã���ǰ�汾��2.0\nCPUռ�ã�" + std::to_string(static_cast<int>(cpuUsage)) + "%" + "     �ڴ�ռ�ã�" + std::to_string(static_cast<int>(memoryUsagePercentage)) + "%";

			// ���»�ӭ�ı���ʾ���ı�
			SetWindowText(A_text, usageStr.c_str());
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
							SetWindowPos(host_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // �ö�
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
							SetWindowPos(host_hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // ȡ���ö�
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
								SetWindowPos(hwnd_LANchat, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // ȡ���ö����촰��
								SetWindowPos(hwnd_iTools, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // ȡ���ö������䴰��
							}
						}
					}
					break;
				}

				case 32912: { // ����
					if (isSettingsWindowOpen == 0) { // ����Ƿ��Ѿ����ˡ����á��Ӵ���
						/* ʹ����ʼ�ճ�������Ļ�м� */
						// ��ȡ��Ļ��Ⱥ͸߶�
						int screenWidth = GetSystemMetrics(SM_CXSCREEN);
						int screenHeight = GetSystemMetrics(SM_CYSCREEN);
						// ���㴰�����Ͻ�����
						int windowWidth = 300; // ���ڿ��
						int windowHeight = 350; // ���ڸ߶�
						int windowX = (screenWidth - windowWidth) / 2;
						int windowY = (screenHeight - windowHeight) / 2;

						// ��ť�����ʱ������ ������վ��ݴ� �Ӵ���
						hwnd_Settings = CreateWindowEx(WS_EX_CLIENTEDGE, "SettingsWindowClass", "����", WS_SYSMENU | WS_MINIMIZEBOX, windowX, windowY, windowWidth, windowHeight, NULL, NULL, GetModuleHandle(NULL), NULL);
						ShowWindow(hwnd_Settings, SW_SHOW);
						isSettingsWindowOpen = 1; // ��������Ϊ1����ʾ��ӳ�䡱�Ӵ����Ѵ�
					} else {
						MessageBox(hwnd, "�����Ѵ��ڣ�=-=", "��ʾ=-=", MB_OK | MB_ICONASTERISK);
						// �ö���ȡ���ö����������û�
						SetWindowPos(hwnd_Settings, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
						SetWindowPos(hwnd_Settings, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
					}
					break;
				}

				case 32913: { // ��ϸ
					MessageBox(hwnd, "�������⼰����취��\n��ʹ�õĹ����п��ܻ�������������Ӧ�������������������������ĵȴ����룬��ȴ�ʱ�����������ǿ�ƹرճ����������Ҽ�-����������������-�л���Ӧ�ó���ѡ��ҳ-ѡ��˳���-�����������\n\n���������������ͷ�����\n���Ƿǳ�Ը����������������ͷ�����Ӧ�õ���һ���汾�У����Ƕ��ڱȽ����ѶȵĹ��ܻῪ�������������½⡣���ǻᾡ�����ɴ󲿷����ô�����������һ�ӭ���ڳ���ʹ���з���BUG������©����ʱ�����������Ա�����һ���汾���޸��������ڵײ��BUG����ǣ����������ĵ�BUG����ͨ����Ҫ���õ�ʱ���������޸��������½⣡\n\n����Ŀ�����Դ���룺\n�˳�����С��èDev-C++������ʹ��C++���Կ�������������ݲ���Դ������ѧϰ��Ҫ���ǿ�Ϊ���ṩ���˼·\n\nʹ��˵��/����������\n���ǲ����ռ������κ���˽��Ϣ�������Ὣ��Ϣ�ϴ����ƶˡ������ʹ�ô˳���������κεĺ�������������˳е���\n\n��󣬸�л����ʹ�ã�\n					             ���������ߣ�Ling", "��ϸ^u^", MB_OK );
					break;
				}

				case 32914: { // ��������
					MessageBox(hwnd, "�������ݣ�\n\n2.0�汾��֮ǰ�汾��������ʡ�ԡ�2.0�汾��2023��7��5����ʽ��ʼ��������ͬ���9��27����ɿ�������ǰ�汾2126�д��룬��89515���ַ�\n\n��󣬸�л����ʹ�ã�", "��ϸ^u^", MB_OK );
					break;
				}

				case 1: { // ������վ��ݴ�
					if (isWebsiteWindowOpen == 0) { // ����Ƿ��Ѿ����ˡ���վ���Ӵ���
						/* ʹ����ʼ�ճ�������Ļ�м� */
						// ��ȡ��Ļ��Ⱥ͸߶�
						int screenWidth = GetSystemMetrics(SM_CXSCREEN);
						int screenHeight = GetSystemMetrics(SM_CYSCREEN);
						// ���㴰�����Ͻ�����
						int windowWidth = 520; // ���ڿ��
						int windowHeight = 365; // ���ڸ߶�
						int windowX = (screenWidth - windowWidth) / 2;
						int windowY = (screenHeight - windowHeight) / 2;

						// ��ť�����ʱ������ ������վ��ݴ� �Ӵ���
						hwnd_website = CreateWindowEx(WS_EX_CLIENTEDGE, "ChildWindowClass", "������վ��ݴ�", WS_SYSMENU | WS_MINIMIZEBOX, windowX, windowY, windowWidth, windowHeight, NULL, NULL, GetModuleHandle(NULL), NULL);
						ShowWindow(hwnd_website, SW_SHOW);
						isWebsiteWindowOpen = 1; // ��������Ϊ1����ʾ����վ���Ӵ����Ѵ�
					} else {
						MessageBox(hwnd, "�����Ѵ��ڣ�=-=", "��ʾ=-=", MB_OK | MB_ICONASTERISK);
						// �ö���ȡ���ö����������û�
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
						int windowWidth = 410; // ���ڿ��
						int windowHeight = 360; // ���ڸ߶�
						int windowX = (screenWidth - windowWidth) / 2;
						int windowY = (screenHeight - windowHeight) / 2;

						// ��ť�����ʱ������ ����ӳ�丨������ �Ӵ���
						hwnd_mapping = CreateWindowEx(WS_EX_CLIENTEDGE, childwindow, "����ӳ�丨������", WS_SYSMENU | WS_MINIMIZEBOX, windowX, windowY, windowWidth, windowHeight, NULL, NULL, GetModuleHandle(NULL), NULL);
						ShowWindow(hwnd_mapping, SW_SHOW);
						isMappingWindowOpen = 1; // ��������Ϊ1����ʾ��ӳ�䡱�Ӵ����Ѵ�
					} else {
						MessageBox(hwnd, "�����Ѵ��ڣ�=-=", "��ʾ=-=", MB_OK | MB_ICONASTERISK);
						// �ö���ȡ���ö����������û�
						SetWindowPos(hwnd_mapping, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
						SetWindowPos(hwnd_mapping, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
					}
					break;
				}

				case 3: { // ����������
					if (isLANchatWindowOpen == 0) { // ����Ƿ��Ѿ����ˡ����족�Ӵ���
						/* ʹ����ʼ�ճ�������Ļ�м� */
						// ��ȡ��Ļ��Ⱥ͸߶�
						int screenWidth = GetSystemMetrics(SM_CXSCREEN);
						int screenHeight = GetSystemMetrics(SM_CYSCREEN);
						// ���㴰�����Ͻ�����
						int windowWidth = 400; // ���ڿ��
						int windowHeight = 300; // ���ڸ߶�
						int windowX = (screenWidth - windowWidth) / 2;
						int windowY = (screenHeight - windowHeight) / 2;

						// ��ť�����ʱ������ ���������� �Ӵ���
						hwnd_LANchat = CreateWindowEx(WS_EX_CLIENTEDGE, "LANChatWindowClass", "������������Ϣ", WS_SYSMENU | WS_MINIMIZEBOX, windowX, windowY, windowWidth, windowHeight, NULL, NULL, GetModuleHandle(NULL), NULL);
						ShowWindow(hwnd_LANchat, SW_SHOW);
						isLANchatWindowOpen = 1; // ��������Ϊ1����ʾ�����족�Ӵ����Ѵ�
					} else {
						MessageBox(hwnd, "�����Ѵ��ڣ�=-=", "��ʾ=-=", MB_OK | MB_ICONASTERISK);
						// �ö���ȡ���ö����������û�
						SetWindowPos(hwnd_LANchat, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
						SetWindowPos(hwnd_LANchat, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
					}
					break;
				}

				case 4: { // ʵ�ù�����
					if (isiToolsWindowOpen == 0) { // ����Ƿ��Ѿ����ˡ������䡱�Ӵ���
						/* ʹ����ʼ�ճ�������Ļ�м� */
						// ��ȡ��Ļ��Ⱥ͸߶�
						int screenWidth = GetSystemMetrics(SM_CXSCREEN);
						int screenHeight = GetSystemMetrics(SM_CYSCREEN);
						// ���㴰�����Ͻ�����
						int windowWidth = 400; // ���ڿ��
						int windowHeight = 300; // ���ڸ߶�
						int windowX = (screenWidth - windowWidth) / 2;
						int windowY = (screenHeight - windowHeight) / 2;

						// ��ť�����ʱ������ ʵ�ù����� �Ӵ���
						hwnd_iTools = CreateWindowEx(WS_EX_CLIENTEDGE, "iToolsWindowClass", "ʵ�ù�����", WS_SYSMENU | WS_MINIMIZEBOX, windowX, windowY, windowWidth, windowHeight, NULL, NULL, GetModuleHandle(NULL), NULL);
						ShowWindow(hwnd_iTools, SW_SHOW);
						isiToolsWindowOpen = 1; // ��������Ϊ1����ʾ�������䡱�Ӵ����Ѵ�
					} else {
						MessageBox(hwnd, "�����Ѵ��ڣ�=-=", "��ʾ=-=", MB_OK | MB_ICONASTERISK);
						// �ö���ȡ���ö����������û�
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
					// ���ء����족����
					ShowWindow(hwnd_LANchat, SW_SHOW);
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

		/* ������ť */
		case WM_CREATE: { // ������ť
			HWND htext = CreateWindow("Static", "���ոյ���˹رհ�ť����������һ���Ĳ����ǣ�", WS_CHILD | WS_VISIBLE, 0, 0, 230, 40, hwnd, NULL, hInst, NULL);
			HWND hHideToTrayCheckbox = CreateWindow("button", "���ص�������", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 55, 40, 120, 20, hwnd, (HMENU)1, NULL, NULL);
			HWND hExitProgramCheckbox = CreateWindow("button", "�˳�����", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 55, 60, 120, 20, hwnd, (HMENU)2, NULL, NULL);
			HWND hNoReminderCheckbox = CreateWindow("button", "��������,�´��˳�ʱ�Զ�ѡ��", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 5, 80, 220, 20, hwnd, (HMENU)3, NULL, NULL);
			HWND determinebutton = CreateWindow("button", "ȷ��", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 20, 108, 80, 35, hwnd, (HMENU)4, hInst, NULL);
			HWND cancellationbutton = CreateWindow("button", "ȡ��", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 130, 108, 80, 35, hwnd, (HMENU)5, hInst, NULL);
			// ���� ���ص������� ��ѡ���ѡ��״̬Ϊѡ��
			SendMessage(hHideToTrayCheckbox, BM_SETCHECK, BST_CHECKED, 0);
			break;
		}

		case WM_COMMAND: { // ����ť����¼�
			switch (wParam) {

				case 3 : { // ��������
					HWND hNoReminderCheckbox = GetDlgItem(hwnd, 3); // ��ȡ ��������... ��ѡ��ľ��
					if (SendMessage(hNoReminderCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �ж� ��������... ��ѡ���Ƿ񱻹�ѡ
						// ��ѡ
						MessageBox(hwnd_exit, "�´��˳�ʱ���Զ�ѡ���粻���Զ�ѡ����������и���", "��ʾOwO", MB_OK | MB_ICONINFORMATION);
					}
					break;
				}

				case 4 : { //ȷ��
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
						// ���ء����족����
						ShowWindow(hwnd_LANchat, SW_HIDE);
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
						lstrcpy(nid.szInfo, TEXT("������Ŷ��qwq")); // ������Ϣ����
						Shell_NotifyIcon(NIM_MODIFY, &nid); // ������Ϣ
					} else if (SendMessage(hExitProgramCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) {
						// �û�ѡ���ˡ��˳�����
						// ���ٴ���
						DestroyWindow(hwnd);
						Shell_NotifyIcon(NIM_DELETE, &nid); // �Ƴ�����ͼ��
						PostQuitMessage(0); // ���û��������˳�����ʱ�����ڽ�һֱ����Ϊʹ��״̬
					}
					break;
				}

				case 5 : { //ȡ��
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

		/* ������ť */
		case WM_CREATE: { // ������ť
			HWND Nevernotifytext = CreateWindow("BUTTON", "��������", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 5, 5, 280, 80, hwnd, NULL, NULL, NULL);
			HWND Nevernotifyresetting = CreateWindow("button", "����", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 115, 30, 60, 30, hwnd, (HMENU)1, NULL, NULL);
			HWND testtext = CreateWindow("BUTTON", "�����͹���", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 5, 85, 280, 100, hwnd, NULL, NULL, NULL);
			A_strongTop = CreateWindow("button", "��ǿ�ö�", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 15, 105, 80, 15, hwnd, (HMENU)2, hInst, NULL);

			// ����ȫ�ֱ�����ֵ���ø�ѡ��Ĺ�ѡ״̬
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
				// ���� ���� ��ť״̬Ϊ����
				EnableWindow(Nevernotifyresetting, FALSE);
			}
			break;
		}

		case WM_COMMAND: { // ����ť����¼�
			switch (wParam) {

				case 1 : { // ����
					//����ļ�����
					std::ofstream file("data.json", std::ios::trunc);
					file.close();
					// д��
					std::ofstream newFile("data.json");
					newFile << "*���ļ��洢��ݲ˵��������������";
					newFile.close();
					// ���� ���� ��ť״̬Ϊ����
					HWND Nevernotifyresetting = GetDlgItem(hwnd, 1); // ��ȡ ���� ��ť�ľ��
					EnableWindow(Nevernotifyresetting, FALSE);
					MessageBox(hwnd, "����ɲ�����OwO", "�����ã�OwO", MB_OK | MB_ICONINFORMATION);
					break;
				}

				case 2 : {
					// ��ȡ��ѡ��Ĺ�ѡ״̬
					int checked = SendMessage(A_strongTop, BM_GETCHECK, 0, 0);

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

		/* ������ť */
		case WM_CREATE: { // ������ť
			HWND usingopen = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_DROPDOWNLIST, 165, 5, 160, 200, hwnd, (HMENU)0, NULL, NULL);
			SendMessage(usingopen, CB_ADDSTRING, 0, (LPARAM) "ʹ��Ĭ���������");
			SendMessage(usingopen, CB_ADDSTRING, 0, (LPARAM) "ʹ�ùȸ��������");
			SendMessage(usingopen, CB_SETCURSEL, 0, 0);

			HWND searchEngineGroupBox = CreateWindow("BUTTON", "��������", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 4, 30, 250, 100, hwnd, NULL, NULL, NULL);
			HWND baiduSearchbutton = CreateWindow("button", "�ٶ�����", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 15, 50, 70, 30, hwnd, (HMENU)11, NULL, NULL);
			HWND sogouSearchbutton = CreateWindow("button", "�ѹ�����", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 95, 50, 70, 30, hwnd, (HMENU)12, NULL, NULL);
			HWND bingSearchbutton = CreateWindow("button", "��Ӧ����", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 175, 50, 70, 30, hwnd, (HMENU)13, NULL, NULL);
			HWND sosoSearchbutton = CreateWindow("button", "��������", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 15, 90, 70, 30, hwnd, (HMENU)14, NULL, NULL);
			HWND sllSearchbutton = CreateWindow("button", "360����", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 95, 90, 70, 30, hwnd, (HMENU)15, NULL, NULL);

			HWND programmingGroupBox = CreateWindow("BUTTON", "���/ѧϰ", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 256, 30, 250, 100, hwnd, NULL, NULL, NULL);
			HWND programmingCatbutton = CreateWindow("button", "���è����", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 265, 50, 80, 30, hwnd, (HMENU)21, NULL, NULL);
			HWND Dingdingbutton = CreateWindow("button", "��Ѷ�۶�", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 355, 50, 70, 30, hwnd, (HMENU)22, NULL, NULL);
			HWND Giteebutton = CreateWindow("button", "Gitee", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 435, 50, 65, 30, hwnd, (HMENU)23, NULL, NULL);
			HWND CSDNbutton = CreateWindow("button", "CSDN", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 265, 90, 65, 30, hwnd, (HMENU)24, NULL, NULL);

			HWND videoGroupBox = CreateWindow("BUTTON", "��Ƶƽ̨", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 4, 130, 250, 100, hwnd, NULL, NULL, NULL);
			HWND bilibilibutton = CreateWindow("button", "��������", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 15, 150, 70, 30, hwnd, (HMENU)31, NULL, NULL);
			HWND douyinbutton = CreateWindow("button", "����", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 95, 150, 70, 30, hwnd, (HMENU)32, NULL, NULL);
			HWND xiguabutton = CreateWindow("button", "������Ƶ", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 175, 150, 70, 30, hwnd, (HMENU)33, NULL, NULL);
			HWND kuaishoubutton = CreateWindow("button", "����", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 15, 190, 70, 30, hwnd, (HMENU)34, NULL, NULL);

			HWND geshizhuanhGroupBox = CreateWindow("BUTTON", "��ʽת��", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 256, 130, 250, 100, hwnd, NULL, NULL, NULL);
			HWND convertiobutton = CreateWindow("button", "Convertio", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 265, 150, 80, 30, hwnd, (HMENU)41, NULL, NULL);
			HWND xunjiebutton = CreateWindow("button", "Ѹ����Ƶ", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 355, 150, 70, 30, hwnd, (HMENU)42, NULL, NULL);

			HWND ziyuanGroupBox = CreateWindow("BUTTON", "��Դ/�ز�", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 4, 230, 250, 100, hwnd, NULL, NULL, NULL);
			HWND aigeibutton = CreateWindow("button", "������", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 15, 250, 70, 30, hwnd, (HMENU)51, NULL, NULL);
			HWND pickfreebutton = CreateWindow("button", "��Դ����", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 95, 250, 70, 30, hwnd, (HMENU)52, NULL, NULL);
			HWND zitibutton = CreateWindow("button", "��������", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 175, 250, 70, 30, hwnd, (HMENU)53, NULL, NULL);

			HWND yuleGroupBox = CreateWindow("BUTTON", "����", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 256, 230, 250, 100, hwnd, NULL, NULL, NULL);
			HWND Factsbutton = CreateWindow("button", "ȫ��ʵ��", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 265, 250, 70, 30, hwnd, (HMENU)61, NULL, NULL);
			HWND chishabutton = CreateWindow("button", "�����ɶ", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 345, 250, 70, 30, hwnd, (HMENU)62, NULL, NULL);
			HWND mcbutton = CreateWindow("button", "����MC", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 425, 250, 70, 30, hwnd, (HMENU)63, NULL, NULL);
			HWND gcfzbutton = CreateWindow("button", "��������", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 265, 290, 70, 30, hwnd, (HMENU)64, NULL, NULL);
			break;
		}

		case WM_COMMAND: { // ����ť����¼�
			switch (wParam) {

				case 11 : { // �ٶ�����
					HWND Usingopen = GetDlgItem(hwnd, 0); // ��ȡ����ѡ���ľ��
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // ��ȡѡ�������
					if (selectedIndex == 0) {
						// ѡ��ʹ��Ĭ���������
						const char* url = "https://www.baidu.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// ѡ��ʹ�ùȸ��������
						const char* url = "https://www.baidu.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 12 : { // �ѹ�����
					HWND Usingopen = GetDlgItem(hwnd, 0); // ��ȡ����ѡ���ľ��
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // ��ȡѡ�������
					if (selectedIndex == 0) {
						// ѡ��ʹ��Ĭ���������
						const char* url = "https://www.sogou.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// ѡ��ʹ�ùȸ��������
						const char* url = "https://www.sogou.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 13 : { // ��Ӧ����
					HWND Usingopen = GetDlgItem(hwnd, 0); // ��ȡ����ѡ���ľ��
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // ��ȡѡ�������
					if (selectedIndex == 0) {
						// ѡ��ʹ��Ĭ���������
						const char* url = "https://cn.bing.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// ѡ��ʹ�ùȸ��������
						const char* url = "https://cn.bing.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 14 : { // ��������
					HWND Usingopen = GetDlgItem(hwnd, 0); // ��ȡ����ѡ���ľ��
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // ��ȡѡ�������
					if (selectedIndex == 0) {
						// ѡ��ʹ��Ĭ���������
						const char* url = "https://www.soso.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// ѡ��ʹ�ùȸ��������
						const char* url = "https://www.soso.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}



				case 15 : { // 360����
					HWND Usingopen = GetDlgItem(hwnd, 0); // ��ȡ����ѡ���ľ��
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // ��ȡѡ�������
					if (selectedIndex == 0) {
						// ѡ��ʹ��Ĭ���������
						const char* url = "https://www.so.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// ѡ��ʹ�ùȸ��������
						const char* url = "https://www.so.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 21 : { // ���è����
					HWND Usingopen = GetDlgItem(hwnd, 0); // ��ȡ����ѡ���ľ��
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // ��ȡѡ�������
					if (selectedIndex == 0) {
						// ѡ��ʹ��Ĭ���������
						const char* url = "https://shequ.codemao.cn/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// ѡ��ʹ�ùȸ��������
						const char* url = "https://shequ.codemao.cn/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 22 : { // ��Ѷ�۶�
					HWND Usingopen = GetDlgItem(hwnd, 0); // ��ȡ����ѡ���ľ��
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // ��ȡѡ�������
					if (selectedIndex == 0) {
						// ѡ��ʹ��Ĭ���������
						const char* url = "https://coding.qq.com/home/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// ѡ��ʹ�ùȸ��������
						const char* url = "https://coding.qq.com/home/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 23 : { // Gitee
					HWND Usingopen = GetDlgItem(hwnd, 0); // ��ȡ����ѡ���ľ��
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // ��ȡѡ�������
					if (selectedIndex == 0) {
						// ѡ��ʹ��Ĭ���������
						const char* url = "https://gitee.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// ѡ��ʹ�ùȸ��������
						const char* url = "https://gitee.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 24 : { // CSDN
					HWND Usingopen = GetDlgItem(hwnd, 0); // ��ȡ����ѡ���ľ��
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // ��ȡѡ�������
					if (selectedIndex == 0) {
						// ѡ��ʹ��Ĭ���������
						const char* url = "https://www.csdn.net/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// ѡ��ʹ�ùȸ��������
						const char* url = "https://www.csdn.net/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 31 : { // bilibili
					HWND Usingopen = GetDlgItem(hwnd, 0); // ��ȡ����ѡ���ľ��
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // ��ȡѡ�������
					if (selectedIndex == 0) {
						// ѡ��ʹ��Ĭ���������
						const char* url = "https://www.bilibili.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// ѡ��ʹ�ùȸ��������
						const char* url = "https://www.bilibili.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 32 : { // ����
					HWND Usingopen = GetDlgItem(hwnd, 0); // ��ȡ����ѡ���ľ��
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // ��ȡѡ�������
					if (selectedIndex == 0) {
						// ѡ��ʹ��Ĭ���������
						const char* url = "https://www.douyin.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// ѡ��ʹ�ùȸ��������
						const char* url = "https://www.douyin.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 33 : { // ������Ƶ
					HWND Usingopen = GetDlgItem(hwnd, 0); // ��ȡ����ѡ���ľ��
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // ��ȡѡ�������
					if (selectedIndex == 0) {
						// ѡ��ʹ��Ĭ���������
						const char* url = "https://www.ixigua.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// ѡ��ʹ�ùȸ��������
						const char* url = "https://www.ixigua.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 34 : { // ����
					HWND Usingopen = GetDlgItem(hwnd, 0); // ��ȡ����ѡ���ľ��
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // ��ȡѡ�������
					if (selectedIndex == 0) {
						// ѡ��ʹ��Ĭ���������
						const char* url = "https://www.kuaishou.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// ѡ��ʹ�ùȸ��������
						const char* url = "https://www.kuaishou.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 41 : { // Convertio
					HWND Usingopen = GetDlgItem(hwnd, 0); // ��ȡ����ѡ���ľ��
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // ��ȡѡ�������
					if (selectedIndex == 0) {
						// ѡ��ʹ��Ĭ���������
						const char* url = "https://convertio.co/zh/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// ѡ��ʹ�ùȸ��������
						const char* url = "https://convertio.co/zh/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 42 : { // Ѹ����Ƶ
					HWND Usingopen = GetDlgItem(hwnd, 0); // ��ȡ����ѡ���ľ��
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // ��ȡѡ�������
					if (selectedIndex == 0) {
						// ѡ��ʹ��Ĭ���������
						const char* url = "https://app.xunjieshipin.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// ѡ��ʹ�ùȸ��������
						const char* url = "https://app.xunjieshipin.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 51 : { // ������
					HWND Usingopen = GetDlgItem(hwnd, 0); // ��ȡ����ѡ���ľ��
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // ��ȡѡ�������
					if (selectedIndex == 0) {
						// ѡ��ʹ��Ĭ���������
						const char* url = "https://www.aigei.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// ѡ��ʹ�ùȸ��������
						const char* url = "https://www.aigei.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 52 : { // ��Դ����
					HWND Usingopen = GetDlgItem(hwnd, 0); // ��ȡ����ѡ���ľ��
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // ��ȡѡ�������
					if (selectedIndex == 0) {
						// ѡ��ʹ��Ĭ���������
						const char* url = "http://www.pickfree.cn/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// ѡ��ʹ�ùȸ��������
						const char* url = "http://www.pickfree.cn/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 53 : { // ��������
					HWND Usingopen = GetDlgItem(hwnd, 0); // ��ȡ����ѡ���ľ��
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // ��ȡѡ�������
					if (selectedIndex == 0) {
						// ѡ��ʹ��Ĭ���������
						const char* url = "https://font.chinaz.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// ѡ��ʹ�ùȸ��������
						const char* url = "https://font.chinaz.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 61 : { // ȫ��ʵ��
					HWND Usingopen = GetDlgItem(hwnd, 0); // ��ȡ����ѡ���ľ��
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // ��ȡѡ�������
					if (selectedIndex == 0) {
						// ѡ��ʹ��Ĭ���������
						const char* url = "https://www.skylinewebcams.com/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// ѡ��ʹ�ùȸ��������
						const char* url = "https://www.skylinewebcams.com/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 62 : { // �����ɶ
					HWND Usingopen = GetDlgItem(hwnd, 0); // ��ȡ����ѡ���ľ��
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // ��ȡѡ�������
					if (selectedIndex == 0) {
						// ѡ��ʹ��Ĭ���������
						const char* url = "http://guozhivip.com/eat/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// ѡ��ʹ�ùȸ��������
						const char* url = "http://guozhivip.com/eat/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 63 : { // ����
					HWND Usingopen = GetDlgItem(hwnd, 0); // ��ȡ����ѡ���ľ��
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // ��ȡѡ�������
					if (selectedIndex == 0) {
						// ѡ��ʹ��Ĭ���������
						const char* url = "https://classic.minecraft.net/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// ѡ��ʹ�ùȸ��������
						const char* url = "https://classic.minecraft.net/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
					}
					break;
				}

				case 64 : { // ��������
					HWND Usingopen = GetDlgItem(hwnd, 0); // ��ȡ����ѡ���ľ��
					int selectedIndex = SendMessage(Usingopen, CB_GETCURSEL, 0, 0); // ��ȡѡ�������
					if (selectedIndex == 0) {
						// ѡ��ʹ��Ĭ���������
						const char* url = "https://likexia.gitee.io/idle-factory/";
						ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					} else if (selectedIndex == 1) {
						// ѡ��ʹ�ùȸ��������
						const char* url = "https://likexia.gitee.io/idle-factory/";
						ShellExecute(NULL, "open", "chrome.exe", url, NULL, SW_SHOWNORMAL);
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
				int result = MessageBox(hwnd, "���ڵ���ʱ���˳�����ʧ���ȣ�ȷ���˳���\n�����������ڵ���˳�ѡ�����ص�������", "��ʾ=-=", MB_OKCANCEL | MB_ICONQUESTION);
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
			break;
		}

		/* ������ť */
		case WM_CREATE: { // ������ť
			usernameLabel = CreateWindow("STATIC", "�û�����", WS_VISIBLE | WS_CHILD, 90, 20, 80, 20, hwnd, NULL, NULL, NULL);
			usernameEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 160, 20, 150, 26, hwnd, NULL, NULL, NULL); // ʹ��WS_EX_CLIENTEDGE��ʽʹ�������Ĭ��
			passwordLabel = CreateWindow("STATIC", "���룺", WS_VISIBLE | WS_CHILD, 90, 60, 80, 20, hwnd, NULL, NULL, NULL);
			passwordEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD | ES_AUTOHSCROLL, 160, 60, 150, 26, hwnd, NULL, NULL, NULL); // ������ES_AUTOHSCROLL�Ա������������
			showPasswordCheckbox = CreateWindow("BUTTON", "��ʾ����", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 315, 60, 80, 20, hwnd, (HMENU) 0, NULL, NULL);
			ipLabel = CreateWindow("STATIC", "IP��ַ��", WS_VISIBLE | WS_CHILD, 90, 100, 80, 20, hwnd, NULL, NULL, NULL);
			ipComboBox = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN, 160, 100, 150, 200, hwnd, NULL, NULL, NULL);
			SendMessage(ipComboBox, CB_ADDSTRING, 0, (LPARAM) "192.168.60.250");
			SendMessage(ipComboBox, CB_ADDSTRING, 0, (LPARAM) "192.168.60.199");
			SendMessage(ipComboBox, CB_ADDSTRING, 0, (LPARAM) "192.168.0.104");
			SendMessage(ipComboBox, CB_SETCURSEL, 0, 0);
			sharedFolderLabel = CreateWindow("STATIC", "����Ŀ¼��", WS_VISIBLE | WS_CHILD, 90, 140, 75, 20, hwnd, NULL, NULL, NULL);
			sharedFolderEdit = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN, 160, 140, 150, 200, hwnd, NULL, NULL, NULL);
			SendMessage(sharedFolderEdit, CB_ADDSTRING, 0, (LPARAM) "�������");
			SendMessage(sharedFolderEdit, CB_ADDSTRING, 0, (LPARAM) "home");
			SendMessage(sharedFolderEdit, CB_SETCURSEL, 0, 0);
			driveLabel = CreateWindow("STATIC", "�̷���", WS_VISIBLE | WS_CHILD, 90, 180, 80, 20, hwnd, NULL, NULL, NULL);
			driveComboBox = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWNLIST | WS_VSCROLL, 160, 180, 150, 200, hwnd, NULL, NULL, NULL);
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
			connectButton = CreateWindow("BUTTON", "ӳ��", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 90, 220, 100, 30, hwnd, (HMENU) 1, NULL, NULL);
			disconnectButton = CreateWindow("BUTTON", "�Ͽ�ȫ��", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 210, 220, 100, 30, hwnd, (HMENU) 2, NULL, NULL);
			countdownCheckbox = CreateWindow("BUTTON", "�״�ӳ��ɹ���������ʱ", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 90, 260, 200, 20, hwnd, (HMENU)3, NULL, NULL);
			countdownLabel = CreateWindow("STATIC", "��Ͽ����л��У�", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 90, 290, 180, 20, hwnd, NULL, NULL, NULL);
			countdownEdit = CreateWindow("EDIT", "40", WS_VISIBLE | WS_CHILD | WS_BORDER, 270, 290, 40, 20, hwnd, NULL, NULL, NULL);
			countdownUnitComboBox = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_DROPDOWNLIST, 310, 289, 40, 200, hwnd, NULL, NULL, NULL); // ����ʹ��CBS_DROPDOWNLIST��ʽʹ�ؼ��޷��༭
			SendMessage(countdownUnitComboBox, CB_ADDSTRING, 0, (LPARAM) "��");
			SendMessage(countdownUnitComboBox, CB_ADDSTRING, 0, (LPARAM) "��");
			SendMessage(countdownUnitComboBox, CB_SETCURSEL, 0, 0);
			detailedButton = CreateWindow("BUTTON", "��ϸ", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 280, 70, 30, hwnd, (HMENU) 4, NULL, NULL);
			jieshudaojishicheckbox = CreateWindow("BUTTON", "�Ͽ��󲻽�������ʱ", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 90, 310, 160, 20, hwnd, NULL, NULL, NULL);
			// ���õ���ʱ
			EnableWindow(countdownLabel, FALSE);
			EnableWindow(countdownEdit, FALSE);
			EnableWindow(countdownUnitComboBox, FALSE);
			break;
		}

		/* ��ʱ�� */
		case WM_TIMER: {
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
					string command = "net use * /del /Y";
					system(command.c_str());
					//ȡ�����õ���ʱ
					EnableWindow(countdownEdit, TRUE);
					EnableWindow(countdownUnitComboBox, TRUE);
					// ɾ�ɵ���ʱ
					DestroyWindow(countdownLabel);
					// ���µ���ʱ
					countdownLabel = CreateWindow("STATIC", "��Ͽ����л��У�", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 90, 290, 180, 20, hwnd, NULL, NULL, NULL);
					// ��ʾһ����Ϣ�������������ѶϿ�ӳ��
					MessageBox(hwnd, "�ѶϿ�ȫ��ӳ�����������", "�Ͽ��ɹ�����OwO", MB_OK | MB_ICONINFORMATION);
				}
			}
			break;
		}

		case WM_COMMAND: { // ����ť����¼�
			switch (wParam) {

				case 0: { // ��ʾ����
					LONG_PTR style = GetWindowLongPtr(passwordEdit, GWL_STYLE); // ��ȡ��������ʽ
					if (style & ES_PASSWORD) { // �ж��������ʽ���Ƿ����ES_PASSWORD��־
						/* �����������������Ϊ�����ַ�������ES_PASSWORD�� */
						// ��ȡ����������������
						char passwordBuf[256];
						GetWindowText(passwordEdit, passwordBuf, sizeof(passwordBuf));
						// ɾ���ɵ������ַ����������
						DestroyWindow(passwordEdit);
						// �����µ��������������
						passwordEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 160, 60, 150, 26, hwnd, NULL, NULL, NULL);
						// �����ݸ��Ƶ������������
						SetWindowText(passwordEdit, passwordBuf);
					} else {
						/* �����������������Ϊ���ģ�������ES_PASSWORD�� */
						// ��ȡ����������������
						char passwordBuf[256];
						GetWindowText(passwordEdit, passwordBuf, sizeof(passwordBuf));
						// ɾ���ɵ��������������
						DestroyWindow(passwordEdit);
						// �����µ������ַ����������
						passwordEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD | ES_AUTOHSCROLL, 160, 60, 150, 26, hwnd, NULL, NULL, NULL);
						// �����ݸ��Ƶ������������
						SetWindowText(passwordEdit, passwordBuf);
					}
					break; // �˳�ѭ�������û�������������һ�еĴ��룬��������Ӧ��������
				}

				case 1: {// ӳ��Զ�̹����ļ���
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
							// ���ӳɹ�
							SetWindowText(passwordEdit, ""); // �������������ı�����Ϊ���ַ���
							MessageBox(hwnd, "û���κ����⣡OwO", "�ɹ�����OwO", MB_OK | MB_ICONINFORMATION);
						} else {
							// ����ʧ��
							SetWindowText(passwordEdit, ""); // �������������ı�����Ϊ���ַ���
							MessageBox(hwnd, "���˵�С���⣡X_X", "�����ˣ�X_X", MB_OK | MB_ICONERROR);
						}
					} else if (username.empty()) {
						// û���û������л���û���û��������ӷ�ʽ
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
							// ���ӳɹ�
							SetWindowText(passwordEdit, ""); // �������������ı�����Ϊ���ַ���
							MessageBox(hwnd, "û���κ����⣡OwO", "�ɹ�����OwO", MB_OK | MB_ICONINFORMATION);
						} else {
							// ����ʧ��
							SetWindowText(passwordEdit, ""); // �������������ı�����Ϊ���ַ���
							MessageBox(hwnd, "���˵�С���⣡X_X", "�����ˣ�X_X", MB_OK | MB_ICONERROR);
						}
					} else if (password.empty()) {
						// û�����룬�л���û����������ӷ�ʽ
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
							// ���ӳɹ�
							SetWindowText(passwordEdit, ""); // �������������ı�����Ϊ���ַ���
							MessageBox(hwnd, "û���κ����⣡OwO", "�ɹ�����OwO", MB_OK | MB_ICONINFORMATION);
						} else {
							// ����ʧ��
							SetWindowText(passwordEdit, ""); // �������������ı�����Ϊ���ַ���
							MessageBox(hwnd, "���˵�С���⣡X_X", "�����ˣ�X_X", MB_OK | MB_ICONERROR);
						}
					} else {
						// ���û��������룬�л������û�������������ӷ�ʽ
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
							// ���ӳɹ�
							SetWindowText(passwordEdit, ""); // �������������ı�����Ϊ���ַ���
							MessageBox(hwnd, "û���κ����⣡OwO", "�ɹ�����OwO", MB_OK | MB_ICONINFORMATION);
						} else {
							// ����ʧ��
							SetWindowText(passwordEdit, ""); // �������������ı�����Ϊ���ַ���
							MessageBox(hwnd, "���˵�С���⣡X_X", "�����ˣ�X_X", MB_OK | MB_ICONERROR);
						}
					}
					break;
				}

				case 2: { // �Ͽ�ȫ��
					// �Ͽ�ȫ��Զ�̹����ļ���
					string command = "net use * /del /Y";
					system(command.c_str());
					if (SendMessage(jieshudaojishicheckbox, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� �Ͽ��󲻽�������ʱ ��ѡ���Ƿ�δ����ѡ
						// δ��ѡ
						// ���õ���ʱ����
						countdownMinutes = 0;
						countdownDuration = 0;
						// ɾ�ɵ���ʱ
						DestroyWindow(countdownLabel);
						// ���µ���ʱ
						countdownLabel = CreateWindow("STATIC", "��Ͽ����л��У�", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 90, 290, 180, 20, hwnd, NULL, NULL, NULL);
					}
					if (SendMessage(countdownCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �жϸ�ѡ���Ƿ񱻹�ѡ
						// ��ѡ
						//ȡ�����õ���ʱ
						EnableWindow(countdownLabel, TRUE);
						EnableWindow(countdownEdit, TRUE);
						EnableWindow(countdownUnitComboBox, TRUE);
					} else {
						// ȡ����ѡ
						// ���õ���ʱ
						EnableWindow(countdownLabel, FALSE);
						EnableWindow(countdownEdit, FALSE);
						EnableWindow(countdownUnitComboBox, FALSE);
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
						countdownLabel = CreateWindow("STATIC", "��Ͽ����л��У�", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 90, 290, 180, 20, hwnd, NULL, NULL, NULL);
						// ���õ���ʱ
						EnableWindow(countdownLabel, FALSE);
						EnableWindow(countdownEdit, FALSE);
						EnableWindow(countdownUnitComboBox, FALSE);
					}
					break;
				}

				case 4: { // ��ϸ
					MessageBox(hwnd, "Q&A�ʴ�Question&Answer����\nQ������ʲô����ʲô�ã�\nA�����ǿ��Խ�NAS����洢��ӳ�䵽���������������������������൱��Ӳ�̣��ĸ�������������ʹ������Я��ʹ������Զ������洢���豸\nQ��NAS��ʲô��\nA��NAS��Network Attached Storage�����總���洢���������˵���������������ϣ��߱����ϴ洢���ܵ�װ�ã����Ҳ��Ϊ������洢����������һ��ר�����ݴ洢������������Դ�� �ٶȰٿ�\n\n�߼���\n���������ڹ�ѡ\"�Զ��򿪴洢�豸\"��ѡ���ӳ�䣬ӳ��ɹ����Զ���ӳ����ļ���\n\n������֧�֣�\nʹ�õ���ӳ����룺net use [�̷�]: \\[IP]\[�����ļ���]��ʹ�õ��ĶϿ����룺net use * /del /Y�����ϴ����Ϊ�����У�������ʾ�������룬������������ֱ��ʹ�á������ṩ�Ĵ��벢��Ϊ�������õ������д��룬���������������ѯ����ӳ�丨������֧�ֶ��ֲ�ͬ��ӳ���߼��жϣ�������ǹ���Ա����������˻�ֱ�ӽ���ӳ�䡣֧���Զ��嵹��ʱʱ�估��λ����������ڵ���ʱʱ����˳��������ѡ�֧�����ص���̨��ϵͳ���̣��������е���ʱ���������������ڵ���˳�-ѡ�����ص�������-Ȼ��ȷ������������˲������ѿ������������ã�\n\n�������⼰����취��\n��ʹ�õĹ����п��ܻ�������������Ӧ�������������������������ĵȴ����룬��ȴ�ʱ�����������ǿ�ƹرճ����������Ҽ�-����������������-�л���Ӧ�ó���ѡ��ҳ-ѡ��˳���-�����������\n\n��󣬸�л����ʹ�ã�", "��ϸ^u^", MB_OK );
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

/* �˺������� ���������� �Ӵ��ڵ���Ϣ */
LRESULT CALLBACK LANChatWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	// �����Ӵ�����Ϣ
	switch (msg) {

			static HWND hdescription; // ˵���ı�����
			static HWND hIPAddressInput; // IP��ַ�������
			static HWND hMessageInput; // ��Ϣ�������
			static HWND hGenerateButton; // ���ɰ�ť���
			static HWND hResultText; // ����ı�����
			static HWND hcmdButton; // ���ư�ť���
			static HWND hcopyButton; // ���ư�ť���

		// �����Ӵ��ڵĹر���Ϣ
		case WM_CLOSE: {
			// �ر��Ӵ���
			DestroyWindow(hwnd);
			break;
		}

		/* ������ʱ���������߳�ֹͣ */
		case WM_DESTROY: { // ����ʱ
			isLANchatWindowOpen = 0; // ��������Ϊ0����ʾ�����족�Ӵ����ѹر�
			break;
		}

		/* ������ť */
		case WM_CREATE: { // ������ť
			hdescription = CreateWindow("STATIC", "��Ϊ�����޷�ʵ�ִ˹���������Ҫ�����н����ɵ�����ճ���������к�س���Win+R�����к�����cmd�س���\n��IP��                             ������Ϣ��", WS_VISIBLE | WS_CHILD, 5, 5, 380, 50, hwnd, NULL, hInst, NULL);
			hIPAddressInput = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",  WS_CHILD | WS_VISIBLE | WS_BORDER, 45, 38, 118, 20, hwnd, NULL, hInst, NULL);
			hMessageInput = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL, 10, 65, 370, 60, hwnd, NULL, hInst, NULL);
			hGenerateButton = CreateWindow("BUTTON", "����", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 155, 130, 80, 30, hwnd, (HMENU)1, hInst, NULL);
			hResultText = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL, 10, 165, 370, 90, hwnd, NULL, hInst, NULL);
			hcmdButton = CreateWindow("BUTTON", "��������", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 55, 130, 80, 30, hwnd, (HMENU)2, hInst, NULL);
			hcopyButton = CreateWindow("BUTTON", "����", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 255, 130, 80, 30, hwnd, (HMENU)3, hInst, NULL);
			break;
		}

		case WM_COMMAND: { // ����ť����¼�
			switch (wParam) {

				case 1 : {
					// ��ȡIP��ַ�������ı�
					char ipAddress[256];
					GetWindowTextA(hIPAddressInput, ipAddress, sizeof(ipAddress));
					// ��ȡ��Ϣ�������ı�
					char message[256];
					GetWindowTextA(hMessageInput, message, sizeof(message));
					// ��IP��ַ����Ϣ���
					std::string result = "msg /SERVER:" + std::string(ipAddress) + " * " + std::string(message);
					// ���½���ı���
					SetWindowTextA(hResultText, result.c_str());
					break;
				}

				case 2 : {
					// ģ�ⰴ��Win+R�������д���
					keybd_event(VK_LWIN, 0, 0, 0);
					keybd_event('R', 0, 0, 0);
					keybd_event('R', 0, KEYEVENTF_KEYUP, 0);
					keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
					Sleep(500); // �ȴ����д��ڴ�

					// ģ������cmd������Enter��
					keybd_event('C', 0, 0, 0);
					keybd_event('C', 0, KEYEVENTF_KEYUP, 0);
					keybd_event('M', 0, 0, 0);
					keybd_event('M', 0, KEYEVENTF_KEYUP, 0);
					keybd_event('D', 0, 0, 0);
					keybd_event('D', 0, KEYEVENTF_KEYUP, 0);
					keybd_event(VK_RETURN, 0, 0, 0);
					keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0);
					keybd_event(VK_RETURN, 0, 0, 0);
					keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0);
					break;
				}

				case 3: {
					// ��ȡ�ı�����ı�
					char clipboardText[256];
					GetWindowTextA(hResultText, clipboardText, sizeof(clipboardText));
					if (OpenClipboard(NULL)) {
						EmptyClipboard();
						CloseClipboard();
						HGLOBAL hClipboardData = GlobalAlloc(GMEM_MOVEABLE, (strlen(clipboardText) + 1) * sizeof(char));
						if (hClipboardData != NULL) {
							char* pClipboardText = static_cast<char*>(GlobalLock(hClipboardData));
							if (pClipboardText != NULL) {
								strcpy_s(pClipboardText, strlen(clipboardText) + 1, clipboardText);
								GlobalUnlock(hClipboardData);
								if (OpenClipboard(NULL)) {
									SetClipboardData(CF_TEXT, hClipboardData);
									CloseClipboard();
									MessageBoxA(hwnd, "�ѽ��ı����Ƶ������壡", "���Ƴɹ�^_O", MB_ICONINFORMATION | MB_OK);
								}
							}
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

		/* ������ť */
		case WM_CREATE: { // ������ť
			HWND SystemToolsGroupBox = CreateWindow("BUTTON", "ϵͳ����", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 5, 5, 250, 200, hwnd, NULL, NULL, NULL);
			HWND restartButton = CreateWindow("BUTTON", "������Դ�������������", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 15, 30, 190, 30, hwnd, (HMENU)1, NULL, NULL);
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
	wc.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 0);
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

	/* ����ʱ����������Ƿ��Ѵ��ڣ���������� */
	// ����һ��������
	HANDLE hMutex = CreateMutex(NULL, TRUE, "MyAppMutex");
	// ��黥�����Ƿ��Ѿ�����
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		// ����������Ѿ����ڣ���ʾ�Ѿ���һ�������ڽ��������У�������ٵ�ǰʵ��
		CloseHandle(hMutex);
		// �ڳ����˳�ǰ�ͷŻ�����
		CloseHandle(hMutex);
		MessageBox(host_hwnd, "�˳�������������У���������û�У�����ϵͳ����", "��ʾO_O", MB_OK | MB_ICONASTERISK); // ��Ϊ���½����ڲ��ܷ�����Ϣ���ɴ�������ֻ�������û����в鿴
		return 0;
	}

	/* ʹ����ʼ�ճ�������Ļ�м� */
	// ��ȡ��Ļ��Ⱥ͸߶�
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	// ���㴰�����Ͻ�����
	int windowWidth = 380; // ���ڿ��
	int windowHeight = 280; // ���ڸ߶�
	int windowX = (screenWidth - windowWidth) / 2;
	int windowY = (screenHeight - windowHeight) / 2;

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

	// ע�� ���������� �Ӵ��ڵ���
	wc.lpfnWndProc	 = LANChatWndProc;
	wc.lpszClassName = "LANChatWindowClass";
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

	host_hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, "WindowClass", "��ݲ˵�", WS_VISIBLE | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, windowX, windowY, windowWidth, windowHeight, NULL, NULL, hInstance, NULL);

	if (host_hwnd == NULL) {
		MessageBox(NULL, "���ڴ���ʧ�ܣ�", "����", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// �����˵���
	HMENU hMenu = CreateMenu();
	// ���������á��˵���
	HMENU hSubMenu = CreatePopupMenu();
	AppendMenu(hSubMenu, MF_STRING, 32913, "��ϸ");
	AppendMenu(hSubMenu, MF_STRING, 32914, "��������");
	AppendMenu(hSubMenu, MF_STRING, 32912, "����");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSubMenu, "����");
	// ���˵���������������
	SetMenu(host_hwnd, hMenu);

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
		MessageBox(NULL, "����ͼ�����ʧ�ܣ�X_X", "����X_X", MB_ICONERROR | MB_OK);
	}
	strcpy(nid.szTip, "��ݲ˵�");
	// �������ͼ��
	Shell_NotifyIcon(NIM_ADD, &nid);

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
	} else { // ����ļ����Ƿ������*���ļ��洢��ݲ˵�������������ݡ�
		std::ifstream file("data.json");
		if (!file) {
			MessageBox(NULL, "�޷���data.json�ļ���", "����", MB_ICONEXCLAMATION | MB_OK);
			return 0;
		}
		// ��ȡdata.json�ļ����鿴�Ƿ��С�*���ļ��洢��ݲ˵�������������ݡ�
		std::string line;
		bool found = false;
		while (std::getline(file, line)) {
			if (line.find("*���ļ��洢��ݲ˵��������������") != std::string::npos) {
				found = true;
				break;
			}
		}

		file.close();
		// �����������ô�½�һ���µ�data.json�ļ���д���ı�
		if (!found) {
			std::ofstream newFile("data.json");
			newFile << "*���ļ��洢��ݲ˵��������������";
			newFile.close();
		}
	}

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
