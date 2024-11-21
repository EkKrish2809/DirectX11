struct GSToPS
{
    float4 Pos : SV_POSITION;
    float3 TexPos : TEXCOORD0; 
    nointerpolation uint Selection : TEXCOORD1;
    nointerpolation float4 Opacity : TEXCOORD2; 
};

Texture2D LensTx0 : register(t0);
Texture2D LensTx1 : register(t1);
Texture2D LensTx2 : register(t2);
Texture2D LensTx3 : register(t3);
Texture2D LensTx4 : register(t4);
Texture2D LensTx5 : register(t5);
Texture2D LensTx6 : register(t6);
Texture2D DepthTx : register(t7);

SamplerState PointClampSampler    : register(s4);

float4 main(GSToPS input) : SV_TARGET
{
    float4 color = 0;
	
	[branch]
    switch (input.Selection)
    {
        case 0:
            color = float4(0.0f, 0.0f, 0.0f, 1.0f);
            break;
        case 1:
            color = LensTx1.SampleLevel(PointClampSampler, input.TexPos.xy, 0);
            break;
        case 2:
            color = LensTx2.SampleLevel(PointClampSampler, input.TexPos.xy, 0);
            break;
        case 3:
            color = LensTx3.SampleLevel(PointClampSampler, input.TexPos.xy, 0);
            break;
        case 4:
            color = LensTx4.SampleLevel(PointClampSampler, input.TexPos.xy, 0);
            break;
        case 5:
            color = LensTx5.SampleLevel(PointClampSampler, input.TexPos.xy, 0);
            break;
        case 6:
            color = LensTx6.SampleLevel(PointClampSampler, input.TexPos.xy, 0);
            break;
        default:
            break;
    };

    color *= 1.1 - saturate(input.TexPos.z);
    color *= input.Opacity.x;

    return color;
}
