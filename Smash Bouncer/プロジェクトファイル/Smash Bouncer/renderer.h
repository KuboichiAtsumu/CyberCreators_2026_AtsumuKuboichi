//==============================================================================================================================================
//
// レンダラーに関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _RENDERER_H_ //このマクロ定義がされなかったら
#define _RENDERER_H_ //2重インクルード防止のマクロ定義

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "main.h"//メイン
#include <functional>

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CRenderer
{
public:
	//======================================
	// 関数
	//======================================
	CRenderer();//コンストラクタ
	~CRenderer();//デストラクタ
	HRESULT Init(HWND hWnd, BOOL bWindow);//初期化処理
	void Uninit();//終了処理
	void Update();//更新処理
	void Draw();//描画処理
	LPDIRECT3DDEVICE9 GetDevice();//3Dデバイスの取得
	void onwire();//ワイヤーフレーム有効
	void offwire();//ワイヤーフレーム無効

private:
	//======================================
	// 変数
	//======================================
	LPDIRECT3D9 m_pD3D;//Direct3D
	LPDIRECT3DDEVICE9 m_pD3DDevice;//Direct3Dデバイス
	bool m_bStop;//一時停止フラグ
};

#endif
