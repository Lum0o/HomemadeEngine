#include "RenderEngine.h"

#include "Window/Window.h"

RenderEngine::RenderEngine()
{
    mDebugController = nullptr;
    
#if defined(DEBUG) || defined(_DEBUG)
    // Enable the D3D12 debug layer.
    {
        D3D12GetDebugInterface(IID_PPV_ARGS(&mDebugController));
        mDebugController->EnableDebugLayer();
    }
#endif

    mFactory = nullptr;
    mDevice = nullptr;
    mFence = nullptr;

    mRtvHeap = nullptr;
    mDsvHeap = nullptr;
    mRtvDescriptorSize = 0;
    mDsvDescriptorSize = 0;
    mCbvSrvDescriptorSize = 0;
    
    mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    m4xMsaaQuality = 0;

    mCommandQueue = nullptr;
    mCommandList = nullptr;
    mDirectCmdListAlloc = nullptr;

    mSwapChain = nullptr;
    mClientWidth = 0;
    mClientHeight = 0;

    mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    mDepthStencilBuffer = nullptr;

    mScissorRectangles = {};
    
}

RenderEngine::~RenderEngine()
{
    RELPTRDX(mDebugController);
    
    RELPTRDX(mFactory);
    RELPTRDX(mDevice);
    RELPTRDX(mFence);

    RELPTRDX(mRtvHeap);
    RELPTRDX(mDsvHeap);
    
    RELPTRDX(mCommandQueue);
    RELPTRDX(mCommandList);
    RELPTRDX(mDirectCmdListAlloc);
    
    RELPTRDX(mSwapChain);
    
    for(auto descriptor : mSwapChainBuffer)
        RELPTRDX(descriptor);

    RELPTRDX(mDepthStencilBuffer);

    for(auto scissorRectangle : mScissorRectangles)
        RELPTR(scissorRectangle);

}

void RenderEngine::Init()
{
    InitWindowMembers();
    
    CreateDeviceAndFactory();
    CreateFenceAndDescriptorSizes();
    CheckMSAAQualitySupport();
    CreateCommandQueueAndCommandList();
    CreateSwapChain();
    CreateDescriptorHeaps();
    CreateRenderTargetView();
    CreateDepthStencilBufferView();
    SetViewport();
}

void RenderEngine::InitWindowMembers()
{
    hWnd = Window::GetHWND();
    RECT r;
    GetClientRect(hWnd, &r);
    mClientWidth = r.right - r.left;
    mClientHeight = r.bottom - r.top;
}

void RenderEngine::CreateDeviceAndFactory()
{
    CreateDXGIFactory1(IID_PPV_ARGS(&mFactory));
    // Try to create hardware device.
    
    HRESULT hardwareResult = D3D12CreateDevice(
     nullptr, // default adapter
     D3D_FEATURE_LEVEL_11_0,
     IID_PPV_ARGS(&mDevice));
    // Fallback to WARP device.
    if(FAILED(hardwareResult))
    {
        IDXGIAdapter* pWarpAdapter;
        mFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter));
        D3D12CreateDevice(
        pWarpAdapter,
        D3D_FEATURE_LEVEL_11_0,
        IID_PPV_ARGS(&mDevice));
    }
}

void RenderEngine::CreateFenceAndDescriptorSizes()
{
    mDevice->CreateFence(
 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
    mRtvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(
     D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    mDsvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(
     D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    mCbvSrvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(
     D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void RenderEngine::CheckMSAAQualitySupport()
{
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
    msQualityLevels.Format = mBackBufferFormat;
    msQualityLevels.SampleCount = 4;
    msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    msQualityLevels.NumQualityLevels = 0;
    mDevice->CheckFeatureSupport(
     D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
     &msQualityLevels,
     sizeof(msQualityLevels));
    m4xMsaaQuality = msQualityLevels.NumQualityLevels;
    assert(m4xMsaaQuality > 0 && "Unexpected MSAA quality level.");
}

void RenderEngine::CreateCommandQueueAndCommandList()
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    
    mDevice->CreateCommandQueue(
        &queueDesc, IID_PPV_ARGS(&mCommandQueue));
        mDevice->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(&mDirectCmdListAlloc));
    
    mDevice->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        mDirectCmdListAlloc, // Associated command allocator
        nullptr, // Initial PipelineStateObject
        IID_PPV_ARGS(&mCommandList));
    
    // Start off in a closed state. This is because the first time we
    // refer to the command list we will Reset it, and it needs to be
    // closed before calling Reset.
    mCommandList->Close();
}

void RenderEngine::CreateSwapChain()
{
        // Release the previous swapchain we will be recreating.
        RELPTRDX(mSwapChain);
    
        DXGI_SWAP_CHAIN_DESC sd;
        sd.BufferDesc.Width = mClientWidth;
        sd.BufferDesc.Height = mClientHeight;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferDesc.Format = mBackBufferFormat;
        sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        sd.SampleDesc.Count = m4xMsaaState ? 4 : 1;
        sd.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = mSwapChainBufferCount;
        sd.OutputWindow = hWnd;
        sd.Windowed = true;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    
        // Note: Swap chain uses queue to perform flush.
        mFactory->CreateSwapChain(
            mCommandQueue,
            &sd,
            &mSwapChain);
}

void RenderEngine::CreateDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;

    rtvHeapDesc.NumDescriptors = mSwapChainBufferCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask = 0;

     mDevice->CreateDescriptorHeap(
        &rtvHeapDesc, IID_PPV_ARGS(&mRtvHeap));

    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;

    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.NodeMask = 0;

    mDevice->CreateDescriptorHeap(
        &dsvHeapDesc, IID_PPV_ARGS(&mDsvHeap));
}

void RenderEngine::CreateRenderTargetView()
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
    
    for (UINT i = 0; i < mSwapChainBufferCount; i++)
    {
        // Get the ith buffer in the swap chain.
        mSwapChain->GetBuffer(
            i, IID_PPV_ARGS(&mSwapChainBuffer[i]));
        
        // Create an RTV to it.
        mDevice->CreateRenderTargetView(
            mSwapChainBuffer[i], nullptr, rtvHeapHandle);
        
        // Next entry in heap.
        rtvHeapHandle.Offset(1, mRtvDescriptorSize);
    }
}

void RenderEngine::CreateDepthStencilBufferView()
{
    // Create the depth/stencil buffer and view.
    D3D12_RESOURCE_DESC depthStencilDesc;
    
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = 0;
    depthStencilDesc.Width = mClientWidth;
    depthStencilDesc.Height = mClientHeight;
    depthStencilDesc.DepthOrArraySize = 1;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.Format = mDepthStencilFormat;
    depthStencilDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
    depthStencilDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    D3D12_CLEAR_VALUE optClear;
    optClear.Format = mDepthStencilFormat;
    optClear.DepthStencil.Depth = 1.0f;
    optClear.DepthStencil.Stencil = 0;
    
    mDevice->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &depthStencilDesc,
        D3D12_RESOURCE_STATE_COMMON,
        &optClear,
         IID_PPV_ARGS(&mDepthStencilBuffer));
    
    // Create descriptor to mip level 0 of entire resource using the
    // format of the resource.
    mDevice->CreateDepthStencilView(
        mDepthStencilBuffer,
        nullptr,
        DepthStencilView());
    
    // Transition the resource from its initial state to be used as a depth buffer.
    mCommandList->ResourceBarrier(
        1,
        &CD3DX12_RESOURCE_BARRIER::Transition(
            mDepthStencilBuffer,
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_DEPTH_WRITE));
}

void RenderEngine::SetViewport()
{
    mVp.TopLeftX = 0.0f;
    mVp.TopLeftY = 0.0f;
    mVp.Width = static_cast<float>(mClientWidth);
    mVp.Height = static_cast<float>(mClientHeight);
    mVp.MinDepth = 0.0f;
    mVp.MaxDepth = 1.0f;
    mCommandList->RSSetViewports(1, &mVp);
}

void RenderEngine::AddScissorRectangles(int left, int top, int right, int bottom)
{
    D3D12_RECT* scissorRect = new D3D12_RECT( left, top, right, bottom );
    mScissorRectangles.push_back(scissorRect);
    
    mCommandList->RSSetScissorRects(1, scissorRect);
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderEngine::DepthStencilView() const
{
    return mDsvHeap->GetCPUDescriptorHandleForHeapStart();
}

