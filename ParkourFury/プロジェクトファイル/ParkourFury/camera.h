//==============================================================================================================================================
//
// カメラに関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _CAMERA_H_ //このマクロ定義がされなかったら
#define _CAMERA_H_ //2重インクルード防止のマクロ定義

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "main.h"

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CCamera
{
public:
	//======================================
	// 列挙型定義
	//======================================

	//状態
	enum class STATE
	{
		NONE = 0,//停止
		LEFTTURN,//左回転
		RIGHTTURN,//右回転
		LOOP_TURN,//ループターン
		MAX
	};

	//======================================
	// 関数
	//======================================
	CCamera();//コンストラクタ
	~CCamera();//デストラクタ
	HRESULT Init();//初期化処理
	void Uninit();//終了処理
	void Update();//更新処理
	void RotationStage();//ステージカメラ回転処理
	void SetCamera();//カメラの設定

	//視点
	void SetPosV(const D3DXVECTOR3& pos) { m_posV = pos; }//設定
	D3DXVECTOR3& GetPosV() { return m_posV; }//取得

	//注視点
	void SetPosR(const D3DXVECTOR3& pos) { m_posR = pos; }//設定
	D3DXVECTOR3& GetPosR() { return m_posR; }//取得

	//角度
	void SetRot(const D3DXVECTOR3& rot) { m_rot = rot; }//設定
	D3DXVECTOR3& GetRot() { return m_rot; }//取得

	//状態
	void SetState(STATE state);//設定

	//カメラワーク情報
	void SetCameraWork(const D3DXVECTOR3& TargetPos_V, const D3DXVECTOR3& TargetPos_R, int nMoveFrame);//設定

	//現在の移動フレーム
	int GetMoveFrame() { return m_nMoveFrame; }//取得

	//揺れの情報
	void SetShake(int nFrame, int nShake)
	{
		m_nShakeFrame = nFrame;//揺れるフレーム数
		m_nMaxNumShake = nShake;//振れの量
	}

	//リセット
	void Reset();

	//定数取得
	float GetBossMinDistance() { return BOSS_MIN_DISTANCE; }//ボス戦カメラ距離最小値
	float GetBossMaxDistance() { return BOSS_MAX_DISTANCE; }//ボス戦カメラ距離最大値

	//======================================
	// 定数
	//======================================
	static inline const D3DXVECTOR3 FirstPosV = { 0.0f, 50.0f, -500.0f };

private:
	//======================================
	// 関数
	//======================================
	void Turn();//回転処理
	void Move();//移動処理
	void CameraWork();//カメラワーク処理
	void SetPosRPlayer();//プレイヤーに注視点を設定する処理

	//======================================
	// 定数
	//======================================
	const D3DXVECTOR3 VecU = { 0.0f, 1.0f, 0.0f };
	const int MOVE_FRAME = 90;//移動フレーム
	const float ADJUST_POS_SPEED = 10.0f;//カメラ座標調整速度
	const float MAX_ROLL = 0.05f;//回転速度
	const float MAX_STAGEROTATION = 0.1f;//ステージカメラ回転速度
	const float TITLE_POSV_Y = 1000.0f;//タイトル時の高さ
	const float TITLE_DISTANCE = 3000.0f;//タイトル時の視点から注視点までの距離
	const float TITLE_TURN_SPEED = 0.005f;//タイトル画面回転速度
	const float BOSS_MIN_DISTANCE = 650.0f;//ボス戦カメラ距離最小値
	const float BOSS_MAX_DISTANCE = 1000.0f;//ボス戦カメラ距離最大値
 
	//======================================
	// 変数
	//======================================
	D3DXVECTOR3 m_posV;//視点
	D3DXVECTOR3 m_posR;//注視点
	D3DXVECTOR3 m_rot;//角度
	D3DXVECTOR3 m_TargetRot;//目標の角度
	D3DXVECTOR3 m_move_V;//視点移動量
	D3DXVECTOR3 m_move_R;//注視点移動量
	D3DXMATRIX m_mtxProjection;//プロジェクション行列
	D3DXMATRIX m_mtxView;//ビュー行列
	STATE m_State;//状態
	int m_nShakeFrame;//揺れるフレーム数
	int m_nMaxNumShake;//揺れの量の最大値
	int m_nMoveFrame;//移動フレーム
	float m_fLength;//距離
	float m_fDistanceMag;//視点と注視点の距離倍率
	float m_fAddHeight;//視点の高さ加算量
	float m_fMoveRotY;//回転量
	bool m_bWork;//カメラワークフラグ
	bool m_bSetMove_V;//視点移動量設定フラグ
	bool m_bSetMove_R;//注視点移動量設定フラグ
};

#endif
