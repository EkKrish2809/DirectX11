#ifndef LIGHT_H
#define LIGHT_H

#pragma warning(disable : 4838)
#include "../../Math/XNAMath_204/xnamath.h"

class Light
{
public:
	Light() {}
	Light(const Light& other) {}
	~Light() {}

	void SetAmbientColor(float red, float green, float blue, float alpha)
    {
        m_ambientColor = XMFLOAT4(red, green, blue, alpha);
        return;
    }
	void SetDiffuseColor(float red, float green, float blue, float alpha)
    {
        m_diffuseColor = XMFLOAT4(red, green, blue, alpha);
        return;
    }
	void SetDirection(float x, float y, float z)
    {
        m_direction = XMFLOAT3(x, y, z);
        return;
    }
	void SetSpecularColor(float red, float green, float blue, float alpha)
    {
        m_specularColor = XMFLOAT4(red, green, blue, alpha);
        return;
    }

	void SetSpecularPower(float power)
    {
        m_specularPower = power;
        return;
    }
	void SetPosition(float x, float y, float z)
    {
        m_position = XMFLOAT3(x, y, z);
        return;
    }
	void SetLookAt(float x, float y, float z)
    {
        m_lookAt = XMFLOAT3(x, y, z);
    }

	XMFLOAT4 GetAmbientColor()
    {
        return m_ambientColor;
    }
	XMFLOAT4 GetDiffuseColor()
    {
        return m_diffuseColor;
    }
	XMFLOAT3 GetDirection()
    {
        return m_direction;
    }
	XMFLOAT4 GetSpecularColor()
    {
        return m_specularColor;
    }
	float GetSpecularPower()
    {
        return m_specularPower;
    }
	XMFLOAT3 GetPosition()
    {
        return m_position;
    }

	void GenerateViewMatrix()
    {
        XMFLOAT3 up;
        XMVECTOR positionVector, lookAtVector, upVector;

        up.x = 0.f;
        up.y = 1.f;
        up.z = 0.f;

        // positionVector = XMLoadFloat3(&m_position);
        // lookAtVector = XMLoadFloat3(&m_lookAt);
        // upVector = XMLoadFloat3(&up);
        positionVector = XMVectorSet(m_position.x, m_position.y, m_position.z, 1.0f);
        lookAtVector = XMVectorSet(m_lookAt.x, m_lookAt.y, m_lookAt.z, 0.0f);
        upVector = XMVectorSet(up.x, up.y, up.z, 0.0f);

        m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
        // m_viewMatrix = XMMatrixLookAtLH(m_position, m_lookAt, up);
    }
	void GenerateProjectionMatrix(float screenNear, float screenDepth)
    {
        float fieldOfView, screenAspect;

        fieldOfView = 3.14159265358979323846f / 2.0f;
        screenAspect = 1.f;

        m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);
    }

	void GetViewMatrix(XMMATRIX& viewMatrix)
    {
        viewMatrix = m_viewMatrix;
    }
	void GetProjectionMatrix(XMMATRIX& projectionMatrix)
    {
        projectionMatrix = m_projectionMatrix;
    }

    void GenerateOrthoMatrix(float width, float nearPlane, float depthPlane)
    {
        m_orthoMatrix = XMMatrixOrthographicLH(width, width, nearPlane, depthPlane);
    }

    void GetOrthoMatrix(XMMATRIX& orthoMatrix)
    {
        orthoMatrix = m_orthoMatrix;
    }

private:
	XMFLOAT4 m_ambientColor;
	XMFLOAT4 m_diffuseColor;
	XMFLOAT3 m_direction;
	XMFLOAT4 m_specularColor;
	float m_specularPower;
	XMFLOAT3 m_position;
	XMFLOAT3 m_lookAt;
	XMMATRIX m_viewMatrix;
	XMMATRIX m_projectionMatrix;
    XMMATRIX m_orthoMatrix;
};

#endif