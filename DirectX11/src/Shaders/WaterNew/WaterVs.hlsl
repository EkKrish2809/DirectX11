
// in vec3 position;
// in vec2 uv;
// in vec3 surface_normal;
cbuffer CBWater : register(b0)
{    
    float4x4        model;
    float4x4        view;
    float4x4        projection;
    float3          light_position;
    float4x4        shadow_matrix; //bias*P*V

    float3          camera_position;
    float3          camera_direction;
    float           time;
    uint            water_effect;
}

struct VS_out
{
    float4 position                 : SV_POSITION;
    float3 frag_position            : FRAGPOS;
    float3 frag_normal_transformed  : NORMAL;
    float2 uv_frag                  : TEXCOORD;
    float red                       : COLOR0;
    float green                     : COLOR1;
    float blue                      : COLOR2;
    float4 shadow_coord             : TEXCOORD1;
};



VS_out main(float3 position:POSITION, float2 uv:TEXCOORD, float3 surface_normal:NORMAL)
{
   VS_out output;
   output.position = mul(model, float4(position, 1.0));
   output.position = mul(view, output.position);
   output.position = mul(projection, output.position);

   float3x3 normalMat = (float3x3)(model);
   normalMat = transpose(1/(normalMat));

   float3 normal_transformed = float3(0.0, 0.0, 0.0);
   normal_transformed = normalize(mul(normalMat, surface_normal));

   output.frag_normal_transformed = normal_transformed;
   output.frag_position = (float3)(mul(model, float4(position, 1.0)));

   output.shadow_coord = mul(model, float4(position, 1.0));
   output.shadow_coord = mul(shadow_matrix, output.shadow_coord);

   output.uv_frag = uv;

   return output;
}
