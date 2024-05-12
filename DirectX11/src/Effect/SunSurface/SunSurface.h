#ifndef SUNSURFACE_H
#define SUNSURFACE_H

// D3D related headers
#include <dxgi.h>
#include <D3D11.h>
#include <d3dcompiler.h>

// #include "../Effect.h"
// #pragma warning(disable : 4838)
// #include "XNAMath_204/xnamath.h"

#include "../../Common/DXShaders.h"
// #include "../../Common/DDSTextureLoader.h"
#include "../../Common/Camera.h"
#include "../../Common/Noise.h"
#include "../../Common/Sphere.h"



extern XMMATRIX PerspectiveProjectionMatrix;

class SunSurface : public Effect
{
    public:
        SunSurface(IDXGISwapChain * SwapChain,
                ID3D11Device *Device,
                ID3D11DeviceContext *DeviceContext,
                ID3D11RenderTargetView *RenderTargetView):
                m_SwapChain(SwapChain),
                m_Device(Device),
                m_DeviceContext(DeviceContext),
                m_RenderTargetView(RenderTargetView) 
        {

        }

        Camera camera;
        ID3D11VertexShader *gpID3D11VertexShader = NULL;
        ID3D11PixelShader *gpID3D11PixelShader = NULL;
        ID3D11InputLayout *gpID3D11InputLayout = NULL;
        ID3D11Buffer *gpID3D11Buffer_ConstantBuffer = NULL;
        ID3D11ShaderResourceView *gpID3D11ShaderResourceView_Texture_PS = NULL;
        ID3D11SamplerState *gpID3D11SamplerState_Texture_PS = NULL;

        // Noise
        Noise *noise = nullptr;

        struct CBUFFER_NOISE
        {
            XMMATRIX WorldMatrix;
            XMMATRIX ViewMatrix;
            XMMATRIX ProjectionMatrix;
            float u_scale;
            XMVECTOR LightPosition;
            int u_lightingEnabled;
        };

        // SunSurface() {}


        ~SunSurface()
        {
            

            if (gpID3D11SamplerState_Texture_PS)
            {
                gpID3D11SamplerState_Texture_PS->Release();
                gpID3D11SamplerState_Texture_PS = NULL;
            }

            if (m_textureSRV)
            {
                m_textureSRV->Release();
                m_textureSRV = NULL;
            }

            if (gpID3D11Buffer_ConstantBuffer)
            {
                gpID3D11Buffer_ConstantBuffer->Release();
                gpID3D11Buffer_ConstantBuffer = NULL;
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

        BOOL Initialize(void)
        {
            HRESULT hr = S_OK;
            // vertex shader
            ID3DBlob *pID3DBlob_VertexShaderCode = NULL;
            shader = new DXShaders;
            if (shader->CreateAndCompileVertexShaderObjects("./src/Shaders/SunSurface/SunSurface_VS.hlsl", VERTEX_SHADER, &gpID3D11VertexShader, &pID3DBlob_VertexShaderCode) == FALSE)
            {
                fopen_s(&gpFile, gszLogFilePathName, "a+");
                fprintf(gpFile, "shader->CreateAndCompileShaderObjects() Failed for Vertex Shader...\n");
                fclose(gpFile);
                return FALSE;
            }
            // Set this vertex shader in VErtex Shader Stage of Pipeline
            m_DeviceContext->VSSetShader(gpID3D11VertexShader, NULL, 0);

            ID3DBlob *pID3DBlob_PixelShaderCode = NULL;
            if (shader->CreateAndCompilePixelShaderObjects("./src/Shaders/SunSurface/SunSurface_PS.hlsl", PIXEL_SHADER, &gpID3D11PixelShader, &pID3DBlob_PixelShaderCode) == FALSE)
            {
                fopen_s(&gpFile, gszLogFilePathName, "a+");
                fprintf(gpFile, "shader->CreateAndCompilePixelShaderObjects() Failed for Pixel Shader...\n");
                fclose(gpFile);
                return FALSE;
            }
            // Set pixel shader in the Pixel Shader stage of Pipeline
            m_DeviceContext->PSSetShader(gpID3D11PixelShader, NULL, 0);

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

            d3d11InputElementDescriptor[1].SemanticName = "NORMAL";
            d3d11InputElementDescriptor[1].SemanticIndex = 0;
            d3d11InputElementDescriptor[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;// out data type in OpenGL eg.. out vec4 FragColor; (Here RGB doesn't mean color, Instead it is "3 floats")
            d3d11InputElementDescriptor[1].AlignedByteOffset = 0; // Is It 2nd last Para of glVertexAttribPointer() ??????
            d3d11InputElementDescriptor[1].InputSlot = 1; // parallel to enum for Attributes in OpenGL
            d3d11InputElementDescriptor[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA; // data is used per vertes
            d3d11InputElementDescriptor[1].InstanceDataStepRate = 0; // dhanga

            // create input layout
            hr = m_Device->CreateInputLayout(d3d11InputElementDescriptor, _ARRAYSIZE(d3d11InputElementDescriptor), pID3DBlob_VertexShaderCode->GetBufferPointer(), pID3DBlob_VertexShaderCode->GetBufferSize(), &gpID3D11InputLayout);
            if (FAILED(hr))
            {
                fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
                fprintf(gpFile, "gpID3D11Device->CreateInputLayout() Failed ...\n");
                fclose(gpFile);
                return FALSE;
            }
            else
            {
                fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
                fprintf(gpFile, "gpID3D11Device->CreateInputLayout() Successfull ...\n");
                fclose(gpFile);
            }

            // Set this Input Layout in Input Assembly Stages of Pipeline
            m_DeviceContext->IASetInputLayout(gpID3D11InputLayout);

            // Release the Blob
            pID3DBlob_VertexShaderCode->Release();
            pID3DBlob_VertexShaderCode = NULL;

            pID3DBlob_PixelShaderCode->Release();
            pID3DBlob_PixelShaderCode = NULL;

            // ############ Geometry ##############
            getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
            numVertices = getNumberOfSphereVertices();
            numElements = getNumberOfSphereElements();
            // create vertex buffer for above position vertices
            //  A. initialize Buffer Descriptor... like glGenBuffer()
            // Position
            D3D11_BUFFER_DESC d3d11BufferDescriptor;
            ZeroMemory((void *)&d3d11BufferDescriptor, sizeof(D3D11_BUFFER_DESC));
            d3d11BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
            d3d11BufferDescriptor.ByteWidth = numVertices * 3 * sizeof(float);
            d3d11BufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;

            // B. initialize subresource data structure to put data into the buffer
            D3D11_SUBRESOURCE_DATA d3d11SubresourceData;
            ZeroMemory((void *)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
            d3d11SubresourceData.pSysMem = sphere_vertices;

            // C. Create the actual buffer
            hr = m_Device->CreateBuffer(&d3d11BufferDescriptor, &d3d11SubresourceData, &gpID3D11Buffer_PositionBuffer);
            if (FAILED(hr))
            {
                fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
                fprintf(gpFile, "gpID3D11Device::CreateBuffer() Failed for Position ...\n");
                fclose(gpFile);
                return FALSE;
            }
            else
            {
                fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
                fprintf(gpFile, "gpID3D11Device::CreateBuffer()) Succedded for Position ...\n");
                fclose(gpFile);
            }

            // Texture
            ZeroMemory((void *)&d3d11BufferDescriptor, sizeof(D3D11_BUFFER_DESC));
            d3d11BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
            d3d11BufferDescriptor.ByteWidth = sizeof(float) * _ARRAYSIZE(sphere_normals);
            d3d11BufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;

            // B. initialize subresource data structure to put data into the buffer
            ZeroMemory((void *)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
            d3d11SubresourceData.pSysMem = sphere_normals;

            // C. Create the actual buffer
            hr = m_Device->CreateBuffer(&d3d11BufferDescriptor,
                                            &d3d11SubresourceData,
                                            &gpID3D11Buffer_NormalBuffer);
            if (FAILED(hr))
            {
                fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
                fprintf(gpFile, "gpID3D11Device::CreateBuffer() Failed for NOrmal ...\n");
                fclose(gpFile);
                return FALSE;
            }
            else
            {
                fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
                fprintf(gpFile, "gpID3D11Device::CreateBuffer()) Succedded for NOrmal ...\n");
                fclose(gpFile);
            }

            // Index Buffer
            ZeroMemory((void *)&d3d11BufferDescriptor, sizeof(D3D11_BUFFER_DESC));
            d3d11BufferDescriptor.Usage = D3D11_USAGE_DEFAULT; // GPU readable/writable (like GL_STATIC_DRAW)
            d3d11BufferDescriptor.ByteWidth = numElements * sizeof(short);// glBufferData() cha  2nd para
            d3d11BufferDescriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
            
            // B. initialize subResource data structure to put data into the buffer (glBindBuffer())
            ZeroMemory((void *)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
            d3d11SubresourceData.pSysMem = sphere_elements; // 3rd para of glBufferData()

            // C. create the actual buffer
            hr = m_Device->CreateBuffer(&d3d11BufferDescriptor,
                                                &d3d11SubresourceData,
                                                &gpID3D11Buffer_IndexBuffer_Sphere);
            if (FAILED(hr))
            {
                fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
                fprintf(gpFile, "gpID3D11Device::CreateBuffer() Failed for Index ...\n");
                fclose(gpFile);
                return(hr);
            }
            else
            {
                fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
                fprintf(gpFile, "gpID3D11Device::CreateBuffer()) Succedded for Index ...\n");
                fclose(gpFile);
	}

            // Constant Buffer (for uniforms)
            // A. initialize Buffer descriptor
            ZeroMemory((void *)&d3d11BufferDescriptor, sizeof(D3D11_BUFFER_DESC));
            d3d11BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
            d3d11BufferDescriptor.ByteWidth = sizeof(CBUFFER_NOISE);
            d3d11BufferDescriptor.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

            // B. Create Actual Buffer
            hr = m_Device->CreateBuffer(&d3d11BufferDescriptor, NULL, &gpID3D11Buffer_ConstantBuffer);
            if (FAILED(hr))
            {
                fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
                fprintf(gpFile, "gpID3D11Device::CreateBuffer() Failed for Constants ...\n");
                fclose(gpFile);
                return FALSE;
            }
            else
            {
                fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
                fprintf(gpFile, "gpID3D11Device::CreateBuffer()) Succedded for Constants ...\n");
                fclose(gpFile);
            }

            // C. set Constant Buffer into the Vertex shader state of pipeline
            m_DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer);
            m_DeviceContext->PSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer);

            // Create Texture Sampler State
            D3D11_SAMPLER_DESC d3d11SamplerDescriptor;
            ZeroMemory((void *)&d3d11SamplerDescriptor, sizeof(D3D11_SAMPLER_DESC));
            d3d11SamplerDescriptor.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            d3d11SamplerDescriptor.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
            d3d11SamplerDescriptor.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
            d3d11SamplerDescriptor.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
            d3d11SamplerDescriptor.ComparisonFunc = D3D11_COMPARISON_NEVER;
            d3d11SamplerDescriptor.MinLOD = 0;
            d3d11SamplerDescriptor.MaxLOD = D3D11_FLOAT32_MAX;

            hr = m_Device->CreateSamplerState(&d3d11SamplerDescriptor, &gpID3D11SamplerState_Texture_PS);
            if (FAILED(hr))
            {
                fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
                fprintf(gpFile, "gpID3D11Device->CreateSamplerState() Failed for Pixel Shader ...\n");
                fclose(gpFile);
                // return (hr);
                return FALSE;
            }
            else
            {
                fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
                fprintf(gpFile, "gpID3D11Device->CreateSamplerState() Successfull for Pixel Shader ...\n");
                fclose(gpFile);
            }

            // load noise texture
            noise = new Noise(*m_Device, *m_DeviceContext, *gpFile);
            noise->CreateNoise3D(&m_textureSRV);
            if (m_textureSRV == NULL)
            {
                fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
                fprintf(gpFile, "noise->CreateNoise3D() Failed for Noise ...\n");
                fclose(gpFile);
                return FALSE;
            }
            else
            {
                fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
                fprintf(gpFile, "noise->CreateNoise3D() Successfull for Noise ...\n");
                fclose(gpFile);
            }

            return TRUE;
        }

        void RenderFrame(XMMATRIX viewMat)
        {
            // code
            // set Position Buffer into InputAssembly stage of pipeline (glVertexAttribPointer() che last 2 para)
            UINT stride = sizeof(float) * 3;
            UINT offset = 0;
            m_DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_PositionBuffer, &stride, &offset);
            // Set color buffer into Input Assembly stage of pipeline
            stride = sizeof(float) * 3;
            offset = 0;
            m_DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_NormalBuffer, &stride, &offset);
            m_DeviceContext->IASetIndexBuffer(gpID3D11Buffer_IndexBuffer_Sphere, DXGI_FORMAT_R16_UINT, // This corresponds to short
											0);
            // 2. Render
            // Set primitive topology in Input Assembly Stage
            m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            // transformations
            // A. initialize matrices
            XMMATRIX worldMatrix = XMMatrixTranslation(0.0f, 0.0f, 4.0f);
            XMMATRIX viewMatrix = XMMatrixIdentity();
            // XMMATRIX scaleMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);

            // B. Put them into ConstantBuffer
            CBUFFER_NOISE cBuffer;
            ZeroMemory((void *)&cBuffer, sizeof(CBUFFER_NOISE));
            cBuffer.WorldMatrix = (worldMatrix);
            cBuffer.ViewMatrix = (viewMatrix);
            cBuffer.ProjectionMatrix = (PerspectiveProjectionMatrix);
            cBuffer.LightPosition = XMVectorSet(LightPosition[0], LightPosition[1], LightPosition[2], 1.0f);
            cBuffer.u_lightingEnabled = 1;
            cBuffer.u_scale = scaleFactor;

            // C. Push them into the shader (like glUniformMatrix4fv())
            m_DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0, NULL, &cBuffer, 0, 0);

            // // set Shader View in Pixel Shader
            m_DeviceContext->PSSetShaderResources(0, 1, &m_textureSRV);

            // // Set Sampler State in Pixel Texture (corresponds to 2nd line in Pixel shader)
            m_DeviceContext->PSSetSamplers(0, 1, &gpID3D11SamplerState_Texture_PS);

            // draw primitive
            m_DeviceContext->DrawIndexed(numElements, 0, 0);
        }

        void Update(void)
        {
            if (isScale == TRUE)
            {
                scaleFactor = scaleFactor - 0.005f;
                if (scaleFactor < 0.0f)
                {
                    isScale = FALSE;
                }
            }
            else
            {
                scaleFactor = scaleFactor + 0.005f;
                if (scaleFactor > 5.0f)
                {
                    isScale = TRUE;
                }
            }
        }

    private:
        IDXGISwapChain * m_SwapChain;
        ID3D11Device *m_Device;
        ID3D11DeviceContext *m_DeviceContext;
        ID3D11RenderTargetView *m_RenderTargetView;
        ID3D11ShaderResourceView *m_textureSRV = NULL;

        ID3D11Buffer *gpID3D11Buffer_PositionBuffer;
        ID3D11Buffer *gpID3D11Buffer_NormalBuffer;
        ID3D11Buffer *gpID3D11Buffer_IndexBuffer_Sphere = NULL;

        DXShaders *shader = NULL;
        float sphere_vertices[1146];
        float sphere_normals[1146];
        float sphere_textures[764];
        unsigned short sphere_elements[2280];

        unsigned int numVertices = 0;
        unsigned int numElements = 0;
        float LightPosition[4] = {0.0f, 0.0f, 4.0f, 1.0f};
        float scaleFactor = 0.5f;
        BOOL isScale = TRUE;
};

#endif
