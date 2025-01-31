//==============================================================================================================================================
//
// バレットの処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "bullet.h"
#include "boss.h"
#include "particle.h"
#include "player.h"
#include "blockmanager.h"
#include "enemymanager.h"
#include "bulletmanager.h"

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CBullet::CBullet(int nPriority) : CObjectX(nPriority),
	m_pPrev(nullptr),
	m_pNext(nullptr),
	m_Type(TYPE::NONE),
	m_nLife(0)
{
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CBullet::~CBullet()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CBullet::Init()
{
	//モデル生成
	CXfile* pModel = CXfile::GetInstance();
	BindModel(pModel->GetAddress(CTag::TAG::BULLET, static_cast<int>(m_Type)));//設定
	SetSize();//サイズ設定

	//基底クラス初期化処理
	if (FAILED(CObjectX::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CBullet::Uninit()
{
	//基底クラス終了処理
	CObjectX::Uninit();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CBullet::Update()
{
	//基底クラスの更新処理
	CObjectX::Update();

	//寿命が0の場合
	if (m_nLife <= 0)
	{
		//終了処理
		Uninit();
	}
	//寿命が残っている場合
	else
	{
		//寿命を減らす
		m_nLife--;
	}

	//座標を更新
	SetOldPos(GetPos());
	SetPos(GetPos() + GetMove());

	//エネミーとの当たり判定
	CollisionEnemy();

	//ボスとの当たり判定
	CollisionBoss();

	//ブロックとの当たり判定
	CollisionBlock();
}

//===========================================================================================================
// エネミーとの当たり判定
//===========================================================================================================
void CBullet::CollisionEnemy()
{
	//当たり判定用の構造体情報を設定
	CEnemy::CollisionParam aParam = {};
	aParam.pos = GetPos();//座標
	aParam.size = GetSize();//サイズ
	aParam.pEnemy = nullptr;//触れたエネミーの情報

	//エネミーとの当たり判定がfalseの場合
	if (!CManager::GetInstance()->GetScene()->GetEnemyManager()->Collision(aParam)) return;
	
	//終了処理
	Uninit();

	//エネミータイプがボールではない場合
	if (aParam.pEnemy->GetEnemyType() == CEnemy::TYPE::BALL) return;

	//撃破効果音再生
	CManager::GetInstance()->GetSound()->Play(CSound::SOUND::HIT);

	//エネミーの終了処理
	aParam.pEnemy->Uninit();

	//パーティクル生成
	for (int nCnt = 0; nCnt < 30; nCnt++)
	{
		//ランダムにカラーを設定
		float fR = static_cast<float>(rand() % 10 + 1) * 0.1f;
		float fG = static_cast<float>(rand() % 10 + 1) * 0.1f;
		float fB = static_cast<float>(rand() % 10 + 1) * 0.1f;

		CParticle::Create(CParticle::TYPE::NORMAL, aParam.pEnemy->GetPos(), { 25.0f, 25.0f, 0.0f }, 20, { fR, fG, fB, 1.0f });
	}

	//プレイヤーカテゴリーのオブジェクト情報を取得
	CObject* pFindObj = CObject::FindObject(CObject::Category::PLAYER);

	//オブジェクト情報が存在する場合
	if (pFindObj == nullptr) return;

	//プレイヤークラスにダウンキャスト
	CPlayer* pPlayer = CObject::DownCast<CPlayer, CObject>(pFindObj);

	//キル数加算
	pPlayer->IncreaseKill();
}

//===========================================================================================================
// ボスとの当たり判定
//===========================================================================================================
void CBullet::CollisionBoss()
{
	//ボスカテゴリーのオブジェクト情報を取得
	CObject* pFindObj = CObject::FindObject(CObject::Category::BOSS);
	CBoss* pBoss = nullptr;

	//オブジェクト情報が存在する場合
	if (pFindObj != nullptr)
	{
		//ボスクラスにダウンキャスト
		pBoss = CObject::DownCast<CBoss, CObject>(pFindObj);
	}

	//当たり判定用の構造体情報を設定
	CBoss::CollisionParam aParam = {};
	aParam.pos = GetPos();//座標
	aParam.size = GetSize();//サイズ

	//ボスとの当たり判定がtrueの場合
	if (pBoss != nullptr && pBoss->Collision(aParam))
	{
		//終了処理
		Uninit();

		//ボスのライフ減少処理
		pBoss->DecreaseLife();
	}
}

//===========================================================================================================
// ブロックとの当たり判定
//===========================================================================================================
void CBullet::CollisionBlock()
{
	//当たり判定用パラメータ設定
	CBlock::CollisionParam aParam;
	aParam.pos = GetPos();//座標
	aParam.oldpos = GetOldPos();//過去の座標
	aParam.size = GetSize();//サイズ
	aParam.move = GetMove();//移動量
	aParam.rot = GetRot();//角度
	aParam.type = CBlock::TYPE::NONE;//当たったブロックのタイプ

	//上側の判定を取得
	CBlock::CollisionFlag aFlag = CManager::GetInstance()->GetScene()->GetBlockManager()->Collision(aParam);
	
	//いずれかの当たり判定がtrue
	if (aFlag.X || aFlag.Y_UP || aFlag.Y_DOWN || aFlag.Z)
	{
		//弾を削除
		Uninit();

		//パーティクル生成
		for (int nCnt = 0; nCnt < 10; nCnt++)
		{
			CParticle::Create(CParticle::TYPE::NORMAL, aParam.pos, { 15.0f, 15.0f, 0.0f }, 10, { 1.0f, 1.0f, 0.0f, 1.0f });
		}
	}
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CBullet::Draw()
{
	//基底クラス描画処理
	CObjectX::Draw();
}

//===========================================================================================================
// 生成処理
//===========================================================================================================
CBullet* CBullet::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& move, const D3DXVECTOR3& rot, TYPE m_type)
{
	//タイプに応じてメモリを動的確保
	CBullet* pBullet = NEW CBullet(static_cast<int>(CObject::Category::BULLET));
	switch (m_type)
	{
		//プレイヤー
	case TYPE::PLAYER:
		break;

	default:
		break;
	}

	//バレット情報が存在する場合
	if (pBullet != nullptr)
	{
		//パラメータ設定
		pBullet->SetPos(pos);//座標
		pBullet->SetMove(move);//移動量
		pBullet->SetRot(rot);//角度
		pBullet->SetBulletType(m_type);//タイプ
		pBullet->SetLife(30);

		//初期化処理
		pBullet->Init();

		//マネージャー登録
		CManager::GetInstance()->GetScene()->GetBulletManager()->Regist(pBullet);
	}

	return pBullet;
}
