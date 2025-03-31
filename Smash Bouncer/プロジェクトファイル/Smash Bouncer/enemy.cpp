//==============================================================================================================================================
//
// エネミーの処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "enemy.h"//エネミー
#include "greenslime.h"//グリーンスライム
#include "blueslime.h"//ブルースライム
#include "whiteghost.h"//ホワイトゴースト
#include "blackghost.h"//ブラックゴースト
#include "redbat.h"//レッドバット
#include "purplebat.h"//パープルバット
#include "midboss.h"//中ボス
#include "boss.h"//ボス
#include "billboard.h"//ビルボードヘッダー

//===========================================================================================================
// 静的メンバ変数初期化
//===========================================================================================================
int CEnemy::m_nAllEnemy = 0;//敵の総数

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CEnemy::CEnemy(int nPriority) : CObjectX(nPriority)
{
	//メンバ変数初期化
	m_pPrev = nullptr;//前のエネミー情報
	m_pNext = nullptr;//次のエネミー情報
	m_EnemyType = {};//エネミータイプ
	m_State = ENEMY_STATE_NORMAL;//状態
	m_FirstPos = {};//最初の位置
	m_nIntervalShot = MIN_INTERVAL;//次の発射までの時間
	m_nCntShot = 0;//弾発射までのカウント
	m_nCntState = 0;//状態カウント
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
	//テクスチャ生成
	CTexture* pTex = CManager::GetTexture();//取得
	int nTextureIdx = pTex->Regist(CXfile::GetSaveModelInfo(CXfile::ENEMY, m_EnemyType)->aTextureFile);//登録
	BindTexture(pTex->GetAddress(nTextureIdx));//設定
	SetTextureIdx(nTextureIdx);//ID設定

	//モデル生成
	CXfile* pModel = CManager::GetModel();//取得
	int nModelIdx = pModel->Regist(CXfile::GetSaveModelInfo(CXfile::ENEMY, m_EnemyType)->aModelFile);//登録
	BindModel(pModel->GetAddress(nModelIdx));//設定
	SetModelIdx(nModelIdx);//ID設定
	SetSize();//サイズ設定

	//自分がいるステージIDを登録
	SetPostStageID();

	//初期位置を保存
	m_FirstPos = GetPos();

	//敵の総数を加算
	m_nAllEnemy++;

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
void CEnemy::Uninit()
{
	//Xファイルオブジェクト終了処理
	CObjectX::Uninit();
}

//===========================================================================================================
// 解放処理
//===========================================================================================================
void CEnemy::Release()
{
	//Xファイルオブジェクト解放処理
	CObjectX::Release();

	//敵の総数を減算
	m_nAllEnemy--;
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CEnemy::Update()
{
	//ローカル変数宣言
	CXfile::CModel aModelInfo = GetModelInfo();

	for (int nCntMat = 0; nCntMat < (int)aModelInfo.dwNumMat; nCntMat++)
	{
		//状態に応じて色を変化
		switch (m_State)
		{
			//通常状態
		case ENEMY_STATE_NORMAL:
			//初期値の色に設定
			aModelInfo.Diffuse[nCntMat] = aModelInfo.FirstDiffuse[nCntMat];
			break;

			//ダメージ状態
		case ENEMY_STATE_DAMAGE:
			//赤色の設定
			aModelInfo.Diffuse[nCntMat] = { 1.0f, 0.0f, 0.0f, 1.0f };

			//状態カウントが残っている場合
			if (m_nCntState > 0)
			{
				//カウントを減少
				m_nCntState--;

				//カウントが0の場合
				if (m_nCntState <= 0)
				{
					//通常状態に設定
					m_State = ENEMY_STATE_NORMAL;
				}
			}
			break;

		default:
			break;
		}
	}

	//更新されたモデル情報を設定
	BindModel(&aModelInfo);

	//Xファイルオブジェクト更新処理
	CObjectX::Update();
}

//===========================================================================================================
// 移動処理
//===========================================================================================================
void CEnemy::Move()
{
	//情報を取得
	D3DXVECTOR3 Pos = GetPos();//位置
	D3DXVECTOR3 Move = GetMove();//移動量

	//移動量を加算
	Pos += Move;

	//敵の位置が移動幅を超えた場合
	if (Pos.z >= m_FirstPos.z + MOVE_WIDTH || Pos.z <= m_FirstPos.z - MOVE_WIDTH)
	{
		//移動方向を切り替える
		Move *= -1.0f;
	}

	//情報を設定
	SetPos(Pos);//位置
	SetMove(Move);//移動量
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CEnemy::Draw()
{
	//エディタモードの場合
	if (CManager::GetScene()->GetEditMode())
	{//描画せずに処理を抜ける
		return;
	}

	//Xファイルオブジェクト描画処理
	CObjectX::Draw();
}

//===========================================================================================================
// 生成処理
//===========================================================================================================
CEnemy* CEnemy::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nType)
{
	//タイプに応じて動的確保
	CEnemy* pEnemy = nullptr;
	switch (nType)
	{
	//グリーンスライム
	case GREEN_SLIME:
		pEnemy = NEW CGreenSlime();
		break;

	//ブルースライム
	case BLUE_SLIME:
		pEnemy = NEW CBlueSlime();
		break;

	//ホワイトゴースト
	case WHITE_GHOST:
		pEnemy = NEW CWhiteGhost();
		break;

	//ブラックゴースト
	case BLACK_GHOST:
		pEnemy = NEW CBlackGhost();
		break;

	//レッドバット
	case RED_BAT:
		pEnemy = NEW CRedBat();
		break;

	//パープルバット
	case PURPLE_BAT:
		pEnemy = NEW CPurpleBat();
		break;

	//中ボス
	case MID_BOSS:
		pEnemy = NEW CMidBoss();
		break;

	//ボス
	case BOSS:
		pEnemy = NEW CBoss();
		break;

	default:
		break;
	}

	//パラメータ設定
	pEnemy->SetType(TYPE::ENEMY);//オブジェクトタイプ設定
	pEnemy->SetEnemyType((ENEMYTYPE)nType);//エネミータイプ設定
	pEnemy->SetPos(pos);//位置設定
	pEnemy->SetRot(rot);//向き設定
	pEnemy->SetMove({ 0.0f, 0.0f, MOVE_SPEED });//移動量設定
	pEnemy->SetLife(ENEMY_LIFE[nType]);//ライフ設定

	//初期化処理
	pEnemy->Init();

	return pEnemy;
}

//===========================================================================================================
// 当たり判定
//===========================================================================================================
bool CEnemy::Collision(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	//ローカル変数宣言
	bool bCollision = false; //当たり判定フラグ
	D3DXVECTOR3 Pos = GetPos();//位置情報
	D3DXVECTOR3 Size = GetSize();//サイズ情報

	//重なった場合
	if (pos.x + size.x * 0.5f >= Pos.x - Size.x * 0.5f && pos.x - size.x * 0.5f <= Pos.x + Size.x * 0.5f
		&& pos.z - size.z * 0.5f <= Pos.z + Size.z * 0.5f && pos.z + size.z * 0.5f >= Pos.z - Size.z * 0.5f)
	{
		//当たり判定をtrueにする
		bCollision = true;
	}

	//当たり判定結果を返す
	return bCollision;
}

//===========================================================================================================
// ヒット処理
//===========================================================================================================
void CEnemy::Hit()
{
	//ローカル変数宣言
	D3DXVECTOR3 pos = GetPos();//位置情報
	int nLife = GetLife();//ライフ情報

	//エネミーのライフを減らす
	nLife--;

	//SE再生
	CManager::GetSound()->Play(CSound::SE_HIT);

	//エネミーのライフが0になった場合
	if (nLife <= 0)
	{
		//生成位置を調整
		pos.y += GetSize().y * 0.5f;

		//ビルボード生成処理
		CBillboard::Create(CBillboard::BILLBOARD_EXPLOSION, pos, { 40.0f,40.0f,0.0f });

		//終了処理
		Uninit();
	}
	//エネミーのライフが残っている場合
	else
	{
		//ダメージ状態に設定
		m_State = ENEMY_STATE_DAMAGE;

		//状態カウント設定
		m_nCntState = 5;
	}

	//エネミーのライフを設定する
	SetLife(nLife);
}
