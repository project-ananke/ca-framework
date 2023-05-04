struct VSInput
{
    float3 position_local : POS;
};

struct VSOutput
{
    float4 position_clip : SV_POSITION;
};

VSOutput vs_main(VSInput input)
{
    VSOutput output = (VSOutput)0;
    output.position_clip = float4(input.position_local, 1.0);
    return output;
}

float4 ps_main(VSOutput input) : SV_TARGET
{
    return float4(0.0, 0.0, 1.0, 1.0);
}