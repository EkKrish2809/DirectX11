#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <windows.h>
#include <winuser.h>
#include <stdio.h>
#include <stdlib.h>

#include <dxgi.h>
#include <D3D11.h>
#include <d3dcompiler.h>

#include "Common.h"

#define RENDER_TEXTURE_WIDTH 800
#define RENDER_TEXTURE_HEIGHT 600

extern FILE *gpFile;
extern char gszLogFilePathName[];

class Framebuffer
{
public:
    HRESULT CreateFramebuffer(ID3D11Device *pID3D11Device, ID3D11Texture2D **ppTex2D, ID3D11RenderTargetView **ppRTV, ID3D11ShaderResourceView **ppSRV,
                              ID3D11Texture2D **ppTexDepth2D, ID3D11DepthStencilView **ppDSV_Depth, BOOL DepthNeeded)
    {
        // local variables
        HRESULT hr = S_OK;

        // code
        // Steps : Render to texture
        // 1. Initialize and create texture for Render to Texture
        D3D11_TEXTURE2D_DESC d3d11Texture2Ddescriptor_texture;
        ZeroMemory((void *)&d3d11Texture2Ddescriptor_texture, sizeof(D3D11_TEXTURE2D_DESC));
        d3d11Texture2Ddescriptor_texture.Width = RENDER_TEXTURE_WIDTH;
        d3d11Texture2Ddescriptor_texture.Height = RENDER_TEXTURE_HEIGHT;
        d3d11Texture2Ddescriptor_texture.MipLevels = 1;
        d3d11Texture2Ddescriptor_texture.ArraySize = 1;
        // d3d11Texture2Ddescriptor_texture.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        d3d11Texture2Ddescriptor_texture.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
        d3d11Texture2Ddescriptor_texture.SampleDesc.Count = 1;
        d3d11Texture2Ddescriptor_texture.Usage = D3D11_USAGE_DEFAULT;
        d3d11Texture2Ddescriptor_texture.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        d3d11Texture2Ddescriptor_texture.CPUAccessFlags = 0;
        d3d11Texture2Ddescriptor_texture.MiscFlags = 0;

        hr = pID3D11Device->CreateTexture2D(&d3d11Texture2Ddescriptor_texture, NULL, ppTex2D);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device->CreateTexture2D() Failed For d3d11Texture2Ddescriptor_texture...\n");
            fclose(gpFile);
            return (hr);
        }

        // 2. Create Render Target View From above Texture
        D3D11_RENDER_TARGET_VIEW_DESC d3d11RenderTargetViewDescriptor_texture;
        ZeroMemory((void *)&d3d11RenderTargetViewDescriptor_texture, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
        d3d11RenderTargetViewDescriptor_texture.Format = d3d11Texture2Ddescriptor_texture.Format;
        d3d11RenderTargetViewDescriptor_texture.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        d3d11RenderTargetViewDescriptor_texture.Texture2D.MipSlice = 0;

        hr = pID3D11Device->CreateRenderTargetView(*ppTex2D, &d3d11RenderTargetViewDescriptor_texture, ppRTV);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device->CreateRenderTargetView() Failed For d3d11RenderTargetViewDescriptor_texture...\n");
            fclose(gpFile);
            return (hr);
        }

        // 3. Create Shader Resource View
        D3D11_SHADER_RESOURCE_VIEW_DESC d3d11ShaderResourceViewDescriptor_texture;
        ZeroMemory((void *)&d3d11ShaderResourceViewDescriptor_texture, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
        d3d11ShaderResourceViewDescriptor_texture.Format = d3d11Texture2Ddescriptor_texture.Format;
        d3d11ShaderResourceViewDescriptor_texture.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        d3d11ShaderResourceViewDescriptor_texture.Texture2D.MostDetailedMip = 0;
        d3d11ShaderResourceViewDescriptor_texture.Texture2D.MipLevels = 1;

        hr = pID3D11Device->CreateShaderResourceView(*ppTex2D, &d3d11ShaderResourceViewDescriptor_texture, ppSRV);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device->CreateShaderResourceView() Failed For d3d11ShaderResourceViewDescriptor_texture...\n");
            fclose(gpFile);
            return (hr);
        }

        if (DepthNeeded == TRUE)
        {
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

            hr = pID3D11Device->CreateTexture2D(&d3d11Texture2Ddescriptor_textureDepth, NULL, ppTexDepth2D);
            if (FAILED(hr))
            {
                fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
                fprintf(gpFile, "gpID3D11Device->CreateTexture2D() Failed For d3d11Texture2Ddescriptor_textureDepth...\n");
                fclose(gpFile);
                return (hr);
            }

            // 5. Create Depth Stencil View
            D3D11_DEPTH_STENCIL_VIEW_DESC d3d11DepthStencilViewDescriptor;
            ZeroMemory((void *)&d3d11DepthStencilViewDescriptor, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
            d3d11DepthStencilViewDescriptor.Format = d3d11Texture2Ddescriptor_textureDepth.Format;
            d3d11DepthStencilViewDescriptor.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

            hr = pID3D11Device->CreateDepthStencilView(*ppTexDepth2D, &d3d11DepthStencilViewDescriptor, ppDSV_Depth);
            if (FAILED(hr))
            {
                fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
                fprintf(gpFile, "gpID3D11Device->CreateDepthStencilView() Failed For d3d11DepthStencilViewDescriptor...\n");
                fclose(gpFile);
                return (hr);
            }
        }

        return (hr);
    }

    void BindFBO(ID3D11RenderTargetView * pRTV, ID3D11DepthStencilView * pDSV, INT textureWidth, INT TextureHeight)
    {
        // local variable
        D3D11_VIEWPORT d3d11Viewport;

        // 5. Set this new RTV in the pipeline
        pID3D11DeviceContext->OMSetRenderTargets(1, &pRTV, pDSV); // OM = Output Merger

        // 6. Initialize Viewport structure
        d3d11Viewport.TopLeftX = 0.0f;
        d3d11Viewport.TopLeftY = 0.0f;
        // d3d11Viewport.Width = g_width;
        // d3d11Viewport.Height = g_height;
        d3d11Viewport.Width = textureWidth;
        d3d11Viewport.Height = TextureHeight;
        d3d11Viewport.MinDepth = 0.0f;
        d3d11Viewport.MaxDepth = 1.0f;

        // 7. Tell device context to Set this Viewport in the pipeline
        pID3D11DeviceContext->RSSetViewports(1, &d3d11Viewport);
    }

    void UnbindFBO(void)
    {
        
    }

private:
    // D3D related global variables
    IDXGISwapChain *pIDXGISwapChain = NULL;
    ID3D11Device *m_Device = NULL;
    ID3D11DeviceContext *m_DeviceContext = NULL;
    ID3D11RenderTargetView *m_RenderTargetView = NULL;
    float clearColor[4];

    ID3D11RasterizerState *m_RasterizerState = NULL;
    ID3D11DepthStencilView *m_DepthStencilView = NULL;
};

#endif
