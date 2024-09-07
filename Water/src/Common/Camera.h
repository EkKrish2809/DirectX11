#pragma once

#include <Windows.h>
// This includes the global perspectiveProjectionMatrix
// #include "../common.h"
#include <DirectXMath.h>

extern DirectX::XMMATRIX PerspectiveProjectionMatrix;

// using namespace vmath;

const float YAW = -90.0;
const float PITCH = 0.0;
const float SPEED = 10.5;
const float SENSITIVITY = 0.1;
const float ZOOM = 45.0;

class Camera
{
public:
    float width;
    float height;

    DirectX::XMVECTOR position = DirectX::XMVectorSet(0.0, 20.0, 5.0, 0.0);
    DirectX::XMVECTOR front = DirectX::XMVectorSet(0.0, 0.0, -1.0, 0.0);
    DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0, 1.0, 0.0, 0.0);
    DirectX::XMVECTOR right;
    DirectX::XMVECTOR worldUp = up;

    // euler angles
    float yaw = YAW;
    float pitch = PITCH;
    // Camera options
    float movementSpeed = SPEED;
    float mouseSensitivity = SENSITIVITY;
    float zoom = ZOOM;

    float lastX = 0.0;
    float lastY = 0.0;
    BOOL firstMouse = TRUE;

    Camera() {}

    Camera(float _width, float _height, float *_position)
    {
        width = _width;
        height = _height;

        // Camera attributes
        position = DirectX::XMVectorSet(_position[0], _position[1], _position[2], 1.0);

        updateCameraVectors();
    }

    DirectX::XMMATRIX getViewMatrix(void)
    {
        DirectX::XMMATRIX tempViewMatrix = DirectX::XMMatrixIdentity();
        DirectX::XMVECTOR center;
        // center = position + front;
        center = DirectX::XMVectorAdd(position, front);

        tempViewMatrix = DirectX::XMMatrixLookAtLH(position, center, up);

        return tempViewMatrix;
    }

    DirectX::XMMATRIX getViewMatrixInv(float height)
    {
        DirectX::XMMATRIX tempViewMatrix = DirectX::XMMatrixIdentity();
        DirectX::XMVECTOR center;

        // float distance = 2.0f * (DirectX::XMVectorGetY(position) - 0.0f);
        // float posY = DirectX::XMVectorGetY(position) - distance;
        // DirectX::XMVECTOR position_ = DirectX::XMVectorSet(DirectX::XMVectorGetX(position), 
        //                                                 posY, 
        //                                                 DirectX::XMVectorGetZ(position), 
        //                                                 0.0f);
        DirectX::XMVECTOR position_ = DirectX::XMVectorSet(DirectX::XMVectorGetX(position), 
                                                        -DirectX::XMVectorGetY(position) + (height * 2.0f), 
                                                        DirectX::XMVectorGetZ(position), 
                                                        0.0f);
        // center = position + front;
        center = DirectX::XMVectorAdd(position_, front);
        // center = DirectX::XMVectorSet(DirectX::XMVectorGetX(center), 
        //                             0.0f, 
        //                             DirectX::XMVectorGetX(center), 
        //                             DirectX::XMVectorGetX(center));

        tempViewMatrix = DirectX::XMMatrixLookAtLH(position_, center, up);

        return tempViewMatrix;
    }

    DirectX::XMVECTOR getEye(void)
    {
        return position;
    }

    DirectX::XMVECTOR getCenter(void)
    {
        // return (position + front);
        return DirectX::XMVectorAdd(position, front);
    }

    DirectX::XMVECTOR getUp()
    {
        return up;
    }

    void updateCameraVectors(void)
    {

        DirectX::XMVECTOR front_ = DirectX::XMVectorSet(
            cos(degToRad(yaw)) * cos(degToRad(pitch)),
            sin(degToRad(pitch)),
            sin(degToRad(yaw)) * cos(degToRad(pitch)),
            0.0);

        front = DirectX::XMVector3Normalize(front_);

        right = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(front, worldUp));
        up = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(right, front));
    }

    void updateCameraVectorsInv(void)
    {
        pitch = -pitch;
        DirectX::XMVECTOR front_ = DirectX::XMVectorSet(
            cos(degToRad(yaw)) * cos(degToRad(pitch)),
            sin(degToRad(pitch)),
            sin(degToRad(yaw)) * cos(degToRad(pitch)),
            0.0);

        front = DirectX::XMVector3Normalize(front_);

        right = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(front, worldUp));
        up = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(right, front));

        // float distance = 2.0f * (position[1] - 0.0f);
        // position[1] -= distance;
    }

    void updateResolution(float _width, float _height)
    {
        width = _width;
        height = _height;
    }

    void keyboardInputs(WPARAM keyPressed)
    {
        // in
        float velocity = movementSpeed * 0.1;
        if (keyPressed == 'w')
        {
            // position = position + (front * velocity);
            front = DirectX::XMVectorSet(DirectX::XMVectorGetX(front) * velocity, 
                                        DirectX::XMVectorGetY(front) * velocity, 
                                        DirectX::XMVectorGetZ(front) * velocity, 
                                        DirectX::XMVectorGetW(front) * velocity);
            
            // position = position + front;
            position = DirectX::XMVectorAdd(position, front);
        }

        // left
        if (keyPressed == 'a')
        {
            // position = position - (right * velocity);
            right = DirectX::XMVectorSet(DirectX::XMVectorGetX(right) * velocity, 
                                        DirectX::XMVectorGetY(right) * velocity, 
                                        DirectX::XMVectorGetZ(right) * velocity, 
                                        DirectX::XMVectorGetW(right) * velocity);
            position = DirectX::XMVectorSubtract(position, right);
        }

        // out
        if (keyPressed == 's')
        {
            // position = position - (front * velocity);
            front = DirectX::XMVectorSet(DirectX::XMVectorGetX(front) * velocity, 
                                        DirectX::XMVectorGetY(front) * velocity, 
                                        DirectX::XMVectorGetZ(front) * velocity, 
                                        DirectX::XMVectorGetW(front) * velocity);
            position = DirectX::XMVectorSubtract(position, front);
        }

        // right
        if (keyPressed == 'd')
        {
            // position = position + (right * velocity);
            right = DirectX::XMVectorSet(DirectX::XMVectorGetX(right) * velocity, 
                                        DirectX::XMVectorGetY(right) * velocity, 
                                        DirectX::XMVectorGetZ(right) * velocity, 
                                        DirectX::XMVectorGetW(right) * velocity);
            position = DirectX::XMVectorAdd(position, right);
        }

        // up
        if (keyPressed == 'v')
        {
            // position = position + (up * velocity);
            up = DirectX::XMVectorSet(DirectX::XMVectorGetX(up) * velocity, 
                                        DirectX::XMVectorGetY(up) * velocity, 
                                        DirectX::XMVectorGetZ(up) * velocity, 
                                        DirectX::XMVectorGetW(up) * velocity);
            position = DirectX::XMVectorAdd(position, up);
        }

        // down
        if (keyPressed == ' ')
        {
            // position = position - (up * velocity);
            up = DirectX::XMVectorSet(DirectX::XMVectorGetX(up) * velocity, 
                                        DirectX::XMVectorGetY(up) * velocity, 
                                        DirectX::XMVectorGetZ(up) * velocity, 
                                        DirectX::XMVectorGetW(up) * velocity);
            position = DirectX::XMVectorSubtract(position, up);
        }

        // if (keyPressed.shiftKey)
        // {
        //     movementSpeed = 6.5;
        // }
    }

    void invertPitch(void)
    {
        pitch = -pitch;
        updateCameraVectors();
        // updateCameraVectorsInv();
        pitch = -pitch;
    }

    ///////////////// TO DO IN WINDOWS
    // Detects shift key up to decrease the movementSpeed
    // void inputOnKeyUp(event)
    // {
    //     if (event.code == 'ShiftLeft')
    //     {
    //         movementSpeed = 4.5;
    //     }
    // }

    void mouseInputs(float mouseX, float mouseY)
    {
        if (firstMouse)
        {
            lastX = mouseX;
            lastY = mouseY;
            firstMouse = FALSE;
        }
        float xoffset = mouseX - lastX;
        float yoffset = lastY - mouseY;
        lastX = mouseX;
        lastY = mouseY;

        BOOL constrainPitch = TRUE;
        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;

        yaw = fmod((yaw + xoffset), 360.0f);

        pitch += yoffset;

        if (constrainPitch)
        {
            if (pitch > 89.0)
                pitch = 89.0;
            if (pitch < -89.0)
                pitch = -89.0;
        }

        updateCameraVectors();
    }

    // Process mouse scroll
    void mouseScroll(float scrollDelta)
    {
        zoom -= (float)scrollDelta;

        // PrintLog("Zoom = %f\n", zoom);

        if (zoom < -10000.0f)
            zoom = -10000.0f;

        if (zoom > 10000.0f)
            zoom = 10000.0f;

        PerspectiveProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(45.0f), (float)width/(float)height, 0.01f, 1000.0f);
    }

    float degToRad(float degrees)
    {
        return (degrees * DirectX::XM_PI / 180.0);
    }
};
