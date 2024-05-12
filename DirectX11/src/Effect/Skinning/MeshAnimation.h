#ifndef MESHANIMATION_H
#define MESHANIMATION_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <d3d11.h>
// #include <DirectXMath.h>

// using namespace DirectX;

#include "../Model/SafeRelease.hpp"

extern FILE *gpFile;

#define MAX_BONE_INFLUENCE 4

struct VERTEX_ {
	FLOAT X, Y, Z;
	FLOAT Xn, Yn, Zn;   // normal
	XMFLOAT2 texcoord;
    FLOAT Tx, Ty, Tz;   // tangent
    FLOAT BTx, BTy, BTz;    // bit tangebt

    //bone indexes which will influence this vertex
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	//weights from each bone
	float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture_ {
	std::string type;
	std::string path;
	ID3D11ShaderResourceView *texture;

	void Release() {
		SafeRelease(texture);
	}
};

class MeshAnimation {
public:
    std::vector<VERTEX_> vertices_;
    std::vector<UINT> indices_;
    std::vector<Texture_> textures_;
    ID3D11Device *dev_;

    MeshAnimation(ID3D11Device *dev, const std::vector<VERTEX_>& vertices, const std::vector<UINT>& indices, const std::vector<Texture_>& textures) :
            vertices_(vertices),
            indices_(indices),
            textures_(textures),
            dev_(dev),
            VertexBuffer_(nullptr),
            IndexBuffer_(nullptr) {
        this->setupMesh(this->dev_);
    }

    void Draw(ID3D11DeviceContext *devcon) {
        UINT stride = sizeof(VERTEX_);
        UINT offset = 0;

        devcon->IASetVertexBuffers(0, 1, &VertexBuffer_, &stride, &offset);
        devcon->IASetIndexBuffer(IndexBuffer_, DXGI_FORMAT_R32_UINT, 0);

        devcon->PSSetShaderResources(0, 1, &textures_[0].texture);

        devcon->DrawIndexed(static_cast<UINT>(indices_.size()), 0, 0);
        // devcon->Draw(static_cast<UINT>(vertices_.size()), 0);

        // fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
        // fprintf(gpFile, "Indices Count - %zd\n", indices_.size());
        // fclose(gpFile);
    }

    void Close() {
        SafeRelease(VertexBuffer_);
        SafeRelease(IndexBuffer_);
    }
private:
    // Render data
    ID3D11Buffer *VertexBuffer_, *IndexBuffer_;

    // Functions
    // Initializes all the buffer objects/arrays
    void setupMesh(ID3D11Device *dev) {
        HRESULT hr;

        D3D11_BUFFER_DESC vbd;
        vbd.Usage = D3D11_USAGE_IMMUTABLE;
        vbd.ByteWidth = static_cast<UINT>(sizeof(VERTEX_) * vertices_.size());
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbd.CPUAccessFlags = 0;
        vbd.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA initData;
        initData.pSysMem = &vertices_[0];

        hr = dev->CreateBuffer(&vbd, &initData, &VertexBuffer_);
        if (FAILED(hr)) {
            Close();
            throw std::runtime_error("Failed to create vertex buffer.");
        }

        D3D11_BUFFER_DESC ibd;
        ibd.Usage = D3D11_USAGE_IMMUTABLE;
        ibd.ByteWidth = static_cast<UINT>(sizeof(UINT) * indices_.size());
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.CPUAccessFlags = 0;
        ibd.MiscFlags = 0;

        initData.pSysMem = &indices_[0];

        hr = dev->CreateBuffer(&ibd, &initData, &IndexBuffer_);
        if (FAILED(hr)) {
            Close();
            throw std::runtime_error("Failed to create index buffer.");
        }
    }
};

#endif