//==============================================================================================================================================
//
// スクリーンに関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "main.h"//メイン
#include "manager.h"//マネージャー

//===========================================================================================================
// 定数
//===========================================================================================================
RECT CProcess::m_windowRect = {};//ウィンドウ切り替え用変数
bool CProcess::m_isFullscreen = false;//ウィンドウ切り替え用フラグ

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CProcess::CProcess()
{
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CProcess::~CProcess()
{
}

//===========================================================================================================
// メイン関数
//===========================================================================================================
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hInstancePrev*/, _In_ LPSTR /*lpCmdLine*/, _In_ int nCmdShow)
{
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	CProcess process;
	CManager manager;
	int nCountFPS = 0;//FPSカウンタ

	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),//WNDCLASSEXのメモリサイズ
		CS_CLASSDC,//ウインドウのスタイル
		WindowProc,//ウインドウプロシージャ
		0,//0にする
		0,//0にする
		hInstance,//インスタンスハンドル
		LoadIcon(nullptr,IDI_ASTERISK),//タスクバーのアイコン
		LoadCursor(nullptr,IDC_CROSS),//マウスカーソル
		(HBRUSH)(COLOR_ACTIVECAPTION),//クライアント領域の背景色
		nullptr,//メニューバー
		process.CLASS_NAME,//ウインドウクラスの名前
		LoadIcon(nullptr,IDI_APPLICATION)//ファイルのアイコン
	};

	HWND hWnd;//ウインドウハンドル(識別子)
	MSG msg;//メッセージを格納する変数
	RECT rect = { 0,0,process.SCREEN_WIDTH,process.SCREEN_HEIGHT };
	DWORD dwCurrentTime;//現在時刻
	DWORD dwExecLastTime;//最後に処理した時刻
	DWORD dwFrameCount;//フレームカウント
	DWORD dwFPSLastTime;//最後にFPSを計測した時刻

	RegisterClassEx(&wcex);//ウインドウクラスの登録

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);//クライアント領域を指定のサイズに調整

	//ウインドウを生成
	hWnd = CreateWindowEx(0,//拡張ウインドウスタイル
		process.CLASS_NAME,//ウインドウクラスの名前
		process.WINDOW_NAME,//ウインドウの名前
		WS_OVERLAPPEDWINDOW,//ウインドウスタイル
		CW_USEDEFAULT,//ウインドウ左上X座標
		CW_USEDEFAULT,//ウインドウ左上Y座標
		(rect.right - rect.left),//ウインドウの幅
		(rect.bottom - rect.top),//ウインドウの高さ
		nullptr,//親ウインドウのハンドル
		nullptr,//メニューハンドルまたは子ウインドウID
		hInstance,//インスタンスハンドル
		nullptr);//ウインドウの作成データ

	//初期化処理
	if (FAILED(manager.Init(hInstance, hWnd, TRUE)))
	{
		return -1;//初期化処理が失敗した場合
	}

	//ウインドウの表示
	ShowWindow(hWnd, nCmdShow);//ウインドウの表示状態の設定
	UpdateWindow(hWnd);//クライアント領域を更新

	//分解能を設定
	timeBeginPeriod(1);

	dwCurrentTime = 0;//初期化する
	dwExecLastTime = timeGetTime();//現在時刻を取得(保存)
	dwFrameCount = 0;
	dwFPSLastTime = timeGetTime();
	unsigned int FPS_TIME_SPACE = 500;

	//メッセージループ
	while (1)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) != 0)
		{
			//windowsの処理
			if (msg.message == WM_QUIT) //終了
			{
				break;//WM_QUITメッセージを受け取ったらメッセージループを抜ける
			}
			else
			{
				//メッセージの設定
				TranslateMessage(&msg);//仮想キーメッセージを文字メッセージへ変換
				DispatchMessage(&msg);//ウインドウプロシージャへメッセージを送出
			}
		}
		else
		{
			//DirectXの処理
			dwCurrentTime = timeGetTime(); //現在時刻を取得

			if ((dwCurrentTime - dwFPSLastTime) >= FPS_TIME_SPACE) //0.5秒経過
			{
				nCountFPS = (dwFrameCount * 1000) / (dwCurrentTime - dwFPSLastTime);//FPSを計測
				dwFPSLastTime = dwCurrentTime;//FPSを測定した時刻を保存
				dwFrameCount = 0;//フレームカウントをクリア
			}
			if ((dwCurrentTime - dwExecLastTime) >= (1000 / process.MAX_FRAME))
			{
				//時間経過
				dwExecLastTime = dwCurrentTime;//現在時刻を保存

				//更新処理
				manager.Update();

				//描画処理
				manager.Draw();

				dwFrameCount++;//フレームカウントを加算
			}
		}
	}

	//マネージャー解放処理
	manager.Uninit();

	UnregisterClass(process.CLASS_NAME, wcex.hInstance);//ウインドウクラスの登録を解除

	return(int)msg.wParam;
}

//===========================================================================================================
// ウインドウプロシージャ
//===========================================================================================================
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nID; //返り値格納用

	switch (uMsg)
	{
	case WM_DESTROY://ウインドウ破棄のメッセージ
		PostQuitMessage(0);//WM_QUITメッセージを送る
		break;

	case WM_KEYDOWN://キー押下のメッセージ

		switch (wParam)
		{
		case VK_ESCAPE://[ESC]キー入力

			nID = MessageBox(hWnd, "終了しますか？", "終了メッセージ", MB_YESNOCANCEL | MB_ICONQUESTION);//終了メッセージ

			if (nID == IDYES)//はい
			{
				DestroyWindow(hWnd);//ウインドウを破棄する(WM_DESTROYメッセージを送る)
				break;
			}
			break;
		case VK_F11:
			CProcess::ToggleFullscreen(hWnd);
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

//===========================================================================================================
// ウィンドウをフルスクリーンに変える処理
//===========================================================================================================
void CProcess::ToggleFullscreen(HWND hWnd)
{
	// 現在のウィンドウスタイルを取得
	DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);

	if (m_isFullscreen)
	{
		// ウィンドウモードに切り替え
		SetWindowLong(hWnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPos(hWnd, HWND_TOP, m_windowRect.left, m_windowRect.top,
			m_windowRect.right - m_windowRect.left, m_windowRect.bottom - m_windowRect.top,
			SWP_FRAMECHANGED | SWP_NOACTIVATE);
		ShowWindow(hWnd, SW_NORMAL);
	}
	else
	{
		// フルスクリーンモードに切り替え
		GetWindowRect(hWnd, &m_windowRect);
		SetWindowLong(hWnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
		SetWindowPos(hWnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
			SWP_FRAMECHANGED | SWP_NOACTIVATE);
		ShowWindow(hWnd, SW_MAXIMIZE);
	}

	m_isFullscreen = !m_isFullscreen;
}

