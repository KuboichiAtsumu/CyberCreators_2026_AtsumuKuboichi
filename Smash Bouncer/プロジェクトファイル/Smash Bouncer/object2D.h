#pragma once
//==============================================================================================================================================
//
// 2Dオブジェクトに関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _OBJECT2D_H_ //このマクロ定義がされなかったら
#define _OBJECT2D_H_ //2重インクルード防止のマクロ定義

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "object.h"//オブジェクト

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CObject2D : public CObject
{
public:
	//======================================
	// 関数
	//======================================
	CObject2D(int nPriority = 3);//コンストラクタ
	~CObject2D() override;//デストラクタ
	HRESULT Init() override;//初期化処理
	void Uninit() override;//終了処理
	void Release() override;//解放処理
	void Update() override;//更新処理
	void UpdateAnim();//アニメーション処理
	void Draw() override;//描画処理

	//パラメータ設定
	void SetPos(D3DXVECTOR2 pos) { m_pos = pos; }//位置
	void SetSize(D3DXVECTOR2 size) { m_size = size; }//サイズ
	void SetMove(D3DXVECTOR2 move) { m_move = move; }//移動量
	void SetColor(D3DXCOLOR col) { m_col = col; }//カラー

	//パラメータ情報取得
	D3DXVECTOR2 GetPos() { return m_pos; }//位置
	D3DXVECTOR2 GetSize() { return m_size; }//サイズ
	D3DXVECTOR2 GetMove() { return m_move; }//移動量
	D3DXCOLOR GetColor() { return m_col; }//カラー

private:
	//======================================
	// 変数
	//======================================
	D3DXVECTOR2 m_pos;//位置
	D3DXVECTOR2 m_size;//サイズ
	D3DXVECTOR2 m_move;//移動量
	D3DXVECTOR3 m_rot;//向き
	D3DXCOLOR m_col;//カラー
	int m_nDivisionTex;//テクスチャ分割数
	int m_nDelayAnim;//アニメーション遅延
	int m_nPatternAnim;//アニメーションパターン
	int m_nCounterAnim;//アニメーションカウンタ
	float m_fAngle;//対角線の角度
	float m_fLength;//対角線の長さ
};

#endif