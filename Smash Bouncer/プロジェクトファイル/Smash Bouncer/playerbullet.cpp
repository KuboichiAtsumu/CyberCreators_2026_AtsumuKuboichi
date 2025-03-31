//==============================================================================================================================================
//
// プレイヤーバレットの処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "playerbullet.h"//プレイヤーバレットヘッダー
#include "blockmanager.h"//ブロックマネージャーヘッダー
#include "enemymanager.h"//エネミーマネージャーヘッダー
#include "itemmanager.h"//アイテムマネージャーヘッダー
#include "gimmickmanager.h"//ギミックマネージャーヘッダー
#include "score.h"//スコアヘッダー
#include "tutorial.h"//チュートリアルシーン
#include "addscore.h"//加算スコア表示
#include "title.h"//タイトルシーン

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CPlayerBullet::CPlayerBullet(int nPriority) : CBullet(nPriority)
{
	//メンバ変数初期化
	m_fScoreMag = 1.0f;//スコア倍率
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CPlayerBullet::~CPlayerBullet()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CPlayerBullet::Init()
{
	//バレット初期化処理
	if (FAILED(CBullet::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CPlayerBullet::Uninit()
{
	//バレット終了処理
	CBullet::Uninit();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CPlayerBullet::Update()
{
	//バレットの更新処理
	CBullet::Update();

	//ローカル変数宣言
	D3DXVECTOR3 pos = GetPos();//位置情報
	D3DXVECTOR3 oldpos = GetOldPos();//過去の位置情報
	D3DXVECTOR3 rot = GetRot();//向き情報
	D3DXVECTOR3 move = GetMove();//移動量情報
	D3DXVECTOR3 size = GetSize();//サイズ情報
	int nLife = GetLife();//ライフ情報
	CBlockManager* pBlockManager = nullptr;//ブロックマネージャー
	CEnemyManager* pEnemyManager = nullptr;//エネミーマネージャー
	CItemManager* pItemManager = nullptr;//アイテムマネージャー
	CGimmickManager* pGimmickManager = nullptr;//ギミックマネージャー

	//シーンに応じてマネージャー情報を取得
	switch (CManager::GetScene()->GetMode())
	{
		//タイトル
	case CScene::MODE_TITLE:
		pBlockManager = CTitle::GetBlockManager();
		break;

		//チュートリアル
	case CScene::MODE_TUTORIAL:
		pBlockManager = CTutorial::GetBlockManager();
		pEnemyManager = CTutorial::GetEnemyManager();
		break;

		//ゲーム
	case CScene::MODE_GAME:
		pBlockManager = CGame::GetBlockManager();
		pEnemyManager = CGame::GetEnemyManager();
		pItemManager = CGame::GetItemManager();
		pGimmickManager = CGame::GetGimmickManager();
		break;

	default:
		break;
	}

	//ブロック反射処理
	if (pBlockManager != nullptr && pBlockManager->Reflection(pos, oldpos, move, rot, size))
	{
		//スコア倍率を増加
		m_fScoreMag += 0.1f;
	}
	
	//エネミーの当たり判定
	if (pEnemyManager != nullptr && pEnemyManager->Collision(pos, size))
	{
		//スコアを加算
		if (CGame::GetScore() != nullptr)
		{
			CGame::GetScore()->AddScore((int)(100.0f * m_fScoreMag));
		}

		//現在の位置情報を保存
		D3DXVECTOR3 CreatePos = GetPos();

		//生成位置の高さを調整
		CreatePos.y += 40.0f;

		for (int nCnt = 0; nCnt < 4; nCnt++)
		{
			//加算スコアを表示
			CAddScore::Create(nCnt, CreatePos, { 10.0f, 20.0f, 0.0f }, (int)(100.0f * m_fScoreMag));
		}

		//終了処理
		Uninit();
	}

	//アイテムマネージャーの情報が存在する場合
	if (pItemManager != nullptr)
	{
		//アイテムの当たり判定
		pItemManager->Collision(pos, size, nLife);
	}

	//ギミックマネージャーの情報が存在する場合
	if (pGimmickManager != nullptr)
	{
		//ギミックとの当たり判定
		pGimmickManager->Collision(pos, move, size, rot);
	}

	//更新された情報を設定
	SetPos(pos);//位置
	SetRot(rot);//向き
	SetMove(move);//移動量
	SetLife(nLife);//ライフ
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CPlayerBullet::Draw()
{
	//バレット描画処理
	CBullet::Draw();
}
