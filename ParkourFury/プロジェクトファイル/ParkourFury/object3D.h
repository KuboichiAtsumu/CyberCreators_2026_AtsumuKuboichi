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
#include "object.h"

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

	//座標
	void SetPos(const D3DXVECTOR3& pos) { m_pos = pos; }//設定
	D3DXVECTOR3& GetPos() { return m_pos; }//取得

	//更新前の座標
	void SetOldPos(const D3DXVECTOR3& oldpos) { m_oldpos = oldpos; }//設定
	D3DXVECTOR3& GetOldPos() { return m_oldpos; }//取得

	//移動量
	void SetMove(const D3DXVECTOR3& move) { m_move = move; }//設定
	D3DXVECTOR3& GetMove() { return m_move; }//取得

	//角度
	void SetRot(const D3DXVECTOR3& rot) { m_rot = rot; }//設定
	D3DXVECTOR3& GetRot() { return m_rot; }//取得

	//サイズ
	void SetSize(const D3DXVECTOR3& size) { m_size = size; }//設定
	D3DXVECTOR3& GetSize() { return m_size; }//取得

	//カラー
	void SetColor(const D3DXCOLOR& col) { m_col = col; }//設定
	D3DXCOLOR& GetColor() { return m_col; }//取得

	//テクスチャ分割数
	void SetDivisionTex(int nWidth, int nHeight)//設定
	{
		m_nDivisionWidth = nWidth;//横の分割数
		m_nDivisionHeight = nHeight;//縦の分割数
	}
	int GetDivisionWidth() { return m_nDivisionWidth; }//横の取得
	int GetDivisionHeight() { return m_nDivisionHeight; }//縦の取得

	//アニメーションパターン
	void SetDelayAnim(int nDelayAnim) { m_nDelayAnim = nDelayAnim; }//アニメーション遅延設定
	int GetPatternAnimWidth() { return m_nPatternAnimWidth; }//横のパターンの取得
	int GetPatternAnimHeight() { return m_nPatternAnimHeight; }//縦のパターンの取得

private:
	//======================================
	// 変数
	//======================================
	D3DXVECTOR3 m_pos;//座標
	D3DXVECTOR3 m_oldpos;//過去の座標
	D3DXVECTOR3 m_move;//移動量
	D3DXVECTOR3 m_rot;//角度
	D3DXVECTOR3 m_size;//サイズ
	D3DXCOLOR m_col;//カラー
	int m_nDivisionWidth;//横のテクスチャ分割数
	int m_nDivisionHeight;//横のテクスチャ分割数
	int m_nDelayAnim;//アニメーション遅延
	int m_nPatternAnimWidth;//横のアニメーションパターン
	int m_nPatternAnimHeight;//縦のアニメーションパターン
	int m_nCounterAnim;//アニメーションカウンタ
};

#endif