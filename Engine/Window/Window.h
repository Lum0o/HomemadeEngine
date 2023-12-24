#pragma once
#include "Resources/framework.h"
#define MAX_LOADSTRING 100

// Handles the Windows window
// ref to :
// https://learn.microsoft.com/fr-fr/cpp/windows/walkthrough-creating-windows-desktop-applications-cpp?view=msvc-170
// for more documentation about create a window in cpp
class Window
{
public:
	Window();
	~Window() = default;

	bool InitWindow() const;

	static inline HWND& GetHWND() { return hWnd; }

	inline HINSTANCE& GetHInstance() { return mHInst; }

	inline wstring GetWindowTitle() { return mTitle; }

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:

	static HWND hWnd;
	HINSTANCE mHInst; // current instance
	WCHAR mSzTitle[MAX_LOADSTRING] = L"Engine"; // The title bar text
	WCHAR mSzWindowClass[MAX_LOADSTRING] = L"Engine"; // the main window class name (as to be unique in all your windows launch on your pc)
	wstring mTitle;

	HINSTANCE mPrevInstance = nullptr;
	LPWSTR mCmdLine = nullptr;
	int mCmdShow = SW_SHOW;

	BOOL InitInstance() const;

	ATOM MyRegisterClass() const;

};
