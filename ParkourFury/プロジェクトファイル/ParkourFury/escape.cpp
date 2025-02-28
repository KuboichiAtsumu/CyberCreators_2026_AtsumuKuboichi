//==============================================================================================================================================
//
// チェイスエリアの警告に関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "escape.h"

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CEscape::CEscape() : CUI()
{
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CEscape::~CEscape()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CEscape::Init()
{
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
void CEscape::Uninit()
{
	//基底クラス終了処理
	CUI::Uninit();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CEscape::Update()
{
	//基底クラス更新処理
	CUI::Update();
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CEscape::Draw()
{
	//基底クラス描画処理
	CUI::Draw();
}
