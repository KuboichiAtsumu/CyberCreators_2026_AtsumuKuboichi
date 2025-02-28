//==============================================================================================================================================
//
// 空に関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _SKY_H_ //このマクロ定義がされなかったら
#define _SKY_H_ //2重インクルード防止のマクロ定義

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "back_3D.h"

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CSky : public CBack3D
{
public:
	//======================================
	// 関数
	//======================================
	CSky(int nPriority = static_cast<int>(Category::BACK_3D));//コンストラクタ
	~CSky() override;//デストラクタ
	HRESULT Init() override;//初期化処理
	void Uninit() override;//終了処理
	void Release() override;//解放処理
	void Update() override;//更新処理
	void Draw() override;//描画処理

private:
	//======================================
	// 定数
	//======================================
	static constexpr float SCALE = 400.0f;
	const float TURN_SPEED = 0.005f;
};

#endif
