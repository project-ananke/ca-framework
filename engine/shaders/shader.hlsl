struct VSInput
{
    float3 position_local : POSITION;
    float4 color : COLOR;
};

struct VSOutput
{
    float4 position_clip : SV_POSITION;
    float4 color : COLOR;
};

cbuffer Constants : register(b0) 
{
    float4x4 ortho_proj; 
};

VSOutput vs_main(VSInput input)
{
    VSOutput output = (VSOutput)0;

    output.position_clip = mul(ortho_proj, float4(input.position_local, 1.0));
    output.color = float4(input.color);

    return output;
}

float4 ps_main(VSOutput input) : SV_TARGET
{
    return input.color;
}