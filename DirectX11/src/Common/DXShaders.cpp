/*
* ***********************************************************************************
*   Author: Pradnya Vijay Gokhale                                                   *
* ***********************************************************************************
*/

#include <io.h>

#include "DXShaders.h"

BOOL DXShaders::CreateAndCompileVertexShaderObjects(const char *shaderFilename, DXShaderType shaderType, ID3D11VertexShader **shader, ID3DBlob **pID3DBlob_ShaderCode)
{
	// variable declarations
	// GLuint shaderObject = 0;
	char shaderNameTag[50];
	char *glShaderType = NULL;

	int fdShaderFile = -1;
	long shaderFileSize = -1;
	char *shaderSourceCodeBuffer = NULL;

	int status = 0;
	int infoLogLength = 0;
	char *log = NULL;

	// code
	fdShaderFile = _open(shaderFilename, _O_RDONLY, _S_IREAD);
	if (fdShaderFile == -1)
	{
        fopen_s(&gpFile, gszLogFilePathName, "a+");
		fprintf(gpFile, "Failed To Open Shader File %s! Exitting Now ... 	:: %ls\n", shaderFilename, __FUNCTIONW__);
        fclose(gpFile);
        // m_log->Logger("Failed To Open Shader File %s! Exitting Now ... 	:: %ls\n", shaderFilename, __FUNCTIONW__);
		return(FALSE);
	}

	shaderFileSize = _lseek(fdShaderFile, 0, SEEK_END);
	shaderFileSize = shaderFileSize + 1; // '\0'
	shaderSourceCodeBuffer = (char *)malloc(shaderFileSize);
	_lseek(fdShaderFile, 0, SEEK_SET);

	shaderFileSize = _read(fdShaderFile, (char *)shaderSourceCodeBuffer, shaderFileSize);
	if (shaderFileSize < 0)
	{
        fopen_s(&gpFile, gszLogFilePathName, "a+");
		fprintf(gpFile,"Failed To Read Shader File %s! Exitting Now ... 	:: %ls\n", shaderFilename, __FUNCTIONW__);
        fclose(gpFile);
        // m_log->Logger("Failed To Read Shader File %s! Exitting Now ... 	:: %ls\n", shaderFilename, __FUNCTIONW__);
		return(FALSE);
	}
	
	shaderSourceCodeBuffer[shaderFileSize] = '\0';
	_close(fdShaderFile);

	switch (shaderType)
	{
	case VERTEX_SHADER:
		strcpy_s(shaderNameTag, 50, "Vertex");
		glShaderType = DX_VERTEX_SHADER;
		break;

	case HULL_SHADER:
		strcpy_s(shaderNameTag, 50, "Tessellation Control");
		glShaderType = DX_HULL_SHADER;
		break;

	case DOMAIN_SHADER:
		strcpy_s(shaderNameTag, 50, "Tessellation Evaluation");
		glShaderType = DX_DOMAIN_SHADER;
		break;

	case GEOMETRY_SHADER:
		strcpy_s(shaderNameTag, 50, "Geometry");
		glShaderType = DX_GEOMETRY_SHADER;
		break;

	case PIXEL_SHADER:
		strcpy_s(shaderNameTag, 50, "Fragment");
		glShaderType = DX_PIXEL_SHADER;
		break;

	case COMPUTE_SHADER:
		strcpy_s(shaderNameTag, 50, "Compute");
		glShaderType = DX_COMPUTE_SHADER;
		break;

	default:
        fopen_s(&gpFile, gszLogFilePathName, "a+");
		fprintf(gpFile,"Invalid Shader Type! Exitting Now ...	:: %ls\n", __FUNCTIONW__);
        fclose(gpFile);
        // m_log->Logger("Invalid Shader Type! Exitting Now ...	:: %ls\n", __FUNCTIONW__);
		return(0);
	}

	// CREATING SHADER OBJECT
   hr = D3DCompile(shaderSourceCodeBuffer,
                    lstrlenA(shaderSourceCodeBuffer)+1,
                    glShaderType,
                    NULL,
                    D3D_COMPILE_STANDARD_FILE_INCLUDE,
                    "main",
                    "vs_5_0",
                    0,0,
                    pID3DBlob_ShaderCode,
                    &pID3DBlob_Error);
    if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		if (pID3DBlob_Error)
		{
			fprintf(gpFile, "D3DCompile() Failed for Vertex Shader :: < %s > ... 	:: %ls\n", (char *)pID3DBlob_Error->GetBufferPointer(), __FUNCTIONW__);
        	// m_log->Logger("D3DCompile() Failed for Vertex Shader :: < %s > ... 	:: %ls\n", (char *)pID3DBlob_Error->GetBufferPointer(), __FUNCTIONW__);
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
		}
		else
		{
        	// m_log->Logger("D3DCompile() Failed for Vertex Shader :: < UNKNOWN > ... 	:: %ls\n", __FUNCTIONW__);
			fprintf(gpFile, "D3DCompile() Failed for Vertex Shader :: < UNKNOWN > ... 	:: %ls\n", __FUNCTIONW__);
		}
		fclose(gpFile);
        return (FALSE);
	}
	else
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "D3DCompile() Succedded for Vertex Shader... 	:: %ls\n", __FUNCTIONW__);
		fclose(gpFile);
        	// m_log->Logger("D3DCompile() Succedded for Vertex Shader... 	:: %ls\n", __FUNCTIONW__);
	}
	
   // create vertex shader
    hr = pID3D11Device->CreateVertexShader((*pID3DBlob_ShaderCode)->GetBufferPointer(),
                                            (*pID3DBlob_ShaderCode)->GetBufferSize(),
                                            NULL,
                                            &(*shader));
    if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "ID3D11Device::CreateVertexShader() Failed ... 	:: %ls\n", __FUNCTIONW__);
		fclose(gpFile);
        // m_log->Logger("ID3D11Device::CreateVertexShader() Failed ... 	:: %ls\n", __FUNCTIONW__);
        return (FALSE);
	}
	else
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "ID3D11Device::CreateVertexShader() Successfull ... 	:: %ls\n", __FUNCTIONW__);
		fclose(gpFile);
        // m_log->Logger("ID3D11Device::CreateVertexShader() Successfull ... 	:: %ls\n", __FUNCTIONW__);
	}

    return (TRUE);
}

BOOL DXShaders::CreateAndCompileGeometryShaderObjects(const char *shaderFilename, DXShaderType shaderType, ID3D11GeometryShader **shader, ID3DBlob **pID3DBlob_ShaderCode)
{
	// variable declarations
	// GLuint shaderObject = 0;
	char shaderNameTag[50];
	char *glShaderType = NULL;

	int fdShaderFile = -1;
	long shaderFileSize = -1;
	char *shaderSourceCodeBuffer = NULL;

	int status = 0;
	int infoLogLength = 0;
	char *log = NULL;

	// code
	fdShaderFile = _open(shaderFilename, _O_RDONLY, _S_IREAD);
	if (fdShaderFile == -1)
	{
        fopen_s(&gpFile, gszLogFilePathName, "a+");
		fprintf(gpFile, "Failed To Open Shader File %s! Exitting Now ... 	:: %ls\n", shaderFilename, __FUNCTIONW__);
        fclose(gpFile);
        // m_log->Logger("Failed To Open Shader File %s! Exitting Now ... 	:: %ls\n", shaderFilename, __FUNCTIONW__);
		return(FALSE);
	}

	shaderFileSize = _lseek(fdShaderFile, 0, SEEK_END);
	shaderFileSize = shaderFileSize + 1; // '\0'
	shaderSourceCodeBuffer = (char *)malloc(shaderFileSize);
	_lseek(fdShaderFile, 0, SEEK_SET);

	shaderFileSize = _read(fdShaderFile, (char *)shaderSourceCodeBuffer, shaderFileSize);
	if (shaderFileSize < 0)
	{
        fopen_s(&gpFile, gszLogFilePathName, "a+");
		fprintf(gpFile,"Failed To Read Shader File %s! Exitting Now ... 	:: %ls\n", shaderFilename, __FUNCTIONW__);
        fclose(gpFile);
        // m_log->Logger("Failed To Read Shader File %s! Exitting Now ... 	:: %ls\n", shaderFilename, __FUNCTIONW__);
		return(FALSE);
	}
	
	shaderSourceCodeBuffer[shaderFileSize] = '\0';
	_close(fdShaderFile);

	switch (shaderType)
	{
	case VERTEX_SHADER:
		strcpy_s(shaderNameTag, 50, "Vertex");
		glShaderType = DX_VERTEX_SHADER;
		break;

	case HULL_SHADER:
		strcpy_s(shaderNameTag, 50, "Tessellation Control");
		glShaderType = DX_HULL_SHADER;
		break;

	case DOMAIN_SHADER:
		strcpy_s(shaderNameTag, 50, "Tessellation Evaluation");
		glShaderType = DX_DOMAIN_SHADER;
		break;

	case GEOMETRY_SHADER:
		strcpy_s(shaderNameTag, 50, "Geometry");
		glShaderType = DX_GEOMETRY_SHADER;
		break;

	case PIXEL_SHADER:
		strcpy_s(shaderNameTag, 50, "Fragment");
		glShaderType = DX_PIXEL_SHADER;
		break;

	case COMPUTE_SHADER:
		strcpy_s(shaderNameTag, 50, "Compute");
		glShaderType = DX_COMPUTE_SHADER;
		break;

	default:
        fopen_s(&gpFile, gszLogFilePathName, "a+");
		fprintf(gpFile,"Invalid Shader Type! Exitting Now ... 	:: %ls\n", __FUNCTIONW__);
        fclose(gpFile);
        // m_log->Logger("Invalid Shader Type! Exitting Now ... 	:: %ls\n", __FUNCTIONW__);
		return(0);
	}

	// CREATING SHADER OBJECT
   hr = D3DCompile(shaderSourceCodeBuffer,
                    lstrlenA(shaderSourceCodeBuffer)+1,
                    glShaderType,
                    NULL,
                    D3D_COMPILE_STANDARD_FILE_INCLUDE,
                    "main",
                    "gs_5_0",
                    0,0,
                    pID3DBlob_ShaderCode,
                    &pID3DBlob_Error);
    if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		if (pID3DBlob_Error)
		{
			fprintf(gpFile, "D3DCompile() Failed for Geometry Shader :: < %s > ... 	:: %ls\n", (char *)pID3DBlob_Error->GetBufferPointer(), __FUNCTIONW__);
        	// m_log->Logger("D3DCompile() Failed for Geometry Shader :: < %s > ... 	:: %ls\n", (char *)pID3DBlob_Error->GetBufferPointer(), __FUNCTIONW__);
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
		}
		else
		{
			fprintf(gpFile, "D3DCompile() Failed for Geometry Shader :: < UNKNOWN > ... 	:: %ls\n", __FUNCTIONW__);
        	// m_log->Logger("D3DCompile() Failed for Geometry Shader :: < UNKNOWN > ... 	:: %ls\n", __FUNCTIONW__);
		}
		fclose(gpFile);
        return (FALSE);
	}
	else
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "D3DCompile() Succedded for Geometry Shader... 	:: %ls\n", __FUNCTIONW__);
		fclose(gpFile);
        // m_log->Logger("D3DCompile() Succedded for Geometry Shader... 	:: %ls\n", __FUNCTIONW__);
	}
	
   // create vertex shader
    hr = pID3D11Device->CreateGeometryShader((*pID3DBlob_ShaderCode)->GetBufferPointer(),
                                            (*pID3DBlob_ShaderCode)->GetBufferSize(),
                                            NULL,
                                            &(*shader));
    if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "ID3D11Device::CreateGeometryShader() Failed ... 	:: %ls\n", __FUNCTIONW__);
		fclose(gpFile);
        // m_log->Logger("ID3D11Device::CreateGeometryShader() Failed ... 	:: %ls\n", __FUNCTIONW__);
        return (FALSE);
	}
	else
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "ID3D11Device::CreateGeometryShader() Successfull ... 	:: %ls\n", __FUNCTIONW__);
		fclose(gpFile);
        // m_log->Logger("ID3D11Device::CreateGeometryShader() Successfull ... 	:: %ls\n", __FUNCTIONW__);
	}

    return (TRUE);
}

BOOL DXShaders::CreateAndCompilePixelShaderObjects(const char *shaderFilename, DXShaderType shaderType, ID3D11PixelShader **shader, ID3DBlob **pID3DBlob_ShaderCode)
{
	// variable declarations
	// GLuint shaderObject = 0;
	char shaderNameTag[50];
	char *glShaderType = NULL;

	int fdShaderFile = -1;
	long shaderFileSize = -1;
	char *shaderSourceCodeBuffer = NULL;

	int status = 0;
	int infoLogLength = 0;
	char *log = NULL;

	// code
	fdShaderFile = _open(shaderFilename, _O_RDONLY, _S_IREAD);
	if (fdShaderFile == -1)
	{
        fopen_s(&gpFile, gszLogFilePathName, "a+");
		fprintf(gpFile, "Failed To Open Shader File %s! Exitting Now ... 	:: %ls\n", shaderFilename, __FUNCTIONW__);
        fclose(gpFile);
        // m_log->Logger("Failed To Open Shader File %s! Exitting Now ... 	:: %ls\n", shaderFilename, __FUNCTIONW__);
		return(FALSE);
	}

	shaderFileSize = _lseek(fdShaderFile, 0, SEEK_END);
	shaderFileSize = shaderFileSize + 1; // '\0'
	shaderSourceCodeBuffer = (char *)malloc(shaderFileSize);
	_lseek(fdShaderFile, 0, SEEK_SET);

	shaderFileSize = _read(fdShaderFile, (char *)shaderSourceCodeBuffer, shaderFileSize);
	if (shaderFileSize < 0)
	{
        fopen_s(&gpFile, gszLogFilePathName, "a+");
		fprintf(gpFile,"Failed To Read Shader File %s! Exitting Now ... 	:: %ls\n", shaderFilename, __FUNCTIONW__);
        fclose(gpFile);
        // m_log->Logger("Failed To Read Shader File %s! Exitting Now ... 	:: %ls\n", shaderFilename, __FUNCTIONW__);
		return(FALSE);
	}
	
	shaderSourceCodeBuffer[shaderFileSize] = '\0';
	_close(fdShaderFile);

	switch (shaderType)
	{
	case VERTEX_SHADER:
		strcpy_s(shaderNameTag, 50, "Vertex");
		glShaderType = DX_VERTEX_SHADER;
		break;

	case HULL_SHADER:
		strcpy_s(shaderNameTag, 50, "Tessellation Control");
		glShaderType = DX_HULL_SHADER;
		break;

	case DOMAIN_SHADER:
		strcpy_s(shaderNameTag, 50, "Tessellation Evaluation");
		glShaderType = DX_DOMAIN_SHADER;
		break;

	case GEOMETRY_SHADER:
		strcpy_s(shaderNameTag, 50, "Geometry");
		glShaderType = DX_GEOMETRY_SHADER;
		break;

	case PIXEL_SHADER:
		strcpy_s(shaderNameTag, 50, "Fragment");
		glShaderType = DX_PIXEL_SHADER;
		break;

	case COMPUTE_SHADER:
		strcpy_s(shaderNameTag, 50, "Compute");
		glShaderType = DX_COMPUTE_SHADER;
		break;

	default:
        fopen_s(&gpFile, gszLogFilePathName, "a+");
		fprintf(gpFile,"Invalid Shader Type! Exitting Now ... 	:: %ls\n", __FUNCTIONW__);
        fclose(gpFile);
        // m_log->Logger("Invalid Shader Type! Exitting Now ... 	:: %ls\n", __FUNCTIONW__);
		return(0);
	}

	// CREATING SHADER OBJECT
   hr = D3DCompile(shaderSourceCodeBuffer,
                    lstrlenA(shaderSourceCodeBuffer)+1,
                    glShaderType,
                    NULL,
                    D3D_COMPILE_STANDARD_FILE_INCLUDE,
                    "main",
                    "ps_5_0",
                    0,0,
                    pID3DBlob_ShaderCode,
                    &pID3DBlob_Error);
    if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		if (pID3DBlob_Error)
		{
			fprintf(gpFile, "D3DCompile() Failed for Pixel Shader :: < %s > ... 	:: %ls\n", (char *)pID3DBlob_Error->GetBufferPointer(), __FUNCTIONW__);
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
		}
		else
		{
			fprintf(gpFile, "D3DCompile() Failed for Pixel Shader :: < UNKNOWN > ... 	:: %ls\n", __FUNCTIONW__);
		}
		fclose(gpFile);
        return (FALSE);
	}
	else
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "D3DCompile() Succedded for Pixel Shader... 	:: %ls\n", __FUNCTIONW__);
		fclose(gpFile);
	}
	
   // create vertex shader
    hr = pID3D11Device->CreatePixelShader((*pID3DBlob_ShaderCode)->GetBufferPointer(),
                                            (*pID3DBlob_ShaderCode)->GetBufferSize(),
                                            NULL,
                                            &(*shader));
    if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "ID3D11Device::CreatePixelShader() Failed ... 	:: %ls\n",__FUNCTIONW__);
		fclose(gpFile);
        return (FALSE);
	}
	else
	{
		fopen_s(&gpFile, gszLogFilePathName, "a+"); // opening file in append mode
		fprintf(gpFile, "ID3D11Device::CreatePixelShader() Successfull ... 	:: %ls\n", __FUNCTIONW__);
		fclose(gpFile);
	}

    return (TRUE);
}




/*
BOOL DXShaders::LinkShaderProgramObject(GLuint shaderProgramObject)
{
	// variable declarations
	int status = 0;
	int infoLogLength = 0;
	char *log = NULL;

	// code
	glLinkProgram(shaderProgramObject);

	// Step 4
	glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		PrintLog("Shader Program Linking Failed.\n");
		glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char *)malloc(sizeof(char) * infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject, infoLogLength, &written, log);
				PrintLog("Shader Program Linking Log : %s\n", log);
				free(log);
				return(FALSE);
			}
		}
	}

	else
	{
		PrintLog("Shader Program Linking Succeeded.\n");
	}

	return(TRUE);
}

void DXShaders::UninitializeShaders(GLuint shaderProgramObject)
{
	// code
	if (shaderProgramObject)
	{
		glUseProgram(shaderProgramObject);

		GLsizei numAttachedShaders = 0;
		glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numAttachedShaders);

		GLuint *shaderObjects = NULL;
		shaderObjects = (GLuint *)malloc(sizeof(GLuint) * numAttachedShaders);

		glGetAttachedShaders(shaderProgramObject, numAttachedShaders, &numAttachedShaders, shaderObjects);

		for (GLsizei i = 0; i < numAttachedShaders; i++)
		{
			glDetachShader(shaderProgramObject, shaderObjects[i]);
			glDeleteShader(shaderObjects[i]);
			shaderObjects[i] = 0;
		}

		free(shaderObjects);
		shaderObjects = NULL;

		glUseProgram(0);

		glDeleteProgram(shaderProgramObject);
		shaderProgramObject = 0;
	}
}
*/

