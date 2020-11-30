﻿// WinApiGame.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "WinApiGame.h"
#include <windowsx.h>
#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Разместите код здесь.

	// Инициализация глобальных строк
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WINAPIGAME, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINAPIGAME));

	MSG msg;

	// Цикл основного сообщения:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINAPIGAME));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	//wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.hbrBackground = (HBRUSH)(GetStockObject(WHITE_BRUSH));
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINAPIGAME);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
const int CELL_SIZE = 120;
HBRUSH hBrash1, hBrash2;
int playerTurn = 1;

BOOL GetGameRect(HWND hwnd, RECT* pRect)
{
	RECT rc;
	if (GetClientRect(hwnd, &rc))
	{
		int width = rc.right - rc.left;
		int height = rc.bottom - rc.top;


		pRect->left = (width - CELL_SIZE * 3) / 2;
		pRect->right = pRect->left + CELL_SIZE * 3;
		pRect->top = (height - CELL_SIZE * 3) / 2;
		pRect->bottom = pRect->top + CELL_SIZE * 3;

		return TRUE;
	}
	SetRectEmpty(pRect);
	return FALSE;
}

void DrawLine(HDC hdc, int x1, int y1, int x2, int y2) {
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
}

int GetCellNumber(HWND hWnd, int x, int y)
{
	POINT pt = { x,y };
	RECT rc;

	if (GetGameRect(hWnd, &rc)) {
		if (PtInRect(&rc, pt)) {
			x = pt.x - rc.left;
			y = pt.y - rc.top;

			int col = x / CELL_SIZE;
			int row = y / CELL_SIZE;

			return col + row * 3;
		}
	}
	return -1;
}

BOOL GetCellRect(HWND hWnd, int index, RECT* pRect) {
	RECT rcBoard;

	SetRectEmpty(pRect);

	if (index < 0 || index > 8)
		return FALSE;
	
	if (GetGameRect(hWnd, &rcBoard))
	{
		int y = index / 3;
		int x = index % 3;

		pRect->left = rcBoard.left + x * CELL_SIZE + 1;
		pRect->top = rcBoard.top + y * CELL_SIZE + 1;
		pRect->right = pRect->left + CELL_SIZE - 1;
		pRect->bottom = pRect->top + CELL_SIZE - 1;

		return TRUE;
	}

	return FALSE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		hBrash1 = CreateSolidBrush(RGB(255, 0, 0));
		hBrash2 = CreateSolidBrush(RGB(0, 0, 255));
	}
	break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Разобрать выбор в меню:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_LBUTTONDOWN:
	{
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		int index = GetCellNumber(hWnd, xPos, yPos);

		HDC hdc = GetDC(hWnd);
		if (NULL != hdc) {
		/*	WCHAR temp[100];
			wsprintf(temp, L"Index = %d", index);
			TextOut(hdc, xPos, yPos, temp, lstrlen(temp));*/

			if (index != -1) {
				RECT rcCell;
				if (GetCellRect(hWnd, index, &rcCell))
				{
					FillRect(hdc, &rcCell, (playerTurn==2) ? hBrash1 : hBrash2);
				}

				playerTurn = (playerTurn == 1) ? 2 : 1;
			}


			ReleaseDC(hWnd, hdc);
		}
	}
	break;
	case WM_GETMINMAXINFO:
	{
		MINMAXINFO* pMinMax = (MINMAXINFO*)lParam;

		pMinMax->ptMinTrackSize.x = CELL_SIZE * 5;
		pMinMax->ptMinTrackSize.y = CELL_SIZE * 5;

	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Добавьте сюда любой код прорисовки, использующий HDC...
		RECT rc;
		if (GetGameRect(hWnd, &rc)) {
			//FillRect(hdc, &rc,(HBRUSH)GetStockObject(WHITE_BRUSH));
		//	Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
		}
		for (int i = 1; i < 3; ++i)
		{
			DrawLine(hdc, rc.left + CELL_SIZE * i, rc.top, rc.left + CELL_SIZE * i, rc.bottom);

			DrawLine(hdc, rc.left, rc.top + CELL_SIZE * i, rc.right, rc.top + CELL_SIZE * i);
		}


		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		DeleteObject(hBrash1);
		DeleteObject(hBrash2);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
