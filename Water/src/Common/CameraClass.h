#ifndef CAMERACLASS_H
#define CAMERACLASS_H

#include <DirectXMath.h>

class CameraClass
{
    public:
        CameraClass();
        CameraClass(const CameraClass&);
        ~CameraClass();

        void SetPosition(float, float, float);
        void SetRotation(float, float, float);

        DirectX::XMFLOAT3 GetPosition();
        DirectX::XMFLOAT3 GetRotation();

        void Render();
        void GetViewMatrix(DirectX::XMMATRIX&);

        void GenerateBaseViewMatrix();
        void GetBaseViewMatrix(DirectX::XMMATRIX&);

        void RenderReflection(float);
	    void GetReflectionViewMatrix(DirectX::XMMATRIX&);

    private:
        float m_positionX, m_positionY, m_positionZ;
        float m_rotationX, m_rotationY, m_rotationZ;

        DirectX::XMMATRIX m_viewMatrix, m_baseViewMatrix, m_reflectionViewMatrix;
};

#endif