//==============================================================================================================================================
//
// チュートリアル画面に関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "tutorial.h"
#include "edit3D.h"

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CTutorial::CTutorial() : CScene()
{
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
#include "back_3D.h"
#include "editmanager.h"
#include "blockmanager.h"
#include "enemymanager.h"
#include "bulletmanager.h"
#include "gimmickmanager.h"
#include "signboardmanager.h"
HRESULT CTutorial::Init()
{
	//BGM再生
	CManager::GetInstance()->GetSound()->Play(CSound::SOUND::BGM_TUTORIAL);

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

	//看板マネージャーのインスタンス取得
	CSignBoardManager* pSignBoardManager = NEW CSignBoardManager();
	SetSignBoardManager(pSignBoardManager);//インスタンス設定
	if (FAILED(pSignBoardManager->Init())) return E_FAIL;//初期化処理

	//シーン初期化処理
	if (FAILED(CScene::Init())) return E_FAIL;

	//ステージ背景生成
	CBack3D::Create({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, CBack3D::TYPE::SKY);

	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CTutorial::Uninit()
{
	//BGM再生終了
	CManager::GetInstance()->GetSound()->Stop(CSound::SOUND::BGM_TUTORIAL);

	//基底クラス終了処理
	CScene::Uninit();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CTutorial::Update()
{
	//インスタンス取得
	CManager* pManager = CManager::GetInstance();//マネージャー
	CScene* pScene = pManager->GetScene();//シーン
	CInputKeyboard* pKeyboard = pManager->GetKeyboard();//キーボード
	CInputJoypad* pJoypad = pManager->GetJoypad();//ジョイパッド

	//Enterキーが押された場合
	//STARTボタンが押された場合
	if ((pKeyboard->GetTrigger(DIK_RETURN) ||
		pJoypad->GetTrigger(CInputJoypad::JOYKEY::JOYKEY_START)) &&
		pScene->GetSceneState() == SCENE::GAME)
	{
		//SE再生
		pManager->GetSound()->Play(CSound::SOUND::SELECT);

		//ゲーム画面へ移行
		pManager->GetFade()->SetFade(MODE::MODE_GAME);
	}

	//カメラの更新処理
	pManager->GetCamera()->Update();

	//基底クラス更新
	CScene::Update();

	//デバッグのみ
#ifdef _DEBUG

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
			pJoypad->GetTrigger(CInputJoypad::JOYKEY::JOYKEY_START))
		{
			//ゲーム画面へ移行
			pManager->GetFade()->SetFade(MODE::MODE_GAME);
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
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CTutorial::Draw()
{
	//カメラの設定
	CManager::GetInstance()->GetCamera()->SetCamera();
}

//===========================================================================================================
// 生成処理
//===========================================================================================================
CTutorial* CTutorial::Create()
{
	//メモリを動的確保
	CTutorial* pTutorial = NEW CTutorial();

	return pTutorial;
}
