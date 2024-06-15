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

// ����ȫ�ֱ���
const char childwindow[] = "Drive mapping"; // ����ӳ�丨������ �Ӵ��ڵ�ȫ�ֱ���
int isExitWindowOpen = 0; // ����ʹ���˳����Ӵ���ֻ�ܴ�һ��
int isSettingsWindowOpen = 0; // ����ʹ�����á��Ӵ���ֻ�ܴ�һ��
int isWebsiteWindowOpen = 0; // ����ʹ����վ���Ӵ���ֻ�ܴ�һ��
int isMappingWindowOpen = 0; // ����ʹ��ӳ�䡱�Ӵ���ֻ�ܴ�һ��
int isiToolsWindowOpen = 0; // ����ʹ�������䡱�Ӵ���ֻ�ܴ�һ��

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
HWND htext; // �ı�
HWND LockPassword; // �������� ��ѡ��
HWND Saveusername; // ӳ��ɹ��󱣴��û��� ��ѡ��
HWND notice; // ��֪ �ı�
HWND ForcedisconnectallButton; // ǿ�ƶϿ�ȫ��(������Դ������) ��ť
// �ؼ�ֵ
string username; // �˺�
string password; // ����
string ipAddress; // IP��ַ
string sharedFolder; // ����Ŀ¼
string drive; // �̷�
// ����ʱ����ʱ���ȫ�ֱ���
int countdownMinutes = 0; // ��
int countdownDuration = 0; // ��
// ��������
bool hasContentExecuted = false;
bool noContentExecuted = false;

/* �������д�������ĵط� */
HWND A_Top, A_AOU, A_text, A_text2, A_1, A_2, A_3, A_strongTop;
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
					int windowHeight = 175; // ���ڸ߶�
					int windowX = (screenWidth - windowWidth) / 2;
					int windowY = (screenHeight - windowHeight) / 2;
					hwnd_exit = CreateWindowEx(WS_EX_CLIENTEDGE, "ExitWindowClass", "�˳�ǰѯ��U_U", WS_SYSMENU, windowX, windowY, windowWidth, windowHeight, NULL, NULL, GetModuleHandle(NULL), NULL);
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
			A_Top = CreateWindow("button", "�ö��˳�������д���", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 48, 5, 170, 15, hwnd, (HMENU)00000, hInst, NULL);
			A_AOU = CreateWindow("button", "�Զ��������Ĵ洢�豸", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 40, 25, 185, 15, hwnd, (HMENU)857, hInst, NULL); // �Զ��������Ĵ洢�豸 = Auto Open Inserted USB
			A_text = CreateWindow("Static", "��ӭʹ�ã�", WS_CHILD | WS_VISIBLE, 0, 195, 370, 50, hwnd, NULL, hInst, NULL);
			A_text2 = CreateWindow("Static", "��ӭʹ�ã�", WS_CHILD | WS_VISIBLE, 0, 228, 370, 20, hwnd, NULL, hInst, NULL);
			A_1 = CreateWindow("button", "������վ��ݴ�", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 65, 45, 140, 45, hwnd, (HMENU)1, hInst, NULL);
			A_2 = CreateWindow("button", "����ӳ�丨������", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 65, 95, 140, 45, hwnd, (HMENU)2, hInst, NULL);
			A_3 = CreateWindow("button", "ʵ�ù�����", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 65, 145, 140, 45, hwnd, (HMENU)3, hInst, NULL);
			// ������ʱ��
			SetTimer(hwnd, 1, 500, NULL); // ��ʱ��ID: 1�����ʱ��: 500���루0.5�룩
			SetTimer(hwnd, 2, 10000, NULL); // ��ʱ��ID: 2�����ʱ��: 10000���루10�룩
			// ���ѡ����ı�
			std::string motivationalTexts[] = {
				"����û�а��ߵ�·��ÿһ������",
				"�����ؿ��գ�����������",
				"ʧ���ǳɳ��Ļ��ᣬ�¸����",
				"ÿһ�εĳ��Զ���һ�ֽ���",
				"��Ҫ����ʧ�ܣ����ǳɹ���һ����",
				"��ֲ�и�����ջ��ջ�ɹ�",
				"֪�㳣�֣�������",
				"�־���ô���ղ�ס��̫����",
				"�ɹ���Ҫ����Ŭ��������",
				"ֻ�����ܾ����Լ���δ��",
				"���ӿڳ������ӿ���",
				"�ɹ��Ĺؼ����ڼ�ֲ�и",
				"����Ī˵����ˮ����",
				"�¸������ս������ø�ǿ��",
				"ÿһ�ε�Ŭ������ֵ�õ�",
				"�ᵱ�������һ����ɽС",
				"��Ҫ���¸ı䣬���ǳɳ��Ļ���",
				"����˼������ῴ������Ŀ�����",
				"��һ�䣬Ϣһŭ����һ�ţ���һ��",
				"���Ŭ��һ����Ϊ������ر�"
			};
			std::srand(static_cast<unsigned int>(std::time(nullptr))); // �������������
			int randomIndex = std::rand() % (sizeof(motivationalTexts) / sizeof(motivationalTexts[0]));
			// �������ı��ϲ�Ϊһ���ַ���
			std::string usageStr = "��ӭʹ�ã��汾��2.1\n" + motivationalTexts[randomIndex];
			// ���»�ӭ�ı���ʾ���ı�
			SetWindowText(A_text, usageStr.c_str());
			break;
		}

		/* ��ʱ�� */
		case WM_TIMER: {
			if (wParam == 2) { // �жϼ�ʱ��ID
				// ���ѡ����ı�
				std::string motivationalTexts[] = {
					"����û�а��ߵ�·��ÿһ������",
					"�����ؿ��գ�����������",
					"ʧ���ǳɳ��Ļ��ᣬ�¸����",
					"ÿһ�εĳ��Զ���һ�ֽ���",
					"��Ҫ����ʧ�ܣ����ǳɹ���һ����",
					"��ֲ�и�����ջ��ջ�ɹ�",
					"֪�㳣�֣�������",
					"�־���ô���ղ�ס��̫����",
					"�ɹ���Ҫ����Ŭ��������",
					"ֻ�����ܾ����Լ���δ��",
					"���ӿڳ������ӿ���",
					"�ɹ��Ĺؼ����ڼ�ֲ�и",
					"����Ī˵����ˮ����",
					"�¸������ս������ø�ǿ��",
					"ÿһ�ε�Ŭ������ֵ�õ�",
					"�ᵱ�������һ����ɽС",
					"��Ҫ���¸ı䣬���ǳɳ��Ļ���",
					"����˼������ῴ������Ŀ�����",
					"��һ�䣬Ϣһŭ����һ�ţ���һ��",
					"���Ŭ��һ����Ϊ������ر�"
				};
				std::srand(static_cast<unsigned int>(std::time(nullptr))); // �������������
				int randomIndex = std::rand() % (sizeof(motivationalTexts) / sizeof(motivationalTexts[0]));

				// �������ı��ϲ�Ϊһ���ַ���
				std::string usageStr = "��ӭʹ�ã��汾��2.1\n" + motivationalTexts[randomIndex];

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

				// �������ı��ϲ�Ϊһ���ַ���
				std::string usageStr = "CPUռ�ã�" + std::to_string(static_cast<int>(cpuUsage)) + "%" + "     �ڴ�ռ�ã�" + std::to_string(static_cast<int>(memoryUsagePercentage)) + "%";

				// ���»�ӭ2�ı���ʾ���ı�
				SetWindowText(A_text2, usageStr.c_str());
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

				case 32912: { // ����
					if (isSettingsWindowOpen == 0) { // ����Ƿ��Ѿ����ˡ����á��Ӵ���
						/* ʹ����ʼ�ճ�������Ļ�м� */
						// ��ȡ��Ļ��Ⱥ͸߶�
						int screenWidth = GetSystemMetrics(SM_CXSCREEN);
						int screenHeight = GetSystemMetrics(SM_CYSCREEN);
						// ���㴰�����Ͻ�����
						int windowWidth = 300; // ���ڿ��
						int windowHeight = 325; // ���ڸ߶�
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
					MessageBox(hwnd, "�������⼰����취��\n��ʹ�õĹ����п��ܻ�������������Ӧ�������������������������ĵȴ����룬��ȴ�ʱ�����������ǿ�ƹرճ����������Ҽ�-����������������-�л���Ӧ�ó���ѡ��ҳ-ѡ��˳���-�����������\n\n���������������ͷ�����\n���Ƿǳ�Ը����������������ͷ�����Ӧ�õ���һ���汾�У����Ƕ��ڱȽ����ѶȵĹ��ܻῪ�������������½⡣���ǻᾡ�����ɴ󲿷����ô�����������һ�ӭ���ڳ���ʹ���з���BUG������©����ʱ�����������Ա�����һ���汾���޸��������ڵײ��BUG����ǣ����������ĵ�BUG����ͨ����Ҫ���õ�ʱ���������޸��������½⣡\n\n��ϵ�������ȡ�°汾��\n��ϵ�������QQȺ��938213028������Ⱥ�ڻ�ȡ���°汾������һ�汾�������ݻ���Ϣ������\n\n����Ŀ�����Դ���룺\n�˳�����С��èDev-C++������ʹ��C++���Կ�������������ݲ���Դ������ѧϰ��Ҫ���ǿ�Ϊ���ṩ���˼·\n\nʹ��˵��/����������\n���ǲ����ռ������κ���˽��Ϣ�������Ὣ��Ϣ�ϴ����ƶˡ������ʹ�ô˳���������κεĺ�������������˳е���\n\n���ǵ����Ĳ��ܻ��������������Ͷ����ǵ�֧�֣���󣬸�л����ʹ�ã����������ߣ�Ling", "��ϸ^u^", MB_OK );
					break;
				}

				case 32914: { // ��������
					MessageBox(hwnd, "����2.0��֮ǰ�汾����\n2.0�汾��֮ǰ�汾��������ʡ�ԡ�2.0�汾��2023��7��5����ʽ��ʼ��������2023��9��27����ɿ�����2126�д��룬��89515���ַ�\n����2.1�汾����\n2.1�汾��2023��9��28����ʽ��ʼ��������2023��10��12����ɿ�����3194�д��룬��142734���ַ�\n�������ݣ�1.�Ż��˲��ִ��ڵĲ��֡�2.�����������ڡ����á�����ӳ�丨�����ߡ�ʵ�ù������һЩ��Ϣ��3.��Ϊ������������Ϣ�����������Ծ�ɾ���ˡ�4.�޸�����֪��BUG\n��������������\n\n���ǵ����Ĳ��ܻ��������������Ͷ����ǵ�֧�֣���󣬸�л����ʹ�ã����������ߣ�Ling", "��������^u^", MB_OK );
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
						int windowWidth = 505; // ���ڿ��
						int windowHeight = 340; // ���ڸ߶�
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
			if (SendMessage(A_Top, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �ж� �ö� ��ѡ���Ƿ񱻹�ѡ
				// ��ѡ
				SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // �ö�
			}
			HWND htext = CreateWindow("Static", "���ոյ���˹رհ�ť����������һ���Ĳ����ǣ�", WS_CHILD | WS_VISIBLE, 3, 0, 230, 40, hwnd, NULL, hInst, NULL);
			HWND hHideToTrayCheckbox = CreateWindow("button", "���ص�������", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 55, 35, 120, 20, hwnd, (HMENU)1, NULL, NULL);
			HWND hExitProgramCheckbox = CreateWindow("button", "�˳�����", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 55, 55, 120, 20, hwnd, (HMENU)2, NULL, NULL);
			HWND hNoReminderCheckbox = CreateWindow("button", "��������,�´��˳�ʱ�Զ�ѡ��", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 5, 75, 220, 20, hwnd, (HMENU)3, NULL, NULL);
			HWND determinebutton = CreateWindow("button", "ȷ��", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 23, 100, 80, 35, hwnd, (HMENU)4, hInst, NULL);
			HWND cancellationbutton = CreateWindow("button", "ȡ��", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 127, 100, 80, 35, hwnd, (HMENU)5, hInst, NULL);
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
			if (SendMessage(A_Top, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �ж� �ö� ��ѡ���Ƿ񱻹�ѡ
				// ��ѡ
				SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // �ö�
			}
			HWND Nevernotifytext = CreateWindow("BUTTON", "����-��ݲ˵�", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 5, 5, 280, 180, hwnd, NULL, NULL, NULL);
			HWND Nevernotifyresetting = CreateWindow("button", "���ò�������", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 15, 30, 100, 30, hwnd, (HMENU)1, NULL, NULL);
			HWND testtext = CreateWindow("BUTTON", "ʵ���Թ���", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 5, 185, 280, 100, hwnd, NULL, NULL, NULL);
			A_strongTop = CreateWindow("button", "��ǿ�ö�", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 15, 205, 80, 15, hwnd, (HMENU)2, hInst, NULL);

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
			if (SendMessage(A_Top, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �ж� �ö� ��ѡ���Ƿ񱻹�ѡ
				// ��ѡ
				SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // �ö�
			}
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
			hasContentExecuted = false; // ���ñ�־����
			noContentExecuted = false; // ���ñ�־����
			break;
		}

		/* ������ť */
		case WM_CREATE: { // ������ť
			if (SendMessage(A_Top, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �ж� �ö� ��ѡ���Ƿ񱻹�ѡ
				// ��ѡ
				SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // �ö�
			}
			hGroupBoxMapping = CreateWindow("BUTTON", "ӳ��/�Ͽ�", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 250, 5, 240, 300, hwnd, NULL, NULL, NULL);
			usernameLabel = CreateWindow("STATIC", "�û�����", WS_VISIBLE | WS_CHILD, 260, 30, 80, 20, hwnd, NULL, NULL, NULL);
			usernameEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 330, 27, 150, 26, hwnd, NULL, NULL, NULL); // ʹ��WS_EX_CLIENTEDGE��ʽʹ�������Ĭ��
			passwordLabel = CreateWindow("STATIC", "���룺", WS_VISIBLE | WS_CHILD, 260, 60, 80, 20, hwnd, NULL, NULL, NULL);
			passwordEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD | ES_AUTOHSCROLL, 330, 57, 150, 26, hwnd, NULL, NULL, NULL); // ������ES_AUTOHSCROLL�Ա������������
			ipLabel = CreateWindow("STATIC", "IP��ַ��", WS_VISIBLE | WS_CHILD, 260, 90, 80, 20, hwnd, NULL, NULL, NULL);
			ipComboBox = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN, 330, 87, 150, 200, hwnd, NULL, NULL, NULL);
			SendMessage(ipComboBox, CB_ADDSTRING, 0, (LPARAM) "192.168.60.250");
			SendMessage(ipComboBox, CB_ADDSTRING, 0, (LPARAM) "192.168.60.199");
			SendMessage(ipComboBox, CB_SETCURSEL, 0, 0);
			sharedFolderLabel = CreateWindow("STATIC", "����Ŀ¼��", WS_VISIBLE | WS_CHILD, 260, 120, 75, 20, hwnd, NULL, NULL, NULL);
			sharedFolderEdit = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN, 330, 117, 150, 200, hwnd, NULL, NULL, NULL);
			SendMessage(sharedFolderEdit, CB_ADDSTRING, 0, (LPARAM) "�������");
			SendMessage(sharedFolderEdit, CB_ADDSTRING, 0, (LPARAM) "home");
			SendMessage(sharedFolderEdit, CB_SETCURSEL, 0, 0);
			driveLabel = CreateWindow("STATIC", "�̷���", WS_VISIBLE | WS_CHILD, 260, 150, 80, 20, hwnd, NULL, NULL, NULL);
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
			countdownLabel = CreateWindow("STATIC", "��Ͽ����л��У�", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 260, 180, 180, 20, hwnd, NULL, NULL, NULL);
			connectButton = CreateWindow("BUTTON", "ӳ��", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 260, 202, 103, 25, hwnd, (HMENU) 1, NULL, NULL);
			disconnectButton = CreateWindow("BUTTON", "�Ͽ�ȫ��", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 375, 202, 103, 25, hwnd, (HMENU) 2, NULL, NULL);
			ForcedisconnectallButton = CreateWindow("BUTTON", "ǿ�ƶϿ�ȫ��(������Դ������)", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 260, 232, 218, 25, hwnd, (HMENU) 333, NULL, NULL);
			htext = CreateWindow("Static", "���»س���(Enter)���л�����һ��(��ǰ����������ӳ�䰴ť)", WS_CHILD | WS_VISIBLE, 260, 263, 220, 40, hwnd, NULL, hInst, NULL);

			hGroupBoxSettings = CreateWindow("BUTTON", "����/����", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 5, 5, 240, 300, hwnd, NULL, NULL, NULL);
			showPasswordCheckbox = CreateWindow("BUTTON", "��ʾ����", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, 85, 80, 20, hwnd, (HMENU)999, NULL, NULL);
			LockPassword = CreateWindow("BUTTON", "��������", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 100, 85, 80, 20, hwnd, (HMENU)998, NULL, NULL);
			countdownCheckbox = CreateWindow("BUTTON", "�״�ӳ��ɹ���������ʱ", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, 130, 200, 20, hwnd, (HMENU)3, NULL, NULL);
			hStaticSetCountdownTime = CreateWindow("Static", "���õ���ʱʱ�䣺", WS_CHILD | WS_VISIBLE, 10, 150, 120, 20, hwnd, NULL, hInst, NULL);
			countdownEdit = CreateWindow("EDIT", "40", WS_VISIBLE | WS_CHILD | WS_BORDER, 130, 150, 40, 20, hwnd, NULL, NULL, NULL);
			countdownUnitComboBox = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_DROPDOWNLIST, 170, 149, 40, 200, hwnd, NULL, NULL, NULL); // ����ʹ��CBS_DROPDOWNLIST��ʽʹ�ؼ��޷��༭
			SendMessage(countdownUnitComboBox, CB_ADDSTRING, 0, (LPARAM) "��");
			SendMessage(countdownUnitComboBox, CB_ADDSTRING, 0, (LPARAM) "��");
			SendMessage(countdownUnitComboBox, CB_SETCURSEL, 0, 0);
			jieshudaojishicheckbox = CreateWindow("BUTTON", "�Ͽ��󲻽�������ʱ", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, 170, 160, 20, hwnd, NULL, NULL, NULL);
			detailedButton = CreateWindow("BUTTON", "��ϸ", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 271, 70, 30, hwnd, (HMENU) 4, NULL, NULL);
			hStaticMappingKernel = CreateWindow("Static", "ӳ���ں�", WS_CHILD | WS_VISIBLE, 10, 25, 60, 20, hwnd, NULL, hInst, NULL);
			hBtnWNetAddConnection2A = CreateWindow("button", "ʹ��WNetAddConnection2A����", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 10, 45, 230, 20, hwnd, (HMENU)5, NULL, NULL);
			hBtnNetCommandLine = CreateWindow("button", "ʹ��NET����������", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 10, 65, 150, 20, hwnd, (HMENU)6, NULL, NULL);
			Saveusername = CreateWindow("BUTTON", "ӳ��ɹ��󱣴��û���", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, 105, 200, 20, hwnd, NULL, NULL, NULL);
			notice = CreateWindow("Static", "ʹ����֪��������û����洢��D�̣�û��D�̽��洢�ڴ˳�������λ�õ��ļ����С��Ƽ����˳������ļ��з��ڳ�C������������С��ɴ�����ݷ�ʽ������ʹ��", WS_CHILD | WS_VISIBLE, 10, 190, 220, 80, hwnd, NULL, hInst, NULL);
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
			break;
		}

		/* ��ʱ�� */
		case WM_TIMER: {
			if (wParam == 2) { // �жϼ�ʱ��ID
				if (GetFocus() == usernameEdit) { // ����û��������ѡ��
					if (GetAsyncKeyState(VK_RETURN) & 0x8000) { // �û�����˻س���
						SetFocus(passwordEdit); // �л������������
						Sleep(500);	// �ȴ�һ�ᣬ��ֹ����
					}
				} else if (GetFocus() == passwordEdit) { // ������������ѡ��
					if (GetAsyncKeyState(VK_RETURN) & 0x8000) { // �û�����˻س���
						SendMessage(connectButton, BM_CLICK, 0, 0); // ���ӳ�䰴ť
						Sleep(500); // �ȴ�һ�ᣬ��ֹ����
					}
				}
				// �жϵ�ǰλ���Ƿ���C��
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
							//std::cout << "�ļ�������" << std::endl;
							if (!noContentExecuted) {
								DestroyWindow(usernameEdit); // ɾ���û��������
								// ����һ��û�������б��
								usernameEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 330, 27, 150, 26, hwnd, NULL, NULL, NULL);
								hasContentExecuted = false; // ���ñ�־����
								noContentExecuted = true; // ���ñ�־����
							}
						} else {
							// �ļ����ڣ���ȡ�û����б����������б�
							//std::cout << "�ļ��Ѵ���" << std::endl;
							// �жϴ洢�ļ��Ƿ�������
							std::ifstream inputFile(filePath);
							if (inputFile.is_open()) {
								std::string line;
								if (std::getline(inputFile, line)) {
									// ����ļ������ݣ����������б�򲢽�������ӵ������б���
									if (!hasContentExecuted) {
										DestroyWindow(usernameEdit); // ɾ���û��������
										// ����һ��û�������б��
										usernameEdit = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | WS_VSCROLL, 330, 27, 150, 200, hwnd, NULL, NULL, NULL); // ʹ��WS_EX_CLIENTEDGE��ʽʹ�������Ĭ��
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
											// ����һ��û�������б��
											usernameEdit = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | WS_VSCROLL, 330, 27, 150, 200, hwnd, NULL, NULL, NULL); // ʹ��WS_EX_CLIENTEDGE��ʽʹ�������Ĭ��
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
										usernameEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 330, 27, 150, 26, hwnd, NULL, NULL, NULL);
										hasContentExecuted = false; // ���ñ�־����
										noContentExecuted = true; // ���ñ�־����
									}
								}
								inputFile.close();
							}
						}
					} else {
						//std::cout << "D�̲����ڻ�ǹ̶�/���ƶ�����" << std::endl;
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
						//std::cout << "�ļ�������" << std::endl;
						if (!noContentExecuted) {
							DestroyWindow(usernameEdit); // ɾ���û��������
							// ����һ��û�������б��
							usernameEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 330, 27, 150, 26, hwnd, NULL, NULL, NULL);
							hasContentExecuted = false; // ���ñ�־����
							noContentExecuted = true; // ���ñ�־����
						}
					} else {
						// �ļ����ڣ���ȡ�û����б����������б�
						//std::cout << "�ļ��Ѵ���" << std::endl;
						// �жϴ洢�ļ��Ƿ�������
						std::ifstream inputFile(filePath);
						if (inputFile.is_open()) {
							std::string line;
							if (std::getline(inputFile, line)) {
								// ����ļ������ݣ����������б�򲢽�������ӵ������б���
								if (!hasContentExecuted) {
									DestroyWindow(usernameEdit); // ɾ���û��������
									// ����һ��û�������б��
									usernameEdit = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | WS_VSCROLL, 330, 27, 150, 200, hwnd, NULL, NULL, NULL); // ʹ��WS_EX_CLIENTEDGE��ʽʹ�������Ĭ��
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
										// ����һ��û�������б��
										usernameEdit = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | WS_VSCROLL, 330, 27, 150, 200, hwnd, NULL, NULL, NULL); // ʹ��WS_EX_CLIENTEDGE��ʽʹ�������Ĭ��
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
									usernameEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 330, 27, 150, 26, hwnd, NULL, NULL, NULL);
									hasContentExecuted = false; // ���ñ�־����
									noContentExecuted = true; // ���ñ�־����
								}
							}
							inputFile.close();
						}
					}
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
							countdownLabel = CreateWindow("STATIC", "��Ͽ����л��У�", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 260, 180, 180, 20, hwnd, NULL, NULL, NULL);
							// ��ʾ��Ϣ��
							std::string komplettMessage = "�ѶϿ�����ӳ���������������\n�����룺" + std::to_string(result) + "��";
							MessageBox(hwnd, komplettMessage.c_str(), "����ʱ��������OwO", MB_OK | MB_ICONINFORMATION);
						} else if (SendMessage(hBtnNetCommandLine, BM_GETCHECK, 0, 0) == BST_CHECKED) {
							string command = "net use * /del /Y";
							system(command.c_str());
							//ȡ�����õ���ʱ
							EnableWindow(countdownEdit, TRUE);
							EnableWindow(countdownUnitComboBox, TRUE);
							// ɾ�ɵ���ʱ
							DestroyWindow(countdownLabel);
							// ���µ���ʱ
							countdownLabel = CreateWindow("STATIC", "��Ͽ����л��У�", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 260, 180, 180, 20, hwnd, NULL, NULL, NULL);
							// ��ʾһ����Ϣ�������������ѶϿ�ӳ��
							MessageBox(hwnd, "�ѶϿ�ȫ��ӳ�����������", "����ʱ��������OwO", MB_OK | MB_ICONINFORMATION);
						}
					}
				}
			}
			break;
		}

		case WM_COMMAND: { // ����ť����¼�
			switch (wParam) {

				case 999: { // ��ʾ����
					LONG_PTR style = GetWindowLongPtr(passwordEdit, GWL_STYLE); // ��ȡ��������ʽ
					if (style & ES_PASSWORD) { // �ж��������ʽ���Ƿ����ES_PASSWORD��־
						/* �����������������Ϊ�����ַ�������ES_PASSWORD�� */
						// ��ȡ����������������
						char passwordBuf[256];
						GetWindowText(passwordEdit, passwordBuf, sizeof(passwordBuf));
						// ɾ���ɵ������ַ����������
						DestroyWindow(passwordEdit);
						// �����µ��������������
						passwordEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 330, 57, 150, 26, hwnd, NULL, NULL, NULL);
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
						passwordEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD | ES_AUTOHSCROLL, 330, 57, 150, 26, hwnd, NULL, NULL, NULL);
						// �����ݸ��Ƶ������������
						SetWindowText(passwordEdit, passwordBuf);
					}
					break; // �˳�ѭ�������û�������������һ�еĴ��룬��������Ӧ��������
				}

				case 998 : { // ��������
					if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �ж� �������� ��ѡ���Ƿ񱻹�ѡ
						// ��ѡ
						MessageBox(hwnd, "����ѡ���������븴ѡ�򣬵�ӳ��ɹ���ʧ�ܺ󽫲����������\n����ע�⣬�������ӳ�������������������Է���������й¶��", "��ʾOwO", MB_OK | MB_ICONINFORMATION);
					}
					break;
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
								std::cout << "����������ӳ��ɹ���\n�����룺" << result << "��" << std::endl;
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� �������� ��ѡ���Ƿ�δ����ѡ
									SetWindowText(passwordEdit, ""); // �������������ı�����Ϊ���ַ���
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
								std::string successMessage = "����������ӳ��ɹ���OwO\n�����룺" + std::to_string(result) + "��";
								MessageBox(hwnd, successMessage.c_str(), "�ɹ�����OwO", MB_OK | MB_ICONINFORMATION);
							} else {
								std::cout << "����������ӳ��ʧ�ܣ�\n�����룺" << result << "��" << std::endl;
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� �������� ��ѡ���Ƿ�δ����ѡ
									SetWindowText(passwordEdit, ""); // �������������ı�����Ϊ���ַ���
								}
								std::string errorMessage = "����������ӳ��ʧ�ܣ�X_X\n�����룺" + std::to_string(result) + "��";
								MessageBox(hwnd, errorMessage.c_str(), "�����ˣ�X_X", MB_OK | MB_ICONERROR);
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
								// ���ӳɹ�
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� �������� ��ѡ���Ƿ�δ����ѡ
									SetWindowText(passwordEdit, ""); // �������������ı�����Ϊ���ַ���
								}
								MessageBox(hwnd, "û���κ����⣡OwO", "�ɹ�����OwO", MB_OK | MB_ICONINFORMATION);
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
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� �������� ��ѡ���Ƿ�δ����ѡ
									SetWindowText(passwordEdit, ""); // �������������ı�����Ϊ���ַ���
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
								std::string successMessage = "����������ӳ��ɹ���OwO\n�����룺" + std::to_string(result) + "��";
								MessageBox(hwnd, successMessage.c_str(), "�ɹ�����OwO", MB_OK | MB_ICONINFORMATION);
							} else {
								std::cout << "����������ӳ��ʧ�ܣ�\n�����룺" << result << "��" << std::endl;
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� �������� ��ѡ���Ƿ�δ����ѡ
									SetWindowText(passwordEdit, ""); // �������������ı�����Ϊ���ַ���
								}
								std::string errorMessage = "����������ӳ��ʧ�ܣ�X_X\n�����룺" + std::to_string(result) + "��";
								MessageBox(hwnd, errorMessage.c_str(), "�����ˣ�X_X", MB_OK | MB_ICONERROR);
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
								// ���ӳɹ�
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� �������� ��ѡ���Ƿ�δ����ѡ
									SetWindowText(passwordEdit, ""); // �������������ı�����Ϊ���ַ���
								}
								MessageBox(hwnd, "û���κ����⣡OwO", "�ɹ�����OwO", MB_OK | MB_ICONINFORMATION);
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
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� �������� ��ѡ���Ƿ�δ����ѡ
									SetWindowText(passwordEdit, ""); // �������������ı�����Ϊ���ַ���
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
								std::string successMessage = "����������ӳ��ɹ���OwO\n�����룺" + std::to_string(result) + "��";
								MessageBox(hwnd, successMessage.c_str(), "�ɹ�����OwO", MB_OK | MB_ICONINFORMATION);
							} else {
								std::cout << "����������ӳ��ʧ�ܣ�\n�����룺" << result << "��" << std::endl;
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� �������� ��ѡ���Ƿ�δ����ѡ
									SetWindowText(passwordEdit, ""); // �������������ı�����Ϊ���ַ���
								}
								std::string errorMessage = "����������ӳ��ʧ�ܣ�X_X\n�����룺" + std::to_string(result) + "��";
								MessageBox(hwnd, errorMessage.c_str(), "�����ˣ�X_X", MB_OK | MB_ICONERROR);
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
								// ���ӳɹ�
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� �������� ��ѡ���Ƿ�δ����ѡ
									SetWindowText(passwordEdit, ""); // �������������ı�����Ϊ���ַ���
								}
								MessageBox(hwnd, "û���κ����⣡OwO", "�ɹ�����OwO", MB_OK | MB_ICONINFORMATION);
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
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� �������� ��ѡ���Ƿ�δ����ѡ
									SetWindowText(passwordEdit, ""); // �������������ı�����Ϊ���ַ���
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
								std::string successMessage = "����������ӳ��ɹ���OwO\n�����룺" + std::to_string(result) + "��";
								MessageBox(hwnd, successMessage.c_str(), "�ɹ�����OwO", MB_OK | MB_ICONINFORMATION);
							} else {
								std::cout << "����������ӳ��ʧ�ܣ�\n�����룺" << result << "��" << std::endl;
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� �������� ��ѡ���Ƿ�δ����ѡ
									SetWindowText(passwordEdit, ""); // �������������ı�����Ϊ���ַ���
								}
								std::string errorMessage = "����������ӳ��ʧ�ܣ�X_X\n�����룺" + std::to_string(result) + "��";
								MessageBox(hwnd, errorMessage.c_str(), "�����ˣ�X_X", MB_OK | MB_ICONERROR);
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
								// ���ӳɹ�
								if (SendMessage(LockPassword, BM_GETCHECK, 0, 0) == BST_UNCHECKED) { // �ж� �������� ��ѡ���Ƿ�δ����ѡ
									SetWindowText(passwordEdit, ""); // �������������ı�����Ϊ���ַ���
								}
								MessageBox(hwnd, "û���κ����⣡OwO", "�ɹ�����OwO", MB_OK | MB_ICONINFORMATION);
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
						countdownLabel = CreateWindow("STATIC", "��Ͽ����л��У�", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 260, 180, 180, 20, hwnd, NULL, NULL, NULL);
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
						std::string komplettMessage = "�ѶϿ�����ӳ���������������\n�����룺" + std::to_string(result) + "��";
						MessageBox(hwnd, komplettMessage.c_str(), "�������OwO", MB_OK | MB_ICONINFORMATION);
					} else if (SendMessage(hBtnNetCommandLine, BM_GETCHECK, 0, 0) == BST_CHECKED) {
						string command = "net use * /del /Y";
						system(command.c_str());
						MessageBox(hwnd, "����ɲ�����OwO", "�������OwO", MB_OK | MB_ICONINFORMATION);
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
						countdownLabel = CreateWindow("STATIC", "��Ͽ����л��У�", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 260, 180, 180, 20, hwnd, NULL, NULL, NULL);
						// ���õ���ʱ
						EnableWindow(countdownLabel, FALSE);
						EnableWindow(countdownEdit, FALSE);
						EnableWindow(countdownUnitComboBox, FALSE);
					}
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
						countdownLabel = CreateWindow("STATIC", "��Ͽ����л��У�", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 260, 180, 180, 20, hwnd, NULL, NULL, NULL);
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
						std::string komplettMessage = "�ѶϿ�����ӳ���������������\n������������볢�������������X_x\n�����룺" + std::to_string(result) + "��";
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
						MessageBox(hwnd, "����ɲ�����OwO\n������������볢�������������X_x", "�������OwO", MB_OK | MB_ICONINFORMATION);
					}
					break;
				}

				case 4: { // ��ϸ
					MessageBox(hwnd, "Q&A�ʴ�Question&Answer����\nQ������ʲô����ʲô�ã�\nA�����ǿ��Խ�NAS����洢��ӳ�䵽���������������������������൱��Ӳ�̣��ĸ�������������ʹ������Я��ʹ������Զ������洢���豸\nQ��NAS��ʲô��\nA��NAS��Network Attached Storage�����總���洢���������˵���������������ϣ��߱����ϴ洢���ܵ�װ�ã����Ҳ��Ϊ������洢����������һ��ר�����ݴ洢������������Դ�� �ٶȰٿ�\n\nʹ����֪��\n������û������洢��D���У�����У������û��D�̽��洢�ڴ˳�������λ�õ��ļ����С��Ƽ����˳�����ͬ�ļ��з��ڳ�C������������С��ɴ�����ݷ�ʽ������ʹ��\n\n�߼���\n���������ڹ�ѡ\"�Զ��򿪴洢�豸\"��ѡ���ӳ�䣬ӳ��ɹ����Զ���ӳ����ļ���\n\n������֧�֣�\nʹ�õ��Ĳ���ӳ����룺net use [�̷�]: \\[IP]\[�����ļ���]��ʹ�õ��Ĳ��ֶϿ����룺net use * /del /Y�����ϴ����Ϊ�����У�������ʾ�������룬������������ֱ��ʹ�á������ṩ�Ĵ��벢��Ϊ�������õ������д��룬���������������ѯ����ӳ�丨������֧�ֶ��ֲ�ͬ��ӳ���߼��жϣ�������ǹ���Ա����������˻�ֱ�ӽ���ӳ�䡣֧���Զ��嵹��ʱʱ�估��λ����������ڵ���ʱʱ����˳��������ѡ�֧�����ص���̨��ϵͳ���̣��������е���ʱ���������������ڵ���˳�-ѡ�����ص�������-Ȼ��ȷ������������˲������ѿ������������ã�\n\n�������⼰����취��\n��ʹ�õĹ����п��ܻ�������������Ӧ�������������������������ĵȴ����룬��ȴ�ʱ�����������ǿ�ƹرճ����������Ҽ�-����������������-�л���Ӧ�ó���ѡ��ҳ-ѡ��˳���-�����������\n\n���ǵ����Ĳ��ܻ��������������Ͷ����ǵ�֧�֣���󣬸�л����ʹ�ã����������ߣ�Ling", "��ϸ^u^", MB_OK );
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
			if (SendMessage(A_Top, BM_GETCHECK, 0, 0) == BST_CHECKED) { // �ж� �ö� ��ѡ���Ƿ񱻹�ѡ
				// ��ѡ
				SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // �ö�
			}
			HWND SystemToolsGroupBox = CreateWindow("BUTTON", "ϵͳ����", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 5, 5, 210, 305, hwnd, NULL, NULL, NULL);
			HWND restartButton = CreateWindow("BUTTON", "������Դ�������������", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 15, 30, 190, 30, hwnd, (HMENU)1, NULL, NULL);
			HWND OthertoolsGroupBox = CreateWindow("BUTTON", "��������", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 220, 5, 210, 305, hwnd, NULL, NULL, NULL);
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
	int windowWidth = 280; // ���ڿ��
	int windowHeight = 300; // ���ڸ߶�
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
