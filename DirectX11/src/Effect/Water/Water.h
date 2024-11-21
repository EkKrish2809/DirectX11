#ifndef WATER_H
#define WATER_H

// D3D related headers
#include <dxgi.h>
#include <D3D11.h>
#include <d3dcompiler.h>

#pragma warning(disable : 4838)
#include "XNAMath_204/xnamath.h"

#include "../../Common/DXShaders.h"
// #include "../../Common/DDSTextureLoader.h"
#include "../../Common/Camera.h"
#include "../../Common/Framebuffer.h"
#include "../Model/TextureLoader.h"

#include "WaterQuad.h"
#include "TexturedQuad.h"

// #define RENDER_TEXTURE_WIDTH    1024
// #define RENDER_TEXTURE_HEIGHT   1024

#define REFLECTION_FBO_WIDTH 1920
#define REFLECTION_FBO_HEIGHT 1080

#define REFRACTION_FBO_WIDTH 1920
#define REFRACTION_FBO_HEIGHT 1080

#define WATER_WAVE_SPEED 0.2

extern XMMATRIX PerspectiveProjectionMatrix;

// D3D related global variables
extern IDXGISwapChain *pIDXGISwapChain;
extern ID3D11Device *pID3D11Device;
extern ID3D11DeviceContext *pID3D11DeviceContext;
extern ID3D11RenderTargetView *pID3D11RenderTargetView;
extern float clearColor[4];

extern ID3D11RasterizerState *gpID3D11RasterizerState;
extern ID3D11DepthStencilView *gpID3D11DepthStencilView;

extern FILE *gpFile;
extern char gszLogFilePathName[];

class Water
{
public:
    
  

    Camera camera;
    ID3D11VertexShader *gpID3D11VertexShader_WaterQuad = NULL;
    ID3D11PixelShader *gpID3D11PixelShader_WaterQuad = NULL;
    ID3D11InputLayout *gpID3D11InputLayout_WaterQuad = NULL;
    ID3D11VertexShader *gpID3D11VertexShader_WaterBedQuad = NULL;
    ID3D11PixelShader *gpID3D11PixelShader_WaterBedQuad = NULL;
    ID3D11InputLayout *gpID3D11InputLayout_WaterBedQuad = NULL;
    ID3D11Buffer *gpID3D11Buffer_PositionBuffer_WaterQuad = NULL;
    ID3D11Buffer *gpID3D11Buffer_TextureBuffer_WaterQuad = NULL;
    ID3D11Buffer *gpID3D11Buffer_ConstantBuffer_WaterQuad  = NULL;
    ID3D11Buffer *gpID3D11Buffer_PositionBuffer_WaterBedQuad = NULL;
    ID3D11Buffer *gpID3D11Buffer_TextureBuffer_WaterBedQuad = NULL;
    ID3D11Buffer *gpID3D11Buffer_ConstantBuffer_WaterBedQuad  = NULL;
    ID3D11ShaderResourceView *gpID3D11ShaderResourceView_Texture_PS = NULL;
    ID3D11SamplerState *gpID3D11SamplerState_Texture_PS = NULL;

    Framebuffer *fbo_default = NULL;
    Framebuffer *fbo_reflection = NULL;
    Framebuffer *fbo_refraction = NULL;
    WaterQuad *object = NULL;

    TexturedQuad *texQuad = NULL;

    Water() {}

    Water(IDXGISwapChain *SwapChain,
          ID3D11Device *Device,
          ID3D11DeviceContext *DeviceContext,
          ID3D11RenderTargetView *RenderTargetView,
          ID3D11DepthStencilView *DepthStencilView) 
          : m_SwapChain(SwapChain),
          m_Device(Device),
          m_DeviceContext(DeviceContext),
          m_RenderTargetView(RenderTargetView),
          m_DepthStencilView(DepthStencilView)
    {
        shader = new DXShaders;
        fbo_reflection = new Framebuffer();
        fbo_refraction = new Framebuffer();
        
    }

    ~Water() {}


    BOOL initializeWaterQuad(void)
    {

        HRESULT hr = S_OK;
        // vertex shader
        ID3DBlob *pID3DBlob_VertexShaderCode = NULL;
        
        if (!shader->CreateAndCompileVertexShaderObjects("./src/Shaders/Water/WaterQuad_VS.hlsl", VERTEX_SHADER, &gpID3D11VertexShader_WaterQuad, &pID3DBlob_VertexShaderCode))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+");
            fprintf(gpFile, "shader->CreateAndCompileShaderObjects() Failed for Vertex Shader...\n");
            fclose(gpFile);
        }
        // Set this vertex shader in VErtex Shader Stage of Pipeline
        // pID3D11DeviceContext->VSSetShader(gpID3D11VertexShader_WaterQuad,
        //                                 NULL,
        //                                 0);
        

        ID3DBlob *pID3DBlob_PixelShaderCode = NULL;
        if (shader->CreateAndCompilePixelShaderObjects("./src/Shaders/Water/WaterQuad_PS.hlsl", PIXEL_SHADER, &gpID3D11PixelShader_WaterQuad, &pID3DBlob_PixelShaderCode) == FALSE)
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+");
            fprintf(gpFile, "shader->CreateAndCompilePixelShaderObjects() Failed for Pixel Shader...\n");
            fclose(gpFile);
            // return FALSE;
        }
        // Set pixel shader in the Pixel Shader stage of Pipeline
        // m_DeviceContext->PSSetShader(gpID3D11PixelShader_WaterQuad,
        //                                   NULL,
        //                                   0);


        // ############ Input Layout [just like Attributes in OpenGL] ##############
        D3D11_INPUT_ELEMENT_DESC d3d11InputElementDescriptor[2];
        ZeroMemory((void *)&d3d11InputElementDescriptor, sizeof(D3D11_INPUT_ELEMENT_DESC) * _ARRAYSIZE(d3d11InputElementDescriptor));
        // initialize input layout structure
        d3d11InputElementDescriptor[0].SemanticName = "POSITION";
        d3d11InputElementDescriptor[0].SemanticIndex = 0;
        d3d11InputElementDescriptor[0].Format = DXGI_FORMAT_R32G32_FLOAT;
        d3d11InputElementDescriptor[0].AlignedByteOffset = 0;
        d3d11InputElementDescriptor[0].InputSlot = 0;
        d3d11InputElementDescriptor[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        d3d11InputElementDescriptor[0].InstanceDataStepRate = 0;

        d3d11InputElementDescriptor[1].SemanticName = "TEXCOORD";
        d3d11InputElementDescriptor[1].SemanticIndex = 0;
        d3d11InputElementDescriptor[1].Format = DXGI_FORMAT_R32G32_FLOAT;
        d3d11InputElementDescriptor[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        d3d11InputElementDescriptor[1].InputSlot = 1;
        d3d11InputElementDescriptor[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        d3d11InputElementDescriptor[1].InstanceDataStepRate = 0;

        // create input layout
        hr = m_Device->CreateInputLayout(d3d11InputElementDescriptor,
                                         _ARRAYSIZE(d3d11InputElementDescriptor),
                                         pID3DBlob_VertexShaderCode->GetBufferPointer(),
                                         pID3DBlob_VertexShaderCode->GetBufferSize(),
                                         &gpID3D11InputLayout_WaterQuad);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device->CreateInputLayout() Failed ...\n");
            fclose(gpFile);
        }

        // Set this Input Layout in Input Assembly Stages of Pipeline
        // m_DeviceContext->IASetInputLayout(gpID3D11InputLayout_WaterQuad);

        // Release the Blob
        pID3DBlob_VertexShaderCode->Release();
        pID3DBlob_VertexShaderCode = NULL;

        pID3DBlob_PixelShaderCode->Release();
        pID3DBlob_PixelShaderCode = NULL;

        // ############ Geometry ##############
        const float squareVertices[] =
            {
                -1.0f, -1.0f,
                -1.0f, 1.0f,
                1.0f, -1.0f,

                1.0f, -1.0f,
                -1.0f, 1.0f,
                1.0f, 1.0f
            };

        const float squareTexcoords[] =
            {
                // Tex : u,v

                +0.0f,
                +0.0f,
                +0.0f,
                +1.0f,
                +1.0f,
                +0.0f,

                +1.0f,
                +0.0f,
                +0.0f,
                +1.0f,
                +1.0f,
                +1.0f,
            };
        
        // create vertex buffer for above position vertices
        //  A. initialize Buffer Descriptor... like glGenBuffer()
        D3D11_BUFFER_DESC d3d11BufferDescriptor;
        ZeroMemory((void *)&d3d11BufferDescriptor, sizeof(D3D11_BUFFER_DESC));
        d3d11BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
        d3d11BufferDescriptor.ByteWidth = sizeof(float) * _ARRAYSIZE(squareVertices);
        d3d11BufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        // B. initialize subresource data structure to put data into the buffer
        D3D11_SUBRESOURCE_DATA d3d11SubresourceData;
        ZeroMemory((void *)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
        d3d11SubresourceData.pSysMem = squareVertices;

        // C. Create the actual buffer
        hr = pID3D11Device->CreateBuffer(&d3d11BufferDescriptor,
                                         &d3d11SubresourceData,
                                         &gpID3D11Buffer_PositionBuffer_WaterQuad);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device::CreateBuffer() Failed for Position ...\n");
            fclose(gpFile);
            return FALSE;
        }

        // Texture
        ZeroMemory((void *)&d3d11BufferDescriptor, sizeof(D3D11_BUFFER_DESC));
        d3d11BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
        d3d11BufferDescriptor.ByteWidth = sizeof(float) * _ARRAYSIZE(squareTexcoords);
        d3d11BufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        // B. initialize subresource data structure to put data into the buffer
        ZeroMemory((void *)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
        d3d11SubresourceData.pSysMem = squareTexcoords;

        // C. Create the actual buffer
        hr = pID3D11Device->CreateBuffer(&d3d11BufferDescriptor,
                                         &d3d11SubresourceData,
                                         &gpID3D11Buffer_TextureBuffer_WaterQuad);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device::CreateBuffer() Failed for Texture ...\n");
            fclose(gpFile);
            return FALSE;
        }

        // Constant Buffer (for uniforms)
        // A. initialize Buffer descriptor
        ZeroMemory((void *)&d3d11BufferDescriptor, sizeof(D3D11_BUFFER_DESC));
        d3d11BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
        d3d11BufferDescriptor.ByteWidth = sizeof(CBUFFER_WATER);
        d3d11BufferDescriptor.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        d3d11BufferDescriptor.CPUAccessFlags = 0;

        // B. Create Actual Buffer
        hr = m_Device->CreateBuffer(&d3d11BufferDescriptor,
                                         NULL,
                                         &gpID3D11Buffer_ConstantBuffer_WaterQuad);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device::CreateBuffer() Failed for Constants ...\n");
            fclose(gpFile);
            return FALSE;
        }

        // C. set Constant Buffer into the Vertex shader state of pipeline
        // m_DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer_WaterQuad);
        // m_DeviceContext->PSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer_WaterQuad);

        // Create Texture Sampler State
        D3D11_SAMPLER_DESC d3d11SamplerDescriptor;
        ZeroMemory((void *)&d3d11SamplerDescriptor, sizeof(D3D11_SAMPLER_DESC));
        d3d11SamplerDescriptor.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        // d3d11SamplerDescriptor.Filter = D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR;
        d3d11SamplerDescriptor.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        d3d11SamplerDescriptor.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        d3d11SamplerDescriptor.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        d3d11SamplerDescriptor.ComparisonFunc = D3D11_COMPARISON_NEVER;
        d3d11SamplerDescriptor.BorderColor[0] = 1.0f;
        d3d11SamplerDescriptor.BorderColor[1] = 1.0f;
        d3d11SamplerDescriptor.BorderColor[2] = 1.0f;
        d3d11SamplerDescriptor.BorderColor[3] = 1.0f;
        d3d11SamplerDescriptor.MinLOD = 0;
        d3d11SamplerDescriptor.MaxLOD = D3D11_FLOAT32_MAX;
        d3d11SamplerDescriptor.MaxAnisotropy = 1;

        hr = m_Device->CreateSamplerState(&d3d11SamplerDescriptor, &gpID3D11SamplerState_Texture_PS);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device->CreateSamplerState() Failed for Pixel Shader ...\n");
            fclose(gpFile);
            // return (hr);
            return FALSE;
        }

        // reflection
        hr = fbo_reflection->CreateFramebuffer(m_Device, &RTTexture_Reflection, &RTVReflection, &rtSRVReflection, &rtTextureDepth_Reflection, &rtDSVDepth_Reflection, TRUE);
        // bool isReflection = fbo_reflection->InitializeReflectionFBO(m_Device, m_DeviceContext, 1024, 1024, RTTexture_Reflection, RTVReflection, rtDSVDepth_Reflection);
        // if (!isReflection)
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "CreateFramebuffer() Failed ...\n");
            fclose(gpFile);
        }

        // refraction
        hr = fbo_refraction->CreateFramebuffer(m_Device, &RTTexture_Refraction, &RTVRefraction, &rtSRVRefraction, &rtTextureDepth_Refraction, &rtDSVDepth_Refraction, TRUE);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "CreateFramebuffer() Failed ...\n");
            fclose(gpFile);
        }

        // Above Water
        object = new WaterQuad(pIDXGISwapChain, m_Device, m_DeviceContext, pID3D11RenderTargetView);//, myLog);
        if (!object->Initialize())
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "object->Initialize() Failed ...\n");
            fclose(gpFile);
        }

        hr = CreateWICTextureFromFile(m_Device, m_DeviceContext, L"assets/Textures/water/waterDUDV.png", nullptr, &gpID3D11ShaderResourceView_DUDVMap);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "Texture couldn't be loaded : DUDVMap ...\n");
            fclose(gpFile);
        }

        hr = CreateWICTextureFromFile(m_Device, m_DeviceContext, L"assets/Textures/water/waterNormalMap.png", nullptr, &gpID3D11ShaderResourceView_NormalMap);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "Texture couldn't be loaded : NormalMap ...\n");
            fclose(gpFile);
        }
           
        // for debugging
        // m_SwapChain(SwapChain),
        //   m_Device(Device),
        //   m_DeviceContext(DeviceContext),
        //   m_RenderTargetView(RenderTargetView),
        //   m_DepthStencilView(DepthStencilView)
        texQuad = new TexturedQuad(pIDXGISwapChain, m_Device, m_DeviceContext, pID3D11RenderTargetView);
        if (texQuad->Initialize() == FALSE)
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "Debug Quad failed to Initialize()...\n");
            fclose(gpFile);
        }

        // Blending
        D3D11_BLEND_DESC blendDesc;
        ZeroMemory((void*)&blendDesc, sizeof(D3D11_BLEND_DESC));
        blendDesc.AlphaToCoverageEnable = FALSE;
        blendDesc.IndependentBlendEnable = FALSE;
        blendDesc.RenderTarget[0].BlendEnable = TRUE;
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        hr = m_Device->CreateBlendState(&blendDesc, &pBlendStateEnable);
        if (FAILED(hr))
        {
            // AMC::Log::GetInstance()->WriteLogFile(__FUNCTION__, AMC::LOG_ERROR, L"Failed To Create Blend State : %s", AMC::Log::GetInstance()->GetErrorMessage(hr));
            return FALSE;
        }

        return TRUE;
    }

    int initializeWaterBedQuad(void)
    {
        HRESULT hr = S_OK;
        // vertex shader
        ID3DBlob *pID3DBlob_VertexShaderCode = NULL;
        if (shader->CreateAndCompileVertexShaderObjects("./src/Shaders/Water/WaterBedQuad_VS.hlsl", VERTEX_SHADER, &gpID3D11VertexShader_WaterBedQuad, &pID3DBlob_VertexShaderCode) == FALSE)
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+");
            fprintf(gpFile, "shader->CreateAndCompileShaderObjects() Failed for Vertex Shader...\n");
            fclose(gpFile);
            return FALSE;
        }
        // // Set this vertex shader in VErtex Shader Stage of Pipeline
        // pID3D11DeviceContext->VSSetShader(gpID3D11VertexShader,
        //                                 NULL,
        //                                 0);

        ID3DBlob *pID3DBlob_PixelShaderCode = NULL;
        if (shader->CreateAndCompilePixelShaderObjects("./src/Shaders/Water/WaterBedQuad_PS.hlsl", PIXEL_SHADER, &gpID3D11PixelShader_WaterBedQuad, &pID3DBlob_PixelShaderCode) == FALSE)
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+");
            fprintf(gpFile, "shader->CreateAndCompilePixelShaderObjects() Failed for Pixel Shader...\n");
            fclose(gpFile);
            return FALSE;
        }
        // // Set pixel shader in the Pixel Shader stage of Pipeline
        // m_DeviceContext->PSSetShader(gpID3D11PixelShader,
        //                                   NULL,
        //                                   0);

                // ############ Input Layout [just like Attributes in OpenGL] ##############
        D3D11_INPUT_ELEMENT_DESC d3d11InputElementDescriptor[2];
        ZeroMemory((void *)&d3d11InputElementDescriptor, sizeof(D3D11_INPUT_ELEMENT_DESC) * _ARRAYSIZE(d3d11InputElementDescriptor));
        // initialize input layout structure
        d3d11InputElementDescriptor[0].SemanticName = "POSITION";
        d3d11InputElementDescriptor[0].SemanticIndex = 0;
        d3d11InputElementDescriptor[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        d3d11InputElementDescriptor[0].AlignedByteOffset = 0;
        d3d11InputElementDescriptor[0].InputSlot = 0;
        d3d11InputElementDescriptor[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        d3d11InputElementDescriptor[0].InstanceDataStepRate = 0;

        d3d11InputElementDescriptor[1].SemanticName = "TEXCOORD";
        d3d11InputElementDescriptor[1].SemanticIndex = 0;
        d3d11InputElementDescriptor[1].Format = DXGI_FORMAT_R32G32_FLOAT;
        d3d11InputElementDescriptor[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        d3d11InputElementDescriptor[1].InputSlot = 0;
        d3d11InputElementDescriptor[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        d3d11InputElementDescriptor[1].InstanceDataStepRate = 0;

        // create input layout
        hr = m_Device->CreateInputLayout(d3d11InputElementDescriptor,
                                         _ARRAYSIZE(d3d11InputElementDescriptor),
                                         pID3DBlob_VertexShaderCode->GetBufferPointer(),
                                         pID3DBlob_VertexShaderCode->GetBufferSize(),
                                         &gpID3D11InputLayout_WaterBedQuad);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device->CreateInputLayout() Failed ...\n");
            fclose(gpFile);
            return FALSE;
        }

        // Set this Input Layout in Input Assembly Stages of Pipeline
        // m_DeviceContext->IASetInputLayout(gpID3D11InputLayout_WaterBedQuad); // DO IT IN DISPLAY

        // Release the Blob
        pID3DBlob_VertexShaderCode->Release();
        pID3DBlob_VertexShaderCode = NULL;

        pID3DBlob_PixelShaderCode->Release();
        pID3DBlob_PixelShaderCode = NULL;

        //################### GEOMETRY ####################
        // declaration vertex data arrays
        const float waterBedQuadPosition[] = 
        {
            -1.0f, -1.0f,
            -1.0f, 1.0f,
            1.0f, -1.0f,

            1.0f, -1.0f,
            -1.0f, 1.0f,
            1.0f, 1.0f

            // you should be passing 2 as 2nd parameter of glVertexAttribPointer(), as we are only using x and y coordinates
        };

        const float waterBedTexcoord[] = 
        {
            1.0f, 1.0f,
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f
        };

        // create vertex buffer for above position vertices
        //  A. initialize Buffer Descriptor... like glGenBuffer()
        // Position
        D3D11_BUFFER_DESC d3d11BufferDescriptor;
        ZeroMemory((void *)&d3d11BufferDescriptor, sizeof(D3D11_BUFFER_DESC));
        d3d11BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
        d3d11BufferDescriptor.ByteWidth = sizeof(float) * _ARRAYSIZE(waterBedQuadPosition);
        d3d11BufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        // B. initialize subresource data structure to put data into the buffer
        D3D11_SUBRESOURCE_DATA d3d11SubresourceData;
        ZeroMemory((void *)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
        d3d11SubresourceData.pSysMem = waterBedQuadPosition;

        // C. Create the actual buffer
        hr = pID3D11Device->CreateBuffer(&d3d11BufferDescriptor,
                                         &d3d11SubresourceData,
                                         &gpID3D11Buffer_PositionBuffer_WaterBedQuad);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device::CreateBuffer() Failed for Position ...\n");
            fclose(gpFile);
            // return (hr);
            return FALSE;
        }

        // Texture
        ZeroMemory((void *)&d3d11BufferDescriptor, sizeof(D3D11_BUFFER_DESC));
        d3d11BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
        d3d11BufferDescriptor.ByteWidth = sizeof(float) * _ARRAYSIZE(waterBedTexcoord);
        d3d11BufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        // B. initialize subresource data structure to put data into the buffer
        ZeroMemory((void *)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
        d3d11SubresourceData.pSysMem = waterBedTexcoord;

        // C. Create the actual buffer
        hr = pID3D11Device->CreateBuffer(&d3d11BufferDescriptor,
                                         &d3d11SubresourceData,
                                         &gpID3D11Buffer_TextureBuffer_WaterBedQuad);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device::CreateBuffer() Failed for Texture ...\n");
            fclose(gpFile);
            return FALSE;
        }

        return TRUE;
    }

    bool InitializeReflectionFBO(ID3D11Device *device, ID3D11DeviceContext *context, int reflectionTextureWidth, int reflectionTextureHeight, 
                                ID3D11Texture2D *reflectionTexture, ID3D11RenderTargetView *reflectionRTV, ID3D11DepthStencilView *reflectionDSV)
    {
        HRESULT hr;

        // Step 1: Check the maximum texture size supported (similar to checking max renderbuffer size)
        D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS hwOptions;
        hr = device->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &hwOptions, sizeof(hwOptions));

        if (FAILED(hr))
        {
            std::cout << "Failed to check feature support\n";
            return false;
        }

        if (reflectionTextureWidth > D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION || reflectionTextureHeight > D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION)
        {
            std::cout << "Requested texture size exceeds the hardware limits\n";
            return false;
        }

        // Step 2: Create the texture for rendering (similar to glTexImage2D)
        D3D11_TEXTURE2D_DESC texDesc = {};
        texDesc.Width = reflectionTextureWidth;
        texDesc.Height = reflectionTextureHeight;
        texDesc.MipLevels = 1;
        texDesc.ArraySize = 1;
        texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Equivalent to GL_RGB
        texDesc.SampleDesc.Count = 1;                // No MSAA
        texDesc.Usage = D3D11_USAGE_DEFAULT;
        texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

        hr = device->CreateTexture2D(&texDesc, nullptr, &reflectionTexture);
        if (FAILED(hr))
        {
            std::cout << "Failed to create the reflection texture\n";
            return false;
        }

        // Step 3: Create a render target view (equivalent to glFramebufferTexture2D)
        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
        rtvDesc.Format = texDesc.Format;
        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

        hr = device->CreateRenderTargetView(reflectionTexture, &rtvDesc, &reflectionRTV);
        if (FAILED(hr))
        {
            std::cout << "Failed to create the render target view\n";
            return false;
        }

        // Step 4: Create the depth-stencil buffer (equivalent to glRenderbufferStorage)
        D3D11_TEXTURE2D_DESC depthDesc = {};
        depthDesc.Width = reflectionTextureWidth;
        depthDesc.Height = reflectionTextureHeight;
        depthDesc.MipLevels = 1;
        depthDesc.ArraySize = 1;
        depthDesc.Format = DXGI_FORMAT_D16_UNORM; // Equivalent to GL_DEPTH_COMPONENT16
        depthDesc.SampleDesc.Count = 1;
        depthDesc.Usage = D3D11_USAGE_DEFAULT;
        depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

        ID3D11Texture2D *depthStencilBuffer;
        hr = device->CreateTexture2D(&depthDesc, nullptr, &depthStencilBuffer);
        if (FAILED(hr))
        {
            std::cout << "Failed to create the depth-stencil buffer\n";
            return false;
        }

        // Create the depth-stencil view (equivalent to glFramebufferRenderbuffer)
        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Format = depthDesc.Format;
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

        hr = device->CreateDepthStencilView(depthStencilBuffer, &dsvDesc, &reflectionDSV);
        if (FAILED(hr))
        {
            std::cout << "Failed to create the depth-stencil view\n";
            return false;
        }

        // The framebuffer is ready to be used
        return true;
    }

    bool initializeRefractionFBO(int, int)
    {
    }

    void RenderFrame(Camera camera, ID3D11RenderTargetView *pRTV, ID3D11DepthStencilView *pDSV)
    {
        // Reflection
        Reflection(camera);
        // Refraction(camera);

        // water
        clearColor[0] = 0.2f;
        clearColor[1] = 0.2f;
        clearColor[2] = 0.2f;
        clearColor[3] = 1.0f;
        m_DeviceContext->ClearRenderTargetView(pRTV, clearColor);
        m_DeviceContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH , 1.0f, 0); // 2nd and 3rd para are analogus to glCrearDepth

        PerspectiveProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), (float)g_width / (float)g_height, 0.01f, 1000.0f);

        
        // float blendFactor[4] = { 0.f, 0.f, 0.f, 0.5f }; // Not used in this context
        // UINT sampleMask = 0xffffffff; // Sample mask- all samples are updated
        // m_DeviceContext->OMSetBlendState(pBlendStateEnable, blendFactor, sampleMask);
        // Reflection(camera);
        // Refraction(camera);
        fbo_reflection->BindFBO(pRTV, pDSV, g_width, g_height);

        // RenderWaterQuad(viewMat);
        RenderWaterQuad(camera);
        // m_DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
        // object->RenderObject(cameraPosition, cameraCenter, cameraUp, false);
        object->RenderObject(camera, false);

        // texQuad->RenderObject(camera, rtSRVReflection);
        // texQuad->RenderObject(camera, rtSRVRefraction);
        // texQuad->RenderObject(camera, gpID3D11ShaderResourceView_DUDVMap);
        // texQuad->RenderObject(camera, gpID3D11ShaderResourceView_NormalMap);
    }

    void RenderWaterQuad(Camera camera)
    {
        // code
        m_DeviceContext->IASetInputLayout(gpID3D11InputLayout_WaterQuad);

        // set Position Buffer into InputAssembly stage of pipeline (glVertexAttribPointer() che last 2 para)
        UINT stride = sizeof(float) * 2;
        UINT offset = 0;
        m_DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_PositionBuffer_WaterQuad, &stride, &offset);

        // stride = sizeof(float) * 2;
        // offset = 0;
        // m_DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_TextureBuffer_WaterQuad, &stride, &offset);
        
        // 2. Render
        // Set primitive topology in Input Assembly Stage
        m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        XMMATRIX translationMatrix = XMMatrixIdentity();
        XMMATRIX worldMatrix = XMMatrixIdentity();
        XMMATRIX viewMatrix = XMMatrixIdentity();
        XMMATRIX scaleMatrix = XMMatrixIdentity();
        XMMATRIX rotateMatrix = XMMatrixIdentity();

        translationMatrix = /*XMMatrixScaling(4.0f, 0.0f, 4.0f) */ XMMatrixTranslation(0.0f, 0.0f, 0.0f);
        
        worldMatrix = translationMatrix;// * rotateMatrix * scaleMatrix;
        viewMatrix = XMMatrixLookAtLH(XMVectorSet(cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0f), 
                                       XMVectorSet(cameraCenter.x, cameraCenter.y, cameraCenter.z, 0.0f) , 
                                       XMVectorSet(cameraUp.x, cameraUp.y, cameraUp.z, 0.0f));
        // B. Put them into ConstantBuffer
        CBUFFER_WATER constBuffer;
        ZeroMemory((void *)&constBuffer, sizeof(CBUFFER_WATER));
        constBuffer.WorldMatrix = (worldMatrix);
        // constBuffer.ViewMatrix = (viewMatrix);
        constBuffer.ViewMatrix = (camera.getViewMatrix());
        constBuffer.ProjectionMatrix = (PerspectiveProjectionMatrix);

        // constBuffer.cameraPosition = cameraPosition;
        // constBuffer.cameraPosition = XMVectorSet(cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0f);
        constBuffer.cameraPosition = camera.getEye();
        constBuffer.lightPosition = XMVectorSet(lightPosition.x, lightPosition.y, lightPosition.z, 1.0f);
        // constBuffer.lightPosition = lightPosition;
        constBuffer.moveFactorOffset = moveFactor + WATER_WAVE_SPEED/4;
        constBuffer.lightColor = XMVectorSet(lightColor.x, lightColor.y, lightColor.z, 0.0f);
        // constBuffer.lightColor = lightColor;
        moveFactor += 0.00001f;

        m_DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer_WaterQuad, 0, NULL, &constBuffer, 0, 0);

        m_DeviceContext->VSSetShader(gpID3D11VertexShader_WaterQuad, 0, 0);
        m_DeviceContext->PSSetShader(gpID3D11PixelShader_WaterQuad, 0, 0);
        m_DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer_WaterQuad);
        m_DeviceContext->PSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer_WaterQuad);

        // // set Shader View in Pixel Shader
        m_DeviceContext->PSSetShaderResources(0, 1, &rtSRVReflection);
        m_DeviceContext->PSSetShaderResources(1, 1, &rtSRVRefraction);
        m_DeviceContext->PSSetShaderResources(2, 1, &gpID3D11ShaderResourceView_DUDVMap);
        m_DeviceContext->PSSetShaderResources(3, 1, &gpID3D11ShaderResourceView_NormalMap);

        // Set Sampler State in Pixel Texture (corresponds to 2nd line in Pixel shader)
        m_DeviceContext->PSSetSamplers(0, 1, &gpID3D11SamplerState_Texture_PS);

        // draw primitive
        m_DeviceContext->Draw(6, 0);

       
    }

    void Reflection(Camera camera)
    {
        float clearColor[4];
        
        // code 
        // reflection
        // fbo_reflection->BindFBO(RTVReflection, rtDSVDepth_Reflection);
        // fbo_reflection->BindFBO(RTVReflection, NULL, 800, 600);
        // fbo_reflection->BindFBO(RTVReflection, nullptr, 512, 512);
        fbo_reflection->BindFBO(RTVReflection, rtDSVDepth_Reflection, 800, 600);
        
        clearColor[0] = 0.0f;
        clearColor[1] = 0.3f;
        clearColor[2] = 0.5f;
        clearColor[3] = 0.0f;
        m_DeviceContext->ClearRenderTargetView(RTVReflection, clearColor);
        // m_DeviceContext->ClearDepthStencilView(rtDSVDepth_Reflection, D3D11_CLEAR_DEPTH , 1.0f, 0); // 2nd and 3rd para are analogus to glClearDepth

        // PerspectiveProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), (float)1024 / (float)1024, 0.01f, 1000.0f);

        // object->RenderObject(cameraPosition, cameraCenter, cameraUp, true);
        object->RenderObject(camera, true);

        fbo_reflection->UnbindFBO();
    }

    void Refraction(Camera camera)
    {
        float clearColor[4];
        
        // code 
        // reflection
        // fbo_reflection->BindFBO(RTVReflection, rtDSVDepth_Reflection);
        // fbo_reflection->BindFBO(RTVRefraction, NULL, 800, 600);
        fbo_refraction->BindFBO(RTVRefraction, rtDSVDepth_Refraction, 800, 600);
        // fbo_refraction->BindFBO(RTVRefraction, nullptr, 512, 512);
        
        clearColor[0] = 0.0f;
        clearColor[1] = 0.1f;
        clearColor[2] = 0.3f;
        clearColor[3] = 0.0f;
        m_DeviceContext->ClearRenderTargetView(RTVRefraction, clearColor);
        m_DeviceContext->ClearDepthStencilView(rtDSVDepth_Refraction, D3D11_CLEAR_DEPTH , 1.0f, 0); // 2nd and 3rd para are analogus to glCrearDepth

        // object->RenderObject(cameraPosition, cameraCenter, cameraUp, false);
        object->RenderObject(camera, false);

        fbo_refraction->UnbindFBO();
    }

private:

    // __declspec(align(16))  
    struct CBUFFER_WATER
    {
        float moveFactorOffset;
        XMMATRIX WorldMatrix;
        XMMATRIX ViewMatrix;
        XMMATRIX ProjectionMatrix;
        XMVECTOR cameraPosition;
        XMVECTOR lightPosition;
        XMVECTOR lightColor;
    };

    IDXGISwapChain *m_SwapChain;
    ID3D11Device *m_Device;
    ID3D11DeviceContext *m_DeviceContext;
    ID3D11RenderTargetView *m_RenderTargetView;
    ID3D11DepthStencilView *m_DepthStencilView;

    ID3D11Texture2D *gpID3D11Texture2D_RTTexture;
    ID3D11RenderTargetView *gpID3D11RenderTargetView_RTTexture;
    ID3D11ShaderResourceView *gpID3D11ShaderResourceView_RTTexture;
    ID3D11ShaderResourceView *gpID3D11ShaderResourceView_DUDVMap;
    ID3D11ShaderResourceView *gpID3D11ShaderResourceView_NormalMap;
    
    ID3D11ShaderResourceView *rtRefraction;
    ID3D11Texture2D *gpID3D11Texture2D_RTTextureDepth;
    ID3D11DepthStencilView *gpID3D11DepthStencilView_RTTextureDepth;

    // reflection
    ID3D11Texture2D *RTTexture_Reflection = NULL;
    ID3D11RenderTargetView *RTVReflection = NULL;
    ID3D11ShaderResourceView *rtSRVReflection = NULL;
    ID3D11Texture2D *rtTextureDepth_Reflection = NULL;
    ID3D11DepthStencilView *rtDSVDepth_Reflection = NULL;

    // refraction
    ID3D11Texture2D *RTTexture_Refraction = NULL;
    ID3D11RenderTargetView *RTVRefraction = NULL;
    ID3D11ShaderResourceView *rtSRVRefraction = NULL;
    ID3D11Texture2D *rtTextureDepth_Refraction = NULL;
    ID3D11DepthStencilView *rtDSVDepth_Refraction = NULL;

    ID3D11BlendState *pBlendStateEnable = NULL;

    DXShaders *shader = NULL;

    // camera vectors
    XMFLOAT3 cameraPosition = XMFLOAT3(0.0f, 0.75f, -2.0f);
    XMFLOAT3 cameraCenter = XMFLOAT3(0.0f, 0.0f, 0.0f);
    XMFLOAT3 cameraUp = XMFLOAT3(0.0f, 1.0f, 0.0f);

    XMFLOAT3 lightColor = XMFLOAT3(1.0f, 1.0f, 1.0f);
	XMFLOAT3 lightPosition = XMFLOAT3(0.0f, 2.0f, 2.0f);

    float moveFactor = 0.0f;

    HRESULT CreateFBO()
    {
        HRESULT hr = S_OK;
            // Steps : Render to texture
        // 1. Initialize and create texture for Render to Texture
        D3D11_TEXTURE2D_DESC d3d11Texture2Ddescriptor_texture;
        ZeroMemory((void *)&d3d11Texture2Ddescriptor_texture, sizeof(D3D11_TEXTURE2D_DESC));
        d3d11Texture2Ddescriptor_texture.Width = RENDER_TEXTURE_WIDTH;
        d3d11Texture2Ddescriptor_texture.Height = RENDER_TEXTURE_HEIGHT;
        d3d11Texture2Ddescriptor_texture.MipLevels = 1;
        d3d11Texture2Ddescriptor_texture.ArraySize = 1;
        d3d11Texture2Ddescriptor_texture.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        d3d11Texture2Ddescriptor_texture.SampleDesc.Count = 1;
        d3d11Texture2Ddescriptor_texture.Usage = D3D11_USAGE_DEFAULT;
        d3d11Texture2Ddescriptor_texture.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        d3d11Texture2Ddescriptor_texture.CPUAccessFlags = 0;
        d3d11Texture2Ddescriptor_texture.MiscFlags = 0;

        hr = pID3D11Device->CreateTexture2D(&d3d11Texture2Ddescriptor_texture, NULL, &gpID3D11Texture2D_RTTexture);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device->CreateTexture2D() Failed For d3d11Texture2Ddescriptor_texture...\n");
            fclose(gpFile);
            return(hr);
        }

        // 2. Create Render Target View From above Texture
        D3D11_RENDER_TARGET_VIEW_DESC d3d11RenderTargetViewDescriptor_texture;
        ZeroMemory((void*)&d3d11RenderTargetViewDescriptor_texture, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
        d3d11RenderTargetViewDescriptor_texture.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        d3d11RenderTargetViewDescriptor_texture.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

        hr = pID3D11Device->CreateRenderTargetView(gpID3D11Texture2D_RTTexture, &d3d11RenderTargetViewDescriptor_texture, &gpID3D11RenderTargetView_RTTexture);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device->CreateRenderTargetView() Failed For d3d11RenderTargetViewDescriptor_texture...\n");
            fclose(gpFile);
            return(hr);
        }

        // 3. Create Shader Resource View
        D3D11_SHADER_RESOURCE_VIEW_DESC d3d11ShaderResourceViewDescriptor_texture;
        ZeroMemory((void*)&d3d11ShaderResourceViewDescriptor_texture, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
        d3d11ShaderResourceViewDescriptor_texture.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        d3d11ShaderResourceViewDescriptor_texture.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
        d3d11ShaderResourceViewDescriptor_texture.Texture2D.MostDetailedMip = 0;
        d3d11ShaderResourceViewDescriptor_texture.Texture2D.MipLevels = 1;

        hr = pID3D11Device->CreateShaderResourceView(gpID3D11Texture2D_RTTexture, &d3d11ShaderResourceViewDescriptor_texture, &gpID3D11ShaderResourceView_RTTexture);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device->CreateShaderResourceView() Failed For d3d11ShaderResourceViewDescriptor_texture...\n");
            fclose(gpFile);
            return(hr);
        }

        // 4. Create Texture for handling depth
        D3D11_TEXTURE2D_DESC d3d11Texture2Ddescriptor_textureDepth;
        ZeroMemory((void *)&d3d11Texture2Ddescriptor_textureDepth, sizeof(D3D11_TEXTURE2D_DESC));
        d3d11Texture2Ddescriptor_textureDepth.Width = RENDER_TEXTURE_WIDTH;
        d3d11Texture2Ddescriptor_textureDepth.Height = RENDER_TEXTURE_HEIGHT;
        d3d11Texture2Ddescriptor_textureDepth.MipLevels = 1;
        d3d11Texture2Ddescriptor_textureDepth.ArraySize = 1;
        d3d11Texture2Ddescriptor_textureDepth.Format = DXGI_FORMAT_D32_FLOAT;
        d3d11Texture2Ddescriptor_textureDepth.SampleDesc.Count = 1;
        d3d11Texture2Ddescriptor_textureDepth.Usage = D3D11_USAGE_DEFAULT;
        d3d11Texture2Ddescriptor_textureDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        d3d11Texture2Ddescriptor_textureDepth.CPUAccessFlags = 0;
        d3d11Texture2Ddescriptor_textureDepth.MiscFlags = 0;

        hr = pID3D11Device->CreateTexture2D(&d3d11Texture2Ddescriptor_textureDepth, NULL, &gpID3D11Texture2D_RTTextureDepth);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device->CreateTexture2D() Failed For d3d11Texture2Ddescriptor_textureDepth...\n");
            fclose(gpFile);
            return(hr);
        }

        // 5. Create Depth Stencil View
        D3D11_DEPTH_STENCIL_VIEW_DESC d3d11DepthStencilViewDescriptor;
        ZeroMemory((void*)&d3d11DepthStencilViewDescriptor, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
        d3d11DepthStencilViewDescriptor.Format = DXGI_FORMAT_D32_FLOAT;
        d3d11DepthStencilViewDescriptor.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

        hr = pID3D11Device->CreateDepthStencilView(gpID3D11Texture2D_RTTextureDepth, &d3d11DepthStencilViewDescriptor, &gpID3D11DepthStencilView_RTTextureDepth);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device->CreateDepthStencilView() Failed For d3d11DepthStencilViewDescriptor...\n");
            fclose(gpFile);
            return(hr);
        }
    }
};

#endif
