
#ifndef REFLECTIONQUAD_H
#define REFLECTIONQUAD_H

// D3D related headers
#include <dxgi.h>
#include <D3D11.h>
#include <d3dcompiler.h>

#pragma warning(disable : 4838)
#include "XNAMath_204/xnamath.h"

#include "../../Common/DXShaders.h"
// #include "../../Common/MyLogger.h"
// #include "../../Common/DDSTextureLoader.h"
#include "../../Common/Camera.h"
#include "../Model/TextureLoader.h"

// #define RENDER_TEXTURE_WIDTH    1024
// #define RENDER_TEXTURE_HEIGHT   1024

// #define REFLECTION_FBO_WIDTH 1920
// #define REFLECTION_FBO_HEIGHT 1080

// #define REFRACTION_FBO_WIDTH 1920
// #define REFRACTION_FBO_HEIGHT 1080

// #define WATER_WAVE_SPEED 0.002

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

class ReflectionQuad
{
public:
    XMMATRIX m_ProjectionMatrix;

    struct MatrixBufferType
    {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };

    struct ReflectionBufferType
    {
        float moveOffset;
        XMMATRIX reflectionMatrix;
        XMVECTOR Ld;
        XMVECTOR Kd;
        XMVECTOR LightPosition;
        XMVECTOR CamPosition;
    };

    ReflectionQuad()
    {
    }

    ReflectionQuad(IDXGISwapChain *SwapChain,
              ID3D11Device *Device,
              ID3D11DeviceContext *DeviceContext,
              ID3D11RenderTargetView *RenderTargetView
              /*std::shared_ptr<MyLogger> _log*/)
        : m_SwapChain(SwapChain),
          m_Device(Device),
          m_DeviceContext(DeviceContext),
          m_RenderTargetView(RenderTargetView)
    // m_log(_log)
    {
        if (!Initialize())
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "waterQUad.Initialize() Failed ...\n");
            fclose(gpFile);
        }
    }

    ~ReflectionQuad()
    {
        Uninitialize();
    }

    BOOL Initialize(void)
    {
        shader = new DXShaders();

        HRESULT hr = S_OK;
        // vertex shader
        ID3DBlob *pID3DBlob_VertexShaderCode = NULL;

        fopen_s(&gpFile, gszLogFilePathName, "a+");
        fprintf(gpFile, "Debug Quad  :: %ls\n", __FUNCTIONW__);
        fclose(gpFile);

        // if (!shader->CreateAndCompileVertexShaderObjects("./src/Shaders/Water/Quad_VS.hlsl", VERTEX_SHADER, &gpID3D11VertexShader, &pID3DBlob_VertexShaderCode))
        if (!shader->CreateAndCompileVertexShaderObjects("./src/Shaders/Reflection/reflection_vs.hlsl", VERTEX_SHADER, &gpID3D11VertexShader, &pID3DBlob_VertexShaderCode))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+");
            fprintf(gpFile, "shader->CreateAndCompileShaderObjects() Failed for Vertex Shader...    :: %ls\n", __FUNCTIONW__);
            fclose(gpFile);
            return FALSE;
        }

        // Set this vertex shader in VErtex Shader Stage of Pipeline
        // m_DeviceContext->VSSetShader(gpID3D11VertexShader, NULL, 0);

        ID3DBlob *pID3DBlob_PixelShaderCode = NULL;
        if (shader->CreateAndCompilePixelShaderObjects("./src/Shaders/Reflection/reflection_ps.hlsl", PIXEL_SHADER, &gpID3D11PixelShader, &pID3DBlob_PixelShaderCode) == FALSE)
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+");
            fprintf(gpFile, "shader->CreateAndCompilePixelShaderObjects() Failed for Pixel Shader...    :: %ls\n", __FUNCTIONW__);
            fclose(gpFile);
            return FALSE;
        }
        // Set pixel shader in the Pixel Shader stage of Pipeline
        // m_DeviceContext->PSSetShader(gpID3D11PixelShader, NULL, 0);

        // ############ Input Layout [just like Attributes in OpenGL] ##############
        D3D11_INPUT_ELEMENT_DESC d3d11InputElementDescriptor[3];
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
        d3d11InputElementDescriptor[1].InputSlot = 1;
        d3d11InputElementDescriptor[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        d3d11InputElementDescriptor[1].InstanceDataStepRate = 0;

        d3d11InputElementDescriptor[2].SemanticName = "NORMAL";
        d3d11InputElementDescriptor[2].SemanticIndex = 0;
        d3d11InputElementDescriptor[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        d3d11InputElementDescriptor[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        d3d11InputElementDescriptor[2].InputSlot = 2;
        d3d11InputElementDescriptor[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        d3d11InputElementDescriptor[2].InstanceDataStepRate = 0;

        // create input layout
        hr = m_Device->CreateInputLayout(d3d11InputElementDescriptor, _ARRAYSIZE(d3d11InputElementDescriptor), pID3DBlob_VertexShaderCode->GetBufferPointer(),
                                         pID3DBlob_VertexShaderCode->GetBufferSize(), &gpID3D11InputLayout);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device->CreateInputLayout() Failed ...\n");
            fclose(gpFile);
            return FALSE;
        }

        // Set this Input Layout in Input Assembly Stages of Pipeline
        // m_DeviceContext->IASetInputLayout(gpID3D11InputLayout);

        // Release the Blob
        pID3DBlob_VertexShaderCode->Release();
        pID3DBlob_VertexShaderCode = NULL;

        pID3DBlob_PixelShaderCode->Release();
        pID3DBlob_PixelShaderCode = NULL;

        // ############ Geometry ##############
        const float cubeVertices[] =
            {
                // position : x,y,z
                // SIDE 3 ( FRONT )
                -50.0f, 0.0f, 50.0f,
                50.0f, 0.0f, 50.0f,
                -50.0f, 0.0f,-50.0f,
                -50.0f, 0.0f,-50.0f,
                50.0f, 0.0f, 50.0f,
                50.0f, 0.0f,-50.0f,

            };

        const float cubeTexcoords[] =
            {
                // Tex : u,v
                // SIDE 3 ( FRONT )
                0.0f, 0.0f,
                1.0f, 0.0f,
                0.0f, 1.0f,
                0.0f, 1.0f,
                1.0f, 0.0f,
                1.0f, 1.0f,

            };

        const float cubeNormals[] = 
        {
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
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
        hr = m_Device->CreateBuffer(&d3d11BufferDescriptor, &d3d11SubresourceData, &gpID3D11Buffer_PositionBuffer);
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
        hr = m_Device->CreateBuffer(&d3d11BufferDescriptor, &d3d11SubresourceData, &gpID3D11Buffer_TextureBuffer);
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
        d3d11BufferDescriptor.ByteWidth = sizeof(unsigned long) * _ARRAYSIZE(cubeNormals);
        d3d11BufferDescriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
        d3d11BufferDescriptor.CPUAccessFlags = 0;
        d3d11BufferDescriptor.MiscFlags = 0;
        d3d11BufferDescriptor.StructureByteStride = 0;

        // Give the subresource structure a pointer to the index data.
        ZeroMemory((void *)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
        d3d11SubresourceData.pSysMem = cubeNormals;
        d3d11SubresourceData.SysMemPitch = 0;
        d3d11SubresourceData.SysMemSlicePitch = 0;

        // Create the index buffer.
        hr = m_Device->CreateBuffer(&d3d11BufferDescriptor, &d3d11SubresourceData, &m_NormalBuffer);
        if(FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+");
            fprintf(gpFile, "gpID3D11Device::CreateBuffer() Failed for Normal ...\n");
            fclose(gpFile);
            return FALSE;
        }

        // Constant Buffer (for uniforms)
        // A. initialize Buffer descriptor
        ZeroMemory((void *)&d3d11BufferDescriptor, sizeof(D3D11_BUFFER_DESC));
        d3d11BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
        d3d11BufferDescriptor.ByteWidth = sizeof(MatrixBufferType);
        d3d11BufferDescriptor.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

        // B. Create Actual Buffer
        hr = m_Device->CreateBuffer(&d3d11BufferDescriptor, NULL, &matrixConstantBuffer);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device::CreateBuffer() Failed for Constants ...\n");
            fclose(gpFile);
            return (FALSE);
        }

        D3D11_BUFFER_DESC reflectionMatrixBuffer;
        ZeroMemory((void *)&reflectionMatrixBuffer, sizeof(D3D11_BUFFER_DESC));
        reflectionMatrixBuffer.Usage = D3D11_USAGE_DEFAULT;
        reflectionMatrixBuffer.ByteWidth = sizeof(ReflectionBufferType);
        reflectionMatrixBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

        // B. Create Actual Buffer
        hr = m_Device->CreateBuffer(&reflectionMatrixBuffer, NULL, &reflectionConstantBuffer);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device::CreateBuffer() Failed for Constants ...\n");
            fclose(gpFile);
            return (FALSE);
        }

        // Create Texture Sampler State
        D3D11_SAMPLER_DESC d3d11SamplerDescriptor;
        ZeroMemory((void *)&d3d11SamplerDescriptor, sizeof(D3D11_SAMPLER_DESC));
        d3d11SamplerDescriptor.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        d3d11SamplerDescriptor.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        d3d11SamplerDescriptor.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        d3d11SamplerDescriptor.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

        hr = m_Device->CreateSamplerState(&d3d11SamplerDescriptor, &gpID3D11SamplerState_Texture);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device->CreateSamplerState() Failed ...\n");
            fclose(gpFile);
            return (FALSE);
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
            fprintf(gpFile, "Texture couldn't be loaded : DUDVMap ...\n");
            fclose(gpFile);
        }

        return TRUE;
    }

    void RenderObject(XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMMATRIX reflectionMatrix, XMVECTOR camPos, ID3D11ShaderResourceView *texture, ID3D11ShaderResourceView *textureReflection, ID3D11ShaderResourceView *textureRefraction)
    {
        // code

        m_DeviceContext->IASetInputLayout(gpID3D11InputLayout);
        // set Position Buffer into InputAssembly stage of pipeline (glVertexAttribPointer() che last 2 para)
        UINT stride = sizeof(float) * 3;
        UINT offset = 0;
        m_DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_PositionBuffer, &stride, &offset);

        // Set color buffer into Input Assembly stage of pipeline
        stride = sizeof(float) * 2;
        offset = 0;
        m_DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_TextureBuffer, &stride, &offset);

        stride = sizeof(float) * 3;
        offset = 0;
        m_DeviceContext->IASetVertexBuffers(2, 1, &m_NormalBuffer, &stride, &offset);

        // 2. Render
        // Set primitive topology in Input Assembly Stage
        m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // B. Put them into ConstantBuffer
        MatrixBufferType constBuffer;
        ZeroMemory((void *)&constBuffer, sizeof(MatrixBufferType));
        // constBuffer.world = XMMatrixTranspose(worldMatrix);
        // constBuffer.view = XMMatrixTranspose(viewMatrix);
        // constBuffer.projection = XMMatrixTranspose(PerspectiveProjectionMatrix);
        constBuffer.world = (worldMatrix);
        constBuffer.view = (viewMatrix);
        constBuffer.projection = (PerspectiveProjectionMatrix);

        // C. Push them into the shader (like glUniformMatrix4fv())
        m_DeviceContext->UpdateSubresource(matrixConstantBuffer, 0, NULL, &constBuffer, 0, 0);

        // B. Put them into ConstantBuffer
        ReflectionBufferType constBuffer_r;
        ZeroMemory((void *)&constBuffer_r, sizeof(ReflectionBufferType));
        // constBuffer_r.reflectionMatrix = XMMatrixTranspose(reflectionMatrix);
        constBuffer_r.reflectionMatrix = (reflectionMatrix);
        constBuffer_r.Ld = XMVectorSet(LightDiffuse[0], LightDiffuse[1], LightDiffuse[2], LightDiffuse[3]);
		constBuffer_r.Kd = XMVectorSet(MaterialDiffuse[0], MaterialDiffuse[1], MaterialDiffuse[2], MaterialDiffuse[3]);
		constBuffer_r.LightPosition = XMVectorSet(LightPosition[0], LightPosition[1], LightPosition[2], LightPosition[3]);
		constBuffer_r.CamPosition = camPos;
		constBuffer_r.moveOffset = moveFactor + 0.2/4;
        moveFactor += 0.00001f;

        // C. Push them into the shader (like glUniformMatrix4fv())
        m_DeviceContext->UpdateSubresource(reflectionConstantBuffer, 0, NULL, &constBuffer_r, 0, 0);

        m_DeviceContext->VSSetShader(gpID3D11VertexShader, NULL, 0);
        m_DeviceContext->PSSetShader(gpID3D11PixelShader, NULL, 0);
        m_DeviceContext->VSSetConstantBuffers(0, 1, &matrixConstantBuffer);
        m_DeviceContext->VSSetConstantBuffers(1, 1, &reflectionConstantBuffer);
        m_DeviceContext->PSSetConstantBuffers(0, 1, &reflectionConstantBuffer);

        // m_DeviceContext->PSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer);

        // set Shader View in Pixel Shader
        m_DeviceContext->PSSetShaderResources(0, 1, &textureRefraction);
        m_DeviceContext->PSSetShaderResources(1, 1, &textureReflection);
        m_DeviceContext->PSSetShaderResources(2, 1, &gpID3D11ShaderResourceView_DUDVMap);
        m_DeviceContext->PSSetShaderResources(3, 1, &gpID3D11ShaderResourceView_NormalMap);

        // Set Sampler State in Pixel Texture (corresponds to 2nd line in Pixel shader)
        m_DeviceContext->PSSetSamplers(0, 1, &gpID3D11SamplerState_Texture);

        // draw primitive
        m_DeviceContext->Draw(6, 0);
    }



    void Uninitialize(void)
    {
        if (gpID3D11SamplerState_Texture)
        {
            gpID3D11SamplerState_Texture->Release();
            gpID3D11SamplerState_Texture = NULL;
        }

        if (gpID3D11ShaderResourceView_Smiley)
        {
            gpID3D11ShaderResourceView_Smiley->Release();
            gpID3D11ShaderResourceView_Smiley = NULL;
        }

        if (gpID3D11ShaderResourceView_NormalMap)
        {
            gpID3D11ShaderResourceView_NormalMap->Release();
            gpID3D11ShaderResourceView_NormalMap = NULL;
        }

        if (matrixConstantBuffer)
        {
            matrixConstantBuffer->Release();
            matrixConstantBuffer = NULL;
        }

        if (reflectionConstantBuffer)
        {
            reflectionConstantBuffer->Release();
            reflectionConstantBuffer = NULL;
        }

        if (gpID3D11Buffer_TextureBuffer)
        {
            gpID3D11Buffer_TextureBuffer->Release();
            gpID3D11Buffer_TextureBuffer = NULL;
        }

        if (gpID3D11Buffer_PositionBuffer)
        {
            gpID3D11Buffer_PositionBuffer->Release();
            gpID3D11Buffer_PositionBuffer = NULL;
        }

        if (gpID3D11InputLayout)
        {
            gpID3D11InputLayout->Release();
            gpID3D11InputLayout = NULL;
        }

        if (gpID3D11PixelShader)
        {
            gpID3D11PixelShader->Release();
            gpID3D11PixelShader = NULL;
        }

        if (gpID3D11VertexShader)
        {
            gpID3D11VertexShader->Release();
            gpID3D11VertexShader = NULL;
        }
    }

private:
    IDXGISwapChain *m_SwapChain;
    ID3D11Device *m_Device;
    ID3D11DeviceContext *m_DeviceContext;
    ID3D11RenderTargetView *m_RenderTargetView;

    ID3D11VertexShader *gpID3D11VertexShader;
    ID3D11PixelShader *gpID3D11PixelShader;
    ID3D11InputLayout *gpID3D11InputLayout;
    ID3D11Buffer *gpID3D11Buffer_PositionBuffer;
    ID3D11Buffer *gpID3D11Buffer_TextureBuffer;
    ID3D11Buffer *matrixConstantBuffer;
    ID3D11Buffer *reflectionConstantBuffer;
    ID3D11ShaderResourceView *gpID3D11ShaderResourceView_Smiley = nullptr;
    ID3D11ShaderResourceView *gpID3D11ShaderResourceView_NormalMap;
    ID3D11ShaderResourceView *gpID3D11ShaderResourceView_DUDVMap;
    ID3D11SamplerState *gpID3D11SamplerState_Texture;

    ID3D11Buffer *m_NormalBuffer;

    DXShaders *shader = NULL;

    float angle = 0.0f;
    float moveFactor = 0.0f;

    float LightDiffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    float MaterialDiffuse[4] = {0.5f, 0.5f, 0.5f, 1.0f};
    float LightPosition[4] = {0.0f, 10.0f, 0.0f, 1.0f};
};

#endif