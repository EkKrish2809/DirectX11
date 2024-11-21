#ifndef SHADOW_H
#define SHADOW_H

#include "../../Common/Camera.h"
#include "../../Common/DXShaders.h"

#include "ShadowShader.h"
#include "DepthShader.h"
#include "Light.h"

#include "../Reflection/rendertextureclass.h"

const int SHADOWMAP_WIDTH = 1024;
const int SHADOWMAP_HEIGHT = 1024;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;
const float SHADOWMAP_DEPTH = 50.f;
const float SHADOWMAP_NEAR = 1.f;



class Shadow
{
public:
    Shadow(IDXGISwapChain *SwapChain,
              ID3D11Device *Device,
              ID3D11DeviceContext *DeviceContext,
              ID3D11RenderTargetView *RenderTargetView) :
              m_SwapChain(SwapChain),
                m_Device(Device),
                m_DeviceContext(DeviceContext),
                m_RenderTargetView(RenderTargetView) {}

    ~Shadow() {}

    BOOL Initialize()
    {
        HRESULT hr = S_OK;
        // ############ Geometry ##############
        const float cubeVertices[] = 
        {
            // position : x,y,z
            // SIDE 1 ( TOP )
            -1.0f, +1.0f, +1.0f,
            +1.0f, +1.0f, +1.0f,
            -1.0f, +1.0f, -1.0f,

            -1.0f, +1.0f, -1.0f,
            +1.0f, +1.0f, +1.0f,
            +1.0f, +1.0f, -1.0f,

            // SIDE 2 ( BOTTOM )
            +1.0f, -1.0f, -1.0f,
            +1.0f, -1.0f, +1.0f,
            -1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            +1.0f, -1.0f, +1.0f,
            -1.0f, -1.0f, +1.0f,

            // SIDE 3 ( FRONT )
            -1.0f, +1.0f, -1.0f,
            +1.0f, +1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            +1.0f, +1.0f, -1.0f,
            +1.0f, -1.0f, -1.0f,

            // SIDE 4 ( BACK )
            +1.0f, -1.0f, +1.0f,
            +1.0f, +1.0f, +1.0f,
            -1.0f, -1.0f, +1.0f,

            -1.0f, -1.0f, +1.0f,
            +1.0f, +1.0f, +1.0f,
            -1.0f, +1.0f, +1.0f,

            // SIDE 5 ( LEFT )
            -1.0f, +1.0f, +1.0f,
            -1.0f, +1.0f, -1.0f,
            -1.0f, -1.0f, +1.0f,

            -1.0f, -1.0f, +1.0f,
            -1.0f, +1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,

            // SIDE 6 ( RIGHT )
            +1.0f, -1.0f, -1.0f,
            +1.0f, +1.0f, -1.0f,
            +1.0f, -1.0f, +1.0f,

            +1.0f, -1.0f, +1.0f,
            +1.0f, +1.0f, -1.0f,
            +1.0f, +1.0f, +1.0f,
        };

        const float cubeTexcoords[] =
        {
            // Tex : u,v
            // SIDE 1 ( TOP )
            +0.0f, +0.0f,
            +0.0f, +1.0f,
            +1.0f, +0.0f,

            +1.0f, +0.0f,
            +0.0f, +1.0f,
            +1.0f, +1.0f,

            // SIDE 2 ( BOTTOM )
            +0.0f, +0.0f,
            +0.0f, +1.0f,
            +1.0f, +0.0f,

            +1.0f, +0.0f,
            +0.0f, +1.0f,
            +1.0f, +1.0f,

            // SIDE 3 ( FRONT )
            +0.0f, +0.0f,
            +0.0f, +1.0f,
            +1.0f, +0.0f,

            +1.0f, +0.0f,
            +0.0f, +1.0f,
            +1.0f, +1.0f,

            // SIDE 4 ( BACK )
            +0.0f, +0.0f,
            +0.0f, +1.0f,
            +1.0f, +0.0f,

            +1.0f, +0.0f,
            +0.0f, +1.0f,
            +1.0f, +1.0f,

            // SIDE 5 ( LEFT )
            +0.0f, +0.0f,
            +0.0f, +1.0f,
            +1.0f, +0.0f,

            +1.0f, +0.0f,
            +0.0f, +1.0f,
            +1.0f, +1.0f,

            // SIDE 6 ( RIGHT )
            +0.0f, +0.0f,
            +0.0f, +1.0f,
            +1.0f, +0.0f,

            +1.0f, +0.0f,
            +0.0f, +1.0f,
            +1.0f, +1.0f,

    };

        const float cubeNormals[] = 
        {
		// normal : x,y,z         
		// SIDE 1 ( TOP )
		+0.0f, +1.0f, +0.0f,
		+0.0f, +1.0f, +0.0f,
		+0.0f, +1.0f, +0.0f,

		+0.0f, +1.0f, +0.0f,
		+0.0f, +1.0f, +0.0f,
		+0.0f, +1.0f, +0.0f,

		// SIDE 2 ( BOTTOM )
		+0.0f, -1.0f, +0.0f,
		+0.0f, -1.0f, +0.0f,
		+0.0f, -1.0f, +0.0f,

		+0.0f, -1.0f, +0.0f,
		+0.0f, -1.0f, +0.0f,
		+0.0f, -1.0f, +0.0f,

		// SIDE 3 ( FRONT )
        +0.0f, +0.0f, -1.0f,
		+0.0f, +0.0f, -1.0f,
		+0.0f, +0.0f, -1.0f,

		+0.0f, +0.0f, -1.0f,
		+0.0f, +0.0f, -1.0f,
		+0.0f, +0.0f, -1.0f,

		// SIDE 4 ( BACK )
        +0.0f, +0.0f, +1.0f,
		+0.0f, +0.0f, +1.0f,
		+0.0f, +0.0f, +1.0f,

		+0.0f, +0.0f, +1.0f,
		+0.0f, +0.0f, +1.0f,
		+0.0f, +0.0f, +1.0f,

		// SIDE 5 ( LEFT )
		-1.0f, +0.0f, +0.0f,
		-1.0f, +0.0f, +0.0f,
		-1.0f, +0.0f, +0.0f,

		-1.0f, +0.0f, +0.0f,
		-1.0f, +0.0f, +0.0f,
		-1.0f, +0.0f, +0.0f,

		// SIDE 6 ( RIGHT )
		+1.0f, +0.0f, +0.0f,
		+1.0f, +0.0f, +0.0f,
		+1.0f, +0.0f, +0.0f,

		+1.0f, +0.0f, +0.0f,
		+1.0f, +0.0f, +0.0f,
		+1.0f, +0.0f, +0.0f,

	};

        // create vertex buffer for above position vertices
        //  A. initialize Buffer Descriptor... like glGenBuffer()
        D3D11_BUFFER_DESC d3d11BufferDescriptor;
        ZeroMemory((void *)&d3d11BufferDescriptor, sizeof(D3D11_BUFFER_DESC));
        d3d11BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
        d3d11BufferDescriptor.ByteWidth = sizeof(float) * _ARRAYSIZE(cubeVertices);
        d3d11BufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        // B. initialize subresource data structure to put data into the buffer
        D3D11_SUBRESOURCE_DATA d3d11SubresourceData;
        ZeroMemory((void *)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
        d3d11SubresourceData.pSysMem = cubeVertices;

        // C. Create the actual buffer
        hr = m_Device->CreateBuffer(&d3d11BufferDescriptor, &d3d11SubresourceData, &m_PositionBufferCube);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device::CreateBuffer() Failed for Position ...\n");
            fclose(gpFile);
            return (FALSE);
        }

        // Texture
        // A. initialize Buffer Descriptor... like glGenBuffer()
        ZeroMemory((void *)&d3d11BufferDescriptor, sizeof(D3D11_BUFFER_DESC));
        d3d11BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
        d3d11BufferDescriptor.ByteWidth = sizeof(float) * _ARRAYSIZE(cubeTexcoords);
        d3d11BufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        // B. initialize subresource data structure to put data into the buffer
        ZeroMemory((void *)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
        d3d11SubresourceData.pSysMem = cubeTexcoords;

        // C. Create the actual buffer
        hr = m_Device->CreateBuffer(&d3d11BufferDescriptor, &d3d11SubresourceData, &m_TextureBufferCube);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device::CreateBuffer() Failed for Texcoord ...\n");
            fclose(gpFile);
            return (FALSE);
        }

        // Normals
        ZeroMemory((void *)&d3d11BufferDescriptor, sizeof(D3D11_BUFFER_DESC));
        d3d11BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
        d3d11BufferDescriptor.ByteWidth = sizeof(float) * _ARRAYSIZE(cubeNormals);
        d3d11BufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        d3d11BufferDescriptor.CPUAccessFlags = 0;
        d3d11BufferDescriptor.MiscFlags = 0;
        d3d11BufferDescriptor.StructureByteStride = 0;

        // Give the subresource structure a pointer to the index data.
        ZeroMemory((void *)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
        d3d11SubresourceData.pSysMem = cubeNormals;
        d3d11SubresourceData.SysMemPitch = 0;
        d3d11SubresourceData.SysMemSlicePitch = 0;

        // Create the index buffer.
        hr = m_Device->CreateBuffer(&d3d11BufferDescriptor, &d3d11SubresourceData, &m_NormalBufferCube);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+");
            fprintf(gpFile, "gpID3D11Device::CreateBuffer() Failed for Normal ...\n");
            fclose(gpFile);
            return FALSE;
        }

        // **************** For Ground ********************
        const float quadVertices[] = 
        {
            // position : x,y,z
            // SIDE 2 ( BOTTOM )
            +1.0f, -1.0f, -1.0f,
            +1.0f, -1.0f, +1.0f,
            -1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            +1.0f, -1.0f, +1.0f,
            -1.0f, -1.0f, +1.0f,
        };

        const float quadTexcoords[] =
        {
            // Tex : u,v
            // SIDE 2 ( BOTTOM )
            +0.0f, +0.0f,
            +0.0f, +10.0f,
            +10.0f, +0.0f,

            +10.0f, +0.0f,
            +0.0f, +10.0f,
            +10.0f, +10.0f,

        };

        const float quadNormals[] = 
        {
            // normal : x,y,z         
            // SIDE 2 ( BOTTOM )
            +0.0f, -1.0f, +0.0f,
            +0.0f, -1.0f, +0.0f,
            +0.0f, -1.0f, +0.0f,

            +0.0f, -1.0f, +0.0f,
            +0.0f, -1.0f, +0.0f,
            +0.0f, -1.0f, +0.0f,

        };
        // create vertex buffer for above position vertices
        //  A. initialize Buffer Descriptor... like glGenBuffer()
        ZeroMemory((void *)&d3d11BufferDescriptor, sizeof(D3D11_BUFFER_DESC));
        d3d11BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
        d3d11BufferDescriptor.ByteWidth = sizeof(float) * _ARRAYSIZE(quadVertices);
        d3d11BufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        // B. initialize subresource data structure to put data into the buffer
        ZeroMemory((void *)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
        d3d11SubresourceData.pSysMem = quadVertices;

        // C. Create the actual buffer
        hr = m_Device->CreateBuffer(&d3d11BufferDescriptor, &d3d11SubresourceData, &m_PositionBufferQuad);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device::CreateBuffer() Failed for Position ...\n");
            fclose(gpFile);
            return (FALSE);
        }

        // Texture
        // A. initialize Buffer Descriptor... like glGenBuffer()
        ZeroMemory((void *)&d3d11BufferDescriptor, sizeof(D3D11_BUFFER_DESC));
        d3d11BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
        d3d11BufferDescriptor.ByteWidth = sizeof(float) * _ARRAYSIZE(quadTexcoords);
        d3d11BufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        // B. initialize subresource data structure to put data into the buffer
        ZeroMemory((void *)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
        d3d11SubresourceData.pSysMem = quadTexcoords;

        // C. Create the actual buffer
        hr = m_Device->CreateBuffer(&d3d11BufferDescriptor, &d3d11SubresourceData, &m_TextureBufferQuad);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device::CreateBuffer() Failed for Texcoord ...\n");
            fclose(gpFile);
            return (FALSE);
        }

        // Normals
        ZeroMemory((void *)&d3d11BufferDescriptor, sizeof(D3D11_BUFFER_DESC));
        d3d11BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
        d3d11BufferDescriptor.ByteWidth = sizeof(float) * _ARRAYSIZE(quadNormals);
        d3d11BufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        d3d11BufferDescriptor.CPUAccessFlags = 0;
        d3d11BufferDescriptor.MiscFlags = 0;
        d3d11BufferDescriptor.StructureByteStride = 0;

        // Give the subresource structure a pointer to the index data.
        ZeroMemory((void *)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
        d3d11SubresourceData.pSysMem = quadNormals;
        d3d11SubresourceData.SysMemPitch = 0;
        d3d11SubresourceData.SysMemSlicePitch = 0;

        // Create the index buffer.
        hr = m_Device->CreateBuffer(&d3d11BufferDescriptor, &d3d11SubresourceData, &m_NormalBufferQuad);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+");
            fprintf(gpFile, "gpID3D11Device::CreateBuffer() Failed for Normal ...\n");
            fclose(gpFile);
            return FALSE;
        }


        depthPass = new DepthShader(m_SwapChain, m_Device, m_DeviceContext, m_RenderTargetView);
        if (depthPass->Initialize() == FALSE)
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+");
            fprintf(gpFile, "DepthShader init Failed...\n");
            fclose(gpFile);
            return FALSE;
        }

        shadow = new ShadowShader(m_SwapChain, m_Device, m_DeviceContext, m_RenderTargetView);
        if (shadow->Initialize() == FALSE)
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+");
            fprintf(gpFile, "ShadowShader init Failed...\n");
            fclose(gpFile);
            return FALSE;
        }

        m_light = new Light;
        m_light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
        m_light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
        // m_light->SetLookAt(0.0f, 0.0f, 0.0f);
        m_light->GenerateOrthoMatrix(20.f, SHADOWMAP_NEAR, SHADOWMAP_DEPTH);

        m_light2 = new Light;
        m_light2->SetDiffuseColor(0.2f, 0.1f, 0.8f, 1.0f);
        // m_light2->SetLookAt(0.0f, 8.0f, 5.0f);
        m_light2->GenerateOrthoMatrix(20.f, SHADOWMAP_NEAR, SHADOWMAP_DEPTH);

        m_light3 = new Light;
        m_light3->SetDiffuseColor(0.98f, 0.537f, 0.906f, 1.0f);
        // m_light3->SetLookAt(2.0f, 10.0f, 10.0f);
        m_light3->GenerateOrthoMatrix(20.f, SHADOWMAP_NEAR, SHADOWMAP_DEPTH);

        m_RenderTexture = new RenderTextureClass();
        if (m_RenderTexture->Initialize(m_Device, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR, 1) == FALSE)
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+");
            fprintf(gpFile, "RenderTextureClass() Failed for m_RenderTexture ...\n");
            fclose(gpFile);
        }

        m_RenderTexture1 = new RenderTextureClass();
        if (m_RenderTexture1->Initialize(m_Device, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR, 1) == FALSE)
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+");
            fprintf(gpFile, "RenderTextureClass() Failed for m_RenderTexture1 ...\n");
            fclose(gpFile);
        }

        m_RenderTexture2 = new RenderTextureClass();
        if (m_RenderTexture2->Initialize(m_Device, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR, 1) == FALSE)
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+");
            fprintf(gpFile, "RenderTextureClass() Failed for m_RenderTexture2 ...\n");
            fclose(gpFile);
        }

        // hr = CreateWICTextureFromFile(m_Device, m_DeviceContext, L"assets/Textures/parallexmap/wall.jpg", nullptr, &m_MarbleTexture);
        hr = CreateWICTextureFromFile(m_Device, m_DeviceContext, L"assets/Textures/parallexmap/bricks2.jpg", nullptr, &m_MarbleTexture);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "Texture couldn't be loaded : DUDVMap ...\n");
            fclose(gpFile);
        }

       
        return TRUE;
    }

    void Ground(Camera camera)
    {
        fopen_s(&gpFile, gszLogFilePathName, "a+");
        fprintf(gpFile, "Ground...\n");
        fclose(gpFile);

        XMMATRIX modelMatrix = XMMatrixIdentity();
        XMMATRIX viewMatrix = XMMatrixIdentity();
        modelMatrix = XMMatrixTranslation(0.0f, 0.0f, 2.0f);
        viewMatrix = camera.getViewMatrix();
        depthPass->SetShaderParameters(modelMatrix, viewMatrix, PerspectiveProjectionMatrix, 6);

        // set Position Buffer into InputAssembly stage of pipeline (glVertexAttribPointer() che last 2 para)
        UINT stride = sizeof(float) * 3;
        UINT offset = 0;
        m_DeviceContext->IASetVertexBuffers(0, 1, &m_PositionBufferQuad, &stride, &offset);

        // Set texture buffer into Input Assembly stage of pipeline
        stride = sizeof(float) * 2;
        offset = 0;
        m_DeviceContext->IASetVertexBuffers(1, 1, &m_TextureBufferCube, &stride, &offset);

        // Set normal buffer into Input Assembly stage of pipeline
        stride = sizeof(float) * 3;
        offset = 0;
        m_DeviceContext->IASetVertexBuffers(2, 1, &m_NormalBufferCube, &stride, &offset);

        depthPass->Render(6);
    }

    void RenderShadow(Camera camera)
    {
        XMMATRIX translateMatrix = XMMatrixIdentity();
        XMMATRIX lightViewMatrix = XMMatrixIdentity();
        XMMATRIX lightProjectionMatrix = XMMatrixIdentity();
        static float rotation = 0;
        static float lightAngle = 270.f;
        float radians;
        float frameTime;
        static float lightPosX = 9.f;
        // m_light->SetPosition(5.0f, 8.0f, -5.0f);
        // m_light->GenerateViewMatrix();

        // m_light2->SetPosition(-5.0f, 8.0f, -5.0f);
        // m_light2->GenerateViewMatrix();
        frameTime = 0.1f;

        lightPosX -= 0.003f * frameTime;

        lightAngle -= 0.03 * frameTime;
        if (lightAngle < 90.f)
        {
            lightAngle = 270.f;

            lightPosX = 9.f;
        }

        radians = lightAngle * 0.0174532925f;

        m_light->SetDirection(sinf(radians), cosf(radians), 0.f);

        m_light->SetPosition(lightPosX, 8.0f, -0.1f);
        m_light->SetLookAt(-lightPosX, 0.0f, 0.0f);
        m_light->GenerateViewMatrix();

        m_light2->SetDirection(sinf(radians), cosf(radians), 0.f);

        m_light2->SetPosition(lightPosX, 8.0f, -0.5f);
        m_light2->SetLookAt(-lightPosX + 5, 0.0f, 0.0f);
        m_light2->GenerateViewMatrix();

        m_light3->SetDirection(sinf(radians), cosf(radians), 0.f);

        m_light3->SetPosition(lightPosX, 8.0f, -0.3f);
        m_light3->SetLookAt(-lightPosX, 0.0f, 0.0f);
        m_light3->GenerateViewMatrix();

        RenderDepthToTexture(translateMatrix, lightViewMatrix, lightProjectionMatrix, m_RenderTexture, m_light, camera);
        RenderDepthToTexture(translateMatrix, lightViewMatrix, lightProjectionMatrix, m_RenderTexture1, m_light2, camera);
        RenderDepthToTexture(translateMatrix, lightViewMatrix, lightProjectionMatrix, m_RenderTexture2, m_light3, camera);
        Render(camera);
    }

    void RenderDepthToTexture(XMMATRIX translateMatrix, XMMATRIX lightViewMatrix, XMMATRIX lightProjectionMatrix, RenderTextureClass* m_RenderTexture, Light* m_Light, Camera camera)
    {
        m_RenderTexture->SetRenderTarget(m_DeviceContext);
        m_RenderTexture->ClearRenderTarget(m_DeviceContext, 0.0f, 0.2f, 0.2f, 1.0f);

        m_Light->GetViewMatrix(lightViewMatrix);
        m_Light->GetOrthoMatrix(lightProjectionMatrix);
        XMMATRIX viewMatrix = camera.getViewMatrix();

        translateMatrix = XMMatrixTranslation(-2.0f, 2.0f, 0.0f);
        depthPass->SetShaderParameters(translateMatrix, lightViewMatrix, lightProjectionMatrix, 36);
        // depthPass->SetShaderParameters(translateMatrix, viewMatrix, PerspectiveProjectionMatrix, 36);

        UINT stride = sizeof(float) * 3;
        UINT offset = 0;
        m_DeviceContext->IASetVertexBuffers(0, 1, &m_PositionBufferCube, &stride, &offset);

        depthPass->Render(36);

        // 2nd model
        translateMatrix = XMMatrixTranslation(2.0f, 2.0f, 0.0f);
        depthPass->SetShaderParameters(translateMatrix, lightViewMatrix, lightProjectionMatrix, 36);
        // depthPass->SetShaderParameters(translateMatrix, viewMatrix, PerspectiveProjectionMatrix, 36);

        stride = sizeof(float) * 3;
        offset = 0;
        m_DeviceContext->IASetVertexBuffers(0, 1, &m_PositionBufferCube, &stride, &offset);

        depthPass->Render(36);

        // ground
        translateMatrix = XMMatrixTranslation(0.0f, -1.0f, 0.0f)  * XMMatrixScaling(10.0f, 1.0f, 10.0f);
        depthPass->SetShaderParameters(translateMatrix, lightViewMatrix, lightProjectionMatrix, 6);
        // depthPass->SetShaderParameters(translateMatrix, viewMatrix, PerspectiveProjectionMatrix, 6);

        stride = sizeof(float) * 3;
        offset = 0;
        m_DeviceContext->IASetVertexBuffers(0, 1, &m_PositionBufferQuad, &stride, &offset);

        depthPass->Render(6);

        SetBackBufferRenderTarget();
        ResetViewport();
    }

    void Render(Camera camera)
    {
        XMMATRIX worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, lightViewMatrix2, lightProjectionMatrix2, lightViewMatrix3, lightProjectionMatrix3;
        float m_shadowMapBias = 0.0022f;
        float color[4];

        color[0] = 0.0f;
        color[1] = 0.3f;
        color[2] = 0.3f;
        color[3] = 1.0f;
        m_DeviceContext->ClearRenderTargetView(m_renderTargetView, color);
        m_DeviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        m_light->GetViewMatrix(lightViewMatrix);
        m_light->GetOrthoMatrix(lightProjectionMatrix);

        m_light2->GetViewMatrix(lightViewMatrix2);
        m_light2->GetOrthoMatrix(lightProjectionMatrix2);

        m_light3->GetViewMatrix(lightViewMatrix3);
        m_light3->GetOrthoMatrix(lightProjectionMatrix3);
        
        // model 1
        worldMatrix = XMMatrixTranslation(-2.0f, 2.0f, 0.0f);
        viewMatrix = camera.getViewMatrix();
        shadow->SetShaderParameters(worldMatrix, viewMatrix, PerspectiveProjectionMatrix, 
                        lightViewMatrix, lightProjectionMatrix, m_MarbleTexture,  m_RenderTexture->GetShaderResourceView(),
                        m_light->GetAmbientColor(), m_light->GetDiffuseColor(), m_light->GetPosition(),
                        m_shadowMapBias, lightViewMatrix2, lightProjectionMatrix2,
                        m_RenderTexture1->GetShaderResourceView(), m_light2->GetPosition(), m_light2->GetDiffuseColor(), 
                        lightViewMatrix3, lightProjectionMatrix3, m_RenderTexture2->GetShaderResourceView(),
                        m_light3->GetPosition(), m_light3->GetDiffuseColor(), 36);

        UINT stride = sizeof(float) * 3;
        UINT offset = 0;
        m_DeviceContext->IASetVertexBuffers(0, 1, &m_PositionBufferCube, &stride, &offset);

        stride = sizeof(float) * 2;
        offset = 0;
        m_DeviceContext->IASetVertexBuffers(1, 1, &m_TextureBufferCube, &stride, &offset);

        stride = sizeof(float) * 3;
        offset = 0;
        m_DeviceContext->IASetVertexBuffers(2, 1, &m_NormalBufferCube, &stride, &offset);

        shadow->Render(36);

        // model 2
        worldMatrix = XMMatrixTranslation(2.0f, 2.0f, 0.0f);
        viewMatrix = camera.getViewMatrix();
        shadow->SetShaderParameters(worldMatrix, viewMatrix, PerspectiveProjectionMatrix, 
                        lightViewMatrix, lightProjectionMatrix, m_MarbleTexture,  m_RenderTexture->GetShaderResourceView(),
                        m_light->GetAmbientColor(), m_light->GetDiffuseColor(), m_light->GetPosition(),
                        m_shadowMapBias, lightViewMatrix2, lightProjectionMatrix2,
                        m_RenderTexture1->GetShaderResourceView(), m_light2->GetPosition(), m_light2->GetDiffuseColor(), 
                        lightViewMatrix3, lightProjectionMatrix3, m_RenderTexture2->GetShaderResourceView(),
                        m_light3->GetPosition(), m_light3->GetDiffuseColor(), 36);

        stride = sizeof(float) * 3;
        offset = 0;
        m_DeviceContext->IASetVertexBuffers(0, 1, &m_PositionBufferCube, &stride, &offset);

        stride = sizeof(float) * 2;
        offset = 0;
        m_DeviceContext->IASetVertexBuffers(1, 1, &m_TextureBufferCube, &stride, &offset);

        stride = sizeof(float) * 3;
        offset = 0;
        m_DeviceContext->IASetVertexBuffers(2, 1, &m_NormalBufferCube, &stride, &offset);

        shadow->Render(36);

        // ground
        worldMatrix = XMMatrixTranslation(0.0f, -1.0f, 0.0f) * XMMatrixScaling(10.0f, 1.0f, 10.0f);
        viewMatrix = camera.getViewMatrix();
        shadow->SetShaderParameters(worldMatrix, viewMatrix, PerspectiveProjectionMatrix, 
                        lightViewMatrix, lightProjectionMatrix, m_MarbleTexture,  m_RenderTexture->GetShaderResourceView(),
                        m_light->GetAmbientColor(), m_light->GetDiffuseColor(), m_light->GetPosition(),
                        m_shadowMapBias, lightViewMatrix2, lightProjectionMatrix2,
                        m_RenderTexture1->GetShaderResourceView(), m_light2->GetPosition(), m_light2->GetDiffuseColor(), 
                        lightViewMatrix3, lightProjectionMatrix3, m_RenderTexture2->GetShaderResourceView(),
                        m_light3->GetPosition(), m_light3->GetDiffuseColor(), 6);

        stride = sizeof(float) * 3;
        offset = 0;
        m_DeviceContext->IASetVertexBuffers(0, 1, &m_PositionBufferCube, &stride, &offset);

        stride = sizeof(float) * 2;
        offset = 0;
        m_DeviceContext->IASetVertexBuffers(1, 1, &m_TextureBufferCube, &stride, &offset);

        stride = sizeof(float) * 3;
        offset = 0;
        m_DeviceContext->IASetVertexBuffers(2, 1, &m_NormalBufferCube, &stride, &offset);

        shadow->Render(6);
    }

    void SetBackBufferRenderTarget()
    {
        m_DeviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
    }

    void ResetViewport()
    {
        m_DeviceContext->RSSetViewports(1, &m_viewport);
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
    IDXGISwapChain *m_SwapChain;
    ID3D11Device *m_Device;
    ID3D11DeviceContext *m_DeviceContext;
    ID3D11RenderTargetView *m_RenderTargetView;

    ShadowShader *shadow = nullptr;
    DepthShader *depthPass = nullptr;
    Light *m_light = nullptr;
    Light *m_light2 = nullptr;
    Light *m_light3 = nullptr;
    RenderTextureClass *m_RenderTexture = nullptr;
    RenderTextureClass *m_RenderTexture1 = nullptr;
    RenderTextureClass *m_RenderTexture2 = nullptr;

    ID3D11Buffer *m_PositionBufferCube = nullptr;
    ID3D11Buffer *m_TextureBufferCube = nullptr;
    ID3D11Buffer *m_NormalBufferCube = nullptr;
    ID3D11Buffer *m_PositionBufferQuad = nullptr;
    ID3D11Buffer *m_TextureBufferQuad = nullptr;
    ID3D11Buffer *m_NormalBufferQuad = nullptr;

    ID3D11RenderTargetView *m_renderTargetView;
    ID3D11DepthStencilView *m_depthStencilView;
    D3D11_VIEWPORT m_viewport;

    ID3D11ShaderResourceView *m_MarbleTexture = nullptr;

    Camera light1;
    Camera light2;
    Camera light3;
};

#endif