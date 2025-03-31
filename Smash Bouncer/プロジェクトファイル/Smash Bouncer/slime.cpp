//==============================================================================================================================================
//
// スライムの処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "slime.h"//スライム
#include "bullet.h"//バレット

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CSlime::CSlime(int nPriority) : CEnemy(nPriority)
{
	//メンバ変数初期化
	m_bTurn = false;//回転判定フラグ
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CSlime::~CSlime()
{
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CSlime::Update()
{
	//エネミーの更新処理
	CEnemy::Update();

	//ローカル変数宣言
	static float fTargetRot;//目標の向き
	int nCntShot = GetCntShot();//現在のカウントを取得

	//エネミーの現在の向き情報を取得
	D3DXVECTOR3 SlimeRot = GetRot();

	//回転判定がfalseの場合
	if (!m_bTurn)
	{
		//カウントが最大まで達した場合
		if (nCntShot >= CProcess::MAX_FRAME * GetInterval())
		{
			//目標の向きをランダムで設定
			fTargetRot = SHOT_ANGLE[rand() % SHOT_PATTERN];

			//回転判定をtrueにする
			m_bTurn = true;
		}
		//カウントが最大まで達していない場合
		else
		{
			//弾発射をカウント
			SetCntShot(nCntShot + 1);//カウントを加算して設定
		}
	}
	//回転判定がtrueの場合
	else
	{
		//目標の向きの値よりもプレイヤーの向きの値のほうが大きい場合
		if (SlimeRot.y - fTargetRot > 0)
		{
			//エネミーを回転させる
			SlimeRot.y -= TURN_SPEED;

			//目標の向きに到達した場合
			if (SlimeRot.y - fTargetRot <= 0.0f)
			{
				//目標の向きを超えないように調整
				SlimeRot.y = fTargetRot;

				//弾の移動量を設定
				D3DXVECTOR3 BulletMove = D3DXVECTOR3(sinf(SlimeRot.y + D3DX_PI) * BULLET_SPEED, 0.0f, cosf(SlimeRot.y + D3DX_PI) * BULLET_SPEED);

				//敵の位置情報を取得
				D3DXVECTOR3 SlimePos = GetPos();

				//発射位置の高さを真ん中に上げる
				SlimePos.y += GetSize().y * 0.5f;

				//弾を生成
				CBullet::Create(SlimePos, BulletMove, CBullet::BULLETTYPE::BOUND_ENEMY);

				//回転判定をfalseにする
				m_bTurn = false;

				//カウントリセット
				SetCntShot(0);

				//次の弾発射までの時間を設定
				SetInterval();
			}
		}
		//プレイヤーの向きの値よりも目標の向きの値のほうが大きい場合
		else
		{
			//エネミーを回転させる
			SlimeRot.y += TURN_SPEED;

			//目標の向きに到達した場合
			if (SlimeRot.y - fTargetRot >= 0.0f)
			{
				//目標の向きを超えないように調整
				SlimeRot.y = fTargetRot;

				//弾の移動量を設定
				D3DXVECTOR3 BulletMove = D3DXVECTOR3(sinf(SlimeRot.y + D3DX_PI) * BULLET_SPEED, 0.0f, cosf(SlimeRot.y + D3DX_PI) * BULLET_SPEED);

				//敵の位置情報を取得
				D3DXVECTOR3 SlimePos = GetPos();

				//発射位置の高さを真ん中に上げる
				SlimePos.y += GetSize().y * 0.5f;

				//弾を生成
				CBullet::Create(SlimePos, BulletMove, CBullet::BULLETTYPE::BOUND_ENEMY);

				//回転判定をfalseにする
				m_bTurn = false;

				//カウントリセット
				SetCntShot(0);

				//次の弾発射までの時間を設定
				SetInterval();
			}
		}
	}

	//向き情報を設定
	SetRot(SlimeRot);
}
