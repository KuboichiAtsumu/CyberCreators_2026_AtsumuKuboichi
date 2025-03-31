#pragma once
//==============================================================================================================================================
//
// 3Dオブジェクトに関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _OBJECT3D_H_ //このマクロ定義がされなかったら
#define _OBJECT3D_H_ //2重インクルード防止のマクロ定義

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "object.h"//オブジェクト
#include "game.h"//ゲームシーン

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CObject3D : public CObject
{
public:
	//======================================
	// 関数
	//======================================
	CObject3D(int nPriority = 3);//コンストラクタ
	~CObject3D() override;//デストラクタ
	HRESULT Init() override;//初期化処理
	void Uninit() override;//終了処理
	void Release() override;//解放処理
	void Update() override;//更新処理
	void UpdateAnim();//テクスチャアニメーション処理
	void Draw() override;//描画処理
	void DecreaseLife() { m_nLife--; }//ライフ減少処理

	//パラメータの設定
	void SetPos(D3DXVECTOR3 pos) { m_pos = pos; }//位置
	void SetOldPos(D3DXVECTOR3 oldpos) { m_oldpos = oldpos; }//過去の位置
	void SetMove(D3DXVECTOR3 move) { m_move = move; }//移動量
	void SetRot(D3DXVECTOR3 rot) { m_rot = rot; }//向き
	void SetSize(D3DXVECTOR3 size) { m_size = size; }//サイズ
	void SetColor(D3DXCOLOR col) { m_col = col; }//カラー
	void SetLife(int nLife) { m_nLife = nLife; }//ライフ
	void SetDelayAnim(int nDelayAnim) { m_nDelayAnim = nDelayAnim; }//アニメーション遅延
	void SetDivisionTex(int nWidth, int nHeight)//テクスチャ分割数
	{
		m_nDivisionWidth = nWidth;//横の分割数
		m_nDivisionHeight = nHeight;//縦の分割数
	}
	void SetPostStageID();//自分がいるステージのID

	//パラメータ取得
	D3DXVECTOR3 GetPos() { return m_pos; }//位置
	D3DXVECTOR3 GetOldPos() { return m_oldpos; }//過去の位置
	D3DXVECTOR3 GetMove() { return m_move; }//移動量
	D3DXVECTOR3 GetRot() { return m_rot; }//向き
	D3DXVECTOR3 GetSize() { return m_size; }//サイズ
	D3DXMATRIX GetMatrix() { return m_mtxWorld; }//ワールドマトリックス
	D3DXCOLOR GetColor() { return m_col; }//カラー
	int GetLife() { return m_nLife; }//ライフ
	int GetPostStageID() { return m_nPostStageID; }//自分がいるステージのID
	int GetEachStageID() { return m_nEachStageID; }//各ステージごとの各オブジェクトのID
	int GetDivisionWidth() { return m_nDivisionWidth; }//横のテクスチャ分割数
	int GetDivisionHeight() { return m_nDivisionHeight; }//縦のテクスチャ分割数
	int GetPatternAnimWidth() { return m_nPatternAnimWidth; }//横のアニメーションパターン
	int GetPatternAnimHeight() { return m_nPatternAnimHeight; }//縦のアニメーションパターン
	static int GetAllEachStageID(int nStage, CObject::TYPE type) { return m_nAllEachStageID[nStage][type]; }//各ステージの各オブジェクトタイプ合計数

private:
	//======================================
	// 変数
	//======================================
	D3DXVECTOR3 m_pos;//位置
	D3DXVECTOR3 m_oldpos;//過去の位置
	D3DXVECTOR3 m_move;//移動量
	D3DXVECTOR3 m_rot;//向き
	D3DXVECTOR3 m_size;//サイズ
	D3DXCOLOR m_col;//カラー
	D3DXMATRIX m_mtxWorld;//ワールドマトリックス
	static int m_nAllEachStageID[CGame::MAX_STAGE][MAX_OBJECT_TYPE];//各ステージの各オブジェクトタイプごとの合計数
	int m_nLife;//寿命&体力
	int m_nPostStageID;//自分がいるステージのID
	int m_nEachStageID;//各ステージごとの各オブジェクトID
	int m_nDivisionWidth;//横のテクスチャ分割数
	int m_nDivisionHeight;//横のテクスチャ分割数
	int m_nDelayAnim;//アニメーション遅延
	int m_nPatternAnimWidth;//横のアニメーションパターン
	int m_nPatternAnimHeight;//縦のアニメーションパターン
	int m_nCounterAnim;//アニメーションカウンタ
};

#endif