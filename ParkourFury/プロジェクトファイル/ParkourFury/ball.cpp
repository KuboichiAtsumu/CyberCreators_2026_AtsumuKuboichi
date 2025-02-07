//==============================================================================================================================================
//
// ボールに関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "ball.h"
#include "game.h"
#include "UImanager.h"
#include "ballmovepoint.h"
#include "blockmanager.h"
#include "gimmickmanager.h"
#include "ballmovepoint.h"

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CBall::CBall() : CEnemy()
{
	//メンバ変数初期化
	m_TargetMove = { 0.0f, 0.0f, 0.0f };//目標の移動量
	m_nPointIdx = 0;//現在いるポジションのインデックス
	m_bUpdateMove = true;//移動量更新フラグ
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CBall::~CBall()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CBall::Init()
{
	//基底クラス初期化処理
	if (FAILED(CEnemy::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CBall::Uninit()
{
	//チェイスエリア警告のオブジェクト情報を検索
	CUI* pUI = CManager::GetInstance()->GetScene()->GetUIManager()->FindUI(CUI::TYPE::ESCAPE);

	//検索結果が不一致
	if (pUI == nullptr || pUI->GetUIType() != CUI::TYPE::ESCAPE)
	{
		return;
	}

	//UI終了処理
	pUI->Uninit();

	//基底クラス終了処理
	CEnemy::Uninit();
}

//===========================================================================================================
// 解放処理
//===========================================================================================================
void CBall::Release()
{
	//基底クラス解放処理
	CEnemy::Release();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CBall::Update()
{
	//基底クラス更新処理
	CEnemy::Update();

	//回転処理
	Turn();

	//重力処理
	Gravity();

	//移動処理
	Move();

	//当たり判定
	CollisionObj();
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CBall::Draw()
{
	//エディタモードの場合
	if (CManager::GetInstance()->GetScene()->GetSceneState() == CScene::SCENE::EDIT_3D)
	{//描画せずに処理を抜ける
		return;
	}

	//基底クラス描画処理
	CEnemy::Draw();
}

//===========================================================================================================
// 当たり判定
//===========================================================================================================
bool CBall::Collision(CollisionParam& aParam)
{
	//当たり判定フラグ
	bool bCollision = false;

	//当たり判定がtrue
	if (CEnemy::Collision(aParam))
	{
		//フラグをtrueにする
		bCollision = true;
	}

	return bCollision;
}

//===========================================================================================================
// リセット処理
//===========================================================================================================
void CBall::Reset()
{
	//パラメータ設定
	SetMove({ 0.0f, 0.0f, 0.0f });//移動量

	//メンバ変数初期化
	m_TargetMove = { 0.0f, 0.0f, 0.0f };//目標の移動量
	m_nPointIdx = 0;//現在いるポジションのインデックス
	m_bUpdateMove = true;//移動量更新フラグ
}

//===========================================================================================================
// 回転処理
//===========================================================================================================
void CBall::Turn()
{
	//ローカル変数宣言
	D3DXVECTOR3 move = GetMove();//移動量情報
	D3DXVECTOR3 rot = GetRot();//角度情報
	float fTurn = 0.0f;//回転量

	//画面左に動いている
	if (move.z > 0.0f)
	{
		fTurn = TURN_SPEED;
	}
	//画面右に動いている
	else
	{
		fTurn = -TURN_SPEED;
	}

	//回転処理
	rot.x += fTurn;

	//角度補正
	CorrectionRot(rot.x);

	//更新された角度情報設定
	SetRot(rot);
}

//===========================================================================================================
// 重力処理
//===========================================================================================================
void CBall::Gravity()
{
	//ローカル変数宣言
	D3DXVECTOR3 move = GetMove();//移動量取得

	//重力を加算
	move.y -= GRAVITY_POWER;

	//重力が上限を上回らないようにする
	if (move.y < MAX_GRAVITY)
	{
		move.y = MAX_GRAVITY;
	}

	//移動量を設定
	SetMove(move);
}

//===========================================================================================================
// 移動処理
//===========================================================================================================
void CBall::Move()
{
	//ローカル変数宣言
	D3DXVECTOR3 pos = GetPos();//座標取得
	D3DXVECTOR3 move = GetMove();//移動量取得

	//更新フラグがtrue
	if (m_bUpdateMove)
	{
		//次のポイントを検索
		CBallMovePoint* pPoint = CBallMovePoint::Find(m_nPointIdx);

		//検索結果が合致しなかったら処理を抜ける
		if (pPoint == nullptr || pPoint->GetIdx() != m_nPointIdx) return;

		//ポイントまでの方向ベクトルを求める
		D3DXVECTOR3 Direction = pPoint->GetPos() - pos;

		//次のポイントへの角度を求める
		float fAngle = atan2f(Direction.x, Direction.z);

		//移動量を設定
		m_TargetMove = { sinf(fAngle) * MOVE_SPEED, move.y, cosf(fAngle) * MOVE_SPEED };

		//フラグをfalseにする
		m_bUpdateMove = false;
	}

	//移動量が目標の値より小さい場合
	if (move.z < m_TargetMove.z)
	{
		//移動量を加算
		move.z += ADD_MOVE;

		//目標の値を超えないように調整
		if (move.z > m_TargetMove.z)
		{
			move.z = m_TargetMove.z;
		}
	}
	//移動量が目標の値より大きい場合
	else if (move.z > m_TargetMove.z)
	{
		//移動量を減算
		move.z -= ADD_MOVE;

		//目標の値を下回らないように調整
		if (move.z < m_TargetMove.z)
		{
			move.z = m_TargetMove.z;
		}
	}
 
	//移動量を設定
	SetMove(move);

	//過去の座標を設定
	SetOldPos(pos);

	//移動量を加算して座標を設定
	SetPos(pos + move);
}

//===========================================================================================================
// 当たり判定
//===========================================================================================================
void CBall::CollisionObj()
{
	//ローカル変数宣言
	D3DXVECTOR3 pos = GetPos();//座標
	D3DXVECTOR3 size = GetSize();//サイズ

	//ブロックの先頭情報取得
	CBlock* pBlock = CManager::GetInstance()->GetScene()->GetBlockManager()->GetTop();

	//ポインタがnullptrになるまで繰り返す
	while (pBlock != nullptr)
	{
		//次のオブジェクト情報を取得
		CBlock* pNext = pBlock->GetNextBlock();

		//ブロック情報が存在する場合
		if (pBlock != nullptr)
		{
			//パラメータ設定
			D3DXVECTOR3 BlockPos = pBlock->GetPos();//座標
			D3DXVECTOR3 BlockSize = pBlock->GetSize();//サイズ

			//ブロックとの当たり判定
			CollisionBlock(BlockPos, BlockSize);
		}

		//オブジェクト情報を次の情報に変更する
		pBlock = pNext;
	}

	//ギミックの先頭情報取得
	CGimmick* pGimmick = CManager::GetInstance()->GetScene()->GetGimmickManager()->GetTop();

	//ポインタがnullptrになるまで繰り返す
	while (pGimmick != nullptr)
	{
		//次のオブジェクト情報を取得
		CGimmick* pNext = pGimmick->GetNext();

		//ボール移動ポイントの情報を取得した場合
		if (pGimmick != nullptr && pGimmick->GetType() == CGimmick::TYPE::BALL_MOVE)
		{
			//パラメータ設定
			D3DXVECTOR3 PointPos = pGimmick->GetPos();//座標
			D3DXVECTOR3 PointSize = pGimmick->GetSize();//サイズ

			//ポイントとの当たり判定がtrue
			if (CollisionPoint(PointPos, PointSize))
			{
				//次の目標ポイントのインデックスを取得
				CBallMovePoint* pBallMovePoint = dynamic_cast<CBallMovePoint*>(pGimmick);
				m_nPointIdx = pBallMovePoint->GetIdx() + 1;

				//フラグをtrueにする
				m_bUpdateMove = true;

				//最終地点に到達した場合
				if (m_nPointIdx == pBallMovePoint->GetAll())
				{
					//終了処理
					Uninit();
				}

				break;
			}
		}

		//オブジェクト情報を次の情報に変更する
		pGimmick = pNext;
	}
}

//===========================================================================================================
// ブロックとの当たり判定
//===========================================================================================================
void CBall::CollisionBlock(D3DXVECTOR3 BlockPos, D3DXVECTOR3 BlockSize)
{
	//ローカル変数宣言
	D3DXVECTOR3 pos = GetPos();//自分自身の座標
	D3DXVECTOR3 oldpos = GetOldPos();//自分自身の過去の座標
	D3DXVECTOR3 size = GetSize();//自分自身のサイズ
	D3DXVECTOR3 move = GetMove();//自分自身の移動量

	//=======================================================================
	// Y軸
	//=======================================================================

	//上側の判定
	if (pos.y - size.y * 0.5f < BlockPos.y + BlockSize.y * 0.5f
		&& oldpos.y - size.y * 0.5f >= BlockPos.y + BlockSize.y * 0.5f)
	{
		//XZ間の範囲内にいる場合
		if ((pos.x >= BlockPos.x - BlockSize.x * 0.5f
			&& pos.x <= BlockPos.x + BlockSize.x * 0.5f)
			&& (pos.z >= BlockPos.z - BlockSize.z * 0.5f
				&& pos.z <= BlockPos.z + BlockSize.z * 0.5f))
		{
			//座標を修正
			pos.y = BlockPos.y + BlockSize.y * 0.5f + size.y * 0.5f;

			//Y軸の移動量をリセット
			move.y *= BOUND_POWER;
		}
	}

	//下側の判定
	if (pos.y + size.y * 0.5f > BlockPos.y - BlockSize.y * 0.5f
		&& oldpos.y + size.y * 0.5f <= BlockPos.y - BlockSize.y * 0.5f)
	{
		//XZ間の範囲内にいる場合
		if ((pos.x > BlockPos.x - BlockSize.x * 0.5f
			&& pos.x < BlockPos.x + BlockSize.x * 0.5f)
			&& (pos.z > BlockPos.z - BlockSize.z * 0.5f
				&& pos.z < BlockPos.z + BlockSize.z * 0.5f))
		{
			//座標を修正
			pos.y = BlockPos.y - BlockSize.y * 0.5f - size.y * 0.5f;

			//Y軸の移動量をリセット
			move.y = 0.0f;
		}
	}

	//=======================================================================
	// X軸
	//=======================================================================

	//左側の判定
	if (pos.x + size.x * 0.5f > BlockPos.x - BlockSize.x * 0.5f
		&& oldpos.x + size.x * 0.5f <= BlockPos.x - BlockSize.x * 0.5f)
	{
		//YZ間の範囲内にいる場合
		if ((pos.y + size.y * 0.5f > BlockPos.y - BlockSize.y * 0.5f
			&& pos.y - size.y * 0.5f < BlockPos.y + BlockSize.y * 0.5f)
			&& (pos.z + size.z * 0.5f > BlockPos.z - BlockSize.z * 0.5f
				&& pos.z - size.z * 0.5f < BlockPos.z + BlockSize.z * 0.5f))
		{
			//座標を修正
			pos.x = BlockPos.x - BlockSize.x * 0.5f - size.x * 0.5f;
			move.x *= -1.0f;
		}
	}

	//右側の判定
	if (pos.x - size.x * 0.5f < BlockPos.x + BlockSize.x * 0.5f
		&& oldpos.x - size.x * 0.5f >= BlockPos.x + BlockSize.x * 0.5f)
	{
		//YZ間の範囲内にいる場合
		if ((pos.y + size.y * 0.5f > BlockPos.y - BlockSize.y * 0.5f
			&& pos.y - size.y * 0.5f < BlockPos.y + BlockSize.y * 0.5f)
			&& (pos.z + size.z * 0.5f > BlockPos.z - BlockSize.z * 0.5f
				&& pos.z - size.z * 0.5f < BlockPos.z + BlockSize.z * 0.5f))
		{
			//座標を修正
			pos.x = BlockPos.x + BlockSize.x * 0.5f + size.x * 0.5f;
			move.x *= -1.0f;
		}
	}

	//=======================================================================
	// Z軸
	//=======================================================================

	//手前側の判定
	if (pos.z + size.z * 0.5f > BlockPos.z - BlockSize.z * 0.5f
		&& oldpos.z + size.z * 0.5f <= BlockPos.z - BlockSize.z * 0.5f)
	{
		//XY間の範囲内にいる場合
		if ((pos.x + size.x * 0.5f > BlockPos.x - BlockSize.x * 0.5f
			&& pos.x - size.x * 0.5f < BlockPos.x + BlockSize.x * 0.5f)
			&& (pos.y + size.y * 0.5f > BlockPos.y - BlockSize.y * 0.5f
				&& pos.y - size.y * 0.5f < BlockPos.y + BlockSize.y * 0.5f))
		{
			//座標を修正
			pos.z = BlockPos.z - BlockSize.z * 0.5f - size.z * 0.5f;
			move.z *= -1.0f;
		}
	}

	//奥側の判定
	if (pos.z - size.z * 0.5f < BlockPos.z + BlockSize.z * 0.5f
		&& oldpos.z - size.z * 0.5f >= BlockPos.z + BlockSize.z * 0.5f)
	{
		//XY間の範囲内にいる場合
		if ((pos.x + size.x * 0.5f > BlockPos.x - BlockSize.x * 0.5f
			&& pos.x - size.x * 0.5f < BlockPos.x + BlockSize.x * 0.5f)
			&& (pos.y + size.y * 0.5f > BlockPos.y - BlockSize.y * 0.5f
				&& pos.y - size.y * 0.5f < BlockPos.y + BlockSize.y * 0.5f))
		{
			//座標を修正
			pos.z = BlockPos.z + BlockSize.z * 0.5f + size.z * 0.5f;
			move.z *= -1.0f;
		}
	}

	//パラメータ設定
	SetPos(pos);//座標
	SetMove(move);//移動量
}

//===========================================================================================================
// ポイントとの当たり判定
//===========================================================================================================
bool CBall::CollisionPoint(D3DXVECTOR3 PointPos, D3DXVECTOR3 PointSize)
{
	//ローカル変数宣言
	D3DXVECTOR3 pos = GetPos();//自分自身の座標
	D3DXVECTOR3 size = GetSize();//自分自身のサイズ
	bool bCollision = false;//当たり判定フラグ

	//範囲内にいる場合
	if ((pos.x >= PointPos.x - PointSize.x * 0.5f && 
			pos.x <= PointPos.x + PointSize.x * 0.5f) && 
		(pos.y - size.y * 0.5f >= PointPos.y - PointSize.y * 0.5f &&
			pos.y - size.y * 0.5f <= PointPos.y + PointSize.y * 0.5f) &&
		(pos.z >= PointPos.z - PointSize.z * 0.5f && 
			pos.z <= PointPos.z + PointSize.z * 0.5f))
	{
		//フラグをtrueにする
		bCollision = true;
	}

	return bCollision;
}
