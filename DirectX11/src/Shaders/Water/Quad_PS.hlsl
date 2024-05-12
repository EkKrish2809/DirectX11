Texture2D myTexture2D;
SamplerState mySamplerState;

struct vertex
{
    float4 position:SV_POSITION;
    float2 texcoord:TEXCOORD;
};

float4 main(vertex input):SV_TARGET
{
    float4 color;
    color = myTexture2D.Sample(mySamplerState, input.texcoord);
    // color = float4(1.0, 0.0, 0.0, 0.0);
    return color;
}