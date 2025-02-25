//===========================================================================================================
// �萔�o�b�t�@
//===========================================================================================================
float4x4 WorldViewProj : WORLDVIEWPROJECTION;//���[���h�r���[�ˉe�s��
float4x4 World : WORLD;//���[���h�s��
float3 LightDirection : DIRECTION;//���C�g�̕����x�N�g��
float4 SurfaceColor = { 1.0f, 1.0f, 1.0f, 1.0f };//�ʂ̃J���[
float4 OutLineColor = { 1.0f, 1.0f, 1.0f, 1.0f };//�A�E�g���C���̃J���[
float OutlineThickness_X;//�A�E�g���C����X���̌���
float OutlineThickness_Y;//�A�E�g���C����Y���̌���
float OutlineThickness_Z;//�A�E�g���C����Z���̌���
float Scale_X;//X���g�嗦
float Scale_Y;//Y���g�嗦
float Scale_Z;//Z���g�嗦

//===========================================================================================================
// ���͍\����
//===========================================================================================================
struct VS_INPUT
{
    float4 Position : POSITION;//���_���W
    float3 Normal : NORMAL;//�@���x�N�g��
};

//===========================================================================================================
// ���_�V�F�[�_�[�̏o��
//===========================================================================================================
struct VS_OUTPUT
{
    float4 Position : POSITION;//�ϊ���̒��_���W
    float3 WorldNormal : TEXCOORD0;//���[���h��Ԃ̖@��
};

//===========================================================================================================
// �g�D�[���V�F�[�f�B���O�̒��_�V�F�[�_�[
//===========================================================================================================
VS_OUTPUT VS_Toon(VS_INPUT input)
{
    VS_OUTPUT output;

    //���[���h��Ԃł̖@�����v�Z
    float3 worldNormal = normalize(mul(input.Normal, (float3x3)World));
    output.WorldNormal = normalize(worldNormal);

    //���f���̊g�嗦��ݒ�
    input.Position.x = input.Position.x * Scale_X;
    input.Position.y = input.Position.y * Scale_Y;
    input.Position.z = input.Position.z * Scale_Z;

    //���_���W��ϊ�
    output.Position = mul(input.Position, WorldViewProj);

    return output;
}

//===========================================================================================================
// �g�D�[���V�F�[�f�B���O�̃s�N�Z���V�F�[�_�[
//===========================================================================================================
float4 PS_Toon(VS_OUTPUT input) : COLOR
{
    //�ʂ̃J���[
    return SurfaceColor;
}

//===========================================================================================================
// �A�E�g���C���̒��_�V�F�[�_�[
//===========================================================================================================
VS_OUTPUT VS_Outline(VS_INPUT input)
{
    VS_OUTPUT output;

    //���[���h��Ԃł̖@�����v�Z
    float3 worldNormal = normalize(mul(input.Normal, (float3x3)World));
    output.WorldNormal = normalize(worldNormal);

    input.Position.x = input.Position.x * Scale_X;
    input.Position.y = input.Position.y * Scale_Y;
    input.Position.z = input.Position.z * Scale_Z;

    //���_�����������o���i�A�E�g���C���̌����𒲐��j
    float3 offset;
    offset.x = normalize(input.Position.x) * OutlineThickness_X;
    offset.y = normalize(input.Position.y) * OutlineThickness_Y;
    offset.z = normalize(input.Position.z) * OutlineThickness_Z;
    float4 worldPos = input.Position + float4(offset, 0.0f);

    //���_���W��ϊ�
    output.Position = mul(worldPos, WorldViewProj);

    return output;
}

//===========================================================================================================
// �A�E�g���C���̃s�N�Z���V�F�[�_�[
//===========================================================================================================
float4 PS_Outline(VS_OUTPUT input) : COLOR
{
    //�A�E�g���C���̃J���[
    return OutLineColor;
}

//===========================================================================================================
// �Z�@��`
//===========================================================================================================
technique ToonShadingWithOutline
{
    pass P0
    {
        //�ʏ�̖ʂ�`��
        CullMode = CCW;

        //�g�D�[���V�F�[�f�B���O
        VertexShader = compile vs_2_0 VS_Toon();
        PixelShader = compile ps_2_0 PS_Toon();
    }
    pass P1
    {
        //���ʂ�`��i�A�E�g���C���p�j
        CullMode = CW;

        //�A�E�g���C���`��
        VertexShader = compile vs_2_0 VS_Outline();
        PixelShader = compile ps_2_0 PS_Outline();
    }
}
