//==============================================================================================================================================
//
// スタートボタンに関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _STARTBUTTON_H_ //このマクロ定義がされなかったら
#define _STARTBUTTON_H_ //2重インクルード防止のマクロ定義

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "UI.h"

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CStartButton : public CUI
{
public:
	//======================================
	// 関数
	//======================================
	CStartButton();//コンストラクタ
	~CStartButton() override;//デストラクタ
	HRESULT Init() override;//初期化処理
	void Uninit() override;//終了処理
	void Update() override;//更新処理
	void Draw() override;//描画処理

private:
	//======================================
	// 列挙型定義
	//======================================

	//状態
	typedef enum
	{
		STAND_BY,//待機中
		BLINKING,//点滅
	}STATE;

	//======================================
	// 定数
	//======================================
	const float STAND_SPEED = 0.02f;//待機中の点滅速度

	//======================================
	// 変数
	//======================================
	STATE m_state;//状態
	bool m_bLightUp;//ライトアップフラグ
};

#endif
