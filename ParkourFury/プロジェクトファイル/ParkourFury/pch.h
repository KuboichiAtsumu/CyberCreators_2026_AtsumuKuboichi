//==============================================================================================================================================
//
// プリコンパイルヘッダー
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _PCH_H_
#define _PCH_H_

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#define DIRECTINPUT_VERSION (0x0800) //ビルド時の警告対処用マクロ
#include <dinput.h>
#include <xaudio2.h>
#include <Xinput.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h> 
#include <time.h>
#include <assert.h>
#include <vector>
#include <stdexcept>
#include <cstdlib>
#include <crtdbg.h>
#include <functional>
#include <fstream>
#include <string>
#include <sstream>
#include <random>
//#include <EffekseerForDXLib_170e_324b/VC/Effekseer.h>
//#include <EffekseerForDXLib_170e_324b/VC/EffekseerRendererDX9.h>

//===========================================================================================================
// マクロ定義
//===========================================================================================================
#ifdef _DEBUG
#define FIND_MEM_LEAKS
#endif // _DEBUG

#ifdef FIND_MEM_LEAKS
#define NEW new (_NORMAL_BLOCK , __FILE__, __LINE__)
#else
#define NEW new
#endif // FIND_MEM_LEAKS

#define FVF_VERTEX_2D	(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define FVF_VERTEX_3D	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define MAX_VER			(4)//頂点数
#define MAX_FRAME		(60)//最大フレーム数
#define SCREEN_WIDTH	(1280.0f)//ウインドウの幅
#define SCREEN_HEIGHT	(720.0f)//ウインドウの高さ

//===========================================================================================================
// ライブラリのリンク
//===========================================================================================================
#pragma comment(lib,"d3d9.lib")//描画処理用
#pragma comment(lib,"d3dx9.lib")//[d3d9.lib]の拡張ライブラリ
#pragma comment(lib,"dxguid.lib")//DirectXコンポネート(部品)使用
#pragma comment(lib,"winmm.lib")//システム時刻取得用
#pragma comment(lib,"dinput8.lib")//入力処理用
#pragma comment(lib,"xinput.lib")//ジョイパッド処理用
//#pragma comment(lib,"Effekseer.lib")
//#pragma comment(lib,"EffekseerRendererDX9.lib")

//===========================================================================================================
// 構造体
//===========================================================================================================

//頂点情報(2D)
typedef struct
{
	D3DXVECTOR3 pos;//頂点座標
	float rhw;//座標変換用係数(1.0fで固定)
	D3DCOLOR col;//頂点カラー
	D3DXVECTOR2 tex;//テクスチャ座標
}VERTEX_2D;

//頂点情報(3D)
typedef struct
{
	D3DXVECTOR3 pos;//頂点座標
	D3DXVECTOR3 nor;//法線ベクトル
	D3DCOLOR col;//頂点カラー
	D3DXVECTOR2 tex;//テクスチャ座標
}VERTEX_3D;

#endif
