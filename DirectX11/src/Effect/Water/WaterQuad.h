#ifndef WATERQUAD_H
#define WATERQUAD_H

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


class WaterQuad
{
    public:
        XMMATRIX m_ProjectionMatrix;
        struct CBUFFER_QUAD
        {
            // XMMATRIX World;
            // XMMATRIX View;
            // XMMATRIX ProjectionMatrix;
            XMMATRIX wvpMatrix;
        };

        WaterQuad()
        {

        }

        WaterQuad(IDXGISwapChain *SwapChain,
            ID3D11Device *Device,
            ID3D11DeviceContext *DeviceContext,
            ID3D11RenderTargetView *RenderTargetView
            /*std::shared_ptr<MyLogger> _log*/)                             
                                                        :m_SwapChain(SwapChain),
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

        ~WaterQuad()
        {
            Uninitialize();
        }

        BOOL Initialize(void)
        {
            shader = new DXShaders();
            
            HRESULT hr = S_OK;
            // vertex shader
            ID3DBlob *pID3DBlob_VertexShaderCode = NULL;
            
            if (!shader->CreateAndCompileVertexShaderObjects("./src/Shaders/Water/Cube_VS.hlsl", VERTEX_SHADER, &gpID3D11VertexShader, &pID3DBlob_VertexShaderCode))
            {
                fopen_s(&gpFile, gszLogFilePathName, "a+");
                fprintf(gpFile, "shader->CreateAndCompileShaderObjects() Failed for Vertex Shader...    :: %ls\n", __FUNCTIONW__);
                fclose(gpFile);
                return FALSE;
            }

            // Set this vertex shader in VErtex Shader Stage of Pipeline
            // m_DeviceContext->VSSetShader(gpID3D11VertexShader, NULL, 0);
        

            ID3DBlob *pID3DBlob_PixelShaderCode = NULL;
            if (shader->CreateAndCompilePixelShaderObjects("./src/Shaders/Water/Cube_PS.hlsl", PIXEL_SHADER, &gpID3D11PixelShader, &pID3DBlob_PixelShaderCode) == FALSE)
            {
                fopen_s(&gpFile, gszLogFilePathName, "a+");
                fprintf(gpFile, "shader->CreateAndCompilePixelShaderObjects() Failed for Pixel Shader...    :: %ls\n", __FUNCTIONW__);
                fclose(gpFile);
                return FALSE;
            }
            // Set pixel shader in the Pixel Shader stage of Pipeline
            // m_DeviceContext->PSSetShader(gpID3D11PixelShader, NULL, 0);

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
            d3d11InputElementDescriptor[1].InputSlot = 1;
            d3d11InputElementDescriptor[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            d3d11InputElementDescriptor[1].InstanceDataStepRate = 0;

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


            //create vertex buffer for above position vertices
            // A. initialize Buffer Descriptor... like glGenBuffer()
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
                return(FALSE);
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
                fprintf(gpFile, "gpID3D11Device::CreateBuffer() Failed for Position ...\n");
                fclose(gpFile);
                return(FALSE);
            }

            // Constant Buffer (for uniforms)
            // A. initialize Buffer descriptor
            ZeroMemory((void *)&d3d11BufferDescriptor, sizeof(D3D11_BUFFER_DESC));
            d3d11BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
            d3d11BufferDescriptor.ByteWidth = sizeof(CBUFFER_QUAD);
            d3d11BufferDescriptor.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

            // B. Create Actual Buffer
            hr = m_Device->CreateBuffer(&d3d11BufferDescriptor, NULL, &gpID3D11Buffer_ConstantBuffer);
            if (FAILED(hr))
            {
                fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
                fprintf(gpFile, "gpID3D11Device::CreateBuffer() Failed for Constants ...\n");
                fclose(gpFile);
                return(FALSE);
            }

            // C. set Constant Buffer into the Vertex shader state of pipeline
            // m_DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer);

            // Create Texture Sampler State
            D3D11_SAMPLER_DESC d3d11SamplerDescriptor;
            ZeroMemory((void*)&d3d11SamplerDescriptor, sizeof(D3D11_SAMPLER_DESC));
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
                return(FALSE);
            }
            
            return TRUE;
        }

        void RenderObject(XMFLOAT3 camPos, XMFLOAT3 center, XMFLOAT3 up)
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

            // 2. Render
            // Set primitive topology in Input Assembly Stage
            m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            // transformations
            // A. initialize matrices
            XMMATRIX worldMatrix = XMMatrixRotationX(angle) * XMMatrixRotationY(angle) * XMMatrixRotationZ(angle) * XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixTranslation(0.0f, 0.1f, 0.0f);
            angle += 0.0001f;
            XMMATRIX viewMatrix = XMMatrixLookAtLH(XMVectorSet(camPos.x, camPos.y, camPos.z, 1.0f), 
                                       XMVectorSet(center.x, center.y, center.z, 0.0f) , 
                                       XMVectorSet(up.x, up.y, up.z, 0.0f));

            XMMATRIX wvpMatrix = worldMatrix * viewMatrix * PerspectiveProjectionMatrix;

            // B. Put them into ConstantBuffer
            CBUFFER_QUAD constBuffer;
            ZeroMemory((void *)&constBuffer, sizeof(CBUFFER_QUAD));
            constBuffer.wvpMatrix = (wvpMatrix);

            // C. Push them into the shader (like glUniformMatrix4fv())
            m_DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0, NULL, &constBuffer, 0, 0);

            m_DeviceContext->VSSetShader(gpID3D11VertexShader, NULL, 0);
            m_DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer);
            
            m_DeviceContext->PSSetShader(gpID3D11PixelShader, NULL, 0);
            m_DeviceContext->PSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer);

            // set Shader View in Pixel Shader
            m_DeviceContext->PSSetShaderResources(0, 1, &gpID3D11ShaderResourceView_Smiley);

            // Set Sampler State in Pixel Texture (corresponds to 2nd line in Pixel shader)
            m_DeviceContext->PSSetSamplers(0, 1, &gpID3D11SamplerState_Texture);

            // draw primitive
            m_DeviceContext->Draw(36, 0);

        }

        void Uninitialize(void)
        {
            // if (m_log)
            // {
            //     m_log = NULL;
            // }

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

            if (gpID3D11Buffer_ConstantBuffer)
            {
                gpID3D11Buffer_ConstantBuffer->Release();
                gpID3D11Buffer_ConstantBuffer = NULL;
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
        ID3D11Buffer *gpID3D11Buffer_ConstantBuffer;
        ID3D11ShaderResourceView *gpID3D11ShaderResourceView_Smiley = nullptr;
        ID3D11ShaderResourceView *gpID3D11ShaderResourceView_NormalMap;
        ID3D11SamplerState *gpID3D11SamplerState_Texture;

        DXShaders *shader = NULL;

        float angle = 0.0f;
};

#endif