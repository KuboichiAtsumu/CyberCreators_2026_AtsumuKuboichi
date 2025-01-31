//===========================================================================================================
// 定数バッファ
//===========================================================================================================
float4x4 WorldViewProj : WORLDVIEWPROJECTION;//ワールドビュー射影行列
float4x4 World : WORLD;//ワールド行列
float3 LightDirection : DIRECTION;//ライトの方向
float OutlineThickness_X;//アウトラインのX軸の厚さ
float OutlineThickness_Y;//アウトラインのX軸の厚さ
float OutlineThickness_Z;//アウトラインのX軸の厚さ
float Scale_X;//X軸拡大率
float Scale_Y;//Y軸拡大率
float Scale_Z;//Z軸拡大率

//===========================================================================================================
// 入力構造体
//===========================================================================================================
struct VS_INPUT
{
    float4 Position : POSITION;//頂点座標
    float3 Normal : NORMAL;//法線ベクトル
};

//===========================================================================================================
// 頂点シェーダーの出力
//===========================================================================================================
struct VS_OUTPUT
{
    float4 Position : POSITION;//変換後の頂点座標
    float3 WorldNormal : TEXCOORD0;//ワールド空間の法線
};

//===========================================================================================================
// トゥーンシェーディングの頂点シェーダー
//===========================================================================================================
VS_OUTPUT VS_Toon(VS_INPUT input)
{
    VS_OUTPUT output;

    //ワールド空間での法線を計算
    float3 worldNormal = mul(float4(input.Normal, 0.0), World).xyz;
    output.WorldNormal = normalize(worldNormal);

    //頂点座標を変換
    output.Position = mul(input.Position, WorldViewProj);

    return output;
}

//===========================================================================================================
// トゥーンシェーディングのピクセルシェーダー
//===========================================================================================================
float4 PS_Toon(VS_OUTPUT input) : COLOR
{
    float3 lightDir = normalize(LightDirection);
    float intensity = saturate(dot(input.WorldNormal, lightDir));//ライト強度

    //トゥーンシェーディングの段階化
    if (intensity > 0.8f)
    {
        intensity = 0.9f;
    }
    else if (intensity > 0.5f)
    {
        intensity = 0.7f;
    }
    else if (intensity > 0.2f)
    {
        intensity = 0.5f;
    }
    else
    {
        intensity = 0.3f;
    }

    //基本的なトゥーンカラー
    return float4(0.0f, intensity, 0.8f, 1.0f);
}

//===========================================================================================================
// アウトラインの頂点シェーダー
//===========================================================================================================
VS_OUTPUT VS_Outline(VS_INPUT input)
{
    VS_OUTPUT output;

    //ワールド空間での法線を計算
    float3 worldNormal = normalize(mul(input.Normal, (float3x3)World));
    output.WorldNormal = normalize(worldNormal);

    input.Position.x = input.Position.x / Scale_X;
    input.Position.y = input.Position.y / Scale_Y;
    input.Position.z = input.Position.z / Scale_Z;

    //頂点を少し押し出す（アウトラインの厚さを調整）
    float3 offset;
    offset.x = normalize(input.Position.x);
    offset.y = normalize(input.Position.y);
    offset.z = normalize(input.Position.z);
    float4 worldPos = input.Position + float4(offset, 0.0f);

    //頂点座標を変換
    output.Position = mul(worldPos, WorldViewProj);

    return output;
}

//===========================================================================================================
// アウトラインのピクセルシェーダー
//===========================================================================================================
float4 PS_Outline(VS_OUTPUT input) : COLOR
{
    return float4(0.8f, 0.8f, 0.8f, 1.0f);//白のアウトライン
}

//===========================================================================================================
// 技法定義
//===========================================================================================================
technique ToonShadingWithOutline
{
    pass P0
    {
        CullMode = CCW;//通常の面を描画

        //トゥーンシェーディング
        VertexShader = compile vs_2_0 VS_Toon();
        PixelShader = compile ps_2_0 PS_Toon();
    }
    pass P1
    {
        CullMode = CW;//裏面を描画（アウトライン用）

        //アウトライン描画
        VertexShader = compile vs_2_0 VS_Outline();
        PixelShader = compile ps_2_0 PS_Outline();
    }
}
