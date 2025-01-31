//==============================================================================================================================================
//
// シーンに関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "scene.h"
#include "title.h"
#include "tutorial.h"
#include "game.h"
#include "result.h"
#include "ranking.h"
#include "edit3D.h"
#include "edit2D.h"
#include "editmanager.h"
#include "blockmanager.h"
#include "gimmickmanager.h"
#include "bulletmanager.h"
#include "enemymanager.h"
#include "areamanager.h"
#include "UImanager.h"
//#include <EffekseerForDXLib_170e_324b/VC/DxLib.h>
//#include <EffekseerForDXLib_170e_324b/VC/EffekseerForDXLib.h>

//===========================================================================================================
// 静的メンバ変数初期化
//===========================================================================================================
CScene::ResultScore CScene::m_aResultScore = {};

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CScene::CScene() : 
	m_pEdit2D{ nullptr },
	m_pEditManager{ nullptr },
	m_pBlockManager{ nullptr },
	m_pGimmickManager{ nullptr },
	m_pBulletManager{ nullptr },
	m_pEnemyManager{ nullptr },
	m_pAreaManager{ nullptr },
	m_pUIManager{ nullptr },
	m_mode{ MODE::MODE_NONE },
	m_Scene{ SCENE::GAME }
{
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CScene::~CScene()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CScene::Init()
{
#ifdef _DEBUG
	//2Dエディタ初期化処理
	m_pEdit2D = NEW CEdit2D();
	if (FAILED(m_pEdit2D->Init()))
	{
		return E_FAIL;
	}
#endif // _DEBUG

	//UIマネージャー初期化処理
	m_pUIManager = NEW CUIManager();
	if (FAILED(m_pUIManager->Init()))
	{
		return E_FAIL;
	}
	
	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CScene::Uninit()
{
#ifdef _DEBUG
	//エディタオブジェクトマネージャー
	if (m_pEditManager != nullptr)
	{
		m_pEditManager->Uninit();
		m_pEditManager = nullptr;
	}

	//2Dオブジェクトマネージャー
	if (m_pEdit2D != nullptr)
	{
		m_pEdit2D->Uninit();
		delete m_pEdit2D;
		m_pEdit2D = nullptr;
	}
#endif // _DEBUG

	//ブロックマネージャー
	if (m_pBlockManager != nullptr)
	{
		m_pBlockManager->Uninit();
		m_pBlockManager = nullptr;
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

	//エネミーマネージャー
	if (m_pEnemyManager != nullptr)
	{
		m_pEnemyManager->Uninit();
		m_pEnemyManager = nullptr;
	}

	//エリアマネージャー
	if (m_pAreaManager != nullptr)
	{
		m_pAreaManager->Uninit();
		m_pAreaManager = nullptr;
	}

	//UIマネージャー
	if (m_pUIManager != nullptr)
	{
		m_pUIManager->Uninit();
		m_pUIManager = nullptr;
	}

	//エネミー総数リセット
	CEnemy::ResetAll();

	//全オブジェクト終了処理
	CObject::UninitAll();

	//カメラの座標情報をリセット
	CManager::GetInstance()->GetCamera()->Reset();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CScene::Update()
{
	//デバッグのみ
#ifdef _DEBUG
	//マネージャーのインスタンス取得
	CManager* pManager = CManager::GetInstance();

	//キーボードのインスタンス取得
	CInputKeyboard* pKeyboard = pManager->GetKeyboard();

	switch (m_Scene)
	{
		//ゲームシーン
	case SCENE::GAME:

		//F5キーが押された場合
		if (pKeyboard->GetTrigger(DIK_F5))
		{
			//2Dエディタシーンにする
			m_Scene = SCENE::EDIT_2D;
		}
		break;

		//2Dエディタシーン
	case SCENE::EDIT_2D:

		//2Dエディタ更新処理
		m_pEdit2D->Update();

		//F5キーが押された場合
		if (pKeyboard->GetTrigger(DIK_F5))
		{
			//ゲームシーンにする
			m_Scene = SCENE::GAME;
		}
		break;

		//3Dエディタシーン
	case SCENE::EDIT_3D:
		//3Dエディタ更新処理
		CEdit3D::EditUpdate();
		break;

	default:
		break;

	}
#endif // DEBUG
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CScene::Draw()
{
}

//===========================================================================================================
// 生成処理
//===========================================================================================================
CScene* CScene::Create(MODE mode)
{
	//ローカル変数宣言
	CScene* pScene = nullptr;//シーン情報のポインタ
	
	//モード別に動的確保
	switch (mode)
	{
		//タイトル
	case CScene::MODE::MODE_TITLE:
		pScene = CTitle::Create();
		break;
	
		//チュートリアル
	case CScene::MODE::MODE_TUTORIAL:
		pScene = CTutorial::Create();
		break;

		//ゲーム
	case CScene::MODE::MODE_GAME:
		pScene = CGame::Create();
		break;

		//リザルト
	case CScene::MODE::MODE_RESULT:
		pScene = CResult::Create();
		break;

		//ランキング
	case CScene::MODE::MODE_RANKING:
		pScene = CRanking::Create();
		break;

	default:
		break;
	}

	//モードの設定
	pScene->SetMode(mode);

	return pScene;
}
