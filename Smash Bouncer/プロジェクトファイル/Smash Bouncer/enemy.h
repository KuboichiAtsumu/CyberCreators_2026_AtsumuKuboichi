//==============================================================================================================================================
//
// エネミーの処理に関するヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _ENEMY_H_
#define _ENEMY_H_

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "objectX.h"//Xファイルオブジェクト

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CEnemy : public CObjectX
{
public:
	//======================================
	// 関数
	//======================================
	CEnemy(int nPriority = 3);//コンストラクタ
	~CEnemy() override;//デストラクタ
	HRESULT Init() override;//初期化処理
	void Uninit() override;//終了処理
	void Release() override;//解放処理
	void Update() override;//更新処理
	void Draw() override;//描画処理
	static CEnemy* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nType);//生成処理
	virtual bool Collision(D3DXVECTOR3 pos, D3DXVECTOR3 size);//当たり判定
	virtual void Hit();//ヒット処理
	virtual void Move();//移動処理
	CEnemy* GetPrevEnemy() { return m_pPrev; }//前のオブジェクト情報取得処理
	CEnemy* GetNextEnemy() { return m_pNext; }//次のオブジェクト情報取得処理
	static int GetAllENemy() { return m_nAllEnemy; }//敵の総数取得処理
	int GetInterval() {	return m_nIntervalShot; }//発射時間取得処理
	int GetCntShot() { return m_nCntShot; }//弾発射カウント取得処理
	void SetPrevEnemy(CEnemy* pEnemy) { m_pPrev = pEnemy; }//前のオブジェクト情報設定処理
	void SetNextEnemy(CEnemy* pEnemy) { m_pNext = pEnemy; }//次のオブジェクト情報設定処理
	void SetInterval() { m_nIntervalShot = rand() % DIFFERENCE_INTERVAL + MIN_INTERVAL; }//発射時間設定処理
	void SetCntShot(int nCnt) { m_nCntShot = nCnt; }//弾発射カウントを設定処理

private:
	//======================================
	// 列挙型定義
	//======================================

	//エネミータイプ
	typedef enum
	{
		GREEN_SLIME,//スライム(緑色)
		BLUE_SLIME,//スライム(青色)
		WHITE_GHOST,//おばけ(白色)
		BLACK_GHOST,//おばけ(黒色)
		RED_BAT,//コウモリ(赤色)
		PURPLE_BAT,//コウモリ(紫色)
		MID_BOSS,//中ボス
		BOSS,//ボス
		MAX_ENEMY_TYPE
	}ENEMYTYPE;

	//エネミーの状態
	typedef enum
	{
		ENEMY_STATE_NORMAL,//通常
		ENEMY_STATE_DAMAGE,//ダメージ
		ENEMY_STATE_MAX
	}ENEMY_STATE;

	//各エネミータイプのライフ
	static constexpr int ENEMY_LIFE[MAX_ENEMY_TYPE] =
	{
		5,
		5,
		5,
		5,
		5,
		5,
		15,
		30,
	};

	//======================================
	// 関数
	//======================================
	void SetEnemyType(ENEMYTYPE type) { m_EnemyType = type; }//タイプ設定

	//======================================
	// 変数
	//======================================
	static constexpr int MIN_INTERVAL = 2;//発射時間最短時間
	static constexpr int DIFFERENCE_INTERVAL = 3;//発射時間の範囲
	static constexpr float TURN_SPEED = 0.05f;//回転速度
	static constexpr float BULLET_SPEED = 4.0f;//弾の発射速度倍率
	static constexpr float MOVE_SPEED = 0.5f;//移動速度
	static constexpr float MOVE_WIDTH = 20.0f;//移動幅
	CEnemy* m_pPrev;//前のオブジェクトのポインタ
	CEnemy* m_pNext;//次のオブジェクトのポインタ
	ENEMYTYPE m_EnemyType;//タイプ
	ENEMY_STATE m_State;//状態
	D3DXVECTOR3 m_FirstPos;//初期位置
	static int m_nAllEnemy;//敵の総数
	int m_nIntervalShot;//弾の発射間隔
	int m_nCntShot;//弾発射までのカウント
	int m_nCntState;//状態カウント
};

#endif
