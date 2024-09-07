#ifndef CUBESHADERCLASS_H
#define CUBESHADERCLASS_H

#include <iostream>
#include <fstream>

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

extern FILE *gpFile;
extern char gszLogFilePathName[];

class CubeShaderClass
{
private:
    struct MatrixBufferType
    {
        DirectX::XMMATRIX world;
        DirectX::XMMATRIX view;
        DirectX::XMMATRIX projection;
    };

    struct LightBufferType
    {
        DirectX::XMVECTOR lightDiffuseColor;
        DirectX::XMFLOAT3 lightDirection;
        float colorTextureBrightness;
    };

public:
    CubeShaderClass();
    CubeShaderClass(const CubeShaderClass &);
    ~CubeShaderClass();

    bool Initialize(ID3D11Device *, HWND);
    void Shutdown();
    bool Render(ID3D11DeviceContext *, int, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX, 
                             DirectX::XMFLOAT3, float);

private:
    bool InitializeShader(ID3D11Device *, HWND, WCHAR *, WCHAR *);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob *, HWND, WCHAR *);

    bool SetShaderParameters(ID3D11DeviceContext *, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX, 
                             DirectX::XMFLOAT3, float);
    void RenderShader(ID3D11DeviceContext *, int);

private:
    ID3D11VertexShader *m_vertexShader;
    ID3D11PixelShader *m_pixelShader;
    ID3D11InputLayout *m_layout;
    ID3D11SamplerState *m_sampleState;
    ID3D11Buffer *m_matrixBuffer;
    ID3D11Buffer *m_lightBuffer;
};

#endif
