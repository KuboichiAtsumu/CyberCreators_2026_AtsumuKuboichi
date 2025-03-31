//==============================================================================================================================================
//
// ギミックの処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "gimmick.h"//ギミック
#include "movefloor.h"//移動床
#include "warphole.h"//ワープホール
#include "switchbase.h"//スイッチベース
#include "switch.h"//スイッチ

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CGimmick::CGimmick(int nPriority) : CObjectX(nPriority)
{
	//メンバ変数初期化
	m_pPrev = nullptr;//前のギミック情報
	m_pNext = nullptr;//次のギミック情報
	m_GimmickType = {};//ギミックタイプ
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CGimmick::~CGimmick()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CGimmick::Init()
{
	//テクスチャ生成
	CTexture* pTex = CManager::GetTexture();//取得
	int nTextureIdx = pTex->Regist(CXfile::GetSaveModelInfo(CXfile::GIMMICK, m_GimmickType)->aTextureFile);//登録
	BindTexture(pTex->GetAddress(nTextureIdx));//設定
	SetTextureIdx(nTextureIdx);//ID設定

	//モデル生成
	CXfile* pModel = CManager::GetModel();//取得
	int nModelIdx = pModel->Regist(CXfile::GetSaveModelInfo(CXfile::GIMMICK, m_GimmickType)->aModelFile);//登録
	BindModel(pModel->GetAddress(nModelIdx));//設定
	SetModelIdx(nModelIdx);//ID設定
	SetSize();//サイズ設定

	//自分がいるステージIDを登録
	SetPostStageID();

	//Xファイルオブジェクト初期化処理
	if (FAILED(CObjectX::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CGimmick::Uninit()
{
	//Xファイルオブジェクト終了処理
	CObjectX::Uninit();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CGimmick::Update()
{
	//Xファイルオブジェクト更新処理
	CObjectX::Update();
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CGimmick::Draw()
{
	//エディタモードの場合
	if (CManager::GetScene()->GetEditMode())
	{//描画せずに処理を抜ける
		return;
	}

	//Xファイルのオブジェクト描画処理
	CObjectX::Draw();
}

//===========================================================================================================
// 生成処理
//===========================================================================================================
CGimmick* CGimmick::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nType)
{
	//タイプに応じて動的確保
	CGimmick* pGimmick = nullptr;
	switch (nType)
	{
		//移動床
	case MOVEFLOOR:
		pGimmick = NEW CMoveFloor(4);
		break;

		//ワープホール
	case WARPHOLE:
		pGimmick = NEW CWarpHole(4);
		break;

		//スイッチベース
	case SWITCHBASE:
		pGimmick = NEW CSwitchBase(4);
		break;

		//スイッチ
	case SWITCH:
		pGimmick = NEW CSwitch(4);
		break;

	default:
		break;
	}

	pGimmick->SetType(TYPE::GIMMICK);//オブジェクトタイプ設定
	pGimmick->SetPos(pos);//位置設定
	pGimmick->SetRot(rot);//向き設定
	pGimmick->SetGimmickType((GIMMICKTYPE)nType);//アイテムタイプ設定
	pGimmick->Init();//初期化処理

	return pGimmick;
}

//===========================================================================================================
// 当たり判定
//===========================================================================================================
bool CGimmick::Collision(D3DXVECTOR3& pos, D3DXVECTOR3& move, D3DXVECTOR3 size, D3DXVECTOR3& rot)
{
	//ローカル変数宣言
	bool bCollision = false;//当たり判定のフラグ

	//情報を取得
	D3DXVECTOR3 Pos = GetPos();//位置
	D3DXVECTOR3 Size = GetSize();//サイズ

	//X座標が重なった場合
	if (pos.x + size.x * 0.5f >= Pos.x - Size.x * 0.5f
		&& pos.x - size.x * 0.5f <= Pos.x + Size.x * 0.5f)
	{
		//Z座標が重なった場合
		if (pos.z + size.z * 0.5f >= Pos.z - Size.z * 0.5f
			&& pos.z + size.z * 0.5f <= Pos.z + Size.z * 0.5f)
		{
			//当たり判定のフラグをtrueにする
			bCollision = true;
		}
	}

	return bCollision;
}
