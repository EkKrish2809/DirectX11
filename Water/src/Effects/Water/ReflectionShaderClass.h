#ifndef _REFLECTIONSHADERCLASS_H_
#define _REFLECTIONSHADERCLASS_H_

#include <D3D11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

#include <fstream>

#include "../../Common/DXShaders.h"

extern FILE *gpFile;
extern char gszLogFilePathName[];

class ReflectionShaderClass
{
private:
    struct MatrixBufferType
    {
        DirectX::XMMATRIX world;
        DirectX::XMMATRIX view;
        DirectX::XMMATRIX projection;
    };

    struct ClipPlaneBufferType
    {
        DirectX::XMVECTOR clipPlane;
    };

    struct LightBufferType
    {
        DirectX::XMVECTOR lightDiffuseColor;
        DirectX::XMFLOAT3 lightDirection;
        float colorTextureBrightness;
    };

public:
    ReflectionShaderClass();
    ReflectionShaderClass(const ReflectionShaderClass &);
    ~ReflectionShaderClass();

    bool Initialize(ID3D11Device *, HWND);
    void Shutdown();
    bool Render(ID3D11DeviceContext *, int, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX, ID3D11ShaderResourceView *, ID3D11ShaderResourceView *, DirectX::XMVECTOR,
                DirectX::XMFLOAT3, float, DirectX::XMVECTOR);

private:
    bool InitializeShader(ID3D11Device *, HWND, WCHAR *, WCHAR *);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob *, HWND, WCHAR *);

    bool SetShaderParameters(ID3D11DeviceContext *, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX, ID3D11ShaderResourceView *, ID3D11ShaderResourceView *, DirectX::XMVECTOR,
                             DirectX::XMFLOAT3, float, DirectX::XMVECTOR);
    void RenderShader(ID3D11DeviceContext *, int);

private:
    ID3D11VertexShader *m_vertexShader;
    ID3D11PixelShader *m_pixelShader;
    ID3D11InputLayout *m_layout;
    ID3D11SamplerState *m_sampleState;
    ID3D11Buffer *m_matrixBuffer;
    ID3D11Buffer *m_clipPlaneBuffer;
    ID3D11Buffer *m_lightBuffer;

    DXShaders *shader;
};

#endif