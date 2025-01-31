//==============================================================================================================================================
//
// プレイヤーに関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _PLAYER_H_ //このマクロ定義がされなかったら
#define _PLAYER_H_ //2重インクルード防止のマクロ定義

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "motion.h"
#include "block.h"

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CPlayer : public CMotionCharacter
{
public:
	//======================================
	// 関数
	//======================================
	CPlayer(int nPriority = 3);//コンストラクタ
	~CPlayer() override;//デストラクタ
	HRESULT Init() override;//初期化処理
	void Uninit() override;//終了処理
	void Release() override;//解放処理
	void Update() override;//更新処理
	void SetMotionState(int nState) override;//モーション状態設定処理
	void SetPartsState() override;//パーツごとの状態設定処理
	void Draw() override;//描画処理
	static CPlayer* Create(const D3DXVECTOR3& pos);//生成処理
	float GetHeat() { return m_fHeat; }//ヒート量取得

	//キル数
	void IncreaseKill() { m_nKill++; }//加算
	int GetKill() { return m_nKill; }//取得

	//======================================
	// 定数
	//======================================
	static constexpr float MAX_HEAT = 100.0f;//最大ヒート量

private:
	//======================================
	// 列挙型定義
	//======================================

	//パーツ
	enum class PARTS
	{
		CENTER = 0,//センター
		STOMACH,//腹
		BODY,//体
		FACE,//顔
		ArmUR,//右腕上
		ArmDR,//右腕下
		HandR,//右手
		ArmUL,//左腕上
		ArmDL,//左下腕
		HandL,//左手
		LegUR,//右脚上
		LegDR,//右脚下
		FeetR,//右足
		LegUL,//左脚上
		LegDL,//左脚下
		FeetL,//左足
		MAX
	};

	//状態
	enum class STATE
	{
		NEUTRAL = 0,//通常
		LANDING_MOVE,//地上移動
		JUMP,//一段目ジャンプ
		DOUBLE_JUMP,//二段目ジャンプ
		FALL,//落下
		WALL_SLIDE,//壁ずり
		WALL_KICK,//壁キック
		DASH,//ダッシュ
		CLIFF_GRAB,//崖掴まり
		SHOT,//発射
		SLIDING,//スライディング
		DAMAGE,//ダメージ
		MAX
	};

	//壁の判定
	enum class WallState
	{
		NONE = 0,
		SLIDE,//壁ずり
		KICK,//壁キック
		CLIFF,//崖掴まり
		MAX
	};

	//触れた壁の線上
	enum class WallLine
	{
		NONE = 0,
		XY,//XY線上
		YZ,//YZ線上
	};

	//======================================
	// 関数
	//======================================

	//操作関係
	void Controll();//操作処理
	void Jump(D3DXVECTOR3& move, float fDiaJump);//ジャンプ
	void Move(D3DXVECTOR3& move, float fDiaSpeed, float fTargetRotY);//移動処理
	void Dash(float& fDiaSpeed, float& fFirstDiaSpeed);//ダッシュ処理
	void Shot();//発射処理
	void Sliding(D3DXVECTOR3& move);//スライディング処理
	void Landing(CBlock::CollisionParam& aParam);//着地中処理
	void Falling(CBlock::CollisionParam& aParam, WallState wallstate);//落下中処理
	WallState WallKick(CBlock* pBlock, CBlock::CollisionParam& aParam);//壁キック処理
	void ReflectAngleCalculate(CBlock* pBlock, CBlock::CollisionParam& aParam, WallLine Line, WallState& wallstate);//反射ベクトル計算処理

	//パラメータ
	void SetWallKickTurn();//壁キック回転速度設定処理
	void IncreaseHeat(float fAdd);//ヒート量増加処理
	void DecreaseHeat();//ヒート量減小処理
	void AddCombo();//コンボ数加算処理
	void ResetCombo();//コンボ数リセット処理

	//エフェクト
	void OccurHeatEffect();//ヒートエフェクト
	void OccurSmoke();//煙
	void DisplayEffect2D();//2Dエフェクト表示処理

	//モデル情報
	void Flash();//点滅処理

	//当たり判定
	void Collision();
	void CollisionBlock();//ブロックとの当たり判定
	WallState CollisionWallKick(CBlock::CollisionParam& aParam);//壁キック判定処理
	void CollisionEnemy();//エネミーとの当たり判定
	void CollisionBoss();//ボスとの当たり判定
	void CollisionBossAttack();//ボスの攻撃との当たり判定
	void CollisionMiniGameArea();//ミニゲームエリア
	void CollisionChaseArea();//チェイスエリア
	void CollisionBossArea();//ボスエリア
	void CollisionArea();//全エリア

	//リスポーン
	void RespawnChaseArea();//チェイスエリアリスポーン処理

	//デバッグ用処理
#ifdef _DEBUG
	void Warp();//ワープ処理
#endif // _DEBUG

	//======================================
	// 定数
	//======================================
	const D3DXVECTOR2 DISPLAY_EFFECT2D = { 330.0f, 550.0f };//2Dエフェクト表示座標
	const int JUMP_TIMES = 2;//ジャンプ回数
	const int DASH_FRAME = 5;//ダッシュのフレーム
	const int DOUBLE_PUSH = 13;//2回入力受付フレーム
	const int RIGOR_CLIFF = 120;//崖に掴まれる時間
	const int POSSIBLE_FALL = 30;//崖掴みから操作可能になるまでの時間
	const int SHOT_SPEED = 10;//弾の発射間隔
	const int DEC_HEAT_CT = 8;//ヒート量減少クールタイム
	const int DAMAGE_TIME = 30;//ダメージ状態の時間
	const int COMBO_TIME = 30;//コンボ継続フレーム
	const int NUM_CREATE_PARTICLE = 50;//パーティクル生成数
	const int MAX_COMBO = 3;//最大コンボ数
	const int DECREASE_COMBO_FRAME = 90;//コンボ数リセットまでのフレーム数
	const float DASH_SPEED = 8.0f;//ダッシュの速度倍率
	const float MAX_TURN = 1.0f;//回転速度
	const float BASE_INERTIA = 0.4f;//減衰強度基準値
	const float JUMP_INERTIA = 1.0f;//ジャンプ減衰量
	const float MAX_GRAVITY = -10.0f;//重力上限
	const float BULLET_SPEED = 15.0f;//弾の速度
	const float ACCEL_SLIDING = 10.0f;//スライディングの加速度
	const float MAX_KICKPOWER_WIDTH = 8.5f;//壁キックのX軸の強さ最大値
	const float MAX_KICKPOWER_HEIGHT = 75.0f;//壁キックのY軸の強さ最大値
	const float SLIDE_POWER = 2.0f;//壁ずりの重力の強さ
	const float RANGE_EFFECT = 10.0f;//ヒートエフェクト発生範囲
	const float RANGE_EFFECT2D_ROT = 0.3f;//2Dエフェクト傾き範囲

	//======================================
	// 変数
	//======================================
	STATE m_State;//状態
	int m_nCntState;//状態カウンタ
	int m_nCntDash;//ダッシュ中のフレームカウンタ
	int m_nCntDoublePush;//2回入力フレームカウンタ
	int m_nCntJump;//ジャンプカウンタ
	int m_nDecTenCT;//ヒート量減少クールタイム
	int m_nComboTime;//コンボ継続フレーム
	int m_nKill;//キル数
	int m_nCombo;//コンボ数
	int m_nDecComboFrame;//コンボ数減少カウントフレーム
	float m_fHeat;//ヒート量
	float m_fInertia;//減衰強度
	float m_fWallKickTurn;//壁キックの回転速度
	float m_fTargetRot_Y;//目標の角度
	bool m_bShooting;//射撃フラグ
	bool m_bBossCreate;//ボス生成済みフラグ
};

#endif