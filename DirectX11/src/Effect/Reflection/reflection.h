#ifndef REFLECTION_H_
#define REFLECTION_H_

#include "../Water/WaterQuad.h"
#include "../Water/TexturedQuad.h"
#include "../../Common/Framebuffer.h"
#include "../../Common/Camera.h"
#include "rendertextureclass.h"
#include "reflectionshader.h"
#include "reflectionquad.h"

extern FILE *gpFile;
extern char gszLogFilePathName[];

class Reflection
{
    public:
        Reflection(){}
        Reflection(ID3D11Device *Device,
                    ID3D11DeviceContext *DeviceContext) : m_device(Device),
                                                            m_deviceContext(DeviceContext){}
        ~Reflection(){}

        BOOL Initialize()
        {
            // code
            cube = new WaterQuad(nullptr, m_device, m_deviceContext, nullptr);
            if (cube->Initialize() == FALSE)
            {
                fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
                fprintf(gpFile, "Cube Initialize() Failed ...\n");
                fclose(gpFile);
            }

            quad = new TexturedQuad(nullptr, m_device, m_deviceContext, nullptr);
            if (quad->Initialize() == FALSE)
            {
                fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
                fprintf(gpFile, "Quad Initialize() Failed ...\n");
                fclose(gpFile);
            }

            reflectionQuad = new ReflectionQuad(nullptr, m_device, m_deviceContext, nullptr);

            // fbo_reflection = new Framebuffer();
            // hr = fbo_reflection->CreateFramebuffer(m_device, &RTTexture_Reflection, &RTVReflection, &rtSRVReflection, &rtTextureDepth_Reflection, &rtDSVDepth_Reflection, TRUE);
            // if (FAILED(hr))
            // {
            //     fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            //     fprintf(gpFile, "CreateFramebuffer() Failed ...\n");
            //     fclose(gpFile);
            // }

            reflection = new RenderTextureClass();
            if (reflection->Initialize(m_device, 2048, 2048, 1000.0f, 0.3f, 1) == FALSE)
            {
                fopen_s(&gpFile, gszLogFilePathName, "a+");
                fprintf(gpFile, "RenderTextureClass() Failed for Reflection ...\n");
                fclose(gpFile);
            }

            refraction = new RenderTextureClass();
            if (refraction->Initialize(m_device, 2048, 2048, 1000.0f, 0.3f, 1) == FALSE)
            {
                fopen_s(&gpFile, gszLogFilePathName, "a+");
                fprintf(gpFile, "RenderTextureClass() Failed for Refraction ...\n");
                fclose(gpFile);
            }

            // reflectionShader = new ReflectionShaderClass();
            // if (reflectionShader->Initialize(m_device, NULL) == FALSE)
            // {
            //     fopen_s(&gpFile, gszLogFilePathName, "a+");
            //     fprintf(gpFile, "ReflectionShaderClass() Failed ...\n");
            //     fclose(gpFile);
            // }

            // floor texture
            hr = CreateWICTextureFromFile(m_device, m_deviceContext, L"assets/Textures/parallexmap/wall.jpg", nullptr, &floor_texture);
            if (FAILED(hr))
            {
                fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
                fprintf(gpFile, "Texture couldn't be loaded : DUDVMap ...\n");
                fclose(gpFile);
            }

            return TRUE;
        }

        void RenderReflectionOnTexture(Camera camera, float rotation)
        {
            XMMATRIX worldMatrix, reflectionViewMatrix, projectionMatrix;

            reflection->SetRenderTarget(m_deviceContext);
            // reflection->ClearRenderTarget(m_deviceContext, 0.0f, 0.2f, 0.3f, 0.0f);
            reflection->ClearRenderTarget(m_deviceContext, 0.1f, 0.1f, 0.1f, 0.0f);

            reflectionViewMatrix = camera.getReflectionMatrix(-1.5f);
            worldMatrix = XMMatrixIdentity();
            projectionMatrix = PerspectiveProjectionMatrix;

            worldMatrix = XMMatrixRotationY(rotation);

            cube->RenderCube(worldMatrix, reflectionViewMatrix, projectionMatrix, XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f));

            // object 2
            worldMatrix = XMMatrixIdentity();
            // worldMatrix =  XMMatrixScaling(10.0f, 10.0f, 10.0f);
            worldMatrix = XMMatrixTranslation(-3.0f, 3.0f, 10.0f);
            cube->RenderCube(worldMatrix, reflectionViewMatrix, projectionMatrix, XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f));

            SetBackBufferRenderTarget();
            ResetViewport();
        }

        void RenderRefractionOnTexture(Camera camera, float rotation)
        {
            XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

            refraction->SetRenderTarget(m_deviceContext);
            // refraction->ClearRenderTarget(m_deviceContext, 0.0f, 0.2f, 0.3f, 0.0f);
            refraction->ClearRenderTarget(m_deviceContext, 0.1f, 0.1f, 0.1f, 0.0f);

            viewMatrix = camera.getViewMatrix();
            worldMatrix = XMMatrixIdentity();
            projectionMatrix = PerspectiveProjectionMatrix;

            worldMatrix = XMMatrixTranslation(-3.0f, -3.0f, 0.0f) * XMMatrixRotationY(rotation);

            cube->RenderCube(worldMatrix, viewMatrix, projectionMatrix, XMVectorSet(1.0f, 0.0f, 1.0f, 1.0f));

             // object 2
            // worldMatrix = XMMatrixIdentity();
            // worldMatrix =  XMMatrixScaling(50.0f, 1.0f, 50.0f);
            // worldMatrix *= XMMatrixTranslation(-3.0f, -10.0f, 10.0f);
            // cube->RenderCube(worldMatrix, viewMatrix, projectionMatrix, XMVectorSet(0.1f, 0.1f, 0.1f, 1.0f));

            SetBackBufferRenderTarget();
            ResetViewport();
        }

        void Render(Camera camera, float rotation)
        {
            XMMATRIX worldMatrix, viewMatrix, projectionMatrix, reflectionViewMatrix;
            XMVECTOR cameraPosition;
            float color[4];

            // m_deviceContext->ClearRenderTargetView(m_renderTargetView, clearColor);
            // m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
//-----------// Setup the color to clear the buffer to.
            color[0] = 0.0f;
            color[1] = 0.5f;
            color[2] = 0.5f;
            color[3] = 1.0f;

            // Clear the back buffer.
            m_deviceContext->ClearRenderTargetView(m_renderTargetView, color); //Already doing it in display()
            
            // Clear the depth buffer.
            m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);//Already doing it in display()
//------------------------------------------------------------------------------------------------------
            worldMatrix = XMMatrixIdentity();
            viewMatrix = camera.getViewMatrix();
            projectionMatrix = PerspectiveProjectionMatrix;

            worldMatrix = XMMatrixRotationY(rotation);

            cube->RenderCube(worldMatrix, viewMatrix, projectionMatrix, XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f));

            // object 2
            worldMatrix = XMMatrixIdentity();
            // worldMatrix =  XMMatrixScaling(10.0f, 10.0f, 10.0f);
            worldMatrix = XMMatrixTranslation(-3.0f, 3.0f, 10.0f);
            cube->RenderCube(worldMatrix, viewMatrix, projectionMatrix, XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f));

            // object 3
            worldMatrix = XMMatrixIdentity();
            worldMatrix = XMMatrixTranslation(-3.0f, -3.0f, 0.0f) * XMMatrixRotationY(rotation);
            cube->RenderCube(worldMatrix, viewMatrix, projectionMatrix, XMVectorSet(1.0f, 0.0f, 1.0f, 1.0f));

            worldMatrix = XMMatrixIdentity();
            worldMatrix = XMMatrixTranslation(0.0f, -1.5f, 0.0f); 
            reflectionViewMatrix = camera.getReflectionMatrix(-1.5f);
            cameraPosition = camera.getEye();
            reflectionQuad->RenderObject(worldMatrix, viewMatrix, projectionMatrix, reflectionViewMatrix, cameraPosition, floor_texture, reflection->GetShaderResourceView(), refraction->GetShaderResourceView());
        }

        void SetBackBufferRenderTarget()
        {
            m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
        }

        void ResetViewport()
        {
            m_deviceContext->RSSetViewports(1, &m_viewport);
        }

        void SetRTV(ID3D11RenderTargetView *rtv)
        {
            m_renderTargetView = rtv;
        }

        void SetDSV(ID3D11DepthStencilView *dsv)
        {
            m_depthStencilView = dsv;
        }

        void SetViewport(D3D11_VIEWPORT viewport)
        {
            m_viewport = viewport;
        }

    private:
        ID3D11Device *m_device = NULL;
        ID3D11DeviceContext *m_deviceContext = NULL;

        WaterQuad *cube = NULL;
        TexturedQuad *quad = NULL;
        Framebuffer *fbo_reflection = NULL;
        RenderTextureClass *reflection = nullptr;
        RenderTextureClass *refraction = nullptr;
        ReflectionShaderClass *reflectionShader = nullptr;
        ReflectionQuad *reflectionQuad = NULL;

        // reflection
        ID3D11Texture2D *RTTexture_Reflection = NULL;
        ID3D11RenderTargetView *RTVReflection = NULL;
        ID3D11ShaderResourceView *rtSRVReflection = NULL;
        ID3D11Texture2D *rtTextureDepth_Reflection = NULL;
        ID3D11DepthStencilView *rtDSVDepth_Reflection = NULL;

        ID3D11ShaderResourceView *floor_texture = NULL;

        ID3D11RenderTargetView *m_renderTargetView;
        ID3D11DepthStencilView *m_depthStencilView;
        D3D11_VIEWPORT m_viewport;

        HRESULT hr;
};

#endif