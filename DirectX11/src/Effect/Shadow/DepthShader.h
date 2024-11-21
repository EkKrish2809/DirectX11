#ifndef DEPTHSHADER_H
#define DEPTHSHADER_H

#include "../../Common/Camera.h"
#include "../../Common/DXShaders.h"

#pragma warning(disable : 4838)
#include "../../Math/XNAMath_204/xnamath.h"

class DepthShader
{
public:
    DepthShader(IDXGISwapChain *SwapChain,
              ID3D11Device *Device,
              ID3D11DeviceContext *DeviceContext,
              ID3D11RenderTargetView *RenderTargetView) :
              m_SwapChain(SwapChain),
          m_Device(Device),
          m_DeviceContext(DeviceContext),
          m_RenderTargetView(RenderTargetView){}
    ~DepthShader(){}

    BOOL Initialize()
    {
        shader = new DXShaders();
        HRESULT hr = S_OK;

        // vertex shader
        ID3DBlob *pID3DBlob_VertexShaderCode = NULL;
        if (!shader->CreateAndCompileVertexShaderObjects("./src/Shaders/Shadow/depthVS.hlsl", VERTEX_SHADER, &m_vertexShader, &pID3DBlob_VertexShaderCode))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+");
            fprintf(gpFile, "shader->CreateAndCompileShaderObjects() Failed for Vertex Shader...    :: %ls\n", __FUNCTIONW__);
            fclose(gpFile);
            return FALSE;
        }

        // pixel shader
        ID3DBlob *pID3DBlob_PixelShaderCode = NULL;
        if (!shader->CreateAndCompilePixelShaderObjects("./src/Shaders/Shadow/depthPS.hlsl", PIXEL_SHADER, &m_pixelShader, &pID3DBlob_PixelShaderCode))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+");
            fprintf(gpFile, "shader->CreateAndCompilePixelShaderObjects() Failed for Pixel Shader...    :: %ls\n", __FUNCTIONW__);
            fclose(gpFile);
            return FALSE;
        }

        // ############ Input Layout [just like Attributes in OpenGL] ##############
        D3D11_INPUT_ELEMENT_DESC d3d11InputElementDescriptor[1];
        ZeroMemory((void *)&d3d11InputElementDescriptor, sizeof(D3D11_INPUT_ELEMENT_DESC) * _ARRAYSIZE(d3d11InputElementDescriptor));
        // initialize input layout structure
        d3d11InputElementDescriptor[0].SemanticName = "POSITION";
        d3d11InputElementDescriptor[0].SemanticIndex = 0;
        d3d11InputElementDescriptor[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        d3d11InputElementDescriptor[0].AlignedByteOffset = 0;
        d3d11InputElementDescriptor[0].InputSlot = 0;
        d3d11InputElementDescriptor[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        d3d11InputElementDescriptor[0].InstanceDataStepRate = 0;

        // create input layout
        hr = m_Device->CreateInputLayout(d3d11InputElementDescriptor, _ARRAYSIZE(d3d11InputElementDescriptor), pID3DBlob_VertexShaderCode->GetBufferPointer(),
                                         pID3DBlob_VertexShaderCode->GetBufferSize(), &m_inputLayout);
        if (FAILED(hr))
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "m_Device->CreateInputLayout() Failed ...\n");
            fclose(gpFile);
            return FALSE;
        }
        else
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "m_Device->CreateInputLayout() Successfull ...\n");
            fclose(gpFile);
        }

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
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "m_Device->CreateBuffer() Failed for MatrixBuffer...\n");
            fclose(gpFile);
            return FALSE;
        }
        else
        {
            fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
            fprintf(gpFile, "m_Device->CreateBuffer() Successfull for MatrixBuffer...\n");
            fclose(gpFile);
        }

        return TRUE;
    }

    bool SetShaderParameters(XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, int indexCount)
    {
        HRESULT result;
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        unsigned int bufferNumber;
        MatrixBufferType* dataPtr;

        worldMatrix = XMMatrixTranspose(worldMatrix);
        viewMatrix = XMMatrixTranspose(viewMatrix);
        projectionMatrix = XMMatrixTranspose(projectionMatrix);

        result = m_DeviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        if (FAILED(result))
        {
            return false;
        }

        dataPtr = (MatrixBufferType*)mappedResource.pData;

        dataPtr->world = worldMatrix;
        dataPtr->view = viewMatrix;
        dataPtr->projection = projectionMatrix;

        m_DeviceContext->Unmap(m_matrixBuffer, 0);

        bufferNumber = 0;

        m_DeviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

        m_DeviceContext->IASetInputLayout(m_inputLayout);

        return true;
    }

    void Render(int indexCount)
    {
        m_DeviceContext->VSSetShader(m_vertexShader, NULL, 0);
        m_DeviceContext->PSSetShader(m_pixelShader, NULL, 0);

        // m_DeviceContext->DrawIndexed(indexCount, 0, 0);
        m_DeviceContext->Draw(indexCount, 0);
    }

    void Uninitialize()
    {
        if (m_matrixBuffer)
        {
            m_matrixBuffer->Release();
            m_matrixBuffer = nullptr;
        }

        if (m_inputLayout)
        {
            m_inputLayout->Release();
            m_inputLayout = nullptr;
        }

        if (m_pixelShader)
        {
            m_pixelShader->Release();
            m_pixelShader = nullptr;
        }

        if (m_vertexShader)
        {
            m_vertexShader->Release();
            m_vertexShader = nullptr;
        }
    }

private:
    IDXGISwapChain *m_SwapChain;
    ID3D11Device *m_Device;
    ID3D11DeviceContext *m_DeviceContext;
    ID3D11RenderTargetView *m_RenderTargetView;

    DXShaders *shader = NULL;

    ID3D11VertexShader *m_vertexShader;
    ID3D11PixelShader *m_pixelShader;
    ID3D11InputLayout *m_inputLayout;
    ID3D11Buffer* m_matrixBuffer;

private:
    struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};
};

#endif