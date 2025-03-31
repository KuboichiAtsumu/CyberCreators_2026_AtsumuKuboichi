//==============================================================================================================================================
//
// バレットの処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "bullet.h"//バレット
#include "playerbullet.h"//プレイヤーバレット
#include "clonebullet.h"//クローンバレット
#include "boundenemybullet.h"//エネミーバレット(反射)
#include "launchenemybullet.h"//エネミーバレット(打ち上げ)
#include "boundinvincible.h"//無敵バレット(反射)
#include "launchinvincible.h"//無敵バレット(打ち上げ)
#include "bulletmanager.h"//バレットマネージャー
#include "tutorial.h"//チュートリアルシーン
#include "title.h"//タイトルシーン

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CBullet::CBullet(int nPriority) : CObjectX(nPriority)
{
	//メンバ変数初期化
	m_pPrev = nullptr;//前のバレット情報
	m_pNext = nullptr;//次のバレット情報
	m_BulletType = NONE;//バレットタイプ
	m_Distance = {};//対象オブジェクトとの各座標距離
	m_fLength = 100.0f;//対象オブジェクトとの対角距離
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
	CXfile* pModel = CManager::GetModel();//取得
	int nModelIdx = pModel->Regist("data/MODEL/bullet000.x");//登録
	BindModel(pModel->GetAddress(nModelIdx));//設定
	SetModelIdx(nModelIdx);//ID設定
	SetSize();//サイズ設定

	//自分がいるステージIDを登録
	SetPostStageID();

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
void CBullet::Uninit()
{
	//Xファイルオブジェクト終了処理
	CObjectX::Uninit();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CBullet::Update()
{
	//ローカル変数宣言
	D3DXVECTOR3 pos = GetPos();//位置情報
	D3DXVECTOR3 move = GetMove();//移動量情報
	int nLife = GetLife();//ライフ情報

	//タイトル画面ではない場合
	if (CManager::GetScene()->GetMode() != CScene::MODE_TITLE)
	{
		//寿命減少
		nLife--;
		SetLife(nLife);

		//寿命が尽きた場合
		if (nLife <= 0)
		{
			//終了処理
			Uninit();
		}
	}

	//過去の位置の設定
	SetOldPos(pos);

	//移動量を加算
	pos += move;

	//情報を設定
	SetPos(pos);//位置
	SetMove(move);//移動量

	//自分がいるステージと現在のステージが一致しない場合
	//ステージ移動中の場合
	if (GetPostStageID() != CGame::GetNumStage() || CGame::GetGameState() == CGame::STATE_MOVE)
	{
		//終了処理
		Uninit();
	}
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CBullet::Draw()
{
	//Xファイルオブジェクト描画処理
	CObjectX::Draw();
}

//===========================================================================================================
// 生成処理
//===========================================================================================================
CBullet* CBullet::Create(D3DXVECTOR3 pos, D3DXVECTOR3 move, BULLETTYPE m_type)
{
	//ローカル変数宣言
	CBulletManager* pBulletManager = nullptr;//バレットマネージャー

	//シーンに応じてマネージャー情報を取得
	switch (CManager::GetScene()->GetMode())
	{
		//タイトル
	case CScene::MODE_TITLE:
		pBulletManager = CTitle::GetBulletManager();
		break;

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

	//タイプごとに動的確保
	CBullet* pBullet = nullptr;
	switch (m_type)
	{
		//プレイヤー
	case PLAYER:
		pBullet = NEW CPlayerBullet();
		break;

		//クローン
	case CLONE:
		pBullet = NEW CCloneBullet();
		break;

		//エネミー(反射)
	case BOUND_ENEMY:
		pBullet = NEW CBoundEnemyBullet();
		break;

		//エネミー(打ち上げ)
	case LAUNCH_ENEMY:
		pBullet = NEW CLaunchEnemyBullet();
		break;

		//無敵(反射)
	case BOUND_INVINCIBLE:
		pBullet = NEW CBoundInvincible();
		break;

		//無敵(打ち上げ)
	case LAUNCH_INVINCIBLE:
		pBullet = NEW CLaunchInvincible();
		break;

	default:
		break;
	}

	//パラメータ設定
	pBullet->SetType(TYPE::BULLET);//オブジェクトタイプの設定
	pBullet->SetPos(pos);//生成位置設定
	pBullet->SetOldPos(pos);//過去の位置
	pBullet->SetMove(move);//生成向き設定

	//初期化処理
	pBullet->Init();
	pBullet->SetBulletType(m_type);//弾のタイプ設定

	//バレット情報をマネージャーに登録
	pBulletManager->Regist(pBullet);

	return pBullet;
}

//===========================================================================================================
// 当たり判定
//===========================================================================================================
bool CBullet::Collision(D3DXVECTOR3 pos)
{
	//ローカル変数宣言
	D3DXVECTOR3 BulletPos = GetPos();//バレットの位置情報

	//対象オブジェクトとの各座標距離を求める
	m_Distance = { abs(pos.x - BulletPos.x), abs(pos.y - BulletPos.y), abs(pos.z - BulletPos.z)	};

	//対象オブジェクトとのXZ間の対角距離を求める
	float fLength = sqrtf(powf(m_Distance.x, 2.0f) + powf(m_Distance.z, 2.0f));

	//対象オブジェクトとの対角距離を求める
	m_fLength = sqrtf(powf(fLength, 2.0f) + powf(m_Distance.y, 2.0f));

	return false;
}

//===========================================================================================================
// 弾のタイプ設定処理
//===========================================================================================================
void CBullet::SetBulletType(BULLETTYPE BulletType)
{
	//タイプを反映
	m_BulletType = BulletType;

	//ライフリセット
	SetLife(360);

	//モデル情報取得
	CXfile::CModel aModelInfo = GetModelInfo();

	//タイプごとに色を設定
	switch (m_BulletType)
	{
	//プレイヤー
	case PLAYER:
		for (int nCntMat = 0; nCntMat < (int)aModelInfo.dwNumMat; nCntMat++)
		{
			//青色
			aModelInfo.Diffuse[nCntMat].r = 0.0f;
			aModelInfo.Diffuse[nCntMat].g = 0.849f;
			aModelInfo.Diffuse[nCntMat].b = 1.0f;
		}
		break;

	//クローン
	case CLONE:
		for (int nCntMat = 0; nCntMat < (int)aModelInfo.dwNumMat; nCntMat++)
		{
			//薄青色
			aModelInfo.Diffuse[nCntMat].r = 0.878f;
			aModelInfo.Diffuse[nCntMat].g = 1.0f;
			aModelInfo.Diffuse[nCntMat].b = 1.0f;
		}
		break;
		
	//エネミー
	case BOUND_ENEMY:
	case LAUNCH_ENEMY:
		for (int nCntMat = 0; nCntMat < (int)aModelInfo.dwNumMat; nCntMat++)
		{
			//赤色
			aModelInfo.Diffuse[nCntMat].r = 0.862f;
			aModelInfo.Diffuse[nCntMat].g = 0.078f;
			aModelInfo.Diffuse[nCntMat].b = 0.235f;
		}
		break;

	//無敵
	case BOUND_INVINCIBLE:
	case LAUNCH_INVINCIBLE:
		for (int nCntMat = 0; nCntMat < (int)aModelInfo.dwNumMat; nCntMat++)
		{
			//黄色
			aModelInfo.Diffuse[nCntMat].r = 1.0f;
			aModelInfo.Diffuse[nCntMat].g = 1.0f;
			aModelInfo.Diffuse[nCntMat].b = 0.0f;
		}
		break;

	default:
		break;
	}

	//モデル情報設定
	BindModel(&aModelInfo);
}
