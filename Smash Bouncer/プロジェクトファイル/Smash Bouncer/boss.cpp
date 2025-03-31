//==============================================================================================================================================
//
// ボスの処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "boss.h"//ボス
#include "player.h"//プレイヤー
#include "bullet.h"//バレット

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CBoss::CBoss(int nPriority) : CEnemy(nPriority)
{
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CBoss::~CBoss()
{
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CBoss::Update()
{
	//現在のステージと座標が一致しない場合
	if (CGame::GetNumStage() != GetPostStageID())
	{
		//更新せずに処理を抜ける
		return;
	}

	//プレイヤーとの距離
	float fLength = 0.0f;

	//情報を取得
	D3DXVECTOR3 Pos = GetPos();//位置
	D3DXVECTOR3 Rot = GetRot();//向き

	//指定した順番にオブジェクト情報を取得する
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
				D3DXVECTOR3 PlayerPos = pPlayer->GetPos();

				//各座標のプレイヤーとバレットの距離を求める
				D3DXVECTOR3 Distance{ Pos.x - PlayerPos.x, Pos.y - PlayerPos.y, Pos.z - PlayerPos.z };

				//プレイヤーまでの角度を求める
				float fAngle = atan2f(Distance.x, Distance.z);

				//プレイヤーまでの距離を求める
				fLength = sqrtf(powf(Distance.x, 2.0f) + powf(Distance.z, 2.0f)) * 0.5f;

				//エネミーをプレイヤーの方向に向ける
				Rot.y = fAngle;
			}

			//オブジェクト情報を次の情報に変更する
			pObj = pNext;
		}
	}

	//情報の設定
	SetRot(Rot);//向き

	//弾発射をカウント
	int nCntShot = GetCntShot();//現在のカウントを取得
	SetCntShot(nCntShot + 1);//カウントを加算して設定

	//カウントが最大まで達した場合
	if (nCntShot >= CProcess::MAX_FRAME * GetInterval())
	{
		//バウンドバレット発射処理
		ShotBound(Pos, Rot);

		//カウントリセット
		SetCntShot(0);

		//次の弾発射までの時間を設定
		SetInterval();
	}

	//移動処理
	CEnemy::Move();

	//エネミーの更新処理
	CEnemy::Update();
}

//===========================================================================================================
// バウンドバレット発射処理
//===========================================================================================================
void CBoss::ShotBound(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	//弾の移動量
	D3DXVECTOR3 BulletMove = {};

	for (int nCnt = 0; nCnt < DIFFUSION_BULLET + 1; nCnt++)
	{
		//1番目のバレットの場合
		if (nCnt == 0)
		{
			//弾の移動量を設定
			BulletMove = D3DXVECTOR3(sinf(rot.y + D3DX_PI) * BULLET_SPEED, 0.0f, cosf(rot.y + D3DX_PI) * BULLET_SPEED);

			//弾を生成
			CBullet::Create(pos, BulletMove, CBullet::BULLETTYPE::BOUND_INVINCIBLE);
		}
		//2番目以降のバレットの場合
		else
		{
			//弾の移動量を設定
			BulletMove = D3DXVECTOR3(sinf(rot.y - DISTANCE_BULLET * (float)nCnt + D3DX_PI) * BULLET_SPEED,
				0.0f,
				cosf(rot.y - DISTANCE_BULLET * (float)nCnt + D3DX_PI) * BULLET_SPEED);

			//弾を生成
			CBullet::Create(pos, BulletMove, CBullet::BULLETTYPE::BOUND_INVINCIBLE);

			//弾の移動量を設定
			BulletMove = D3DXVECTOR3(sinf(rot.y + DISTANCE_BULLET * (float)nCnt + D3DX_PI) * BULLET_SPEED,
				0.0f,
				cosf(rot.y + DISTANCE_BULLET * (float)nCnt + D3DX_PI) * BULLET_SPEED);

			//弾を生成
			CBullet::Create(pos, BulletMove, CBullet::BULLETTYPE::BOUND_INVINCIBLE);
		}
	}
}
