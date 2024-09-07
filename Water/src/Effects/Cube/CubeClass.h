#ifndef CUBECLASS_H
#define CUBECLASS_H

#include <d3d11.h>
#include <DirectXMath.h>
#include <stdio.h>

extern FILE *gpFile;
extern char gszLogFilePathName[];

class CubeClass
{
public:
    CubeClass();
    CubeClass(const CubeClass &);
    ~CubeClass();

    bool Initialize(ID3D11Device *, ID3D11DeviceContext *);
    void Shutdown();
    void Render(ID3D11DeviceContext *);

    int GetIndexCount(void);

private:
    bool InitializeBuffers(ID3D11Device *);
    void RenderBuffers(ID3D11DeviceContext *);

private:
    ID3D11Buffer *m_vertexBuffer, *m_textureBuffer;
    int m_indexCount, m_vertexCount;
};

#endif