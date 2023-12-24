#include "App.h"

#include "RenderEngine/RenderEngine.h"
#include "Resources/Resource.h"
#include "Window/Window.h"
#include "Resources/framework.h"

App::App()
{
    mMainWindow = new Window();
    mRenderEngine = new RenderEngine();
}

App::~App()
{
    RELPTR(mMainWindow);
    RELPTR(mRenderEngine);
}

void App::Init()
{
    mRenderEngine->Init();
}

int App::update()
{
    HACCEL hAccelTable = LoadAccelerators(mMainWindow->GetHInstance(), MAKEINTRESOURCE(IDC_ENGINE));

    MSG msg;

    // Boucle de messages principal:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

