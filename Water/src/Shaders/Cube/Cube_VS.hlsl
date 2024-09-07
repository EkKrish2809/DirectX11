cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};
struct vertex
{
    float4 position:SV_POSITION;
    float2 texcoord:TEXCOORD;
};
vertex main(float4 pos:POSITION, float2 texcoord:TEXCOORD)
{
    vertex output;
    // output.position = mul(worldViewProjectionMatrix, pos);
    output.position = mul(pos, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    output.texcoord = texcoord;
    return output;
}