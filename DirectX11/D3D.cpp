// header files
#include <windows.h>
#include <winuser.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
// #include <string>
#include <math.h>
#include "D3D.h"

// D3D related headers
#include <dxgi.h>
#include <D3D11.h>
#include <d3dcompiler.h>

// #include <DirectXMath.h>
// #include <SimpleMath.h>

#pragma warning(disable:4838)
#include "XNAMath_204/xnamath.h"

// #include "Common/DDSTextureLoader.h"
#include "src/Common/helper_timer.h"
#include "src/Common/Camera.h"



// D3D libraries
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment (lib, "Sphere.lib")
// #pragma comment(lib, "DirectXTK.lib")

// global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
// HRESULT LoadD3D_DDSTexture(TCHAR *ddsTextureResourceID, ID3D11Device *pID3D11Device, ID3D11ShaderResourceView **ppID3D11ShaderResourceView);

// #include "Effect/Grass/Grass.h"
// #include "src/Effect/Model/ModelMain.h"
// #include "src/Effect/Model/ModelNew.h"
// #include "src/Effect/Skinning/SkinningMain.h"
#include "src/Effect/Effect.h"
#include "src/Effect/SunSurface/SunSurface.h"
#include "src/Effect/Water/Water.h"
#include "src/Effect/Water/WaterQuad.h"
#include "src/Effect/WaterNew/WaterNew.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600


// global variable declarations
HWND ghwnd = NULL;
BOOL gbFullScreen = FALSE;
FILE *gpFile = NULL;
char gszLogFilePathName[] = "Log.txt";
BOOL gbActiveWindow = FALSE;

// D3D related global variables
IDXGISwapChain *pIDXGISwapChain = NULL;
ID3D11Device *pID3D11Device = NULL;
ID3D11DeviceContext *pID3D11DeviceContext = NULL;
ID3D11RenderTargetView *pID3D11RenderTargetView = NULL;
float clearColor[4];

ID3D11RasterizerState *gpID3D11RasterizerState = NULL;
ID3D11DepthStencilView *gpID3D11DepthStencilView = NULL; 

float g_width = 0.0f;
float g_height = 0.0f;


XMMATRIX PerspectiveProjectionMatrix;

#include "src/Common/DXShaders.h"
// #include "src/Common/MyLogger.h"


Camera camera;
// std::shared_ptr<MyLogger> myLog = nullptr;
// ModelMain *grass = nullptr;
// ModelNew model;
// SkinningMain model_s;
Water *water = nullptr;
WaterNew* waterNew = NULL;
WaterQuad *waterQUad = nullptr;
Effect *effect_noise = nullptr;
// Timer 
StopWatchInterface *timer = NULL;


// entry-point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
    // function declaration
    HRESULT initialize(void);
    void display(void);
    void update(void);
    void uninitialize(void);

    // variable declarations
    WNDCLASSEX wndclass;
    HWND hwnd;
    MSG msg;
    TCHAR szAppName[] = TEXT("MyWindow");
    BOOL bDone = FALSE;
    int iScreenWidth, iScreenHeight;
    int iRetVal = 0;
    HRESULT hr = S_OK;

    // code
    if (fopen_s(&gpFile, gszLogFilePathName, "w") != 0)
    {
        MessageBox(NULL, TEXT("Creation Log File Failed.."), TEXT("File I/O"), MB_OK);
        exit(0);
    }
    else
    {
        fprintf(gpFile, "Log File Created Successfully\n");
        fclose(gpFile);
    }
    // myLog = std::make_shared<MyLogger>();

    // Initialize COM
	hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "CoInitialize() Failed...\n");
		fclose(gpFile);
        // myLog->Logger("CoInitialize() Failed  :: %ls\n", __FUNCTIONW__);
	}
	else
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "CoInitialize() Succeded...\n");
		fclose(gpFile);
        // myLog->Logger("CoInitialize() Succeded    :: %ls\n", __FUNCTIONW__);
	}

    // initialize the WNDCLASSEX struct
    wndclass.cbSize = sizeof(WNDCLASSEX);
    wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.lpfnWndProc = WndProc; // callback for this window
    wndclass.hInstance = hInstance;
    wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.lpszClassName = szAppName;
    wndclass.lpszMenuName = NULL;
    wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

    // register wndclass
    RegisterClassEx(&wndclass);

    // window dimensions
    iScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    iScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    // create window
    hwnd = CreateWindow(szAppName,
                        TEXT("CAMERA Group : RTR5"),
                        WS_OVERLAPPEDWINDOW,
                        (iScreenWidth / 2) - (WIN_WIDTH / 2),	// x c-ordinate
						(iScreenHeight / 2) - (WIN_HEIGHT / 2), // y - coordinate
                        WIN_WIDTH,
                        WIN_HEIGHT,
                        NULL, NULL,
                        hInstance,
                        NULL);

    // assign local hwnd to global hwnd
    ghwnd = hwnd;

    // initialize
	hr = initialize();
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "initialize() Failed...\n");
		fclose(gpFile);
        // myLog->Logger("initialize() Failed    :: %ls\n", __FUNCTIONW__);
		DestroyWindow(hwnd);
		hwnd = NULL;
	}
	else
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "initialize() Succeded...\n");
		fclose(gpFile);
        // myLog->Logger("initialize() Succeded    :: %ls\n", __FUNCTIONW__);
	}

    // show window
    ShowWindow(hwnd, iCmdShow);

    // forgrounding and focusing the window
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);

    // game loop
    while (bDone == FALSE)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                bDone = TRUE;
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            if (gbActiveWindow == TRUE)
            {
                // render the DREAM
                display();
                update();
            }
        }
    }

    uninitialize();

    // uninitialize COM
	CoUninitialize();
    
    return ((int)(msg.wParam));
}

// callback
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    // function delcarations
    void ToggleFullScreen(void);
    HRESULT resize(int width, int height);
    void uninitialize(void);

    // local variable
	HRESULT hr = S_OK;

    // code
    switch (iMsg)
    {
    case WM_SETFOCUS:
        gbActiveWindow = TRUE;
        break;
    case WM_KILLFOCUS:
        gbActiveWindow = FALSE;
        break;

    case WM_ERASEBKGND:
        // return 0;
        break;
    case WM_CHAR:
        camera.keyboardInputs(wParam);
        switch (wParam)
        {
        case 'f':
        case 'F':
            ToggleFullScreen();
            break;
        default:
            break;
        }
        break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case 27:
            DestroyWindow(hwnd);
            break;
        default:
            break;
        }
        break;
    case WM_MOUSEMOVE:
        camera.mouseInputs(LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_MOUSEWHEEL:
        camera.mouseScroll(GET_WHEEL_DELTA_WPARAM(wParam));
        break;
    case WM_SIZE:
        if (pID3D11DeviceContext)
		{
			hr = resize(LOWORD(lParam), HIWORD(lParam));
			if (FAILED(hr))
			{
				fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
				fprintf(gpFile, "resize() In WndProc() Failed...\n");
				fclose(gpFile);
                // myLog->Logger("resize() In WndProc() Failed    :: %ls\n", __FUNCTIONW__);
				return (hr);
			}
		}
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        uninitialize();
        PostQuitMessage(0);
        break;
    }

    return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullScreen(void)
{
    // variable declarations
    static DWORD dwStyle;
    static WINDOWPLACEMENT wp;
    MONITORINFO mi;

    // code
    wp.length = sizeof(WINDOWPLACEMENT);

    if (gbFullScreen == FALSE)
    {
        dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
        if (dwStyle & WS_OVERLAPPEDWINDOW)
        {
            mi.cbSize = sizeof(MONITORINFO);

            if (GetWindowPlacement(ghwnd, &wp) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
            {
                SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
                SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
            }

            ShowCursor(TRUE);
            gbFullScreen = TRUE;
        }
    }
    else
    {
        SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(ghwnd, &wp);
        SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

        ShowCursor(TRUE);
        gbFullScreen = FALSE;
    }
}

UINT m4xMsaaQuality;

HRESULT initialize(void)
{
    // function declarations
    HRESULT PrintD3DInfo(void);
    HRESULT resize(int width, int height);
    // HRESULT LoadD3D_DDSTexture(TCHAR *ddsTextureResourceID, ID3D11Device *pID3D11Device, ID3D11ShaderResourceView **ppID3D11ShaderResourceView);

    // variable declarations
    HRESULT hr = S_OK;
    D3D_DRIVER_TYPE d3dDriverType;
    D3D_DRIVER_TYPE d3dDriverTypes[] = {D3D_DRIVER_TYPE_HARDWARE,
                                        D3D_DRIVER_TYPE_WARP,
                                        D3D_DRIVER_TYPE_REFERENCE};
    D3D_FEATURE_LEVEL d3dFeatureLevel_required = D3D_FEATURE_LEVEL_11_0;
    D3D_FEATURE_LEVEL d3dFeatureLevel_acquired = D3D_FEATURE_LEVEL_10_0;
    UINT numDrivers = 0;
    UINT createDeviceFlags = 0;
    UINT numFeatureLevels = 1;

    // code
    hr = PrintD3DInfo();

    // 
    

    // 1. Initialize SwapChainDescriptor struct
    DXGI_SWAP_CHAIN_DESC dxgiSwapChainDescriptor;
    ZeroMemory((void *)&dxgiSwapChainDescriptor, sizeof(DXGI_SWAP_CHAIN_DESC));
    dxgiSwapChainDescriptor.BufferCount = 1;
    dxgiSwapChainDescriptor.BufferDesc.Width = WIN_WIDTH;
    dxgiSwapChainDescriptor.BufferDesc.Height = WIN_HEIGHT;
    dxgiSwapChainDescriptor.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // defining the PIXELFORMATDESCRIPTOR as like in OpenGL
    dxgiSwapChainDescriptor.BufferDesc.RefreshRate.Numerator = 60;
    dxgiSwapChainDescriptor.BufferDesc.RefreshRate.Denominator = 1;
    dxgiSwapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    dxgiSwapChainDescriptor.OutputWindow = ghwnd;
    dxgiSwapChainDescriptor.SampleDesc.Count = 1;
    // dxgiSwapChainDescriptor.SampleDesc.Quality = m4xMsaaQuality - 1;
    dxgiSwapChainDescriptor.SampleDesc.Quality = 0;
    dxgiSwapChainDescriptor.Windowed = TRUE;

    // 2. Call D3D11CreateDeviceAndSwapChain() for required driver
    numDrivers = sizeof(d3dDriverTypes) / sizeof(d3dDriverTypes[0]);
    for (UINT i=0; i<numDrivers; i++)
    {
        d3dDriverType = d3dDriverTypes[i];
        hr = D3D11CreateDeviceAndSwapChain(NULL,
                                        d3dDriverType,
                                        NULL,
                                        createDeviceFlags,
                                        &d3dFeatureLevel_required,
                                        numFeatureLevels,
                                        D3D11_SDK_VERSION,
                                        &dxgiSwapChainDescriptor,
                                        &pIDXGISwapChain,
                                        &pID3D11Device,
                                        &d3dFeatureLevel_acquired,
                                        &pID3D11DeviceContext);
        if (SUCCEEDED(hr))
            break;
    }
    if (FAILED(hr))
    {
        fopen_s(&gpFile, gszLogFilePathName, "a+");
        fprintf(gpFile, "D3D11CreateDeviceAndSwapChain() Failed in initialize()...\n", __FUNCTIONW__);
        fclose(gpFile);
        // myLog->Logger("D3D11CreateDeviceAndSwapChain() Failed in initialize() :: %ls\n", __FUNCTIONW__);
    }
    else
    {
        fopen_s(&gpFile, gszLogFilePathName, "a+");
        // print the obtained driver types
        if (d3dDriverType == D3D_DRIVER_TYPE_HARDWARE)
        {
            // myLog->Logger("D3D11 Obtained Hardware Driver :: %ls\n", __FUNCTIONW__);
            fprintf(gpFile, "D3D11 Obtained Hardware Driver...\n");
        }
        else if (d3dDriverType == D3D_DRIVER_TYPE_WARP)
        {
            // myLog->Logger("D3D11 Obtained WARP Driver :: %ls\n", __FUNCTIONW__);
            fprintf(gpFile, "D3D11 Obtained WARP Driver...\n");
        }
        else if (d3dDriverType == D3D_DRIVER_TYPE_REFERENCE)
        {
            // myLog->Logger("D3D11 Obtained Referance Driver :: %ls\n", __FUNCTIONW__);
            fprintf(gpFile, "D3D11 Obtained Referance Driver...\n");
        }
        else
        {
            // myLog->Logger("D3D11 Obtained UNKNOWN Driver :: %ls\n", __FUNCTIONW__);
            fprintf(gpFile, "D3D11 Obtained UNKNOWN Driver...\n");
        }

        // print obtained D3D feature level
        if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_11_0)
        {
            // myLog->Logger("D3D11 Obtained 11.0 Feature Level :: %ls\n", __FUNCTIONW__);
            fprintf(gpFile, "D3D11 Obtained 11.0 Feature Level...\n");
        }
        else if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_1)
        {
            // myLog->Logger("D3D11 Obtained 10.1 Feature Level :: %ls\n", __FUNCTIONW__);
            fprintf(gpFile, "D3D11 Obtained 10.1 Feature Level...\n");
        }
        else if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_0)
        {
            // myLog->Logger("D3D11 Obtained 10.0 Feature Level :: %ls\n", __FUNCTIONW__);
            fprintf(gpFile, "D3D11 Obtained 10.0 Feature Level...\n");
        }
        else
        {
            // myLog->Logger("D3D11 Obtained UNKNOWN Feature Level :: %ls\n", __FUNCTIONW__);
            fprintf(gpFile, "D3D11 Obtained UNKNOWN Feature Level...\n");
        }
        fclose(gpFile);
    }


    
    

    
    // Initialize clear color array
    clearColor[0] = 0.0f;
    clearColor[1] = 0.2f;
    // clearColor[1] = 0.0f;
    clearColor[2] = 0.4f;
    // clearColor[2] = 0.0f;
    clearColor[3] = 1.0f;

    // Enabling rasterizer State
    // A. initialize Rasterizer Descriptor
    D3D11_RASTERIZER_DESC d3d11RasterizerDescriptor;
    ZeroMemory((void *)&d3d11RasterizerDescriptor, sizeof(D3D11_RASTERIZER_DESC));
    d3d11RasterizerDescriptor.CullMode = D3D11_CULL_NONE;
    d3d11RasterizerDescriptor.FillMode = D3D11_FILL_SOLID;
    d3d11RasterizerDescriptor.FrontCounterClockwise = FALSE;
    d3d11RasterizerDescriptor.MultisampleEnable = FALSE;
    d3d11RasterizerDescriptor.ScissorEnable = FALSE;
    d3d11RasterizerDescriptor.DepthClipEnable = TRUE;
    d3d11RasterizerDescriptor.AntialiasedLineEnable = FALSE;
    d3d11RasterizerDescriptor.DepthBias = 0;
    d3d11RasterizerDescriptor.DepthBiasClamp = 0.0;
    d3d11RasterizerDescriptor.SlopeScaledDepthBias = 0.0;

    // B. Create Rasterizer state
    hr = pID3D11Device->CreateRasterizerState(&d3d11RasterizerDescriptor, &gpID3D11RasterizerState);
    if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "pID3D11Device::CreateRasterizerState() Failed for Constants ...\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "pID3D11Device::CreateRasterizerState() Succedded for Constants ...\n");
		fclose(gpFile);
	}

    // C. Set the state into Rasterizer pipeline
    pID3D11DeviceContext->RSSetState(gpID3D11RasterizerState);

    
    // grass = new ModelMain(pIDXGISwapChain, pID3D11Device, pID3D11DeviceContext, pID3D11RenderTargetView);
    // if (!grass->Initialize())
    // {
    //     fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
    //     fprintf(gpFile, "grass.Initialize() Failed ...\n");
    //     fclose(gpFile);
    // }
    // initialize Camera
	float _position[] = {0.0f, 5.0f, -300.0f};
	camera = Camera(WIN_WIDTH, WIN_HEIGHT, _position);

    // setting projection matrix to identity
    PerspectiveProjectionMatrix = XMMatrixIdentity();

    // // call to Grass initialize
    // model.InitD3D(ghwnd);
    // model_s = SkinningMain(pIDXGISwapChain, pID3D11Device, pID3D11DeviceContext, pID3D11RenderTargetView);
    // if (!model_s.Initialize())
    // {
    //     fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
    //     fprintf(gpFile, "model_s.Initialize() Failed ...\n");
    //     fclose(gpFile);
    // }

    // Water
    water = new Water(pIDXGISwapChain, pID3D11Device, pID3D11DeviceContext, pID3D11RenderTargetView, gpID3D11DepthStencilView);
    if (water->initializeWaterQuad() == FALSE)
    {
        fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
        fprintf(gpFile, "water.initializeWaterQuad() Failed ...\n");
        fclose(gpFile);
    }
    // water->initializeWaterBedQuad();

    // waterNew = new WaterNew(pIDXGISwapChain, pID3D11Device, pID3D11DeviceContext, pID3D11RenderTargetView);//, myLog);
    

    // waterQUad = new WaterQuad(pIDXGISwapChain, pID3D11Device, pID3D11DeviceContext, pID3D11RenderTargetView);//, myLog);
    // if (!waterQUad->Initialize())
    // {
    //     fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
    //     fprintf(gpFile, "waterQUad.Initialize() Failed ...\n");
    //     fclose(gpFile);
    // }

    // // Noise
    // effect_noise = new SunSurface(pIDXGISwapChain, pID3D11Device, pID3D11DeviceContext, pID3D11RenderTargetView);
    // if (!effect_noise->Initialize())
    // {
    //     fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
    //     fprintf(gpFile, "effect_noise->Initialize() Failed ...\n");
    //     fclose(gpFile);
    // }

    // Timer initialization
    sdkCreateTimer(&timer);
    sdkStartTimer(&timer);

    // warm-up resize
    hr = resize(WIN_WIDTH, WIN_HEIGHT);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "ininitialize()::resize() Failed ...\n");
		fclose(gpFile);
        // myLog->Logger("ininitialize()::resize() Failed :: %ls\n", __FUNCTIONW__);
		return (hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "ininitialize()::resize() Successfull ...\n");
		fclose(gpFile);
        // myLog->Logger("ininitialize()::resize() Successfull :: %ls\n", __FUNCTIONW__);
	}

    fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
    fprintf(gpFile, "ininitialize() Successfull ...\n");
    fclose(gpFile);

    return (hr);
}


HRESULT PrintD3DInfo(void)
{
    // variable declarations
    HRESULT hr = S_OK;
    IDXGIFactory *pIDXGIFactory = NULL;
    IDXGIAdapter *pIDXGIAdapter = NULL;
    DXGI_ADAPTER_DESC DXGIAdapterDescriptor;
    char str[255];

    /* DXGI -> stands for DirectX Graphics Infrastructre */

    // code
    // 1. Create DXGI Factory
    hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void **)&pIDXGIFactory); // pIDXGIFactory bharun yeil
    if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "CreateDXGIFactory() Failed...\n");
		fclose(gpFile);
        // myLog->Logger("CreateDXGIFactory() Failed :: %ls\n", __FUNCTIONW__);
		return (hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "CreateDXGIFactory() Succeded...\n");
		fclose(gpFile);
        // myLog->Logger("CreateDXGIFactory() Succeded :: %ls\n", __FUNCTIONW__);
	}

    // 2. get DXGI Adapter
    if (pIDXGIFactory->EnumAdapters(0, &pIDXGIAdapter) != DXGI_ERROR_NOT_FOUND) // 1st graphics device i.e. 0th graphics card
    {
        // get the description of found adapter
        ZeroMemory((void *)&DXGIAdapterDescriptor, sizeof(DXGI_ADAPTER_DESC));
        pIDXGIAdapter->GetDesc(&DXGIAdapterDescriptor);

        WideCharToMultiByte(CP_ACP, 0, DXGIAdapterDescriptor.Description, 255, str, 255, NULL, NULL);
        fopen_s(&gpFile, gszLogFilePathName, "a+");
        fprintf(gpFile, "Graphics Card Name = %s\n", str);
        fprintf(gpFile, "Graphics Card Memory = %dGB\n", (int)ceil(DXGIAdapterDescriptor.DedicatedVideoMemory / 1024.0 / 1024.0 / 1024.0));
        fclose(gpFile);
        // myLog->Logger("Graphics Card Name = %s\nGraphics Card Memory = %dGB :: %ls\n", str, (int)ceil(DXGIAdapterDescriptor.DedicatedVideoMemory / 1024.0 / 1024.0 / 1024.0), __FUNCTIONW__);
    }
    else
    {
        fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "IDXGIFactory::EnumAdapters() Failed...\n");
		fclose(gpFile);
        // myLog->Logger("IDXGIFactory::EnumAdapters() Failed :: %ls\n", __FUNCTIONW__);
    }

    if (pIDXGIAdapter)
    {
        pIDXGIAdapter->Release();
        pIDXGIAdapter = NULL;
    }

    if (pIDXGIFactory)
    {
        pIDXGIFactory->Release();
        pIDXGIFactory = NULL;
    }

    return (hr);
}

HRESULT resize(int width, int height)
{
    // variable declarations
    HRESULT hr = S_OK;

    // code
    if (height == 0)
        height = 1;

    // grass.camera.updateResolution(width, height);

    // 1. Release existing Render Target View [RTV]
    if (pID3D11RenderTargetView)
    {
        pID3D11RenderTargetView->Release();
        pID3D11RenderTargetView = NULL;
    }

    // Uninitialize Old DSV
	if (gpID3D11DepthStencilView)
	{
		gpID3D11DepthStencilView->Release();
		gpID3D11DepthStencilView = NULL;
	}

    // 2. Tell SwapChain to resize Buffer according to new Width and Height
    pIDXGISwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

    // 3. Get new resized Buffer from SwapChain in to a DUMMY Texture
    ID3D11Texture2D *pID3D11Texture2D_backbuffer = NULL;
    pIDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pID3D11Texture2D_backbuffer);
    // 4. Now create a new Render Target View [RTV] using above buffer
    hr = pID3D11Device->CreateRenderTargetView(pID3D11Texture2D_backbuffer, NULL, &pID3D11RenderTargetView);
    if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "resize() Failed ...\n");
		fclose(gpFile);
        // myLog->Logger("resize() Failed :: %ls\n", __FUNCTIONW__);
		return (hr);
	}
    // Release DUMMY texture
    pID3D11Texture2D_backbuffer->Release();
    pID3D11Texture2D_backbuffer = NULL;

    // initialize texture2D descriptor
    D3D11_TEXTURE2D_DESC d3d11Texture2DDescriptor;
    ZeroMemory((void*)&d3d11Texture2DDescriptor, sizeof(D3D11_TEXTURE2D_DESC));
    d3d11Texture2DDescriptor.Width = (UINT)width;
    d3d11Texture2DDescriptor.Height = (UINT)height;
    d3d11Texture2DDescriptor.ArraySize = 1;
    d3d11Texture2DDescriptor.MipLevels = 1;
    d3d11Texture2DDescriptor.SampleDesc.Count = 1;
    d3d11Texture2DDescriptor.SampleDesc.Quality = 0;
    d3d11Texture2DDescriptor.Usage = D3D11_USAGE_DEFAULT;
    d3d11Texture2DDescriptor.Format = DXGI_FORMAT_D32_FLOAT;
    d3d11Texture2DDescriptor.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    d3d11Texture2DDescriptor.CPUAccessFlags = 0;
    d3d11Texture2DDescriptor.MiscFlags = 0;

    // create 2D texture as local Depth Buffer
    ID3D11Texture2D *pID3D11Texture2D_DepthBuffer = NULL;
    hr = pID3D11Device->CreateTexture2D(&d3d11Texture2DDescriptor, NULL, &pID3D11Texture2D_DepthBuffer);
    if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "pID3D11Device::CreateTexture2D() Failed ...\n");
		fclose(gpFile);
        // myLog->Logger("pID3D11Device::CreateTexture2D() Failed :: %ls\n", __FUNCTIONW__);
		return(hr);
	}

    // initialize newly created Depth Stencil View Descriptor
    D3D11_DEPTH_STENCIL_VIEW_DESC d3d11DepthStencilViewDescriptor;
    ZeroMemory((void *)&d3d11DepthStencilViewDescriptor, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
    d3d11DepthStencilViewDescriptor.Format = DXGI_FORMAT_D32_FLOAT;
    d3d11DepthStencilViewDescriptor.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
    d3d11DepthStencilViewDescriptor.Texture2D.MipSlice = 0;

    hr = pID3D11Device->CreateDepthStencilView(pID3D11Texture2D_DepthBuffer, &d3d11DepthStencilViewDescriptor, &gpID3D11DepthStencilView);
    if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "pID3D11Device::CreateDepthStencilView() Failed ...\n");
		fclose(gpFile);
        // myLog->Logger("pID3D11Device::CreateDepthStencilView() Failed :: %ls\n", __FUNCTIONW__);
		pID3D11Texture2D_DepthBuffer->Release();
		pID3D11Texture2D_DepthBuffer = NULL;
		return(hr);
	}

    // uninitialize the local Depth Buffer
    pID3D11Texture2D_DepthBuffer->Release();
    pID3D11Texture2D_DepthBuffer = NULL;

    // 5. Set this new RTV in the pipeline
    pID3D11DeviceContext->OMSetRenderTargets(1, &pID3D11RenderTargetView, gpID3D11DepthStencilView);    // OM = Output Merger

    // 6. Initialize Viewport structure
    D3D11_VIEWPORT d3d11Viewport;
    d3d11Viewport.TopLeftX = 0.0f;
    d3d11Viewport.TopLeftY = 0.0f;
    d3d11Viewport.Width = (float)width;
    d3d11Viewport.Height = (float)height;
    d3d11Viewport.MinDepth = 0.0f;
    d3d11Viewport.MaxDepth = 1.0f;

    // 7. Tell device context to Set this Viewport in the pipeline
    pID3D11DeviceContext->RSSetViewports(1, &d3d11Viewport);

    // 8/ Set Projection Matrix
    PerspectiveProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), (float)width/(float)height, 0.01f, 1000.0f);
    // PerspectiveProjectionMatrix = XMMatrixPerspectiveFovLH(0.785398163f, (float)width/(float)height, 0.01f, 100.0f);
    // for (int i=0; i < 16 ; i++)
    // {
    //     fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
    //     fprintf(gpFile, "PerspectiveProjectionMatrix[%d] = %lf\n", i, PerspectiveProjectionMatrix._11);
    //     fclose(gpFile);
    // }
    g_width = width;
    g_height = height;

    return (hr);
}

void display(void)
{
    float elapsedTime = sdkGetTimerValue(&timer) / 1000.0f;


   // 1. Clear RTV using clearColor 
    pID3D11DeviceContext->ClearRenderTargetView(pID3D11RenderTargetView, clearColor);
    pID3D11DeviceContext->ClearDepthStencilView(gpID3D11DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0); // 2nd and 3rd para are analogus to glCrearDepth

    // grass->RenderFrame(elapsedTime, camera.getViewMatrix());
    // model_s.RenderFrame(elapsedTime, camera.getViewMatrix());
    water->RenderFrame(camera.getViewMatrix(), pID3D11RenderTargetView, gpID3D11DepthStencilView);
    // waterQUad->m_ProjectionMatrix = PerspectiveProjectionMatrix;
    // waterQUad->RenderObject();
    // waterNew->RenderFrame();
    // effect_noise->RenderFrame(camera.getViewMatrix());
    // effect_noise->Update();

    // 3. Swap buffers by presenting them
    pIDXGISwapChain->Present(0, 0);

        // model.RenderFrame();
}

void update(void)
{
    // code
}

void uninitialize(void)
{
    // function declarations
    void ToggleFullScreen(void);

    // code
    
    if (timer)
    {
        sdkStopTimer(&timer);
        sdkDeleteTimer(&timer);
        timer = NULL;
    }

    

    if (pID3D11RenderTargetView)
    {
        pID3D11RenderTargetView->Release();
        pID3D11RenderTargetView = NULL;
    }
    
    if (pIDXGISwapChain)
    {
        pIDXGISwapChain->Release();
        pIDXGISwapChain = NULL;
    }

    if (pID3D11DeviceContext)
    {
        pID3D11DeviceContext->Release();
        pID3D11DeviceContext = NULL;
    }

    if (pID3D11Device)
    {
        pID3D11Device->Release();
        pID3D11Device = NULL;
    }

    if (gbFullScreen)
    {
        ToggleFullScreen();
    }

    if (ghwnd)
    {
        DestroyWindow(ghwnd);
        ghwnd = NULL;
    }

    if (gpFile)
    {
        fprintf(gpFile, "Log File Successfully Closed\n");
        fclose(gpFile);
        gpFile = NULL;
    }
}
