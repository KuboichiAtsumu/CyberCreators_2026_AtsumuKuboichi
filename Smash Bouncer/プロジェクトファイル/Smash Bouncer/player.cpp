//==============================================================================================================================================
//
// プレイヤーに関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "player.h"//プレイヤー
#include "block.h"//ブロック
#include "item.h"//アイテム
#include "camera.h"//カメラ
#include "game.h"//ゲームシーン
#include "zone.h"//ゾーン
#include "blockmanager.h"//ブロックマネージャー
#include "bulletmanager.h"//バレットマネージャー
#include "tutorial.h"//チュートリアルシーン

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CPlayer::CPlayer(int nPriority) : CMotionCharacter(nPriority)
{
	//メンバー変数
	m_State = NORMAL;//状態
	m_nCntState = 0;//状態カウンタ
	m_bLightUp = false;//ライトアップフラグ
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CPlayer::~CPlayer()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CPlayer::Init()
{
	//パーツファイル読込処理
	LoadParts(CMotionCharacter::PLAYER);

	//モーション情報読込処理
	LoadMotion(CMotionCharacter::PLAYER);

	//パラメータ設定
	SetRot({ 0.0f, -D3DX_PI * 0.5f, 0.0f });//向き
	SetAllPartsSize();//サイズ

	//Xファイルオブジェクト初期化処理
	if (FAILED(CObjectX::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CPlayer::Uninit()
{
	//Xファイルオブジェクトの終了処理
	CObjectX::Uninit();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CPlayer::Update()
{
	//エディタモードではない場合
	if (CManager::GetScene()->GetEditMode())
	{//描画せずに処理を抜ける
		return;
	}

	//ローカル変数宣言
	CBlockManager* pBlockManager = nullptr;//ブロックマネージャー
	
	//シーンに応じてマネージャー情報を取得
	switch (CManager::GetScene()->GetMode())
	{
		//チュートリアル
	case CScene::MODE_TUTORIAL:
		pBlockManager = CTutorial::GetBlockManager();
		break;

		//ゲーム
	case CScene::MODE_GAME:
		pBlockManager = CGame::GetBlockManager();
		break;

	default:
		break;
	}

	//ダメージ状態の場合
	if (m_State == DAMAGE)
	{
		//状態カウンタが残っている場合
		if (m_nCntState > 0)
		{
			//状態カウンタが残っている場合
			m_nCntState--;
		}
		//状態カウンタが０の場合
		else
		{
			//通常状態にする
			m_State = NORMAL;
		}

		//ライトアップ中の場合
		if (m_bLightUp)
		{
			//モデルを明るく設定
			SetPartsFlash(1.0f);
		}
		else
		{
			//モデルを暗く設定
			SetPartsFlash(0.3f);
		}

		//ライトアップフラグを切り替える
		m_bLightUp = m_bLightUp ? false : true;
	}
	//ダメージ状態ではない場合
	else
	{
		//モデルを明るく設定
		SetPartsFlash(1.0f);

		//移動処理
		Move();

		//反射処理
		Reflect();
	}

	//パラメータ取得
	D3DXVECTOR3 pos = GetPos();//位置
	D3DXVECTOR3 oldpos = GetOldPos();//過去の位置
	D3DXVECTOR3 Size = GetSize();//サイズ

	//ブロックとの当たり判定
	pBlockManager->Collision(pos, oldpos, Size);

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

			//オブジェクトタイプ別に処理
			switch (type)
			{
				//ゾーン
			case CObject::ZONE:
				//ゾーンとの当たり判定
				CollisionZone(pos, pObj);
				break;

			default:
				break;
			}

			//オブジェクト情報を次の情報に変更する
			pObj = pNext;
		}
	}

	//更新された情報を設定
	SetPos(pos);//位置
	SetOldPos(oldpos);//過去の位置
	
	//モーション処理
	CMotionCharacter::Update();
}

//===========================================================================================================
// 移動処理
//===========================================================================================================
void CPlayer::Move()
{
	//ローカル変数宣言
	D3DXVECTOR3 CameraRot = CManager::GetCamera()->GetRot();//カメラの向き情報
	D3DXVECTOR3 pos = GetPos();//位置
	D3DXVECTOR3 oldpos = GetOldPos();//過去の位置
	D3DXVECTOR3 rot = GetRot();//向き
	D3DXVECTOR3 move = GetMove();//移動量
	float fDiaSpeed = GetSpeed();//速度倍率

	//上移動
	if (CManager::GetKeyboard()->GetPress(DIK_W)
		|| (float)CManager::GetJoypad()->GetStick().abAnglePress[CInputJoypad::STICKTYPE_LEFT][CInputJoypad::STICKANGLE_UP] > 0.1f)
	{
		//左上移動
		if (CManager::GetKeyboard()->GetPress(DIK_A)
			|| (float)CManager::GetJoypad()->GetStick().abAnglePress[CInputJoypad::STICKTYPE_LEFT][CInputJoypad::STICKANGLE_LEFT] > 0.1f)
		{
			//移動量を加算
			move.x += sinf(CameraRot.y + D3DX_PI * -0.25f) * fDiaSpeed;//X軸
			move.z += cosf(CameraRot.y + D3DX_PI * -0.25f) * fDiaSpeed;//Z軸

			//向きを更新
			if (rot.y <= CameraRot.y + D3DX_PI * 0.75f && rot.y >= CameraRot.y + D3DX_PI * -0.25f)
			{
				//右回転
				rot.y += MAX_TURN;

				//左上向き
				if (rot.y >= CameraRot.y + D3DX_PI * 0.75f)
				{
					rot.y = CameraRot.y + D3DX_PI * 0.75f;
				}
			}
			else
			{
				//左回転
				rot.y -= MAX_TURN;

				//左上向き
				if (rot.y <= CameraRot.y + D3DX_PI * 0.75f)
				{
					rot.y = CameraRot.y + D3DX_PI * 0.75f;
				}
			}
		}
		//右上移動
		else if (CManager::GetKeyboard()->GetPress(DIK_D)
			|| (float)CManager::GetJoypad()->GetStick().abAnglePress[CInputJoypad::STICKTYPE_LEFT][CInputJoypad::STICKANGLE_RIGHT] > 0.1f)
		{
			//移動量を加算
			move.x += sinf(CameraRot.y + D3DX_PI * 0.25f) * fDiaSpeed;//X軸
			move.z += cosf(CameraRot.y + D3DX_PI * 0.25f) * fDiaSpeed;//Z軸

			//向きを更新
			if (rot.y <= CameraRot.y + D3DX_PI * 0.25f && rot.y >= CameraRot.y + D3DX_PI * -0.75f)
			{
				rot.y -= MAX_TURN;//左回転

				if (rot.y <= CameraRot.y + D3DX_PI * -0.75f)
				{
					rot.y = CameraRot.y + D3DX_PI * -0.75f;//右上向き
				}
			}
			else
			{
				rot.y += MAX_TURN;//右回転

				if (rot.y >= CameraRot.y + D3DX_PI * -0.75f)
				{
					rot.y = CameraRot.y + D3DX_PI * -0.75f;//右上向き
				}
			}
		}
		//真上移動
		else
		{
			//移動量を加算
			move.x += sinf(CameraRot.y) * fDiaSpeed;//X軸
			move.z += cosf(CameraRot.y) * fDiaSpeed;//Z軸

			rot.y = CameraRot.y + D3DX_PI;//上向き
		}
	}

	//下移動
	else if (CManager::GetKeyboard()->GetPress(DIK_S)
		|| (float)CManager::GetJoypad()->GetStick().abAnglePress[CInputJoypad::STICKTYPE_LEFT][CInputJoypad::STICKANGLE_DOWN] > 0.1f)
	{
		//左下移動
		if (CManager::GetKeyboard()->GetPress(DIK_A)
			|| (float)CManager::GetJoypad()->GetStick().abAnglePress[CInputJoypad::STICKTYPE_LEFT][CInputJoypad::STICKANGLE_LEFT] > 0.1f)
		{
			//移動量を加算
			move.x += sinf(CameraRot.y + D3DX_PI * -0.75f) * fDiaSpeed;//X軸
			move.z += cosf(CameraRot.y + D3DX_PI * -0.75f) * fDiaSpeed;//Z軸

			//向きを更新
			if (rot.y >= CameraRot.y + D3DX_PI * -0.75f && rot.y <= CameraRot.y + D3DX_PI * 0.25f)
			{
				rot.y += MAX_TURN;//右回転

				if (rot.y >= CameraRot.y + D3DX_PI * 0.25f)
				{
					rot.y = CameraRot.y + D3DX_PI * 0.25f;//左下向き
				}
			}
			else
			{
				rot.y -= MAX_TURN;//左回転

				if (rot.y <= CameraRot.y + D3DX_PI * 0.25f)
				{
					rot.y = CameraRot.y + D3DX_PI * 0.25f;//左下向き
				}
			}
		}
		//右下移動
		else if (CManager::GetKeyboard()->GetPress(DIK_D)
			|| (float)CManager::GetJoypad()->GetStick().abAnglePress[CInputJoypad::STICKTYPE_LEFT][CInputJoypad::STICKANGLE_RIGHT] > 0.1f)
		{
			//移動量を加算
			move.x += sinf(CameraRot.y + D3DX_PI * 0.75f) * fDiaSpeed;//X軸
			move.z += cosf(CameraRot.y + D3DX_PI * 0.75f) * fDiaSpeed;//Z軸

			//向きを更新
			if (rot.y >= CameraRot.y + D3DX_PI * -0.25f && rot.y <= CameraRot.y + D3DX_PI * 0.75f)
			{
				rot.y -= MAX_TURN;//左回転

				if (rot.y <= CameraRot.y + D3DX_PI * -0.25f)
				{
					rot.y = CameraRot.y + D3DX_PI * -0.25f;//右下向き
				}
			}
			else
			{
				rot.y += MAX_TURN;//右回転

				if (rot.y >= CameraRot.y + D3DX_PI * -0.25f)
				{
					rot.y = CameraRot.y + D3DX_PI * -0.25f;//右下向き
				}
			}
		}
		//真下移動
		else
		{
			//移動量を加算
			move.x += sinf(CameraRot.y + D3DX_PI) * fDiaSpeed;//X軸
			move.z += cosf(CameraRot.y + D3DX_PI) * fDiaSpeed;//Z軸

			rot.y = CameraRot.y;//下向き
		}
	}

	//左移動
	else if (CManager::GetKeyboard()->GetPress(DIK_A)
	|| (float)CManager::GetJoypad()->GetStick().abAnglePress[CInputJoypad::STICKTYPE_LEFT][CInputJoypad::STICKANGLE_LEFT] > 0.1f)
	{
		//移動量を加算
		move.x += sinf(CameraRot.y + D3DX_PI * -0.5f) * fDiaSpeed;//X軸
		move.z += cosf(CameraRot.y + D3DX_PI * -0.5f) * fDiaSpeed;//Z軸

		//向きを更新
		if (rot.y >= CameraRot.y + D3DX_PI * -0.5f && rot.y <= CameraRot.y + D3DX_PI * 0.5f)
		{
			rot.y += MAX_TURN;//右回転

			if (rot.y >= CameraRot.y + D3DX_PI * 0.5f)
			{
				rot.y = CameraRot.y + D3DX_PI * 0.5f;//左向き
			}
		}
		else
		{
			rot.y -= MAX_TURN;//左回転

			if (rot.y <= CameraRot.y + D3DX_PI * 0.5f)
			{
				rot.y = CameraRot.y + D3DX_PI * 0.5f;//左向き
			}
		}
	}

	//右移動
	else if (CManager::GetKeyboard()->GetPress(DIK_D)
	|| (float)CManager::GetJoypad()->GetStick().abAnglePress[CInputJoypad::STICKTYPE_LEFT][CInputJoypad::STICKANGLE_RIGHT] > 0.1f)
	{
		//移動量を加算
		move.x += sinf(CameraRot.y + D3DX_PI * 0.5f) * fDiaSpeed;//X軸
		move.z += cosf(CameraRot.y + D3DX_PI * 0.5f) * fDiaSpeed;//Z軸

		//向きを更新
		if (rot.y >= CameraRot.y + D3DX_PI * -0.5f && rot.y <= CameraRot.y + D3DX_PI * 0.5f)
		{
			rot.y -= MAX_TURN;//左回転

			if (rot.y <= CameraRot.y + D3DX_PI * -0.5f)
			{
				rot.y = CameraRot.y + D3DX_PI * -0.5f;//右向き
			}
		}
		else
		{
			rot.y += MAX_TURN;//右回転

			if (rot.y >= CameraRot.y + D3DX_PI * -0.5f)
			{
				rot.y = CameraRot.y + D3DX_PI * -0.5f;//右向き
			}
		}
	}

	//過去の位置を保存
	oldpos = pos;

	//移動量を加算
	pos += move;

	//移動量を減衰
	move.x += (0.0f - move.x) * MAX_INERTIA;
	move.z += (0.0f - move.z) * MAX_INERTIA;

	//更新された情報を設定
	SetMove(move);
	SetRot(rot);//角度
	SetPos(pos);//位置
	SetOldPos(oldpos);//過去の位置

	//動いていない場合
	//現在の状態情報が移動状態の場合
	if ((move.x <= MAX_JUDGE_MOVE && move.x >= -MAX_JUDGE_MOVE) && (move.z <= MAX_JUDGE_MOVE && move.z >= -MAX_JUDGE_MOVE)
		&& GetState() == MOVE)
	{
		//ニュートラル状態に設定
		SetState(CMotionCharacter::NEUTRAL);
	}
	//動いている場合
	else if((move.x >= MAX_JUDGE_MOVE || move.x <= -MAX_JUDGE_MOVE) || (move.z >= MAX_JUDGE_MOVE || move.z <= -MAX_JUDGE_MOVE))
	{
		//移動状態に設定
		SetState(CMotionCharacter::MOVE);
	}
}

//===========================================================================================================
// 反射処理
//===========================================================================================================
void CPlayer::Reflect()
{
	//ローカル変数宣言
	static bool bStickInput = false;//スティック入力判定
	static int nCommandTime = 0;//コマンド受付フレーム
	static float fFirstAngle = 0.0f;//右スティックが入力された瞬間の角度
	static float fStickAngle = 0.0f;//現在のスティックの角度
	static float fDistance = MIN_REFLECT_POWER;//右スティックの曲げた距離
	D3DXVECTOR3 BulletMove = { 0.0f,0.0f,0.0f };//バレットの移動量
	D3DXVECTOR3 pos = GetPos();//位置
	D3DXVECTOR3 rot = GetRot();//向き
	CBulletManager* pBulletManager = nullptr;//バレットマネージャー

	//シーンに応じてマネージャー情報を取得
	switch (CManager::GetScene()->GetMode())
	{
		//チュートリアル
	case CScene::MODE_TUTORIAL:
		pBulletManager = CTutorial::GetBulletManager();
		break;

		//ゲーム
	case CScene::MODE_GAME:
		pBulletManager = CGame::GetBulletManager();
		break;

	default:
		break;
	}
	
	//コマンド入力受付フレームが残っている場合
	if (nCommandTime > 0)
	{
		//受付フレームを減らす
		nCommandTime--;
	}
	//コマンド入力を受け付けていない場合
	else
	{
		//右スティックの入力情報を初期化
		bStickInput = false;//入力判定
		fFirstAngle = 0.0f;//入力された最初の角度
		fStickAngle = 0.0f;//現在のスティックの角度
		fDistance = MIN_REFLECT_POWER;//現在の角度と最初の角度の差の最大値
	}

	//右スティックが倒された場合
	//バレットタイプがエネミーの場合
	if (((float)CManager::GetJoypad()->GetStick().abAnglePress[CInputJoypad::STICKTYPE_RIGHT][CInputJoypad::STICKANGLE_UP] > 0.1f
		|| (float)CManager::GetJoypad()->GetStick().abAnglePress[CInputJoypad::STICKTYPE_RIGHT][CInputJoypad::STICKANGLE_DOWN] > 0.1f
		|| (float)CManager::GetJoypad()->GetStick().abAnglePress[CInputJoypad::STICKTYPE_RIGHT][CInputJoypad::STICKANGLE_RIGHT] > 0.1f
		|| (float)CManager::GetJoypad()->GetStick().abAnglePress[CInputJoypad::STICKTYPE_RIGHT][CInputJoypad::STICKANGLE_LEFT] > 0.1f))
	{
		//スティックの現在の角度を保存
		fStickAngle = CManager::GetJoypad()->GetAngleR();

		//入力された瞬間の角度を保存
		if (!bStickInput)
		{
			fFirstAngle = fStickAngle;//最初のスティックの角度を保存
			nCommandTime = COMMAND_TIME;//コマンド受付フレームを設定
			bStickInput = true;//入力判定をtrueにする
		}
	
		//コマンド入力受付中の場合
		//最大角度が現在保存されている値より大きい場合
		if (nCommandTime > 0
			&& abs(fStickAngle - fFirstAngle) > fDistance)
		{
			//入力中に動かした最大角度を更新
			fDistance = abs(fStickAngle - fFirstAngle);

			//倍率が超過しないように調整
			if (fDistance > MAX_REFLECT_POWER)
			{
				fDistance = MAX_REFLECT_POWER;
			}
		}
		
		//当たり判定がtrueのバレット情報を取得
		CBullet* pBullet = pBulletManager->Collision(pos);

		//バレットタイプがエネミーの場合
		if (pBullet != nullptr
			&& (pBullet->GetBulletType() == CBullet::BOUND_ENEMY || pBullet->GetBulletType() == CBullet::LAUNCH_ENEMY))
		{
			//アクション状態に設定
			SetState(CMotionCharacter::ACTION);

			//プレイヤーの向きにスティックの角度を反映
			rot.y = fStickAngle;

			//バレットの終了処理
			pBullet->Uninit();

			//新しく生成するプレイヤーバレットの移動量
			BulletMove = D3DXVECTOR3(sinf(rot.y + D3DX_PI) * (fDistance + MIN_REFLECT_SPEED), 0.0f, cosf(rot.y + D3DX_PI) * (fDistance + MIN_REFLECT_SPEED));

			//SE再生
			CManager::GetSound()->Play(CSound::SE_SMASH);

			//カメラの揺れを設定
			CManager::GetCamera()->SetShake(10, int(fDistance));

			//新しくプレイヤーバレットを生成
			CBullet::Create(pos, BulletMove, CBullet::PLAYER);

			//クローン弾を生成
			for (int nCnt = 0; nCnt < int(fDistance); nCnt++)
			{
				CBullet::Create(pos,
					{ sinf(rot.y - DIFFUSION_RATE * (float)(nCnt + 1) + D3DX_PI) * (fDistance + MIN_REFLECT_SPEED), 0.0f, cosf(rot.y - DIFFUSION_RATE * (float)(nCnt + 1) + D3DX_PI) * (fDistance + MIN_REFLECT_SPEED) },
					CBullet::CLONE);

				CBullet::Create(pos,
					{ sinf(rot.y + DIFFUSION_RATE * (float)(nCnt + 1) + D3DX_PI) * (fDistance + MIN_REFLECT_SPEED), 0.0f, cosf(rot.y + DIFFUSION_RATE * (float)(nCnt + 1) + D3DX_PI) * (fDistance + MIN_REFLECT_SPEED) },
					CBullet::CLONE);
			}
		}
	}

	//更新された情報を設定
	SetRot(rot);//角度
	SetPos(pos);//位置
}

//===========================================================================================================
// ゾーンとの当たり判定
//===========================================================================================================
void CPlayer::CollisionZone(D3DXVECTOR3& pos, CObject*& pObj)
{
	//ローカル変数宣言
	CZone* pZone = (CZone*)pObj;//オブジェクト情報をゾーン情報に変換
	D3DXVECTOR3 oldpos = GetOldPos();//過去の位置情報
	D3DXVECTOR3 size = GetSize();//サイズ情報

	//ゾーンタイプが反射ゾーンの場合
	if (pZone->GetZoneType() == CZone::REFLECT)
	{
		//ゾーンの範囲内にいる場合
		if (pZone->Judge(pos))
		{
			//プレイヤーがいるステージの番号を保存する
			CGame::SetNumStage(pZone->GetID());

			//ゲームの状態が移動中の場合
			if (CGame::GetGameState() == CGame::STATE_MOVE)
			{
				//ゲーム中の状態に設定
				CGame::SetGameState(CGame::STATE_GAME);
			}
		}
		
		//ゲーム中の場合
		if (CGame::GetGameState() == CGame::STATE_GAME
			|| CManager::GetScene()->GetMode() == CScene::MODE_TUTORIAL)
		{
			//ゾーンとの当たり判定
			pZone->Collision(pos, oldpos, size);
		}
	}

	//ゾーンタイプがステージ区分ゾーンの場合
	//ゾーンの範囲内にいる場合
	else if (pZone->GetZoneType() == CZone::STAGE)
	{
		//ゾーンとの当たり判定
		pZone->Collision(pos, oldpos, size);
	}
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CPlayer::Draw()
{
	//モーションキャラクター描画処理
	CMotionCharacter::Draw();
}

//===========================================================================================================
// 生成処理
//===========================================================================================================
CPlayer* CPlayer::Create(D3DXVECTOR3 pos)
{
	//メモリを動的確保
	CPlayer* pPlayer = NEW CPlayer();

	//パラメータ設定
	pPlayer->SetType(TYPE::PLAYER);//オブジェクト種類の設定
	pPlayer->SetPos(pos);//生成位置設定
	pPlayer->SetOldPos(pos);//過去の位置

	//初期化処理
	pPlayer->Init();

	return pPlayer;
}

//===========================================================================================================
// 当たり判定
//===========================================================================================================
bool CPlayer::Collision(D3DXVECTOR3& pos)
{
	//ローカル変数宣言
	bool bCollision = false;//当たり判定フラグ
	D3DXVECTOR3 Pos = GetPos();//位置情報
	D3DXVECTOR3 size = GetSize();//サイズ情報

	//範囲内にある場合
	if (pos.x <= Pos.x + size.x * 0.5f && pos.x >= Pos.x - size.x * 0.5f
		&& pos.y <= Pos.y + size.y * 0.5f && pos.y >= Pos.y - size.y * 0.5f
		&& pos.z <= Pos.z + size.z * 0.5f && pos.z >= Pos.z - size.z * 0.5f)
	{
		//当たり判定フラグを有効にする
		bCollision = true;
	}

	return bCollision;
}
