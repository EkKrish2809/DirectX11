cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer ReflectionBuffer
{
    float moveFactorOffset;
    matrix reflectionMatrix;
    float4 ld;
    float4 kd;
    float4 lightPosition;
    float4 cameraPosition;
};

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float4 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float4 reflectionPosition : TEXCOORD1;
    float3 toCameraVector : CAMVECTOR;
	float3 lightDirection : LIGHTDIR;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    matrix reflectProjectWorld;


    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    // output.position = mul(input.position, worldMatrix);
    // output.position = mul(output.position, viewMatrix);
    // output.position = mul(output.position, projectionMatrix);
    output.position = mul(worldMatrix, input.position);
    output.position = mul(viewMatrix, output.position);
    output.position = mul(projectionMatrix, output.position);
    
    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex * 5;

     // Create the reflection projection world matrix.
    // reflectProjectWorld = mul(reflectionMatrix, projectionMatrix);
    // reflectProjectWorld = mul(worldMatrix, reflectProjectWorld);
    reflectProjectWorld = mul(projectionMatrix, reflectionMatrix);
    reflectProjectWorld = mul(reflectProjectWorld, worldMatrix);

    // Calculate the input position against the reflectProjectWorld matrix.
    // output.reflectionPosition = mul(input.position, reflectProjectWorld);
    output.reflectionPosition = mul(reflectProjectWorld, input.position);

    // Light
    float4 worldPosition = mul(worldMatrix, input.position);
    output.toCameraVector = cameraPosition - worldPosition.xyz;

    // calculate light direction
	output.lightDirection = worldPosition.xyz - lightPosition;

    return output;
}
