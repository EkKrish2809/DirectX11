#include <cmath>
#include "Noise.h"

#define MAXB 0x100
#define N 0x1000
#define NP 12 // 2^N
#define NM 0xfff

#define s_curve(t) (t * t * (3. - 2. * t))
#define lerp(t, a, b) (a + t * (b - a))
#define setup(i, b0, b1, r0, r1) \
	t = vec[i] + N;              \
	b0 = ((int)t) & BM;          \
	b1 = (b0 + 1) & BM;          \
	r0 = t - (int)t;             \
	r1 = r0 - 1.;
#define at2(rx, ry) (rx * q[0] + ry * q[1])
#define at3(rx, ry, rz) (rx * q[0] + ry * q[1] + rz * q[2])

static void initNoise();

static int p[MAXB + MAXB + 2];
static double g3[MAXB + MAXB + 2][3];
static double g2[MAXB + MAXB + 2][2];
static double g1[MAXB + MAXB + 2];

int start;
int B;
int BM;

int Noise3DTexSize = 64;
BYTE *Noise3DTexPtr;

void Noise::CreateNoise3D(ID3D11ShaderResourceView **srv)
{
	make3DNoiseTexture();
	init3DNoiseTexture(srv);
}

void Noise::SetNoiseFrequency(int frequency)
{
	start = 1;
	B = frequency;
	BM = B - 1;
}

double Noise::noise1(double arg)
{
	int bx0, bx1;
	double rx0, rx1, sx, t, u, v, vec[1];

	vec[0] = arg;
	if (start)
	{
		start = 0;
		initNoise();
	}

	setup(0, bx0, bx1, rx0, rx1);

	sx = s_curve(rx0);
	u = rx0 * g1[p[bx0]];
	v = rx1 * g1[p[bx1]];

	return (lerp(sx, u, v));
}

double Noise::noise2(double vec[2])
{
	int bx0, bx1, by0, by1, b00, b10, b01, b11;
	double rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
	int i, j;

	if (start)
	{
		start = 0;
		initNoise();
	}

	setup(0, bx0, bx1, rx0, rx1);
	setup(1, by0, by1, ry0, ry1);

	i = p[bx0];
	j = p[bx1];

	b00 = p[i + by0];
	b10 = p[j + by0];
	b01 = p[i + by1];
	b11 = p[j + by1];

	sx = s_curve(rx0);
	sy = s_curve(ry0);

	q = g2[b00];
	u = at2(rx0, ry0);
	q = g2[b10];
	v = at2(rx1, ry0);
	a = lerp(sx, u, v);

	q = g2[b01];
	u = at2(rx0, ry1);
	q = g2[b11];
	v = at2(rx1, ry1);
	b = lerp(sx, u, v);

	return lerp(sy, a, b);
}

double Noise::noise3(double vec[3])
{
	int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
	double rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
	int i, j;

	if (start)
	{
		start = 0;
		initNoise();
	}

	setup(0, bx0, bx1, rx0, rx1);
	setup(1, by0, by1, ry0, ry1);
	setup(2, bz0, bz1, rz0, rz1);

	i = p[bx0];
	j = p[bx1];

	b00 = p[i + by0];
	b10 = p[j + by0];
	b01 = p[i + by1];
	b11 = p[j + by1];

	t = s_curve(rx0);
	sy = s_curve(ry0);
	sz = s_curve(rz0);

	q = g3[b00 + bz0];
	u = at3(rx0, ry0, rz0);
	q = g3[b10 + bz0];
	v = at3(rx1, ry0, rz0);
	a = lerp(t, u, v);

	q = g3[b01 + bz0];
	u = at3(rx0, ry1, rz0);
	q = g3[b11 + bz0];
	v = at3(rx1, ry1, rz0);
	b = lerp(t, u, v);

	c = lerp(sy, a, b);

	q = g3[b00 + bz1];
	u = at3(rx0, ry0, rz1);
	q = g3[b10 + bz1];
	v = at3(rx1, ry0, rz1);
	a = lerp(t, u, v);

	q = g3[b01 + bz1];
	u = at3(rx0, ry1, rz1);
	q = g3[b11 + bz1];
	v = at3(rx1, ry1, rz1);
	b = lerp(t, u, v);

	d = lerp(sy, a, b);

	return lerp(sz, c, d);
}

void Noise::normalize2(double v[2])
{
	double s;

	s = sqrt(v[0] * v[0] + v[1] * v[1]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
}

void Noise::normalize3(double v[3])
{
	double s;

	s = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
	v[2] = v[2] / s;
}

void Noise::initNoise()
{
	int i, j, k;

	srand(30757);
	for (i = 0; i < B; i++)
	{
		p[i] = i;
		g1[i] = (double)((rand() % (B + B)) - B) / B;

		for (j = 0; j < 2; j++)
			g2[i][j] = (double)((rand() % (B + B)) - B) / B;
		normalize2(g2[i]);

		for (j = 0; j < 3; j++)
			g3[i][j] = (double)((rand() % (B + B)) - B) / B;
		normalize3(g3[i]);
	}

	while (--i)
	{
		k = p[i];
		p[i] = p[j = rand() % B];
		p[j] = k;
	}

	for (i = 0; i < B + 2; i++)
	{
		p[B + i] = p[i];
		g1[B + i] = g1[i];
		for (j = 0; j < 2; j++)
			g2[B + i][j] = g2[i][j];
		for (j = 0; j < 3; j++)
			g3[B + i][j] = g3[i][j];
	}
}

// My harmonic summing functions - PDB

//
// In what follows "alpha" is the weight when the sum is formed.
// Typically it is 2, As this approaches 1 the function is noisier.
// "beta" is the harmonic scaling/spacing, typically 2.
//

double Noise::PerlinNoise1D(double x, double alpha, double beta, int n)
{
	int i;
	double val, sum = 0;
	double p, scale = 1;

	p = x;
	for (i = 0; i < n; i++)
	{
		val = noise1(p);
		sum += val / scale;
		scale *= alpha;
		p *= beta;
	}
	return (sum);
}

double Noise::PerlinNoise2D(double x, double y, double alpha, double beta, int n)
{
	int i;
	double val, sum = 0;
	double p[2], scale = 1;

	p[0] = x;
	p[1] = y;
	for (i = 0; i < n; i++)
	{
		val = noise2(p);
		sum += val / scale;
		scale *= alpha;
		p[0] *= beta;
		p[1] *= beta;
	}
	return (sum);
}

double Noise::PerlinNoise3D(double x, double y, double z, double alpha, double beta, int n)
{
	int i;
	double val, sum = 0;
	double p[3], scale = 1;

	p[0] = x;
	p[1] = y;
	p[2] = z;
	for (i = 0; i < n; i++)
	{
		val = noise3(p);
		sum += val / scale;
		scale *= alpha;
		p[0] *= beta;
		p[1] *= beta;
		p[2] *= beta;
	}
	return (sum);
}

void Noise::make3DNoiseTexture()
{
	int f, i, j, k, inc;
	int startFrequency = 4;
	int numOctaves = 4;
	double ni[3];
	double inci, incj, inck;
	int frequency = startFrequency;
	BYTE *ptr;
	double amp = 0.5;

	Noise3DTexPtr = (BYTE *)malloc(Noise3DTexSize * Noise3DTexSize * Noise3DTexSize * 4);
	for (f = 0, inc = 0; f < numOctaves; ++f, frequency *= 2, ++inc, amp *= 0.5)
	{
		SetNoiseFrequency(frequency);
		ptr = Noise3DTexPtr;
		ni[0] = ni[1] = ni[2] = 0;

		inci = 1.0 / (Noise3DTexSize / frequency);
		for (i = 0; i < Noise3DTexSize; ++i, ni[0] += inci)
		{
			incj = 1.0 / (Noise3DTexSize / frequency);
			for (j = 0; j < Noise3DTexSize; ++j, ni[1] += incj)
			{
				inck = 1.0 / (Noise3DTexSize / frequency);
				for (k = 0; k < Noise3DTexSize; ++k, ni[2] += inck, ptr += 4)
					*(ptr + inc) = (BYTE)(((noise3(ni) + 1.0) * amp) * 128.0);
			}
		}
	}
}

// void init3DNoiseTexture(int texSize, GLuint *texure)
// {
// 	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
// 	glGenTextures(1, texure);

// 	glBindTexture(GL_TEXTURE_3D, *texure);

// 	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
// 	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
// 	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
// 	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// 	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// 	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, texSize, texSize, texSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, Noise3DTexPtr);

// 	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
// 	free(Noise3DTexPtr);

// 	glBindTexture(GL_TEXTURE_3D, 0);
// }

// Texture Loading Function
HRESULT Noise::init3DNoiseTexture(ID3D11ShaderResourceView **ppID3D11ShaderResourceView)
{
	// variable declarations
	HRESULT hr = S_OK;

	// code

	// initialize texture2D descriptor
	D3D11_TEXTURE3D_DESC d3d11Texture2DDescriptor;
	ZeroMemory((void *)&d3d11Texture2DDescriptor, sizeof(D3D11_TEXTURE3D_DESC));
	d3d11Texture2DDescriptor.Width = (UINT)64;
	d3d11Texture2DDescriptor.Depth = (UINT)64;
	d3d11Texture2DDescriptor.Height = (UINT)64;
	// d3d11Texture2DDescriptor.ArraySize = 1;
	d3d11Texture2DDescriptor.MipLevels = 1; // for Multi sampling enabled state
	// d3d11Texture2DDescriptor.SampleDesc.Count = 1;
	// d3d11Texture2DDescriptor.SampleDesc.Quality = 0;
	d3d11Texture2DDescriptor.Usage = D3D11_USAGE_DEFAULT;
	d3d11Texture2DDescriptor.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3d11Texture2DDescriptor.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	d3d11Texture2DDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // give Default right to CPU
	d3d11Texture2DDescriptor.MiscFlags = 0;							  // no other flags

	// D3D11_SUBRESOURCE_DATA d3d11SubResourceData;
	// ZeroMemory((void *)&d3d11SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	// d3d11SubResourceData.pSysMem = checkerboard; // 3rd para of glBufferData()

	// Create a 2D texture as local Depth Buffer(it is going to be used as Depth Buffer)
	ID3D11Texture3D *pID3D11Texture3D_CheckerBoard = NULL;
	// hr = gpID3D11Device->CreateTexture2D(&d3d11Texture2DDescriptor, &d3d11SubResourceData, &pID3D11Texture2D_DepthBuffer);
	hr = m_Device->CreateTexture3D(&d3d11Texture2DDescriptor, NULL, &pID3D11Texture3D_CheckerBoard);
	if (FAILED(hr))
	{
		fopen_s(&m_FilePtr, "Log.txt", "a+"); // opening file in append mode
		fprintf(m_FilePtr, "gpID3D11Device::CreateTexture2D() Failed For Noise3D ...\n");
		fclose(m_FilePtr);
		return (hr);
	}


	m_DeviceContext->UpdateSubresource(pID3D11Texture3D_CheckerBoard,
											 0,
											 NULL,
											 Noise3DTexPtr,
											 64 * 4,	// for Bounding box of 3rd para (pitch of the bounding box)
											 256 * 64); // depth of Bounding box

	D3D11_SHADER_RESOURCE_VIEW_DESC d3d11ShaderResourceViewDesc;
	ZeroMemory((void *)&d3d11ShaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	d3d11ShaderResourceViewDesc.Format = d3d11Texture2DDescriptor.Format;
	d3d11ShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	// d3d11ShaderResourceViewDesc.Texture2D.MipLevels = 1;
	d3d11ShaderResourceViewDesc.Texture3D.MipLevels = 1;

	hr = m_Device->CreateShaderResourceView(pID3D11Texture3D_CheckerBoard,
												  &d3d11ShaderResourceViewDesc,
												  ppID3D11ShaderResourceView);
	if (FAILED(hr))
	{
		fopen_s(&m_FilePtr, "Log.txt", "a+"); // opening file in append mode
		fprintf(m_FilePtr, "gpID3D11Device::CreateShaderResourceView() Failed For Noise3D ...\n");
		fclose(m_FilePtr);
		return (hr);
	}
	else
	{
		fopen_s(&m_FilePtr, "Log.txt", "a+"); // opening file in append mode
		fprintf(m_FilePtr, "gpID3D11Device::CreateShaderResourceView() Success For Noise3D ...\n");
		fclose(m_FilePtr);
	}

	if (pID3D11Texture3D_CheckerBoard)
	{
		pID3D11Texture3D_CheckerBoard->Release();
		pID3D11Texture3D_CheckerBoard = NULL;
	}
	return (hr);
}
