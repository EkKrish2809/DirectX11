#ifndef ANIMATOR_H_
#define ANIMATOR_H_

// #include "glm/glm.hpp"
#pragma warning(disable : 4838)
#include "C:/XNAMath_204/xnamath.h"

#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include "Animation.h"

class Animator
{
public:
    std::vector<XMMATRIX> m_FinalBoneMatrices;
    Animation *m_CurrentAnimation;
    float m_CurrentTime;
    float m_DeltaTime;

    void InitializeAnimator(Animation *animation)
    {
        m_CurrentTime = 0.0;
        m_CurrentAnimation = animation;

        m_FinalBoneMatrices.reserve(100);

        for (int i = 0; i < 100; i++)
            m_FinalBoneMatrices.push_back(XMMatrixIdentity()); // setting to identity
    }

    void UpdateAnimation(float dt)
    {
        m_DeltaTime = dt;
        if (m_CurrentAnimation)
        {
            m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
            m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
            CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), XMMatrixIdentity());
        }
    }

    void CalculateBoneTransform(const AssimpNodeData *node, XMMATRIX parentTransform)
    {
        std::string nodeName = node->name;
        XMMATRIX nodeTransform = node->transformation; // current node's transformation

        Bone *Bone = m_CurrentAnimation->FindBone(nodeName); // find the bone associated with current animation

        if (Bone)
        {
            Bone->Update(m_CurrentTime);
            nodeTransform = Bone->GetLocalTransform();  // get bone's local transform 
        }

        XMMATRIX globalTransformation = parentTransform * nodeTransform;   // multiply the bone's local transform with its parent's transform to get global transformation

        auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end())
        {
            int index = boneInfoMap[nodeName].id;
            XMMATRIX offset = boneInfoMap[nodeName].offset;
            m_FinalBoneMatrices[index] = globalTransformation * offset;
        }

        for (int i = 0; i < node->childrenCount; i++)
            CalculateBoneTransform(&node->children[i], globalTransformation); // calculate the bone transform for its children (hierarchy)
    }

    std::vector<XMMATRIX> GetFinalBoneMatrices()
    {
        return m_FinalBoneMatrices;
    }
};

#endif
