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
#include "object.h"

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

	//座標
	void SetPos(const D3DXVECTOR2& pos) { m_pos = pos; }//設定
	D3DXVECTOR2& GetPos() { return m_pos; }//取得

	//サイズ
	void SetSize(const D3DXVECTOR2& size) { m_size = size; }//設定
	D3DXVECTOR2& GetSize() { return m_size; }//取得

	//移動量
	void SetMove(const D3DXVECTOR2& move) { m_move = move; }//設定
	D3DXVECTOR2& GetMove() { return m_move; }//取得

	//角度
	void SetRot(const D3DXVECTOR3& rot) { m_rot = rot; }//設定
	D3DXVECTOR3& GetRot() { return m_rot; }//取得

	//カラー
	void SetColor(const D3DXCOLOR& col) { m_col = col; }//設定
	D3DXCOLOR& GetColor() { return m_col; }//取得

	//横のテクスチャ分割数
	void SetDivisionTex_X(int nDivisionTex) { m_nDivisionWidth = nDivisionTex; }//設定

	//横のアニメーションパターン
	void SetPatternAnim_X(int nPatternAnim) { m_nPatternAnimWidth = nPatternAnim; }//設定

private:
	//======================================
	// 変数
	//======================================
	D3DXVECTOR2 m_pos;//座標
	D3DXVECTOR2 m_size;//サイズ
	D3DXVECTOR2 m_move;//移動量
	D3DXVECTOR3 m_rot;//角度
	D3DXCOLOR m_col;//カラー
	int m_nDivisionWidth;//横のテクスチャ分割数
	int m_nDivisionHeight;//横のテクスチャ分割数
	int m_nDelayAnim;//アニメーション遅延
	int m_nPatternAnimWidth;//横のアニメーションパターン
	int m_nPatternAnimHeight;//縦のアニメーションパターン
	int m_nCounterAnim;//アニメーションカウンタ
	float m_fAngle;//対角線の角度
	float m_fLength;//対角線の長さ
};

#endif