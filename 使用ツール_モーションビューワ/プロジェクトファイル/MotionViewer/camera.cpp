//=======================================================================================================================================================================================================================
//
// カメラの処理
// Author : Atsumu Kuboichi
//
//=======================================================================================================================================================================================================================
#include "camera.h"
#include "game.h"
#include "player.h"

//=======================================================================================================================================================================================================================
// グローバル変数
//=======================================================================================================================================================================================================================
Camera g_Camera;	//カメラの情報

//=======================================================================================================================================================================================================================
// 初期化処理
//=======================================================================================================================================================================================================================
void InitCamera(void)
{
	g_Camera.posV = D3DXVECTOR3(0.0f, GAMECAMERA_POSV_Y, GAMECAMERA_POSV_Z);//視点
	g_Camera.posR = D3DXVECTOR3(0.0f, GetPlayer()->vtxMax.y + 10.0f, 0.0f);//注視点
	g_Camera.vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);//ベクトル(上方向)
	g_Camera.move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);//移動量
	g_Camera.rot = D3DXVECTOR3(atan2f(g_Camera.posV.y - g_Camera.posR.y, g_Camera.posV.z - g_Camera.posR.z), 0.0f, 0.0f);//向き
	g_Camera.fLength = sqrtf(g_Camera.posV.y * g_Camera.posV.y + g_Camera.posV.z * g_Camera.posV.z);//視点から注視点までの距離
}

//=======================================================================================================================================================================================================================
// 更新処理
//=======================================================================================================================================================================================================================
void UpdateCamera(void)
{
	DIMOUSESTATE pMouse = GetMouse();//マウスの情報を取得

	////視点から注視点までの距離
	//g_Camera.posV.y += sinf(g_Camera.rot.x + D3DX_PI) * (pMouse.lZ * MOUSE_SINCE);
	//g_Camera.posV.z += cosf(g_Camera.rot.x + D3DX_PI) * (pMouse.lZ * MOUSE_SINCE);

	////視点から注視点までの距離
	//g_Camera.fLength = sqrtf(g_Camera.posV.y * g_Camera.posV.y + g_Camera.posV.z * g_Camera.posV.z);

	//左クリックを押している場合
	if (GetMousePress(MOUSE_LEFT))
	{
		//右クリックを押している場合
		if (GetMousePress(MOUSE_RIGHT))
		{//注視点の位置を移動
			g_Camera.posR.x += sinf(g_Camera.rot.y + D3DX_PI) * pMouse.lX * MOUSE_SINCE * 4.0f;
			g_Camera.posR.z -= cosf(g_Camera.rot.y + D3DX_PI) * pMouse.lY * MOUSE_SINCE * 4.0f;
		}
		else
		{//マウスの動きに合わせて視点を動かす
			g_Camera.rot.y += pMouse.lX * MOUSE_SINCE * CAMERA_ROLL;
		}

		//視点の位置の更新
		g_Camera.posV.x = sinf(g_Camera.rot.y + D3DX_PI) * g_Camera.fLength + g_Camera.posR.x;
		g_Camera.posV.z = cosf(g_Camera.rot.y + D3DX_PI) * g_Camera.fLength + g_Camera.posR.z;
	}
	//ホイールがクリックされている場合
	else if (GetMousePress(MOUSE_WHEEL))
	{
		//Y座標を移動
		g_Camera.posV.y -= pMouse.lY * MOUSE_SINCE * 4.0f;
	}
	
	////回転数値の調整
	//if (g_Camera.rot.y > D3DX_PI)//3.14以上
	//{
	//	g_Camera.rot.y = -D3DX_PI;//-3.14
	//}
	//if (g_Camera.rot.y < -D3DX_PI)//-3.14以下
	//{
	//	g_Camera.rot.y = D3DX_PI;//3.14
	//}
}

//=======================================================================================================================================================================================================================
// 設定処理
//=======================================================================================================================================================================================================================
void SetCamera(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	//デバイスの取得

	//プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&g_Camera.mtxProjection);

	//プロジェクションマトリックスを生成
	D3DXMatrixPerspectiveFovLH(&g_Camera.mtxProjection, D3DXToRadian(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 1.0f, 5000.0f);

	//プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &g_Camera.mtxProjection);

	//ビューマトリックスの初期化
	D3DXMatrixIdentity(&g_Camera.mtxView);

	//ビューマトリックスの生成
	D3DXMatrixLookAtLH(&g_Camera.mtxView, &g_Camera.posV, &g_Camera.posR, &g_Camera.vecU);

	//ビューマトリックスの設定
	pDevice->SetTransform(D3DTS_VIEW, &g_Camera.mtxView);
}

//=======================================================================================================================================================================================================================
// 情報の取得
//=======================================================================================================================================================================================================================
Camera* GetCamera(void)
{
	return &g_Camera;
}
