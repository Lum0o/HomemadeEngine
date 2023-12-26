#pragma once
#include <list>

#include "Resources/framework.h"

class Window;

class RenderEngine
{
public:
    RenderEngine();
    ~RenderEngine();

    void Init();
    
    void AddScissorRectangles(int left, int top, int right, int bottom);

private:
    
    // The factory is used to create the swap chain and enumerate all the Devices of the pc
    IDXGIFactory4* mFactory;
    // emulates 3D hardware functionality and used to create all the other Direct3D object 
    ID3D12Device* mDevice;
    //the fence is useful to synchronize the CPU and the GPU
    ID3D12Fence* mFence;

    //descriptors that describe render target resources (array backing of all the RTV descriptor you'll use)
    ID3D12DescriptorHeap* mRtvHeap;
    //descriptors that describe depth/stencil resources (array backing of all the DSV descriptor you'll use)
    ID3D12DescriptorHeap* mDsvHeap;
    
    UINT mRtvDescriptorSize;
    UINT mDsvDescriptorSize;
    UINT mCbvSrvDescriptorSize;

    // quality level of 4X MSAA
    UINT m4xMsaaQuality;
    // 4X MSAA enabled
    bool m4xMsaaState = false; 
    
    DXGI_FORMAT mBackBufferFormat;
    static const int mSwapChainBufferCount = 2;
    int mCurrBackBuffer = 0;

    //Two descriptor (RTV) for each buffer
    ID3D12Resource* mSwapChainBuffer[mSwapChainBufferCount];
    //Swap chain contain the two buffers used to render objects on the screen, it swap between the backBuffer and the frontBuffer
    IDXGISwapChain* mSwapChain;

    //the commandQueue is used by the GPU to execute command sent by the CPU
    ID3D12CommandQueue* mCommandQueue;
    //The graphics Command List is used to send command to the command queue.
    ID3D12GraphicsCommandList* mCommandList;
    //memory backing for the command list when it's executed by the GPU
    ID3D12CommandAllocator* mDirectCmdListAlloc;

    //Format link to the resource, describe what kind of resources are send to the GPU 
    DXGI_FORMAT mDepthStencilFormat;
    //Depth stencil buffer, used to know the depth of object to render. 
    ID3D12Resource* mDepthStencilBuffer;

    //the viewPort is the zone where the backbuffer is draw
    D3D12_VIEWPORT mVp;

    UINT mClientWidth;
    UINT mClientHeight;

    HWND hWnd;

    //List of scissor rectangles ( "Dead-zone" where the backbuffer doesn't draw )
    list<D3D12_RECT*> mScissorRectangles;

    //can't be call in the constructor because we have to be sure that
    //our window is initialized before binding theses members
    void InitWindowMembers();

    //All those methods initialize our renderEngine 
    void CreateDeviceAndFactory();
    void CreateFenceAndDescriptorSizes();
    void CheckMSAAQualitySupport();
    void CreateCommandQueueAndCommandList();
    void CreateSwapChain();
    void CreateDescriptorHeaps();
    void CreateRenderTargetView();
    void CreateDepthStencilBufferView();
    void SetViewport();
    
    D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;
    
    ID3D12Debug* mDebugController;
};
