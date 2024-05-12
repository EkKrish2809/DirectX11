#pragma once

#include <vector>
#include <assimp/scene.h>
#include <list>

#pragma warning(disable : 4838)
#include "C:/XNAMath_204/xnamath.h"

#include "Assimp_XNA_helper.h"

struct KeyPosition
{
	// XMVECTOR position;
	FLOAT x, y, z;
	float timeStamp;
};

struct KeyRotation
{
	// XMVECTOR orientation;
	FLOAT x, y, z, w;
	float timeStamp;
};

struct KeyScale
{
	// XMVECTOR scale;
	FLOAT x, y, z;
	float timeStamp;
};


class Bone
{
public:
    std::vector<KeyPosition> m_Positions;
	std::vector<KeyRotation> m_Rotations;
	std::vector<KeyScale> m_Scales;
	int m_NumPositions;
	int m_NumRotations;
	int m_NumScalings;

	XMMATRIX m_LocalTransform;
	std::string m_Name;
	int m_ID;

        // Reading the Bone Animation from aiNodeAnim channel
    Bone(const std::string &name, int ID, const aiNodeAnim *channel): m_Name(name),
                                                                        m_ID(ID),
                                                                        m_LocalTransform(XMMatrixIdentity())
    {
        m_NumPositions = channel->mNumPositionKeys;

        for (int positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex)    // reading Positions and timestamp
        {
            aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
            float timeStamp = channel->mPositionKeys[positionIndex].mTime;
            KeyPosition data;
            // data.position = AssimpXNAHelper::GetXNAVec(aiPosition);
			data.x = aiPosition.x;
			data.y = aiPosition.y;
			data.z = aiPosition.z;
            data.timeStamp = timeStamp;
            m_Positions.push_back(data);
        }

        m_NumRotations = channel->mNumRotationKeys;
        for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex)    // reading Rotations (quaternion) and timestamp
        {
            aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
            float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
            KeyRotation data;
            // data.orientation = AssimpXNAHelper::GetXNAQuat(aiOrientation);
			data.x = aiOrientation.x;
			data.y = aiOrientation.y;
			data.z = aiOrientation.z;
			data.w = aiOrientation.w;
            data.timeStamp = timeStamp;
            m_Rotations.push_back(data);
        }

        m_NumScalings = channel->mNumScalingKeys;
        for (int keyIndex = 0; keyIndex < m_NumScalings; ++keyIndex)                // reding Scale and timestamp
        {
            aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
            float timeStamp = channel->mScalingKeys[keyIndex].mTime;
            KeyScale data;
            // data.scale = AssimpXNAHelper::GetXNAVec(scale);
			data.x = scale.x;
			data.y = scale.y;
			data.z = scale.z;
            data.timeStamp = timeStamp;
            m_Scales.push_back(data);
        }
    }

    void Update(float animationTime)
	{
		XMMATRIX translation = InterpolatePosition(animationTime);
		XMMATRIX rotation = InterpolateRotation(animationTime);
		XMMATRIX scale = InterpolateScaling(animationTime);
		m_LocalTransform = translation * rotation * scale;  // check the order
	}

    XMMATRIX GetLocalTransform() { return m_LocalTransform; }
	std::string GetBoneName() const { return m_Name; }
	int GetBoneID() { return m_ID; }

    float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
	{
		float scaleFactor = 0.0f;
		float midWayLength = animationTime - lastTimeStamp;
		float framesDiff = nextTimeStamp - lastTimeStamp;
		scaleFactor = midWayLength / framesDiff;
		return scaleFactor;
	}

    // interpolate position
    XMMATRIX InterpolatePosition(float animationTime)
	{
		if (1 == m_NumPositions)
		{
			// return glm::translate(glm::mat4(1.0f), m_Positions[0].position);
			XMMATRIX translation = XMMatrixIdentity();
			translation = translation *XMMatrixTranslation(m_Positions[0].x, m_Positions[0].y, m_Positions[0].z);
			return translation;
		}

		int p0Index = GetPositionIndex(animationTime);
		int p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(m_Positions[p0Index].timeStamp,
			m_Positions[p1Index].timeStamp, animationTime);
		XMVECTOR finalPosition = XMVectorLerp(XMVectorSet(m_Positions[p0Index].x, m_Positions[p0Index].y, m_Positions[p0Index].z, 0.0f),
											 XMVectorSet(m_Positions[p1Index].x, m_Positions[p1Index].y, m_Positions[p1Index].z, 0.0f)
											, scaleFactor);
			
		XMMATRIX finalTrans = XMMatrixIdentity();
		finalTrans = finalTrans * XMMatrixTranslation(XMVectorGetX(finalPosition), XMVectorGetY(finalPosition), XMVectorGetZ(finalPosition));
		return finalTrans;
		// return glm::translate(glm::mat4(1.0f), finalPosition);
	}

	 int GetPositionIndex(float animationTime)
	{
		for (int index = 0; index < m_NumPositions - 1; ++index)
		{
			if (animationTime < m_Positions[index + 1].timeStamp)
				return index;
		}
		// assert(0);
        return 0;
	}

    // interpolate rotations
    XMMATRIX InterpolateRotation(float animationTime)
	{
		if (1 == m_NumRotations)
		{
			
			auto rotation = XMVector3Normalize(XMVectorSet(m_Rotations[0].x, m_Rotations[0].y, m_Rotations[0].z, 0.0f));
			XMMATRIX rotationMat = XMMatrixIdentity();
			// vmath::quaternionToMatrix(rotation, rotationMat);
			rotationMat = XMMatrixRotationQuaternion(rotation);
			return rotationMat;
		}

		int p0Index = GetRotationIndex(animationTime);
		int p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(m_Rotations[p0Index].timeStamp,
			m_Rotations[p1Index].timeStamp, animationTime);
		XMVECTOR finalRotation = XMQuaternionSlerp(XMVectorSet(m_Rotations[p0Index].x, m_Rotations[p0Index].y, m_Rotations[p0Index].z, 0.0f), 
													XMVectorSet(m_Rotations[p1Index].x, m_Rotations[p1Index].y, m_Rotations[p1Index].z, 0.0f)
													, scaleFactor);
		finalRotation = XMVector3Normalize(finalRotation);
		XMMATRIX finalRoatationMat = XMMatrixIdentity();
		// vmath::quaternionToMatrix(finalRotation, finalRoatationMat);
		finalRoatationMat = XMMatrixRotationQuaternion(finalRotation);
		return finalRoatationMat;

	}

	int GetRotationIndex(float animationTime)
	{
		for (int index = 0; index < m_NumRotations - 1; ++index)
		{
			if (animationTime < m_Rotations[index + 1].timeStamp)
				return index;
		}
		// assert(0);
        return 0;
	}

    // interpolate scaling
    XMMATRIX InterpolateScaling(float animationTime)
	{
		if (1 == m_NumScalings)
		{
			XMMATRIX scaleMat = XMMatrixIdentity();
			scaleMat = scaleMat * XMMatrixScaling(m_Scales[0].x, 
												m_Scales[0].y, 
												m_Scales[0].z);
			return scaleMat;
			// return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);
		}

		int p0Index = GetScaleIndex(animationTime);
		int p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(m_Scales[p0Index].timeStamp,
			m_Scales[p1Index].timeStamp, animationTime);
		XMVECTOR finalScale = XMVectorLerp(XMVectorSet(m_Scales[p0Index].x, m_Scales[p0Index].y, m_Scales[p0Index].z, 0.0f), 
										XMVectorSet(m_Scales[p1Index].x, m_Scales[p1Index].y, m_Scales[p1Index].z, 0.0f)
										, scaleFactor);
		XMMATRIX finalscaleMat = XMMatrixIdentity();
		finalscaleMat = finalscaleMat * XMMatrixScaling(XMVectorGetX(finalScale), XMVectorGetY(finalScale), XMVectorGetZ(finalScale));
		return finalscaleMat;
		// return glm::scale(glm::mat4(1.0f), finalScale);
	}

    int GetScaleIndex(float animationTime)
	{
		for (int index = 0; index < m_NumScalings - 1; ++index)
		{
			if (animationTime < m_Scales[index + 1].timeStamp)
				return index;
		}
		// assert(0);
        return 0;
	}

};
