#ifndef _TEXTURECLASS_H_
#define _TEXTURECLASS_H_

#include <D3D11.h>
#include <DirectXMath.h>
#include <xstring>

// #include "DDSTextureLoader.h"
// #include "WICTextureLoader.h"
#include "TextureLoader.h"

class TextureClass
{
public:
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, WCHAR*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

private:
	// bool LoadDDSTexture(ID3D11Device*, const std::wstring& );
	ID3D11ShaderResourceView* m_texture;
};

#endif