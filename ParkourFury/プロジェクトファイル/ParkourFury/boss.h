//==============================================================================================================================================
//
// ボスに関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _BOSS_H_ //このマクロ定義がされなかったら
#define _BOSS_H_ //2重インクルード防止のマクロ定義

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "motion.h"

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CBoss : public CMotionCharacter
{
public:
	//======================================
	// 構造体定義
	//======================================

	//当たり判定用パラメータ
	struct CollisionParam
	{
		D3DXVECTOR3 pos;//座標
		D3DXVECTOR3 size;//サイズ
	};

	//======================================
	// 関数
	//======================================
	CBoss(int nPriority = static_cast<int>(Category::BOSS));//コンストラクタ
	~CBoss() override;//デストラクタ
	HRESULT Init() override;//初期化処理
	void Uninit() override;//終了処理
	void Release() override;//解放処理
	void Update() override;//更新処理
	void Draw() override;//描画処理
	static CBoss* Create(D3DXVECTOR3 pos);//生成処理
	bool Collision(CollisionParam& aParam);//当たり判定
	void DecreaseLife();//ライフ減少処理

	//HP
	int GetHP() { return m_nLife; }//現在のHP
	int GetMaxHP() { return MAX_LIFE; }//最大HP

private:
	//======================================
	// 列挙型定義
	//======================================

	//状態
	enum class STATE
	{
		NEUTRAL = 0,//通常
		ATTACK,//攻撃
		MAX
	};

	//攻撃タイプ
	enum class ATTACK_TYPE
	{
		BULLET_DEPTH = 0,//弾(奥)
		BULLET_WIDTH,//弾(横)
		BLOCK,//ブロック生成
		MAX
	};

	//攻撃パターン
	enum class ATTACK_PATTERN
	{
		BULLET_DEPTH = 0,//弾(奥)
		BULLET_WIDTH,//弾(横)
		WALL_BLOCK,//壁生成
		FORTH_BLOCK,//ブロック生成
		BULLET_AREA,//弾(特定の位置)
		MAX
	};

	//======================================
	// 構造体定義
	//======================================

	//攻撃情報
	struct AttackInfo
	{
		ATTACK_TYPE Type;//タイプ
		int nCreate;//生成数
		std::vector<D3DXVECTOR3> pos;//座標
		std::vector<D3DXVECTOR3> rot;//角度
		std::vector<D3DXVECTOR3> scale;//拡大率
	};

	//======================================
	// 関数
	//======================================
	void LoadAttackPattern();//攻撃パターン読込処理
	bool Collision_X(CollisionParam aParam);//X軸の当たり判定
	bool Collision_Y(CollisionParam aParam);//Y軸の当たり判定
	bool Collision_Z(CollisionParam aParam);//Z軸の当たり判定
	void SetPosition();//立ち位置の設定処理
	void Gravity();//重力処理
	void Move();//移動処理
	void Attack();//攻撃処理
	void SetAttackPattern();//攻撃パターン設定処理
	void BulletDepth();//弾(奥)の攻撃処理
	void BulletWidth();//弾(横)の攻撃処理
	void BlockCreate();//ブロック生成攻撃処理
	void CollisionBlock();//ブロックとの当たり判定
	void AngleModifying();//角度修正処理
	void SetCameraWorkInfo();//カメラワーク情報設定処理

	//======================================
	// 定数
	//======================================
	const int SETPOS_TIME = 240;//立ち位置変更の間隔
	const int ATTACK_TIME = 90;//攻撃間隔
	const int MAX_LIFE = 20;//最大ライフ
	const int NUM_ATTACK = 5;//攻撃の数
	const float GRAVITY_POWER = 0.5f;//重力の強さ
	const float MAX_GRAVITY = -10.0f;//重力の最大強度
	const float BULLET_RANGE = 200.0f;//弾の攻撃発生の範囲(プレイヤーが中心)
	const float BLOCK_RANGE_Y = 200.0f;//ブロック生成Y軸範囲
	const float BLOCK_RANGE_Z = 400.0f;//ブロック生成Z軸範囲
	const float MAX_BLOCK_SCALE = 2.0f;//生成ブロックの最大拡大率

	//======================================
	// 変数
	//======================================

	//攻撃関係
	std::vector<AttackInfo> m_AttackInfo;//攻撃の情報
	int m_nAttackPattern;//攻撃パターン

	//パラメータ
	STATE m_State;//状態
	int m_nLife;//ライフ
	int m_nSetCoolTime;//立ち位置変更までのクールタイム
	int m_nAttackCT;//攻撃までのクールタイム
	int m_nIdxPosition;//現在いるポイントのインデックス
	int m_nOldPosition;//前のポイントのインデックス
	float m_fInertia;//移動量減衰率
	bool m_bAttack;//攻撃フラグ
};

#endif
