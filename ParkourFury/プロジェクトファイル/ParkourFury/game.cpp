//==============================================================================================================================================
//
// ゲーム画面に関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "game.h"
#include "timer.h"
#include "back_3D.h"
#include "UI.h"
#include "editmanager.h"
#include "blockmanager.h"
#include "gimmickmanager.h"
#include "bulletmanager.h"
#include "enemymanager.h"
#include "areamanager.h"
#include "signboardmanager.h"
#include "player.h"
#include "edit3D.h"

//===========================================================================================================
// 静的メンバ変数初期化
//===========================================================================================================
CGame* CGame::m_pGame = nullptr;

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CGame::CGame() : CScene(),
	m_pTimer{ nullptr },
	m_Area{ GAME_AREA::NORMAL }
{
	//メンバ変数初期化
	m_SecondlyHeat.clear();
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CGame::~CGame()
{
}

//===========================================================================================================
// インスタンス取得
//===========================================================================================================
CGame* CGame::GetInstance()
{
	//ゲーム中ではない場合
	if (CManager::GetInstance()->GetScene()->GetMode() != MODE::MODE_GAME)
	{
		//インスタンスを取得しない
		return nullptr;
	}

	//インスタンスが存在しない場合
	if (m_pGame == nullptr)
	{
		//インスタンスを生成
		m_pGame = NEW CGame();

		//タイマー生成
		m_pGame->m_pTimer = NEW CTimer();//メモリを動的確保
		m_pGame->m_pTimer->Init();//初期化処理
	}

	return m_pGame;
}

//===========================================================================================================
// インスタンス破棄
//===========================================================================================================
void CGame::Release()
{
	//タイマー
	if (m_pGame->m_pTimer != nullptr)
	{
		m_pGame->m_pTimer->Uninit();
		delete m_pGame->m_pTimer;
		m_pGame->m_pTimer = nullptr;
	}

	//インスタンスが存在する場合
	if (m_pGame != nullptr)
	{
		delete m_pGame;
		m_pGame = nullptr;
	}
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CGame::Init()
{
	//BGM再生
	CManager::GetInstance()->GetSound()->Play(CSound::SOUND::BGM_GAME);

#ifdef _DEBUG
	//エディタマネージャー初期化処理
	CEdit3DManager* pEditManager = NEW CEdit3DManager();
	SetEditManager(pEditManager);//インスタンス設定
	if (FAILED(pEditManager->Init())) return E_FAIL;//初期化処理
#endif // _DEBUG

	//ブロックマネージャーのインスタンス取得
	CBlockManager* pBlockManager = NEW CBlockManager();
	SetBlockManager(pBlockManager);//インスタンス設定
	if (FAILED(pBlockManager->Init())) return E_FAIL;//初期化処理

	//エネミーマネージャーのインスタンス取得
	CEnemyManager* pEnemyManager = NEW CEnemyManager();
	SetEnemyManager(pEnemyManager);//インスタンス設定
	if (FAILED(pEnemyManager->Init())) return E_FAIL;//初期化処理

	//バレットマネージャーのインスタンス取得
	CBulletManager* pBulletManager = NEW CBulletManager();
	SetBulletManager(pBulletManager);//インスタンス設定
	if (FAILED(pBulletManager->Init())) return E_FAIL;//初期化処理

	//ギミックマネージャーのインスタンス取得
	CGimmickManager* pGimmickManager = NEW CGimmickManager();
	SetGimmickManager(pGimmickManager);//インスタンス設定
	if (FAILED(pGimmickManager->Init())) return E_FAIL;//初期化処理

	//エリアマネージャーのインスタンス取得
	CAreaManager* pAreaManager = NEW CAreaManager();
	SetAreaManager(pAreaManager);//インスタンス設定
	if (FAILED(pAreaManager->Init())) return E_FAIL;//初期化処理

	//看板マネージャーのインスタンス取得
	CSignBoardManager* pSignBoardManager = NEW CSignBoardManager();
	SetSignBoardManager(pSignBoardManager);//インスタンス設定
	if (FAILED(pSignBoardManager->Init())) return E_FAIL;//初期化処理

	//シーン初期化処理
	if (FAILED(CScene::Init()))
	{
		return E_FAIL;
	}

	//インスタンス生成
	GetInstance();

	//ステージ背景生成
	CBack3D::Create({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, CBack3D::TYPE::SKY);

	//エリア背景生成
	CBack3D::Load();

	//UI生成
	CUI::Create(CUI::TYPE::TIMER_HEADING, { 1190.0f, 25.0f }, { 80.0f, 30.0f }, { 0.0f, 0.0f, 0.0f });
	CUI::Create(CUI::TYPE::TIME_NUMBER, { 1220.0f, 90.0f }, { 60.0f, 90.0f }, { 0.0f, 0.0f, 0.0f });

	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CGame::Uninit()
{
	//インスタンス取得
	CManager* pManager = CManager::GetInstance();

	//BGM再生終了
	pManager->GetSound()->Stop(CSound::SOUND::BGM_GAME);

	//リザルト用スコアの構造体情報取得
	ResultScore aResultScore = GetResultScore();

	//ゴールタイム取得
	aResultScore.nGoalTime = m_pGame->m_pTimer->GetTime();

	//プレイヤーカテゴリーのオブジェクト情報を取得
	CObject* pFindObj = CObject::FindObject(CObject::Category::PLAYER);

	//オブジェクト情報が存在する場合
	if (pFindObj == nullptr) return;

	//プレイヤークラスにダウンキャスト
	CPlayer* pPlayer = CObject::DownCast<CPlayer, CObject>(pFindObj);

	//キル数取得
	aResultScore.fNumKillenemy = static_cast<float>(pPlayer->GetKill()) / static_cast<float>(CEnemy::GetAll()) * 100.0f;

	//リザルト用スコア設定
	SetResultScore(aResultScore);

	//エリア判定を通常エリアにする
	m_pGame->m_Area = GAME_AREA::NORMAL;

	//フォグを無効化
	pManager->GetRenderer()->OffFog();

	//タイマーリセット
	m_pGame->m_pTimer->Reset();

	//インスタンスの破棄
	Release();

	//基底クラス終了処理
	CScene::Uninit();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CGame::Update()
{	
	//マネージャーのインスタンス取得
	CManager* pManager = CManager::GetInstance();

	//デバッグのみ
#ifdef _DEBUG

	//キーボードのインスタンス取得
	CInputKeyboard* pKeyboard = pManager->GetKeyboard();
	
	//シーンのインスタンス取得
	CScene* pScene = pManager->GetScene();

	switch (pScene->GetSceneState())
	{
		//ゲームシーン
	case SCENE::GAME:

		//F4キーが押された場合
		if (pKeyboard->GetTrigger(DIK_F4))
		{
			//3Dエディタシーンにする
			pScene->SetScene(SCENE::EDIT_3D);
		}
		
		//Enterキーが押された場合
		if (pKeyboard->GetTrigger(DIK_RETURN) ||
			pManager->GetJoypad()->GetTrigger(CInputJoypad::JOYKEY::JOYKEY_START))
		{
			//リザルト画面へ移行
			pManager->GetFade()->SetFade(MODE::MODE_RESULT);
		}

		break;

		//3Dエディタシーン
	case SCENE::EDIT_3D:

		//F4キーが押された場合
		if (pKeyboard->GetTrigger(DIK_F4))
		{
			//ゲームシーンにする
			pScene->SetScene(SCENE::GAME);
		}

		break;

	default:
		break;
	}
#endif // DEBUG

	//タイマー
	m_pGame->m_pTimer->Update();

	//カメラの更新処理
	pManager->GetCamera()->Update();

	//フォグ設定処理
	SetFog();

	//基底クラス更新
	CScene::Update();
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CGame::Draw()
{
	//カメラの設定
	CManager::GetInstance()->GetCamera()->SetCamera();
}

//===========================================================================================================
// 生成処理
//===========================================================================================================
CGame* CGame::Create()
{
	//メモリを動的確保
	CGame* pGame = NEW CGame();

	return pGame;
}

//===========================================================================================================
// 平均ヒート量計算処理
//===========================================================================================================
void CGame::AverageHeat()
{
	//プレイヤーカテゴリーのオブジェクトを検索
	CObject* pFindObj = CObject::FindObject(CObject::Category::PLAYER);

	//検索結果が不一致
	if (pFindObj == nullptr)
	{
		//処理を抜ける
		return;
	}

	//プレイヤークラスにダウンキャスト
	CPlayer* pPlayer = CObject::DownCast<CPlayer, CObject>(pFindObj);

	//ダウンキャスト失敗
	if (pPlayer == nullptr)
	{
		//処理を抜ける
		return;
	}

	//現在のヒート量を保存
	m_SecondlyHeat.push_back(pPlayer->GetHeat());

	//現在のデータ数を取得
	int nSize = m_SecondlyHeat.size();

	//保存されているヒート量の合計値を計算
	float fAllHeat = 0.0f;
	for (int nCnt = 0; nCnt < nSize; nCnt++)
	{
		fAllHeat += m_SecondlyHeat.at(nCnt);
	}

	//リザルト用スコアの構造体情報取得
	ResultScore aResultScore = GetResultScore();

	//平均ヒート量を更新して設定
	aResultScore.fAverageHeat = fAllHeat / static_cast<float>(nSize);
	SetResultScore(aResultScore);
}

//===========================================================================================================
// フォグ設定処理
//===========================================================================================================
void CGame::SetFog()
{
	//レンダラーインスタンス取得
	CRenderer* pRenderer = CManager::GetInstance()->GetRenderer();

	//通常エリア
	if (m_pGame->m_Area == GAME_AREA::NORMAL)
	{
		//フォグを無効化
		pRenderer->OffFog();
	}
	//イベントエリア
	else
	{
		//フォグを有効化
		pRenderer->OnFog();
	}
}