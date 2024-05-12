#pragma once
#include<assimp/quaternion.h>
#include<assimp/vector3.h>
#include<assimp/matrix4x4.h>

#pragma warning(disable : 4838)
#include "C:/XNAMath_204/xnamath.h"

class AssimpXNAHelper
{
public:
    static inline XMMATRIX ConvertMatrixToXNAFormat(const aiMatrix4x4 &from)
    {
        XMMATRIX to;
        // the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
        to._11 = from.a1;
        to._21 = from.a2;
        to._31 = from.a3;
        to._41 = from.a4;

        to._12 = from.b1;
        to._22 = from.b2;
        to._32 = from.b3;
        to._42 = from.b4;
        
        to._13 = from.c1;
        to._23 = from.c2;
        to._33 = from.c3;
        to._43 = from.c4;
        
        to._14 = from.d1;
        to._24 = from.d2;
        to._34 = from.d3;
        to._44 = from.d4;
        return to;
    }

    static inline XMVECTOR GetXNAVec(const aiVector3D &vec)
    {
        return XMVectorSet(vec.x, vec.y, vec.z, 0.0f);
    }

    static inline XMVECTOR GetXNAQuat(const aiQuaternion &pOrientation)
    {
        // return quaternion(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
        return XMQuaternionConjugate(XMVectorSet(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z));
        // return XMQuaternionConjugate(pOrientation);
    }
};