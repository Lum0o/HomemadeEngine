#include "App.h"

#include "RenderEngine/RenderEngine.h"
#include "Resources/Resource.h"
#include "Window/Window.h"
#include "Resources/framework.h"
#include "Resources/GameTimer.h"

App::App()
{
    mMainWindow = new Window();
    mRenderEngine = new RenderEngine();
    mGameTimer = GameTimer::GetInstance();
}

App::~App()
{
    RELPTR(mMainWindow);
    RELPTR(mRenderEngine);
    RELPTR(mGameTimer);
}

void App::Init()
{
    mRenderEngine->Init();
}

int App::Run()
{
    HACCEL hAccelTable = LoadAccelerators(mMainWindow->GetHInstance(), MAKEINTRESOURCE(IDC_ENGINE));
    
     MSG msg = {0};
    mGameTimer->Reset();
    
    while(msg.message != WM_QUIT)
    {
        // If there are Window messages then process them.
        if(PeekMessage( &msg, 0, 0, 0, PM_REMOVE ))
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        // Otherwise, do animation/game stuff.
        else
        {
            mGameTimer->Tick();
            if( !mAppPaused )
            {
                //CalculateFrameStats();
                Update();
                //Draw();
            }
            else
            {
            Sleep(100);
            }
        }
    }
 return (int)msg.wParam;

}

void App::Update()
{
    
}

