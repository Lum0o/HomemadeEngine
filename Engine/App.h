#pragma once
class Window;
class RenderEngine;


// Class used to launch and update all the different parts of the Engine
class App
{
public:
    
    App();
    ~App();

    void Init();

    int update();
    
private:
    Window* mMainWindow;  //Window where the game is displayed
    RenderEngine* mRenderEngine; //Render part of the global engine, powered by DirectX12
};
