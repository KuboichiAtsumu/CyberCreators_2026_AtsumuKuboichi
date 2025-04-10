//==============================================================================================================================================
//
// ボス移動ポイント処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "bossmovepoint.h"

//===========================================================================================================
// 静的メンバ変数初期化
//===========================================================================================================
std::vector<CBossMovePoint*> CBossMovePoint::m_BossMovePoint;//全ポイントの情報
int CBossMovePoint::m_nAll = 0;//合計数

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CBossMovePoint::CBossMovePoint() : CGimmick()
{
	//インデックスを設定
	m_nIdx = m_nAll;

	//合計数を加算
	m_nAll++;

	//自分自身の情報を設定
	m_BossMovePoint.push_back(this);
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CBossMovePoint::~CBossMovePoint()
{
	//合計数を減算
	m_nAll--;
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CBossMovePoint::Init()
{
	//ギミック初期化処理
	if (FAILED(CGimmick::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CBossMovePoint::Uninit()
{
	//ギミック終了処理
	CGimmick::Uninit();
}

//===========================================================================================================
// 解放処理
//===========================================================================================================
void CBossMovePoint::Release()
{
	//ポインタをnullptrにする
	m_BossMovePoint.front() = nullptr;

	//先頭の要素を削除
	m_BossMovePoint.erase(m_BossMovePoint.begin());
	
	//ギミック解放処理
	CGimmick::Release();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CBossMovePoint::Update()
{
	//ギミック更新処理
	CGimmick::Update();
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CBossMovePoint::Draw()
{
}

//===========================================================================================================
// 検索処理
//===========================================================================================================
CBossMovePoint* CBossMovePoint::Find(int nIdx)
{
	//要素数を取得
	int nSize = m_BossMovePoint.size();

	for (int nCnt = 0; nCnt < nSize; nCnt++)
	{
		//指定されたインデックスと一致した場合
		if (m_BossMovePoint.at(nCnt)->m_nIdx == nIdx)
		{
			return m_BossMovePoint.at(nCnt);
		}
	}

	return nullptr;
}
