//==============================================================================================================================================
//
// カメラの処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "camera.h"//カメラ
#include "manager.h"//マネージャー
#include "keyboard.h"//キーボード
#include "player.h"//プレイヤー
#include "zone.h"//ゾーン

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CCamera::CCamera()
{
	//メンバ変数初期化
	m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);//上方向
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);//向き
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);//移動量
	m_posV = FirstPosV;//視点
	m_fLength = sqrtf(powf(m_posV.y, 2.0f) + powf(m_posV.z, 2.0f)) * 0.5f;//距離
	m_posR = D3DXVECTOR3(0.0f, 0.0f, 0.0f);//注視点
	m_nShakeFrame = 0;//揺れるフレーム数
	m_nMaxNumShake = 0;//揺れの量の最大値
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CCamera::~CCamera()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CCamera::Init()
{
	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CCamera::Uninit()
{
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CCamera::Update()
{
#ifdef _DEBUG
	//移動処理
	Move();
#endif // _DEBUG

	//ゲーム中の場合
	if (!CManager::GetScene()->GetEditMode())
	{
		//指定した順番にオブジェクト情報を取得する
		for (int nCnt = 0; nCnt < MAX_PRIORITY; nCnt++)
		{
			//先頭のオブジェクト情報を取得
			CObject* pObj = CObject::GetTop(nCnt);

			//オブジェクト情報がnullptrになるまで繰り返す
			while (pObj != nullptr)
			{
				//次のオブジェクト情報を取得
				CObject* pNext = pObj->GetNext();

				//オブジェクトタイプ情報を取得
				CObject::TYPE type = pObj->GetType();

				//オブジェクトタイプがプレイヤーの場合
				//ゲームの状態が移動中の場合
				if (type == CObject::PLAYER && CGame::GetGameState() == CGame::STATE_MOVE)
				{
					//プレイヤーの位置を参照した視点・注視点設定
					SetPlayerPos(pObj);
				}

				//オブジェクトタイプがゾーンの場合
				//ゲームの状態がゲーム中の場合
				if (type == CObject::ZONE && CGame::GetGameState() == CGame::STATE_GAME)
				{
					//ゾーンの位置を参照した視点・注視点設定
					SetZonePos(pObj);
				}

				//オブジェクト情報を次の情報に変更する
				pObj = pNext;
			}
		}
	}
}

//===========================================================================================================
// 移動処理
//===========================================================================================================
void CCamera::Move()
{
	DIMOUSESTATE pMouse = CManager::GetMouse()->Get();	//マウスの情報を取得

	//左クリックを押している場合
	if (CManager::GetMouse()->GetPress(CInputMouse::MOUSE_LEFT))
	{//マウスの動きに合わせて視点を動かす
		m_rot.y += 0.05f * pMouse.lX * 0.1f;
		m_move.y -= 50.0f * pMouse.lY * 0.1f;
	}

	//ホイールの動きに合わせてカメラを引く
	m_posV.x += 5.0f * sinf(m_rot.y) * pMouse.lZ * 0.1f;
	m_posV.y += 5.0f * pMouse.lZ * 0.1f;
	m_posV.z += 5.0f * cosf(m_rot.y) * pMouse.lZ * 0.1f;

	//上移動(Z軸)
	if (CManager::GetKeyboard()->GetPress(DIK_UP))//↑キーが押された
	{
		//左上移動(Z軸)
		if (CManager::GetKeyboard()->GetPress(DIK_LEFT))//←キーが押された
		{
			//移動量を加算
			m_move.x += sinf(m_rot.y + D3DX_PI * -0.25f) * MAX_SPEED;//X軸
			m_move.z += cosf(m_rot.y + D3DX_PI * -0.25f) * MAX_SPEED;//Z軸
		}
		//右上移動(Z軸)
		else if (CManager::GetKeyboard()->GetPress(DIK_RIGHT))//→キーが押された
		{
			//移動量を加算
			m_move.x += sinf(m_rot.y + D3DX_PI * 0.25f) * MAX_SPEED;//X軸
			m_move.z += cosf(m_rot.y + D3DX_PI * 0.25f) * MAX_SPEED;//Z軸
		}
		//真上移動(Z軸)
		else
		{
			//移動量を加算
			m_move.x -= sinf(m_rot.y + D3DX_PI) * MAX_SPEED;//X軸
			m_move.z -= cosf(m_rot.y + D3DX_PI) * MAX_SPEED;//Z軸
		}
	}
	//下移動(Z軸)
	else if (CManager::GetKeyboard()->GetPress(DIK_DOWN))//↓キーが押された
	{
		//左下移動(Z軸)
		if (CManager::GetKeyboard()->GetPress(DIK_LEFT))//←キーが押された
		{
			//移動量を加算
			m_move.x += sinf(m_rot.y + D3DX_PI * -0.75f) * MAX_SPEED;//X軸
			m_move.z += cosf(m_rot.y + D3DX_PI * -0.75f) * MAX_SPEED;//Z軸
		}
		//右下移動(Z軸)
		else if (CManager::GetKeyboard()->GetPress(DIK_RIGHT))//→キーが押された
		{
			//移動量を加算
			m_move.x += sinf(m_rot.y + D3DX_PI * 0.75f) * MAX_SPEED;//X軸
			m_move.z += cosf(m_rot.y + D3DX_PI * 0.75f) * MAX_SPEED;//Z軸
		}
		//真下移動(Z軸)
		else
		{
			//移動量を加算
			m_move.x += sinf(m_rot.y + D3DX_PI) * MAX_SPEED;//X軸
			m_move.z += cosf(m_rot.y + D3DX_PI) * MAX_SPEED;//Z軸
		}
	}
	//左移動
	else if (CManager::GetKeyboard()->GetPress(DIK_LEFT))//←キーが押された
	{
		//移動量を加算
		m_move.x += sinf(m_rot.y + D3DX_PI * -0.5f) * MAX_SPEED;//X軸
		m_move.z += cosf(m_rot.y + D3DX_PI * -0.5f) * MAX_SPEED;//Z軸
	}
	//右移動
	else if (CManager::GetKeyboard()->GetPress(DIK_RIGHT))//→キーが押された
	{
		//移動量を加算
		m_move.x += sinf(m_rot.y + D3DX_PI * 0.5f) * MAX_SPEED;//X軸
		m_move.z += cosf(m_rot.y + D3DX_PI * 0.5f) * MAX_SPEED;//Z軸
	}

	//移動量の加算
	m_posV += m_move;

	//移動量を減衰
	m_move -= m_move;

	//注視点の位置の更新
	m_posR.x = sinf(m_rot.y) * m_fLength + m_posV.x;
	m_posR.z = cosf(m_rot.y) * m_fLength + m_posV.z;
}

//===========================================================================================================
// プレイヤーの位置を参照した視点・注視点設定
//===========================================================================================================
void CCamera::SetPlayerPos(CObject* pObj)
{
	//オブジェクト情報をプレイヤー情報に変換
	CPlayer* pPlayer = (CPlayer*)pObj;

	//プレイヤーの位置情報を保存
	D3DXVECTOR3 PlayerPos = pPlayer->GetPos();

	//プレイヤーがカメラより右側にいる場合
	if (PlayerPos.x >= m_posV.x)
	{
		//プレイヤーに合わせて視点・注視点を更新
		m_posV.x = PlayerPos.x;//視点
		m_posR.x = PlayerPos.x;//注視点
	}
}

//===========================================================================================================
// ゾーンの位置を参照した視点・注視点設定
//===========================================================================================================
void CCamera::SetZonePos(CObject* pObj)
{
	//オブジェクト情報をプレイヤー情報に変換
	CZone* pZone = (CZone*)pObj;

	//ゾーンタイプがステージ区分の場合
	//ゾーンのIDと現在のステージが一致する場合
	if (pZone->GetZoneType() == CZone::STAGE
		&& pZone->GetID() == CGame::GetNumStage())
	{
		//ゾーンの位置情報を取得
		D3DXVECTOR3 ZonePos = pZone->GetPos();

		//カメラがゾーンの中心より左側にある場合
		if (m_posV.x < ZonePos.x)
		{
			m_posV.x += ADJUST_POS_SPEED;
			m_posR.x += ADJUST_POS_SPEED;

			//視点の位置がゾーンの右側を超えないように調整
			if (m_posV.x > ZonePos.x)
			{
				m_posV.x = ZonePos.x;
			}

			//注視点の位置がゾーンの右側を超えないように調整
			if (m_posR.x > ZonePos.x)
			{
				m_posR.x = ZonePos.x;
			}
		}
	}
}

//===========================================================================================================
// カメラの設定
//===========================================================================================================
void CCamera::SetCamera()
{
	//ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();//デバイスのポインタ
	D3DXVECTOR3 adjust = { 0.0f, 0.0f, 0.0f };//揺れの量

	//プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxProjection);

	//プロジェクションマトリックスを生成
	D3DXMatrixPerspectiveFovLH(&m_mtxProjection, D3DXToRadian(45.0f), (float)CProcess::SCREEN_WIDTH / (float)CProcess::SCREEN_HEIGHT, 1.0f, 5000.0f);

	//プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &m_mtxProjection);

	//ビューマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxView);

	//フレーム数が0以上の場合
	if (m_nShakeFrame > 0)
	{
		//フレーム数を減少
		m_nShakeFrame--;

		//揺れの量の最大値が1以上の場合
		if (m_nMaxNumShake >= 1)
		{
			//揺れの量を設定
			adjust.x = (float)(rand() % (m_nMaxNumShake * 2) - m_nMaxNumShake);
			adjust.y = (float)(rand() % (m_nMaxNumShake * 2) - m_nMaxNumShake);
			adjust.z = (float)(rand() % (m_nMaxNumShake * 2) - m_nMaxNumShake);
		}
	}

	//揺れの量を加算した視点と注視点の値を保存
	D3DXVECTOR3 posV = m_posV + adjust;
	D3DXVECTOR3 posR = m_posR + adjust;

	//ビューマトリックスの生成
	D3DXMatrixLookAtLH(&m_mtxView, &posV, &posR, &m_vecU);

	//ビューマトリックスの設定
	pDevice->SetTransform(D3DTS_VIEW, &m_mtxView);
}
