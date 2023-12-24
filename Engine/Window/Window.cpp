#include "Window.h"
#include "Resources/Resource.h"

HWND Window::hWnd = nullptr;

Window::Window()
{
    mHInst = GetModuleHandle(nullptr);
    mPrevInstance = nullptr;
    mCmdLine = nullptr;
    mCmdShow = SW_SHOW;
    mTitle = L"Engine";
    bool res = InitWindow();
}

bool Window::InitWindow() const
{
    ATOM at = MyRegisterClass();

    // Perform application initialization:
    if (!InitInstance())
    {
        return FALSE;
    }

    return TRUE;
}

BOOL Window::InitInstance() const
{
    bool fullscreen = false;

    hWnd = CreateWindowW(mSzWindowClass, mSzTitle,
        fullscreen ? WS_POPUP : WS_OVERLAPPEDWINDOW,
        100, 100, 1600, 900,
        nullptr, nullptr, mHInst, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, mCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

ATOM Window::MyRegisterClass() const
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = mHInst;
    wcex.hIcon = LoadIcon(mHInst, MAKEINTRESOURCE(IDI_ENGINE));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = mSzWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}