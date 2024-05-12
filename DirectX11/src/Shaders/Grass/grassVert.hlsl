cbuffer constantBuffer
{
    float4x4 worldMatrix;
    float4x4 ViewMatrix;
    float4x4 ProjectionMatrix;
    float3 u_cameraPosition;
    float u_time;
    float u_windStrength;
}
struct vertex
{
    float4 position:POSITION;
    // float2 texcoord:TEXCOORD;
};
// vertex main(float4 pos:POSITION, float2 texcoord:TEXCOORD)
vertex main(float4 pos:POSITION)
{
    vertex output;
    float4 position = mul(worldMatrix, pos);
    position = mul(ViewMatrix, position);
    output.position = position;
    // output.texcoord = texcoord;
    return output;
}
