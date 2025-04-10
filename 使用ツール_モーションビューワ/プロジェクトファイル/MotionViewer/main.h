//=======================================================================================================================================================================================================================
//
// スクリーンの処理に関するヘッダーファイル
// Author : Atsumu Kuboichi
//
//=======================================================================================================================================================================================================================
#ifndef _MAIN_H_ //このマクロ定義がされなかったら
#define _MAIN_H_ //2重インクルード防止のマクロ定義

#define DIRECTINPUT_VERSION (0x0800) //ビルド時の警告対処用マクロ
#include <Windows.h>
#include "d3dx9.h"		//描画処理用
#include "dinput.h"		//入力処理用
#include "xaudio2.h"	//サウンド処理用
#include "Xinput.h"		//ジョイパッド処理用
#include "input.h"		//キー入力処理用
#include "debugproc.h"	//デバッグ表示用

//=======================================================================================================================================================================================================================
// ライブラリのリンク
//=======================================================================================================================================================================================================================
#pragma comment(lib,"d3d9.lib")		//描画処理用
#pragma comment(lib,"d3dx9.lib")	//[d3d9.lib]の拡張ライブラリ
#pragma comment(lib,"dxguid.lib")	//DirectXコンポネート(部品)使用
#pragma comment(lib,"winmm.lib")	//システム時刻取得用
#pragma comment(lib,"dinput8.lib")	//入力処理用
#pragma comment(lib,"xinput.lib")	//ジョイパッド処理用

//=======================================================================================================================================================================================================================
// マクロ定義
//=======================================================================================================================================================================================================================
#define SCREEN_WIDTH	(1280)					//ウインドウの幅
#define SCREEN_HEIGHT	(720)					//ウインドウの高さ
#define MAX_VER			(4)						//頂点の数
#define MAX_FRAME		(60)					//フレーム数
#define MAX_TIMEF		(500)					//FPS計測時間間隔
#define MAX_TEXT		(256)					//テキスト読み込み数
#define MAX_CHAR		(512)					//FPS表示の最大文字数
#define CLASS_NAME		"WindowClass"			//ウインドウクラスの名前
#define WINDOW_NAME		"モーションビューワ"	//ウインドウの名前
#define PLAYER_TXT		"data\\SetCharacter.txt"//プレイヤー情報のtxtファイル
#define FVF_VERTEX_2D	(D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define FVF_VERTEX_3D	(D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1)

//=======================================================================================================================================================================================================================
// 画面の種類
//=======================================================================================================================================================================================================================
typedef enum
{
	MODE_GAME,		//ゲーム画面
	MODE_MAX
}MODE;

//=======================================================================================================================================================================================================================
// プロトタイプ宣言
//=======================================================================================================================================================================================================================
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);//ウインドウプロシージャ
LPDIRECT3DDEVICE9 GetDevice(void);												//デバイスの取得
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);						//初期化処理
void Uninit(void);																//終了処理
void Update(void);																//更新処理 
void Draw(void);																//描画処理
void SetMode(MODE mode);														//モードの設定
void onwire(void);																//ワイヤーフレームの有効化	
void offwire(void);																//ワイヤーフレームの無効化
void ToggleFullscreen(HWND hWnd);												//ウィンドウをフルスクリーンにする方法

//=======================================================================================================================================================================================================================
// 頂点情報[2D]の構造体
//=======================================================================================================================================================================================================================
typedef struct
{
	D3DXVECTOR3 pos;//頂点座標
	float rhw;		//座標変換用係数(1.0fで固定)
	D3DCOLOR col;	//頂点カラー
	D3DXVECTOR2 tex;//テクスチャ座標
}VERTEX_2D;

//=======================================================================================================================================================================================================================
// 頂点情報[3D]の構造体
//=======================================================================================================================================================================================================================
typedef struct
{
	D3DXVECTOR3 pos;//頂点座標
	D3DXVECTOR3 nor;//法線ベクトル
	D3DCOLOR col;	//頂点カラー
	D3DXVECTOR2 tex;//テクスチャ座標
}VERTEX_3D;

#endif
