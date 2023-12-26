#pragma once
class Window;
class RenderEngine;
class GameTimer;


// Class used to launch and update all the different parts of the Engine
class App
{
public:
    
    App();
    ~App();

    void Init();

    int Run();
    void Update();
    
private:
    Window* mMainWindow;  //Window where the game is displayed
    RenderEngine* mRenderEngine; //Render part of the global engine, powered by DirectX12
    GameTimer* mGameTimer; //timer of the app, store deltaTime and totalTime

    bool mAppPaused = false;
};
