Texture2D shadow_buffer_tex;
Texture2D tex_depth_refraction;
Texture2D tex_refraction;
Texture2D tex_reflection;
SamplerState mySamplerState;

cbuffer CBWater : register(b0)
{    
    float4x4 model;
    float4x4 view;
    float4x4 projection;
    float3 light_position;
    float4x4 shadow_matrix; //bias*P*V

    float3 camera_position;
    float3 camera_direction;
    float time;
    uint water_effect;
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


float get_wave_0(float x, float y){
   x = x+time/100;
   y = y+time/100;
   float wave = sin(x*64*3.1415)*sin(y*16*3.1415);
   wave = wave/2;
   wave = wave*0.01;
   wave += (sin((x+0.23-time/200)*64*3.1415)*sin((y+0.76+time/200)*128*3.1415))/2*0.001;
   wave += (sin((x+0.7664+time/50)*128*3.1415)*sin((y+0.2346-time/50)*128*3.1415))/2*0.0005;
   return wave;
}

//a*sin(d*(x,y)*w+t*phi)
float get_wave_1(float x, float y){
   float wave = 0.01*sin( dot(normalize(float2(1,0)), float2(x, y)) *128+time);
   wave += 0.01*sin( dot(normalize(float2(15,1)), float2(x, y)) *128+time);
   wave += 0.01*sin( dot(normalize(float2(10,-1)), float2(x, y)) *256+time*1.5);
   wave += 0.01*sin( dot(normalize(float2(3,1)), float2(x, y)) *256+time*1.5);
   wave += 0.02*sin( dot(normalize(float2(6,-1)), float2(x, y)) *64+time*0.8);
   wave += 0.015*sin( dot(normalize(float2(8,1)), float2(x, y)) *128+time*0.7);
   return wave/12;
}

float get_wave(float x, float y){
   float wave = 0;
      // wave = get_wave_0(x, y);
   if(water_effect == 0u){
   }
   else if(water_effect == 1u){
      wave = get_wave_1(x, y);
   }
   else if(water_effect == 2u){
      wave = 2*get_wave_0(x, y);
   }
   else if(water_effect == 3u){
      wave = 2*get_wave_1(x, y);
   }
   return wave;
}

float4 main(VS_out input) : SV_TARGET
{
   float4 color;

   float wave = get_wave(input.uv_frag.x, input.uv_frag.y);

   float3 pos_before_x = float3(input.uv_frag.x-0.01, get_wave(input.uv_frag.x-0.01, input.uv_frag.y), input.uv_frag.y);
   float3 pos_after_x = float3(input.uv_frag.x+0.01, get_wave(input.uv_frag.x+0.01, input.uv_frag.y), input.uv_frag.y);
   float3 pos_before_y = float3(input.uv_frag.x, get_wave(input.uv_frag.x, input.uv_frag.y-0.01), input.uv_frag.y-0.01);
   float3 pos_after_y = float3(input.uv_frag.x, get_wave(input.uv_frag.x, input.uv_frag.y+0.01), input.uv_frag.y+0.01);

   //get normal of wave, for lighting purpose
   float3 normal_wave = normalize(cross( pos_after_x-pos_before_x, pos_after_y-pos_before_y));

   float3 light_dir = normalize(light_position-input.frag_position);
   float diffuse_light = 0.0;

   diffuse_light = dot(normal_wave, light_dir);
   float light_dist = length(light_position-input.frag_position);
   diffuse_light /= 1+pow(light_dist, -0.5);

   //reflexion of light for specular light calculation, not the image reflexion
   float3 reflexion = 2*normal_wave*dot(normal_wave, light_dir)-light_dir;
   reflexion = normalize(reflexion);
   float3 view_dir = normalize(camera_position-input.frag_position);

   float spec_light = pow(max(dot(reflexion, view_dir), 0.0), 128);
   spec_light = clamp(spec_light, 0.0, 1.0);

   float lum = 0.8*diffuse_light+spec_light;

   // float4 screen_pos = projection*view*float4(input.frag_position, 1.0);
   float4 screen_pos = mul(view, float4(input.frag_position, 1.0));
   screen_pos = mul(projection, screen_pos);
   // projection*view*float4(input.frag_position, 1.0);
   float2 corr_screen_pos_refraction = screen_pos.xy*0.5/screen_pos.w+float2(0.5, 0.5);
   float2 corr_screen_pos_reflection = float2(corr_screen_pos_refraction.x, 1-corr_screen_pos_refraction.y); //must invert y

   //distort the reflection depending on the wave
   corr_screen_pos_refraction += float2(wave, wave);
   corr_screen_pos_reflection += float2(wave, wave);

   float3 colour_refraction = tex_refraction.Sample( mySamplerState, corr_screen_pos_refraction ).rgb;
   float3 colour_reflection = tex_reflection.Sample( mySamplerState, corr_screen_pos_reflection ).rgb;
   float3 depth_refraction = tex_depth_refraction.Sample( mySamplerState, corr_screen_pos_refraction ).rbg;

   float far = 1000;
   float near = 0.1;

   // float water_depth_lin = gl_FragCoord.z/gl_FragCoord.w;
   float water_depth_lin = input.position.z/input.position.w;

   float depth_refraction_lin = 2*near*far/(far+near-(2.0*depth_refraction.x-1.0)*(far-near));

   float3 direction_camera = normalize(camera_position-input.frag_position);
   float angle_view = abs(dot(direction_camera, input.frag_normal_transformed)); //take the camera normal, 1: perpendicular, 0: along the water line
   // clamp(angle_view, 0.0, 1.0);
   angle_view = pow(angle_view, 2); //higher pow will means, the area of view where we see the bottom of water is smaller

   float depth_water = depth_refraction_lin-water_depth_lin;
   depth_water = depth_water/8; //shorter distance for the depth (a bit too big for this depth)

   //depth should go from 0 to 1
   float3 colour_depth = (1.0-depth_water)*colour_refraction+depth_water*float3(0.5, 0.5, 0.7);

   color.a = 0.5;
   color.rgb = (1-angle_view)*colour_reflection + angle_view*colour_depth + 0.5*diffuse_light+spec_light;

   return color;// + float4(0.0, 1.0, 0.0, 0.0);
   // return float4(colour_depth, 0.5);
}

void main_colors_const(){
   // color = float4(red, green, blue, 0.0);
}
