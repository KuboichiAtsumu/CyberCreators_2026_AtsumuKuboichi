//==============================================================================================================================================
//
// ブロックに関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "block.h"
#include "slone.h"
#include "bossblock.h"
#include "blockmanager.h"

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CBlock::CBlock(int nPriority) : 
	CObjectX(nPriority),
	m_pPrev(nullptr),
	m_pNext(nullptr),
	m_Type(CBlock::TYPE::NONE),
	m_Normal( 0.0f, 0.0f, 0.0f )
{
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CBlock::~CBlock()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CBlock::Init()
{
	//モデル生成
	CXfile* pModel = CXfile::GetInstance();//インスタンスを取得
	BindModel(pModel->GetAddress(CTag::TAG::BLOCK, static_cast<int>(m_Type)));//設定
	SetSize();//サイズ設定

	//ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//デバイスのポインタ

	//シェーダー読込
	ID3DXEffect* pEffect = GetEffect();
	if (FAILED(D3DXCreateEffectFromFile(pDevice, "ToonShader.fx", nullptr, nullptr, D3DXSHADER_DEBUG, nullptr, &pEffect, nullptr)))
	{
		MessageBox(0, "シェーダーの読み込みに失敗しました！", "エラー", MB_OK);
	}
	SetEffect(pEffect);

	//基底クラス初期化処理
	if (FAILED(CObjectX::Init())) return E_FAIL;

	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CBlock::Uninit()
{
	//基底クラス終了処理
	CObjectX::Uninit();
}

//===========================================================================================================
// 解放処理
//===========================================================================================================
void CBlock::Release()
{
	//基底クラス解放処理
	CObjectX::Release();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CBlock::Update()
{
	//基底クラス更新処理
	CObjectX::Update();
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CBlock::Draw()
{
	//エディタモードの場合は描画しない
	if (CManager::GetInstance()->GetScene()->GetSceneState() == CScene::SCENE::EDIT_3D)
	{
		return;
	}

	//基底クラス描画処理
	CObjectX::Draw();
}

//===========================================================================================================
// 当たり判定
//===========================================================================================================
CBlock::CollisionFlag CBlock::Collision(CollisionParam& aParam)
{
	//情報を取得
	CollisionFlag aFlag = {};//当たり判定フラグ構造体
	D3DXVECTOR3 Pos = GetPos();//座標
	D3DXVECTOR3 Size = GetSize();//サイズ

	//=======================================================================
	// Y軸
	//=======================================================================

	//上側の判定
	if (aParam.pos.y < Pos.y + Size.y * 0.5f && 
		aParam.oldpos.y >= Pos.y + Size.y * 0.5f)
	{
		//XZ間の範囲内にいる場合
		if ((aParam.pos.x + aParam.size.x * 0.5f > Pos.x - Size.x * 0.5f && 
			aParam.pos.x - aParam.size.x * 0.5f < Pos.x + Size.x * 0.5f) && 
			(aParam.pos.z + aParam.size.z * 0.5f > Pos.z - Size.z * 0.5f && 
				aParam.pos.z - aParam.size.z * 0.5f < Pos.z + Size.z * 0.5f))
		{
			//座標を修正
			aParam.pos.y = Pos.y + Size.y * 0.5f;

			//Y軸の移動量をリセット
			aParam.move.y = 0.0f;

			//Y軸の上側の当たり判定をtrueにする
			aFlag.Y_UP = true;

			//自分自身のタイプを保存
			aParam.type = m_Type;
		}
	}

	//下側の判定
	if (aParam.pos.y + aParam.size.y > Pos.y - Size.y * 0.5f && 
		aParam.oldpos.y + aParam.size.y <= Pos.y - Size.y * 0.5f)
	{
		//XZ間の範囲内にいる場合
		if ((aParam.pos.x + aParam.size.x * 0.5f > Pos.x - Size.x * 0.5f && 
			aParam.pos.x - aParam.size.x * 0.5f < Pos.x + Size.x * 0.5f) && 
			(aParam.pos.z + aParam.size.z * 0.5f > Pos.z - Size.z * 0.5f && 
				aParam.pos.z - aParam.size.z * 0.5f < Pos.z + Size.z * 0.5f))
		{
			//座標を修正
			aParam.pos.y = Pos.y - Size.y * 0.5f - aParam.size.y;

			//Y軸の移動量をリセット
			aParam.move.y = 0.0f;

			//Y軸の下側の当たり判定をtrueにする
			aFlag.Y_DOWN = true;

			//自分自身のタイプを保存
			aParam.type = m_Type;
		}
	}

	//=======================================================================
	// X軸
	//=======================================================================

	//左側の判定
	if (aParam.pos.x + aParam.size.x * 0.5f > Pos.x - Size.x * 0.5f && 
		aParam.oldpos.x + aParam.size.x * 0.5f <= Pos.x - Size.x * 0.5f)
	{
		//YZ間の範囲内にいる場合
		if ((aParam.pos.y + aParam.size.y > Pos.y - Size.y * 0.5f && 
			aParam.pos.y < Pos.y + Size.y * 0.5f) && 
			(aParam.pos.z + aParam.size.z * 0.5f > Pos.z - Size.z * 0.5f && 
				aParam.pos.z - aParam.size.z * 0.5f < Pos.z + Size.z * 0.5f))
		{
			//座標を修正
 			aParam.pos.x = Pos.x - Size.x * 0.5f - aParam.size.x * 0.5f;

			//X軸の当たり判定
			aFlag.X = true;

			//自分自身のタイプを保存
			aParam.type = m_Type;
		}
	}

	//右側の判定
	if (aParam.pos.x - aParam.size.x * 0.5f < Pos.x + Size.x * 0.5f && 
		aParam.oldpos.x - aParam.size.x * 0.5f >= Pos.x + Size.x * 0.5f)
	{
		//YZ間の範囲内にいる場合
		if ((aParam.pos.y + aParam.size.y > Pos.y - Size.y * 0.5f && 
			aParam.pos.y < Pos.y + Size.y * 0.5f) && 
			(aParam.pos.z + aParam.size.z * 0.5f > Pos.z - Size.z * 0.5f && 
				aParam.pos.z - aParam.size.z * 0.5f < Pos.z + Size.z * 0.5f))
		{
			//座標を修正
			aParam.pos.x = Pos.x + Size.x * 0.5f + aParam.size.x * 0.5f;

			//X軸の当たり判定をtrueにする
			aFlag.X = true;

			//自分自身のタイプを保存
			aParam.type = m_Type;
		}
	}

	//=======================================================================
	// Z軸
	//=======================================================================

	//手前側の判定
	if (aParam.pos.z + aParam.size.z * 0.5f > Pos.z - Size.z * 0.5f && 
		aParam.oldpos.z + aParam.size.z * 0.5f <= Pos.z - Size.z * 0.5f)
	{
		//XY間の範囲内にいる場合
		if ((aParam.pos.x + aParam.size.x * 0.5f > Pos.x - Size.x * 0.5f && 
			aParam.pos.x - aParam.size.x * 0.5f < Pos.x + Size.x * 0.5f) && 
			(aParam.pos.y + aParam.size.y > Pos.y - Size.y * 0.5f && 
				aParam.pos.y < Pos.y + Size.y * 0.5f))
		{
			//座標を修正
			aParam.pos.z = Pos.z - Size.z * 0.5f - aParam.size.z * 0.5f;

			//Z軸の当たり判定をtrueにする
			aFlag.Z = true;

			//自分自身のタイプを保存
			aParam.type = m_Type;
		}
	}

	//奥側の判定
	if (aParam.pos.z - aParam.size.z * 0.5f < Pos.z + Size.z * 0.5f && 
		aParam.oldpos.z - aParam.size.z * 0.5f >= Pos.z + Size.z * 0.5f)
	{
		//XY間の範囲内にいる場合
		if ((aParam.pos.x + aParam.size.x * 0.5f > Pos.x - Size.x * 0.5f && 
			aParam.pos.x - aParam.size.x * 0.5f < Pos.x + Size.x * 0.5f) && 
			(aParam.pos.y + aParam.size.y > Pos.y - Size.y * 0.5f && 
				aParam.pos.y < Pos.y + Size.y * 0.5f))
		{
			//座標を修正
			aParam.pos.z = Pos.z + Size.z * 0.5f + aParam.size.z * 0.5f;

			//Z軸の当たり判定をtrueにする
			aFlag.Z = true;

			//自分自身のタイプを保存
			aParam.type = m_Type;
		}
	}

	return aFlag;
}

//===========================================================================================================
// XY間の垂直ベクトル計算処理
//===========================================================================================================
D3DXVECTOR3 CBlock::SEGMENT_XY(float StartX, float StartY, float EndX, float EndY)
{
	//垂直ベクトルを求める
	D3DXVECTOR3 Start(StartX, StartY, 0.0f);
	D3DXVECTOR3 End(EndX, EndY, 0.0f);

	D3DXVECTOR3 r = End - Start;

	m_Normal.x = -r.y;
	m_Normal.y = r.x;

	//ベクトルを正規化
	D3DXVec3Normalize(&m_Normal, &m_Normal);

	return m_Normal;
}

//===========================================================================================================
// XZ間の垂直ベクトル計算処理
//===========================================================================================================
D3DXVECTOR3 CBlock::SEGMENT_XZ(float StartX, float StartZ, float EndX, float EndZ)
{
	//垂直ベクトルを求める
	D3DXVECTOR3 Start(StartX, 0.0f, StartZ);
	D3DXVECTOR3 End(EndX, 0.0f, EndZ);

	D3DXVECTOR3 r = End - Start;

	m_Normal.x = -r.z;
	m_Normal.z = r.x;

	//ベクトルを正規化
	D3DXVec3Normalize(&m_Normal, &m_Normal);

	return m_Normal;
}

//===========================================================================================================
// YZ間の垂直ベクトル計算処理
//===========================================================================================================
D3DXVECTOR3 CBlock::SEGMENT_YZ(float StartY, float StartZ, float EndY, float EndZ)
{
	//垂直ベクトルを求める
	D3DXVECTOR3 Start(0.0f, StartY, StartZ);
	D3DXVECTOR3 End(0.0f, EndY, EndZ);

	D3DXVECTOR3 r = End - Start;

	m_Normal.z = -r.y;
	m_Normal.y = r.z;

	//ベクトルを正規化
	D3DXVec3Normalize(&m_Normal, &m_Normal);

	return m_Normal;
}

//===========================================================================================================
// 生成処理
//===========================================================================================================
CBlock* CBlock::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& scale, TYPE type)
{
	//タイプに応じてメモリを動的確保
	CBlock* pBlock = nullptr;

	switch (type)
	{
		//ノーマル
	case TYPE::NORMAL:
		pBlock = NEW CBlock(static_cast<int>(Category::BLOCK));
		break;

		//坂道
	case TYPE::SLONE:
		pBlock = NEW CSlone();
		break;

		//ボスブロック
	case TYPE::BOSS:
		pBlock = NEW CBossBlock();

	default:
		break;
	}

	//ブロック情報が存在する場合
	if (pBlock != nullptr)
	{
		//パラメータ設定
		pBlock->SetPos(pos);//座標
		pBlock->SetRot(rot);//角度
		pBlock->SetScale(scale);//拡大率
		pBlock->SetBlockType(type);//ブロックタイプ設定

		//初期化処理
		pBlock->Init();

		//マネージャー登録処理
		CManager::GetInstance()->GetScene()->GetBlockManager()->Regist(pBlock);
	}

	return pBlock;
}
