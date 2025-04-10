//==============================================================================================================================================
//
// タイマー表示に関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "timenumber.h"
#include "game.h"
#include "timer.h"

//===========================================================================================================
// 静的メンバ変数初期化
//===========================================================================================================
int CTimeNumber::m_nDigit = 0;//桁数

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CTimeNumber::CTimeNumber() : CUI(),
	m_nIdx(m_nDigit)
{
	m_nDigit++;//桁数を加算
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CTimeNumber::~CTimeNumber()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CTimeNumber::Init()
{
	//座標設定
	SetTimeNumberPos(GetPos());

	//テクスチャ分割数
	SetDivisionTex_X(10);//横

	//エディット可能フラグ
	SetEditFlag(false);

	//基底クラス初期化処理
	if (FAILED(CUI::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CTimeNumber::Uninit()
{
	//桁数を減算
	m_nDigit--;

	//基底クラス終了処理
	CUI::Uninit();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CTimeNumber::Update()
{
	//エディタ中
	if (CManager::GetInstance()->GetScene()->GetSceneState() != CScene::SCENE::GAME) return;

	//ローカル変数宣言
	int nTimer = CGame::GetInstance()->GetTimer()->GetTime();//現在のタイマー
	int nMulti = 1;//各桁の倍数

	//インデックスに合わせて倍数を求める
	for (int nCntTime = 0; nCntTime < m_nIdx + 1; nCntTime++)
	{
		nMulti *= 10;
	}

	// 該当する桁の数字を求める
	int nPosTex = (nTimer % nMulti) / (nMulti / 10);

	//横のアニメーションパターンを設定
	SetPatternAnim_X(nPosTex);

	//タイマーが最大の場合
	if (nTimer == CGame::GetInstance()->GetTimer()->MAX_TIMER)
	{
		//赤色に設定
		SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });
	}

	//基底クラス更新処理
	CUI::Update();
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CTimeNumber::Draw()
{
	//基底クラス描画処理
	CUI::Draw();
}

//===========================================================================================================
// 座標設定
//===========================================================================================================
void CTimeNumber::SetTimeNumberPos(const D3DXVECTOR2& pos)
{
	//桁数に応じて左にずらす
	SetPos({ pos.x - m_nIdx * (GetSize().x), pos.y });
}
