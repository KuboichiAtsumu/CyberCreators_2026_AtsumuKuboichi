//==============================================================================================================================================
//
// ゲーム画面に関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "game.h"//ゲームシーン
#include "fade.h"//フェード
#include "edit3D.h"//3Dエディタ
#include "camera.h"//カメラ
#include "field.h"//フィールド
#include "player.h"//プレイヤー
#include "zone.h"//ゾーン
#include "score.h"//スコア情報
#include "scorenumber.h"//スコア表示
#include "stagenumber.h"//ステージ数表示
#include "gameUI.h"//見出し
#include "blockmanager.h"//ブロックマネージャー
#include "enemymanager.h"//エネミーマネージャー
#include "itemmanager.h"//アイテムマネージャー
#include "pointmanager.h"//ポイントマネージャー
#include "gimmickmanager.h"//ギミックマネージャー
#include "bulletmanager.h"//バレットマネージャー

//===========================================================================================================
// 静的メンバ変数初期化
//===========================================================================================================
CGame::STAGE CGame::m_Stage = STAGE_1;//現在のステージ
CGame::GAMESTATE CGame::m_GameState = STATE_GAME;//ゲームの状態
CBlockManager* CGame::m_pBlockManager = nullptr;//ブロックマネージャー情報
CEnemyManager* CGame::m_pEnemyManager = nullptr;//エネミーマネージャー情報
CItemManager* CGame::m_pItemManager = nullptr;//アイテムマネージャー情報
CPointManager* CGame::m_pPointManager = nullptr;//ポイントマネージャー情報
CGimmickManager* CGame::m_pGimmickManager = nullptr;//ギミックマネージャー情報
CBulletManager* CGame::m_pBulletManager = nullptr;//バレットマネージャー情報
CScore* CGame::m_pScore = nullptr;//スコア情報

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CGame::CGame()
{
	//メンバ変数初期化
	m_Stage = STAGE_1;//現在のステージ
	m_GameState = STATE_GAME;//ゲームの状態
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CGame::~CGame()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CGame::Init()
{
	//モデル・テクスチャ情報の読み込みと登録処理
	CXfile::LoadModelFile();//モデルファイルパス読込
	CXfile::LoadTextureFile();//テクスチャファイルパス読込
	CXfile::Regist();//登録処理

	//デバッグ時のみ
#ifdef _DEBUG
	//エディタ用オブジェクト情報読込
	CEdit3D::LoadInfo(CManager::GAME_STAGE);

	//指定したステージにプレイヤー生成
	CPlayer::Create({ FIRST_SPAWN + (CZone::STAGEZONE_WIDTH * (float)SPAWN_STAGE), 0.0f, 0.0f });
#else
	//初期位置にプレイヤー生成
	CPlayer::Create({ FIRST_SPAWN, 0.0f, 0.0f });
#endif // _DEBUG

	//フィールド生成処理
	CField::Create({ -450.0f, 0.0f, 0.0f }, { CField::FIELD_WIDTH, 0.0f, CField::FIELD_HEIGHT });

	//ステージテクスチャファイルパス読込処理
	CZone::LoadZoneTexFile();

	//ステージ区分ゾーン生成
	for (int nCnt = 0; nCnt < MAX_STAGE; nCnt++)
	{
		CZone::Create({ 10.0f + CZone::STAGEZONE_WIDTH * (float)nCnt, 0.01f, 0.0f }, { CZone::STAGEZONE_WIDTH, 0.0f, CZone::STAGEZONE_HEIGHT }, CZone::STAGE);
	}

	//反射ゾーン生成
	for (int nCnt = 0; nCnt < MAX_STAGE; nCnt++)
	{
		CZone::Create({ FIRST_SPAWN + (CZone::STAGEZONE_WIDTH * (float)nCnt), 0.02f, 0.0f }, { CZone::REFLECTZONE_WIDTH, 0.0f, CZone::REFLECTZONE_HEIGHT }, CZone::REFLECT);
	}

	//スコア数字生成
	for (int nCnt = 0; nCnt < CScoreNumber::MAX_NUMBER; nCnt++)
	{
		CScoreNumber::Create(nCnt, { 1240.0f, 50.0f }, { 40.0f, 80.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
	}

	//現在のステージ生成
	for (int nCnt = 0; nCnt < CStageNumber::MAX_NUMBER; nCnt++)
	{
		CStageNumber::Create(nCnt);
	}

	//見出し生成
	CGameUI::Create(CGameUI::UI_ATTENTION, { CProcess::SCREEN_WIDTH * 0.5f, 650.0f }, { 800.0, 80.0f });//警告
	CGameUI::Create(CGameUI::UI_STAGE, { 130.0f, 55.0f }, { 250.0, 100.0f });//警告
	CGameUI::Create(CGameUI::UI_ALLSTAGE, { 400.0f, 55.0f }, { 100.0, 95.0f });//警告

	//ブロックの生成
	m_pBlockManager = NEW CBlockManager();
	m_pBlockManager->Init();//初期化処理

	//エネミーの生成
	m_pEnemyManager = NEW CEnemyManager();
	m_pEnemyManager->Init();//初期化処理

	//アイテムの生成
	m_pItemManager = NEW CItemManager();
	m_pItemManager->Init();//初期化処理

	//ポイントの生成
	m_pPointManager = NEW CPointManager();
	m_pPointManager->Init();//初期化処理

	//ギミックの生成
	m_pGimmickManager = NEW CGimmickManager();
	m_pGimmickManager->Init();//初期化処理

	//バレットの生成
	m_pBulletManager = NEW CBulletManager();
	m_pBulletManager->Init();//初期化処理

	//スコア情報生成
	m_pScore = NEW CScore();
	m_pScore->Init();//初期化処理

	//BGM再生
	CManager::GetSound()->Play(CSound::BGM_GAME);

	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CGame::Uninit()
{
	//BGM再生
	CManager::GetSound()->Stop(CSound::BGM_GAME);

	//ブロックマネージャー
	if (m_pBlockManager != nullptr)
	{
		m_pBlockManager->Uninit();
		m_pBlockManager = nullptr;
	}

	//エネミーマネージャー
	if (m_pEnemyManager != nullptr)
	{
		m_pEnemyManager->Uninit();
		m_pEnemyManager = nullptr;
	}

	//アイテムマネージャー
	if (m_pItemManager != nullptr)
	{
		m_pItemManager->Uninit();
		m_pEnemyManager = nullptr;
	}

	//ポイントマネージャー
	if (m_pPointManager != nullptr)
	{
		m_pPointManager->Uninit();
		m_pPointManager = nullptr;
	}

	//ギミックマネージャー
	if (m_pGimmickManager != nullptr)
	{
		m_pGimmickManager->Uninit();
		m_pGimmickManager = nullptr;
	}

	//バレットマネージャー
	if (m_pBulletManager != nullptr)
	{
		m_pBulletManager->Uninit();
		m_pBulletManager = nullptr;
	}

	//スコア
	if (m_pScore != nullptr)
	{
		m_pScore->Uninit();
		m_pScore = nullptr;
	}

	//カメラの位置情報をリセット
	CManager::GetCamera()->Reset();

	//デバッグ時のみ
#ifdef _DEBUG
	//全エディタオブジェクトの終了処理
	CEdit3D::UninitAll();
#endif // _DEBUG

	//シーンの終了処理
	CScene::Uninit();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CGame::Update()
{
	//シーン更新処理
	CScene::Update();

	//デバッグのみ
#ifdef _DEBUG
	//ゲームモードの場合
	if(!CManager::GetScene()->GetEditMode())
	{
		//Enterキーが押された場合
		if (CManager::GetKeyboard()->GetTrigger(DIK_RETURN)
			|| CManager::GetJoypad()->GetTrigger(CInputJoypad::JOYKEY_A)
			|| CManager::GetJoypad()->GetTrigger(CInputJoypad::JOYKEY_START))
		{
			//リザルト画面へ移行
			CManager::GetFade()->SetFade(MODE_RESULT);
		}
	}
#endif // DEBUG

	//エネミーを全て倒した場合
	if (CObject3D::GetAllEachStageID(CGame::GetNumStage(), CObject::ENEMY) <= 0)
	{
		//ゲームの状態を移動中にする
		CGame::SetGameState(CGame::STATE_MOVE);
	}

	//敵の総数が0になった場合
	if (CEnemy::GetAllENemy() <= 0)
	{
		//リザルト画面へ移行
		CManager::GetFade()->SetFade(MODE_RESULT);
	}
	
	//カメラの更新処理
	CManager::GetCamera()->Update();
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CGame::Draw()
{
	//カメラの設定
	CManager::GetCamera()->SetCamera();
}

//===========================================================================================================
// 生成処理
//===========================================================================================================
CGame* CGame::Create()
{
	//メモリを動的確保
	CGame* pGame = NEW CGame();

	//初期化処理
	pGame->Init();

	return pGame;
}

//===========================================================================================================
// ゲームの状態設定処理
//===========================================================================================================
void CGame::SetGameState(GAMESTATE state)
{
	//ゲームの状態を指定された状態に設定
	m_GameState = state;

	//設定された状態が移動状態の場合
	if (m_GameState == STATE_MOVE)
	{
		//指定した順番にオブジェクト情報を取得する
		for (int nCnt = 0; nCnt < MAX_PRIORITY; nCnt++)
		{
			//先頭
			CObject* pObj = CObject::GetTop(nCnt);

			//オブジェクト情報がnullptrになるまで繰り返す
			while (pObj != nullptr)
			{
				//次のオブジェクト情報を取得
				CObject* pNext = pObj->GetNext();

				//オブジェクトタイプの取得
				CObject::TYPE type = pObj->GetType();

				//オブジェクトタイプがゾーンの場合
				if (type == CObject::ZONE)
				{
					//オブジェクト情報をゾーン情報に変換
					CZone* pZone = (CZone*)pObj;

					//ゾーンタイプが反射ゾーンの場合
					//現在のステージとゾーンのIDが一致する場合
					if (pZone->GetZoneType() == CZone::REFLECT && pZone->GetID() == GetNumStage())
					{
						//当たり判定を無効化する
						pZone->SetCollisionFlag(false);
					}
				}

				//オブジェクト情報を次の情報に変更する
				pObj = pNext;
			}
		}
	}
}
