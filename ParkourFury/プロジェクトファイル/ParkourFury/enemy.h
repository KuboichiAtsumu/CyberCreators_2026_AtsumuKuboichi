//==============================================================================================================================================
//
// エネミーに関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _ENEMY_H_ //このマクロ定義がされなかったら
#define _ENEMY_H_ //2重インクルード防止のマクロ定義

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "objectX.h"

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CEnemy : public CObjectX
{
public:
	//======================================
	// 列挙型定義
	//======================================

	//タイプ
	enum class TYPE
	{
		NONE = 0,
		NORMAL,//ノーマル
		BALL,//ボール
		MOVE,//ムーブ
		BULLET,//バレット
		MAX
	};

	//======================================
	// 構造体定義
	//======================================

	//当たり判定用パラメータ
	struct CollisionParam
	{
		D3DXVECTOR3 pos;//座標
		D3DXVECTOR3 size;//サイズ
		CEnemy* pEnemy;//当たったエネミーの情報
	};

	//======================================
	// 関数
	//======================================
	CEnemy(int nPriority = static_cast<int>(Category::ENEMY));//コンストラクタ
	~CEnemy() override;//デストラクタ
	HRESULT Init() override;//初期化処理
	void Uninit() override;//終了処理
	void Release() override;//解放処理
	void Update() override;//更新処理
	void Draw() override;//描画処理
	virtual bool Collision(CollisionParam& aParam);//当たり判定
	static CEnemy* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, TYPE type);//生成処理

	//前のオブジェクト情報
	void SetPrevEnemy(CEnemy* pEnemy) { m_pPrev = pEnemy; }//設定
	CEnemy* GetPrevEnemy() { return m_pPrev; }//取得

	//次のオブジェクト情報
	void SetNextEnemy(CEnemy* pEnemy) { m_pNext = pEnemy; }//設定
	CEnemy* GetNextEnemy() { return m_pNext; }//取得

	//タイプ
	void SetEnemyType(TYPE type) { m_Type = type; }//設定
	TYPE GetEnemyType() { return m_Type; }//取得

	//総数
	static void ResetAll() { m_nAll = 0; }//リセット
	static int GetAll() { return m_nAll; }//取得

private:
	//======================================
	// 関数
	//======================================
	bool Collision_X(CollisionParam aParam);//X軸の当たり判定
	bool Collision_Y(CollisionParam aParam);//Y軸の当たり判定
	bool Collision_Z(CollisionParam aParam);//Z軸の当たり判定

	//======================================
	// モデルファイルパス
	//======================================
	static constexpr const char* MODEL_FILE[static_cast<int>(TYPE::MAX)] =
	{
		"",
		"data/MODEL/ENEMY/NormalEnemy.x",//ノーマル
		"data/MODEL/ENEMY/ball.x",//ボール
		"data/MODEL/ENEMY/MoveEnemy.x",//動く敵
		"data/MODEL/ENEMY/BulletEnemy.x",//弾を撃つ敵
	};

	//======================================
	// 変数
	//======================================
	static int m_nAll;//総数
	CEnemy* m_pPrev;//前のオブジェクト情報へのポインタ
	CEnemy* m_pNext;//次のオブジェクト情報へのポインタ
	TYPE m_Type;//種類
};

#endif
