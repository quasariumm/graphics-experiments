struct PSIn
{
    float4 Position : SV_Position;
};

float4 main(PSIn IN) : SV_Target
{
    return 1.0;
}