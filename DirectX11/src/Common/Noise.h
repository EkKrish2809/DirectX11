#ifndef NOISE_H
#define NOISE_H

#include<stdio.h>
#include<stdlib.h>
// D3D related headers
#include <dxgi.h>
#include <D3D11.h>
#include <d3dcompiler.h>

// GLEW
#include <cmath>

// HRESULT init3DNoiseTexture(int texSize, BYTE *texPtr);
// void make3DNoiseTexture();

class Noise
{
    public:
        Noise(ID3D11Device &Device,
                    ID3D11DeviceContext &DeviceContext,
                    FILE &gpFile):
                    m_Device(&Device),
                    m_DeviceContext(&DeviceContext),
                    m_FilePtr(&gpFile)
        {

        }

        ~Noise() {}

        void CreateNoise3D(ID3D11ShaderResourceView **srv);
        void SetNoiseFrequency(int frequency);

        double noise1(double arg);
        double noise2(double vec[2]);
        double noise3(double vec[3]);

        void normalize2(double v[2]);
        void normalize3(double v[3]);
        void initNoise();

        // My harmonic summing functions - PDB

        //
        // In what follows "alpha" is the weight when the sum is formed.
        // Typically it is 2, As this approaches 1 the function is noisier.
        // "beta" is the harmonic scaling/spacing, typically 2.
        //

        double PerlinNoise1D(double x, double alpha, double beta, int n);
        double PerlinNoise2D(double x, double y, double alpha, double beta, int n);
        double PerlinNoise3D(double x, double y, double z, double alpha, double beta, int n);
        void make3DNoiseTexture();
        HRESULT init3DNoiseTexture(ID3D11ShaderResourceView **srv);

    private:
        ID3D11Device *m_Device;
        ID3D11DeviceContext *m_DeviceContext;
        FILE *m_FilePtr;
};

#endif
