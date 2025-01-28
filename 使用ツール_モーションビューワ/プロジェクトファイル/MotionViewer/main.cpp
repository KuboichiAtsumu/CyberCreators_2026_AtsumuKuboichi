//=======================================================================================================================================================================================================================
//
// スクリーンに関する処理
// Author : Atsumu Kuboichi
//
//=======================================================================================================================================================================================================================
#include "main.h"
#include <cstdio>
#include "light.h"
#include "game.h"

//=======================================================================================================================================================================================================================
// グローバル変数
//=======================================================================================================================================================================================================================
LPDIRECT3D9 g_pD3D = NULL;				//Direct3Dオブジェクトへのポインタ
LPDIRECT3DDEVICE9 g_pD3DDevice = NULL;	//Direct3Dデバイスへのポインタ
LPD3DXFONT g_pFont = NULL;				//フォントへのポインタ
MODE g_mode = MODE_GAME;				//現在のモード
int g_nCountFPS = 0;					//FPSカウンタ
bool g_isFullscreen = false;			//ウィンドウを切り替えるためのフラグ
RECT g_windowRect;						//ウィンドウを切り替えるための変数

//=======================================================================================================================================================================================================================
// メイン関数
//=======================================================================================================================================================================================================================
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hInstancePrev*/, _In_ LPSTR /*lpCmdLine*/, _In_ int nCmdShow)
{
	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),				//WNDCLASSEXのメモリサイズ
		CS_CLASSDC,						//ウインドウのスタイル
		WindowProc,						//ウインドウプロシージャ
		0,								//0にする
		0,								//0にする
		hInstance,						//インスタンスハンドル
		LoadIcon(NULL,IDI_ASTERISK),	//タスクバーのアイコン
		LoadCursor(NULL,IDC_CROSS),		//マウスカーソル
		(HBRUSH)(COLOR_ACTIVECAPTION),	//クライアント領域の背景色
		NULL,							//メニューバー
		CLASS_NAME,						//ウインドウクラスの名前
		LoadIcon(NULL,IDI_APPLICATION)	//ファイルのアイコン
	};

	HWND hWnd;				//ウインドウハンドル(識別子)
	MSG msg;				//メッセージを格納する変数
	RECT rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
	DWORD dwCurrentTime;	//現在時刻
	DWORD dwExecLastTime;	//最後に処理した時刻
	DWORD dwFrameCount;		//フレームカウント
	DWORD dwFPSLastTime;	//最後にFPSを計測した時刻

	RegisterClassEx(&wcex);	//ウインドウクラスの登録

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);	//クライアント領域を指定のサイズに調整

	//ウインドウを生成
	hWnd = CreateWindowEx(0,		//拡張ウインドウスタイル
		CLASS_NAME,					//ウインドウクラスの名前
		WINDOW_NAME,				//ウインドウの名前
		WS_OVERLAPPEDWINDOW,		//ウインドウスタイル
		CW_USEDEFAULT,				//ウインドウ左上X座標
		CW_USEDEFAULT,				//ウインドウ左上Y座標
		(rect.right-rect.left),		//ウインドウの幅
		(rect.bottom - rect.top),	//ウインドウの高さ
		NULL,						//親ウインドウのハンドル
		NULL,						//メニューハンドルまたは子ウインドウID
		hInstance,					//インスタンスハンドル
		NULL);						//ウインドウの作成データ

	//初期化処理
	if (FAILED(Init(hInstance, hWnd, TRUE)))
	{
		return-1;//初期化処理が失敗した場合
	}

	//ウインドウの表示
	ShowWindow(hWnd, nCmdShow);	//ウインドウの表示状態の設定
	UpdateWindow(hWnd);			//クライアント領域を更新

	//分解能を設定
	timeBeginPeriod(1);

	dwCurrentTime = 0;				//初期化する
	dwExecLastTime = timeGetTime();	//現在時刻を取得(保存)
	dwFrameCount = 0;
	dwFPSLastTime = timeGetTime();

	//メッセージループ
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
		{
			//windowsの処理
			if (msg.message == WM_QUIT) //終了
			{
				break;//WM_QUITメッセージを受け取ったらメッセージループを抜ける
			}
			else
			{
				//メッセージの設定
				TranslateMessage(&msg);	//仮想キーメッセージを文字メッセージへ変換
				DispatchMessage(&msg);	//ウインドウプロシージャへメッセージを送出
			}
		}
		else
		{
			//DirectXの処理
			dwCurrentTime = timeGetTime(); //現在時刻を取得

			if ((dwCurrentTime - dwFPSLastTime) >= MAX_TIMEF) //0.5秒経過
			{
				g_nCountFPS = (dwFrameCount * 1000) / (dwCurrentTime - dwFPSLastTime);	//FPSを計測
				dwFPSLastTime = dwCurrentTime;											//FPSを測定した時刻を保存
				dwFrameCount = 0;														//フレームカウントをクリア
			}
			if ((dwCurrentTime - dwExecLastTime) >= (1000 / MAX_FRAME))
			{
				//時間経過
				dwExecLastTime = dwCurrentTime; //現在時刻を保存

				Update();	//更新処理
				Draw();		//描画処理

				dwFrameCount++; //フレームカウントを加算
			}
		}
	}
	
	Uninit(); //終了処理
	
	UnregisterClass(CLASS_NAME, wcex.hInstance);//ウインドウクラスの登録を解除

	return(int)msg.wParam;
}

//=======================================================================================================================================================================================================================
// ウインドウプロシージャ
//=======================================================================================================================================================================================================================
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nID; //返り値格納用

	switch (uMsg)
	{
	case WM_DESTROY:	//ウインドウ破棄のメッセージ
		PostQuitMessage(0);	//WM_QUITメッセージを送る
		break;

	case WM_KEYDOWN:	//キー押下のメッセージ
		
		switch (wParam)
		{
		case VK_ESCAPE:	//[ESC]キー入力

			nID = MessageBox(hWnd, "終了しますか？", "終了メッセージ", MB_YESNOCANCEL| MB_ICONQUESTION);	//終了メッセージ

			if (nID == IDYES) //はい
			{
				DestroyWindow(hWnd);//ウインドウを破棄する(WM_DESTROYメッセージを送る)
				break;
			}
			break;
		case VK_F11:
			ToggleFullscreen(hWnd);
			break;
		}
		break;

	case WM_CLOSE: //閉じるボタンのメッセージ

		nID = MessageBox(hWnd, "終了しますか？", "終了メッセージ", MB_YESNOCANCEL | MB_ICONQUESTION);	//終了メッセージ

		if (nID == IDYES) //はい
		{
			DestroyWindow(hWnd);//ウインドウを破棄する(WM_DESTROYメッセージを送る)
		}
		else
		{
			return 0; //0を返さないと終了
		}
		break;

	case WM_LBUTTONDOWN:	//マウス左クリックのメッセージ
		SetFocus(hWnd);	//ウインドウにフォーカスを合わせる
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//=======================================================================================================================================================================================================
// 初期化処理
//=======================================================================================================================================================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	D3DDISPLAYMODE d3ddm;			//ディスプレイモード
	D3DPRESENT_PARAMETERS d3dpp;	//プレゼンテーションパラメータ

	//Direct3Dオブジェクトの生成
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (g_pD3D == NULL)
	{
		return E_FAIL;
	}

	//現在のディスプレイモードを取得
	if (FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	//デバイスのプレゼンテーションパラメータの設定
	ZeroMemory(&d3dpp, sizeof(d3dpp));							//パラメータのゼロクリア
	d3dpp.BackBufferWidth = SCREEN_WIDTH;						//ゲーム画面サイズ(幅)
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;						//ゲーム画面サイズ(高さ)
	d3dpp.BackBufferFormat = d3ddm.Format;						//バックバッファの形式
	d3dpp.BackBufferCount = 1;									//バックバッファの数
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;					//ダブルバッファ切り替え
	d3dpp.EnableAutoDepthStencil = TRUE;						//デプスバッファ・ステンシャルバッファ作成
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;					//デプスバッファを16bitに
	d3dpp.Windowed = bWindow;									//ウインドウモード
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;	//リフレッシュレート
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;	//インターバル

	//Direct3Dデバイスの生成(描画処理と頂点処理をハードウェアで行う)
	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp,
		&g_pD3DDevice)))
	{
		//Direct3Dデバイスの生成(描画処理はハードウェア、頂点処理はCPUで行う)
		if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp,
			&g_pD3DDevice)))
		{
			//Direct3Dデバイスの生成(描画処理と頂点処理をCPUで行う)
			if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF,
				hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&d3dpp,
				&g_pD3DDevice)))
			{
				return E_FAIL;
			}
		}
	}

	//レンダーステートの設定
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//テクスチャステージステート
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	//キーボード
	if (FAILED(InitKeyboard(hInstance, hWnd)))
	{
		return E_FAIL;
	}

	//マウス
	if (FAILED(InitMouse(hInstance, hWnd)))
	{
		return E_FAIL;
	}

	//ジョイパッド
	if (FAILED(InitJoypad()))
	{
		return E_FAIL;
	}

	InitLight();	//ライト
	InitDebugProc();//デバッグ表示
	SetMode(g_mode);//モードの設定

	//デバック表示用フォントの生成
	D3DXCreateFont(g_pD3DDevice, 18, 0, 0, 0, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Terminal", &g_pFont);

	return S_OK;
}

//=======================================================================================================================================================================================================
// 終了処理
//=======================================================================================================================================================================================================
void Uninit(void)
{
	UninitKeyboard();	//キーボード
	UninitMouse();		//マウス
	UninitJoypad();		//ジョイパッド
	UninitLight();		//ライト
	UninitDebugProc();	//デバッグ表示

	//Direct3Dデバイスの破棄
	if (g_pD3DDevice != NULL)
	{
		g_pD3DDevice->Release();
		g_pD3DDevice = NULL;
	}
	//Direct3Dオブジェクトの破棄
	if (g_pD3D != NULL)
	{
		g_pD3D->Release();
		g_pD3D = NULL;
	}
}

//=======================================================================================================================================================================================================
// 更新処理
//=======================================================================================================================================================================================================
void Update(void)
{
	UpdateKeyboard();	//キーボード
	UpdateMouse();		//マウス
	UpdateJoypad();		//コントローラー
	UpdateStick();		//コントローラースティック
	UpdateLight();		//ライト
	UpdateDebugProc();	//デバッグ表示

	switch (g_mode)
	{
	case MODE_GAME: //ゲーム画面
		UpdateGame();
		break;

	default:
		break;
	}
}

//=======================================================================================================================================================================================================
// 描画処理
//=======================================================================================================================================================================================================
void Draw(void)
{
	//画面クリア(バックバッファ＆Zバッファのクリア)
	g_pD3DDevice->Clear(0, NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		D3DCOLOR_RGBA(30, 144, 255, 0), 1.0f, 0);

	//描画開始
	if (SUCCEEDED(g_pD3DDevice->BeginScene()))
	{
		switch (g_mode)
		{
		case MODE_GAME: //ゲーム画面
			DrawGame();
			break;

		default:
			break;
		}

		DrawDebugProc();	//デバッグ表示

		g_pD3DDevice->EndScene();	//描画終了
	}

	g_pD3DDevice->Present(NULL, NULL, NULL, NULL);	//バックバッファとフロントバッファ入れ替え
}

//=======================================================================================================================================================================================================
// デバイスの取得
//=======================================================================================================================================================================================================
LPDIRECT3DDEVICE9 GetDevice(void)
{
	return g_pD3DDevice;
}

//=======================================================================================================================================================================================================
// モードの設定
//=======================================================================================================================================================================================================
void SetMode(MODE mode)
{
	//現在の画面の終了処理
	switch (g_mode)
	{
	case MODE_GAME: //ゲーム画面
		UninitGame();
		break;

	default:
		break;
	}

	g_mode = mode; //現在の画面を切り替える

	//新しい画面の初期化処理
	switch (g_mode)
	{
	case MODE_GAME: //ゲーム画面
		InitGame();
		break;

	default:
		break;
	}
}

//=======================================================================================================================================================================================================
// ワイヤーフレームの有効化
//=======================================================================================================================================================================================================
void onwire(void)
{
	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
}

//=======================================================================================================================================================================================================
// ワイヤーフレームの無効化
//=======================================================================================================================================================================================================
void offwire(void)
{
	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

//=======================================================================================================================================================================================================
// ウィンドウをフルスクリーンに変える処理
//=======================================================================================================================================================================================================
void ToggleFullscreen(HWND hWnd)
{
	// 現在のウィンドウスタイルを取得
	DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);

	if (g_isFullscreen)
	{
		// ウィンドウモードに切り替え
		SetWindowLong(hWnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPos(hWnd, HWND_TOP, g_windowRect.left, g_windowRect.top,
			g_windowRect.right - g_windowRect.left, g_windowRect.bottom - g_windowRect.top,
			SWP_FRAMECHANGED | SWP_NOACTIVATE);
		ShowWindow(hWnd, SW_NORMAL);
	}
	else
	{
		// フルスクリーンモードに切り替え
		GetWindowRect(hWnd, &g_windowRect);
		SetWindowLong(hWnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
		SetWindowPos(hWnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
			SWP_FRAMECHANGED | SWP_NOACTIVATE);
		ShowWindow(hWnd, SW_MAXIMIZE);
	}

	g_isFullscreen = !g_isFullscreen;
}
