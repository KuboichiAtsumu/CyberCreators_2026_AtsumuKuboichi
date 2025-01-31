//==============================================================================================================================================
//
// ブロックに関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "enemy.h"
#include "enemymanager.h"
#include "ball.h"
#include "normalenemy.h"
#include "moveenemy.h"
#include "bulletenemy.h"

//===========================================================================================================
// 静的メンバ変数初期化
//===========================================================================================================
int CEnemy::m_nAll = 0;

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CEnemy::CEnemy(int nPriority) : CObjectX(nPriority),
	m_pPrev{ nullptr },
	m_pNext{ nullptr },
	m_Type{ CEnemy::TYPE::NONE }
{
	//総数加算
	m_nAll++;
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CEnemy::~CEnemy()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CEnemy::Init()
{
	//タイプ情報をint型で保存
	int nType = static_cast<int>(m_Type);

	//モデル生成
	CXfile* pModel = CXfile::GetInstance();
	int nModelIdx = pModel->Regist(CTag::TAG::ENEMY, nType, MODEL_FILE[nType]);
	BindModel(pModel->GetAddress(nModelIdx));//設定
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
void CEnemy::Uninit()
{
	//基底クラス終了処理
	CObjectX::Uninit();
}

//===========================================================================================================
// 解放処理
//===========================================================================================================
void CEnemy::Release()
{
	//基底クラス解放処理
	CObjectX::Release();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CEnemy::Update()
{
	//基底クラス更新処理
	CObjectX::Update();
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CEnemy::Draw()
{
	//エディタモードの場合
	if (CManager::GetInstance()->GetScene()->GetSceneState() == CScene::SCENE::EDIT_3D)
	{//描画せずに処理を抜ける
		return;
	}

	//Xファイルのオブジェクト描画処理
	CObjectX::Draw();
}

//===========================================================================================================
// 当たり判定
//===========================================================================================================
bool CEnemy::Collision(CollisionParam& aParam)
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
		//自分自身の情報を設定
		aParam.pEnemy = this;
		return true;
	}
	//いずれかの軸の判定がfalseの場合
	else
	{
		return false;
	}
}

//===========================================================================================================
// X軸の当たり判定
//===========================================================================================================
bool CEnemy::Collision_X(CollisionParam aParam)
{
	//ローカル変数宣言
	D3DXVECTOR3 Pos = GetPos();//座標情報
	D3DXVECTOR3 Size = GetSize();//サイズ情報

	//X軸が重なっている場合
	if (aParam.pos.x + aParam.size.x * 0.5f >= Pos.x - Size.x * 0.5f
		&& aParam.pos.x - aParam.size.x * 0.5f <= Pos.x + Size.x * 0.5f)
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
bool CEnemy::Collision_Y(CollisionParam aParam)
{
	//ローカル変数宣言
	D3DXVECTOR3 Pos = GetPos();//座標情報
	D3DXVECTOR3 Size = GetSize();//サイズ情報

	//Y軸が重なっている場合
	if (aParam.pos.y + aParam.size.y * 0.5f >= Pos.y - Size.y * 0.5f
		&& aParam.pos.y - aParam.size.y * 0.5f <= Pos.y + Size.y * 0.5f)
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
bool CEnemy::Collision_Z(CollisionParam aParam)
{
	//ローカル変数宣言
	D3DXVECTOR3 Pos = GetPos();//座標情報
	D3DXVECTOR3 Size = GetSize();//サイズ情報

	//Z軸が重なっている場合
	if (aParam.pos.z + aParam.size.z * 0.5f >= Pos.z - Size.z * 0.5f
		&& aParam.pos.z - aParam.size.z * 0.5f <= Pos.z + Size.z * 0.5f)
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
// 生成処理
//===========================================================================================================
CEnemy* CEnemy::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, TYPE type)
{
	//タイプに応じてメモリを動的確保
	CEnemy* pEnemy = nullptr;
	switch (type)
	{
		//ボール
	case TYPE::BALL:
		pEnemy = NEW CBall();
		break;

		//ノーマル
	case TYPE::NORMAL:
		pEnemy = NEW CNormalEnemy();
		break;

		//ムーブ
	case TYPE::MOVE:
		pEnemy = NEW CMoveEnemy();
		break;

		//バレット
	case TYPE::BULLET:
		pEnemy = NEW CBulletEnemy();
		break;

	default:
		break;
	}

	//ポインタに情報が存在する場合
	if (pEnemy != nullptr)
	{
		//パラメータ設定
		pEnemy->SetPos(pos);//座標
		pEnemy->SetRot(rot);//角度
		pEnemy->SetScale(scale);//拡大率
		pEnemy->SetEnemyType(type);//ブロックタイプ設定

		//初期化処理
		pEnemy->Init();

		//マネージャー登録処理
		CManager::GetInstance()->GetScene()->GetEnemyManager()->Regist(pEnemy);
	}

	return pEnemy;
}

