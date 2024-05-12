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
    }

    XMMATRIX getViewMatrix(void)
    {
        XMMATRIX tempViewMatrix = XMMatrixIdentity();
        XMVECTOR center;
        center = position + front;

        tempViewMatrix = XMMatrixLookAtLH(position, center, up);

        return tempViewMatrix;
    }

    XMVECTOR getEye(void)
    {
        return position;
    }

    XMVECTOR getCenter(void)
    {
        return (position + front);
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

    void updateResolution(float _width, float _height)
    {
        width = _width;
        height = _height;
    }

    void keyboardInputs(WPARAM keyPressed)
    {
        // in
        float velocity = movementSpeed * 0.1;
        if (keyPressed == 'w' || keyPressed == 'W')
        {
            position = position + (front * velocity);
        }

        // left
        if (keyPressed == 'a' || keyPressed == 'A')
        {
            position = position - (right * velocity);
        }

        // out
        if (keyPressed == 's' || keyPressed == 'S')
        {
            position = position - (front * velocity);
        }

        // right
        if (keyPressed == 'd' || keyPressed == 'D')
        {
            position = position + (right * velocity);
        }

        // up
        if (keyPressed == 'v' || keyPressed == 'V')
        {
            position = position + (up * velocity);
        }

        // down
        if (keyPressed == ' ')
        {
            position = position - (up * velocity);
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