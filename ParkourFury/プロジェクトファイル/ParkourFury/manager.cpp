//==============================================================================================================================================
//
// ゲームマネージャー
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "manager.h"
#include "xfile.h"
#include "texture.h"

//===========================================================================================================
// 静的メンバ変数初期化
//===========================================================================================================
CManager* CManager::m_pManager = nullptr;

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CManager::CManager() : 
	m_pRenderer{ nullptr },
	m_pKeyboard{ nullptr },
	m_pMouse{ nullptr },
	m_pJoypad{ nullptr },
	m_pSound{ nullptr },
	m_pCamera{ nullptr },
	m_pLight{ nullptr },
	m_pScene{ nullptr },
	m_pFade{ nullptr },
	m_pCollision{ nullptr },
	m_pTag{ nullptr }
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

	//タグのインスタンスを生成
	m_pTag = NEW CTag();

	//当たり判定のインスタンスを生成
	m_pCollision = NEW CCollision();

	//計算処理のインスタンスを生成
	m_pCalculate = NEW CCalculate();
	
	//フェード画面生成
	m_pFade = CFade::Create();

	//モードの初期化
	SetMode(CScene::MODE::MODE_TITLE);

	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CManager::Uninit()
{
	//サウンド全停止
	GetSound()->Stop();

	//タグ
	if (m_pTag != nullptr)
	{
		delete m_pTag;
		m_pTag = nullptr;
	}

	//フェード
	if (m_pFade != nullptr)
	{
		m_pFade->Uninit();
		delete m_pFade;
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

	//当たり判定
	if (m_pCollision != nullptr)
	{
		delete m_pCollision;
		m_pCollision = nullptr;
	}

	//計算処理
	if (m_pCalculate != nullptr)
	{
		delete m_pCalculate;
		m_pCalculate = nullptr;
	}

	//テクスチャ一括管理
	CTexture::Release();

	//Xファイル一括管理
	CXfile::Release();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CManager::Update()
{
	//ジョイパッド更新処理
	m_pJoypad->Update();

	//キーボード更新処理
	m_pKeyboard->Update();

	//マウス更新処理
	m_pMouse->Update();

	//フェード更新処理
	m_pFade->Update();

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

	//シーン初期化処理
	m_pScene->Init();
}

//===========================================================================================================
// インスタンスの取得
//===========================================================================================================
CManager* CManager::GetInstance()
{
	//インスタンスが存在しない場合
	if (m_pManager == nullptr)
	{
		//インスタンスを生成
		m_pManager = NEW CManager();
	}

	return m_pManager;
}

//===========================================================================================================
// インスタンスの破棄
//===========================================================================================================
void CManager::Release()
{
	//インスタンスが存在する場合
	if (m_pManager != nullptr)
	{
		//インスタンスを破棄
		m_pManager->Uninit();
		delete m_pManager;
		m_pManager = nullptr;
	}
}
