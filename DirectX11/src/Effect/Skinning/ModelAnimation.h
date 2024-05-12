#pragma once

#include <vector>
#include <d3d11_1.h>

#pragma warning(disable : 4838)
#include "XNAMath_204/xnamath.h"

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <assert.h>

#include "MeshAnimation.h"
#include "../Model/TextureLoader.h"
#include "Assimp_XNA_helper.h"

struct BoneInfo
{
    /*id is index in finalBoneMatrices*/
    int id;

    /*offset matrix transforms vertex from model space to bone space*/
    XMMATRIX offset;
};

class ModelAnimation
{
public:

    ModelAnimation():dev_(nullptr),
                    devcon_(nullptr),
                    meshes_(),
                    directory_(),
                    textures_loaded_(),
                    hwnd_(nullptr)
    {
        // empty
    }
    ~ModelAnimation() {}

    bool Load(HWND hwnd, ID3D11Device *dev, ID3D11DeviceContext *devcon, std::string filename)
    {
        Assimp::Importer importer;

        const aiScene *pScene = importer.ReadFile(filename,
                                                  aiProcess_Triangulate |
                                                      aiProcess_ConvertToLeftHanded);

        if (pScene == nullptr)
            return false;

        this->directory_ = filename.substr(0, filename.find_last_of("/\\"));

        this->dev_ = dev;
        this->devcon_ = devcon;
        this->hwnd_ = hwnd;

        processNode(pScene->mRootNode, pScene);

        return true;
    }

    void Draw(ID3D11DeviceContext *devcon)
    {
        for (size_t i = 0; i < meshes_.size(); ++i)
        {
            meshes_[i].Draw(devcon);
        }
    }

    void Close()
    {
        for (auto &t : textures_loaded_)
            t.Release();

        for (size_t i = 0; i < meshes_.size(); i++)
        {
            meshes_[i].Close();
        }
    }

    auto &GetBoneInfoMap() { return m_BoneInfoMap; }
    int &GetBoneCount() { return m_BoneCounter; }

private:
    ID3D11Device *dev_;
    ID3D11DeviceContext *devcon_;
    std::vector<MeshAnimation> meshes_;
    std::string directory_;
    std::vector<Texture_> textures_loaded_;
    HWND hwnd_;
    std::map<std::string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter = 0;

    void processNode(aiNode *node, const aiScene *scene)
    {
        for (UINT i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            meshes_.push_back(this->processMesh(mesh, scene));
        }

        for (UINT i = 0; i < node->mNumChildren; i++)
        {
            this->processNode(node->mChildren[i], scene);
        }
    }

    MeshAnimation processMesh(aiMesh *mesh, const aiScene *scene)
    {
        // Data to fill
        std::vector<VERTEX_> vertices;
        std::vector<UINT> indices;
        std::vector<Texture_> textures;

        // Walk through each of the mesh's vertices
        for (UINT i = 0; i < mesh->mNumVertices; i++)
        {
            VERTEX_ vertex;
            SetVertexBoneDataToDefault(vertex);

            vertex.X = mesh->mVertices[i].x;
            vertex.Y = mesh->mVertices[i].y;
            vertex.Z = mesh->mVertices[i].z;

            vertex.Xn = mesh->mNormals[i].x;
            vertex.Yn = mesh->mNormals[i].y;
            vertex.Zn = mesh->mNormals[i].z;

            if (mesh->mTextureCoords[0])
            {
                vertex.texcoord.x = (float)mesh->mTextureCoords[0][i].x;
                vertex.texcoord.y = (float)mesh->mTextureCoords[0][i].y;
            }

            vertices.push_back(vertex);
        }

        for (UINT i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];

            for (UINT j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

            std::vector<Texture_> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            std::vector<Texture_> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", scene);
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            std::vector<Texture_> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal", scene);
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
            std::vector<Texture_> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height", scene);
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        }

        // Bone weights
        ExtractBoneWeightForVertices(vertices, mesh, scene);

        return MeshAnimation(dev_, vertices, indices, textures);
    }

    void SetVertexBoneDataToDefault(VERTEX_ &vertex)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
        {
            vertex.m_BoneIDs[i] = -1;
            vertex.m_Weights[i] = 0.0f;
        }
    }

     void ExtractBoneWeightForVertices(std::vector<VERTEX_> &vertices, aiMesh *mesh, const aiScene *scene)
    {
        auto &boneInfoMap = m_BoneInfoMap;
        int &boneCount = m_BoneCounter;

        for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) // iterating for total count of bones
        {
            int boneID = -1;
            std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();

            // if the current bone is End bone --
            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                BoneInfo newBoneInfo;
                newBoneInfo.id = boneCount;
                newBoneInfo.offset = AssimpXNAHelper::ConvertMatrixToXNAFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
                boneInfoMap[boneName] = newBoneInfo;
                boneID = boneCount;
                boneCount++;
            }
            else
            {
                boneID = boneInfoMap[boneName].id;
            }
            assert(boneID != -1);
            // if (boneID == -1)
            // {

            // }
            auto weights = mesh->mBones[boneIndex]->mWeights;
            int numWeights = mesh->mBones[boneIndex]->mNumWeights;

            // for all the weights, set the bone data
            for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
            {
                int vertexId = weights[weightIndex].mVertexId;
                float weight = weights[weightIndex].mWeight;
                assert(vertexId <= vertices.size());
                SetVertexBoneData(vertices[vertexId], boneID, weight);
            }
        }
    }

    void SetVertexBoneData(VERTEX_ &vertex, int boneID, float weight)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
        {
            if (vertex.m_BoneIDs[i] < 0)
            {
                vertex.m_Weights[i] = weight;
                vertex.m_BoneIDs[i] = boneID;
                break;
            }
        }
    }


    std::vector<Texture_> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName, const aiScene *scene)
    {
        std::vector<Texture_> textures;
        for (UINT i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            // Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;
            for (UINT j = 0; j < textures_loaded_.size(); j++)
            {
                if (std::strcmp(textures_loaded_[j].path.c_str(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded_[j]);
                    skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if (!skip)
            { // If texture hasn't been loaded already, load it
                HRESULT hr;
                Texture_ texture;

                const aiTexture *embeddedTexture = scene->GetEmbeddedTexture(str.C_Str());
                if (embeddedTexture != nullptr)
                {
                    texture.texture = loadEmbeddedTexture(embeddedTexture);
                }
                else
                {
                    std::string filename = std::string(str.C_Str());
                    filename = directory_ + '/' + filename;
                    std::wstring filenamews = std::wstring(filename.begin(), filename.end());
                    hr = CreateWICTextureFromFile(dev_, devcon_, filenamews.c_str(), nullptr, &texture.texture);
                    if (FAILED(hr))
                        MessageBox(hwnd_, "Texture couldn't be loaded", "Error!", MB_ICONERROR | MB_OK);
                }
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                this->textures_loaded_.push_back(texture); // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            }
        }
        return textures;
    }

    ID3D11ShaderResourceView *loadEmbeddedTexture(const aiTexture *embeddedTexture)
    {
        HRESULT hr;
        ID3D11ShaderResourceView *texture = nullptr;

        if (embeddedTexture->mHeight != 0)
        {
            // Load an uncompressed ARGB8888 embedded texture
            D3D11_TEXTURE2D_DESC desc;
            desc.Width = embeddedTexture->mWidth;
            desc.Height = embeddedTexture->mHeight;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            desc.CPUAccessFlags = 0;
            desc.MiscFlags = 0;

            D3D11_SUBRESOURCE_DATA subresourceData;
            subresourceData.pSysMem = embeddedTexture->pcData;
            subresourceData.SysMemPitch = embeddedTexture->mWidth * 4;
            subresourceData.SysMemSlicePitch = embeddedTexture->mWidth * embeddedTexture->mHeight * 4;

            ID3D11Texture2D *texture2D = nullptr;
            hr = dev_->CreateTexture2D(&desc, &subresourceData, &texture2D);
            if (FAILED(hr))
                MessageBox(hwnd_, "CreateTexture2D failed!", "Error!", MB_ICONERROR | MB_OK);

            hr = dev_->CreateShaderResourceView(texture2D, nullptr, &texture);
            if (FAILED(hr))
                MessageBox(hwnd_, "CreateShaderResourceView failed!", "Error!", MB_ICONERROR | MB_OK);

            return texture;
        }

        // mHeight is 0, so try to load a compressed texture of mWidth bytes
        const size_t size = embeddedTexture->mWidth;

        hr = CreateWICTextureFromMemory(dev_, devcon_, reinterpret_cast<const unsigned char *>(embeddedTexture->pcData), size, nullptr, &texture);
        if (FAILED(hr))
            MessageBox(hwnd_, "Texture couldn't be created from memory!", "Error!", MB_ICONERROR | MB_OK);

        return texture;
    }
};