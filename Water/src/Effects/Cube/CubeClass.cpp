#include "CubeClass.h"

CubeClass::CubeClass()
{
    m_vertexBuffer = 0;
    m_textureBuffer = 0;
}

CubeClass::CubeClass(const CubeClass &other)
{
}

CubeClass::~CubeClass()
{
}

bool CubeClass::Initialize(ID3D11Device *device, ID3D11DeviceContext *deviceContext)
{
    // Initialize the vertex and index buffer that hold the geometry for the terrain.
    bool result = InitializeBuffers(device);
    if (!result)
    {
        return false;
    }
}

bool CubeClass::InitializeBuffers(ID3D11Device *device)
{
    
    // ############ Geometry ##############
    const float cubeVertices[] =
        {
            // position : x,y,z
            // SIDE 1 ( TOP )
            -1.0f,
            +1.0f,
            +1.0f,
            +1.0f,
            +1.0f,
            +1.0f,
            -1.0f,
            +1.0f,
            -1.0f,

            -1.0f,
            +1.0f,
            -1.0f,
            +1.0f,
            +1.0f,
            +1.0f,
            +1.0f,
            +1.0f,
            -1.0f,

            // SIDE 2 ( BOTTOM )
            +1.0f,
            -1.0f,
            -1.0f,
            +1.0f,
            -1.0f,
            +1.0f,
            -1.0f,
            -1.0f,
            -1.0f,

            -1.0f,
            -1.0f,
            -1.0f,
            +1.0f,
            -1.0f,
            +1.0f,
            -1.0f,
            -1.0f,
            +1.0f,

            // SIDE 3 ( FRONT )
            -1.0f,
            +1.0f,
            -1.0f,
            +1.0f,
            +1.0f,
            -1.0f,
            -1.0f,
            -1.0f,
            -1.0f,

            -1.0f,
            -1.0f,
            -1.0f,
            +1.0f,
            +1.0f,
            -1.0f,
            +1.0f,
            -1.0f,
            -1.0f,

            // SIDE 4 ( BACK )
            +1.0f,
            -1.0f,
            +1.0f,
            +1.0f,
            +1.0f,
            +1.0f,
            -1.0f,
            -1.0f,
            +1.0f,

            -1.0f,
            -1.0f,
            +1.0f,
            +1.0f,
            +1.0f,
            +1.0f,
            -1.0f,
            +1.0f,
            +1.0f,

            // SIDE 5 ( LEFT )
            -1.0f,
            +1.0f,
            +1.0f,
            -1.0f,
            +1.0f,
            -1.0f,
            -1.0f,
            -1.0f,
            +1.0f,

            -1.0f,
            -1.0f,
            +1.0f,
            -1.0f,
            +1.0f,
            -1.0f,
            -1.0f,
            -1.0f,
            -1.0f,

            // SIDE 6 ( RIGHT )
            +1.0f,
            -1.0f,
            -1.0f,
            +1.0f,
            +1.0f,
            -1.0f,
            +1.0f,
            -1.0f,
            +1.0f,

            +1.0f,
            -1.0f,
            +1.0f,
            +1.0f,
            +1.0f,
            -1.0f,
            +1.0f,
            +1.0f,
            +1.0f,
        };

    const float cubeTexcoords[] =
        {
            // Tex : u,v
            // SIDE 1 ( TOP )
            +0.0f,
            +0.0f,
            +0.0f,
            +1.0f,
            +1.0f,
            +0.0f,

            +1.0f,
            +0.0f,
            +0.0f,
            +1.0f,
            +1.0f,
            +1.0f,

            // SIDE 2 ( BOTTOM )
            +0.0f,
            +0.0f,
            +0.0f,
            +1.0f,
            +1.0f,
            +0.0f,

            +1.0f,
            +0.0f,
            +0.0f,
            +1.0f,
            +1.0f,
            +1.0f,

            // SIDE 3 ( FRONT )
            +0.0f,
            +0.0f,
            +0.0f,
            +1.0f,
            +1.0f,
            +0.0f,

            +1.0f,
            +0.0f,
            +0.0f,
            +1.0f,
            +1.0f,
            +1.0f,

            // SIDE 4 ( BACK )
            +0.0f,
            +0.0f,
            +0.0f,
            +1.0f,
            +1.0f,
            +0.0f,

            +1.0f,
            +0.0f,
            +0.0f,
            +1.0f,
            +1.0f,
            +1.0f,

            // SIDE 5 ( LEFT )
            +0.0f,
            +0.0f,
            +0.0f,
            +1.0f,
            +1.0f,
            +0.0f,

            +1.0f,
            +0.0f,
            +0.0f,
            +1.0f,
            +1.0f,
            +1.0f,

            // SIDE 6 ( RIGHT )
            +0.0f,
            +0.0f,
            +0.0f,
            +1.0f,
            +1.0f,
            +0.0f,

            +1.0f,
            +0.0f,
            +0.0f,
            +1.0f,
            +1.0f,
            +1.0f,

        };

    // Set the index count to the same as the vertex count.
	m_indexCount = sizeof(cubeVertices);

    // create vertex buffer for above position vertices
    //  A. initialize Buffer Descriptor... like glGenBuffer()
    D3D11_BUFFER_DESC d3d11BufferDescriptor;
    ZeroMemory((void *)&d3d11BufferDescriptor, sizeof(D3D11_BUFFER_DESC));
    d3d11BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
    d3d11BufferDescriptor.ByteWidth = sizeof(float) * _ARRAYSIZE(cubeVertices);
    d3d11BufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    // B. initialize subresource data structure to put data into the buffer
    D3D11_SUBRESOURCE_DATA d3d11SubresourceData;
    ZeroMemory((void *)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
    d3d11SubresourceData.pSysMem = cubeVertices;

    // C. Create the actual buffer
    HRESULT hr = device->CreateBuffer(&d3d11BufferDescriptor, &d3d11SubresourceData, &m_vertexBuffer);
    if (FAILED(hr))
    {
        fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
        fprintf(gpFile, "gpID3D11Device::CreateBuffer() Failed for Position ...\n");
        fclose(gpFile);
        return (false);
    }

    // Texture
    // A. initialize Buffer Descriptor... like glGenBuffer()
    ZeroMemory((void *)&d3d11BufferDescriptor, sizeof(D3D11_BUFFER_DESC));
    d3d11BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
    d3d11BufferDescriptor.ByteWidth = sizeof(float) * _ARRAYSIZE(cubeTexcoords);
    d3d11BufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    // B. initialize subresource data structure to put data into the buffer
    ZeroMemory((void *)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
    d3d11SubresourceData.pSysMem = cubeTexcoords;

    // C. Create the actual buffer
    hr = device->CreateBuffer(&d3d11BufferDescriptor, &d3d11SubresourceData, &m_textureBuffer);
    if (FAILED(hr))
    {
        fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
        fprintf(gpFile, "gpID3D11Device::CreateBuffer() Failed for Position ...\n");
        fclose(gpFile);
        return (false);
    }

    return true;
}

int CubeClass::GetIndexCount()
{
	return m_indexCount;
}

void CubeClass::Render(ID3D11DeviceContext *deviceContext)
{
    // Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
    RenderBuffers(deviceContext);

    return;
}

void CubeClass::RenderBuffers(ID3D11DeviceContext *deviceContext)
{
    unsigned int stride;
    unsigned int offset;

    // Set vertex buffer stride and offset.
    stride = sizeof(float) * 3;
    offset = 0;

    // Set the vertex buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set color buffer into Input Assembly stage of pipeline
    stride = sizeof(float) * 2;
    offset = 0;
    deviceContext->IASetVertexBuffers(1, 1, &m_textureBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
    // deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    return;
}