#pragma once

// D3D related headers
#include <dxgi.h>
#include <D3D11.h>
#include <d3dcompiler.h>

#pragma warning(disable : 4838)
#include "XNAMath_204/xnamath.h"

#include "../../DXShaders.h"
#include "../../Common/DDSTextureLoader.h"
#include "../../Common/Camera.h"


extern XMMATRIX PerspectiveProjectionMatrix;

// D3D related global variables
extern IDXGISwapChain *pIDXGISwapChain;
extern ID3D11Device *pID3D11Device;
extern ID3D11DeviceContext *pID3D11DeviceContext ;
extern ID3D11RenderTargetView *pID3D11RenderTargetView ;
extern float clearColor[4];

extern ID3D11RasterizerState *gpID3D11RasterizerState;
extern ID3D11DepthStencilView *gpID3D11DepthStencilView; 

extern FILE *gpFile;
extern char gszLogFilePathName[];
 
class Grass
{

public:

    struct CBUFFER
    {
        XMMATRIX WorldMatrix;
        XMMATRIX ViewMatrix;
        XMMATRIX ProjectionMatrix;

        XMVECTOR CameraPosition;
        float ElapsedTime;
        float WindStrength;
    };
    // CBUFFER constBuffer;
    Camera camera;
    ID3D11VertexShader *gpID3D11VertexShader = NULL;
    ID3D11GeometryShader *gpID3D11GeometryShader = NULL;
    ID3D11PixelShader *gpID3D11PixelShader = NULL;
    ID3D11InputLayout *gpID3D11InputLayout = NULL;
    ID3D11Buffer *gpID3D11Buffer_PositionBuffer = NULL;
    ID3D11Buffer *gpID3D11Buffer_TextureBuffer = NULL;
    ID3D11Buffer *gpID3D11Buffer_ConstantBuffer = NULL;
    ID3D11ShaderResourceView *gpID3D11ShaderResourceView_Texture_PS = NULL;
    ID3D11ShaderResourceView *gpID3D11ShaderResourceView_Texture_GS = NULL;
    ID3D11SamplerState *gpID3D11SamplerState_Texture_PS = NULL;
    ID3D11SamplerState *gpID3D11SamplerState_Texture_GS = NULL;

    Grass() {}
    ~Grass()
    {
        if (gpID3D11SamplerState_Texture_GS)
        {
            gpID3D11SamplerState_Texture_GS->Release();
            gpID3D11SamplerState_Texture_GS = NULL;
        }

        if (gpID3D11ShaderResourceView_Texture_GS)
        {
            gpID3D11ShaderResourceView_Texture_GS->Release();
            gpID3D11ShaderResourceView_Texture_GS = NULL;
        }

        if (gpID3D11SamplerState_Texture_PS)
        {
            gpID3D11SamplerState_Texture_PS->Release();
            gpID3D11SamplerState_Texture_PS = NULL;
        }

        if (gpID3D11ShaderResourceView_Texture_PS)
        {
            gpID3D11ShaderResourceView_Texture_PS->Release();
            gpID3D11ShaderResourceView_Texture_PS = NULL;
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

    BOOL Initialize()
    {
        HRESULT hr = S_OK;
        // vertex shader
        ID3DBlob *pID3DBlob_VertexShaderCode = NULL;
        shader = new DXShaders;
        if (shader->CreateAndCompileVertexShaderObjects("./Shaders/Grass/grassVert.hlsl", VERTEX_SHADER, &gpID3D11VertexShader, &pID3DBlob_VertexShaderCode) == FALSE)
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+");
            fprintf(gpFile, "shader->CreateAndCompileShaderObjects() Failed for Vertex Shader...\n");
            fclose(gpFile);
            return FALSE;
        }
        // Set this vertex shader in VErtex Shader Stage of Pipeline
        pID3D11DeviceContext->VSSetShader(gpID3D11VertexShader,
                                          NULL,
                                          0);

        // Geometry Shader
        ID3DBlob *pID3DBlob_GeometryShaderCode = NULL; // Blob is similar to 'void*' (for habdeling Big Sized data)
                                                       // ID3DBlob *pID3DBlob_Error = NULL;
        if (shader->CreateAndCompileGeometryShaderObjects("./Shaders/Grass/grassGeom.hlsl", GEOMETRY_SHADER, &gpID3D11GeometryShader, &pID3DBlob_GeometryShaderCode) == FALSE)
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+");
            fprintf(gpFile, "shader->CreateAndCompileGeometryShaderObjects() Failed for Geometry Shader...\n");
            fclose(gpFile);
            return FALSE;
        }
        //  Set this Geometry shader in Geometry Shader Stage of Pipeline
        pID3D11DeviceContext->GSSetShader(gpID3D11GeometryShader,
                                          NULL, // as we haven't used clss linkage if shared shader variables are used
                                          0);

        ID3DBlob *pID3DBlob_PixelShaderCode = NULL;
        if (shader->CreateAndCompilePixelShaderObjects("./Shaders/Grass/grassPixel.hlsl", PIXEL_SHADER, &gpID3D11PixelShader, &pID3DBlob_PixelShaderCode) == FALSE)
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+");
            fprintf(gpFile, "shader->CreateAndCompilePixelShaderObjects() Failed for Pixel Shader...\n");
            fclose(gpFile);
            return FALSE;
        }
        // Set pixel shader in the Pixel Shader stage of Pipeline
        pID3D11DeviceContext->PSSetShader(gpID3D11PixelShader,
                                          NULL,
                                          0);

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
        hr = pID3D11Device->CreateInputLayout(d3d11InputElementDescriptor,
                                              _ARRAYSIZE(d3d11InputElementDescriptor),
                                              pID3DBlob_VertexShaderCode->GetBufferPointer(),
                                              pID3DBlob_VertexShaderCode->GetBufferSize(),
                                              &gpID3D11InputLayout);
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
            // return TRUE;
        }

        // Set this Input Layout in Input Assembly Stages of Pipeline
        pID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayout);

        // Release the Blob
        pID3DBlob_VertexShaderCode->Release();
        pID3DBlob_VertexShaderCode = NULL;

        pID3DBlob_PixelShaderCode->Release();
        pID3DBlob_PixelShaderCode = NULL;

        // ############ Geometry ##############
        const float squareVertices[] =
            {
                -1.0f,
                +1.0f,
                -1.0f,
                +1.0f,
                +1.0f,
                -1.0f,
                -1.0f,
                -1.0f,
                -1.0f,

                -1.0f,
                -1.0f,
                -1.0f,
                +1.0f,
                +1.0f,
                -1.0f,
                +1.0f,
                -1.0f,
                -1.0f,
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
        d3d11BufferDescriptor.ByteWidth = sizeof(float) * _ARRAYSIZE(grassPosition);
        d3d11BufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        // B. initialize subresource data structure to put data into the buffer
        D3D11_SUBRESOURCE_DATA d3d11SubresourceData;
        ZeroMemory((void *)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
        d3d11SubresourceData.pSysMem = grassPosition;

        // C. Create the actual buffer
        hr = pID3D11Device->CreateBuffer(&d3d11BufferDescriptor,
                                         &d3d11SubresourceData,
                                         &gpID3D11Buffer_PositionBuffer);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device::CreateBuffer() Failed for Position ...\n");
            fclose(gpFile);
            // return (hr);
            return FALSE;
        }
        else
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device::CreateBuffer()) Succedded for Position ...\n");
            fclose(gpFile);
            // return TRUE;
        }

        // Texture
            // A. initialize Buffer Descriptor... like glGenBuffer()
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
                                            &gpID3D11Buffer_TextureBuffer);
            if (FAILED(hr))
            {
                fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
                fprintf(gpFile, "gpID3D11Device::CreateBuffer() Failed for Position ...\n");
                fclose(gpFile);
                return(hr);
            }
            else
            {
                fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
                fprintf(gpFile, "gpID3D11Device::CreateBuffer()) Succedded for Position ...\n");
                fclose(gpFile);
            }
        

        // Constant Buffer (for uniforms)
        // A. initialize Buffer descriptor
        ZeroMemory((void *)&d3d11BufferDescriptor, sizeof(D3D11_BUFFER_DESC));
        d3d11BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
        d3d11BufferDescriptor.ByteWidth = sizeof(CBUFFER);
        d3d11BufferDescriptor.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

        // B. Create Actual Buffer
        hr = pID3D11Device->CreateBuffer(&d3d11BufferDescriptor,
                                         NULL,
                                         &gpID3D11Buffer_ConstantBuffer);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device::CreateBuffer() Failed for Constants ...\n");
            fclose(gpFile);
            // return (hr);
            return FALSE;
        }
        else
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device::CreateBuffer()) Succedded for Constants ...\n");
            fclose(gpFile);
            // return TRUE;
        }

        // C. set Constant Buffer into the Vertex shader state of pipeline
        pID3D11DeviceContext->VSSetConstantBuffers(0,
                                                   1,
                                                   &gpID3D11Buffer_ConstantBuffer);

        // Geometry shader Const Buffer
        pID3D11DeviceContext->GSSetConstantBuffers(0, // index of buffer
                                                   1, // how many buffers
                                                   &gpID3D11Buffer_ConstantBuffer);

        // Load Grass texture
        hr = LoadD3D_DDSTexture(MAKEINTRESOURCE(GRASS1_DDS_TEXTURE), pID3D11Device, &gpID3D11ShaderResourceView_Texture_PS);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "LoadD3D_DDSTexture() User Defined function Failed for Grass 1 ...\n");
            fclose(gpFile);
            // return (hr);
            return FALSE;
        }
        else
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "LoadD3D_DDSTexture() User Defined function Successfull for Grass 1 ...\n");
            fclose(gpFile);
            // return TRUE;
        }

        hr = LoadD3D_DDSTexture(MAKEINTRESOURCE(FLOWMAP_DDS_TEXTURE), pID3D11Device, &gpID3D11ShaderResourceView_Texture_GS);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "LoadD3D_DDSTexture() User Defined function Failed for Flowmap ...\n");
            fclose(gpFile);
            // return (hr);
            return FALSE;
        }
        else
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "LoadD3D_DDSTexture() User Defined function Successfull for Grass 1 ...\n");
            fclose(gpFile);
            // return TRUE;
        }

        // Create Texture Sampler State
        D3D11_SAMPLER_DESC d3d11SamplerDescriptor;
        ZeroMemory((void *)&d3d11SamplerDescriptor, sizeof(D3D11_SAMPLER_DESC));
        d3d11SamplerDescriptor.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        d3d11SamplerDescriptor.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        d3d11SamplerDescriptor.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        d3d11SamplerDescriptor.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

        hr = pID3D11Device->CreateSamplerState(&d3d11SamplerDescriptor, &gpID3D11SamplerState_Texture_PS);
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

        hr = pID3D11Device->CreateSamplerState(&d3d11SamplerDescriptor, &gpID3D11SamplerState_Texture_GS);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device->CreateSamplerState() Failed for Geometry shader...\n");
            fclose(gpFile);
            // return (hr);
            return FALSE;
        }
        else
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "gpID3D11Device->CreateSamplerState() Successfull for Geometry shader ...\n");
            fclose(gpFile);
        }

        return TRUE;
    }

    void RenderFrame(float time)
    {
        // code
        
        // set Position Buffer into InputAssembly stage of pipeline (glVertexAttribPointer() che last 2 para)
        UINT stride = sizeof(float) * 3;
        UINT offset = 0;
        pID3D11DeviceContext->IASetVertexBuffers(0,
                                                 1,
                                                 &gpID3D11Buffer_PositionBuffer,
                                                 &stride,
                                                 &offset);
        // Set color buffer into Input Assembly stage of pipeline
        stride = sizeof(float) * 2;
        offset = 0;
        pID3D11DeviceContext->IASetVertexBuffers(1,
                                                1,
                                                &gpID3D11Buffer_TextureBuffer,
                                                &stride,
                                                &offset);

        
        // 2. Render
        // Set primitive topology in Input Assembly Stage
        pID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

        // transformations
        
        // A. initialize matrices
        XMMATRIX worldMatrix = XMMatrixTranslation(0.0f, 0.0f, 5.0f);
        XMMATRIX viewMatrix = XMMatrixIdentity();
        XMMATRIX scaleMatrix = XMMatrixScaling(10.0f, 10.0f, 10.0f);
        
        // XMMATRIX wvpMatrix = worldMatrix * viewMatrix * PerspectiveProjectionMatrix;
        
        // B. Put them into ConstantBuffer
        CBUFFER constBuffer;
        ZeroMemory((void *)&constBuffer, sizeof(CBUFFER));
        constBuffer.WorldMatrix = scaleMatrix * worldMatrix;
        // constBuffer.ViewMatrix = camera.getViewMatrix();
        constBuffer.ViewMatrix = viewMatrix;
        constBuffer.ProjectionMatrix = PerspectiveProjectionMatrix;

        // constant buffer for grass
        constBuffer.CameraPosition = camera.getEye();
        // constBuffer.CameraPosition = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        constBuffer.ElapsedTime = time;
        constBuffer.WindStrength = 0.04f;

        
        // C. Push them into the shader (like glUniformMatrix4fv())
        pID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,
                                                0,
                                                NULL,
                                                &constBuffer,
                                                0,
                                                0);

        
        pID3D11DeviceContext->GSSetShaderResources(0, 1, &gpID3D11ShaderResourceView_Texture_GS);
        pID3D11DeviceContext->GSSetSamplers(0, 1, &gpID3D11SamplerState_Texture_GS);

        // set Shader View in Pixel Shader
        pID3D11DeviceContext->PSSetShaderResources(0, 1, &gpID3D11ShaderResourceView_Texture_PS);

        // Set Sampler State in Pixel Texture (corresponds to 2nd line in Pixel shader)
        pID3D11DeviceContext->PSSetSamplers(0, 1, &gpID3D11SamplerState_Texture_PS);

        // draw primitive
        pID3D11DeviceContext->Draw(6, 0);
    }

    // Texture Loading Function
    HRESULT LoadD3D_DDSTexture(TCHAR ddsTextureResourceID[], ID3D11Device *pID3D11Device, ID3D11ShaderResourceView **ppID3D11ShaderResourceView)
    {
        // variable declarations
        HRESULT hr = S_OK;

        HRSRC hResource = NULL;
        HGLOBAL hGlobal = NULL;
        void *pTextureResourceImageData = NULL;
        DWORD dwTextureResourceSize = 0;

        // code
        // FindResource() -> Get handle to the Resource from Executable file
        hResource = FindResource(GetModuleHandle(NULL), ddsTextureResourceID, RT_RCDATA);
        if (hResource == NULL)
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+");
            fprintf_s(gpFile, "LoadD3D_DDSTexture() : FindResource() Failed for Texture Resource.\n");
            fclose(gpFile);
            return (E_FAIL);
        }

        // LoadResource() -> Use the above obtained handle to load the Resource
        hGlobal = LoadResource(GetModuleHandle(NULL), hResource);
        if (hGlobal == NULL)
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+");
            fprintf_s(gpFile, "LoadD3D_DDSTexture() : LoadResource() Failed for Texture Resource.\n");
            fclose(gpFile);
            return (E_FAIL);
        }

        // LockResource() -> Get starting byte offset of resource loaded in the memory
        pTextureResourceImageData = LockResource(hGlobal);
        if (pTextureResourceImageData == NULL)
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+");
            fprintf_s(gpFile, "LoadD3D_DDSTexture() : LockResource() Failed for Texture Resource.\n");
            fclose(gpFile);
            return (E_FAIL);
        }

        // SizeofResource() -> Return size of loaded resource in bytes
        dwTextureResourceSize = SizeofResource(GetModuleHandle(NULL), hResource);
        if (dwTextureResourceSize == NULL)
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+");
            fprintf_s(gpFile, "LoadD3D_DDSTexture() : SizeofResource() Failed for Texture Resource.\n");
            fclose(gpFile);
            return (E_FAIL);
        }

        hr = DirectX::CreateDDSTextureFromMemory(pID3D11Device,                              // device
                                                 (const uint8_t *)pTextureResourceImageData, // image data / pixels
                                                 (size_t)dwTextureResourceSize,              // size of image data
                                                 NULL,                                       // ID3DResource ** -> we do not need it
                                                 ppID3D11ShaderResourceView);                // Shader resource view
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "LoadD3D_DDSTexture() : CreateDDSTextureFromMemory()  Failed...\n");
            fclose(gpFile);
            return (hr);
        }
        else
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "LoadD3D_DDSTexture() : CreateDDSTextureFromMemory() Succeded...\n");
            fclose(gpFile);
        }

        return (hr);
    }

private:
    

    float grassPosition[3] = {1.0f, 1.0f, 1.0f};
    DXShaders *shader = NULL;
};