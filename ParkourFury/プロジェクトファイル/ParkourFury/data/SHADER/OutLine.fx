//===========================================================================================================
// 定数バッファ
//===========================================================================================================
float4x4 WorldViewProj : WORLDVIEWPROJECTION;//ワールドビュー射影行列
float4x4 World : WORLD;//ワールド行列
float3 LightDirection : DIRECTION;//ライトの方向ベクトル
float4 SurfaceColor = { 1.0f, 1.0f, 1.0f, 1.0f };//面のカラー
float4 OutLineColor = { 1.0f, 1.0f, 1.0f, 1.0f };//アウトラインのカラー
float OutlineThickness_X;//アウトラインのX軸の厚さ
float OutlineThickness_Y;//アウトラインのY軸の厚さ
float OutlineThickness_Z;//アウトラインのZ軸の厚さ
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
    float3 worldNormal = normalize(mul(input.Normal, (float3x3)World));
    output.WorldNormal = normalize(worldNormal);

    //モデルの拡大率を設定
    input.Position.x = input.Position.x * Scale_X;
    input.Position.y = input.Position.y * Scale_Y;
    input.Position.z = input.Position.z * Scale_Z;

    //頂点座標を変換
    output.Position = mul(input.Position, WorldViewProj);

    return output;
}

//===========================================================================================================
// トゥーンシェーディングのピクセルシェーダー
//===========================================================================================================
float4 PS_Toon(VS_OUTPUT input) : COLOR
{
    //面のカラー
    return SurfaceColor;
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

    input.Position.x = input.Position.x * Scale_X;
    input.Position.y = input.Position.y * Scale_Y;
    input.Position.z = input.Position.z * Scale_Z;

    //頂点を少し押し出す（アウトラインの厚さを調整）
    float3 offset;
    offset.x = normalize(input.Position.x) * OutlineThickness_X;
    offset.y = normalize(input.Position.y) * OutlineThickness_Y;
    offset.z = normalize(input.Position.z) * OutlineThickness_Z;
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
    //アウトラインのカラー
    return OutLineColor;
}

//===========================================================================================================
// 技法定義
//===========================================================================================================
technique ToonShadingWithOutline
{
    pass P0
    {
        //通常の面を描画
        CullMode = CCW;

        //トゥーンシェーディング
        VertexShader = compile vs_2_0 VS_Toon();
        PixelShader = compile ps_2_0 PS_Toon();
    }
    pass P1
    {
        //裏面を描画（アウトライン用）
        CullMode = CW;

        //アウトライン描画
        VertexShader = compile vs_2_0 VS_Outline();
        PixelShader = compile ps_2_0 PS_Outline();
    }
}
