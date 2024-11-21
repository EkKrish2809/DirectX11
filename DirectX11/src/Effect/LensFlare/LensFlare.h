#pragma once

// D3D related headers
#include <dxgi.h>
#include <D3D11.h>
#include <d3dcompiler.h>

#pragma warning(disable : 4838)
#include "XNAMath_204/xnamath.h"

#include "../../Common/DXShaders.h"
#include "../../Common/DDSTextureLoader.h"
#include "../../Common/Camera.h"
#include "../Model/TextureLoader.h"

// extern ID3D11DepthStencilView *gpID3D11DepthStencilView;

struct CBufferLensFlare
{
    XMFLOAT4 ScreenSpacePosition;
};

class LensFlare
{
public:
    LensFlare(IDXGISwapChain *SwapChain,
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
    }
    ~LensFlare() {}

    BOOL Initialize()
    {
        HRESULT hr = S_OK;
        // vertex shader
        ID3DBlob *pID3DBlob_VertexShaderCode = NULL;
        if (shader->CreateAndCompileVertexShaderObjects("./src/Shaders/LensFlare/Vertex.hlsl", VERTEX_SHADER, &gpID3D11VertexShader, &pID3DBlob_VertexShaderCode) == FALSE)
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+");
            fprintf(gpFile, "shader->CreateAndCompileShaderObjects() Failed for Vertex Shader...\n");
            fclose(gpFile);
            return FALSE;
        }
        // Set this vertex shader in VErtex Shader Stage of Pipeline
        // m_DeviceContext->VSSetShader(gpID3D11VertexShader, NULL, 0);

        // Geometry Shader
        ID3DBlob *pID3DBlob_GeometryShaderCode = NULL; 
        if (shader->CreateAndCompileGeometryShaderObjects("./src/Shaders/LensFlare/Geometry.hlsl", GEOMETRY_SHADER, &gpID3D11GeometryShader, &pID3DBlob_GeometryShaderCode) == FALSE)
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+");
            fprintf(gpFile, "shader->CreateAndCompileGeometryShaderObjects() Failed for Geometry Shader...\n");
            fclose(gpFile);
            return FALSE;
        }
        //  Set this Geometry shader in Geometry Shader Stage of Pipeline
        // m_DeviceContext->GSSetShader(gpID3D11GeometryShader, NULL, 0);

        ID3DBlob *pID3DBlob_PixelShaderCode = NULL;
        if (shader->CreateAndCompilePixelShaderObjects("./src/Shaders/LensFlare/Pixel.hlsl", PIXEL_SHADER, &gpID3D11PixelShader, &pID3DBlob_PixelShaderCode) == FALSE)
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+");
            fprintf(gpFile, "shader->CreateAndCompilePixelShaderObjects() Failed for Pixel Shader...\n");
            fclose(gpFile);
            return FALSE;
        }
        // Set pixel shader in the Pixel Shader stage of Pipeline
        // m_DeviceContext->PSSetShader(gpID3D11PixelShader, NULL, 0);

        // Set this Input Layout in Input Assembly Stages of Pipeline
        // Here we are setting it as NULLm, as we are not going to use Vertex Buffer for passing vertices to the Vertex shader 
        // but instead we will generate in inside the Vertex shader
        m_DeviceContext->IASetInputLayout(NULL);

        // load textures
        hr = CreateWICTextureFromFile(m_Device, m_DeviceContext, L"assets/Textures/lensFlare/flare0.jpg", nullptr, &lensTexture00);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "Texture couldn't be loaded : LensTexture 0 ...\n");
            fclose(gpFile);
        }

        hr = CreateWICTextureFromFile(m_Device, m_DeviceContext, L"assets/Textures/lensFlare/flare1.jpg", nullptr, &lensTexture01);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "Texture couldn't be loaded : LensTexture 1 ...\n");
            fclose(gpFile);
        }

        hr = CreateWICTextureFromFile(m_Device, m_DeviceContext, L"assets/Textures/lensFlare/flare2.jpg", nullptr, &lensTexture02);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "Texture couldn't be loaded : LensTexture 2 ...\n");
            fclose(gpFile);
        }

        hr = CreateWICTextureFromFile(m_Device, m_DeviceContext, L"assets/Textures/lensFlare/flare3.jpg", nullptr, &lensTexture03);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "Texture couldn't be loaded : LensTexture 3 ...\n");
            fclose(gpFile);
        }

        hr = CreateWICTextureFromFile(m_Device, m_DeviceContext, L"assets/Textures/lensFlare/flare4.jpg", nullptr, &lensTexture04);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "Texture couldn't be loaded : LensTexture 4 ...\n");
            fclose(gpFile);
        }

        hr = CreateWICTextureFromFile(m_Device, m_DeviceContext, L"assets/Textures/lensFlare/flare5.jpg", nullptr, &lensTexture05);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "Texture couldn't be loaded : LensTexture 5 ...\n");
            fclose(gpFile);
        }

        hr = CreateWICTextureFromFile(m_Device, m_DeviceContext, L"assets/Textures/lensFlare/flare6.jpg", nullptr, &lensTexture06);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "Texture couldn't be loaded : LensTexture 6 ...\n");
            fclose(gpFile);
        }

        // Create Texture Sampler State
        D3D11_SAMPLER_DESC d3d11SamplerDescriptor;
        ZeroMemory((void *)&d3d11SamplerDescriptor, sizeof(D3D11_SAMPLER_DESC));
        d3d11SamplerDescriptor.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        d3d11SamplerDescriptor.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        d3d11SamplerDescriptor.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        d3d11SamplerDescriptor.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

        hr = m_Device->CreateSamplerState(&d3d11SamplerDescriptor, &samplerState_PS);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gm_Device->CreateSamplerState() Failed for Pixel Shader ...\n");
            fclose(gpFile);
            // return (hr);
            return FALSE;
        }

        return TRUE;
    }

    void Render()
    {
        m_DeviceContext->IASetInputLayout(nullptr);
        m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

        m_DeviceContext->GSSetShaderResources(0, 1, &lensTexture00);
        m_DeviceContext->GSSetShaderResources(1, 1, &lensTexture01);
        m_DeviceContext->GSSetShaderResources(2, 1, &lensTexture02);
        m_DeviceContext->GSSetShaderResources(3, 1, &lensTexture03);
        m_DeviceContext->GSSetShaderResources(4, 1, &lensTexture04);
        m_DeviceContext->GSSetShaderResources(5, 1, &lensTexture05);
        m_DeviceContext->GSSetShaderResources(6, 1, &lensTexture06);
        // m_DeviceContext->GSSetShaderResources(7, 1, gpID3D11DepthStencilView->GetResource());

        m_DeviceContext->PSSetShaderResources(0, 1, &lensTexture00);
        m_DeviceContext->PSSetShaderResources(1, 1, &lensTexture01);
        m_DeviceContext->PSSetShaderResources(2, 1, &lensTexture02);
        m_DeviceContext->PSSetShaderResources(3, 1, &lensTexture03);
        m_DeviceContext->PSSetShaderResources(4, 1, &lensTexture04);
        m_DeviceContext->PSSetShaderResources(5, 1, &lensTexture05);
        m_DeviceContext->PSSetShaderResources(6, 1, &lensTexture06);

        m_DeviceContext->VSSetShader(gpID3D11VertexShader, 0, 0);
        m_DeviceContext->GSSetShader(gpID3D11GeometryShader, 0, 0);
        m_DeviceContext->PSSetShader(gpID3D11PixelShader, 0, 0);

        m_DeviceContext->GSSetSamplers(0, 1, &samplerState_PS);
        m_DeviceContext->PSSetSamplers(0, 1, &samplerState_PS);

        m_DeviceContext->Draw(7, 0);
    }

private:
    IDXGISwapChain *m_SwapChain;
    ID3D11Device *m_Device;
    ID3D11DeviceContext *m_DeviceContext;
    ID3D11RenderTargetView *m_RenderTargetView;
    ID3D11DepthStencilView *m_DepthStencilView;

    ID3D11VertexShader *gpID3D11VertexShader = NULL;
    ID3D11GeometryShader *gpID3D11GeometryShader = NULL;
    ID3D11PixelShader *gpID3D11PixelShader = NULL;
    ID3D11InputLayout *gpID3D11InputLayout = NULL;
    ID3D11Buffer *gpID3D11Buffer_PositionBuffer = NULL;
    ID3D11Buffer *gpID3D11Buffer_TextureBuffer = NULL;
    ID3D11Buffer *gpID3D11Buffer_ConstantBuffer = NULL;
    ID3D11ShaderResourceView *lensTexture00 = NULL;
    ID3D11ShaderResourceView *lensTexture01 = NULL;
    ID3D11ShaderResourceView *lensTexture02 = NULL;
    ID3D11ShaderResourceView *lensTexture03 = NULL;
    ID3D11ShaderResourceView *lensTexture04 = NULL;
    ID3D11ShaderResourceView *lensTexture05 = NULL;
    ID3D11ShaderResourceView *lensTexture06 = NULL;
    ID3D11ShaderResourceView *depthTexture = NULL;

    ID3D11SamplerState *samplerState_PS = NULL;
    ID3D11SamplerState *samplerState_GS = NULL;

    DXShaders *shader = NULL;
};