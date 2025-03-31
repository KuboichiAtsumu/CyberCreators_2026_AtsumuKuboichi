//==============================================================================================================================================
//
// メイン処理に関するヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _MAIN_H_ //このマクロ定義がされなかったら
#define _MAIN_H_ //2重インクルード防止のマクロ定義
#define DIRECTINPUT_VERSION (0x0800) //ビルド時の警告対処用マクロ

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include <Windows.h>
#include "d3dx9.h" //描画処理用
#include "dinput.h" //入力処理用
#include "xaudio2.h" //サウンド処理用
#include "Xinput.h" //ジョイパッド処理用
#include <stdio.h>
#include <iostream>
#include <stdlib.h> 
#include <time.h>

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#ifdef _DEBUG
#define FIND_MEM_LEAKS
#endif // _DEBUG

#ifdef FIND_MEM_LEAKS
#define NEW new (_NORMAL_BLOCK , __FILE__, __LINE__)
#else
#define NEW new
#endif // FIND_MEM_LEAKS

//===========================================================================================================
// ライブラリのリンク
//===========================================================================================================
#pragma comment(lib,"d3d9.lib") //描画処理用
#pragma comment(lib,"d3dx9.lib") //[d3d9.lib]の拡張ライブラリ
#pragma comment(lib,"dxguid.lib") //DirectXコンポネート(部品)使用
#pragma comment(lib,"winmm.lib") //システム時刻取得用
#pragma comment(lib,"dinput8.lib") //入力処理用
#pragma comment(lib,"xinput.lib") //ジョイパッド処理用

//===========================================================================================================
// クラス
//===========================================================================================================
class CProcess
{
public:
	//======================================
	// 関数
	//======================================
	CProcess();//コンストラクタ
	~CProcess();//デストラクタ
	static void ToggleFullscreen(HWND hWnd);//フルスクリーン処理

	//======================================
	// 変数
	//======================================
	const char* WINDOW_NAME = "Bound Smash";//ウィンドウネーム
	static constexpr const char* CLASS_NAME = "WindowClass";//ウインドウクラスの名前
	static constexpr int FVF_VERTEX_2D = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
	static constexpr int FVF_VERTEX_3D = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1;
	static constexpr int MAX_TEXT = 256;//テキスト文字数
	static constexpr int MAX_VER = 4;//頂点数
	static constexpr int MAX_FRAME = 60;//フレーム数
	static constexpr int SCREEN_WIDTH = 1280;//ウインドウの幅
	static constexpr int SCREEN_HEIGHT = 720;//ウインドウの高さ

private:
	//======================================
	// 変数
	//======================================
	static RECT m_windowRect;//ウィンドウ切り替え用変数
	static bool m_isFullscreen;//ウィンドウ切り替え用フラグ
};

//===========================================================================================================
// 構造体
//===========================================================================================================

//=======================================================
// 頂点情報(2D)
//=======================================================
typedef struct
{
	D3DXVECTOR3 pos; //頂点座標
	float rhw; //座標変換用係数(1.0fで固定)
	D3DCOLOR col; //頂点カラー
	D3DXVECTOR2 tex; //テクスチャ座標
}VERTEX_2D;

//=======================================================
// 頂点情報(3D)
//=======================================================
typedef struct
{
	D3DXVECTOR3 pos;//頂点座標
	D3DXVECTOR3 nor;//法線ベクトル
	D3DCOLOR col;	//頂点カラー
	D3DXVECTOR2 tex;//テクスチャ座標
}VERTEX_3D;

//===========================================================================================================
// プロトタイプ宣言
//===========================================================================================================
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);//ウインドウプロシージャ

#endif
