#ifndef PARALLEX_H
#define PARALLEX_H

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

struct ConstBufferVS
{
    XMFLOAT3 u_viewPosition;
    XMMATRIX u_projectionMatrix;
    XMMATRIX u_viewMatrix;
    XMMATRIX u_modelMatrix;
    XMFLOAT3 u_lightPosition;
};

struct ConstBufferPS
{
    INT keyPresed;
    FLOAT heightScale;
};

class ParallexMapping
{
public:
    ParallexMapping(IDXGISwapChain *SwapChain,
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
    ~ParallexMapping() {}

    BOOL Initialize()
    {
        HRESULT hr = S_OK;
        // vertex shader
        ID3DBlob *pID3DBlob_VertexShaderCode = NULL;
        if (shader->CreateAndCompileVertexShaderObjects("./src/Shaders/ParallexMapping/vertex.hlsl", VERTEX_SHADER, &m_vertexShader, &pID3DBlob_VertexShaderCode) == FALSE)
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+");
            fprintf(gpFile, "shader->CreateAndCompileShaderObjects() Failed for Vertex Shader...\n");
            fclose(gpFile);
            return FALSE;
        }
        // Set this vertex shader in VErtex Shader Stage of Pipeline
        // m_DeviceContext->VSSetShader(gpID3D11VertexShader, NULL, 0);

        ID3DBlob *pID3DBlob_PixelShaderCode = NULL;
        if (shader->CreateAndCompilePixelShaderObjects("./src/Shaders/ParallexMapping/pixel.hlsl", PIXEL_SHADER, &m_pixelShader, &pID3DBlob_PixelShaderCode) == FALSE)
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+");
            fprintf(gpFile, "shader->CreateAndCompilePixelShaderObjects() Failed for Pixel Shader...\n");
            fclose(gpFile);
            return FALSE;
        }
        // Set pixel shader in the Pixel Shader stage of Pipeline
        // m_DeviceContext->PSSetShader(gpID3D11PixelShader, NULL, 0);

        // Set this Input Layout in Input Assembly Stages of Pipeline
        // ############ Input Layout [just like Attributes in OpenGL] ##############
        D3D11_INPUT_ELEMENT_DESC d3d11InputElementDescriptor[5];
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
        d3d11InputElementDescriptor[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;         // out data type in OpenGL eg.. out vec4 FragColor; (Here RGB doesn't mean color, Instead it is "3 floats")
        d3d11InputElementDescriptor[1].AlignedByteOffset = 0;                        // Is It 2nd last Para of glVertexAttribPointer() ??????
        d3d11InputElementDescriptor[1].InputSlot = 1;                                // parallel to enum for Attributes in OpenGL
        d3d11InputElementDescriptor[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA; // data is used per vertes
        d3d11InputElementDescriptor[1].InstanceDataStepRate = 0;                     // dhanga

        d3d11InputElementDescriptor[2].SemanticName = "TEXCOORD";
        d3d11InputElementDescriptor[2].SemanticIndex = 0;
        d3d11InputElementDescriptor[2].Format = DXGI_FORMAT_R32G32_FLOAT;
        d3d11InputElementDescriptor[2].AlignedByteOffset = 0;
        d3d11InputElementDescriptor[2].InputSlot = 2;
        d3d11InputElementDescriptor[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        d3d11InputElementDescriptor[2].InstanceDataStepRate = 0;

        d3d11InputElementDescriptor[3].SemanticName = "TANGENT";
        d3d11InputElementDescriptor[3].SemanticIndex = 0;
        d3d11InputElementDescriptor[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        d3d11InputElementDescriptor[3].AlignedByteOffset = 0;
        d3d11InputElementDescriptor[3].InputSlot = 3;
        d3d11InputElementDescriptor[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        d3d11InputElementDescriptor[3].InstanceDataStepRate = 0;

        d3d11InputElementDescriptor[4].SemanticName = "BITANGENT";
        d3d11InputElementDescriptor[4].SemanticIndex = 0;
        d3d11InputElementDescriptor[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        d3d11InputElementDescriptor[4].AlignedByteOffset = 0;
        d3d11InputElementDescriptor[4].InputSlot = 4;
        d3d11InputElementDescriptor[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        d3d11InputElementDescriptor[4].InstanceDataStepRate = 0;

        // create input layout
        hr = m_Device->CreateInputLayout(d3d11InputElementDescriptor, _ARRAYSIZE(d3d11InputElementDescriptor), pID3DBlob_VertexShaderCode->GetBufferPointer(), pID3DBlob_VertexShaderCode->GetBufferSize(), &m_layout);
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
        m_DeviceContext->IASetInputLayout(m_layout);

        // Release the Blob
        pID3DBlob_VertexShaderCode->Release();
        pID3DBlob_VertexShaderCode = NULL;

        pID3DBlob_PixelShaderCode->Release();
        pID3DBlob_PixelShaderCode = NULL;

        // ############ Geometry ##############
          // declaration vertex data arrays
        const float position[] =
            {
                1.0f, 1.0f, 0.0f,
                -1.0f, 1.0f, 0.0f,
                -1.0f, -1.0f, 0.0f,
                1.0f, -1.0f, 0.0f
                };

        const float texcoord[] =
            {
                1.0f, 1.0f,
                0.0f, 1.0f,
                0.0f, 0.0f,
                1.0f, 0.0f};

        const float normals[] =
            {
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
            };

        // calculate tangent/bitangent vectors of both triangles
        XMVECTOR tangent1, bitangent1;
        XMVECTOR tangent2, bitangent2;

        // triangle 1
        // ----------
        XMFLOAT3 edge1 = XMFLOAT3(position[3] - position[0], position[4] - position[1], position[5] - position[2]); // - XMFLOAT3(position[0], position[1], position[2]);
        XMFLOAT3 edge2 = XMFLOAT3(position[6] - position[0], position[7] - position[1], position[8] - position[2]); // - XMFLOAT3(position[0], position[1], position[2]);
        XMFLOAT2 deltaUV1 = XMFLOAT2(texcoord[2] - texcoord[0], texcoord[3] - texcoord[1]);                         // - XMFLOAT2(texcoord[0], texcoord[1]);
        XMFLOAT2 deltaUV2 = XMFLOAT2(texcoord[4] - texcoord[0], texcoord[5] - texcoord[1]);                         // - XMFLOAT2(texcoord[0], texcoord[1]);

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent1 = XMVectorSet(f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
                               f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
                               f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z), 0.0f);
        tangent1 = XMVector3Normalize(tangent1);

        bitangent1 = XMVectorSet(f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x),
                                 f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y),
                                 f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z), 0.0f);
        bitangent1 = XMVector3Normalize(bitangent1);

        // triangle 2
        // ----------
        edge1 = XMFLOAT3(position[6] - position[0], position[7] - position[1], position[8] - position[2]);   // - XMFLOAT3(position[0], position[1], position[2]);
        edge2 = XMFLOAT3(position[9] - position[0], position[10] - position[1], position[11] - position[2]); // - XMFLOAT3(position[0], position[1], position[2]);
        deltaUV1 = XMFLOAT2(texcoord[4] - texcoord[0], texcoord[5] - texcoord[1]);                           // - XMFLOAT2(texcoord[0], texcoord[1]);
        deltaUV2 = XMFLOAT2(texcoord[6] - texcoord[0], texcoord[7] - texcoord[1]);                           // - XMFLOAT2(texcoord[0], texcoord[1]);

        f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent2 = XMVectorSet(f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
                               f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
                               f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z), 0.0f);
        tangent2 = XMVector3Normalize(tangent2);

        bitangent2 = XMVectorSet(f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x),
                                 f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y),
                                 f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z), 0.0f);
        bitangent2 = XMVector3Normalize(bitangent2);

        const float quad_position[] = {
            position[0], position[1], position[2],
            position[3], position[4], position[5],
            position[6], position[7], position[8],

            position[0], position[1], position[2],
            position[6], position[7], position[8],
            position[9], position[10], position[11]};
        const float quad_normals[] = {
            normals[0], normals[1], normals[2],
            normals[0], normals[1], normals[2],
            normals[0], normals[1], normals[2],

            normals[0], normals[1], normals[2],
            normals[0], normals[1], normals[2],
            normals[0], normals[1], normals[2]};
        const float quad_texcoords[] = {
            texcoord[0],
            texcoord[1],
            texcoord[2],
            texcoord[3],
            texcoord[4],
            texcoord[5],

            texcoord[0],
            texcoord[1],
            texcoord[4],
            texcoord[5],
            texcoord[6],
            texcoord[7],
        };
        const float quad_tangent[] = {
            XMVectorGetX(tangent1), XMVectorGetY(tangent1), XMVectorGetZ(tangent1),
            XMVectorGetX(tangent1), XMVectorGetY(tangent1), XMVectorGetZ(tangent1),
            XMVectorGetX(tangent1), XMVectorGetY(tangent1), XMVectorGetZ(tangent1),

            XMVectorGetX(tangent2), XMVectorGetY(tangent2), XMVectorGetZ(tangent2),
            XMVectorGetX(tangent2), XMVectorGetY(tangent2), XMVectorGetZ(tangent2),
            XMVectorGetX(tangent2), XMVectorGetY(tangent2), XMVectorGetZ(tangent2)};
        const float quad_bitangent[] = {
            XMVectorGetX(bitangent1), XMVectorGetY(bitangent1), XMVectorGetZ(bitangent1),
            XMVectorGetX(bitangent1), XMVectorGetY(bitangent1), XMVectorGetZ(bitangent1),
            XMVectorGetX(bitangent1), XMVectorGetY(bitangent1), XMVectorGetZ(bitangent1),

            XMVectorGetX(bitangent2), XMVectorGetY(bitangent2), XMVectorGetZ(bitangent2),
            XMVectorGetX(bitangent2), XMVectorGetY(bitangent2), XMVectorGetZ(bitangent2),
            XMVectorGetX(bitangent2), XMVectorGetY(bitangent2), XMVectorGetZ(bitangent2)};

        //  A. initialize Buffer Descriptor... like glGenBuffer()
        // Position
        D3D11_BUFFER_DESC d3d11BufferDescriptor;
        ZeroMemory((void *)&d3d11BufferDescriptor, sizeof(D3D11_BUFFER_DESC));
        d3d11BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
        d3d11BufferDescriptor.ByteWidth = sizeof(float) * _ARRAYSIZE(quad_position);
        d3d11BufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        // B. initialize subresource data structure to put data into the buffer
        D3D11_SUBRESOURCE_DATA d3d11SubresourceData;
        ZeroMemory((void *)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
        d3d11SubresourceData.pSysMem = quad_position;

        // C. Create the actual buffer
        hr = m_Device->CreateBuffer(&d3d11BufferDescriptor, &d3d11SubresourceData, &m_ID3D11Buffer_PositionBuffer);
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

        // Normals
        ZeroMemory((void *)&d3d11BufferDescriptor, sizeof(D3D11_BUFFER_DESC));
        d3d11BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
        d3d11BufferDescriptor.ByteWidth = sizeof(float) * _ARRAYSIZE(quad_normals);
        d3d11BufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        // B. initialize subresource data structure to put data into the buffer
        ZeroMemory((void *)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
        d3d11SubresourceData.pSysMem = quad_normals;

        // C. Create the actual buffer
        hr = m_Device->CreateBuffer(&d3d11BufferDescriptor, &d3d11SubresourceData, &m_ID3D11Buffer_NormalBuffer);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device::CreateBuffer() Failed for Normals ...\n");
            fclose(gpFile);
            return FALSE;
        }
        else
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device::CreateBuffer()) Succedded for Normals ...\n");
            fclose(gpFile);
        }

        // texcoords
        ZeroMemory((void *)&d3d11BufferDescriptor, sizeof(D3D11_BUFFER_DESC));
        d3d11BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
        d3d11BufferDescriptor.ByteWidth = sizeof(float) * _ARRAYSIZE(quad_texcoords);
        d3d11BufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        // B. initialize subresource data structure to put data into the buffer
        ZeroMemory((void *)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
        d3d11SubresourceData.pSysMem = quad_texcoords;

        // C. Create the actual buffer
        hr = m_Device->CreateBuffer(&d3d11BufferDescriptor, &d3d11SubresourceData, &m_ID3D11Buffer_TexcoordsBuffer);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device::CreateBuffer() Failed for Texcoords ...\n");
            fclose(gpFile);
            return FALSE;
        }
        else
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device::CreateBuffer()) Succedded for Texcoords ...\n");
            fclose(gpFile);
        }

        // Tangent
        ZeroMemory((void *)&d3d11BufferDescriptor, sizeof(D3D11_BUFFER_DESC));
        d3d11BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
        d3d11BufferDescriptor.ByteWidth = sizeof(float) * _ARRAYSIZE(quad_tangent);
        d3d11BufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        // B. initialize subresource data structure to put data into the buffer
        ZeroMemory((void *)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
        d3d11SubresourceData.pSysMem = quad_tangent;

        // C. Create the actual buffer
        hr = m_Device->CreateBuffer(&d3d11BufferDescriptor, &d3d11SubresourceData, &m_ID3D11Buffer_TangentBuffer);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device::CreateBuffer() Failed for Tangent ...\n");
            fclose(gpFile);
            return FALSE;
        }
        else
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device::CreateBuffer()) Succedded for Tangent ...\n");
            fclose(gpFile);
        }

        // Bitangent
        ZeroMemory((void *)&d3d11BufferDescriptor, sizeof(D3D11_BUFFER_DESC));
        d3d11BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
        d3d11BufferDescriptor.ByteWidth = sizeof(float) * _ARRAYSIZE(quad_bitangent);
        d3d11BufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        // B. initialize subresource data structure to put data into the buffer
        ZeroMemory((void *)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
        d3d11SubresourceData.pSysMem = quad_bitangent;

        // C. Create the actual buffer
        hr = m_Device->CreateBuffer(&d3d11BufferDescriptor, &d3d11SubresourceData, &m_ID3D11Buffer_BitangentBuffer);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device::CreateBuffer() Failed for Bitangent ...\n");
            fclose(gpFile);
            return FALSE;
        }
        else
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device::CreateBuffer()) Succedded for Bitangent ...\n");
            fclose(gpFile);
        }

        // Constant Buffer (for uniforms)
        // A. initialize Buffer descriptor
        ZeroMemory((void *)&d3d11BufferDescriptor, sizeof(D3D11_BUFFER_DESC));
        d3d11BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
        d3d11BufferDescriptor.ByteWidth = sizeof(ConstBufferVS);
        d3d11BufferDescriptor.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        d3d11BufferDescriptor.CPUAccessFlags = 0;

        // B. Create Actual Buffer
        hr = m_Device->CreateBuffer(&d3d11BufferDescriptor, NULL, &m_ID3D11Buffer_CBVS);
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

        // ZeroMemory((void *)&d3d11BufferDescriptor, sizeof(D3D11_BUFFER_DESC));
        // d3d11BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
        // d3d11BufferDescriptor.ByteWidth = sizeof(ConstBufferPS);
        // d3d11BufferDescriptor.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        // d3d11BufferDescriptor.CPUAccessFlags = 0;

        // // B. Create Actual Buffer
        // hr = m_Device->CreateBuffer(&d3d11BufferDescriptor, NULL, &m_ID3D11Buffer_CBPS);
        // if (FAILED(hr))
        // {
        //     fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
        //     fprintf(gpFile, "gpID3D11Device::CreateBuffer() Failed for Constants ...\n");
        //     fclose(gpFile);
        //     return FALSE;
        // }
        // else
        // {
        //     fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
        //     fprintf(gpFile, "gpID3D11Device::CreateBuffer()) Succedded for Constants ...\n");
        //     fclose(gpFile);
        // }

        // C. set Constant Buffer into the Vertex shader state of pipeline
        m_DeviceContext->VSSetConstantBuffers(0, 1, &m_ID3D11Buffer_CBVS);
        // m_DeviceContext->PSSetConstantBuffers(0, 1, &m_ID3D11Buffer_CBPS);

        // load textures
        hr = CreateWICTextureFromFile(m_Device, m_DeviceContext, L"assets/Textures/parallexmap/renderImg/renderImg_DiffuseMap.png", nullptr, &m_diffuseMap);
        // hr = CreateWICTextureFromFile(m_Device, m_DeviceContext, L"assets/Textures/parallexmap/wall.jpg", nullptr, &m_diffuseMap);
        // hr = CreateWICTextureFromFile(m_Device, m_DeviceContext, L"assets/Textures/parallexmap/bricks2.jpg", nullptr, &m_diffuseMap);
        // hr = CreateWICTextureFromFile(m_Device, m_DeviceContext, L"assets/Textures/parallexmap/ground.jpg", nullptr, &m_diffuseMap);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "Texture couldn't be loaded : DiffuseMap ...\n");
            fclose(gpFile);
        }

        hr = CreateWICTextureFromFile(m_Device, m_DeviceContext, L"assets/Textures/parallexmap/renderImg/renderImg_DiffuseMap.png", nullptr, &m_displacementMap);
        // hr = CreateWICTextureFromFile(m_Device, m_DeviceContext, L"assets/Textures/parallexmap/wall_displace.png", nullptr, &m_displacementMap);
        // hr = CreateWICTextureFromFile(m_Device, m_DeviceContext, L"assets/Textures/parallexmap/bricks2_disp.jpg", nullptr, &m_displacementMap);
        // hr = CreateWICTextureFromFile(m_Device, m_DeviceContext, L"assets/Textures/parallexmap/ground_depth01.jpg", nullptr, &m_displacementMap);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "Texture couldn't be loaded : DisplacementMap ...\n");
            fclose(gpFile);
        }

        hr = CreateWICTextureFromFile(m_Device, m_DeviceContext, L"assets/Textures/parallexmap/renderImg/renderImg.png", nullptr, &m_normalMap);
        // hr = CreateWICTextureFromFile(m_Device, m_DeviceContext, L"assets/Textures/parallexmap/bricks2_normal.jpg", nullptr, &m_normalMap);
        // hr = CreateWICTextureFromFile(m_Device, m_DeviceContext, L"assets/Textures/parallexmap/ground_normal01.jpg", nullptr, &m_normalMap);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "Texture couldn't be loaded : NormalMap ...\n");
            fclose(gpFile);
        }

        // Create Texture Sampler State
        D3D11_SAMPLER_DESC d3d11SamplerDescriptor;
        ZeroMemory((void *)&d3d11SamplerDescriptor, sizeof(D3D11_SAMPLER_DESC));
        d3d11SamplerDescriptor.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        d3d11SamplerDescriptor.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        d3d11SamplerDescriptor.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        d3d11SamplerDescriptor.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

        hr = m_Device->CreateSamplerState(&d3d11SamplerDescriptor, &m_samplerState);
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

    void Render(Camera camera)
    {
        // set Position Buffer into InputAssembly stage of pipeline (glVertexAttribPointer() che last 2 para)
        UINT stride = sizeof(float) * 3;
        UINT offset = 0;
        m_DeviceContext->IASetVertexBuffers(0, 1, &m_ID3D11Buffer_PositionBuffer, &stride, &offset);
        // Set color buffer into Input Assembly stage of pipeline
        stride = sizeof(float) * 3;
        offset = 0;
        m_DeviceContext->IASetVertexBuffers(1, 1, &m_ID3D11Buffer_NormalBuffer, &stride, &offset);

        stride = sizeof(float) * 2;
        offset = 0;
        m_DeviceContext->IASetVertexBuffers(2, 1, &m_ID3D11Buffer_TexcoordsBuffer, &stride, &offset);

        stride = sizeof(float) * 3;
        offset = 0;
        m_DeviceContext->IASetVertexBuffers(3, 1, &m_ID3D11Buffer_TangentBuffer, &stride, &offset);

        stride = sizeof(float) * 3;
        offset = 0;
        m_DeviceContext->IASetVertexBuffers(4, 1, &m_ID3D11Buffer_BitangentBuffer, &stride, &offset);

        // Set primitive topology in Input Assembly Stage
        m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

        // transformations
        XMMATRIX worldMatrix = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
        XMMATRIX viewMatrix = XMMatrixIdentity();
        // viewMatrix = XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, -2.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
        viewMatrix = camera.getViewMatrix();

        // fill constantbuffers
        ConstBufferVS cbVS;
        ZeroMemory((void *)&cbVS, sizeof(ConstBufferVS));
        cbVS.u_viewPosition = XMFLOAT3(0.0f, 0.0f, -2.0f);
        // cbVS.u_viewPosition = XMFLOAT3(XMVectorGetX(camera.getEye()), XMVectorGetY(camera.getEye()), XMVectorGetZ(camera.getEye()));
        cbVS.u_projectionMatrix = PerspectiveProjectionMatrix;
        cbVS.u_viewMatrix = viewMatrix;
        cbVS.u_modelMatrix = worldMatrix;
        // cbVS.u_lightPosition = XMFLOAT3(0.5f, 1.0f, 0.3f);
        cbVS.u_lightPosition = XMFLOAT3(0.0f, 0.0f+ lightPosZ, -5.3f );

        // ConstBufferPS cbPS;
        // ZeroMemory((void *)&cbPS, sizeof(ConstBufferPS));
        // cbPS.heightScale = 0.7f;
        // cbPS.keyPresed = 1;

        // C. Push them into the shader (like glUniformMatrix4fv())
        m_DeviceContext->UpdateSubresource(m_ID3D11Buffer_CBVS, 0, NULL, &cbVS, 0, 0);
        // m_DeviceContext->UpdateSubresource(m_ID3D11Buffer_CBPS, 0, NULL, &cbPS, 0, 0);

        m_DeviceContext->VSSetShader(m_vertexShader, NULL, 0);
        m_DeviceContext->PSSetShader(m_pixelShader, NULL, 0);

        // set Shader View in Pixel Shader
        m_DeviceContext->PSSetShaderResources(0, 1, &m_diffuseMap);
        m_DeviceContext->PSSetShaderResources(1, 1, &m_normalMap);
        m_DeviceContext->PSSetShaderResources(2, 1, &m_displacementMap);

        // Set Sampler State in Pixel Texture (corresponds to 2nd line in Pixel shader)
        m_DeviceContext->PSSetSamplers(0, 1, &m_samplerState);

        m_DeviceContext->Draw(6, 0);
    }

    void Keyboard(WPARAM keyPressed)
    {
        if (keyPressed == 'Z')
        {
            lightPosZ += 0.01f;
        }
        else if (keyPressed == 'z')
        {
            lightPosZ -= 0.01f;
        }
    }

private:
    IDXGISwapChain *m_SwapChain;
    ID3D11Device *m_Device;
    ID3D11DeviceContext *m_DeviceContext;
    ID3D11RenderTargetView *m_RenderTargetView;
    ID3D11DepthStencilView *m_DepthStencilView;

    DXShaders *shader = NULL;

    ID3D11VertexShader *m_vertexShader;
    ID3D11PixelShader *m_pixelShader;
    ID3D11InputLayout *m_layout;
    ID3D11SamplerState *m_samplerState;

    ID3D11ShaderResourceView *m_diffuseMap;
    ID3D11ShaderResourceView *m_displacementMap;
    ID3D11ShaderResourceView *m_normalMap;

    ID3D11Buffer *m_ID3D11Buffer_PositionBuffer;
    ID3D11Buffer *m_ID3D11Buffer_NormalBuffer;
    ID3D11Buffer *m_ID3D11Buffer_TexcoordsBuffer;
    ID3D11Buffer *m_ID3D11Buffer_TangentBuffer;
    ID3D11Buffer *m_ID3D11Buffer_BitangentBuffer;

    ID3D11Buffer *m_ID3D11Buffer_CBVS;
    ID3D11Buffer *m_ID3D11Buffer_CBPS;

    float lightPosZ = 0.0f;
};

#endif