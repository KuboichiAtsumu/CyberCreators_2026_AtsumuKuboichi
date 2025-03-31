//==============================================================================================================================================
//
// バットの処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "bat.h"//コウモリ
#include "bullet.h"//バレット
#include "player.h"//プレイヤー
#include "effect3D.h"//3Dエフェクト

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CBat::CBat(int nPriority) : CEnemy(nPriority)
{
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CBat::~CBat()
{
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CBat::Update()
{
	//プレイヤーとの距離
	float fLength = 0.0f;

	//情報を取得
	D3DXVECTOR3 pos = GetPos();//位置
	D3DXVECTOR3 rot = GetRot();//向き
	D3DXVECTOR3 PlayerPos = {};

	//指定した順番にオブジェクト情報を取得
	for (int nCnt = 0; nCnt < MAX_PRIORITY; nCnt++)
	{
		//先頭
		CObject* pObj = CObject::GetTop(nCnt);

		//オブジェクト情報がnullptrになるまで繰り返す
		while (pObj != nullptr)
		{
			//次のオブジェクト情報を取得
			CObject* pNext = pObj->GetNext();

			//オブジェクトタイプの取得
			CObject::TYPE type = pObj->GetType();

			//タイプがプレイヤーの場合
			if (type == CObject::TYPE::PLAYER)
			{
				//オブジェクト情報をプレイヤー情報に変換
				CPlayer* pPlayer = (CPlayer*)pObj;

				//プレイヤーの位置情報を取得
				PlayerPos = pPlayer->GetPos();

				//各座標のプレイヤーとバレットの距離を求める
				D3DXVECTOR3 Distance{ pos.x - PlayerPos.x, pos.y - PlayerPos.y, pos.z - PlayerPos.z };

				//プレイヤーまでの角度を求める
				float fAngle = atan2f(Distance.x, Distance.z);

				//プレイヤーまでの距離を求める
				fLength = sqrtf(powf(Distance.x, 2.0f) + powf(Distance.z, 2.0f)) * 0.5f;

				//エネミーをプレイヤーの方向に向ける
				rot.y = fAngle;
			}

			//オブジェクト情報を次の情報に変更する
			pObj = pNext;
		}
	}

	//情報の設定
	SetRot(rot);//向き

	//弾発射をカウント
	int nCntShot = GetCntShot();//現在のカウントを取得
	SetCntShot(nCntShot + 1);//カウントを加算して設定

	//カウントが最大まで達した場合
	if (nCntShot >= CProcess::MAX_FRAME * GetInterval())
	{
		//弾の移動量を設定
		D3DXVECTOR3 BulletMove = D3DXVECTOR3(sinf(rot.y + D3DX_PI) * (fLength * 0.03f), 10.0f, cosf(rot.y + D3DX_PI) * (fLength * 0.03f));

		//弾を生成
		CBullet::Create(pos, BulletMove, CBullet::BULLETTYPE::LAUNCH_ENEMY);

		//ターゲットを生成
		PlayerPos.y += 0.03f;
		CEffect3D::Create(CEffect3D::EFFECT_3D_TARGET, PlayerPos, { 0.0f, 0.0f, 0.0f }, { 30.0f, 0.0f, 30.0f });

		//カウントリセット
		SetCntShot(0);

		//次の弾発射までの時間を設定
		SetInterval();
	}

	//エネミーの更新処理
	CEnemy::Update();
}
