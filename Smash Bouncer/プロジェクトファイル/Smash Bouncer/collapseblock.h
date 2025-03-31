#pragma once
//==============================================================================================================================================
//
// 崩れるブロックに関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _COLLAPSEBLOCK_H_ //このマクロ定義がされなかったら
#define _COLLAPSEBLOCK_H_ //2重インクルード防止のマクロ定義

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "BLOCK.h"//ブロックヘッダー

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CCollapseBlock : public CBlock
{
public:
	//======================================
	// 関数
	//======================================
	CCollapseBlock(int nPriority = 3);//コンストラクタ
	~CCollapseBlock() override;//デストラクタ
	HRESULT Init() override;//初期化処理
	void Uninit() override;//終了処理
	void Update() override;//更新処理
	void Draw() override;//描画処理
	void Collision(D3DXVECTOR3& pos, D3DXVECTOR3& oldpos, D3DXVECTOR3& size) override;//当たり判定
	virtual bool Reflection(D3DXVECTOR3& pos, D3DXVECTOR3& oldpos, D3DXVECTOR3& move, D3DXVECTOR3& rot, D3DXVECTOR3& size) override;//反射処理

private:
	//======================================
	// 関数
	//======================================
	void Hit();//ヒット処理

	//======================================
	// 変数
	//======================================

	static constexpr int MAX_LIFE = 3;//最大ライフ
	int m_nLife;//ライフ

	//体力ごとのテクスチャ
	static constexpr const char* COLLAPSE_TEXTURE[MAX_LIFE] = 
	{
		"data/TEXTURE/CollapseBlock000.png",
		"data/TEXTURE/CollapseBlock001.png",
		"data/TEXTURE/CollapseBlock002.png"
	};
};

#endif
