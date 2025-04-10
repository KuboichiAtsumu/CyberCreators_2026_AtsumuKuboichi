#pragma once
//==============================================================================================================================================
//
// 鍵付きブロックに関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _KEYBLOCK_H_ //このマクロ定義がされなかったら
#define _KEYBLOCK_H_ //2重インクルード防止のマクロ定義

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "BLOCK.h"//ブロックヘッダー

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CKeyBlock : public CBlock
{
public:
	//======================================
	// 関数
	//======================================
	CKeyBlock(int nPriority = 3);//コンストラクタ
	~CKeyBlock() override;//デストラクタ
	HRESULT Init() override;//初期化処理
	void Uninit() override;//終了処理
	void Update() override;//更新処理
	void Draw() override;//描画処理
	void Collision(D3DXVECTOR3& pos, D3DXVECTOR3& oldpos, D3DXVECTOR3& size) override;//当たり判定
};

#endif
