struct VSInput
{
    float3 pos : POS;
    float3 col : COL;
};

struct VSOutput
{
    float4 pos_clip : SV_POSITION;
    float4 col : COL;
};

VSOutput vs_main(VSInput input)
{
    VSOutput output = (VSOutput)0;
    output.pos_clip = float4(input.pos, 1.0);
    output.col = float4(input.col, 1.0);
    
    return output;
}

float4 ps_main(VSOutput output) : SV_TARGET
{
    return output.col;
}
