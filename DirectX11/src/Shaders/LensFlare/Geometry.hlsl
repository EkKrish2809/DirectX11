
struct VSToGS
{
    float4 Pos : SV_POSITION;
    nointerpolation uint VertexId : VERTEXID;
};

struct GSToPS
{
    float4 Pos : SV_POSITION;
    float3 TexPos : TEXCOORD0; 
    nointerpolation uint Selection : TEXCOORD1;
    nointerpolation float4 Opacity : TEXCOORD2; 
};

// struct LightData
// {
//     float4 screenSpacePosition;// only this one is used here
// };

cbuffer LightCBuffer : register(b2)
{
    // LightData lightData;
    float4 screenSpacePosition;
}

Texture2D LensTx0 : register(t0);
Texture2D LensTx1 : register(t1);
Texture2D LensTx2 : register(t2);
Texture2D LensTx3 : register(t3);
Texture2D LensTx4 : register(t4);
Texture2D LensTx5 : register(t5);
Texture2D LensTx6 : register(t6);
Texture2D DepthTx : register(t7);

SamplerState PointClampSampler    : register(s4);

void AppendToStream(inout TriangleStream<GSToPS> triStream, GSToPS p1, uint selector, float2 posMod, float2 size)
{
    float2 pos = (screenSpacePosition.xy - 0.5) * float2(2, -2);
    float2 moddedPos = pos * posMod;
    float dis = distance(pos, moddedPos);

    p1.Pos.xy = moddedPos + float2(-size.x, -size.y);
    p1.TexPos.z = dis;
    p1.Selection = selector;
    p1.TexPos.xy = float2(0, 0);
    triStream.Append(p1);
	
    p1.Pos.xy = moddedPos + float2(-size.x, size.y);
    p1.TexPos.xy = float2(0, 1);
    triStream.Append(p1);

    p1.Pos.xy = moddedPos + float2(size.x, -size.y);
    p1.TexPos.xy = float2(1, 0);
    triStream.Append(p1);
	
    p1.Pos.xy = moddedPos + float2(size.x, size.y);
    p1.TexPos.xy = float2(1, 1);
    triStream.Append(p1);
}


[maxvertexcount(4)]
void main(point VSToGS p[1], inout TriangleStream<GSToPS> triStream)
{
    GSToPS output = (GSToPS) 0;
	float2 flareSize = float2(256, 256);
	[branch]
    switch (p[0].VertexId)
    {
        case 0:
            LensTx0.GetDimensions(flareSize.x, flareSize.y);
            break;
        case 1:
            LensTx1.GetDimensions(flareSize.x, flareSize.y);
            break;
        case 2:
            LensTx2.GetDimensions(flareSize.x, flareSize.y);
            break;
        case 3:
            LensTx3.GetDimensions(flareSize.x, flareSize.y);
            break;
        case 4:
            LensTx4.GetDimensions(flareSize.x, flareSize.y);
            break;
        case 5:
            LensTx5.GetDimensions(flareSize.x, flareSize.y);
            break;
        case 6:
            LensTx6.GetDimensions(flareSize.x, flareSize.y);
            break;
        default:
            break;
    };
    
    uint width, height, levels;
    DepthTx.GetDimensions(0, width, height, levels);
    
    float2 ScreenResolution = float2(width, height);
    flareSize /= ScreenResolution;
    float referenceDepth = saturate(screenSpacePosition.z);

	const float2 step = 1.0f / ScreenResolution;
    const float2 range = 10.5f * step;
    float samples = 0.0f;
    float depthAccumulated = 0.0f;
    for (float y = -range.y; y <= range.y; y += step.y)
    {
        for (float x = -range.x; x <= range.x; x += step.x)
        {
            samples += 1.0f;
            depthAccumulated += DepthTx.SampleLevel(PointClampSampler, screenSpacePosition.xy + float2(x, y), 0).r >= referenceDepth - 0.001 ? 1 : 0;
        }
    }
    depthAccumulated /= samples;
    output.Pos = float4(0, 0, 0, 1);
    output.Opacity = float4(depthAccumulated, 0, 0, 0);

	[branch]
    if (depthAccumulated > 0)
    {
      const float MODS[] = { 1, 0.55, 0.4, 0.1, -0.1, -0.3, -0.5 };
      AppendToStream(triStream, output, p[0].VertexId, MODS[p[0].VertexId], flareSize);
    }
}