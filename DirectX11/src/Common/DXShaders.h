#pragma once


#include <stdio.h>
#include <iostream>
#include <fcntl.h> // For _O_RDONLY
#include <sys/stat.h> // For _S_IREAD

// D3D related headers
#include <dxgi.h>
#include <D3D11.h>
#include <d3dcompiler.h>
// #include "MyLogger.h"

extern FILE *gpFile;
extern char gszLogFilePathName[];
extern IDXGISwapChain *pIDXGISwapChain;
extern ID3D11Device *pID3D11Device;
extern ID3D11DeviceContext *pID3D11DeviceContext;
extern ID3D11RenderTargetView *pID3D11RenderTargetView;
// extern std::shared_ptr<MyLogger> myLog;

enum DXShaderType
{
	VERTEX_SHADER = 0,
	HULL_SHADER,
	DOMAIN_SHADER,
	GEOMETRY_SHADER,
	PIXEL_SHADER,
	COMPUTE_SHADER
};

#define DX_VERTEX_SHADER    "VS"
#define DX_HULL_SHADER      "HS"
#define DX_DOMAIN_SHADER    "DS"
#define DX_GEOMETRY_SHADER  "GS"
#define DX_PIXEL_SHADER     "PS"
#define DX_COMPUTE_SHADER   "CS"


class DXShaders
{
    public:
        // DXShaders(std::shared_ptr<MyLogger> _log) : m_log(_log)
        DXShaders()
        {

        }
        ~DXShaders() {}
        BOOL CreateAndCompileVertexShaderObjects(const char *shaderFilename, DXShaderType shaderType, ID3D11VertexShader **shader, ID3DBlob **pID3DBlob_ShaderCode);
        BOOL CreateAndCompileGeometryShaderObjects(const char *shaderFilename, DXShaderType shaderType, ID3D11GeometryShader **shader, ID3DBlob **pID3DBlob_ShaderCode);
        BOOL CreateAndCompilePixelShaderObjects(const char *shaderFilename, DXShaderType shaderType, ID3D11PixelShader **shader, ID3DBlob **pID3DBlob_ShaderCode);
        // BOOL LinkShaderProgramObject(GLuint shaderProgramObject);
        // void UninitializeShaders(GLuint shaderProgramObject);

    private:
        // ID3D11VertexShader *gpID3D11VertexShader = NULL;
        ID3DBlob *pID3DBlob_ShaderCode = NULL;
        ID3DBlob *pID3DBlob_Error = NULL;
        HRESULT hr;
        // std::shared_ptr<MyLogger> m_log;
};




