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
#include <DirectXMath.h>

#include "src/Common/helper_timer.h"
#include "src/Common/CameraClass.h"
#include "src/Common/Camera.h"
#include "src/Common/rendertextureclass.h"
#include "src/Common/DXShaders.h"

#include "src/Effects/Water/ReflectionShaderClass.h"
#include "src/Effects/Water/WaterClass.h"
#include "src/Effects/Water/WaterShaderClass.h"

#include "src/Effects/Terrain/terrainclass.h"
#include "src/Effects/Terrain/terrainshaderclass.h"

#include "src/Effects/Cube/CubeClass.h"
#include "src/Effects/Cube/CubeShaderClass.h"

// #pragma comment(linker, "/NODEFAULTLIB:LIBCMT")
// #pragma comment(linker, "/DEFAULTLIB:MSVCRT")

// #pragma comment(linker, "/NODEFAULTLIB:MSVCRT")
// #pragma comment(linker, "/DEFAULTLIB:LIBCMT")


// D3D libraries
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
// #pragma comment(lib, "DirectXTK.lib")
// #pragma comment (lib, "Sphere.lib")

// global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


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
ID3D11RasterizerState *gpID3D11RasterizerStateNoCulling = NULL;
ID3D11DepthStencilView *gpID3D11DepthStencilView = NULL; 

ID3D11DepthStencilState* m_depthStencilState;
ID3D11DepthStencilState* m_depthDisabledStencilState;
ID3D11BlendState* m_alphaEnableBlendingState;
ID3D11BlendState* m_alphaDisableBlendingState;
ID3D11BlendState* m_alphaBlendState2;

ID3D11RenderTargetView* m_renderTargetView;
ID3D11DepthStencilView* m_depthStencilView;

float g_width = 0.0f;
float g_height = 0.0f;


DirectX::XMMATRIX PerspectiveProjectionMatrix;



CameraClass* m_Camera;

Camera myCam;

TerrainClass* m_Terrain;
TerrainShaderClass* m_TerrainShader;

CubeClass* m_Cube;
CubeShaderClass* m_CubeShader;

RenderTextureClass *m_RefractionTexture, *m_ReflectionTexture;
ReflectionShaderClass* m_ReflectionShader;
WaterClass* m_Water;
WaterShaderClass* m_WaterShader;

// Timer 
StopWatchInterface *timer = NULL;

// for translation
float objX = 0.0f, objY = 0.0f, objZ = 0.0f;

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

    // Initialize COM
	hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "CoInitialize() Failed...\n");
		fclose(gpFile);
	}
	else
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "CoInitialize() Succeded...\n");
		fclose(gpFile);
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
        myCam.keyboardInputs(wParam);
        switch (wParam)
        {
        case 'f':
        case 'F':
            ToggleFullScreen();
            break;
		case 'x':
			objX -= 1.0f;
            break;
        case 'X':
			objX += 1.0f;
            break;
		case 'y':
			objY -= 1.0f;
            break;
        case 'Y':
			objY += 1.0f;
            break;
		case 'z':
			objZ -= 1.0f;
            break;	
        case 'Z':
			objZ += 1.0f;
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
        myCam.mouseInputs(LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_MOUSEWHEEL:
        myCam.mouseScroll(GET_WHEEL_DELTA_WPARAM(wParam));
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


HRESULT initialize(void)
{
    // function declarations
    HRESULT PrintD3DInfo(void);
    HRESULT resize(int width, int height);

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
    }
    else
    {
        fopen_s(&gpFile, gszLogFilePathName, "a+");
        // print the obtained driver types
        if (d3dDriverType == D3D_DRIVER_TYPE_HARDWARE)
        {
            fprintf(gpFile, "D3D11 Obtained Hardware Driver...\n");
        }
        else if (d3dDriverType == D3D_DRIVER_TYPE_WARP)
        {
            fprintf(gpFile, "D3D11 Obtained WARP Driver...\n");
        }
        else if (d3dDriverType == D3D_DRIVER_TYPE_REFERENCE)
        {
            fprintf(gpFile, "D3D11 Obtained Referance Driver...\n");
        }
        else
        {
            fprintf(gpFile, "D3D11 Obtained UNKNOWN Driver...\n");
        }

        // print obtained D3D feature level
        if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_11_0)
        {
            fprintf(gpFile, "D3D11 Obtained 11.0 Feature Level...\n");
        }
        else if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_1)
        {
            fprintf(gpFile, "D3D11 Obtained 10.1 Feature Level...\n");
        }
        else if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_0)
        {
            fprintf(gpFile, "D3D11 Obtained 10.0 Feature Level...\n");
        }
        else
        {
            fprintf(gpFile, "D3D11 Obtained UNKNOWN Feature Level...\n");
        }
        fclose(gpFile);
    }


    // Initialize the description of the stencil state.
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil state.
	hr = pID3D11Device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if(FAILED(hr))
	{
		return false;
	}

	// Set the depth stencil state.
	pID3D11DeviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
    
    // Clear the second depth stencil state before setting the parameters.
    D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
	// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the state using the device.
	hr = pID3D11Device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);
	if(FAILED(hr))
	{
		return false;
	}
    

    
    // Initialize clear color array
    clearColor[0] = 0.4f;
    clearColor[1] = 0.4f;
    clearColor[2] = 0.4f;
    clearColor[3] = 1.0f;

    // Enabling rasterizer State
    // A. initialize Rasterizer Descriptor
    D3D11_RASTERIZER_DESC d3d11RasterizerDescriptor;
    ZeroMemory((void *)&d3d11RasterizerDescriptor, sizeof(D3D11_RASTERIZER_DESC));
    d3d11RasterizerDescriptor.CullMode = D3D11_CULL_BACK;
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

    // No culling
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
    hr = pID3D11Device->CreateRasterizerState(&d3d11RasterizerDescriptor, &gpID3D11RasterizerStateNoCulling);
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

    // Clear the blend state description.
    D3D11_BLEND_DESC blendStateDescription;
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// Create an alpha enabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// Create the blend state using the description.
	hr = pID3D11Device->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState);
	if(FAILED(hr))
	{
        fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "pID3D11Device::CreateBlendState() failed for AlphaEnable ...\n");
		fclose(gpFile);
		return hr;
	}
    else
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "pID3D11Device::CreateBlendState() Succedded for AlphaEnable ...\n");
		fclose(gpFile);
	}

	// Modify the description to create an alpha disabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;

	// Create the second blend state using the description.
	hr = pID3D11Device->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState);
	if(FAILED(hr))
	{
        
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "pID3D11Device::CreateBlendState() failed for AlphaDisable ...\n");
		fclose(gpFile);
		return hr;
	}
    else
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "pID3D11Device::CreateBlendState() Succedded for AlphaDisable ...\n");
		fclose(gpFile);
	}

	// Create a secondary alpha blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// Create the blend state using the description.
	hr = pID3D11Device->CreateBlendState(&blendStateDescription, &m_alphaBlendState2);
	if(FAILED(hr))
	{
        fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "pID3D11Device::CreateBlendState() failed for m_alphaBlendState2 ...\n");
		fclose(gpFile);
		return hr;
	}
     else
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "pID3D11Device::CreateBlendState() Succedded for m_alphaBlendState2 ...\n");
		fclose(gpFile);
	}

	// Setting up camera
	float _position[] = {0.0, 0.0, 10.0};
	myCam = Camera(WIN_WIDTH, WIN_HEIGHT, _position);

    // initialize Camera
	// Create the camera object.
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
        fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "Camera Class init failed ...\n");
		fclose(gpFile);
		// return false;
	}
    else
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "Camera Class init Succedded  ...\n");
		fclose(gpFile);
	}

	// Initialize a base view matrix with the camera for 2D user interface rendering.
	m_Camera->SetPosition(0.0f, 5.0f, -22.0f);
	m_Camera->GenerateBaseViewMatrix();
    DirectX::XMMATRIX baseViewMatrix;
	m_Camera->GetBaseViewMatrix(baseViewMatrix);

    // setting projection matrix to identity
    PerspectiveProjectionMatrix = DirectX::XMMatrixIdentity();
    

    //-------------------- Initialize Effects --------------------
    // Terrain
    // Create the terrain object.
	m_Terrain = new TerrainClass;
	if(!m_Terrain)
	{
        fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "TerrainClass class init failed ...\n");
		fclose(gpFile);
		// return false;
	}

	// Initialize the terrain object.
	bool result = m_Terrain->Initialize(pID3D11Device, pID3D11DeviceContext, "./src/assets/textures/hm.bmp", "./src/assets/textures/cm.bmp", 20.0f, L"./src/assets/textures/dirt.png", 
								   L"./src/assets/textures/normal.png");
	if(!result)
	{
        fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "Could not initialize the terrain object ...\n");
		fclose(gpFile);
		// MessageBox(ghwnd, L"Could not initialize the terrain object.", L"Error", MB_OK);
		// return false;
	}

	// Create the terrain shader object.
	m_TerrainShader = new TerrainShaderClass;
	if(!m_TerrainShader)
	{
        fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "TerrainShaderClass class init failed ...\n");
		fclose(gpFile);
		// return false;
	}

	// Initialize the terrain shader object.
	result = m_TerrainShader->Initialize(pID3D11Device, ghwnd);
	if(!result)
	{
        fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "Could not initialize the terrain shader object ...\n");
		fclose(gpFile);
		// MessageBox(ghwnd, L"Could not initialize the terrain shader object.", L"Error", MB_OK);
		// return false;
	}


	// cube
	m_Cube = new CubeClass;
	if(!m_Cube)
	{
        fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "CubeClass class init failed ...\n");
		fclose(gpFile);
		// return false;
	}
	// Initialize the Cube object.
	result = m_Cube->Initialize(pID3D11Device, pID3D11DeviceContext);
	if(!result)
	{
        fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "Could not initialize the Cube object ...\n");
		fclose(gpFile);
		// return false;
	}

	// Create the Cube shader object.
	m_CubeShader = new CubeShaderClass;
	if(!m_CubeShader)
	{
        fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "CubeShaderClass class init failed ...\n");
		fclose(gpFile);
		// return false;
	}
	// Initialize the cube shader object.
	result = m_CubeShader->Initialize(pID3D11Device, ghwnd);
	if(!result)
	{
        fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "Could not initialize the Cube shader object ...\n");
		fclose(gpFile);
		// MessageBox(ghwnd, L"Could not initialize the terrain shader object.", L"Error", MB_OK);
		// return false;
	}



    // Water
   // Create the refraction render to texture object.
	m_RefractionTexture = new RenderTextureClass;
	if(!m_RefractionTexture)
	{
        fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "Refraction class init failed ...\n");
		fclose(gpFile);
		// return false;
	}
    else
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "Refraction Class init Succedded  ...\n");
		fclose(gpFile);
	}

	// Initialize the refraction render to texture object.
	result = m_RefractionTexture->Initialize(pID3D11Device, WIN_WIDTH, WIN_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		// MessageBox(ghwnd, L"Could not initialize the refraction render to texture object.", L"Error", MB_OK);
        fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "Could not initialize the refraction render to texture object....\n");
		fclose(gpFile);
		// return false;
	}
    else
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "refraction render to texture object init Succedded  ...\n");
		fclose(gpFile);
	}

	// Create the reflection render to texture object.
	m_ReflectionTexture = new RenderTextureClass;
	if(!m_ReflectionTexture)
	{
        fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "Reflection Texture failed...\n");
		fclose(gpFile);
		return false;
	}
    else
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "Reflection Texture Succedded  ...\n");
		fclose(gpFile);
	}

	// Initialize the reflection render to texture object.
	result = m_ReflectionTexture->Initialize(pID3D11Device,  WIN_WIDTH, WIN_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		// MessageBox(ghwnd, L"Could not initialize the reflection render to texture object.", L"Error", MB_OK);
        fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "Could not initialize the reflection render to texture object.....\n");
		fclose(gpFile);
		// return false;
	}
    else
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, " reflection render to texture object Succedded  ...\n");
		fclose(gpFile);
	}

	// Create the reflection shader object.
	m_ReflectionShader = new ReflectionShaderClass;
	if(!m_ReflectionShader)
	{
        fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "Reflection Shader init failed ...\n");
		fclose(gpFile);
		// return false;
	}
     else
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, " Reflection Shader init Succedded  ...\n");
		fclose(gpFile);
	}

	// Initialize the reflection shader object.
	result = m_ReflectionShader->Initialize(pID3D11Device, ghwnd);
	if(!result)
	{
		// MessageBox(ghwnd, L"Could not initialize the reflection shader object.", L"Error", MB_OK);
        fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "Could not initialize the reflection shader object......\n");
		fclose(gpFile);
		// return false;
	}
    else
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "reflection shader object Succedded  ...\n");
		fclose(gpFile);
	}

	// Create the water object.
	m_Water = new WaterClass;
	if(!m_Water)
	{
        fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "Water Class init failed ...\n");
		fclose(gpFile);
		// return false;
	}
    else
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "Water Class init Succedded  ...\n");
		fclose(gpFile);
	}

	// Initialize the water object.
	result = m_Water->Initialize(pID3D11Device, pID3D11DeviceContext, L"./src/assets/textures/waterNormalMap.png", 3.75f, 110.0f);
	if(!result)
	{
		// MessageBox(ghwnd, L"Could not initialize the water object.", L"Error", MB_OK);
        fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "Could not initialize the water object.......\n");
		fclose(gpFile);
		// return false;
	}
    else
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "Water Object init Succedded  ...\n");
		fclose(gpFile);
	}

	// Create the water shader object.
	m_WaterShader = new WaterShaderClass;
	if(!m_WaterShader)
	{
         fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "Water Shader Class init failed ...\n");
		fclose(gpFile);
		// return false;
	}
    else
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "Water Shader Class init Succedded  ...\n");
		fclose(gpFile);
	}

	// Initialize the water shader object.
	result = m_WaterShader->Initialize(pID3D11Device, ghwnd);
	if(!result)
	{
		// MessageBox(ghwnd, L"Could not initialize the water shader object.", L"Error", MB_OK);
        fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "Could not initialize the water shader object.......\n");
		fclose(gpFile);
		// return false;
	}
    else
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "Water Shader object init Succedded  ...\n");
		fclose(gpFile);
	}


    // Timer initialization
    sdkCreateTimer(&timer);
    sdkStartTimer(&timer);

    // warm-up resize
    hr = resize(WIN_WIDTH, WIN_HEIGHT);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "initialize()::resize() Failed ...\n");
		fclose(gpFile);
        // myLog->Logger("ininitialize()::resize() Failed :: %ls\n", __FUNCTIONW__);
		return (hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "initialize()::resize() Successfull ...\n");
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
		return (hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "CreateDXGIFactory() Succeded...\n");
		fclose(gpFile);
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
    }
    else
    {
        fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "IDXGIFactory::EnumAdapters() Failed...\n");
		fclose(gpFile);
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

DirectX::XMMATRIX OrthographicProjectionMatrix;
D3D11_VIEWPORT d3d11Viewport;
HRESULT resize(int width, int height)
{
    // variable declarations
    HRESULT hr = S_OK;

    // code
    if (height == 0)
        height = 1;

	myCam.updateResolution(width, height);

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
    // D3D11_VIEWPORT d3d11Viewport;
    d3d11Viewport.TopLeftX = 0.0f;
    d3d11Viewport.TopLeftY = 0.0f;
    d3d11Viewport.Width = (float)width;
    d3d11Viewport.Height = (float)height;
    d3d11Viewport.MinDepth = 0.0f;
    d3d11Viewport.MaxDepth = 1.0f;

    // 7. Tell device context to Set this Viewport in the pipeline
    pID3D11DeviceContext->RSSetViewports(1, &d3d11Viewport);

    // 8. Set Projection Matrix
    PerspectiveProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(45.0f), (float)width/(float)height, 0.01f, 1000.0f);
    OrthographicProjectionMatrix = DirectX::XMMatrixOrthographicLH((float)width, (float)height, 0.01f, 1000.0f);

    g_width = width;
    g_height = height;

    return (hr);
}

void display(void)
{
    // function delcaration
    bool Render(float);
    void RenderRefractionToTexture();
    void RenderReflectionToTexture(float);
	bool RenderTerrain();


    // local variables
    float elapsedTime = sdkGetTimerValue(&timer) / 1000.0f;

	
    // Do the water frame processing.
	m_Water->Frame();

    // Render the refraction of the scene to a texture.
	RenderRefractionToTexture();

	// Render the reflection of the scene to a texture.
	RenderReflectionToTexture(elapsedTime);


    // code
    // 1. Clear RTV using clearColor 
    pID3D11DeviceContext->ClearRenderTargetView(pID3D11RenderTargetView, clearColor);
    pID3D11DeviceContext->ClearDepthStencilView(gpID3D11DepthStencilView, D3D11_CLEAR_DEPTH , 1.0f, 0); // 2nd and 3rd para are analogus to glCrearDepth
	// pID3D11DeviceContext->ClearRenderTargetView(m_renderTargetView, clearColor);
    // pID3D11DeviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0); // 2nd and 3rd para are analogus to glCrearDepth

	// RenderTerrain();

    bool result = Render(elapsedTime);
	if(!result)
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
        fprintf(gpFile, "Render() Failed ...\n");
        fclose(gpFile);
	}
    else
    {
        fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
        fprintf(gpFile, "Render() Successfull ...\n");
        fclose(gpFile);
    }
    // 3. Swap buffers by presenting them
    pIDXGISwapChain->Present(0, 0);
}

void EnableSecondBlendState()
{
	float blendFactor[4];
	

	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	
	// Turn on the alpha blending.
	pID3D11DeviceContext->OMSetBlendState(m_alphaBlendState2, blendFactor, 0xffffffff);

	return;
}

void TurnOnAlphaBlending()
{
	float blendFactor[4];
	

	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	
	// Turn on the alpha blending.
	pID3D11DeviceContext->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);

	return;
}


void TurnOffAlphaBlending()
{
	float blendFactor[4];
	

	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	
	// Turn off the alpha blending.
	pID3D11DeviceContext->OMSetBlendState(m_alphaDisableBlendingState, blendFactor, 0xffffffff);

	return;
}

void SetBackBufferRenderTarget()
{
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	pID3D11DeviceContext->OMSetRenderTargets(1, &pID3D11RenderTargetView, gpID3D11DepthStencilView);

	return;
}

void ResetViewport()
{
	// Set the viewport.
    pID3D11DeviceContext->RSSetViewports(1, &d3d11Viewport);

	return;
}


// render

bool RenderTerrain()
{
	DirectX::XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	DirectX::XMFLOAT3 cameraPosition;

	// float color[4];

	// // Setup the color to clear the buffer to.
	// color[0] = 0.0f;
	// color[1] = 0.0f;
	// color[2] = 0.0f;
	// color[3] = 1.0f;

	// // Clear the back buffer.
	// pID3D11DeviceContext->ClearRenderTargetView(m_renderTargetView, color);
	// // pID3D11DeviceContext->ClearRenderTargetView(pID3D11RenderTargetView, color);
    
	// // Clear the depth buffer.
	// pID3D11DeviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	// // pID3D11DeviceContext->ClearDepthStencilView(gpID3D11DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();
	worldMatrix = DirectX::XMMatrixIdentity();
	m_Camera->GetViewMatrix(viewMatrix);
	projectionMatrix = PerspectiveProjectionMatrix;
	cameraPosition = m_Camera->GetPosition();

	// worldMatrix = DirectX::XMMatrixTranslation(cameraPosition.x , cameraPosition.y, cameraPosition.z) * DirectX::XMMatrixTranslation(objX, objY, objZ);;
	worldMatrix = DirectX::XMMatrixTranslation(objX, objY, objZ);
	// Render the terrain using the terrain shader.
	m_Terrain->Render(pID3D11DeviceContext);
	bool result = m_TerrainShader->Render(pID3D11DeviceContext, m_Terrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
					 m_Terrain->GetColorTexture(), m_Terrain->GetNormalTexture(), DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 1.0f), 
					 2.0f);
	if(!result)
	{
		return false;
	}
	return true;
}

bool Render(float angle)
{
	DirectX::XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix, baseViewMatrix, reflectionViewMatrix;
	bool result;
	// DirectX::XMVECTOR cameraPosition;
	DirectX::XMFLOAT3 cameraPosition;

	// // Clear the scene.
	// // m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
    // float color[4];

	// // Setup the color to clear the buffer to.
	// color[0] = 0.0f;
	// color[1] = 0.0f;
	// color[2] = 0.0f;
	// color[3] = 1.0f;

	// // Clear the back buffer.
	// // pID3D11DeviceContext->ClearRenderTargetView(m_renderTargetView, color);
	// pID3D11DeviceContext->ClearRenderTargetView(pID3D11RenderTargetView, color);
    
	// // Clear the depth buffer.
	// // pID3D11DeviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	// pID3D11DeviceContext->ClearDepthStencilView(gpID3D11DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Generate the view matrix based on the camera's position.
	/*m_Camera->Render();*/

	// Generate the reflection matrix based on the camera's position and the height of the water.
	/*m_Camera->RenderReflection(m_Water->GetWaterHeight());*/

	// Get the world, view, projection, ortho, and base view matrices from the camera and Direct3D objects.
	// m_Direct3D->GetWorldMatrix(worldMatrix);
	worldMatrix = DirectX::XMMatrixIdentity();
	// m_Camera->GetViewMatrix(viewMatrix);
	viewMatrix = myCam.getViewMatrix();
    projectionMatrix = PerspectiveProjectionMatrix;
    orthoMatrix = OrthographicProjectionMatrix;
	// m_Camera->GetBaseViewMatrix(baseViewMatrix);
	// m_Camera->GetReflectionViewMatrix(reflectionViewMatrix);
	reflectionViewMatrix = myCam.getViewMatrixInv(m_Water->GetWaterHeight());
	myCam.invertPitch();

	// Get the position of the camera.
	/*cameraPosition = m_Camera->GetPosition();*/
	cameraPosition = DirectX::XMFLOAT3(DirectX::XMVectorGetX(myCam.getEye()), DirectX::XMVectorGetY(myCam.getEye()), DirectX::XMVectorGetZ(myCam.getEye()));

	// Translate the sky dome to be centered around the camera position.
    worldMatrix = DirectX::XMMatrixTranslation(cameraPosition.x, cameraPosition.y, cameraPosition.z);

	// Turn off back face culling and the Z buffer.
	// m_Direct3D->TurnOffCulling();
    pID3D11DeviceContext->RSSetState(gpID3D11RasterizerState);
	// m_Direct3D->TurnZBufferOff();
    pID3D11DeviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);

	// // Render the sky dome using the sky dome shader.
	// m_SkyDome->Render(m_Direct3D->GetDeviceContext());
	// m_SkyDomeShader->Render(m_Direct3D->GetDeviceContext(), m_SkyDome->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
	// 			m_SkyDome->GetApexColor(), m_SkyDome->GetCenterColor());

	// Turn back face culling back on.
	// m_Direct3D->TurnOnCulling();
    pID3D11DeviceContext->RSSetState(gpID3D11RasterizerStateNoCulling);

	// Enable additive blending so the clouds blend with the sky dome color.
	EnableSecondBlendState();

	// // Render the sky plane using the sky plane shader.
	// m_SkyPlane->Render(m_Direct3D->GetDeviceContext());
	// m_SkyPlaneShader->Render(m_Direct3D->GetDeviceContext(), m_SkyPlane->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
	// 			 m_SkyPlane->GetCloudTexture(), m_SkyPlane->GetPerturbTexture(), m_SkyPlane->GetTranslation(), m_SkyPlane->GetScale(), 
	// 			 m_SkyPlane->GetBrightness());

	// Turn off blending.
	TurnOffAlphaBlending();

	// Turn the Z buffer back on.
	// m_Direct3D->TurnZBufferOn();
    pID3D11DeviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	//cube above water
	worldMatrix = DirectX::XMMatrixIdentity();
	worldMatrix = DirectX::XMMatrixScaling(4.0f, 4.0f, 4.0f) * DirectX::XMMatrixRotationY(angle) * DirectX::XMMatrixTranslation(200.0f + objX, objY + 7.0f, 225.0f + objZ); 
	m_Cube->Render(pID3D11DeviceContext);
	result = m_CubeShader->Render(pID3D11DeviceContext, m_Cube->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
				 DirectX::XMFLOAT3(0.0f, -1.0f, -1.0f), 2.0f);
	if(!result)
	{
		return false;
	}

	// Reset the world matrix.
	// m_Direct3D->GetWorldMatrix(worldMatrix);
    worldMatrix = DirectX::XMMatrixIdentity();
	worldMatrix = DirectX::XMMatrixTranslation(objX, objY, objZ);
	// Render the terrain using the terrain shader.
	m_Terrain->Render(pID3D11DeviceContext);
	result = m_TerrainShader->Render(pID3D11DeviceContext, m_Terrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
					 m_Terrain->GetColorTexture(), m_Terrain->GetNormalTexture(), DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, -1.0f, -1.0f), 
					 2.0f);
	if(!result)
	{
		return false;
	}

	// Translate to the location of the water and render it.
	// worldMatrix = DirectX::XMMatrixTranslation(240.0f - 100.0f, m_Water->GetWaterHeight(), 250.0f);
	worldMatrix = DirectX::XMMatrixTranslation(240.0f + objX, m_Water->GetWaterHeight() + objY, 250.0f + objZ);
	// worldMatrix = DirectX::XMMatrixTranslation(20.0f, m_Water->GetWaterHeight() , 20.0f);
	m_Water->Render(pID3D11DeviceContext);
	m_WaterShader->Render(pID3D11DeviceContext, m_Water->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, reflectionViewMatrix, 
			      m_RefractionTexture->GetShaderResourceView(), m_ReflectionTexture->GetShaderResourceView(), m_Water->GetTexture(), 
			      /*m_Camera->GetPosition()*/ DirectX::XMFLOAT3(DirectX::XMVectorGetX(myCam.getEye()), DirectX::XMVectorGetY(myCam.getEye()), DirectX::XMVectorGetZ(myCam.getEye())), 
				  m_Water->GetNormalMapTiling(), m_Water->GetWaterTranslation(), m_Water->GetReflectRefractScale(),
			      m_Water->GetRefractionTint(), DirectX::XMFLOAT3(0.0f, -1.0f, -1.0f), m_Water->GetSpecularShininess());

	// Reset the world matrix.
	// m_Direct3D->GetWorldMatrix(worldMatrix);
    worldMatrix = DirectX::XMMatrixIdentity();

	// Turn off the Z buffer to begin all 2D rendering.
	// m_Direct3D->TurnZBufferOff();
    pID3D11DeviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);

	// Turn on the alpha blending before rendering the text.
	TurnOnAlphaBlending();

	// // Render the text user interface elements.
	// result = m_Text->Render(m_Direct3D->GetDeviceContext(), m_FontShader, worldMatrix, orthoMatrix);
	// if(!result)
	// {
	// 	return false;
	// }

	// Turn off alpha blending after rendering the text.
	TurnOffAlphaBlending();

	// Turn the Z buffer back on now that all 2D rendering has completed.
	// m_Direct3D->TurnZBufferOn();
    pID3D11DeviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	// Present the rendered scene to the screen.
	// m_Direct3D->EndScene();

	return true;
}


void RenderRefractionToTexture()
{
	DirectX::XMVECTOR clipPlane;
	DirectX::XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
    worldMatrix = DirectX::XMMatrixIdentity();

	// Setup a clipping plane based on the height of the water to clip everything above it to create a refraction.
	clipPlane = DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, m_Water->GetWaterHeight() + 0.1f);

	// Set the render target to be the refraction render to texture.
	m_RefractionTexture->SetRenderTarget(pID3D11DeviceContext);

	// Clear the refraction render to texture.
	m_RefractionTexture->ClearRenderTarget(pID3D11DeviceContext, 0.2f, 0.2f, 0.2f, 1.0f);

	// Generate the view matrix based on the camera's position.
	/*m_Camera->Render();*/

	// Get the matrices from the camera and d3d objects.
	// m_Direct3D->GetWorldMatrix(worldMatrix);
	/*m_Camera->GetViewMatrix(viewMatrix);*/
	viewMatrix = myCam.getViewMatrix();
    projectionMatrix = PerspectiveProjectionMatrix;
	// m_Direct3D->GetProjectionMatrix(projectionMatrix);
	worldMatrix = DirectX::XMMatrixTranslation(objX, objY, objZ);
	// Render the terrain using the reflection shader and the refraction clip plane to produce the refraction effect.
	m_Terrain->Render(pID3D11DeviceContext);
	m_ReflectionShader->Render(pID3D11DeviceContext, m_Terrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
							   m_Terrain->GetColorTexture(), m_Terrain->GetNormalTexture(), DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f), DirectX::XMFLOAT3(0.0f, -1.0f, -1.0f), 2.0f, 
							   clipPlane);
	
	// Reset the render target back to the original back buffer and not the render to texture anymore.
	SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	ResetViewport();

	return;
}


void RenderReflectionToTexture(float angle)
{
	DirectX::XMVECTOR clipPlane;
	DirectX::XMMATRIX reflectionViewMatrix, worldMatrix, projectionMatrix;
	DirectX::XMFLOAT3 cameraPosition;


	// Setup a clipping plane based on the height of the water to clip everything below it.
	clipPlane = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, -m_Water->GetWaterHeight());
	
	// Set the render target to be the reflection render to texture.
	m_ReflectionTexture->SetRenderTarget(pID3D11DeviceContext);

	// Clear the reflection render to texture.
	m_ReflectionTexture->ClearRenderTarget(pID3D11DeviceContext, 0.2f, 0.2f, 0.2f, 1.0f);

	// Use the camera to render the reflection and create a reflection view matrix.
	/*m_Camera->RenderReflection(m_Water->GetWaterHeight());*/

	// Get the camera reflection view matrix instead of the normal view matrix.
	/*m_Camera->GetReflectionViewMatrix(reflectionViewMatrix);*/
	reflectionViewMatrix = myCam.getViewMatrixInv(m_Water->GetWaterHeight());
	myCam.invertPitch();

	// Get the world and projection matrices from the d3d object.
	// m_Direct3D->GetWorldMatrix(worldMatrix);
    worldMatrix = DirectX::XMMatrixIdentity();
	// m_Direct3D->GetProjectionMatrix(projectionMatrix);
    projectionMatrix = PerspectiveProjectionMatrix;

	// Get the position of the camera.
	/*cameraPosition = m_Camera->GetPosition();*/
	cameraPosition = DirectX::XMFLOAT3(DirectX::XMVectorGetX(myCam.getEye()), DirectX::XMVectorGetY(myCam.getEye()), DirectX::XMVectorGetZ(myCam.getEye()));

	// Invert the Y coordinate of the camera around the water plane height for the reflected camera position.
	cameraPosition.y = -cameraPosition.y + (m_Water->GetWaterHeight() * 2.0f);

	// Translate the sky dome and sky plane to be centered around the reflected camera position.
	worldMatrix = DirectX::XMMatrixTranslation(cameraPosition.x, cameraPosition.y, cameraPosition.z);

	// Turn off back face culling and the Z buffer.
	// m_Direct3D->TurnOffCulling();
    pID3D11DeviceContext->RSSetState(gpID3D11RasterizerState);
	// m_Direct3D->TurnZBufferOff();
    pID3D11DeviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);

	// // Render the sky dome using the reflection view matrix.
	// m_SkyDome->Render(m_Direct3D->GetDeviceContext());
	// m_SkyDomeShader->Render(m_Direct3D->GetDeviceContext(), m_SkyDome->GetIndexCount(), worldMatrix, reflectionViewMatrix, projectionMatrix, 
	// 						m_SkyDome->GetApexColor(), m_SkyDome->GetCenterColor());


	


	// Enable back face culling.
	// m_Direct3D->TurnOnCulling();
    pID3D11DeviceContext->RSSetState(gpID3D11RasterizerStateNoCulling);

	// Enable additive blending so the clouds blend with the sky dome color.
	EnableSecondBlendState();

	// // Render the sky plane using the sky plane shader.
	// m_SkyPlane->Render(m_Direct3D->GetDeviceContext());
	// m_SkyPlaneShader->Render(m_Direct3D->GetDeviceContext(), m_SkyPlane->GetIndexCount(), worldMatrix, reflectionViewMatrix, projectionMatrix, 
	// 						 m_SkyPlane->GetCloudTexture(), m_SkyPlane->GetPerturbTexture(), m_SkyPlane->GetTranslation(), m_SkyPlane->GetScale(), 
	// 						 m_SkyPlane->GetBrightness());

	
	// Turn off blending and enable the Z buffer again.
	TurnOffAlphaBlending();
	// m_Direct3D->TurnZBufferOn();
    pID3D11DeviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	// Render Cube above water
	worldMatrix = DirectX::XMMatrixIdentity();
	worldMatrix = DirectX::XMMatrixScaling(4.0f, 4.0f, 4.0f) * DirectX::XMMatrixRotationY(angle) * DirectX::XMMatrixTranslation(200.0f + objX, objY + 7.0f, 225.0f + objZ);
	m_Cube->Render(pID3D11DeviceContext);
	// m_ReflectionShader->Render(pID3D11DeviceContext, m_Cube->GetIndexCount(), worldMatrix, reflectionViewMatrix, projectionMatrix, 
	// 						   m_Terrain->GetColorTexture(), m_Terrain->GetNormalTexture(), DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f), DirectX::XMFLOAT3(0.0f, -1.0f, -1.0f), 2.0f, 
	// 						   clipPlane);
	m_CubeShader->Render(pID3D11DeviceContext, m_Cube->GetIndexCount(), worldMatrix, reflectionViewMatrix, projectionMatrix, 
				 DirectX::XMFLOAT3(0.0f, -1.0f, -1.0f), 2.0f);

	// Reset the world matrix.
	// m_Direct3D->GetWorldMatrix(worldMatrix);
    worldMatrix = DirectX::XMMatrixIdentity();
	worldMatrix = DirectX::XMMatrixTranslation(objX, objY, objZ);
	// Render the terrain using the reflection view matrix and reflection clip plane.
	m_Terrain->Render(pID3D11DeviceContext);
	m_ReflectionShader->Render(pID3D11DeviceContext, m_Terrain->GetIndexCount(), worldMatrix, reflectionViewMatrix, projectionMatrix, 
							   m_Terrain->GetColorTexture(), m_Terrain->GetNormalTexture(), DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f), DirectX::XMFLOAT3(0.0f, -1.0f, -1.0f), 2.0f, 
							   clipPlane);

	


	// Reset the render target back to the original back buffer and not the render to texture anymore.
	SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	ResetViewport();

	return;
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

    // Release the water shader object.
	if(m_WaterShader)
	{
		m_WaterShader->Shutdown();
		delete m_WaterShader;
		m_WaterShader = 0;
	}

	// Release the water object.
	if(m_Water)
	{
		m_Water->Shutdown();
		delete m_Water;
		m_Water = 0;
	}

	// Release the reflection shader object.
	if(m_ReflectionShader)
	{
		m_ReflectionShader->Shutdown();
		delete m_ReflectionShader;
		m_ReflectionShader = 0;
	}

	// Release the reflection render to texture object.
	if(m_ReflectionTexture)
	{
		m_ReflectionTexture->Shutdown();
		delete m_ReflectionTexture;
		m_ReflectionTexture = 0;
	}

	// Release the refraction render to texture object.
	if(m_RefractionTexture)
	{
		m_RefractionTexture->Shutdown();
		delete m_RefractionTexture;
		m_RefractionTexture = 0;
	}

    // Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}
    
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
