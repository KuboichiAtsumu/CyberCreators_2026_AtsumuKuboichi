//==============================================================================================================================================
//
// ボスに関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "boss.h"

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CBoss::CBoss(int nPriority) : 
	CMotionCharacter(nPriority),
	m_State{ STATE::NEUTRAL },
	m_nLife{ MAX_LIFE },
	m_nSetCoolTime{ 120 },
	m_nAttackCT{ ATTACK_TIME },
	m_nIdxPosition{ -1 },
	m_nOldPosition { m_nIdxPosition },
	m_nAttackPattern{ -1 },
	m_bAttack{ false }
{
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CBoss::~CBoss()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
#include "UI.h"
HRESULT CBoss::Init()
{
	//基底クラス初期化処理
	if (FAILED(CMotionCharacter::Init()))
	{
		return E_FAIL;
	}

	//攻撃パターン読み込み処理
	LoadAttackPattern();

	// パラメータ設定
	SetRot({ 0.0f, D3DX_PI, 0.0f });// 角度

	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CBoss::Uninit()
{
	//基底クラス終了処理
	CMotionCharacter::Uninit();
}

//===========================================================================================================
// 解放処理
//===========================================================================================================
void CBoss::Release()
{
	//基底クラス解放処理
	CMotionCharacter::Release();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
#include "game.h"
void CBoss::Update()
{
	//エディタモードの場合
	if (CManager::GetInstance()->GetScene()->GetSceneState() == CScene::SCENE::EDIT_3D)
	{//更新せずに処理を抜ける
		return;
	}

	//モーション状態設定処理
	SetMotionState(static_cast<int>(m_State));

	//パーツごとの状態設定処理
	SetPartsState();

	//ゲームシーンのインスタンス取得
	CGame* pGame = CGame::GetInstance();

	//現在のエリアがボスエリアではない場合
	if (pGame != nullptr && 
		pGame->GetArea() != CGame::GAME_AREA::BOSS)
	{//更新せずに処理を抜ける
		return;
	}

	//基底クラス更新処理
	CMotionCharacter::Update();

	//立ち位置の設定処理
	SetPosition();

	//重力処理
	Gravity();

	//移動処理
	Move();

	//攻撃処理
	Attack();

	//ブロックとの当たり判定
	CollisionBlock();

	//角度修正処理
	AngleModifying();

	//攻撃状態でモーションが終わっている場合
	if (m_State == STATE::ATTACK && !GetMotionFlag())
	{
		//ニュートラル状態にする
		m_State = STATE::NEUTRAL;
	}
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CBoss::Draw()
{
	//基底クラス描画処理
	CMotionCharacter::Draw();
}

//===========================================================================================================
// 生成処理
//===========================================================================================================
#include "enemymanager.h"
CBoss* CBoss::Create(D3DXVECTOR3 pos)
{
	//タイプに応じてメモリを動的確保
	CBoss* pBoss = NEW CBoss(static_cast<int>(Category::BOSS));
	
	//ブロック情報が存在する場合
	if (pBoss != nullptr)
	{
		//パラメータ設定
		pBoss->SetPos(pos);//座標
		pBoss->SetCharacterType(CMotionCharacter::TYPE::BOSS);//キャラクタータイプ
	
		//初期化処理
		pBoss->Init();
	}

	return pBoss;
}

//===========================================================================================================
// 当たり判定
//===========================================================================================================
bool CBoss::Collision(CollisionParam& aParam)
{
	//列挙型定義
	enum class Vec3
	{
		X = 0,
		Y,
		Z,
		MAX
	};

	//ローカル変数宣言
	bool bCollision[static_cast<int>(Vec3::MAX)] = {};//各軸の当たり判定フラグ

	//各軸の番号をint型で保存
	int X = static_cast<int>(Vec3::X);
	int Y = static_cast<int>(Vec3::Y);
	int Z = static_cast<int>(Vec3::Z);

	//現在立っている線上情報を取得
	STAND_LINE Line = GetStandLine();

	//XY線上にいる場合
	if (Line == STAND_LINE::XY)
	{
		//X軸から判定
		bCollision[X] = Collision_X(aParam);
		bCollision[Z] = Collision_Z(aParam);
	}
	//YZ線上にいる場合
	else if (Line == STAND_LINE::YZ)
	{
		//Z軸から判定
		bCollision[Z] = Collision_Z(aParam);
		bCollision[X] = Collision_X(aParam);
	}

	//Y軸の判定
	bCollision[Y] = Collision_Y(aParam);

	//全ての軸の判定がtrueの場合
	if (bCollision[X] && bCollision[Y] && bCollision[Z])
	{
		return true;
	}
	//いずれかの軸の判定がfalseの場合
	else
	{
		return false;
	}
}

//===========================================================================================================
// ライフ減少処理
//===========================================================================================================
void CBoss::DecreaseLife()
{
	//ライフ減少
	m_nLife--;

	//ライフが0になった場合
	if (m_nLife == 0)
	{
		//終了処理
		Uninit();

		//リザルト画面へ移行
		CManager::GetInstance()->GetFade()->SetFade(CScene::MODE::MODE_RESULT);
	}
}

//===========================================================================================================
// 攻撃パターン読込処理
//===========================================================================================================
void CBoss::LoadAttackPattern()
{
	//ローカル変数宣言
	std::string str;//ファイル検索用
	std::stringstream ss;//文字列保存用
	AttackInfo aAttackInfo = {};//モーション情報
	
	//ファイルを開く
	std::ifstream file("data/ATTACK/BossAttack.txt");

	//ファイルが開けない場合
	if (!file)
	{
		assert(false);
		return;
	}

	//EOFが見つかるまで検索を繰り返す
	while (1)
	{
		//検索
		std::getline(file, str);

		//EOFが見つかった場合
		if (str.find("END") != std::string::npos)
		{
			//ファイルを閉じる
			file.close();
			break;
		}
		//AttackSetが見つかった場合
		else if (str.find("AttackSet") != std::string::npos)
		{
			//構造体情報初期化
			aAttackInfo.nCreate = 0;

			//各モーションのデータを検索
			while (1)
			{
				//検索
				std::getline(file, str);

				//EndSetが見つかった場合
				if (str.find("EndSet") != std::string::npos)
				{
					//構造体情報を格納
					m_AttackInfo.push_back(aAttackInfo);

					//読み込み終了
					break;
				}

				//Typeが見つかった場合
				if (str.find("Type") != std::string::npos)
				{
					//タイプ保存用
					int nType = 0;

					//読み込んだ文字列を保存
					ss = static_cast<std::stringstream>(str);

					//攻撃の種類を読み込む
					ss >> str >> nType;
					aAttackInfo.Type = static_cast<ATTACK_TYPE>(nType);
				}

				//AttackInfoが見つかった場合
				if (str.find("AttackInfo") != std::string::npos)
				{
					//インデックスを保存
					int nIdx = aAttackInfo.nCreate;

					//各キーセットのデータを検索
					while (1)
					{
						//検索
						std::getline(file, str);

						//EndInfoが見つかった場合
						if (str.find("EndInfo") != std::string::npos)
						{
							//生成数を加算
							aAttackInfo.nCreate++;

							//各キーセットのデータの検索を終了する
							break;
						}
						//POSが見つかった場合
						else if (str.find("POS") != std::string::npos)
						{
							//読み込んだ文字列を保存
							ss = static_cast<std::stringstream>(str);

							//座標を読み込む
							aAttackInfo.pos.push_back({ 0.0f, 0.0f, 0.0f });
							ss >> str >> aAttackInfo.pos.at(nIdx).x >> aAttackInfo.pos.at(nIdx).y >> aAttackInfo.pos.at(nIdx).z;
						}
						//ROTが見つかった場合
						else if (str.find("ROT") != std::string::npos)
						{
							//読み込んだ文字列を保存
							ss = static_cast<std::stringstream>(str);

							//角度を読み込む
							aAttackInfo.rot.push_back({ 0.0f, 0.0f, 0.0f });
							ss >> str >> aAttackInfo.rot.at(nIdx).x >> aAttackInfo.rot.at(nIdx).y >> aAttackInfo.rot.at(nIdx).z;
						}
						//SCALEが見つかった場合
						else if (str.find("SCALE") != std::string::npos)
						{
							//読み込んだ文字列を保存
							ss = static_cast<std::stringstream>(str);

							//拡大率を読み込む
							aAttackInfo.scale.push_back({ 0.0f, 0.0f, 0.0f });
							ss >> str >> aAttackInfo.scale.at(nIdx).x >> aAttackInfo.scale.at(nIdx).y >> aAttackInfo.scale.at(nIdx).z;
						}
						//上記以外が見つかった場合
						else
						{
							//スキップして読み込みを続ける
							continue;
						}
					}
				}
				//上記以外が見つかった場合
				else
				{
					//スキップして読み込みを続ける
					continue;
				}
			}
		}
		//上記以外が見つかった場合
		else
		{
			//検索を続ける
			continue;
		}
	}
}

//===========================================================================================================
// X軸の当たり判定
//===========================================================================================================
bool CBoss::Collision_X(CollisionParam aParam)
{
	//ローカル変数宣言
	D3DXVECTOR3 Pos = GetPos();//座標情報
	D3DXVECTOR3 Size = GetSize();//サイズ情報

	//X軸が重なっている場合
	if (aParam.pos.x + aParam.size.x * 0.5f >= Pos.x - Size.x * 0.5f && 
		aParam.pos.x - aParam.size.x * 0.5f <= Pos.x + Size.x * 0.5f)
	{
		return true;
	}
	//X軸が重なっていない場合
	else
	{
		return false;
	}
}

//===========================================================================================================
// Y軸の当たり判定
//===========================================================================================================
bool CBoss::Collision_Y(CollisionParam aParam)
{
	//ローカル変数宣言
	D3DXVECTOR3 Pos = GetPos();//座標情報
	D3DXVECTOR3 Size = GetSize();//サイズ情報

	//Y軸が重なっている場合
	if (aParam.pos.y + aParam.size.y * 0.5f >= Pos.y &&
		aParam.pos.y - aParam.size.y * 0.5f <= Pos.y + Size.y)
	{
		return true;
	}
	//Y軸が重なっていない場合
	else
	{
		return false;
	}
}

//===========================================================================================================
// Z軸の当たり判定
//===========================================================================================================
bool CBoss::Collision_Z(CollisionParam aParam)
{
	//ローカル変数宣言
	D3DXVECTOR3 Pos = GetPos();//座標情報
	D3DXVECTOR3 Size = GetSize();//サイズ情報

	//Z軸が重なっている場合
	if (aParam.pos.z + aParam.size.z * 0.5f >= Pos.z - Size.z * 0.5f && 
		aParam.pos.z - aParam.size.z * 0.5f <= Pos.z + Size.z * 0.5f)
	{
		return true;
	}
	//Z軸が重なっていない場合
	else
	{
		return false;
	}
}

//===========================================================================================================
// 立ち位置の設定処理
//===========================================================================================================
#include "bossmovepoint.h"
#include "blockmanager.h"
void CBoss::SetPosition()
{
	//攻撃中の場合
	if (m_State == STATE::ATTACK)
	{
		//処理を抜ける
		return;
	}

	//クールタイムを減らす
	if (m_nSetCoolTime > 0)
	{
		m_nSetCoolTime--;
	}

	//クールタイムが0ではない場合
	if (m_nSetCoolTime != 0)
	{
		//処理を抜ける
		return;
	}

	//過去の位置を保存
	m_nOldPosition = m_nIdxPosition;

	//次のポイントに移動
	m_nIdxPosition++;

	//一周したら最初のポイントにループ
	if (m_nIdxPosition == static_cast<int>(CBossMovePoint::Point::MAX))
	{
		m_nIdxPosition = 0;
	}

	//次の立ち位置を取得
	CBossMovePoint* pPoint = CBossMovePoint::Find(m_nIdxPosition);

	//現在いるポイントのインデックスを変更
	m_nIdxPosition = pPoint->GetIdx();

	//座標情報を設定
	SetPos(pPoint->GetPos());

	//クールタイムをリセット
	m_nSetCoolTime = SETPOS_TIME;

	//攻撃フラグをtrueにする
	m_bAttack = true;

	//カメラワーク情報設定処理
	SetCameraWorkInfo();
}

//===========================================================================================================
// カメラワーク情報設定処理
//===========================================================================================================
#include "player.h"
void CBoss::SetCameraWorkInfo()
{
	//ローカル変数宣言
	CCamera* pCamera = CManager::GetInstance()->GetCamera();//カメラの情報
	CPlayer* pPlayer = nullptr;//プレイヤーへのポインタ
	D3DXVECTOR3 PlayerPos = {};//プレイヤー座標
	D3DXVECTOR3 BossPos = GetPos();//ボス座標

	//プレイヤーカテゴリーのオブジェクト情報を取得
	CObject* pFindPlayer = CObject::FindObject(CObject::Category::PLAYER);

	//オブジェクト情報が存在する場合
	if (pFindPlayer != nullptr)
	{
		//プレイヤークラスにダウンキャスト
		pPlayer = CObject::DownCast<CPlayer, CObject>(pFindPlayer);

		//ダウンキャスト成功
		if (pPlayer != nullptr)
		{
			//プレイヤー座標取得
			PlayerPos = pPlayer->GetPos();
		}
		//ダウンキャスト失敗
		else
		{
			//処理を抜ける
			return;
		}
	}
	//オブジェクト情報が存在しない場合
	else
	{
		//処理を抜ける
		return;
	}

	//Y座標を中心部に調整
	PlayerPos.y += pPlayer->GetSize().y * 0.5f;
	BossPos.y += GetSize().y * 0.5f;

	//カメラの注視点をプレイヤーとボスの間に設定
	D3DXVECTOR3 CameraPosR = (PlayerPos + BossPos) * 0.5f;

	//プレイヤーとボスの距離に合わせてカメラの距離を計算
	float fDistance = sqrtf(powf((PlayerPos.y - BossPos.y), 2.0f) + powf((PlayerPos.z - BossPos.z), 2.0f)) * 0.5f + pCamera->GetBossMinDistance();

	//最大値を超えないように調整
	if (fDistance > pCamera->GetBossMaxDistance()) fDistance = pCamera->GetBossMaxDistance();

	//カメラワーク情報設定
	pCamera->SetCameraWork({ CameraPosR.x - fDistance , CameraPosR.y, CameraPosR.z }, CameraPosR, 30);
}

//===========================================================================================================
// 重力処理
//===========================================================================================================
void CBoss::Gravity()
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
void CBoss::Move()
{
	//ローカル変数宣言
	D3DXVECTOR3 pos = GetPos();//座標情報取得
	D3DXVECTOR3 move = GetMove();//移動量情報取得

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
// 攻撃処理
//===========================================================================================================
#include "bossarea.h"
#include "player.h"
#include "bossattack.h"
#include "target.h"
void CBoss::Attack()
{
	//攻撃フラグがfalseの場合
	if (!m_bAttack)
	{
		//処理を抜ける
		return;
	}

	//攻撃までのクールタイムが1以上の場合
	if (m_nAttackCT > 0)
	{
		//攻撃までのクールタイムを減らす
		m_nAttackCT--;
	}

	//減算後、クールタイムが0ではない場合
	if (m_nAttackCT != 0)
	{
		//処理を抜ける
		return;
	}

	//攻撃パターン設定
	SetAttackPattern();

	//攻撃タイプに応じて攻撃
	switch (m_AttackInfo.at(m_nAttackPattern).Type)
	{
		//弾(奥)
	case ATTACK_TYPE::BULLET_DEPTH:
		BulletDepth();
		break;

		//弾(横)
	case ATTACK_TYPE::BULLET_WIDTH:
		BulletWidth();
		break;

		//ブロック生成
	case ATTACK_TYPE::BLOCK:
		BlockCreate();
		break;
	}

	//攻撃状態にする
	m_State = STATE::ATTACK;

	//攻撃フラグをfalseにする
	m_bAttack = false;

	//攻撃までのクールタイムを設定
	m_nAttackCT = ATTACK_TIME;
}

//===========================================================================================================
// 攻撃パターン設定処理
//===========================================================================================================
#include "areamanager.h"
void CBoss::SetAttackPattern()
{
	//立ち位置に応じて次の行動を変化させる
	switch (m_nIdxPosition)
	{
		//左下
	case static_cast<int>(CBossMovePoint::Point::LEFT_DOWN):
		//最初の攻撃の場合
		if (m_nOldPosition == -1)
		{
			//ブロック生成
			m_nAttackPattern = static_cast<int>(CBoss::ATTACK_PATTERN::FORTH_BLOCK);
		}
		//二週目以降の場合
		else
		{
			//弾(奥)
			m_nAttackPattern = static_cast<int>(CBoss::ATTACK_PATTERN::BULLET_DEPTH);
		}
		break;

		//右下
	case static_cast<int>(CBossMovePoint::Point::RIGHT_DOWN):
		//ブロック生成
		m_nAttackPattern = static_cast<int>(CBoss::ATTACK_PATTERN::FORTH_BLOCK);
		break;

		//左上
	case static_cast<int>(CBossMovePoint::Point::LEFT_UP):
		//壁生成
		m_nAttackPattern = static_cast<int>(CBoss::ATTACK_PATTERN::WALL_BLOCK);
		break;

		//右上
	case static_cast<int>(CBossMovePoint::Point::RIGHT_UP):
		//弾(奥)
		m_nAttackPattern = static_cast<int>(CBoss::ATTACK_PATTERN::BULLET_DEPTH);
		break;

	default:
		break;
	}
}

//===========================================================================================================
// 弾(奥)の攻撃処理
//===========================================================================================================
void CBoss::BulletDepth()
{
	//プレイヤーカテゴリーのオブジェクト情報を取得
	CObject* pFindObj = CObject::FindObject(CObject::Category::PLAYER);
	CPlayer* pPlayer = nullptr;

	//オブジェクト情報が存在しない場合
	if (pFindObj == nullptr)
	{
		//処理を抜ける
		return;
	}

	//プレイヤークラスにダウンキャスト
	pPlayer = CObject::DownCast<CPlayer, CObject>(pFindObj);

	//プレイヤー情報が存在しない場合
	if (pPlayer == nullptr)
	{
		//処理を抜ける
		return;
	}

	//プレイヤーの位置を取得
	D3DXVECTOR3 PlayerPos = pPlayer->GetPos();

	//ランダムな位置に攻撃
	for (int nCnt = 0; nCnt < m_AttackInfo.at(m_nAttackPattern).nCreate; nCnt++)
	{
		//生成位置を保存
		D3DXVECTOR3 AttackPos = m_AttackInfo.at(m_nAttackPattern).pos.at(nCnt);
		D3DXVECTOR3 AttackRot = m_AttackInfo.at(m_nAttackPattern).rot.at(nCnt);

		//生成用パラメータ設定
		D3DXVECTOR3 CreatePos(PlayerPos.x + 400.0f, PlayerPos.y + AttackPos.y, PlayerPos.z + AttackPos.z);//生成座標
		D3DXVECTOR3 TargetPos(PlayerPos.x, PlayerPos.y + AttackPos.y, PlayerPos.z + AttackPos.z);//目標の座標
		D3DXVECTOR3 rot(0.0f, CManager::GetInstance()->GetCamera()->GetRot().y + AttackRot.y, 0.0f);//角度
		CorrectionRot(rot.y);//角度補正
		D3DXVECTOR3 move(sinf(rot.y) * 4.0f, 0.0f, cosf(rot.y) * 4.0f);//移動量

		//攻撃を生成
		CObject3D* pObj = CBossAttack::Create(CreatePos, TargetPos, rot, move);

		//ターゲットを生成
		CTarget::Create(TargetPos, pObj);
	}
}

//===========================================================================================================
// 弾(横)の攻撃処理
//===========================================================================================================
void CBoss::BulletWidth()
{
	//ボスエリアの情報を取得
	CArea* pArea = CManager::GetInstance()->GetScene()->GetAreaManager()->FindArea(CArea::TYPE::BOSS);

	//オブジェクト情報が存在しない場合
	if (pArea == nullptr)
	{
		//処理を抜ける
		return;
	}

	//エリアの情報を取得
	D3DXVECTOR3 AreaPos = pArea->GetPos();//座標
	
	//ランダムな位置に攻撃
	for (int nCnt = 0; nCnt < m_AttackInfo.at(m_nAttackPattern).nCreate; nCnt++)
	{
		//生成位置を保存
		D3DXVECTOR3 AttackPos = m_AttackInfo.at(m_nAttackPattern).pos.at(nCnt);
		D3DXVECTOR3 AttackRot = m_AttackInfo.at(m_nAttackPattern).rot.at(nCnt);

		//生成用パラメータ設定
		D3DXVECTOR3 CreatePos(AreaPos.x, AreaPos.y + AttackPos.y, AreaPos.z + AttackPos.z);//生成座標
		D3DXVECTOR3 TargetPos(AreaPos.x, AreaPos.y + AttackPos.y, AreaPos.z);//目標の座標
		D3DXVECTOR3 rot(0.0f, CManager::GetInstance()->GetCamera()->GetRot().y + AttackRot.y, 0.0f);//角度
		CorrectionRot(rot.y);//角度補正
		D3DXVECTOR3 move(-sinf(rot.y) * 4.0f, 0.0f, -cosf(rot.y) * 4.0f);//移動量

		//攻撃を生成
		CObject3D* pObj = CBossAttack::Create(CreatePos, TargetPos, rot, move);

		//ターゲットを生成
		CTarget::Create(TargetPos, pObj);
	}
}

//===========================================================================================================
// ブロック生成攻撃処理
//===========================================================================================================
void CBoss::BlockCreate()
{
	//シーン情報のインスタンスを取得
	CScene* pScene = CManager::GetInstance()->GetScene();

	//ステージ内にあるボスブロックを全て削除
	pScene->GetBlockManager()->UninitTypeAll(CBlock::TYPE::BOSS);

	//ボスエリアの情報を取得
	CArea* pArea = pScene->GetAreaManager()->FindArea(CArea::TYPE::BOSS);

	//オブジェクト情報が存在しない場合
	if (pArea == nullptr)
	{
		//処理を抜ける
		return;
	}

	//エリアの情報を取得
	D3DXVECTOR3 AreaPos = pArea->GetPos();//座標

	//ランダムな位置に攻撃
	for (int nCnt = 0; nCnt < m_AttackInfo.at(m_nAttackPattern).nCreate; nCnt++)
	{
		//生成位置を保存
		D3DXVECTOR3 AttackPos = m_AttackInfo.at(m_nAttackPattern).pos.at(nCnt);

		//生成用パラメータ設定
		D3DXVECTOR3 CreatePos(AreaPos.x, AreaPos.y + AttackPos.y, AreaPos.z + AttackPos.z);

		//生成用パラメータ設定
		D3DXVECTOR3 TargetScale = m_AttackInfo.at(m_nAttackPattern).scale.at(nCnt);

		//生成角度
		D3DXVECTOR3 rot(0.0f, 0.0f, 0.0f);

		//ブロック生成処理
		CBlock::Create(CreatePos, rot, TargetScale, CBlock::TYPE::BOSS);
	}

	//生成パターンに応じて弾の攻撃も出す
	switch (m_nAttackPattern)
	{
		//ブロック生成
	case static_cast<int>(ATTACK_PATTERN::FORTH_BLOCK):
		//弾(横)
		m_nAttackPattern = static_cast<int>(CBoss::ATTACK_PATTERN::BULLET_WIDTH);
		BulletWidth();
		break;

	default:
		break;
	}
}

//===========================================================================================================
// ブロックとの当たり判定
//===========================================================================================================
#include "blockmanager.h"
void CBoss::CollisionBlock()
{
	//当たり判定用パラメータ設定
	CBlock::CollisionParam aParam;
	aParam.pos = GetPos();//座標
	aParam.oldpos = GetOldPos();//過去の座標
	aParam.size = GetSize();//サイズ
	aParam.move = GetMove();//移動量
	aParam.rot = GetRot();//角度
	
	//ブロックの当たり判定
	CManager::GetInstance()->GetScene()->GetBlockManager()->Collision(aParam);
	
	//更新されたパラメータを設定
	SetPos(aParam.pos);//座標
	SetMove(aParam.move);//移動量
	SetRot(aParam.rot);//角度
}

//===========================================================================================================
// 角度修正処理
//===========================================================================================================
#include "player.h"
void CBoss::AngleModifying()
{
	//プレイヤーカテゴリーのオブジェクト情報を取得
	CObject* pFindObj = CObject::FindObject(CObject::Category::PLAYER);
	CPlayer* pPlayer = nullptr;

	//オブジェクト情報が存在しない場合
	if (pFindObj == nullptr)
	{
		//処理を抜ける
		return;
	}

	//プレイヤークラスにダウンキャスト
	pPlayer = CObject::DownCast<CPlayer, CObject>(pFindObj);

	//プレイヤー情報が存在しない場合
	if (pPlayer == nullptr)
	{
		//処理をせずに抜ける
		return;
	}

	//プレイヤーまでの方向ベクトルを求める
	D3DXVECTOR3 Direction = pPlayer->GetPos() - GetPos();
	D3DXVec3Normalize(&Direction, &Direction);//正規化

	//角度情報を取得
	D3DXVECTOR3 rot = GetRot();

	//方向ベクトルのZ座標が負の場合
	if (Direction.z < 0.0f)
	{
		//右に向ける
		rot.y = 0.0f;
	}
	//方向ベクトルのZ座標が正の場合
	else
	{
		//左に向ける
		rot.y = D3DX_PI;
	}

	//角度情報を設定
	SetRot(rot);
}
