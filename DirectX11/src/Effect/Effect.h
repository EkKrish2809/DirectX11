#ifndef EFFECT_H
#define EFFECT_H

#include <windows.h>
// #include<stdio.h>
// #include<stdlib.h>
// D3D related headers
#include <dxgi.h>
#include <D3D11.h>
#include <d3dcompiler.h>

extern FILE *gpFile;
extern char gszLogFilePathName[];

class Effect
{
    public:
        virtual BOOL Initialize(void) = 0;
        virtual void RenderFrame(XMMATRIX) = 0;
        virtual void Update(void) = 0;
        // virtual void Uninitialize(void) = 0;
};

#endif
