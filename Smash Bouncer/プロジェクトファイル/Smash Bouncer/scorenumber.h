#pragma once
//==============================================================================================================================================
//
// スコア表示に関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _SCORENUMBER_H_
#define _SCORENUMBER_H_

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "score.h"//スコア

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CScoreNumber : public CScore
{
public:
	//======================================
	// 関数
	//======================================
	CScoreNumber(int nPriority = 97);//コンストラクタ
	~CScoreNumber() override;//デストラクタ
	HRESULT Init() override;//初期化処理
	void Uninit() override;//終了処理
	void Release() override;//解放処理
	void Update() override;//更新処理
	void Draw() override;//描画処理
	static CScoreNumber* Create(int nIdx, D3DXVECTOR2 pos, D3DXVECTOR2 size, D3DXCOLOR col);//生成処理
	static void SaveScore(int nScore) { m_nSaveScore = nScore; }//スコアを保存する

	//パラメータ設定
	static int GetDigit() { return m_nDigit; }//桁数

	//======================================
	// 変数
	//======================================
	static const int MAX_NUMBER = 6;//桁数

private:
	//======================================
	// 関数
	//======================================

	//パラメータ設定
	void SetIndex(int nIdx) { m_nIdx = nIdx; }//ID
	void SetPos(D3DXVECTOR2 pos);//位置

	//======================================
	// 変数
	//======================================
	static constexpr const char* TEXTURE_FILE = "data/TEXTURE/number000.png";//テクスチャファイル
	const float COLOR_ASCENT = 0.02f;//カラー上昇速度
	static CScoreNumber* m_apNumber[MAX_NUMBER];//各桁の情報
	static int m_nSaveScore;//スコア保存
	static int m_nDigit;//桁数
	int m_nIdx;//ID
	bool m_bUse;//使用フラグ
};

#endif
