//==============================================================================================================================================
//
// プレイヤーに関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "player.h"
#include "bullet.h"
#include "particle.h"
#include "boss.h"
#include "bossattack.h"
#include "game.h"
#include "minigame.h"
#include "ball.h"
#include "ballspawner.h"
#include "smoke.h"
#include "UI.h"
#include "effect2D.h"
#include "gimmickmanager.h"
#include "areamanager.h"
#include "blockmanager.h"
#include "enemymanager.h"
#include "signboardmanager.h"
#include "UImanager.h"

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CPlayer::CPlayer(int nPriority) : CMotionCharacter(nPriority),
	m_State(STATE::NEUTRAL),
	m_nCntState(0),
	m_nCntDash(0),
	m_nCntDoublePush(0),
	m_nCntJump(JUMP_TIMES),
	m_nDecTenCT(DEC_HEAT_CT),
	m_nKill(0),
	m_nComboTime(0),
	m_nCombo(0),
	m_nDecComboFrame(0),
	m_fInertia(BASE_INERTIA),
	m_fHeat(0.0f),
	m_fWallKickTurn(0.0f),
	m_fTargetRot_Y(0.0f),
	m_bShooting(false),
	m_bBossCreate(false),
	m_bCollisionBlock(false)
{
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
	//基底クラス初期化処理
	if (FAILED(CMotionCharacter::Init()))
	{
		return E_FAIL;
	}

	//パラメータ設定
	SetRot({ 0.0f, -D3DX_PI * 0.5f, 0.0f });//角度

	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CPlayer::Uninit()
{
	//基底クラス終了処理
	CMotionCharacter::Uninit();
}

//===========================================================================================================
// 解放処理
//===========================================================================================================
void CPlayer::Release()
{
	//基底クラス解放処理
	CMotionCharacter::Release();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CPlayer::Update()
{
	//エディタモードの場合
	if (CManager::GetInstance()->GetScene()->GetSceneState() != CScene::SCENE::GAME)
	{//更新せずに処理を抜ける
		return;
	}

	//基底クラス更新処理
	CMotionCharacter::Update();

	//状態カウンタを減らす
	if (m_nCntState > 0)
	{
		m_nCntState--;
	}

	//テンション量減少処理
	DecreaseHeat();

	//コンボ数リセット処理
	ResetCombo();

	//モーション処理
	SetMotionState(static_cast<int>(m_State));

	//操作処理
	Controll();

	//当たり判定処理
	Collision();

	//ヒートエフェクト発生処理
	OccurHeatEffect();

	//デバッグ用処理
#ifdef _DEBUG
	//ワープ処理
	Warp();
#endif
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
CPlayer* CPlayer::Create(const D3DXVECTOR3& pos)
{
	//メモリを動的確保
	CPlayer* pPlayer = NEW CPlayer(static_cast<int>(Category::PLAYER));

	//プレイヤーの情報が存在する場合
	if (pPlayer != nullptr)
	{
		//パラメータ設定
		pPlayer->SetPos(pos);//生成座標設定
		pPlayer->SetOldPos(pos);//過去の座標
		pPlayer->SetCharacterType(TYPE::PLAYER);//キャラクタータイプ

		//初期化処理
		pPlayer->Init();
	}

	return pPlayer;
}

//===========================================================================================================
// モーション状態設定処理
//===========================================================================================================
void CPlayer::SetMotionState(int nState)
{
	//壁キックモーション中の場合
	if (nState == static_cast<int>(STATE::WALL_KICK) &&
		GetMotionFlag())
	{
		//角度を反転させる
		D3DXVECTOR3 rot = GetRot();
		rot.y += m_fWallKickTurn;

		//角度補正
		CorrectionRot(rot.y);

		//角度設定
		SetRot(rot);
	}

	//基底クラス処理
	CMotionCharacter::SetMotionState(nState);

	//移動状態ではない場合
	if (nState != static_cast<int>(STATE::LANDING_MOVE))
	{
		//状態のインデックスを保存
		int nIdxState = static_cast<int>(STATE::LANDING_MOVE);

		//移動モーションの情報を取得
		MotionSet aMotion = GetMotionSet(nIdxState);

		//移動状態のフレーム数もカウント
		aMotion.nCntFrame++;

		//カウントが指定のフレーム数になった場合
		if (aMotion.nCntFrame > aMotion.aKeyset.at(aMotion.nCntKeySet).nMaxFrame)
		{
			//フレーム数カウントを初期化
			aMotion.nCntFrame = 0;

			//キーセット数を加算
			aMotion.nCntKeySet++;

			//キー数が最大数になった場合
			if (aMotion.nCntKeySet == aMotion.nMaxKeySet + 1 &&
				static_cast<bool>(aMotion.nLoop))
			{//ループする場合

				//キーセット数を初期化
				aMotion.nCntKeySet = 0;
			}
		}

		//モーション情報を設定
		SetMotionSet(nIdxState, aMotion);
	}

	//パーツごとの状態設定処理
	SetPartsState();
}

//===========================================================================================================
// パーツごとの状態設定処理
//===========================================================================================================
void CPlayer::SetPartsState()
{
	//ローカル変数宣言
	int nState = 0;//状態保存用変数
	int nNumParts = GetPartsSize();//パーツ情報

	for (int nCntParts = 0; nCntParts < nNumParts; nCntParts++)
	{
		//状態をint型に保存
		nState = static_cast<int>(m_State);

		//パーツ情報を取得
		ModelParts aParts = GetPartsInfo(nCntParts);

		//下半身パーツの場合
		if (aParts.Half == HALF_BODY::DOWN_BODY &&
			CObject::MovingJudgeXZ(GetMove()) &&
			GetLandingFlag() &&
			m_State != STATE::DASH &&
			m_State != STATE::DAMAGE &&
			m_State != STATE::SLIDING)
		{
			//移動状態に設定
			nState = static_cast<int>(STATE::LANDING_MOVE);
		}
		//上半身パーツで射撃中
		else if (aParts.Half == HALF_BODY::UP_BODY && 
			m_State != STATE::WALL_SLIDE &&
			m_bShooting)
		{
			//射撃状態に設定
			nState = static_cast<int>(STATE::SHOT);
		}

		//モーション処理
		Motion(nState, nCntParts);
	}
}

//===========================================================================================================
// 操作処理
//===========================================================================================================
void CPlayer::Controll()
{
	//ローカル変数宣言
	CManager* pManager = CManager::GetInstance();//マネージャーのインスタンス
	CInputKeyboard* pKeyBoard = pManager->GetKeyboard();// キーボード情報のインスタンス
	CInputJoypad* pJoyPad = pManager->GetJoypad();// ジョイパッド情報のインスタンス
	D3DXVECTOR3 CameraRot = pManager->GetCamera()->GetRot();//カメラの角度情報
	D3DXVECTOR3 pos = GetPos();//座標
	D3DXVECTOR3 move = GetMove();//移動量
	float fDiaSpeed = GetSpeed();//速度倍率
	float fFirstDiaSpeed = fDiaSpeed;//速度倍率初期値を保存
	float fDiaJump = GetJump();//ジャンプ力
	float fGravity = fDiaJump * 0.04f;//重力

	//ダメージ状態ではない場合
	if (m_State != STATE::DAMAGE)
	{
		//ダッシュ処理
		Dash(fDiaSpeed, fFirstDiaSpeed);

		//スライディング処理
		Sliding(move);

		//ジャンプ処理
		Jump(move, fDiaJump);

		//発射処理
		Shot();

		//壁キック状態の場合
		if (m_State == STATE::WALL_KICK)
		{
			//速度倍率を低下させる
			fDiaSpeed *= 0.001f;
		}

		//崖掴み硬直時間外の場合
		if (m_nCntState <= RIGOR_CLIFF - POSSIBLE_FALL)
		{
			//左移動
			if (pKeyBoard->GetPress(DIK_A) || 
				pJoyPad->GetPress(CInputJoypad::JOYKEY::JOYKEY_LEFT))
			{
				//移動処理
				Move(move, fDiaSpeed, CameraRot.y + D3DX_PI * 0.5f);
			}
			//右移動
			else if (pKeyBoard->GetPress(DIK_D) || 
				pJoyPad->GetPress(CInputJoypad::JOYKEY::JOYKEY_RIGHT))
			{
				//移動処理
				Move(move, fDiaSpeed, CameraRot.y - D3DX_PI * 0.5f);
			}
		}
	}

	//崖掴まり状態ではない場合
	if (m_State != STATE::CLIFF_GRAB &&
		m_State != STATE::WALL_SLIDE)
	{
		//重力を加算
		move.y -= fGravity;

		//重力が上限を上回らないようにする
		if (move.y < MAX_GRAVITY)
		{
			move.y = MAX_GRAVITY;
		}
	}
	//崖掴まり状態の場合
	else
	{
		//カウントが0になった場合
		if (m_nCntState == 0)
		{
			//強制的に落下状態にする
			m_State = STATE::FALL;
		}
	}

	//移動量を減衰
	move.x += -move.x * m_fInertia;
	move.z += -move.z * m_fInertia;

	//移動量を設定
	SetMove(move);

	//過去の座標を設定
	SetOldPos(pos);

	//移動量を加算して座標を設定
	SetPos(pos + move);
}

//===========================================================================================================
// ジャンプ処理
//===========================================================================================================
void CPlayer::Jump(D3DXVECTOR3& move, float fDiaJump)
{
	//インスタンス取得
	CManager* pManager = CManager::GetInstance();//マネージャー
	CSound* pSound = pManager->GetSound();//サウンド
	CCalculate* pCalculate = pManager->GetCalculate();//計算処理

	//ジャンプ
	if ((CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_SPACE) || 
		CManager::GetInstance()->GetJoypad()->GetTrigger(CInputJoypad::JOYKEY::JOYKEY_A)) && 
		m_State != STATE::WALL_SLIDE && 
		m_nCntJump > 0)
	{
		//ジャンプ力を加算
		move.y = fDiaJump - (JUMP_INERTIA * (JUMP_TIMES - m_nCntJump));

		//ジャンプカウンタを減らす
		m_nCntJump--;

		//残りのジャンプ回数が1以上
		if (m_nCntJump == JUMP_TIMES - 1)
		{
			//ジャンプ状態にする
			m_State = STATE::JUMP;

			//SE再生
			pSound->Play(CSound::SOUND::JUMP);
		}
		//残りのジャンプ回数が0
		else if (m_nCntJump == 0)
		{
			//二段ジャンプ状態にする
			m_State = STATE::DOUBLE_JUMP;

			//テンション量増加
			IncreaseHeat(10.0f);

			//コンボ数加算処理
			AddCombo();

			//2Dエフェクト表示
			DisplayEffect2D();

			//指定した数文繰り返す
			for (int nCnt = 0; nCnt < NUM_CREATE_PARTICLE; nCnt++)
			{
				//ランダムにカラーを設定
				float fR = pCalculate->RandFloat(0.1f, 1.0f);
				float fG = pCalculate->RandFloat(0.1f, 1.0f);
				float fB = pCalculate->RandFloat(0.1f, 1.0f);

				//パーティクル生成
				CParticle::Create(CParticle::TYPE::NORMAL, GetCenterPos(), { 25.0f, 25.0f, 0.0f }, 40, { fR, fG, fB, 1.0f });
			}

			//SE再生
			pSound->Play(CSound::SOUND::DOUBLE_JUMP);
		}

		//減衰強度を戻す
		m_fInertia = BASE_INERTIA;
	}
}

//===========================================================================================================
// 移動処理
//===========================================================================================================
void CPlayer::Move(D3DXVECTOR3& move, float fDiaSpeed, float fTargetRotY)
{
	//角度情報を取得
	D3DXVECTOR3 rot = GetRot();

	//スライディング状態の場合
	//壁キック状態の場合
	if (m_State == STATE::SLIDING || 
		m_State == STATE::WALL_KICK)
	{
		//処理を抜ける
		return;
	}

	//崖掴まり状態の場合
	else if (m_State == STATE::CLIFF_GRAB)
	{
		//崖側を向いていない場合
		if (rot.y != fTargetRotY)
		{
			//落下状態にする
			m_State = STATE::FALL;
		}
		//崖側を向いている場合
		else
		{
			//処理を抜ける
			return;
		}
	}

	//ヒート量に合わせて倍率を上げる
	float fHeat = 1.0f + (m_fHeat / MAX_HEAT) * 0.5f;

	//移動量を加算
	move.x += -sinf(fTargetRotY) * fDiaSpeed * fHeat;//X軸
	move.z += -cosf(fTargetRotY) * fDiaSpeed * fHeat;//Z軸

	//目標の角度を補正
	CorrectionRot(fTargetRotY);
	rot.y = fTargetRotY;

	//角度を設定
	SetRot(rot);
}

//===========================================================================================================
// ダッシュ処理
//===========================================================================================================
void CPlayer::Dash(float& fDiaSpeed, float& fFirstDiaSpeed)
{
	//インスタンス取得
	CManager* pManager = CManager::GetInstance();//マネージャー
	CInputKeyboard* pKeyBoard = pManager->GetKeyboard();//キーボード
	CInputJoypad* pJoyPad = pManager->GetJoypad();//ジョイパッド
	CCalculate* pCalculate = pManager->GetCalculate();//計算処理

	//2回入力受付許容フレームカウンタが1以上の場合
	if (m_nCntDoublePush > 0)
	{
		//カウンタを減らす
		m_nCntDoublePush--;
	}

	//ダッシュのフレームカウンタが1以上の場合
	if (m_nCntDash > 0)
	{
		//ダッシュ状態にする
		m_State = STATE::DASH;

		//カウンタを減らす
		m_nCntDash--;

		//速度倍率を上げる
		fDiaSpeed = fFirstDiaSpeed * DASH_SPEED;

		//減衰強度リセット
		m_fInertia = BASE_INERTIA;

		//指定した数文繰り返す
		for (int nCnt = 0; nCnt < 10; nCnt++)
		{
			//ランダムにカラーを設定
			float fR = pCalculate->RandFloat(0.1f, 1.0f);
			float fG = pCalculate->RandFloat(0.1f, 1.0f);
			float fB = pCalculate->RandFloat(0.1f, 1.0f);

			//パーティクル生成
			CParticle::Create(CParticle::TYPE::NORMAL, GetCenterPos(), { 40.0f, 40.0f, 0.0f }, 30, { fR, fG, fB, 1.0f });
		}
	}

	//崖掴まり状態か壁キック状態なら処理を抜ける
	if (m_State == STATE::CLIFF_GRAB || m_State == STATE::WALL_KICK) return;

	//移動キーが入力された場合
	if (pKeyBoard->GetTrigger(DIK_A) || 
		pKeyBoard->GetTrigger(DIK_D) ||
		pJoyPad->GetTrigger(CInputJoypad::JOYKEY::JOYKEY_LEFT) || 
		pJoyPad->GetTrigger(CInputJoypad::JOYKEY::JOYKEY_RIGHT))
	{
		//2回入力受付フレームカウンタが0の場合
		if (m_nCntDoublePush <= 0)
		{
			//2回入力受付フレームカウンタを設定
			m_nCntDoublePush = DOUBLE_PUSH;
		}
		//2回入力受付フレームカウンタが1以上の場合
		else
		{
			//ダッシュのフレームカウンタを設定
			m_nCntDash = DASH_FRAME;



			//テンション量増加
			IncreaseHeat(10.0f);

			//コンボ数加算処理
			AddCombo();

			//2Dエフェクト表示
			DisplayEffect2D();
		}
	}
}

//===========================================================================================================
// 発射処理
//===========================================================================================================
void CPlayer::Shot()
{
	//壁ずり中
	if (m_State == STATE::WALL_SLIDE)
	{
		//処理をしない
		return;
	}
	//壁キック中
	if (m_State == STATE::WALL_KICK && GetMotionFlag())
	{
		//処理をしない
		return;
	}

	//インスタンス取得
	CManager* pManager = CManager::GetInstance();//マネージャー
	CInputKeyboard* pKeyboard = pManager->GetKeyboard();//キーボード
	CInputJoypad* pJoypad = pManager->GetJoypad();//ジョイパッド

	//発射ボタンの入力判定がtrue
	if (pKeyboard->GetPress(DIK_F) || pJoypad->GetPress(CInputJoypad::JOYKEY::JOYKEY_B))
	{
		//ローカル変数宣言
		D3DXVECTOR3 BulletPos(0.0f, 0.0f, 0.0f);//発射地点
		ModelParts aParts = {};//パーツ情報保存用
		int nParts = 0;//パーツインデックス保存用

		//発射地点のX軸,Z軸を左手に設定
		nParts = static_cast<int>(PARTS::HandL);
		aParts = GetPartsInfo(nParts);
		BulletPos.x = aParts.WorldPos.x;
		BulletPos.z = aParts.WorldPos.z;

		//発射地点のY軸を上腕部分に設定
		nParts = static_cast<int>(PARTS::ArmUL);
		aParts = GetPartsInfo(nParts);
		BulletPos.y = aParts.WorldPos.y;

		//プレイヤーの情報を取得
		D3DXVECTOR3 rot = GetRot();//角度

		//弾の移動量設定
		D3DXVECTOR3 BulletMove(-sinf(rot.y) * BULLET_SPEED, 0.0f, -cosf(rot.y) * BULLET_SPEED);

		//等間隔で弾を発射
		if ((pKeyboard->GetRepeat(DIK_F, SHOT_SPEED) || pJoypad->GetRepeat(CInputJoypad::JOYKEY::JOYKEY_B, SHOT_SPEED)) &&
			!ShotPointCollisionBlock(BulletPos))//発射地点とブロックの当たり判定がfalse
		{
			//弾を生成
			CBullet::Create(BulletPos, BulletMove, rot, CBullet::TYPE::PLAYER);

			//発射音再生
			pManager->GetSound()->Play(CSound::SOUND::SHOT);

			//射撃フラグをtrueにする
			m_bShooting = true;

			//スライディング状態ではない場合
			if (m_State != STATE::SLIDING)
			{
				//発射状態にする
				m_State = STATE::SHOT;

				//減衰強度リセット
				m_fInertia = BASE_INERTIA;
			}
		}
	}
	//入力されていない場合
	else
	{
		//射撃フラグをfalseにする
		m_bShooting = false;

		//発射状態の場合
		if (m_State == STATE::SHOT)
		{
			//ニュートラル状態にする
			m_State = STATE::NEUTRAL;
		}
	}
}

//===========================================================================================================
// スライディング処理
//===========================================================================================================
void CPlayer::Sliding(D3DXVECTOR3& move)
{
	//インスタンス取得
	CManager* pManager = CManager::GetInstance();//マネージャー
	CCalculate* pCalculate = pManager->GetCalculate();//計算処理

	//センターパーツのインデックスを保存
	int nParts = static_cast<int>(PARTS::CENTER);

	//スライディング中の場合
	if (m_State == STATE::SLIDING)
	{
		//テンション量増加
		IncreaseHeat(0.25f);

		//センターパーツの調整用の座標を設定
		D3DXVECTOR3 pos(0.0f, 5.0f, 0.0f);

		//センターパーツのローカル座標を調整
		SetLocalPos(nParts, pos);

		//モーションが終了している場合
		if (!GetMotionFlag())
		{
			//ニュートラル状態にする
			m_State = STATE::NEUTRAL;
		}

		//煙発生処理
		OccurSmoke();

		//ランダムにカラーを設定
		float fR = pCalculate->RandFloat(0.1f, 1.0f);
		float fG = pCalculate->RandFloat(0.1f, 1.0f);
		float fB = pCalculate->RandFloat(0.1f, 1.0f);

		//パーティクル生成
		CParticle::Create(CParticle::TYPE::NORMAL, GetCenterPos(), { 35.0f, 35.0f, 0.0f }, 20, { fR, fG, fB, 1.0f });
	}
	//スライディング中ではない場合
	else
	{
		//センターパーツ情報を取得
		ModelParts aParts = GetPartsInfo(nParts);

		//ローカル座標を戻す
		SetLocalPos(nParts, aParts.FirstPos);
	}

	//地上で下入力されている場合
	if ((pManager->GetKeyboard()->GetTrigger(DIK_S) ||
		pManager->GetJoypad()->GetTrigger(CInputJoypad::JOYKEY::JOYKEY_DOWN)) &&
		GetLandingFlag() && 
		m_State != STATE::SLIDING)
	{
		//スライディング状態にする
		m_State = STATE::SLIDING;

		//パラメータ情報を取得
		D3DXVECTOR3 rot = GetRot();//角度
	
		//移動量を加算
		move.x -= sinf(rot.y) * ACCEL_SLIDING;//X軸
		move.z -= cosf(rot.y) * ACCEL_SLIDING;//Z軸

		//SE再生
		CManager::GetInstance()->GetSound()->Play(CSound::SOUND::SLIDING);
	}
}

//===========================================================================================================
// 着地中処理
//===========================================================================================================
void CPlayer::Landing(CBlock::CollisionParam& aParam)
{
	//インスタンス取得
	CManager* pManager = CManager::GetInstance();//マネージャー

	//ジャンプ回数リセット
	m_nCntJump = JUMP_TIMES;

	//減衰強度リセット
	m_fInertia = BASE_INERTIA;

	//ダメージ状態の場合処理を抜ける
	if (m_State == STATE::DAMAGE) return;

	//動いていない場合
	if (!CObject::MovingJudgeXZ(aParam.move) && 
		m_State != STATE::SHOT && 
		m_State != STATE::SLIDING)
	{
		//ニュートラル状態に設定
		m_State = STATE::NEUTRAL;
	}

	//動いている場合
	else if (CObject::MovingJudgeXZ(aParam.move) && 
		m_State != STATE::SHOT && 
		m_State != STATE::SLIDING && 
		m_nCntDash == 0)
	{
		//ニュートラル状態に設定
		m_State = STATE::LANDING_MOVE;
	}

	//スライディング中の場合
	else if (m_State == STATE::SLIDING)
	{
		//モーション情報を取得
		MotionSet aMotionSet = GetMotionSet(static_cast<int>(m_State));

		//坂道の場合
		if (aParam.type == CBlock::TYPE::SLONE && 
			(pManager->GetKeyboard()->GetPress(DIK_S) ||
				pManager->GetJoypad()->GetPress(CInputJoypad::JOYKEY::JOYKEY_DOWN)))
		{
			//移動量を減衰しない
			m_fInertia = -0.02f;

			//モーションをループさせる
			aMotionSet.nLoop = 1;
		}
		//上昇中or平行
		if (aParam.pos.y - aParam.oldpos.y >= 0.0f)
		{
			//減衰強度を下げる
			m_fInertia = BASE_INERTIA * 0.2f;

			//モーションをループさせない
			aMotionSet.nLoop = 0;
		}

		//モーション情報の設定
		SetMotionSet(static_cast<int>(m_State), aMotionSet);
	}
}

//===========================================================================================================
// 落下中処理
//===========================================================================================================
void CPlayer::Falling(CBlock::CollisionParam& aParam, WallState wallstate)
{
	//ジャンプの回数が残っている場合減算する
	if (m_nCntJump == JUMP_TIMES) m_nCntJump--;

	//壁ずり状態ではない場合
	//発射状態ではない場合
	if (m_State != STATE::WALL_SLIDE && 
		m_State != STATE::SHOT && 
		m_State != STATE::DAMAGE)
	{
		//落下状態にする
		m_State = STATE::FALL;
	}
	//壁ずり状態の場合
	else if (m_State == STATE::WALL_SLIDE)
	{
		//パーツインデックスを保存
		int nParts = static_cast<int>(PARTS::FACE);

		//顔のパーツ情報を取得
		ModelParts aParts = GetPartsInfo(nParts);

		//カメラの角度情報を取得
		float fCameraRot_Y = CManager::GetInstance()->GetCamera()->GetRot().y;

		//顔を正面に向ける
		aParts.rot.y = -aParam.rot.y + fCameraRot_Y;
		CorrectionRot(aParts.rot.y);

		//更新されたパーツ情報を設定
		SetPartsInfo(nParts, aParts);

		//壁の判定は壁ずりではない場合
		if (wallstate != WallState::SLIDE)
		{
			//落下状態にする
			m_State = STATE::FALL;
		}
	}

	//減衰強度リセット
	m_fInertia = BASE_INERTIA;
}

//===========================================================================================================
// 壁キック処理
//===========================================================================================================
CPlayer::WallState CPlayer::WallKick(CBlock* pBlock, CBlock::CollisionParam& aParam)
{
	//インスタンス取得
	CManager* pManager = CManager::GetInstance();//マネージャー
	CInputKeyboard* pKeyboard = pManager->GetKeyboard();//キーボード
	CInputJoypad* pJoypad = pManager->GetJoypad();//ジョイパッド

	//ローカル変数宣言
	WallState wallstate = WallState::NONE;//壁の接触状態
	WallLine Line = WallLine::NONE;//触れた壁の線上
	D3DXVECTOR3 BlockPos = pBlock->GetPos();//ブロック座標
	D3DXVECTOR3 BlockSize = pBlock->GetSize();//ブロックサイズ
	bool bCollision = false;//壁との接触判定フラグ
	bool bCliff = false;//崖掴まり判定

	//移動キーが押されている場合
	if (pKeyboard->GetPress(DIK_D) ||
		pJoypad->GetPress(CInputJoypad::JOYKEY::JOYKEY_RIGHT) ||
		pKeyboard->GetPress(DIK_A) ||
		pJoypad->GetPress(CInputJoypad::JOYKEY::JOYKEY_LEFT))
	{
		//左側の判定
		if ((aParam.pos.x + aParam.size.x * 0.5f >= BlockPos.x - BlockSize.x * 0.5f && 
				aParam.oldpos.x + aParam.size.x * 0.5f <= BlockPos.x - BlockSize.x * 0.5f) && 
			(aParam.pos.y + aParam.size.y > BlockPos.y - BlockSize.y * 0.5f && 
				aParam.pos.y < BlockPos.y + BlockSize.y * 0.5f) && 
			(aParam.pos.z + aParam.size.z * 0.5f > BlockPos.z - BlockSize.z * 0.5f && 
				aParam.pos.z - aParam.size.z * 0.5f < BlockPos.z + BlockSize.z * 0.5f))
		{
			//ブロックの垂直ベクトルを求める
			pBlock->SEGMENT_XY(BlockPos.x, BlockPos.y - BlockSize.y * 0.5f, BlockPos.x, BlockPos.y + BlockSize.y * 0.5f);

			//接触判定をtrueにする
			bCollision = true;

			//触れた線上をXYに設定
			Line = WallLine::XY;

			//プレイヤーの頂点がブロックよりも高い場合
			if (aParam.pos.y + aParam.size.y >= BlockPos.y + BlockSize.y * 0.5f - 10.0f && 
				aParam.pos.y < BlockPos.y + BlockSize.y * 0.5f - 10.0f)
			{
				//崖掴まり判定をtrueにする
				bCliff = true;

				//カメラの角度を取得
				float fCamera_Rot_Y = pManager->GetCamera()->GetRot().y;

				//角度の加算量
				float fAddRot_Y = 0.0f;

				//カメラの角度の値が正の値の場合
				if (fCamera_Rot_Y >= 0.0f)
				{
					//加算量を設定
					fAddRot_Y = -D3DX_PI * 0.5f;
				}
				//カメラの角度の値が負の値の場合
				else
				{
					//加算量を設定
					fAddRot_Y = D3DX_PI * 0.5f;
				}

				//角度を右向きに調整
				aParam.rot.y = pManager->GetCamera()->GetRot().y + fAddRot_Y;
			}
		}

		//右側の判定
		else if ((aParam.pos.x - aParam.size.x * 0.5f <= BlockPos.x + BlockSize.x * 0.5f && 
					aParam.oldpos.x - aParam.size.x * 0.5f >= BlockPos.x + BlockSize.x * 0.5f) && 
			(aParam.pos.y + aParam.size.y > BlockPos.y - BlockSize.y * 0.5f && 
				aParam.pos.y < BlockPos.y + BlockSize.y * 0.5f) && 
			(aParam.pos.z + aParam.size.z * 0.5f > BlockPos.z - BlockSize.z * 0.5f && 
				aParam.pos.z - aParam.size.z * 0.5f < BlockPos.z + BlockSize.z * 0.5f))
		{
			//ブロックの垂直ベクトルを求める
			pBlock->SEGMENT_XY(BlockPos.x, BlockPos.y + BlockSize.y * 0.5f, BlockPos.x, BlockPos.y - BlockSize.y * 0.5f);

			//接触判定をtrueにする
			bCollision = true;

			//触れた線上をXYに設定
			Line = WallLine::XY;

			//プレイヤーの頂点がブロックよりも高い場合
			if (aParam.pos.y + aParam.size.y >= BlockPos.y + BlockSize.y * 0.5f - 10.0f
				&& aParam.pos.y < BlockPos.y + BlockSize.y * 0.5f - 10.0f)
			{
				//崖掴まり判定をtrueにする
				bCliff = true;

				//カメラの角度を取得
				float fCamera_Rot_Y = pManager->GetCamera()->GetRot().y;

				//角度の加算量
				float fAddRot_Y = 0.0f;

				//カメラの角度の値が正の値の場合
				if (fCamera_Rot_Y >= 0.0f)
				{
					//加算量を設定
					fAddRot_Y = D3DX_PI * 0.5f;
				}
				//カメラの角度の値が負の値の場合
				else
				{
					//加算量を設定
					fAddRot_Y = -D3DX_PI * 0.5f;
				}

				//角度を右向きに調整
				aParam.rot.y = pManager->GetCamera()->GetRot().y + fAddRot_Y;
			}
		}

		//手前側の判定
		else if ((aParam.pos.z + aParam.size.z * 0.5f >= BlockPos.z - BlockSize.z * 0.5f && 
					aParam.oldpos.z + aParam.size.z * 0.5f <= BlockPos.z - BlockSize.z * 0.5f) && 
			(aParam.pos.y + aParam.size.y > BlockPos.y - BlockSize.y * 0.5f && 
				aParam.pos.y < BlockPos.y + BlockSize.y * 0.5f) && 
			(aParam.pos.x + aParam.size.x * 0.5f > BlockPos.x - BlockSize.x * 0.5f && 
				aParam.pos.x - aParam.size.x * 0.5f < BlockPos.x + BlockSize.x * 0.5f))
		{
			//ブロックの垂直ベクトルを求める
			pBlock->SEGMENT_YZ(BlockPos.y - BlockSize.y * 0.5f, BlockPos.z, BlockPos.y + BlockSize.y * 0.5f, BlockPos.z);

			//接触判定をtrueにする
			bCollision = true;

			//触れた線上をYZに設定
			Line = WallLine::YZ;

			//プレイヤーの頂点がブロックよりも高い場合
			if (aParam.pos.y + aParam.size.y >= BlockPos.y + BlockSize.y * 0.5f - 10.0f
				&& aParam.pos.y < BlockPos.y + BlockSize.y * 0.5f - 10.0f)
			{
				//崖掴まり判定をtrueにする
				bCliff = true;

				//カメラの角度を取得
				float fCamera_Rot_Y = pManager->GetCamera()->GetRot().y;

				//角度の加算量
				float fAddRot_Y = 0.0f;

				//カメラの角度の値が正の値の場合
				if (fCamera_Rot_Y >= 0.0f)
				{
					//加算量を設定
					fAddRot_Y = D3DX_PI * 0.5f;
				}
				//カメラの角度の値が負の値の場合
				else
				{
					//加算量を設定
					fAddRot_Y = -D3DX_PI * 0.5f;
				}

				//角度を右向きに調整
				aParam.rot.y = pManager->GetCamera()->GetRot().y + fAddRot_Y;
			}
		}

		//奥側の判定
		else if ((aParam.pos.z - aParam.size.z * 0.5f <= BlockPos.z + BlockSize.z * 0.5f
			&& aParam.oldpos.z - aParam.size.z * 0.5f >= BlockPos.z + BlockSize.z * 0.5f)
			&& (aParam.pos.y + aParam.size.y > BlockPos.y - BlockSize.y * 0.5f
				&& aParam.pos.y < BlockPos.y + BlockSize.y * 0.5f)
			&& (aParam.pos.x + aParam.size.x * 0.5f > BlockPos.x - BlockSize.x * 0.5f
				&& aParam.pos.x - aParam.size.x * 0.5f < BlockPos.x + BlockSize.x * 0.5f))
		{
			//ブロックの垂直ベクトルを求める
			pBlock->SEGMENT_YZ(BlockPos.y + BlockSize.y * 0.5f, BlockPos.z, BlockPos.y - BlockSize.y * 0.5f, BlockPos.z);

			//接触判定をtrueにする
			bCollision = true;

			//触れた線上をYZに設定
			Line = WallLine::YZ;

			//プレイヤーの頂点がブロックよりも高い場合
			if (abs((aParam.pos.y + aParam.size.y) - (BlockPos.y + BlockSize.y * 0.5f)) <= 10.0f)
			{
				//崖掴まり判定をtrueにする
				bCliff = true;

				//カメラの角度を取得
				float fCamera_Rot_Y = pManager->GetCamera()->GetRot().y;

				//角度の加算量
				float fAddRot_Y = 0.0f;

				//カメラの角度の値が正の値の場合
				if (fCamera_Rot_Y >= 0.0f)
				{
					//加算量を設定
					fAddRot_Y = -D3DX_PI * 0.5f;
				}
				//カメラの角度の値が負の値の場合
				else
				{
					//加算量を設定
					fAddRot_Y = D3DX_PI * 0.5f;
				}

				//角度を右向きに調整
				aParam.rot.y = pManager->GetCamera()->GetRot().y + fAddRot_Y;
			}
		}
	}

	//接触判定がtrueの場合
	if (bCollision)
	{
		//反射ベクトル計算処理
		ReflectAngleCalculate(pBlock, aParam, Line, wallstate);
	}

	return wallstate;
}

//===========================================================================================================
// 反射ベクトル計算処理処理
//===========================================================================================================
void CPlayer::ReflectAngleCalculate(CBlock* pBlock, CBlock::CollisionParam& aParam, WallLine Line, WallState& wallstate)
{
	//ローカル変数宣言
	CManager* pManager = CManager::GetInstance();//マネージャーインスタンス
	CCalculate* pCalculate = pManager->GetCalculate();//計算処理インスタンス
	D3DXVECTOR3 Normal = pBlock->GetNormal();//垂直ベクトル
	float fKickPowerWidth = 0.0f;//壁キックの横の強さ
	float fKickPowerHeight = 0.0f;//壁キックの縦の強さ
	float fReflectAngle = 0.0f;//反射ベクトルの角度

	//方向ベクトルを求める
	D3DXVECTOR3 Direction = aParam.pos - GetOldPos();

	//進行ベクトルを正規化
	D3DXVec3Normalize(&Direction, &Direction);

	//係数を求める
	D3DXVECTOR3 Factor(-Direction.x * Normal.x, -Direction.y * Normal.y, -Direction.z * Normal.z);

	//係数を正規化
	D3DXVec3Normalize(&Factor, &Factor);

	//平行ベクトルを求める
	D3DXVECTOR3 Parallel(Direction.x + Factor.x * Normal.x, Direction.y + Factor.y * Normal.y, Direction.z + Factor.z * Normal.z);

	//平行ベクトルを正規化
	D3DXVec3Normalize(&Parallel, &Parallel);

	//反射ベクトルを求める
	D3DXVECTOR3 Reflect(Direction.x + 2.0f * Factor.x * Normal.x, Direction.y + 2.0f * Factor.y * Normal.y, Direction.z + 2.0f * Factor.z * Normal.z);

	//反射ベクトルを正規化
	D3DXVec3Normalize(&Reflect, &Reflect);

	//ジャンプボタンが押された場合
	if (pManager->GetKeyboard()->GetTrigger(DIK_SPACE) || 
		pManager->GetJoypad()->GetTrigger(CInputJoypad::JOYKEY::JOYKEY_A))
	{
		//反射のパワーを設定
		fKickPowerWidth = MAX_KICKPOWER_WIDTH;
		fKickPowerHeight = MAX_KICKPOWER_HEIGHT;

		//壁キック状態に設定
		wallstate = WallState::KICK;

		//コンボ数加算処理
		AddCombo();

		//2Dエフェクト表示
		DisplayEffect2D();

		//指定した数文繰り返す
		for (int nCnt = 0; nCnt < NUM_CREATE_PARTICLE; nCnt++)
		{
			//ランダムにカラーを設定
			float fR = pCalculate->RandFloat(0.1f, 1.0f);
			float fG = pCalculate->RandFloat(0.1f, 1.0f);
			float fB = pCalculate->RandFloat(0.1f, 1.0f);

			//生成座標
			D3DXVECTOR3 CreatePos(aParam.pos.x, aParam.pos.y + aParam.size.y * 0.5f, aParam.pos.z);

			//パーティクル生成
			CParticle::Create(CParticle::TYPE::NORMAL, CreatePos, { 25.0f, 25.0f, 0.0f }, 40, { fR, fG, fB, 1.0f });
		}

		//目標の角度を設定
		m_fTargetRot_Y = GetRot().y + D3DX_PI;

		//角度補正
		CObject::CorrectionRot(m_fTargetRot_Y);

		//壁キック回転速度設定処理
		SetWallKickTurn();

		//触れた線上がXYの場合
		if (Line == WallLine::XY)
		{
			//反射ベクトルのX成分に対して、角度を変更
			if (Reflect.x < 0.0f)
			{
				fReflectAngle = 1.78f;
			}
			else
			{
				fReflectAngle = -1.78f;
			}
		}
		//触れた線上がYZの場合
		else if (Line == WallLine::YZ)
		{
			//反射ベクトルのX成分に対して、角度を変更
			if (Reflect.z < 0.0f)
			{
				fReflectAngle = 1.78f;
			}
			else
			{
				fReflectAngle = -1.78f;
			}
		}
	}
	else
	{
		//頂点部がブロックよりも高い位置の場合
		if (aParam.pos.y + GetSize().y > pBlock->GetPos().y + pBlock->GetSize().y * 0.5f) return;

		//反射のパワーを設定
		fKickPowerHeight = SLIDE_POWER;

		//壁ずり状態に設定
		wallstate = WallState::SLIDE;
	}

	//線上と角度に合わせて移動量を変更
	if (Line == WallLine::XY)
	{
		aParam.move = { sinf(fReflectAngle + D3DX_PI) * fKickPowerWidth, cosf(fReflectAngle + D3DX_PI) * fKickPowerHeight, 0.0f };
	}
	else if (Line == WallLine::YZ)
	{
		aParam.move = { 0.0f, cosf(fReflectAngle + D3DX_PI) * fKickPowerHeight, sinf(fReflectAngle + D3DX_PI) * fKickPowerWidth };
	}
}

//===========================================================================================================
// テンション量増加処理
//===========================================================================================================
void CPlayer::IncreaseHeat(float fAdd)
{
	//テンション量を増加
	m_fHeat += fAdd;

	//テンション量が最大値を超えた場合
	if (m_fHeat > MAX_HEAT)
	{
		//最大値に調整
		m_fHeat = MAX_HEAT;
	}

	//コンボ継続フレームを設定
	m_nComboTime = COMBO_TIME;
}

//===========================================================================================================
// テンション量減少処理
//===========================================================================================================
void CPlayer::DecreaseHeat()
{
	//コンボタイム継続中
	if (m_nComboTime > 0)
	{
		m_nComboTime--;
		return;
	}

	// クールタイムが1以上の場合
	if (m_nDecTenCT > 0)
	{
		// クールタイム減少
		m_nDecTenCT--;
	}

	// クールタイムが0の場合
	if (m_nDecTenCT == 0)
	{
		// クールタイムリセット
		m_nDecTenCT = DEC_HEAT_CT;

		// テンション量が1以上の場合
		if (m_fHeat > 0.0f)
		{
			// テンション量減少
			m_fHeat -= 1.0f;
		}

		// テンション量が0.0fを下回らないように調整
		if (m_fHeat < 0.0f)
		{
			m_fHeat = 0.0f;
		}
	}
}

//===========================================================================================================
// コンボ数加算処理
//===========================================================================================================
void CPlayer::AddCombo()
{
	//コンボ数が最大ではない
	if (m_nCombo < MAX_COMBO)
	{
		//コンボ数を加算
		m_nCombo++;

		//カウンタリセット
		m_nDecComboFrame = 0;
	}
}

//===========================================================================================================
// コンボ数リセット処理
//===========================================================================================================
void CPlayer::ResetCombo()
{
	//コンボ数が0の場合は処理しない
	if (m_nCombo <= 0) return;

	//カウンタを加算
	m_nDecComboFrame++;

	//カウンタが最大
	if (m_nDecComboFrame >= DECREASE_COMBO_FRAME)
	{
		//コンボ数リセット
		m_nCombo = 0;

		//カウンタリセット
		m_nDecComboFrame = 0;

		//2Dエフェクトのポインタ取得
		CEffect2D* pEffect2D = CEffect2D::GetEffect2D();

		//2Dエフェクトを削除
		if (pEffect2D != nullptr)
		{
			//2Dエフェクト終了処理
			pEffect2D->Uninit();
		}
	}
}

//===========================================================================================================
// ヒートエフェクト発生処理
//===========================================================================================================
void CPlayer::OccurHeatEffect()
{
	//計算処理インスタンス取得
	CCalculate* pCalculate = CManager::GetInstance()->GetCalculate();

	//座標を取得
	D3DXVECTOR3 PlayerPos = GetPos();

	//生成Y座標設定
	float CreatePos_Y = PlayerPos.y;

	//生成X座標設定
	float CreatePos_X = pCalculate->RandFloat(PlayerPos.x - RANGE_EFFECT, PlayerPos.x + RANGE_EFFECT);

	//生成Z座標設定
	float CreatePos_Z = pCalculate->RandFloat(PlayerPos.z - RANGE_EFFECT, PlayerPos.z + RANGE_EFFECT);

	//生成座標設定
	D3DXVECTOR3 CreatePos(CreatePos_X, CreatePos_Y, CreatePos_Z);

	//現在のヒート量の割合を求める
	float fRatioHeat = m_fHeat / MAX_HEAT * 10.0f;

	//割合に合わせてパーティクルを発生
	for (int nCnt = 0; nCnt < static_cast<int>(fRatioHeat); nCnt++)
	{
		CParticle::Create(CParticle::TYPE::ASCEND, CreatePos, { 15.0f, 15.0f, 0.0f }, 20, { 1.0f, 0.0f, 0.0f, 1.0f });
	}
}

//===========================================================================================================
// 煙発生処理
//===========================================================================================================
void CPlayer::OccurSmoke()
{
	//計算処理インスタンス取得
	CCalculate* pCalculate = CManager::GetInstance()->GetCalculate();

	//座標を取得
	D3DXVECTOR3 PlayerPos = GetPos();

	//生成Y座標設定
	float CreatePos_Y = PlayerPos.y;

	//生成X座標設定
	float CreatePos_X = pCalculate->RandFloat(PlayerPos.x - RANGE_EFFECT, PlayerPos.x + RANGE_EFFECT);

	//生成Z座標設定
	float CreatePos_Z = pCalculate->RandFloat(PlayerPos.z - RANGE_EFFECT, PlayerPos.z + RANGE_EFFECT);

	//生成座標設定
	D3DXVECTOR3 CreatePos(CreatePos_X, CreatePos_Y, CreatePos_Z);

	//煙エフェクトを発生
	CSmoke::Create(CreatePos, { 20.0f, 20.0f, 0.0f }, 30);
}

//===========================================================================================================
// 2Dエフェクト表示処理
//===========================================================================================================
void CPlayer::DisplayEffect2D()
{
	//インスタンス取得
	CManager* pManager = CManager::GetInstance();//マネージャー
	CUIManager* pUIManager = pManager->GetScene()->GetUIManager();//UIマネージャー
	CCalculate* pCalculate = pManager->GetCalculate();//計算処理

	//インスタンスがnullptr
	if (pUIManager == nullptr)
	{
		return;
	}

	//ヒートゲージフレームを検索
	CUI* pUI = pUIManager->FindUI(CUI::TYPE::HEAT_GAGE_FRAME);

	//ヒートゲージフレームのパラメータ取得
	D3DXVECTOR2 UIpos = pUI->GetPos();//座標
	D3DXVECTOR2 UIsize = pUI->GetSize();//サイズ

	//生成座標設定
	D3DXVECTOR2 CreatePos(UIpos.x, UIpos.y - UIsize.y * 0.5f);

	//向きをランダムに設定
	float fRot_Z = pCalculate->RandFloat(-RANGE_EFFECT2D_ROT, RANGE_EFFECT2D_ROT);

	//エフェクト表示
	CEffect2D::Create(static_cast<CEffect2D::TYPE>(m_nCombo), CreatePos, { 0.0f, 0.0f, fRot_Z });
}

//===========================================================================================================
// 点滅処理
//===========================================================================================================
void CPlayer::Flash()
{
	//ローカル変数宣言
	static bool bLightUp = false;

	//ライトアップ中の場合
	if (bLightUp)
	{
		//モデルを明るく設定
		SetPartsFlash(1.0f);
	}
	else
	{
		//モデルを暗く設定
		SetPartsFlash(0.1f);
	}

	//ライトアップフラグを切り替える
	bLightUp = bLightUp ? false : true;

	//状態カウンタが0の場合
	if (m_nCntState == 0)
	{
		//通常状態にする
		m_State = STATE::NEUTRAL;
	}
}

//===========================================================================================================
// 当たり判定
//===========================================================================================================
void CPlayer::Collision()
{
	//ブロックとの当たり判定
	CollisionBlock();

	//エリアとの当たり判定
	CollisionArea();

	//エネミーとの当たり判定
	CollisionEnemy();

	//ダメージ状態の場合
	if (m_State == STATE::DAMAGE)
	{
		//点滅処理
		Flash();
	}
	//ダメージ状態ではない場合
	else
	{
		//モデルの明るさを戻す
		SetPartsFlash(1.0f);

		//ボスとの当たり判定
		CollisionBoss();

		//ボスの攻撃との当たり判定
		CollisionBossAttack();
	}
}

//===========================================================================================================
// ブロックとの当たり判定
//===========================================================================================================
void CPlayer::CollisionBlock()
{
	//当たり判定用パラメータ設定
	CBlock::CollisionParam aParam;
	aParam.pos = GetPos();//座標
	aParam.oldpos = GetOldPos();//過去の座標
	aParam.size = GetSize();//サイズ
	aParam.move = GetMove();//移動量
	aParam.rot = GetRot();//角度
	aParam.type = CBlock::TYPE::NONE;//当たったブロックのタイプ

	//壁キック判定処理
	WallState wallstate = CollisionWallKick(aParam);

	//上側の判定を取得
	CBlock::CollisionFlag aFlag = CManager::GetInstance()->GetScene()->GetBlockManager()->Collision(aParam);
	SetLandingFlag(aFlag.Y_UP);

	//X軸かZ軸の判定がtrue
	if (aFlag.X || aFlag.Z)
	{
		//ダッシュ状態を解除
		m_nCntDash = 0;
	}

	//Y軸の判定がtrue
	if (aFlag.Y_UP)
	{
		//着地中処理
		Landing(aParam);
	}
	//落ちている場合
	else if(!aFlag.Y_UP && aParam.move.y < 0.00f)
	{
		//落下中処理
		Falling(aParam, wallstate);
	}

	//更新されたパラメータを設定
	SetPos(aParam.pos);//座標
	SetMove(aParam.move);//移動量
	SetRot(aParam.rot);//角度
}

//===========================================================================================================
// 壁キック判定処理
//===========================================================================================================
CPlayer::WallState CPlayer::CollisionWallKick(CBlock::CollisionParam& aParam)
{
	//ローカル変数宣言
	WallState wallstate = WallState::NONE;

	//ブロックの先頭アドレス取得
	CBlock* pBlock = CManager::GetInstance()->GetScene()->GetBlockManager()->GetTop();

	//オブジェクト情報がnullptrになるまで繰り返す
	while (pBlock != nullptr)
	{
		//次のオブジェクト情報を取得
		CBlock* pNext = pBlock->GetNextBlock();

		//ブロックタイプが坂道ではない場合
		if ((pBlock != nullptr && pBlock->GetBlockType() != CBlock::TYPE::SLONE) && !GetLandingFlag())
		{
			//壁判定処理
			wallstate = WallKick(pBlock, aParam);

			//壁の状態がNONEではない場合ループを終了
			if (wallstate != WallState::NONE) break;
		}

		//オブジェクト情報を次の情報に変更する
		pBlock = pNext;
	}

	//壁の判定ごとに状態を変化
	switch (wallstate)
	{
		//壁ずり
	case WallState::SLIDE:
		//プレイヤーを壁ずり状態に設定
		m_State = STATE::WALL_SLIDE;
		break;

		//壁キック
	case WallState::KICK:
		//プレイヤーを壁キック状態に設定
		m_State = STATE::WALL_KICK;

		//減衰強度を弱める
		m_fInertia = BASE_INERTIA * 0.05f;

		//テンション量増加
		IncreaseHeat(10.0f);
		break;

	default:
		break;
	}

	return wallstate;
}

//===========================================================================================================
// エネミーとの当たり判定
//===========================================================================================================
void CPlayer::CollisionEnemy()
{
	//当たり判定用パラメータ設定
	CEnemy::CollisionParam aParam;
	aParam.pos = GetPos();//座標
	aParam.size = GetSize();//サイズ
	aParam.pEnemy = nullptr;//当たったエネミーの情報

	//エネミーとの当たり判定がtrueの場合
	if (CManager::GetInstance()->GetScene()->GetEnemyManager()->Collision(aParam) && 
		aParam.pEnemy != nullptr)
	{
		//ダメージ状態中に通常の敵に当たった場合
		if (aParam.pEnemy->GetEnemyType() != CEnemy::TYPE::BALL && 
			m_State == STATE::DAMAGE)
		{
			//当たったエネミーの情報のポインタをnullptrにする
			aParam.pEnemy = nullptr;

			//処理を抜ける
			return;
		}
		//ボールと衝突した場合
		else if (aParam.pEnemy->GetEnemyType() == CEnemy::TYPE::BALL)
		{
			//チェイスエリアリスポーン処理
			RespawnChaseArea();

			//処理を抜ける
			return;
		}

		//ダメージ状態にする
		m_State = STATE::DAMAGE;

		//状態カウンタを設定
		m_nCntState = 30;

		//エネミーの位置情報を取得
		D3DXVECTOR3 EnemyPos = aParam.pEnemy->GetPos();

		//プレイヤーの移動量情報取得
		D3DXVECTOR3 PlayerMove = GetMove();

		//方向ベクトルを求める
		D3DXVECTOR3 Direction = GetPos() - EnemyPos;
		D3DXVec3Normalize(&Direction, &Direction);

		//移動量情報更新
		PlayerMove.x = Direction.x * 50.0f;
		PlayerMove.y = Direction.y;
		PlayerMove.z = Direction.z * 50.0f;

		//更新された移動量情報を設定
		SetMove(PlayerMove);
	}

	//当たったエネミーの情報のポインタをnullptrにする
	aParam.pEnemy = nullptr;
}

//===========================================================================================================
// ボスとの当たり判定
//===========================================================================================================
void CPlayer::CollisionBoss()
{
	//ボスのオブジェクト情報を検索
	CObject* pFindObj = CObject::FindObject(CObject::Category::BOSS);
	CBoss* pBoss = nullptr;

	//検索結果が合致した場合ダウンキャスト
	if (pFindObj != nullptr) pBoss = CObject::DownCast<CBoss, CObject>(pFindObj);

	//当たり判定用パラメータ設定
	CBoss::CollisionParam aParam = {};
	aParam.pos = GetPos();//座標
	aParam.size = GetSize();//サイズ

	//ボスとの当たり判定がtrueの場合
	if (pBoss != nullptr && pBoss->Collision(aParam))
	{
		//ダメージ状態にする
		m_State = STATE::DAMAGE;

		//状態カウンタを設定
		m_nCntState = DAMAGE_TIME;

		//エネミーの位置情報を取得
		D3DXVECTOR3 BossPos = pBoss->GetPos();

		//プレイヤーの移動量情報取得
		D3DXVECTOR3 PlayerMove = GetMove();

		//方向ベクトルを求める
		D3DXVECTOR3 Direction = aParam.pos - BossPos;
		D3DXVec3Normalize(&Direction, &Direction);

		//移動量情報更新
		PlayerMove.x = Direction.x * 50.0f;
		PlayerMove.y = Direction.y;
		PlayerMove.z = Direction.z * 50.0f;

		//更新された移動量情報を設定
		SetMove(PlayerMove);
	}
}

//===========================================================================================================
// ボスの攻撃との当たり判定
//===========================================================================================================
void CPlayer::CollisionBossAttack()
{
	//ボスの攻撃のオブジェクト情報を検索
	CObject* pFindObj = CObject::FindObject(CObject::Category::BOSS_ATTACK);
	CBossAttack* pBossAttack = nullptr;

	//検索結果が合致した場合ダウンキャスト
	if (pFindObj != nullptr) pBossAttack = CObject::DownCast<CBossAttack, CObject>(pFindObj);

	//当たり判定用パラメータ設定
	CBossAttack::CollisionParam aParam = {};
	aParam.pos = GetPos();//座標
	aParam.size = GetSize();//サイズ

	//ボスとの当たり判定がtrueの場合
	if (pBossAttack != nullptr && pBossAttack->Collision(aParam))
	{
		//ダメージ状態にする
		m_State = STATE::DAMAGE;

		//状態カウンタを設定
		m_nCntState = DAMAGE_TIME;

		//エネミーの位置情報を取得
		D3DXVECTOR3 BossAttackPos = pBossAttack->GetPos();

		//プレイヤーの移動量情報取得
		D3DXVECTOR3 PlayerMove = GetMove();

		//方向ベクトルを求める
		D3DXVECTOR3 Direction = GetPos() - BossAttackPos;
		D3DXVec3Normalize(&Direction, &Direction);

		//移動量情報更新
		PlayerMove.x = Direction.x * 50.0f;
		PlayerMove.y = Direction.y;
		PlayerMove.z = Direction.z * 50.0f;

		//更新された移動量情報を設定
		SetMove(PlayerMove);
	}
}

//===========================================================================================================
// 発射地点のブロック当たり判定
//===========================================================================================================
bool CPlayer::ShotPointCollisionBlock(const D3DXVECTOR3& pos)
{
	//インスタンス取得
	CManager* pManager = CManager::GetInstance();//マネージャー
	CCollision* pCollision = pManager->GetCollision();//当たり判定処理

	//ブロックの先頭情報を取得
	CBlock* pBlock = pManager->GetScene()->GetBlockManager()->GetTop();

	//末尾まで繰り返す
	while (pBlock != nullptr)
	{
		//次の情報を保存
		CBlock* pNext = pBlock->GetNextBlock();

		//当たり判定がtrue
		if (pBlock->GetBlockType() == CBlock::TYPE::NORMAL && //ブロックタイプがノーマル
			pCollision->PointBox3D(pos, pBlock->GetPos(), pBlock->GetSize()))
		{
			//trueを返す
			return true;
		}

		//次の情報を取得
		pBlock = pNext;
	}

	//末尾まで当たり判定がfalse
	return false;
}

//===========================================================================================================
// エリアとの当たり判定
//===========================================================================================================
void CPlayer::CollisionArea()
{
	CollisionMiniGameArea();//ミニゲームエリア
	CollisionChaseArea();//チェイスエリア
	CollisionBossArea();//ボスエリア
}

//ミニゲームエリア
void CPlayer::CollisionMiniGameArea()
{
	//ゲームシーンのインスタンス取得
	CGame* pGame = CGame::GetInstance();

	//インスタンス取得失敗
	if (pGame == nullptr)
	{
		return;
	}

	//プレイヤー座標取得
	D3DXVECTOR3 PlayerPos = GetPos();

	//ミニゲームエリアを検索
	CArea* pArea = CManager::GetInstance()->GetScene()->GetAreaManager()->FindArea(CArea::TYPE::MINI_GAME);

	//エリアとの当たり判定がtrue
	if (pArea != nullptr &&
		pArea->GetAreaType() == CArea::TYPE::MINI_GAME &&
		pArea->Collision(PlayerPos))
	{
		//ミニゲーム中ではないならイベント管理を生成
		if (pGame->GetArea() != CGame::GAME_AREA::MINI_GAME)
		{
			CMiniGame::Create();
		}

		//現在のエリアをミニゲームエリアに設定
		pGame->SetArea(CGame::GAME_AREA::MINI_GAME);
	}

	//ミニゲーム中にエリア外に出た場合過去の位置に戻す
	if (pGame->GetArea() == CGame::GAME_AREA::MINI_GAME && !pArea->Collision(PlayerPos))
	{
		SetPos(GetOldPos());
	}
}

//チェイスエリア
void CPlayer::CollisionChaseArea()
{
	//シーンのインスタンス取得
	CScene* pScene = CManager::GetInstance()->GetScene();

	//プレイヤー座標取得
	D3DXVECTOR3 PlayerPos = GetPos();

	//チェイスエリアが存在しない
	if (pScene->GetAreaManager() == nullptr)
	{
		return;
	}

	//チェイスエリアを検索
	CArea* pArea = pScene->GetAreaManager()->FindArea(CArea::TYPE::CHASE);

	//エリアとの当たり判定がtrue
	if (pArea != nullptr &&
		pArea->GetAreaType() == CArea::TYPE::CHASE &&
		pArea->Collision(PlayerPos))
	{
		//ボールスポナーを検索
		CGimmick* pGimmick = pScene->GetGimmickManager()->Find(CGimmick::TYPE::BALL_SPAWNER);

		//検索結果が合致
		if (pGimmick != nullptr && pGimmick->GetType() == CGimmick::TYPE::BALL_SPAWNER)
		{
			//ダウンキャスト
			CBallSpawner* pBallSpawner = CObject::DownCast<CBallSpawner, CGimmick>(pGimmick);

			//ダウンキャスト成功した場合ボールをスポーン
			if (pBallSpawner != nullptr)
			{
				pBallSpawner->Spawn();
			}
		}
	}
}

//ボスエリア
void CPlayer::CollisionBossArea()
{
	//ゲームシーンのインスタンス取得
	CGame* pGame = CGame::GetInstance();

	//インスタンス取得失敗
	if (pGame == nullptr)
	{
		return;
	}

	//プレイヤー座標取得
	D3DXVECTOR3 PlayerPos = GetPos();

	//ボスエリアを検索
	CArea* pArea = CManager::GetInstance()->GetScene()->GetAreaManager()->FindArea(CArea::TYPE::BOSS);

	//エリアとの当たり判定がtrueの場合、現在のエリアをミニゲームエリアに設定
	if (pArea != nullptr &&
		pArea->GetAreaType() == CArea::TYPE::BOSS &&
		pArea->Collision(PlayerPos) &&
		!m_bBossCreate)
	{
		//ボスオブジェクト検索
		CObject* pObj = CObject::FindObject(Category::BOSS);

		//検索結果が不一致
		if (pObj == nullptr || pObj->GetCategory() != Category::BOSS)
		{
			return;
		}

		//ボスクラスにダウンキャスト
		CBoss* pBoss = DownCast<CBoss, CObject>(pObj);

		//UI生成用パラメータ
		D3DXVECTOR2 FirstPos(1210.0f, 667.0f);//座標
		D3DXVECTOR2 size(30.0f, 20.0f);//サイズ
		D3DXVECTOR3 rot(0.0f, 0.0f, 0.0f);//角度

		//体力のフレームを生成
		CUI::Create(CUI::TYPE::BOSS_HP_FRAME, { FirstPos.x, 445.0f }, { size.x + 12.0f, 480.0f }, { 0.0f, 0.0f, 0.0f });

		//体力のUIを生成
		for (int nCnt = 0; nCnt < pBoss->GetMaxHP(); nCnt++)
		{
			//数に応じて高さを変更
			D3DXVECTOR2 pos(FirstPos.x, FirstPos.y - (size.y + 2.0f) * nCnt);

			//体力UI生成
			CUI::Create(CUI::TYPE::BOSS_HP, pos, size, rot);
		}

		//ボスエリアに設定
		pGame->SetArea(CGame::GAME_AREA::BOSS);

		//生成済みフラグをtrueにする
		m_bBossCreate = true;
	}

	//ミニゲーム中にエリア外に出た場合過去の位置に戻す
	if (pGame->GetArea() == CGame::GAME_AREA::BOSS && !pArea->Collision(PlayerPos)) SetPos(GetOldPos());
}

//===========================================================================================================
// チェイスエリアリスポーン処理
//===========================================================================================================
void CPlayer::RespawnChaseArea()
{
	//シーンのインスタンス取得
	CScene* pScene = CManager::GetInstance()->GetScene();

	//===========================================================================
	// チェイスエリアを検索
	//===========================================================================
	CArea* pArea = pScene->GetAreaManager()->FindArea(CArea::TYPE::CHASE);

	//検索結果が一致
	if (pArea != nullptr && pArea->GetAreaType() == CArea::TYPE::CHASE)
	{
		//チェイスエリアにリスポーン
		SetPos(pArea->GetPos());

		//移動量をリセット
		SetMove({ 0.0f, 0.0f, 0.0f });
	}

	//===========================================================================
	// ボールを検索
	//===========================================================================
	CEnemy* pEnemy = pScene->GetEnemyManager()->FindEnemy(CEnemy::TYPE::BALL);

	//検索結果が不一致
	if (pEnemy == nullptr || pEnemy->GetEnemyType() != CEnemy::TYPE::BALL)
	{
		//処理を抜ける
		return;
	}

	//プレイヤークラスにダウンキャスト
	CBall* pBall = CObject::DownCast<CBall, CEnemy>(pEnemy);

	//ダウンキャスト失敗
	if (pBall == nullptr)
	{
		//処理を抜ける
		return;
	}

	//===========================================================================
	// チェイススポナーを検索
	//===========================================================================
	CGimmick* pGimmick = pScene->GetGimmickManager()->Find(CGimmick::TYPE::BALL_SPAWNER);

	//検索結果が不一致
	if (pGimmick == nullptr || pGimmick->GetType() != CGimmick::TYPE::BALL_SPAWNER)
	{
		//処理を抜ける
		return;
	}

	//スポナーの位置を取得
	D3DXVECTOR3 SpawnerPos = pGimmick->GetPos();

	//ボールをスポナーにリスポーン
	pBall->SetPos(SpawnerPos);

	//ボールの情報をリセット
	pBall->Reset();
}

//===========================================================================================================
// 壁キック回転速度設定処理
//===========================================================================================================
void CPlayer::SetWallKickTurn()
{
	//ローカル変数宣言
	float fAllFrame = 0.0f;

	//壁キックモーション情報取得
	MotionSet aMotionSet = GetMotionSet(static_cast<int>(STATE::WALL_KICK));

	//キーセットのサイズを取得
	int nKeysetSize = aMotionSet.aKeyset.size();

	//各キーセットのフレーム数の合計を求める
	for (int nCnt = 0; nCnt < nKeysetSize - 1; nCnt++)
	{
		fAllFrame += static_cast<float>(aMotionSet.aKeyset.at(nCnt).nMaxFrame + 1);
	}

	//壁キックの回転速度を設定
	m_fWallKickTurn = (m_fTargetRot_Y - GetRot().y) / fAllFrame;
}

#ifdef _DEBUG
//===========================================================================================================
// デバッグ用処理
//===========================================================================================================

//ワープ処理
void CPlayer::Warp()
{
	//インスタンスの取得
	CInputKeyboard* pKeyboard = CManager::GetInstance()->GetKeyboard();//キーボード
	CScene* pScene = CManager::GetInstance()->GetScene();//シーン
	CCamera* pCamera = CManager::GetInstance()->GetCamera();//カメラ

	//Ctrl + Shift + B
	if (pKeyboard->GetPress(DIK_LCONTROL) && 
		pKeyboard->GetPress(DIK_LSHIFT) && 
		pKeyboard->GetPress(DIK_B) && 
		pScene->GetMode() == CScene::MODE::MODE_GAME)
	{
		//ボス看板の情報を取得
		CSignBoard* pBossSignBoard = pScene->GetSignBoardManager()->Find(CSignBoard::TYPE::GUIDANCE_BOSS);

		//ポインタがnullptrなら処理を抜ける
		if (pBossSignBoard == nullptr) return;
		
		//ボス看板にワープ
		D3DXVECTOR3 PlayerPos(-1600.0f, pBossSignBoard->GetPos().y, pBossSignBoard->GetPos().z);
		SetPos(PlayerPos);

		//カメラの向きを調整
		pCamera->SetRot({ 0.0f, D3DX_PI * 0.5f, 0.0f });
	}

	//Ctrl + Shift + M
	else if (pKeyboard->GetPress(DIK_LCONTROL) && 
		pKeyboard->GetPress(DIK_LSHIFT) && 
		pKeyboard->GetPress(DIK_M) && 
		pScene->GetMode() == CScene::MODE::MODE_GAME)
	{
		//ミニゲームエリアの情報を取得
		CArea* pMiniGameArea = pScene->GetAreaManager()->FindArea(CArea::TYPE::MINI_GAME);

		//ポインタがnullptrなら処理を抜ける
		if (pMiniGameArea == nullptr) return;

		//ミニゲームエリアにワープ
		SetPos(pMiniGameArea->GetPos());

		//カメラの向きを調整
		pCamera->SetRot({ 0.0f, D3DX_PI, 0.0f });
	}

	//Ctrl + Shift + C
	if (pKeyboard->GetPress(DIK_LCONTROL) &&
		pKeyboard->GetPress(DIK_LSHIFT) &&
		pKeyboard->GetPress(DIK_C) &&
		pScene->GetMode() == CScene::MODE::MODE_GAME)
	{
		//ボールスポナーを検索
		CGimmick* pGimmick = pScene->GetGimmickManager()->Find(CGimmick::TYPE::BALL_SPAWNER);

		//検索結果が合致
		if (pGimmick != nullptr && pGimmick->GetType() == CGimmick::TYPE::BALL_SPAWNER)
		{
			//ダウンキャスト
			CBallSpawner* pBallSpawner = CObject::DownCast<CBallSpawner, CGimmick>(pGimmick);

			//ダウンキャスト成功した場合スポナーにワープ
			if (pBallSpawner != nullptr) SetPos(pBallSpawner->GetPos());

			//カメラの向きを調整
			pCamera->SetRot({ 0.0f, D3DX_PI * 0.5f, 0.0f });
		}
	}
}

#endif //_DEBUG
