//===========================================================================================================================================================================================
//
// Joypad入力処理に関するヘッダーファイル
// Author : Atsumu Kuboichi
//
//===========================================================================================================================================================================================
#ifndef _JOYPAD_H_  //このマクロ定義がされなかったら
#define _JOYPAD_H_  //2重インクルード防止のマクロ定義

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "input.h"

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CInputJoypad : public CInput
{
public:
	//======================================
	// 列挙型定義
	//======================================
	
	//キーの種類
	typedef enum
	{
		JOYKEY_UP = 0,//↑
		JOYKEY_DOWN,//↓
		JOYKEY_LEFT,//←
		JOYKEY_RIGHT,//→
		JOYKEY_START,//START
		JOYKEY_BACK,//BACK
		JOYKEY_L3,//左スティック押込み
		JOYKEY_R3,//右スティック押込み
		JOYKEY_LB,//LB
		JOYKEY_RB,//RB
		JOYKEY_LT,//LT
		JOYKEY_RT,//RT
		JOYKEY_A,//A
		JOYKEY_B,//B
		JOYKEY_X,//X
		JOYKEY_Y,//Y
		JOYKEY_MAX
	}JOYKEY;

	//スティックの番号
	typedef enum
	{
		STICKTYPE_LEFT = 0,//左スティック
		STICKTYPE_RIGHT,//右スティック
		STICKTYPE_MAX
	}STICKTYPE;

	//スティックの方向
	typedef enum
	{
		STICKANGLE_UP = 0,//上
		STICKANGLE_DOWN,//下
		STICKANGLE_LEFT,//左
		STICKANGLE_RIGHT,//右
		STICKANGLE_MAX,
	}STICKANGLE;

	//======================================
	// 構造体
	//======================================

	//スティックの入力情報
	typedef struct
	{
		float afTplDiameter[STICKTYPE_MAX];//スティックの倒し具合
		float afAngle[STICKTYPE_MAX];//スティックの角度
		bool abAnglePress[STICKTYPE_MAX][STICKANGLE_MAX];//スティックの方向プレス情報
		bool abAngleTrigger[STICKTYPE_MAX][STICKANGLE_MAX];//スティックの方向トリガー情報
		bool abAngleRepeat[STICKTYPE_MAX][STICKANGLE_MAX];//スティックの方向リピート情報
		bool abAngleRelease[STICKTYPE_MAX][STICKANGLE_MAX];//スティックの方向リリース情報
	}STICKINPUT;

	//======================================
	// 関数
	//======================================
	CInputJoypad();//コンストラクタ
	~CInputJoypad();//デストラクタ
	HRESULT Init();//初期化処理
	void Uninit();//終了処理
	void Update();//更新処理
	bool GetPress(JOYKEY key);//プレス情報取得
	bool GetTrigger(JOYKEY key);//トリガー情報取得
	bool GetRepeat(JOYKEY key);//リピート情報取得
	STICKINPUT GetStick();//スティックの情報を取得
	float FindAngleL(D3DXVECTOR3 pos, D3DXVECTOR3 TargetPos);//Lスティックの2点の角度を求める関数
	float FindAngleR(D3DXVECTOR3 pos, D3DXVECTOR3 TargetPos);//Rスティックの2点の角度を求める関数
	float GetAngleL() { return m_fLStickAngle; }//Lスティックの角度を取得
	float GetAngleR() { return m_fRStickAngle; }//Rスティックの角度を取得

private:
	//======================================
	// 関数
	//======================================
	XINPUT_STATE* GetXInputState();//コントローラーの入力情報の取得
	void UpdateStick();//スティックの更新処理
	WORD GetJoypadStick(SHORT sThumbX, SHORT sThumbY, SHORT sDeadZone);

	//======================================
	// 変数
	//======================================
	const int SPEED_REPEAT = 5;//リピート間隔
	const int DEADZONE = 1;//デッドゾーン
	static int m_nCntKey;//長押し時間計測
	XINPUT_STATE m_JoyKeyState;//プレス情報
	XINPUT_STATE m_JoyKeyStateTrigger;//トリガー情報
	XINPUT_STATE m_JoyKeyStateRepeat;//リピート情報
	XINPUT_STATE m_aJoypadCurrentTime;//コントローラーの現在の入力時間
	XINPUT_STATE m_aJoypadExecLastTime;//コントローラーの最後にtrueを返した時間
	D3DXVECTOR3 m_JoyStickPos;//スティックの傾き
	DWORD m_aStickCurrentTime[STICKTYPE_MAX][STICKANGLE_MAX];//スティックの現在の入力時間
	DWORD m_aStickExecLastTime[STICKTYPE_MAX][STICKANGLE_MAX];//スティックの最後にtrueを返した時間
	STICKINPUT m_Stick;//スティックの入力情報
	XINPUT_STATE m_XInput;//コントローラーの状態
	float m_fLStickAngle;//Lスティック角度
	float m_fRStickAngle;//Rスティック角度
	bool m_bAngle[STICKTYPE_MAX][STICKANGLE_MAX];//各スティック角度の入力フラグ
};

#endif
