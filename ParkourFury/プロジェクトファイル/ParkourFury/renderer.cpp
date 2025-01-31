//==============================================================================================================================================
//
// レンダラーに関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "renderer.h"
#include "debugproc.h"
#include "object.h"

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CRenderer::CRenderer()
{
	//メンバ変数初期化
	m_pD3D = nullptr;
	m_pD3DDevice = nullptr;
	m_bStop = false;//一時停止フラグ
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CRenderer::~CRenderer()
{

}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CRenderer::Init(HWND hWnd, BOOL bWindow)
{
	//ローカル変数宣言
	D3DDISPLAYMODE d3ddm;//ディスプレイモード
	D3DPRESENT_PARAMETERS d3dpp;//プレゼンテーションパラメータ
	LPD3DXFONT pFont = nullptr;//フォントへのポインタ

	//Direct3Dオブジェクトの生成
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_pD3D == nullptr)
	{
		return E_FAIL;
	}

	//現在のディスプレイモードを取得
	if (FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	//デバイスのプレゼンテーションパラメータの設定
	ZeroMemory(&d3dpp, sizeof(d3dpp));//パラメータのゼロクリア
	d3dpp.BackBufferWidth = static_cast<UINT>(SCREEN_WIDTH);//ゲーム画面サイズ(横幅)
	d3dpp.BackBufferHeight = static_cast<UINT>(SCREEN_HEIGHT);//ゲーム画面サイズ(縦幅)
	d3dpp.BackBufferFormat = d3ddm.Format;//バックバッファの形式
	d3dpp.BackBufferCount = 1;//バックバッファの数
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;//ダブルバッファ切り替え
	d3dpp.EnableAutoDepthStencil = TRUE;//デプスバッファ・ステンシャルバッファ作成
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;//デプスバッファを24bitに
	d3dpp.Windowed = bWindow;//ウインドウモード
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;//リフレッシュレート
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;//インターバル

	//Direct3Dデバイスの生成(描画処理と頂点処理をハードウェアで行う)
	if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp,
		&m_pD3DDevice)))
	{
		//Direct3Dデバイスの生成(描画処理はハードウェア、頂点処理はCPUで行う)
		if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp,
			&m_pD3DDevice)))
		{
			//Direct3Dデバイスの生成(描画処理と頂点処理をCPUで行う)
			if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF,
				hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&d3dpp,
				&m_pD3DDevice)))
			{
				return E_FAIL;
			}
		}
	}

	//レンダーステートの設定
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//テクスチャステージステート
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	//デバッグ表示
	InitDebugProc();

	//デバック表示用フォントの生成
	D3DXCreateFont(m_pD3DDevice, 18, 0, 0, 0, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Terminal", &pFont);

	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CRenderer::Uninit()
{
	//全オブジェクト終了処理
	CObject::UninitAll();

	//デバッグ表示
	UninitDebugProc();
	
	//Direct3Dデバイスの破棄
	if (m_pD3DDevice != nullptr)
	{
		m_pD3DDevice->Release();
		m_pD3DDevice = nullptr;
	}
	//Direct3Dオブジェクトの破棄
	if (m_pD3D != nullptr)
	{
		m_pD3D->Release();
		m_pD3D = nullptr;
	}
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CRenderer::Update()
{
	//デバッグ時のみ
#ifdef _DEBUG
	//デバッグ表示
	UpdateDebugProc();

	//Pキーが押された場合
	if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_P))
	{
		//一時停止フラグを切り替える
		m_bStop = m_bStop ? false : true;
	}
#endif // _DEBUG

	//一時停止中ではない場合
	if (!m_bStop)
	{
		//全オブジェクト更新処理
		CObject::UpdateAll();
	}
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CRenderer::Draw()
{
	//画面クリア(バックバッファ＆Zバッファのクリア)
	m_pD3DDevice->Clear(0, nullptr,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

	//描画開始
	if (SUCCEEDED(m_pD3DDevice->BeginScene()))
	{
		//シーン
		CManager::GetInstance()->GetScene()->Draw();

		//全ゲームオブジェクト
		CObject::DrawAll();

		//フェード
		CManager::GetInstance()->GetFade()->Draw();

		//デバッグ時のみ
#ifdef _DEBUG
		//デバッグ表示
		DrawDebugProc();
#endif // _DEBUG
		
		//描画終了
		m_pD3DDevice->EndScene();
	}

	//バックバッファとフロントバッファ入れ替え
	m_pD3DDevice->Present(nullptr, nullptr, nullptr, nullptr);
}

//===========================================================================================================
// 3Dデバイスの取得
//===========================================================================================================
LPDIRECT3DDEVICE9 CRenderer::GetDevice()
{
	return m_pD3DDevice;
}

//===========================================================================================================
// ワイヤーフレームの有効化
//===========================================================================================================
void CRenderer::OnWire()
{
	m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
}

//===========================================================================================================
// ワイヤーフレームの無効化
//===========================================================================================================
void CRenderer::OffWire()
{
	m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

//===========================================================================================================
// フォグの有効化
//===========================================================================================================
void CRenderer::OnFog()
{
	//フォグを有効化
	m_pD3DDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);

	//フォグカラー設定
	m_pD3DDevice->SetRenderState(D3DRS_FOGCOLOR, D3DXCOLOR(0.0f, 0.0f, 0.3f, 0.1f));

	//フォグのパラメータ設定(範囲指定)
	m_pD3DDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);

	//始点設定
	float fFogStart = 1000.0f;
	m_pD3DDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD*)(&fFogStart));

	//終点設定
	float fFogEnd = 5000.0f;
	m_pD3DDevice->SetRenderState(D3DRS_FOGEND, *(DWORD*)(&fFogEnd));
}

//===========================================================================================================
// フォグの無効化
//===========================================================================================================
void CRenderer::OffFog()
{
	//フォグを無効化
	m_pD3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
}

