#ifndef _WATERSHADERCLASS_H_
#define _WATERSHADERCLASS_H_

#include <D3D11.h>
#include <DirectXMath.h>

#include <fstream>

#include "../../Common/DXShaders.h"

extern FILE *gpFile;
extern char gszLogFilePathName[];

class WaterShaderClass
{
private:
    struct MatrixBufferType
    {
        DirectX::XMMATRIX world;
        DirectX::XMMATRIX view;
        DirectX::XMMATRIX projection;
        DirectX::XMMATRIX reflection;
    };

    struct CamNormBufferType
    {
        DirectX::XMFLOAT3 cameraPosition;
        float padding1;
        DirectX::XMFLOAT2 normalMapTiling;
        DirectX::XMFLOAT2 padding2;
    };

    struct WaterBufferType
    {
        DirectX::XMVECTOR refractionTint;
        DirectX::XMFLOAT3 lightDirection;
        float waterTranslation;
        float reflectRefractScale;
        float specularShininess;
        DirectX::XMFLOAT2 padding;
    };

public:
    WaterShaderClass();
    WaterShaderClass(const WaterShaderClass &);
    ~WaterShaderClass();

    bool Initialize(ID3D11Device *, HWND);
    void Shutdown();
    bool Render(ID3D11DeviceContext *, int, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX, ID3D11ShaderResourceView *, ID3D11ShaderResourceView *,
                ID3D11ShaderResourceView *, DirectX::XMFLOAT3, DirectX::XMFLOAT2, float, float, DirectX::XMVECTOR, DirectX::XMFLOAT3, float);

private:
    bool InitializeShader(ID3D11Device *, HWND, WCHAR *, WCHAR *);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob *, HWND, WCHAR *);

    bool SetShaderParameters(ID3D11DeviceContext *, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX, ID3D11ShaderResourceView *, ID3D11ShaderResourceView *,
                             ID3D11ShaderResourceView *, DirectX::XMFLOAT3, DirectX::XMFLOAT2, float, float, DirectX::XMVECTOR, DirectX::XMFLOAT3, float);
    void RenderShader(ID3D11DeviceContext *, int);

private:
    ID3D11VertexShader *m_vertexShader;
    ID3D11PixelShader *m_pixelShader;
    ID3D11InputLayout *m_layout;
    ID3D11SamplerState *m_sampleState;
    ID3D11Buffer *m_matrixBuffer;
    ID3D11Buffer *m_camNormBuffer;
    ID3D11Buffer *m_waterBuffer;

    DXShaders *shader;
};

#endif