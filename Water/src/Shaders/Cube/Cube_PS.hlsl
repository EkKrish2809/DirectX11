Texture2D myTexture2D;
SamplerState mySamplerState;

cbuffer LightBuffer
{
	float4 lightDiffuseColor;
	float3 lightDirection;
	float colorTextureBrightness;
};

struct vertex
{
    float4 position:SV_POSITION;
    float2 texcoord:TEXCOORD;
};
float4 main(vertex input):SV_TARGET
{
    float4 color;
    color = myTexture2D.Sample(mySamplerState, input.texcoord);
    return float4(1.0f, 0.0f, 0.0f, 0.0f);
}
