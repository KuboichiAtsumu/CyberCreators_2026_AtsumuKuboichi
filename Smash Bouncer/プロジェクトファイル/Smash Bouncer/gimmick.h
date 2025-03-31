//==============================================================================================================================================
//
// ギミックに関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#pragma once
#ifndef _GIMMICK_H_
#define _GIMMICK_H_

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "objectX.h"//Xファイルオブジェクト

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CGimmick : public CObjectX
{
public:
	//======================================
	// 列挙型定義
	//======================================

	//種類
	typedef enum
	{
		MOVEFLOOR,//移動床
		WARPHOLE,//ワープホール
		SWITCHBASE,//スイッチベース
		SWITCH,//スイッチ
		MAX
	}GIMMICKTYPE;

	//======================================
	// 関数
	//======================================
	CGimmick(int nPriority);//コンストラクタ
	~CGimmick() override;//デストラクタ
	HRESULT Init() override;//初期化処理
	void Uninit() override;//終了処理
	void Update() override;//更新処理
	void Draw() override;//描画処理
	static CGimmick* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nType);//生成処理
	virtual bool Collision(D3DXVECTOR3& pos, D3DXVECTOR3& move, D3DXVECTOR3 size, D3DXVECTOR3& rot);//当たり判定
	GIMMICKTYPE GetGimmickType() { return m_GimmickType; }//ギミックタイプ情報取得
	CGimmick* GetPrevGimmick() { return m_pPrev; }//前のオブジェクト情報を取得
	CGimmick* GetNextGimmick() { return m_pNext; }//次のオブジェクト情報を取得
	void SetPrevGimmick(CGimmick* pGimmick) { m_pPrev = pGimmick; }//前のオブジェクト情報を設定
	void SetNextGimmick(CGimmick* pGimmick) { m_pNext = pGimmick; }//次のオブジェクト情報を設定

private:
	//======================================
	// 関数
	//======================================
	void SetGimmickType(GIMMICKTYPE type) { m_GimmickType = type; }//ギミックタイプ設定
	
	//======================================
	// 変数
	//======================================
	GIMMICKTYPE m_GimmickType;//種類
	CGimmick* m_pPrev;//前のオブジェクトのポインタ
	CGimmick* m_pNext;//次のオブジェクトのポインタ
};

#endif
