#include <windows.h>

/* �����������뵽���ڵ�λ�� */
HWND WB, ZD, DD, A1, B2, C3, D4, E5, F6;
HINSTANCE hInst;
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch (Message) {

		case WM_CREATE: { //����ʱ
			//MessageBox(NULL, "                                               ��ʾ����������\n��ʾ��\n��ݲ˵����������֣�2022-11-7�����°汾Ϊ1.2���������ڿ���1.3�汾�������п��������������ӿ�Ⱥ��864229402\n================================================\n�������ݣ�\n1.2�������ݣ�����һ�����������ְ�ť���ڴ�Ӧ�ô��ڵ����½ǣ���������ʹ��Ӧ�ô��ڶ���һ�Σ�û�¸ɿ����������֡��������˹رմ�Ӧ�ô���ʱ��ѯ�ʣ�ȷ�ϣ����رմ�Ӧ��ʱ���ᵯ��һ����Ϣ���������ǡ���ſɳ��׹رմ�Ӧ�á�\n\n�޸�һ��bug�������һ���򿪰ٶ���ҳ��������IE������򿪰ٶ���ҳ����ΪIE�����Ĭ����Ĭ��������������޸������ڻ��ùȸ�������򿪰ٶ���ҳ��IE�Ѿ����ˣ����ڶ��ùȸ裩��\n\nȻ������Ż����û����飬hhhhh���޸��˴�Ӧ�ô��ڳ���Ϊ��Ӧ�ü�����ͼ�꣨ͼ��Ϊ���ƣ������������˹���ƶ�����Ӧ�ô�����ʱ�Ĺ��״̬�����Ϊ������״̬���ƶ�����ť��ʱ���ᣩ��\n--------------------------------------------------------------------------------------\n1.1�������ݣ�\n������һ���ö����ڵĸ�ѡ��ť�������˴�����󻯰�ť�͵������ڴ�С��\n--------------------------------------------------------------------------------------\n1.0.5�������ݣ�\n������������ť����һ���ǡ���������ʾ�����ڶ����ǡ�һ���򿪰ٶ���ҳ����\n\nȻ������޸��ˡ�ǿ���˳����������ť�����֣��������ǡ�һ���˳�����ѧ���ˡ�����Ϊ�е㳤�����Ծ͸��ˡ����о��ǵ������ǿ���˳�����ʱ��ȷ���꣬�������������ϼ�����ù���ϵͳ�Ĳ����������ʾ���ǳ���nice��\n\n�������޸�������bug����һ��bug�ǵ�������������������ť��bug�����ǵ���򿪺󣬻���һ���ڴ��ڰ����ų��������ھͲ�����������������ˡ��ڶ���bug���˳��˴�Ӧ�ã���Ӧ�û���һֱ����ʹ��״̬��һֱ����ʹ��״̬�ͻᵼ���޷�ɾ����Ӧ�õ�bug�������޸���", "��Ϣ", MB_OK);

			WB = CreateWindow("static", "��ʲô����", WS_CHILD | WS_VISIBLE,
			                  20,   10, 340, 40, hwnd, (HMENU)23333, hInst, NULL);
			ZD = CreateWindow("button", "�ö�����", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			                  150,  58, 80,  15, hwnd, (HMENU)54188, hInst, NULL);
			DD = CreateWindow("button", "��", WS_CHILD | WS_VISIBLE, //���ң����£���������С��
			                  340, 305, 20,  20, hwnd, (HMENU)666, hInst, NULL);
			A1 = CreateWindow("button", "һ���ر��ܲ�Ȧ", WS_CHILD | WS_VISIBLE,
			                  20,   80, 160, 60, hwnd, (HMENU)1, hInst, NULL);
			B2 = CreateWindow("button", "ǿ���˳�����", WS_CHILD | WS_VISIBLE,
			                  200,  80, 160, 60, hwnd, (HMENU)2, hInst, NULL);
			C3 = CreateWindow("button", "��������ʾ��", WS_CHILD | WS_VISIBLE,
			                  20,  160, 160, 60, hwnd, (HMENU)3, hInst, NULL);
			D4 = CreateWindow("button", "һ���򿪰ٶ���ҳ", WS_CHILD | WS_VISIBLE,
			                  200, 160, 160, 60, hwnd, (HMENU)4, hInst, NULL);
			E5 = CreateWindow("button", "�����������", WS_CHILD | WS_VISIBLE,
			                  20,  240, 160, 60, hwnd, (HMENU)5, hInst, NULL);
			F6 = CreateWindow("button", "�ػ�", WS_CHILD | WS_VISIBLE,
			                  200, 240, 160, 60, hwnd, (HMENU)6, hInst, NULL);
			break;
		}

		/* �رմ���ʱ��ѯ���Ƿ�رմ�Ӧ�� */
		//case WM_CLOSE: { //�ر�ʱ
		//	if (MessageBox(NULL, "�Ƿ�رմ�Ӧ�ã�", "ѯ��", MB_YESNO | MB_ICONQUESTION) == IDYES) {
		//		DestroyWindow(hwnd); //���ٴ���
		//	}
		//	break;
		//}

		/* ���ٺ󣬸������߳�ֹͣ */
		case WM_DESTROY: { //����ʱ
			PostQuitMessage(0); //���û��������˳�����ʱ�����ڽ�һֱ����Ϊʹ��״̬
			break;
		}

		case WM_COMMAND: { //����
			switch (wParam) {

				case 54188: { //�ö�����
					HWND hWnd = GetForegroundWindow(); //�������ȡǰ������
					if (GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST) { //��鵱ǰ�����Ƿ�Ϊ�ö�״̬
						SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); //ȡ���ö�
					} else { //�ö� SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); ����"HWND_|>�����<|TOPMOST"����"NO"����ȡ���ö���
						SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); //�ö�
					}
					break;
				}

				case 666: { //��
					RECT rect;
					HWND hwnd = GetForegroundWindow();
					GetWindowRect(hwnd, &rect); //top�ϣ�bottom�£�left��right��
					MoveWindow(hwnd, rect.left + 10, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE); //��
					Sleep(20); //��ֵԽ�󣬶��ķ���Խ��
					MoveWindow(hwnd, rect.left - 10, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE); //��
					Sleep(20);
					MoveWindow(hwnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE); //��ԭ
					break;
				}

				case 1: { //һ���ر��ܲ�Ȧ
					if (MessageBox(NULL, "ȷ��Ҫִ�д˲�����\nȷ�Ϻ������ܲ�Ȧ��ֱ�ӹرգ�\n��ȷ�����Ļ����˺ͳ����Ƿ񱣴棡", "ѯ��", MB_OKCANCEL | MB_ICONQUESTION) == IDOK) {
						system("taskkill /f /t /im IRobotQApp.exe"); //�ر��ܲ�Ȧ�Ľ���
					}
					break;
				}

				case 2: { //ǿ���˳�����
					if (MessageBox(NULL, "ȷ��Ҫִ�д˲�����\nȷ�Ϻ������˳�������ù���ϵͳ�Ĺ�\n�أ��˳������������ܽ�ʦ�˵Ŀ��ƣ�\n--------------------------------------\n������������\nʹ�ô�Ӧ����ɵ����к������\n���Լ��е�����Ӧ�������޹أ�", "ѯ��", MB_OKCANCEL | MB_ICONQUESTION) == IDOK) {
						system("taskkill /f /t /im StudentMain.exe"); //�رռ�����ӽ��ҵĽ���
						MessageBox(NULL, "�������Ҫ���������ϼ�����ù���\nϵͳ������˫�����������ϵļ����\n�ù���ϵͳ�Ŀ�ݷ�ʽ�ȴ����Ӽ���\n--------------------------------------\n������������\nʹ�ô�Ӧ����ɵ����к������\n���Լ��е�����Ӧ�������޹أ�", "��ʾ", MB_OK | MB_ICONASTERISK);
					}
					break;
				}

				case 3: { //��������ʾ��
					system("start cmd.exe"); //��������ʾ��
					break;
				}

				case 4: { //һ���򿪰ٶ���ҳ
					system("start chrome.exe www.baidu.com"); //ʹ�ùȸ�������򿪰ٶ���ҳ
					break;
				}

				case 5: { //�����������
					system("start taskmgr.exe"); //��������������Ҳ����кڴ��ڣ�ֱ����system("taskmgr")�򿪾ͻ��кڴ��ڣ�����system("start taskmgr.exe")�򿪾Ͳ����кڴ��ڣ�
					break;
				}

				case 6: { //�ػ�
					if (MessageBox(NULL, "ȷ��Ҫִ�д˲�����\nȷ�Ϻ����ĵ��Խ���10����Զ��رգ�", "ѯ��", MB_OKCANCEL | MB_ICONQUESTION) == IDOK) {
						system("shutdown -s -t 10"); //�ػ���-s��-t�ǲ��� 10�ǹػ�ʱ�� ��λ���룩
					}
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
		/* ����������Ϣ�����кࣩܶ��ʹ��Ĭ�Ϲ��̴��� */
		default: {
			DefWindowProc(hwnd, Message, wParam, lParam);
			break;
		}
	}
}

/* Win32 GUI����ġ�main������������ִ�п�ʼ�ĵط� */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc; /* ���ڵ����Խṹ */
	HWND hwnd; /* һ����������������H����ָ�򴰿ڵ�ָ�� */
	MSG msg; /* �����ʼ�����ʱλ�� */

	/* ����ṹ������Ҫ�޸ĵ����� */
	memset(&wc, 0, sizeof(wc));
	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.lpfnWndProc	 = WndProc; /* �������Ƿ�����Ϣ�ĵط� */
	wc.hInstance	 = hInstance;
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);

	/* ��ɫ��COLOR_WINDOWֻ��ϵͳ��ɫ��#���壬�볢��Ctrl+������ */
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = "WindowClass";
	wc.hIcon         = LoadIcon(NULL, IDI_SHIELD); /* ʹ�ö���ͼ����ʾ�������� */
	wc.hIconSm		 = LoadIcon(NULL, IDI_SHIELD); /* ʹ�ö���ͼ����ʾ�������� */
	//wc.hCursor       = LoadCursor(NULL, IDC_NO); /* ʹ�ò�����״̬����滻��ԭ��꣨�����ڴ˴����ڣ� */

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "����ע��ʧ�ܣ�", "����", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, "WindowClass", "��ݲ˵� 1.2", WS_VISIBLE | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
	                      CW_USEDEFAULT, /* x */
	                      CW_USEDEFAULT, /* y */
	                      390, /* ���ڳ� */
	                      360, /* ���ڿ� */
	                      NULL, NULL, hInstance, NULL);

	if (hwnd == NULL) {
		MessageBox(NULL, "���ڴ���ʧ�ܣ�", "����", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	/*
		�������ǳ���ĺ��ģ������ﴦ����������
		������WndProc����ע�⣬GetMessage����ֹ��������ֱ�������յ�һЩ����������
		��ѭ���������������ĸ�CPUʹ����
	*/
	while (GetMessage(&msg, NULL, 0, 0) > 0) { /* ���û���յ�����... */
		TranslateMessage(&msg); /* ����Կ����ת��Ϊ�ַ���������ڣ� */
		DispatchMessage(&msg); /* ���͸�WndProc */
	}

	return msg.wParam;
}
