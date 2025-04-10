//==============================================================================================================================================
//
// プレイヤースポナー処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "playerspawner.h"

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CPlayerSpawner::CPlayerSpawner() : CGimmick()
{
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CPlayerSpawner::~CPlayerSpawner()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
#include "player.h"
HRESULT CPlayerSpawner::Init()
{
	//プレイヤーを生成
	CPlayer::Create(GetPos());
	
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
void CPlayerSpawner::Uninit()
{
	//ギミック終了処理
	CGimmick::Uninit();
}

//===========================================================================================================
// 解放処理
//===========================================================================================================
void CPlayerSpawner::Release()
{
	//ギミック解放処理
	CGimmick::Release();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CPlayerSpawner::Update()
{
	//ギミック更新処理
	CGimmick::Update();
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CPlayerSpawner::Draw()
{
#ifdef _DEBUG
	//ギミック描画処理
	CGimmick::Draw();
#endif // _DEBUG
}
