//==============================================================================================================================================
//
// 3Dオブジェクトに関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "objectX.h"//Xファイルオブジェクト

//===========================================================================================================
// 静的メンバ変数初期化
//===========================================================================================================
int CObjectX::m_nNumType[CXfile::MAX_CATEGORY] = {};//各カテゴリーのタイプ数

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CObjectX::CObjectX(int nPriority) : CObject3D(nPriority)
{
	//メンバ変数初期化
	m_aModelInfo = {};//モデル情報
	m_nModelIdx = 0;//モデルのID
	m_vtxMin = { 0.0f, 0.0f, 0.0f };//最小頂点座標
	m_vtxMax = { 0.0f, 0.0f, 0.0f };//最大頂点座標
	m_Size = { 0.0f, 0.0f, 0.0f };//サイズ
	m_Scale = { 1.0f, 1.0f, 1.0f };//拡大率
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CObjectX::~CObjectX()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CObjectX::Init()
{
	//オブジェクト初期化処理
	if (FAILED(CObject::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CObjectX::Uninit()
{
	//3Dオブジェクト終了処理
	CObject3D::Uninit();
}

//===========================================================================================================
// 解放処理
//===========================================================================================================
void CObjectX::Release()
{
	//モデル情報削除
	m_aModelInfo = {};

	//3Dオブジェクト解放処理
	CObject3D::Release();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CObjectX::Update()
{
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CObjectX::Draw()
{
	//ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();//デバイスのポインタ
	D3DXMATRIX mtxRot, mtxTrans, mtxScale;//計算用マトリックス
	D3DMATERIAL9 matDef;//現在のマテリアル保存用
	D3DXMATERIAL* pMat;//マテリアルのデータへのポインタ
	D3DXMATRIX mtxWorld = GetMatrix();//ワールドマトリックス情報
	LPDIRECT3DTEXTURE9 pTexture = GetTexture();//テクスチャ情報
	D3DXVECTOR3 Pos = GetPos();//位置情報
	D3DXVECTOR3 Rot = GetRot();//向き情報

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);

	//拡大率を反映
	D3DXMatrixScaling(&mtxScale, m_Scale.x, m_Scale.y, m_Scale.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxScale);

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, Rot.y, Rot.x, Rot.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, Pos.x, Pos.y, Pos.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

	//法線の長さを1にする。
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &mtxWorld);

	//現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	//マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)m_aModelInfo.pBuffMat->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)m_aModelInfo.dwNumMat; nCntMat++)
	{
		//色合い設定
		pMat->MatD3D.Diffuse = m_aModelInfo.Diffuse[nCntMat];

		//マテリアルの設定
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		//テクスチャの設定
		pDevice->SetTexture(0, pTexture);

		//モデルの描画
		m_aModelInfo.pMesh->DrawSubset(nCntMat);
	}

	//保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}

//===========================================================================================================
// モデル登録
//===========================================================================================================
void CObjectX::SetSize()
{
	//ローカル変数宣言
	BYTE* pVtxBuff;//頂点バッファへのポインタ

	//頂点数の取得
	int nNumVtx = m_aModelInfo.pMesh->GetNumVertices();

	//頂点フォーマットのサイズを取得
	DWORD sizeFVF = D3DXGetFVFVertexSize(m_aModelInfo.pMesh->GetFVF());

	//頂点バッファのロック
	m_aModelInfo.pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

	for (int nIdxVtx = 0; nIdxVtx < nNumVtx; nIdxVtx++)
	{
		//頂点座標の代入
		D3DXVECTOR3 vtx = *(D3DXVECTOR3*)pVtxBuff;

		//モデルの最小値と最大値を取得
		if (vtx.x < m_vtxMin.x)
		{//X座標の最小値
			m_vtxMin.x = vtx.x;
		}
		if (vtx.y < m_vtxMin.y)
		{//Y座標の最小値
			m_vtxMin.y = vtx.y;
		}
		if (vtx.z < m_vtxMin.z)
		{//Z座標の最小値
			m_vtxMin.z = vtx.z;
		}
		if (vtx.x > m_vtxMax.x)
		{//X座標の最大値
			m_vtxMax.x = vtx.x;
		}
		if (vtx.y > m_vtxMax.y)
		{//Y座標の最大値
			m_vtxMax.y = vtx.y;
		}
		if (vtx.z > m_vtxMax.z)
		{//Z座標の最大値
			m_vtxMax.z = vtx.z;
		}

		//頂点フォーマットのサイズ分ポインタを進める
		pVtxBuff += sizeFVF;
	}

	//種類ごとのサイズを求める
	m_Size =
	{
		(m_vtxMax.x - m_vtxMin.x) * m_Scale.x,//x軸
		(m_vtxMax.y - m_vtxMin.y) * m_Scale.y,//y軸
		(m_vtxMax.z - m_vtxMin.z) * m_Scale.z//z軸
	};

	//頂点バッファのアンロック
	m_aModelInfo.pMesh->UnlockVertexBuffer();
}
