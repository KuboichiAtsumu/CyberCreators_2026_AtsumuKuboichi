//==============================================================================================================================================
//
// ビルボードエフェクトに関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _BILLBOARDEFFECT_H_ //このマクロ定義がされなかったら
#define _BILLBOARDEFFECT_H_ //2重インクルード防止のマクロ定義

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "billboard.h"

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CBillboardEffect : public CBillboard
{
public:
	//======================================
	// 列挙型定義
	//======================================
	enum class TYPE
	{
		NONE = 0,
		PARTICLE,//パーティクル
		TARGET,//ターゲット
		SMOKE,//煙
		MINIGAME_CLEAR,//ミニゲームクリア
		MINIGAME_FAILED,//ミニゲーム失敗
		MAX
	};

	//======================================
	// 関数
	//======================================
	CBillboardEffect(int nPriority = static_cast<int>(Category::EFFECT));//コンストラクタ
	~CBillboardEffect() override;//デストラクタ
	HRESULT Init() override;//初期化処理
	void Uninit() override;//終了処理
	void Release() override;//解放処理
	void Update() override;//更新処理
	void Draw() override;//描画処理
	static CBillboardEffect* Create(TYPE type, const D3DXVECTOR3& pos, const D3DXVECTOR3& size);//生成処理

	//タイプ
	void SetType(TYPE type) { m_type = type; };//設定

private:
	//======================================
	// 各タイプテクスチャファイルパス
	//======================================
	static constexpr const char* TEXTURE_FILE[static_cast<int>(TYPE::MAX)] =
	{
		"",//NONE
		"data/TEXTURE/EFFECT/Particle.jpg",//パーティクル
		"data/TEXTURE/EFFECT/Target.png",//ターゲット
		"data/TEXTURE/EFFECT/Smoke.png",//煙
		"data/TEXTURE/EFFECT/MiniGameClear.png",//ミニゲームクリア
		"data/TEXTURE/EFFECT/MiniGameFailed.png",//ミニゲーム失敗
	};

	//======================================
	// 変数
	//======================================
	TYPE m_type;//タイプ
};

#endif