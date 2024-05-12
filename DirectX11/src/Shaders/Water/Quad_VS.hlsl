cbuffer ConstantBuffer
{
    // float4x4 world;
    // float4x4 View;
    // float4x4 Projection;
    float4x4 WorldViewProjection;
}

struct vertex
{
    float4 position:SV_POSITION;
    float2 texcoord:TEXCOORD;
};

vertex main(float4 pos:POSITION, float2 texcoord:TEXCOORD)
{
    vertex output;
    // output.position = mul(pos, world);
    // output.position = mul(output.position, View);
    // output.position = mul(output.position, Projection);
    output.position = mul(WorldViewProjection, pos);
    output.texcoord = texcoord;
    return output;
}
