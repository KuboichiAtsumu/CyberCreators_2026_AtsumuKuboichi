//==============================================================================================================================================
//
// ゲームマネージャー
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "manager.h"//マネージャー
#include "keyboard.h"//キーボード
#include "mouse.h"//マウス
#include "joypad.h"//ジョイパッド
#include "camera.h"//カメラ
#include "light.h"//ライト
#include "fade.h"//フェード

//===========================================================================================================
// 静的メンバ変数初期化
//===========================================================================================================
CRenderer* CManager::m_pRenderer = nullptr;//レンダラ
CInputKeyboard* CManager::m_pKeyboard = nullptr;//キーボード
CInputMouse* CManager::m_pMouse = nullptr;//マウス
CInputJoypad* CManager::m_pJoypad = nullptr;//Pad
CSound* CManager::m_pSound = nullptr;//サウンド
CCamera* CManager::m_pCamera = nullptr;//カメラ
CLight* CManager::m_pLight = nullptr;//ライト
CScene* CManager::m_pScene = nullptr;//シーン
CFade* CManager::m_pFade = nullptr;//フェード
CTexture* CManager::m_pTexture = nullptr;//テクスチャ
CXfile* CManager::m_pModel = nullptr;//モデル

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CManager::CManager()
{
	srand((unsigned)time(nullptr));
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CManager::~CManager()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CManager::Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	//レンダラーの生成
	m_pRenderer = NEW CRenderer();
	m_pRenderer->Init(hWnd, bWindow);

	//キーボードの初期化
	m_pKeyboard = NEW CInputKeyboard();
	m_pKeyboard->Init(hInstance, hWnd);

	//マウスの初期化
	m_pMouse = NEW CInputMouse();
	m_pMouse->Init(hInstance, hWnd);

	//ジョイパッドの初期化
	m_pJoypad = NEW CInputJoypad();
	m_pJoypad->Init();

	//サウンドの初期化
	m_pSound = NEW CSound();
	m_pSound->Init(hWnd);

	//カメラの初期化
	m_pCamera = NEW CCamera();
	m_pCamera->Init();

	//ライトの初期化
	m_pLight = NEW CLight();
	m_pLight->Init();

	//テクスチャの初期化
	m_pTexture = NEW CTexture();

	//モデルの初期化
	m_pModel = NEW CXfile();

	//フェード画面生成
	m_pFade = CFade::Create();

	//モードの初期化
	SetMode(CScene::MODE_TITLE);

	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CManager::Uninit()
{
	//サウンド全停止
	GetSound()->Stop();

	//フェード
	if (m_pFade != nullptr)
	{
		m_pFade->Uninit();
		m_pFade = nullptr;
	}

	//シーン
	if (m_pScene != nullptr)
	{
		m_pScene->Uninit();
		delete m_pScene;
		m_pScene = nullptr;
	}

	//レンダラー
	if (m_pRenderer != nullptr)
	{
		m_pRenderer->Uninit();
		delete m_pRenderer;
		m_pRenderer = nullptr;
	}

	//キーボード
	if (m_pKeyboard != nullptr)
	{
		m_pKeyboard->Uninit();
		delete m_pKeyboard;
		m_pKeyboard = nullptr;
	}

	//マウス
	if (m_pMouse != nullptr)
	{
		m_pMouse->Uninit();
		delete m_pMouse;
		m_pMouse = nullptr;
	}

	//Pad
	if (m_pJoypad != nullptr)
	{
		m_pJoypad->Uninit();
		delete m_pJoypad;
		m_pJoypad = nullptr;
	}

	//サウンド
	if (m_pSound != nullptr)
	{
		m_pSound->Uninit();
		delete m_pSound;
		m_pSound = nullptr;
	}

	//カメラ
	if (m_pCamera != nullptr)
	{
		m_pCamera->Uninit();
		delete m_pCamera;
		m_pCamera = nullptr;
	}

	//ライト
	if (m_pLight != nullptr)
	{
		m_pLight->Uninit();
		delete m_pLight;
		m_pLight = nullptr;
	}

	//テクスチャ
	if (m_pTexture != nullptr)
	{
		m_pTexture->Unload();
		delete m_pTexture;
		m_pTexture = nullptr;
	}

	//モデル
	if (m_pModel != nullptr)
	{
		m_pModel->Unload();
		delete m_pModel;
		m_pModel = nullptr;
	}
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CManager::Update()
{
	//キーボード更新処理
	m_pKeyboard->Update();

	//マウス更新処理
	m_pMouse->Update();

	//ジョイパッド更新処理
	m_pJoypad->Update();

	//シーン更新処理
	m_pScene->Update();

	//レンダラー更新処理
	m_pRenderer->Update();
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CManager::Draw()
{
	//レンダラー描画処理
	m_pRenderer->Draw();
}

//===========================================================================================================
// モードの設定
//===========================================================================================================
void CManager::SetMode(CScene::MODE mode)
{
	//シーン情報が存在する場合
	if (m_pScene != nullptr)
	{
		//終了処理
		m_pScene->Uninit();

		delete m_pScene;
		m_pScene = nullptr;
	}

	//新たなシーンを生成
	m_pScene = CScene::Create(mode);
}
