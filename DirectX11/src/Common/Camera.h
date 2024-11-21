#pragma once

// #include <Windows.h>
// This includes the global perspectiveProjectionMatrix
// #include "../common.h"
#pragma warning(disable:4838)
#include "XNAMath_204\xnamath.h"

extern XMMATRIX PerspectiveProjectionMatrix;

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

    XMVECTOR position = XMVectorSet(0.0, 20.0, 5.0, 0.0);
    XMVECTOR front = XMVectorSet(0.0, 0.0, -1.0, 0.0);
    XMVECTOR up = XMVectorSet(0.0, 1.0, 0.0, 0.0);
    XMVECTOR right;
    XMVECTOR worldUp = up;

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
        position = XMVectorSet(_position[0], _position[1], _position[2], 0.0);

        updateCameraVectors();
        updateReflectionVectors();
    }

    XMMATRIX getViewMatrix(void)
    {
        XMMATRIX tempViewMatrix = XMMatrixIdentity();
        XMVECTOR center;
        center = position + front;

        tempViewMatrix = XMMatrixLookAtLH(position, center, up);

        return tempViewMatrix;
    }

    XMMATRIX getReflectionMatrix(float h)
    {
        XMMATRIX tempViewMatrix = XMMatrixIdentity();
        XMVECTOR center;
        XMVECTOR position_;
        position_ = XMVectorSet(XMVectorGetX(position),
                                -XMVectorGetY(position) + (h * 2.0f),
                                XMVectorGetZ(position), 1.0f);
        center = position_ + front;

        tempViewMatrix = XMMatrixLookAtLH(position_, center, up);

        return tempViewMatrix;
    }

    XMVECTOR getEye(void)
    {
        return position;
    }

    XMVECTOR getCenter(void)
    {
        return (XMVectorAdd(position, front));
    }

    XMVECTOR getUp()
    {
        return up;
    }

    void updateCameraVectors(void)
    {

        XMVECTOR front_ = XMVectorSet(
            cos(degToRad(yaw)) * cos(degToRad(pitch)),
            sin(degToRad(pitch)),
            sin(degToRad(yaw)) * cos(degToRad(pitch)),
            0.0);

        front = XMVector3Normalize(front_);

        right = XMVector3Normalize(XMVector3Cross(front, worldUp));
        up = XMVector3Normalize(XMVector3Cross(right, front));
    }

    void updateReflectionVectors(void)
    {

        XMVECTOR front_ = XMVectorSet(
            cos(degToRad(yaw)) * cos(degToRad(-pitch)),
            sin(degToRad(-pitch)),
            sin(degToRad(yaw)) * cos(degToRad(-pitch)),
            0.0);

        front = XMVector3Normalize(front_);

        right = XMVector3Normalize(XMVector3Cross(front, worldUp));
        up = XMVector3Normalize(XMVector3Cross(right, front));
    }

    void updateResolution(float _width, float _height)
    {
        width = _width;
        height = _height;
    }

    void keyboardInputs(WPARAM keyPressed)
    {
        // in
        float velocity = movementSpeed * 0.01;
        if (keyPressed == 'w' || keyPressed == 'W')
        {
            position = XMVectorAdd(position, XMVectorScale(front, velocity));
            // position = position + (front * velocity);
        }

        // left
        if (keyPressed == 'a' || keyPressed == 'A')
        {
            position = XMVectorSubtract(position, XMVectorScale(right, velocity));
            // position = position - (right * velocity);
        }

        // out
        if (keyPressed == 's' || keyPressed == 'S')
        {
            position = XMVectorSubtract(position, XMVectorScale(front, velocity));
            // position = position - (front * velocity);
        }

        // right
        if (keyPressed == 'd' || keyPressed == 'D')
        {
            position = XMVectorAdd(position, XMVectorScale(right, velocity));
            // position = position + (right * velocity);
        }

        // up
        if (keyPressed == 'v' || keyPressed == 'V')
        {
            position = XMVectorAdd(position, XMVectorScale(up, velocity));
            // position = position + (up * velocity);
        }

        // down
        if (keyPressed == ' ')
        {
            position = XMVectorSubtract(position, XMVectorScale(up, velocity));
            // position = position - (up * velocity);
        }

        // if (keyPressed.shiftKey)
        // {
        //     movementSpeed = 6.5;
        // }
        updateCameraVectors();
    }

    void invertPitch(void)
    {
        // pitch = -pitch;
        updateCameraVectors();
        updateReflectionVectors();
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
        updateReflectionVectors();
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

        PerspectiveProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), (float)width/(float)height, 0.01f, 1000.0f);
    }

    float degToRad(float degrees)
    {
        return (degrees * XM_PI / 180.0);
    }
};

// template <typename T>
// XMVECTOR AddVector3f(XMVECTOR v1, XMVECTOR v2)
// {
//     XMVECTOR out;

    
// }