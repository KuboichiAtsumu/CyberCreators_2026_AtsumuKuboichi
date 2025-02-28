//==============================================================================================================================================
//
// 3D背景に関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _BACK_3D_H_ //このマクロ定義がされなかったら
#define _BACK_3D_H_ //2重インクルード防止のマクロ定義

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "objectX.h"

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CBack3D : public CObjectX
{
public:
	//======================================
	// 列挙型定義
	//======================================

	//タイプ
	enum class TYPE
	{
		NONE = 0,
		SKY,//ステージ背景
		MAX
	};

	//======================================
	// 関数
	//======================================
	CBack3D(int nPriority = static_cast<int>(Category::BACK_3D));//コンストラクタ
	~CBack3D() override;//デストラクタ
	HRESULT Init() override;//初期化処理
	void Uninit() override;//終了処理
	void Release() override;//解放処理
	void Update() override;//更新処理
	void Draw() override;//描画処理
	static CBack3D* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, TYPE type);//生成処理
	static void Load();//読込処理

private:
	//======================================
	// 変数
	//======================================
	TYPE m_Type;//タイプ
};

#endif
