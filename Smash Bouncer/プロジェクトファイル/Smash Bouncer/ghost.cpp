//==============================================================================================================================================
//
// ゴーストの処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "ghost.h"//ゴーストヘッダー
#include "bullet.h"//バレットヘッダー
#include "pointmanager.h"//ポイントマネージャー
#include "effect3D.h"//3Dエフェクト

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CGhost::CGhost(int nPriority) : CEnemy(nPriority)
{
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CGhost::~CGhost()
{
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CGhost::Update()
{
	//弾発射をカウント
	int nCntShot = GetCntShot();//現在のカウントを取得
	SetCntShot(nCntShot + 1);//カウントを加算して設定

	//カウントが最大まで達した場合
	if (nCntShot >= CProcess::MAX_FRAME * GetInterval())
	{
		//ポイントの数を取得
		int nPoint = GetAllEachStageID(CGame::GetNumStage(), CObject::SHOTPOINT);

		//ランダムで発射位置を設定
		int nTarget = rand() % nPoint;

		//先頭のオブジェクト情報を取得
		CPoint* pPoint = CGame::GetPointManager()->GetTop();

		//オブジェクト情報がnullptrになるまで繰り返す
		while (pPoint != nullptr)
		{
			//次のオブジェクト情報を取得
			CPoint* pNext = pPoint->GetNextPoint();

			//ポイントがエネミーと同じステージにある場合
			//発射位置のポイントのIDとターゲット番号が一致する場合
			if (pPoint->GetPostStageID() == GetPostStageID() && pPoint->GetEachStageID() == nTarget)
			{
				//発射位置設定
				SetPoint(pPoint);
				break;
			}
		
			//オブジェクト情報を次の情報に変更する
			pPoint = pNext;
		}

		//カウントリセット
		SetCntShot(0);

		//次の弾発射までの時間を設定
		SetInterval();
	}

	//エネミーの更新処理
	CEnemy::Update();
}

//===========================================================================================================
// 発射位置設定
//===========================================================================================================
void CGhost::SetPoint(CPoint* pPoint)
{
	//ポイントとの距離
	float fLength = 0.0f;

	//情報を取得
	D3DXVECTOR3 pos = GetPos();//位置
	D3DXVECTOR3 rot = GetRot();//向き

	//ポイントの位置情報を取得
	D3DXVECTOR3 PointPos = pPoint->GetPos();

	//各座標のポイントとバレットの距離を求める
	D3DXVECTOR3 Distance{ pos.x - PointPos.x, pos.y - PointPos.y, pos.z - PointPos.z };

	//ポイントまでの角度を求める
	float fAngle = atan2f(Distance.x, Distance.z);

	//ポイントまでの距離を求める
	fLength = sqrtf(powf(Distance.x, 2.0f) + powf(Distance.z, 2.0f)) * 0.5f;

	//エネミーをポイントの方向に向ける
	rot.y = fAngle;

	//情報の設定
	SetRot(rot);//向き

	//弾の移動量を設定
	D3DXVECTOR3 BulletMove = D3DXVECTOR3(sinf(rot.y + D3DX_PI) * (fLength * 0.03f), 10.0f, cosf(rot.y + D3DX_PI) * (fLength * 0.03f));

	//弾を生成
	CBullet::Create(pos, BulletMove, CBullet::BULLETTYPE::LAUNCH_ENEMY);

	//ターゲットを生成
	PointPos.y += 0.03f;
	CEffect3D::Create(CEffect3D::EFFECT_3D_TARGET, PointPos, { 0.0f, 0.0f, 0.0f }, { 30.0f, 0.0f, 30.0f });

	//カウントリセット
	SetCntShot(0);
}
