Texture2D myTexture2D;
SamplerState mySamplerState;

cbuffer constantBuffer_Cube : register(b0)
{
    matrix worldViewProjectionMatrix;
    float3 objectColor;
}

struct vertex
{
    float4 position:SV_POSITION;
    float2 texcoord:TEXCOORD;
};
float4 main(vertex input):SV_TARGET
{
    float4 color;
    color = myTexture2D.Sample(mySamplerState, input.texcoord);
    return float4(objectColor, 1.0f);
}
