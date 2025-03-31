//==============================================================================================================================================
//
// チュートリアル画面に関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "tutorial.h"//チュートリアル
#include "edit3D.h"//3Dエディタ
#include "camera.h"//カメラ
#include "fade.h"//フェード

//オブジェクト
#include "player.h"//プレイヤー
#include "field.h"//フィールド
#include "zone.h"//ゾーン

//UI
#include "heading.h"//見出し
#include "operation.h"//操作方法UI
#include "gamestart.h"//GameStartUI

//マネージャー
#include "blockmanager.h"//ブロックマネージャー
#include "enemymanager.h"//エネミーマネージャー
#include "bulletmanager.h"//バレットマネージャー

//===========================================================================================================
// 静的メンバ変数初期化
//===========================================================================================================
CBlockManager* CTutorial::m_pBlockManager = nullptr;//ブロックマネージャー情報
CEnemyManager* CTutorial::m_pEnemyManager = nullptr;//エネミーマネージャー情報
CBulletManager* CTutorial::m_pBulletManager = nullptr;//バレットマネージャー情報

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CTutorial::CTutorial()
{
	//メンバ変数初期化
	m_nSpawnTime = 0;//エネミースポーンまでのカウント
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CTutorial::~CTutorial()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CTutorial::Init()
{
	//モデル・テクスチャ情報の読み込みと登録処理
	CXfile::LoadModelFile();//モデルファイルパス読込
	CXfile::LoadTextureFile();//テクスチャファイルパス読込
	CXfile::Regist();//登録処理

	//デバッグ時のみ
#ifdef _DEBUG
	//エディタ用オブジェクト情報読込
	CEdit3D::LoadInfo(CManager::TUTORIAL_STAGE);
#endif // _DEBUG

	//指定したステージにプレイヤー生成
	CPlayer::Create({ FIRST_SPAWN, 0.0f, 0.0f });

	//フィールド生成処理
	CField::Create({ -450.0f, 0.0f, 0.0f }, { CField::FIELD_WIDTH, 0.0f, CField::FIELD_HEIGHT });

	//ステージテクスチャファイルパス読込処理
	CZone::LoadZoneTexFile();

	//ステージ区分ゾーン生成
	CZone::Create({ 10.0f, 0.01f, 0.0f }, { CZone::STAGEZONE_WIDTH, 0.0f, CZone::STAGEZONE_HEIGHT }, CZone::STAGE);

	//反射ゾーン生成
	CZone::Create({ FIRST_SPAWN, 0.02f, 0.0f }, { CZone::REFLECTZONE_WIDTH, 0.0f, CZone::REFLECTZONE_HEIGHT }, CZone::REFLECT);

	//UI生成
	CHeading::Create();//見出し
	COperation::Create();//操作方法UI
	CGameStart::Create();//GameStartUI

	//ブロックの生成
	m_pBlockManager = NEW CBlockManager();
	m_pBlockManager->Init();//初期化処理

	//エネミーの生成
	m_pEnemyManager = NEW CEnemyManager();
	m_pEnemyManager->Init();//初期化処理

	//バレットマネージャーの生成
	m_pBulletManager = NEW CBulletManager();
	m_pBulletManager->Init();//初期化処理

	//BGM再生
	CManager::GetSound()->Play(CSound::BGM_TUTORIAL);

	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CTutorial::Uninit()
{
	//BGM再生終了
	CManager::GetSound()->Stop(CSound::BGM_TUTORIAL);

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

	//バレットマネージャー
	if (m_pBulletManager != nullptr)
	{
		m_pBulletManager->Uninit();
		m_pBulletManager = nullptr;
	}

	//デバッグ時のみ
#ifdef _DEBUG
	//全エディタオブジェクトの終了処理
	CEdit3D::UninitAll();
#endif // _DEBUG

	//シーン終了処理
	CScene::Uninit();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CTutorial::Update()
{
	//シーン更新処理
	CScene::Update();

	//Enterキーが押された場合
	//STARTボタンが押された場合
	if ((CManager::GetKeyboard()->GetTrigger(DIK_RETURN)
		|| CManager::GetJoypad()->GetTrigger(CInputJoypad::JOYKEY_START))
		&& !GetEditMode())
	{
		//SE再生
		CManager::GetSound()->Play(CSound::SE_MODE);

		//ゲーム画面へ移行
		CManager::GetFade()->SetFade(MODE_GAME);
	}

	//敵の総数が0になった場合
	if (CEnemy::GetAllENemy() <= 0)
	{
		//エネミースポーンまでのカウントが0の場合
		if (m_nSpawnTime <= 0)
		{
			//敵をスポーンさせる
			m_pEnemyManager->Regist(CEnemy::Create(ENEMY_SPAWN_POS, ENEMY_SPAWN_ROT, 0));//エネミーマネージャーにエネミー情報を登録
			
			//エネミースポーンまでのカウントを設定
			SetSpawnTime();
		}
		//エネミースポーンまでのカウントが残っている場合
		else
		{
			//カウントを減らす
			m_nSpawnTime--;
		}
	}

	//カメラの更新処理
	CManager::GetCamera()->Update();
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CTutorial::Draw()
{
	//カメラの設定
	CManager::GetCamera()->SetCamera();
}

//===========================================================================================================
// 生成処理
//===========================================================================================================
CTutorial* CTutorial::Create()
{
	CTutorial* pTutorial = NEW CTutorial();

	//初期化処理
	pTutorial->Init();

	return pTutorial;
}
