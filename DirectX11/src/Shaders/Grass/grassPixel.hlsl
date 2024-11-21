Texture2D myTexture2D;
SamplerState grassTexture;

struct vertex
{
    float4 position:SV_POSITION;
    float2 texcoord:TEXCOORD;
};

struct geometry_output
{
    float4 position:SV_POSITION;
    float2 texCoord:TEXCOORD;
    float colorVariation:COLOR;
};

float4 main(vertex input, geometry_output input_g):SV_TARGET
{
    float4 color;
    color = myTexture2D.Sample(grassTexture, input_g.texCoord);
    if (color.x < 0.25)
    {
        discard;
    }

    color.xyz = lerp(color.xyz, 0.5 * color.xyz, input_g.colorVariation);

    return color;// + float4(1.0, 0.0, 0.0, 0.0);
}
