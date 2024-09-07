#ifndef _WATERCLASS_H_
#define _WATERCLASS_H_

#include <D3D11.h>
#include <DirectXMath.h>

#include "../../Common/textureclass.h"

extern FILE *gpFile;
extern char gszLogFilePathName[];

class WaterClass
{
    private:
	struct VertexType
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
	};

    public:
	WaterClass();
	WaterClass(const WaterClass&);
	~WaterClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, WCHAR*, float, float);
	void Shutdown();
	void Frame();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();
	
	float GetWaterHeight();
	DirectX::XMFLOAT2 GetNormalMapTiling();
	float GetWaterTranslation();
	float GetReflectRefractScale();
	DirectX::XMVECTOR GetRefractionTint();
	float GetSpecularShininess();

private:
	bool InitializeBuffers(ID3D11Device*, float);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*,ID3D11DeviceContext*, WCHAR*);
	void ReleaseTexture();

private:
	float m_waterHeight;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	TextureClass* m_Texture;
	DirectX::XMFLOAT2 m_normalMapTiling;
	float m_waterTranslation;
	float m_reflectRefractScale;
	DirectX::XMVECTOR m_refractionTint;
	float m_specularShininess;
};


#endif
