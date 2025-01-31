//==============================================================================================================================================
//
// エリアに関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _AREA_H_ //このマクロ定義がされなかったら
#define _AREA_H_ //2重インクルード防止のマクロ定義

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "objectX.h"

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CArea : public CObjectX
{
public:
	//======================================
	// 列挙型定義
	//======================================

	//タイプ
	enum class TYPE
	{
		NONE = 0,
		BOSS,//ボス
		MINI_GAME,//ミニゲーム
		CHASE,//チェイス
		MAX
	};

	//======================================
	// 関数
	//======================================
	CArea(int nPriority = static_cast<int>(Category::AREA));//コンストラクタ
	~CArea() override;//デストラクタ
	HRESULT Init() override;//初期化処理
	void Uninit() override;//終了処理
	void Release() override;//解放処理
	void Update() override;//更新処理
	void Draw() override;//描画処理
	static CArea* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, TYPE type);//生成処理
	virtual bool Collision(D3DXVECTOR3 pos);//当たり判定

	//前のオブジェクト情報
	void SetPrevArea(CArea* pArea) { m_pPrev = pArea; }//設定
	CArea* GetPrevArea() { return m_pPrev; }//決定

	//次のオブジェクト情報
	void SetNextArea(CArea* pArea) { m_pNext = pArea; }//設定
	CArea* GetNextArea() { return m_pNext; }//取得

	//ブロックタイプ
	void SetAreaType(TYPE type) { m_Type = type; }//設定
	TYPE GetAreaType() { return m_Type; }//取得

private:
	//======================================
	// 変数
	//======================================
	CArea* m_pPrev;//前のオブジェクト情報へのポインタ
	CArea* m_pNext;//次のオブジェクト情報へのポインタ
	TYPE m_Type;//種類
};

#endif
