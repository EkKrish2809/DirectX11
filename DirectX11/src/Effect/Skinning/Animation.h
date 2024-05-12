#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <vector>
#include <map>
#include <string>
#include <functional>
// #include "glm/glm.hpp"
#pragma warning(disable : 4838)
#include "C:/XNAMath_204/xnamath.h"


#include <assimp/scene.h>
#include "ModelAnimation.h"
#include "Bone.h"

// node data --> eg. child nodes and transformations
struct AssimpNodeData
{
	XMMATRIX transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};

class Animation
{
    public:
        float m_Duration;
        int m_TicksPerSecond;
        std::vector<Bone> m_Bones;
        AssimpNodeData m_RootNode;
        std::map<std::string, BoneInfo> m_BoneInfoMap;

        void InitializeAnimation(const std::string& animationPath, ModelAnimation* model)
        {
            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
            assert(scene && scene->mRootNode);
            auto animation = scene->mAnimations[0];
            m_Duration = animation->mDuration;
            m_TicksPerSecond = animation->mTicksPerSecond;
            aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
            globalTransformation = globalTransformation.Inverse();
            ReadHierarchyData(m_RootNode, scene->mRootNode);
            ReadMissingBones(animation, *model);
        }

        void ReadHierarchyData(AssimpNodeData& dest, const aiNode *src)
        {
            assert(src);

            dest.name = src->mName.data;
            dest.transformation = AssimpXNAHelper::ConvertMatrixToXNAFormat(src->mTransformation);     // node transformation
            dest.childrenCount = src->mNumChildren;         // children of that node

            for (int i = 0; i < src->mNumChildren; i++)
            {
                AssimpNodeData newData;
                ReadHierarchyData(newData, src->mChildren[i]);  // reading Nore Hierarchy in recursion
                dest.children.push_back(newData);               // adding childern data 
            }
        }

        void ReadMissingBones(const aiAnimation* animation, ModelAnimation& model)
        {
            int size = animation->mNumChannels;

            auto& boneInfoMap = model.GetBoneInfoMap();//getting m_BoneInfoMap from ModelAnimation class
            int& boneCount = model.GetBoneCount(); //getting the m_BoneCounter from ModelAnimation class

            //reading channels(bones engaged in an animation and their keyframes)
            for (int i = 0; i < size; i++)
            {
                auto channel = animation->mChannels[i];
                std::string boneName = channel->mNodeName.data;

                if (boneInfoMap.find(boneName) == boneInfoMap.end())
                {
                    boneInfoMap[boneName].id = boneCount;
                    boneCount++;
                }
                m_Bones.push_back(Bone(channel->mNodeName.data,
                    boneInfoMap[channel->mNodeName.data].id, channel));
            }

            m_BoneInfoMap = boneInfoMap;
        }

        Bone* FindBone(const std::string& name)
        {
            auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
                [&](const Bone& Bone)
                {
                    return Bone.GetBoneName() == name;
                }
            );
            if (iter == m_Bones.end()) return nullptr;
            else return &(*iter);
        }

        inline float GetTicksPerSecond() 
        { 
            return m_TicksPerSecond; 
        }

        inline float GetDuration() 
        { 
            return m_Duration;
        }

        inline const AssimpNodeData& GetRootNode() 
        { 
            return m_RootNode; 
        }
        
        inline const std::map<std::string,BoneInfo>& GetBoneIDMap() 
        { 
            return m_BoneInfoMap;
        }
};

#endif
