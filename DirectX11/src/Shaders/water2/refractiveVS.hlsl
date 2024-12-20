///////////////////////////////////////////////////////////////////////////////////////////////////
//  Proj : GPU GEMS 2 DEMOS
//  File : refractiveVP.cg
//  Desc : generic refraction vertex program
///////////////////////////////////////////////////////////////////////////////////////////////////

cbuffer RefractionCB : register(b0)
{
    float4x4 ModelViewProj;
    float4 vTranslation;
    float4 vCameraPos;
    float4 vParams;
};

// define inputs from application
struct VpIn
{
    float4 Pos       : POSITION;
    float2 TexCoord0 : TEXCOORD0;      
    float3 Normal    : TEXCOORD1;  
    float3 Tangent   : TEXCOORD2; 
};

// define outputs from vertex shader
struct VpOut
{
    float4 HPos  : SV_POSITION;    
    float2 BaseUV  : TEXCOORD0;
    float4 ScreenPos  : TEXCOORD1;
    float3 Eye   : TEXCOORD2;     
};

float3x3 GetTangentSpaceBasis(float3 T, float3 N)
{
   float3x3 objToTangentSpace;
   
   objToTangentSpace[0]=T;           // tangent
   objToTangentSpace[1]=cross(T, N); // binormal
   objToTangentSpace[2]=N;           // normal  
   
   return objToTangentSpace;
}

// vertex program
// VpOut main(VpIn IN, uniform float4x4 ModelViewProj, uniform float4 vTranslation, uniform float4 vCameraPos, uniform float4 vParams)
VpOut main(VpIn IN) 
{
    VpOut OUT;
    
    float4 vPos = IN.Pos+vTranslation;
    OUT.HPos = mul(ModelViewProj, vPos);
    OUT.BaseUV.xy = IN.TexCoord0.xy;
          
    // perspective corrected projection      
    float4 vHPos = mul(ModelViewProj, vPos);         	
    		  
	vHPos.y = -vHPos.y;
    OUT.ScreenPos.xy = (vHPos.xy + vHPos.w)*0.5;    
    OUT.ScreenPos.zw =  float2(1, vHPos.w);      
        
    // get tangent space basis    
    float3x3 objToTangentSpace = GetTangentSpaceBasis(IN.Tangent.xyz, IN.Normal.xyz);
            
    float3 EyeVec=vCameraPos.xyz-vPos;
    float3 LightVec=vCameraPos.xyz-vPos;
    
    OUT.Eye.xyz = mul(objToTangentSpace, EyeVec);
                                              
    return OUT;
}