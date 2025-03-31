//==============================================================================================================================================
//
// Joypad入力処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "joypad.h"//ジョイパッド

//===========================================================================================================
// 静的メンバ変数初期化
//===========================================================================================================
int CInputJoypad::m_nCntKey = 0;//長押し時間

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CInputJoypad::CInputJoypad() : m_JoyKeyState(), m_JoyKeyStateTrigger(), m_JoyKeyStateRepeat()
{
	//メンバ変数初期化
	m_Stick = {};//スティックの入力情報
	m_XInput = {};//コントローラーの状態
	m_aJoypadCurrentTime = {};//コントローラーの現在の入力時間
	m_aJoypadExecLastTime = {};//コントローラーの最後にtrueを返した時間
	m_fLStickAngle = 0.0f;//Lスティック角度
	m_fRStickAngle = 0.0f;//Rスティック角度

	for (int nCntStickType = 0; nCntStickType < STICKTYPE_MAX; nCntStickType++)
	{
		for (int nCntStickAngle = 0; nCntStickAngle < STICKANGLE_MAX; nCntStickAngle++)
		{
			m_aStickCurrentTime[nCntStickType][nCntStickAngle] = {};//スティックの現在の入力時間
			m_aStickExecLastTime[nCntStickType][nCntStickAngle] = {};//スティックの最後にtrueを返した時間
			m_bAngle[nCntStickType][nCntStickAngle] = false;//各スティック角度の入力フラグ
		}
	}
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CInputJoypad::~CInputJoypad()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CInputJoypad::Init()
{
	//メモリのクリア
	memset(&m_JoyKeyState, 0, sizeof(XINPUT_STATE));

	//メモリのクリア
	memset(&m_JoyKeyState, 0, sizeof(XINPUT_VIBRATION));

	//XInputのステートを設定(有効にする)
	XInputEnable(true);

	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CInputJoypad::Uninit()
{
	//長押し時間
	m_nCntKey = 0;

	//XInputのステートを設定(無効にする)
	XInputEnable(false);
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CInputJoypad::Update()
{
	XINPUT_STATE joykeyState; //ジョイパッドの入力情報

	//ジョイパッドの状態を取得
	if (XInputGetState(0, &joykeyState) == ERROR_SUCCESS)
	{
		WORD Button = joykeyState.Gamepad.wButtons;
		WORD OldButton = m_JoyKeyState.Gamepad.wButtons;
		m_JoyKeyStateTrigger.Gamepad.wButtons = Button & ~OldButton;//トリガー処理
		joykeyState.Gamepad.wButtons |= GetJoypadStick(m_JoyKeyState.Gamepad.sThumbLX, m_JoyKeyState.Gamepad.sThumbLY, (SHORT)DEADZONE);

		// 現在の時間を取得する
		m_aJoypadCurrentTime.Gamepad.wButtons = (WORD)timeGetTime();

		if (joykeyState.Gamepad.wButtons && ((m_aJoypadCurrentTime.Gamepad.wButtons - m_aJoypadExecLastTime.Gamepad.wButtons) > SPEED_REPEAT))
		{
			// 最後にtrueを返した時間を保存
			m_aJoypadExecLastTime.Gamepad.wButtons = m_aJoypadCurrentTime.Gamepad.wButtons;

			// コントローラーのリピート情報を保存
			m_JoyKeyStateRepeat = joykeyState;
		}
		else
		{
			// コントローラーのリピート情報を保存
			m_JoyKeyStateRepeat.Gamepad.wButtons = 0;
		}

		m_JoyKeyState = joykeyState;// プレス処理
	}

	//スティックの更新処理
	UpdateStick();
}

//===========================================================================================================
// スティック更新処理
//===========================================================================================================
void CInputJoypad::UpdateStick()
{
	float fX = 0.0f, fY = 0.0f; //スティックのX軸、Y軸

	//コントローラーの状態を取得
	if (XInputGetState(0, &m_XInput) == ERROR_SUCCESS)
	{
		for (int nCntStick = 0; nCntStick < STICKTYPE_MAX; nCntStick++)
		{
			switch (nCntStick)
			{
				//Lスティック
			case STICKTYPE_LEFT:
				fX = (GetXInputState()->Gamepad.sThumbLX);
				fY = (GetXInputState()->Gamepad.sThumbLY);

				//角度を取得
				m_Stick.afAngle[nCntStick] = FindAngleL(D3DXVECTOR3(fX, fY, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f)) * -1;
				break;

				//Rスティック
			case STICKTYPE_RIGHT:
				fX = (GetXInputState()->Gamepad.sThumbRX);
				fY = (GetXInputState()->Gamepad.sThumbRY);

				//角度を取得
				m_Stick.afAngle[nCntStick] = FindAngleR(D3DXVECTOR3(fX, fY, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f)) * -1;
				break;
			}

			//スティックの倒し具合を取得
			m_Stick.afTplDiameter[nCntStick] = fabsf(fX);

			if (m_Stick.afTplDiameter[nCntStick] < fabsf(fY))
			{
				m_Stick.afTplDiameter[nCntStick] = fabsf(fY);
			}

			m_Stick.afTplDiameter[nCntStick] /= 32768.0f; //倒している状態の初期値

			//方向入力フラグを初期化
			for (int nCntAngle = 0; nCntAngle < STICKANGLE_MAX; nCntAngle++)
			{
				m_bAngle[nCntStick][nCntAngle] = false;
			}

			if (m_Stick.afTplDiameter[nCntStick] > 0.1f)
			{
				//角度調節
				if (m_Stick.afAngle[nCntStick] < -D3DX_PI)
				{
					m_Stick.afAngle[nCntStick] = D3DX_PI - (D3DX_PI + m_Stick.afAngle[nCntStick]) * -1.0f;
				}

				//角度が四分割で上に位置する時、上フラグをtrueにする
				if ((m_Stick.afAngle[nCntStick] < D3DX_PI * -0.625f) || (m_Stick.afAngle[nCntStick] > D3DX_PI * 0.625f))
				{
					m_bAngle[nCntStick][STICKANGLE_UP] = true;
				}

				//角度が四分割で下に位置する時、下フラグをtrueにする
				if ((m_Stick.afAngle[nCntStick] > D3DX_PI * -0.375f) && (m_Stick.afAngle[nCntStick] < D3DX_PI * 0.375f))
				{
					m_bAngle[nCntStick][STICKANGLE_DOWN] = true;
				}

				//角度が四分割で右に位置する時、右フラグをtrueにする
				if ((m_Stick.afAngle[nCntStick] > D3DX_PI * 0.125f) && (m_Stick.afAngle[nCntStick] < D3DX_PI * 0.875f))
				{
					m_bAngle[nCntStick][STICKANGLE_RIGHT] = true;
				}

				//角度が四分割で左に位置する時、左フラグをtrueにする
				if ((m_Stick.afAngle[nCntStick] > D3DX_PI * -0.875f) && (m_Stick.afAngle[nCntStick] < D3DX_PI * -0.125f))
				{
					m_bAngle[nCntStick][STICKANGLE_LEFT] = true;
				}
			}

			//角度に応じた入力処理
			for (int nCntAngle = 0; nCntAngle < STICKANGLE_MAX; nCntAngle++)
			{
				//スティックのトリガー情報を保存
				m_Stick.abAngleTrigger[nCntStick][nCntAngle] = (m_Stick.abAnglePress[nCntStick][nCntAngle] ^ m_bAngle[nCntStick][nCntAngle]) & m_bAngle[nCntStick][nCntAngle];

				//スティックのリリース情報を保存
				m_Stick.abAngleRelease[nCntStick][nCntAngle] = (m_Stick.abAnglePress[nCntStick][nCntAngle] ^ m_bAngle[nCntStick][nCntAngle]) & m_bAngle[nCntStick][nCntAngle];

				//現在の時間を取得
				m_aStickCurrentTime[nCntStick][nCntAngle] = timeGetTime();

				if (m_bAngle[nCntStick][nCntAngle] && ((m_aStickCurrentTime[nCntStick][nCntAngle] - m_aStickExecLastTime[nCntStick][nCntAngle]) > (unsigned)SPEED_REPEAT))
				{
					//最後にtrueを返した時間を保存
					m_aStickExecLastTime[nCntStick][nCntAngle] = m_aStickCurrentTime[nCntStick][nCntAngle];

					//スティックのリピート情報を保存
					m_Stick.abAngleRepeat[nCntStick][nCntAngle] = m_bAngle[nCntStick][nCntAngle];
				}
				else
				{
					//スティックのリピート情報を保存
					m_Stick.abAngleRepeat[nCntStick][nCntAngle] = 0;
				}

				//スティックのプレス情報を保存
				m_Stick.abAnglePress[nCntStick][nCntAngle] = m_bAngle[nCntStick][nCntAngle];
			}
		}
	}
}

//===========================================================================================================
// Lスティックの2点の角度を求める
//===========================================================================================================
float CInputJoypad::FindAngleL(D3DXVECTOR3 pos, D3DXVECTOR3 TargetPos)
{
	//角度
	m_fLStickAngle = atan2f(TargetPos.y - pos.y, TargetPos.x - pos.x);

	m_fLStickAngle -= (D3DX_PI * 0.5f);
	m_fLStickAngle *= -1.0f;

	return m_fLStickAngle;
}

//===========================================================================================================
// Rスティックの2点の角度を求める
//===========================================================================================================
float CInputJoypad::FindAngleR(D3DXVECTOR3 pos, D3DXVECTOR3 TargetPos)
{
	//角度
	m_fRStickAngle = atan2f(TargetPos.y - pos.y, TargetPos.x - pos.x);

	m_fRStickAngle -= (D3DX_PI * 0.5f);
	m_fRStickAngle *= -1.0f;

	return m_fRStickAngle;
}

//===========================================================================================================
// プレス情報取得
//===========================================================================================================
bool CInputJoypad::GetPress(JOYKEY key)
{
	return (m_JoyKeyState.Gamepad.wButtons & (0x01 << key)) ? true : false;
}

//===========================================================================================================
// トリガー情報取得
//===========================================================================================================
bool CInputJoypad::GetTrigger(JOYKEY key)
{
	return (m_JoyKeyStateTrigger.Gamepad.wButtons & (0x01 << key)) ? true : false;
}

//===========================================================================================================
// リピート情報取得
//===========================================================================================================
bool CInputJoypad::GetRepeat(JOYKEY key)
{
	//キーを押している間
	if (GetPress(key))
	{
		if (GetTrigger(key))
		{
			m_nCntKey = 0;
			return true;
		}

		m_nCntKey++; //押している間をカウント

		if (m_nCntKey >= SPEED_REPEAT) //押し続ける間
		{
			m_nCntKey = 0;
			return true;
		}
	}

	return false;
}

//===========================================================================================================
// コントローラーの入力情報取得
//===========================================================================================================
XINPUT_STATE* CInputJoypad::GetXInputState()
{
	return &m_XInput;
}

//===========================================================================================================
// スティック情報取得
//===========================================================================================================
CInputJoypad::STICKINPUT CInputJoypad::GetStick()
{
	return m_Stick;
}

//===========================================================================================================
// スティック情報取得
//===========================================================================================================
WORD CInputJoypad::GetJoypadStick(SHORT sThumbX, SHORT sThumbY, SHORT sDeadZone)
{
	WORD wButtons = 0;

	if (sThumbY >= sDeadZone)
	{
		wButtons |= XINPUT_GAMEPAD_DPAD_UP;
	}
	else if (sThumbY <= -sDeadZone)
	{
		wButtons |= XINPUT_GAMEPAD_DPAD_DOWN;
	}

	if (sThumbX <= -sDeadZone)
	{
		wButtons |= XINPUT_GAMEPAD_DPAD_LEFT;
	}
	else if (sThumbX >= sDeadZone)
	{
		wButtons |= XINPUT_GAMEPAD_DPAD_RIGHT;
	}

	return wButtons;
}
