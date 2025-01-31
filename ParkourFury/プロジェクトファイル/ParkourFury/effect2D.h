//==============================================================================================================================================
//
// 2Dエフェクトに関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _EFFECT2D_H_ //このマクロ定義がされなかったら
#define _EFFECT2D_H_ //2重インクルード防止のマクロ定義

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "object2D.h"

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CEffect2D : public CObject2D
{
public:
	//======================================
	// 列挙型定義
	//======================================

	//タイプ
	enum class TYPE
	{
		NONE = 0,
		NICE,//NICE
		GREAT,//GREAT
		EXCELLENT,//EXCELLENT
		BAD,//BAD
		MAX
	};

	//======================================
	// 関数
	//======================================
	CEffect2D(int nPriority = static_cast<int>(Category::EFFECT_2D));//コンストラクタ
	~CEffect2D() override;//デストラクタ
	HRESULT Init() override;//初期化処理
	void Uninit() override;//終了処理
	void Update() override;//更新処理
	void Draw() override;//描画処理
	static CEffect2D* Create(TYPE type, const D3DXVECTOR2& pos, const D3DXVECTOR3& rot);//生成処理

	//自分自身のポインタ
	static CEffect2D* GetEffect2D() { return m_pEffect2D; }//取得

	//タイプ
	void SetEffect2DType(TYPE type) { m_Type = type; }//設定
	TYPE GetEffect2DType() { return m_Type; }//取得

	//目標のサイズ
	void SetTargetSize(const D3DXVECTOR2& TargetSize) { m_TargetSize = TargetSize; }//設定

private:
	//======================================
	// 関数
	//======================================
	void ColorChange();//カラー変更処理
	void ScaleDownSize();//サイズ縮小処理

	//======================================
	// 各タイプテクスチャファイルパス
	//======================================
	static constexpr const char* TEXTURE_FILE[static_cast<int>(TYPE::MAX)] =
	{
		"",//NONE
		"data/TEXTURE/EFFECT/Nice.png",//NICE
		"data/TEXTURE/EFFECT/Great.png",//GREAT
		"data/TEXTURE/EFFECT/Excellent.png",//EXCELLENT
		"data/TEXTURE/EFFECT/Bad.png",//BAD
	};

	//======================================
	// 定数
	//======================================
	const int SCALE_DOWN_FRAME = 10;//縮小するフレーム数
	const float COLOR_SPEED = 0.1f;//カラー変更スピード
	const float MAG_CREATE_SIZE = 4.0f;//生成時のサイズ倍率

	//======================================
	// 変数
	//======================================
	static CEffect2D* m_pEffect2D;//自分自身のポインタ
	D3DXVECTOR2 m_TargetSize;//目標のサイズ
	D3DXVECTOR2 m_ScaleDown;//縮小する値
	TYPE m_Type;//タイプ
	int m_nScaleDownFrame;//縮小フレーム
};

#endif
