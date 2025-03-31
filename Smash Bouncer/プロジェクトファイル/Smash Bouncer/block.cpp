//==============================================================================================================================================
//
// ブロックに関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "block.h"//ブロック
#include "wallblock.h"//壁ブロック
#include "barrier.h"//バリアブロック
#include "boundary.h"//ステージ境界ブロック
#include "battery.h"//バッテリー
#include "keyblock.h"//鍵付きブロック
#include "collapseblock.h"//崩れるブロック
#include "effect3D.h"//3Dエフェクト

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CBlock::CBlock(int nPriority) : CObjectX(nPriority)
{
	//メンバ変数初期化
	m_pPrev = nullptr;//前のブロック情報
	m_pNext = nullptr;//次のブロック情報
	m_BlockType = {};//ブロックタイプ
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
	//テクスチャ生成
	CTexture* pTex = CManager::GetTexture();//取得
	int nTextureIdx = pTex->Regist(CXfile::GetSaveModelInfo(CXfile::BLOCK, m_BlockType)->aTextureFile);//登録
	BindTexture(pTex->GetAddress(nTextureIdx));//設定
	SetTextureIdx(nTextureIdx);//ID設定

	//モデル生成
	CXfile* pModel = CManager::GetModel();//取得
	int nModelIdx = pModel->Regist(CXfile::GetSaveModelInfo(CXfile::BLOCK, m_BlockType)->aModelFile);//登録
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
void CBlock::Uninit()
{
	//Xファイルオブジェクト終了処理
	CObjectX::Uninit();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CBlock::Update()
{
	//Xファイルオブジェクト更新処理
	CObjectX::Update();
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CBlock::Draw()
{
	//エディタモードの場合
	if (CManager::GetScene()->GetEditMode())
	{//描画せずに処理を抜ける
		return;
	}

	//Xファイルのオブジェクト描画処理
	CObjectX::Draw();
}

//===========================================================================================================
// 生成処理
//===========================================================================================================
CBlock* CBlock::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, int nType)
{
	//タイプに応じて動的確保
	CBlock* pBlock = NEW CBlock();
	switch (nType)
	{
		//壁ブロック
	case WALL:
		pBlock = NEW CWallBlock();
		break;

		//バリアブロック
	case BARRIER:
		pBlock = NEW CBarrier();
		break;

		//ステージ境界ブロック
	case BOUNDARY:
		pBlock = NEW CBoundary();
		break;

		//砲台
	case BATTERY:
		pBlock = NEW CBattery();
		break;

		//鍵付きブロック
	case KEY:
		pBlock = NEW CKeyBlock();
		break;

		//崩れるブロック
	case COLLAPSE:
		pBlock = NEW CCollapseBlock();
		break;

	default:
		break;
	}

	//パラメータ設定
	pBlock->SetType(TYPE::BLOCK);//オブジェクトタイプ設定
	pBlock->SetPos(pos);//位置
	pBlock->SetRot(rot);//向き
	pBlock->SetScale(scale);//拡大率
	pBlock->SetBlockType((BLOCKTYPE)nType);//ブロックタイプ設定

	//初期化処理
	pBlock->Init();

	return pBlock;
}

//===========================================================================================================
// 当たり判定
//===========================================================================================================
void CBlock::Collision(D3DXVECTOR3& pos, D3DXVECTOR3& oldpos, D3DXVECTOR3& size)
{
	//情報を取得
	D3DXVECTOR3 Pos = GetPos();//位置
	D3DXVECTOR3 Size = GetSize();//サイズ

	//左側の判定
	if (pos.x + size.x * 0.5f >= Pos.x - Size.x * 0.5f
		&& oldpos.x + size.x * 0.5f < Pos.x - Size.x * 0.5f)
	{//重なった場合
		if (pos.z + size.z * 0.5f > Pos.z - Size.z * 0.5f
			&& pos.z - size.z * 0.5f < Pos.z + Size.z * 0.5f)
		{//対象の手前側から奥側までの範囲内にいる場合
			pos.x = oldpos.x;
		}
	}

	//右側の判定
	if (pos.x - size.x * 0.5f <= Pos.x + Size.x * 0.5f
		&& oldpos.x - size.x * 0.5f > Pos.x + Size.x * 0.5f)
	{//重なった場合
		if (pos.z + size.z * 0.5f > Pos.z - Size.z * 0.5f
			&& pos.z - size.z * 0.5f < Pos.z + Size.z * 0.5f)
		{//対象の手前側から奥側までの範囲内にいる場合
			pos.x = oldpos.x;
		}
	}

	//手前側の判定
	if (pos.z + size.z * 0.5f >= Pos.z - Size.z * 0.5f
		&& oldpos.z + size.z * 0.5f < Pos.z - Size.z * 0.5f)
	{//重なった場合
		if (pos.x + size.x * 0.5f > Pos.x - Size.x * 0.5f
			&& pos.x - size.x * 0.5f < Pos.x + Size.x * 0.5f)
		{//対象の左側から右側までの範囲内にいる場合
			pos.z = oldpos.z;
		}
	}

	//奥側の判定
	if (pos.z - size.z * 0.5f <= Pos.z + Size.z * 0.5f
		&& oldpos.z - size.z * 0.5f > Pos.z + Size.z * 0.5f)
	{//重なった場合
		if (pos.x + size.x * 0.5f > Pos.x - Size.x * 0.5f
			&& pos.x - size.x * 0.5f < Pos.x + Size.x * 0.5f)
		{//対象の左側から右側までの範囲内にいる場合
			pos.z = oldpos.z;
		}
	}
}

//===========================================================================================================
// 反射処理
//===========================================================================================================
bool CBlock::Reflection(D3DXVECTOR3& pos, D3DXVECTOR3& oldpos, D3DXVECTOR3& move, D3DXVECTOR3& rot, D3DXVECTOR3& size)
{
	//ローカル変数宣言
	bool bReflection = false;//反射フラグ
	D3DXVECTOR3 Pos = GetPos();//位置情報
	D3DXVECTOR3 Size = GetSize();//サイズ情報

	//左側の判定
	if (pos.x + size.x * 0.5f >= Pos.x - Size.x * 0.5f && oldpos.x + size.x * 0.5f < Pos.x - Size.x * 0.5f)
	{//重なった場合
		if (pos.z + size.z * 0.5f >= Pos.z - Size.z * 0.5f && pos.z - size.z * 0.5f <= Pos.z + Size.z * 0.5f)
		{//対象の手前側から奥側までの範囲内にいる場合

			//ブロックの垂直ベクトルを求める
			SEGMENT(Pos.x, Pos.z - Size.z * 0.5f, Pos.x, Pos.z + Size.z * 0.5f);

			//弾の方向ベクトルを求める
			D3DXVECTOR3 f = pos - oldpos;

			//進行ベクトルを正規化
			D3DXVec3Normalize(&f, &f);

			//係数を求める
			D3DXVECTOR3 a(-f.x * m_Normal.x, -f.y * m_Normal.y, -f.z * m_Normal.z);

			//係数を正規化
			D3DXVec3Normalize(&a, &a);

			//平行ベクトルを求める
			D3DXVECTOR3 p(f.x + a.x * m_Normal.x, f.y + a.y * m_Normal.y, f.z + a.z * m_Normal.z);

			//平行ベクトルを正規化
			D3DXVec3Normalize(&p, &p);

			//反射ベクトルを求める
			D3DXVECTOR3 r(f.x + 2.0f * a.x * m_Normal.x, f.y + 2.0f * a.y * m_Normal.y, f.z + 2.0f * a.z * m_Normal.z);

			//反射ベクトルを正規化
			D3DXVec3Normalize(&r, &r);

			//反射ベクトルに対して、弾の向きを変更
			if (r.z > 0.0f)
			{//反射ベクトルが上向きの場合
				rot = D3DXVECTOR3(0.0f, (-D3DX_PI * 0.5f) * (2.0f + r.x), 0.0f);
			}
			else
			{//反射ベクトルが下向きの場合
				rot = D3DXVECTOR3(0.0f, (-D3DX_PI * 0.5f) * r.x, 0.0f);
			}

			//角度に合わせて移動量を変更
			move = D3DXVECTOR3(sinf(rot.y + D3DX_PI), 0.0f, cosf(rot.y + D3DX_PI));

			////移動量を変更
			//move.x *= -1.0f;

			//反射フラグをtrueにする
			bReflection = true;

			//衝撃波エフェクト生成
			CEffect3D::Create(CEffect3D::EFFECT_3D_SHOCKWAVE, {pos.x, Pos.y + 10.0f, pos.z}, { -D3DX_PI * 0.5f, D3DX_PI * 0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f });

			//タイトル画面以外
			if (CManager::GetScene()->GetMode() != CScene::MODE_TITLE)
			{
				//効果音再生
				CManager::GetSound()->Play(CSound::SE_BOUND);
			}
		}
	}

	//右側の判定
	if (pos.x - size.x * 0.5f <= Pos.x + Size.x * 0.5f && oldpos.x - size.x * 0.5f > Pos.x + Size.x * 0.5f)
	{//重なった場合
		if (pos.z + size.z * 0.5f >= Pos.z - Size.z * 0.5f && pos.z - size.z * 0.5f <= Pos.z + Size.z * 0.5f)
		{//対象の手前側から奥側までの範囲内にいる場合

			////ブロックの垂直ベクトルを求める
			//SEGMENT(Pos.x, Pos.z + Size.z * 0.5f, Pos.x, Pos.z - Size.z * 0.5f);

			////弾の方向ベクトルを求める
			//D3DXVECTOR3 f = pos - oldpos;

			////進行ベクトルを正規化
			//D3DXVec3Normalize(&f, &f);

			////係数を求める
			//D3DXVECTOR3 a(-f.x * m_Normal.x, -f.y * m_Normal.y, -f.z * m_Normal.z);

			////係数を正規化
			//D3DXVec3Normalize(&a, &a);

			////平行ベクトルを求める
			//D3DXVECTOR3 p(f.x + a.x * m_Normal.x, f.y + a.y * m_Normal.y, f.z + a.z * m_Normal.z);

			////平行ベクトルを正規化
			//D3DXVec3Normalize(&p, &p);

			////反射ベクトルを求める
			//D3DXVECTOR3 r(f.x + 2.0f * a.x * m_Normal.x, f.y + 2.0f * a.y * m_Normal.y, f.z + 2.0f * a.z * m_Normal.z);

			////反射ベクトルを正規化
			//D3DXVec3Normalize(&r, &r);

			////反射ベクトルに対して、弾の向きを変更
			//if (r.z > 0.0f)
			//{//反射ベクトルが上向きの場合
			//	rot = D3DXVECTOR3(0.0f, (D3DX_PI * 0.5f) * (2.0f + r.x), 0.0f);
			//}
			//else
			//{//反射ベクトルが下向きの場合
			//	rot = D3DXVECTOR3(0.0f, (-D3DX_PI * 0.5f) * r.x, 0.0f);
			//}

			////向きに合わせて移動量を変更
			//move = D3DXVECTOR3(sinf(rot.y + D3DX_PI) * 4.0f, 0.0f, cosf(rot.y + D3DX_PI) * 4.0f);

			//移動量を変更
			move.x *= -1.0f;

			//反射フラグをtrueにする
			bReflection = true;

			//衝撃波エフェクト生成
			CEffect3D::Create(CEffect3D::EFFECT_3D_SHOCKWAVE, { pos.x, Pos.y + 10.0f, pos.z }, { -D3DX_PI * 0.5f, -D3DX_PI * 0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f });

			//タイトル画面以外
			if (CManager::GetScene()->GetMode() != CScene::MODE_TITLE)
			{
				//効果音再生
				CManager::GetSound()->Play(CSound::SE_BOUND);
			}
		}
	}

	//奥側の判定
	if (pos.z - size.z * 0.5f <= Pos.z + Size.z * 0.5f && oldpos.z - size.z * 0.5f > Pos.z + Size.z * 0.5f)
	{//重なった場合
		if (pos.x + size.z * 0.5f >= Pos.x - Size.x * 0.5f && pos.x - size.z * 0.5f <= Pos.x + Size.x * 0.5f)
		{//対象の左側から右側までの範囲内にいる場合

			////ブロックの垂直ベクトルを求める
			//SEGMENT(Pos.x - Size.x * 0.5f, Pos.z, Pos.x + Size.x * 0.5f, Pos.z);

			////弾の進行ベクトルを求める
			//D3DXVECTOR3 f = pos - oldpos;

			////進行ベクトルを正規化
			//D3DXVec3Normalize(&f, &f);

			////係数を求める
			//D3DXVECTOR3 a(-f.x * m_Normal.x, -f.y * m_Normal.y, -f.z * m_Normal.z);

			////係数を正規化
			//D3DXVec3Normalize(&a, &a);

			////平行ベクトルを求める
			//D3DXVECTOR3 p(f.x + a.x * m_Normal.x, f.y + a.y * m_Normal.y, f.z + a.z * m_Normal.z);

			////平行ベクトルを正規化
			//D3DXVec3Normalize(&p, &p);

			////反射ベクトルを求める
			//D3DXVECTOR3 r(f.x + 2.0f * a.x * m_Normal.x, f.y + 2.0f * a.y * m_Normal.y, f.z + 2.0f * a.z * m_Normal.z);

			////反射ベクトルを正規化
			//D3DXVec3Normalize(&r, &r);

			////反射ベクトルに対して、弾の向きを変更
			//if (r.x > 0.0f)
			//{//反射ベクトルが右向きの場合
			//	rot = D3DXVECTOR3(0.0f, -D3DX_PI * r.z, 0.0f);
			//}
			//else
			//{//反射ベクトルが左向きの場合
			//	rot = D3DXVECTOR3(0.0f, D3DX_PI * r.z, 0.0f);
			//}

			////向きに合わせて移動量を変更
			//move = D3DXVECTOR3(sinf(rot.y + D3DX_PI) * 4.0f, 0.0f, cosf(rot.y + D3DX_PI) * 4.0f);

			//移動量を変更
			move.z *= -1.0f;

			//反射フラグをtrueにする
			bReflection = true;

			//衝撃波エフェクト生成
			CEffect3D::Create(CEffect3D::EFFECT_3D_SHOCKWAVE, { pos.x, Pos.y + 10.0f, pos.z }, { -D3DX_PI * 0.5f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });

			//タイトル画面以外
			if (CManager::GetScene()->GetMode() != CScene::MODE_TITLE)
			{
				//効果音再生
				CManager::GetSound()->Play(CSound::SE_BOUND);
			}
		}
	}

	//手前側の判定
	if (pos.z + size.z * 0.5f >= Pos.z - Size.z * 0.5f && oldpos.z + size.z * 0.5f < Pos.z - Size.z * 0.5f)
	{//重なった場合
		if (pos.x + size.z * 0.5f >= Pos.x - Size.x * 0.5f && pos.x - size.z * 0.5f <= Pos.x + Size.x * 0.5f)
		{//対象の左側から右側までの範囲内にいる場合

			////ブロックの垂直ベクトルを求める
			//SEGMENT(Pos.x + Size.x * 0.5f, Pos.z, Pos.x - Size.x * 0.5f, Pos.z);

			////弾の進行ベクトルを求める
			//D3DXVECTOR3 f = pos - oldpos;

			////進行ベクトルを正規化
			//D3DXVec3Normalize(&f, &f);

			////係数を求める
			//D3DXVECTOR3 a(-f.x * m_Normal.x, -f.y * m_Normal.y, -f.z * m_Normal.z);

			////係数を正規化
			//D3DXVec3Normalize(&a, &a);

			////平行ベクトルを求める
			//D3DXVECTOR3 p(f.x + a.x * m_Normal.x, f.y + a.y * m_Normal.y, f.z + a.z * m_Normal.z);

			////平行ベクトルを正規化
			//D3DXVec3Normalize(&p, &p);

			////反射ベクトルを求める
			//D3DXVECTOR3 r(f.x + 2.0f * a.x * m_Normal.x, f.y + 2.0f * a.y * m_Normal.y, f.z + 2.0f * a.z * m_Normal.z);

			////反射ベクトルを正規化
			//D3DXVec3Normalize(&r, &r);

			////反射ベクトルに対して、弾の向きを変更
			//if (r.x > 0.0f)
			//{//反射ベクトルが右向きの場合
			//	rot = D3DXVECTOR3(0.0f, -D3DX_PI * (1.0f + r.z), 0.0f);
			//}
			//else
			//{//反射ベクトルが左向きの場合
			//	rot = D3DXVECTOR3(0.0f, D3DX_PI * (1.0f + r.z), 0.0f);
			//}

			////向きに合わせて移動量を変更
			//move = D3DXVECTOR3(sinf(rot.y + D3DX_PI) * 4.0f, 0.0f, cosf(rot.y + D3DX_PI) * 4.0f);

			//移動量を変更
			move.z *= -1.0f;

			//反射フラグをtrueにする
			bReflection = true;

			//衝撃波エフェクト生成
			CEffect3D::Create(CEffect3D::EFFECT_3D_SHOCKWAVE, { pos.x, Pos.y + 10.0f, pos.z }, { -D3DX_PI * 0.5f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });

			//タイトル画面以外
			if (CManager::GetScene()->GetMode() != CScene::MODE_TITLE)
			{
				//効果音再生
				CManager::GetSound()->Play(CSound::SE_BOUND);
			}
		}
	}

	//反射フラグがtrueの場合
	if (bReflection)
	{
		//対象オブジェクトを過去の位置に戻す
		pos = oldpos;
	}

	//反射フラグを返す
	return bReflection;
}

//===========================================================================================================
// 垂直ベクトル計算処理
//===========================================================================================================
void CBlock::SEGMENT(float StartX, float StartZ, float EndX, float EndZ)
{
	//垂直ベクトルを求める
	D3DXVECTOR3 Start(StartX, 0.0f, StartZ);
	D3DXVECTOR3 End(EndX, 0.0f, EndZ);

	D3DXVECTOR3 r = End - Start;

	m_Normal.x = -r.z;
	m_Normal.z = r.x;

	//ベクトルを正規化
	D3DXVec3Normalize(&m_Normal, &m_Normal);
}
