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
vertex main(float4 pos:POSITION, float2 texcoord:TEXCOORD)
{
    vertex output;
    output.position = mul(worldViewProjectionMatrix, pos);
    output.texcoord = texcoord;
    return output;
}