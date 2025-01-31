//==============================================================================================================================================
//
// ギミックに関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _GIMMICK_H_ //このマクロ定義がされなかったら
#define _GIMMICK_H_ //2重インクルード防止のマクロ定義

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "objectX.h"

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CGimmick : public CObjectX
{
public:
	//======================================
	// 構造体
	//======================================

	//当たり判定用パラメータ
	struct CollisionParam
	{
		D3DXVECTOR3 pos;//座標
		D3DXVECTOR3 oldpos;//更新前の座標
		D3DXVECTOR3 rot;//角度
	};

	//======================================
	// 列挙型定義
	//======================================

	//タイプ
	enum class TYPE
	{
		NONE = 0,
		PLAYER_SPAWNER,//プレイヤースポーン位置
		ROTATION_ZONE,//ステージ回転位置
		BOSS_SPAWNER,//ボススポーン位置
		BOSS_MOVE,//ボスの移動ポイント
		BALL_SPAWNER,//ボールスポーン位置
		BALL_MOVE,//ボール移動ポイント
		MAX
	};

	//======================================
	// 関数
	//======================================
	CGimmick(int nPriority = static_cast<int>(Category::GIMMICK));//コンストラクタ
	~CGimmick() override;//デストラクタ
	HRESULT Init() override;//初期化処理
	void Uninit() override;//終了処理
	void Release() override;//解放処理
	void Update() override;//更新処理
	void Draw() override;//描画処理
	static CGimmick* Create(D3DXVECTOR3 pos, D3DXVECTOR3 scale, TYPE type);//生成処理
	virtual bool Collision(CollisionParam& aParam);//当たり判定

	//前のギミック情報
	void SetPrev(CGimmick* pPrev) { m_pPrev = pPrev; }//設定
	CGimmick* GetPrev() { return m_pPrev; }//取得

	//次のギミック情報
	void SetNext(CGimmick* pNext) { m_pNext = pNext; }//設定
	CGimmick* GetNext() { return m_pNext; }//取得

	//タイプ情報
	TYPE GetType() { return m_Type; }//取得

private:
	//======================================
	// 変数
	//======================================
	CGimmick* m_pPrev;//自分の前のギミック情報
	CGimmick* m_pNext;//自分の次のギミック情報
	TYPE m_Type;//タイプ
};

#endif
