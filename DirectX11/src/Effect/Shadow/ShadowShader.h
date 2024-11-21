#ifndef SHADOWSHADER_H
#define SHADOWSHADER_H

#include "../../Common/Framebuffer.h"
#include "../../Common/Camera.h"
#include "../../Common/DXShaders.h"

class ShadowShader
{
public:
    ShadowShader(IDXGISwapChain *SwapChain,
              ID3D11Device *Device,
              ID3D11DeviceContext *DeviceContext,
              ID3D11RenderTargetView *RenderTargetView) :
              m_SwapChain(SwapChain),
          m_Device(Device),
          m_DeviceContext(DeviceContext),
          m_RenderTargetView(RenderTargetView) {}


    ~ShadowShader() {
        Uninitialize();
    }

    BOOL Initialize()
    {
        shader = new DXShaders();
        HRESULT hr = S_OK;

        // vertex shader
        ID3DBlob *pID3DBlob_VertexShaderCode = NULL;
        if (!shader->CreateAndCompileVertexShaderObjects("./src/Shaders/Shadow/shadowVS.hlsl", VERTEX_SHADER, &vertexShader, &pID3DBlob_VertexShaderCode))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+");
            fprintf(gpFile, "shader->CreateAndCompileShaderObjects() Failed for Vertex Shader...    :: %ls\n", __FUNCTIONW__);
            fclose(gpFile);
            return FALSE;
        }

        // pixel shader
        ID3DBlob *pID3DBlob_PixelShaderCode = NULL;
        if (!shader->CreateAndCompilePixelShaderObjects("./src/Shaders/Shadow/shadowPS.hlsl", PIXEL_SHADER, &pixelShader, &pID3DBlob_PixelShaderCode))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+");
            fprintf(gpFile, "shader->CreateAndCompilePixelShaderObjects() Failed for Pixel Shader...    :: %ls\n", __FUNCTIONW__);
            fclose(gpFile);
            return FALSE;
        }

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
                                         pID3DBlob_VertexShaderCode->GetBufferSize(), &InputLayout);
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

        D3D11_BUFFER_DESC matrixBufferDesc;
        matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
        matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        matrixBufferDesc.MiscFlags = 0;
        matrixBufferDesc.StructureByteStride = 0;

        hr = m_Device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
        if (FAILED(hr))
        {
            return FALSE;
        }

        D3D11_SAMPLER_DESC samplerDesc;
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.MipLODBias = 0.f;
        samplerDesc.MaxAnisotropy = 1;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        samplerDesc.BorderColor[0] = 0;
        samplerDesc.BorderColor[1] = 0;
        samplerDesc.BorderColor[2] = 0;
        samplerDesc.BorderColor[3] = 0;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        hr = m_Device->CreateSamplerState(&samplerDesc, &m_sampleStateClamp);
        if (FAILED(hr))
        {
            return FALSE;
        }

        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.MipLODBias = 0.f;
        samplerDesc.MaxAnisotropy = 1;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        samplerDesc.BorderColor[0] = 0;
        samplerDesc.BorderColor[1] = 0;
        samplerDesc.BorderColor[2] = 0;
        samplerDesc.BorderColor[3] = 0;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
        
        hr = m_Device->CreateSamplerState(&samplerDesc, &m_sampleStateWrap);
        if (FAILED(hr))
        {
            return FALSE;
        }

        D3D11_BUFFER_DESC lightBufferDesc;
        lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        lightBufferDesc.ByteWidth = sizeof(LightBufferType);
        lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        lightBufferDesc.MiscFlags = 0;
        lightBufferDesc.StructureByteStride = 0;

        hr = m_Device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
        if (FAILED(hr))
        {
            return FALSE;
        }

        D3D11_BUFFER_DESC lightPositionBufferDesc;
        lightPositionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        lightPositionBufferDesc.ByteWidth = sizeof(LightPositionBufferType);
        lightPositionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        lightPositionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        lightPositionBufferDesc.MiscFlags = 0;
        lightPositionBufferDesc.StructureByteStride = 0;

        hr = m_Device->CreateBuffer(&lightPositionBufferDesc, NULL, &m_lightPositionBuffer);
        if (FAILED(hr))
        {
            return FALSE;
        }

        return TRUE;
    }

    bool SetShaderParameters(XMMATRIX worldMatrix,
									   XMMATRIX viewMatrix, 
									   XMMATRIX projectionMatrix,
									   XMMATRIX lightViewMatrix,
									   XMMATRIX lightProjectionMatrix,
									   ID3D11ShaderResourceView* texture,
									   ID3D11ShaderResourceView* depthMapTexture,
									   XMFLOAT4 ambientColor, 
									   XMFLOAT4 diffuseColor,
									   XMFLOAT3 lightPosition,	
									   float bias,
									   XMMATRIX lightViewMatrix2, 
									   XMMATRIX lightProjectionMatrix2, 
									   ID3D11ShaderResourceView* depthMapTexture2, 
									   XMFLOAT3 lightPosition2, 
									   XMFLOAT4 diffuseColor2,
									   XMMATRIX lightViewMatrix3, 
									   XMMATRIX lightProjectionMatrix3, 
									   ID3D11ShaderResourceView* depthMapTexture3, 
									   XMFLOAT3 lightPosition3, 
									   XMFLOAT4 diffuseColor3,
                                       int indexCount)
    {
        HRESULT result;
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        MatrixBufferType* dataPtr;
        unsigned int bufferNumber;
        LightPositionBufferType* dataPtr2;
        LightBufferType* dataPtr3;

        worldMatrix = XMMatrixTranspose(worldMatrix);
        viewMatrix = XMMatrixTranspose(viewMatrix);
        projectionMatrix = XMMatrixTranspose(projectionMatrix);
        lightViewMatrix = XMMatrixTranspose(lightViewMatrix);
        lightProjectionMatrix = XMMatrixTranspose(lightProjectionMatrix);
        lightViewMatrix2 = XMMatrixTranspose(lightViewMatrix2);
        lightProjectionMatrix2 = XMMatrixTranspose(lightProjectionMatrix2);
        lightViewMatrix3 = XMMatrixTranspose(lightViewMatrix3);
        lightProjectionMatrix3 = XMMatrixTranspose(lightProjectionMatrix3);

        result = m_DeviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        if (FAILED(result))
        {
            return false;
        }

        dataPtr = (MatrixBufferType*)mappedResource.pData;
        dataPtr->world = worldMatrix;
        dataPtr->view = viewMatrix;
        dataPtr->projection = projectionMatrix;
        dataPtr->lightView = lightViewMatrix;
        dataPtr->lightProjection = lightProjectionMatrix;
        dataPtr->lightView2 = lightViewMatrix2;
        dataPtr->lightProjection2 = lightProjectionMatrix2;
        dataPtr->lightView3 = lightViewMatrix3;
        dataPtr->lightProjection3 = lightProjectionMatrix3;
        m_DeviceContext->Unmap(m_matrixBuffer, 0);

        bufferNumber = 0;

        m_DeviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

        result = m_DeviceContext->Map(m_lightPositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        if (FAILED(result))
        {
            return false;
        }
        dataPtr2 = (LightPositionBufferType*)mappedResource.pData;
        dataPtr2->lightPosition = lightPosition;
        dataPtr2->padding = 0.f;
        dataPtr2->lightPosition2 = lightPosition2;
        dataPtr2->padding2 = 0.f;
        dataPtr2->lightPosition3 = lightPosition3;
        dataPtr2->padding3 = 0.f;
        m_DeviceContext->Unmap(m_lightPositionBuffer, 0);

        bufferNumber = 1;

        m_DeviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_lightPositionBuffer);

        result = m_DeviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        if (FAILED(result))
        {
            return false;
        }
        dataPtr3 = (LightBufferType*)mappedResource.pData;
        dataPtr3->ambientColor = ambientColor;
        dataPtr3->diffuseColor = diffuseColor;
        dataPtr3->diffuseColor2 = diffuseColor2;
        dataPtr3->diffuseColor3 = diffuseColor3;
        dataPtr3->bias = bias;
        dataPtr3->padding = XMFLOAT3(0.f, 0.f, 0.f);
        m_DeviceContext->Unmap(m_lightBuffer, 0);

        bufferNumber = 0;

        m_DeviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

        m_DeviceContext->PSSetShaderResources(0, 1, &texture);
        m_DeviceContext->PSSetShaderResources(1, 1, &depthMapTexture);
        m_DeviceContext->PSSetShaderResources(2, 1, &depthMapTexture2);
        m_DeviceContext->PSSetShaderResources(3, 1, &depthMapTexture3);

        m_DeviceContext->IASetInputLayout(InputLayout);

        // m_DeviceContext->VSSetShader(vertexShader, NULL, 0);
        // m_DeviceContext->PSSetShader(pixelShader, NULL, 0);

        m_DeviceContext->PSSetSamplers(0, 1, &m_sampleStateClamp);
        m_DeviceContext->PSSetSamplers(1, 1, &m_sampleStateWrap);

        // m_DeviceContext->DrawIndexed(indexCount, 0, 0);

        return true;
    }

    void Render(int indexCount)
    {
        m_DeviceContext->VSSetShader(vertexShader, NULL, 0);
        m_DeviceContext->PSSetShader(pixelShader, NULL, 0);

        // m_DeviceContext->DrawIndexed(indexCount, 0, 0);
        m_DeviceContext->Draw(indexCount, 0);
    }

    void Uninitialize()
    {
        if (m_lightBuffer)
        {
            m_lightBuffer->Release();
            m_lightBuffer = nullptr;
        }

        if (m_lightPositionBuffer)
        {
            m_lightPositionBuffer->Release();
            m_lightPositionBuffer = nullptr;
        }

        if (m_sampleStateWrap)
        {
            m_sampleStateWrap->Release();
            m_sampleStateWrap = nullptr;
        }

        if (m_sampleStateClamp)
        {
            m_sampleStateClamp->Release();
            m_sampleStateClamp = nullptr;
        }

        if (m_matrixBuffer) 
        {
            m_matrixBuffer->Release();
            m_matrixBuffer = nullptr;
        }

        if (InputLayout)
        {
            InputLayout->Release();
            InputLayout = nullptr;
        }

        if (pixelShader)
        {
            pixelShader->Release();
            pixelShader = nullptr;
        }

        if (vertexShader)
        {
            vertexShader->Release();
            vertexShader = nullptr;
        }
    }

private:
    IDXGISwapChain *m_SwapChain;
    ID3D11Device *m_Device;
    ID3D11DeviceContext *m_DeviceContext;
    ID3D11RenderTargetView *m_RenderTargetView;

    DXShaders *shader = NULL;

    ID3D11VertexShader *vertexShader;
    ID3D11PixelShader *pixelShader;
    ID3D11InputLayout *InputLayout;
    ID3D11Buffer* m_matrixBuffer;
    ID3D11SamplerState* m_sampleStateClamp;
	ID3D11SamplerState* m_sampleStateWrap;
	ID3D11Buffer* m_lightPositionBuffer;
	ID3D11Buffer* m_lightBuffer;

private:
    struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView;
		XMMATRIX lightProjection;
		XMMATRIX lightView2;
		XMMATRIX lightProjection2;
		XMMATRIX lightView3;
		XMMATRIX lightProjection3;
	};

	struct LightPositionBufferType
	{
		XMFLOAT3 lightPosition;
		float padding;
		XMFLOAT3 lightPosition2;
		float padding2;
		XMFLOAT3 lightPosition3;
		float padding3;
	};

	struct LightBufferType
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 diffuseColor;
		XMFLOAT4 diffuseColor2;
		XMFLOAT4 diffuseColor3;
		float bias;
		XMFLOAT3 padding;
	};
};

#endif