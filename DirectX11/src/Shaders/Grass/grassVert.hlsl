cbuffer constantBuffer : register(b0)
{
    float u_time;
    float u_windStrength;
    float4x4 worldMatrix;
    float4x4 ViewMatrix;
    float4x4 ProjectionMatrix;
    float3 u_cameraPosition;
};

struct vertex
{
    float4 position:SV_POSITION;
    // float2 texcoord:TEXCOORD;
};
// vertex main(float3 pos:POSITION, float2 texcoord:TEXCOORD)
vertex main(float3 pos:POSITION)
{
    vertex output;
    // float4 position = mul(worldMatrix, float4(pos, 1.0));
    // position = mul(ViewMatrix, position);
    output.position = float4(pos, 1.0);
    // output.texcoord = texcoord;
    return output;
}
