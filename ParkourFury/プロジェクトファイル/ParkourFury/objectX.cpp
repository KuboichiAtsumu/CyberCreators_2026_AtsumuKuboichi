//==============================================================================================================================================
//
// Xファイルオブジェクトに関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "objectX.h"

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CObjectX::CObjectX(int nPriority) : CObject3D(nPriority),
	m_pEffect(nullptr),
	m_vtxMin( 0.0f, 0.0f, 0.0f ),
	m_vtxMax( 0.0f, 0.0f, 0.0f ),
	m_Size( 0.0f, 0.0f, 0.0f ),
	m_Scale( 1.0f, 1.0f, 1.0f ),
	m_OutLineScale( 1.1f, 1.1f, 1.1f ),
	m_OutLineColor( 1.0f, 1.0f, 1.0f, 1.0f ),
	m_fLength(0.0f),
	m_fAngle(0.0f),
	m_StandLine(STAND_LINE::XY),
	m_bOutLine(false)
{
	//モデル情報初期化
	m_aModelInfo.pMesh = nullptr;
	m_aModelInfo.pBuffMat = nullptr;
	m_aModelInfo.dwNumMat = 0;
	m_aModelInfo.Diffuse.clear();
	m_aModelInfo.FirstDiffuse.clear();
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
	//アウトライン描画フラグがtrue
	if (m_bOutLine)
	{
		//デバイス情報取得
		LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

		//シェーダー読込
		if (FAILED(D3DXCreateEffectFromFile(pDevice, "data/SHADER/OutLine.fx", nullptr, nullptr, D3DXSHADER_DEBUG, nullptr, &m_pEffect, nullptr)))
		{
			return E_FAIL;
		}


	}

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
	//モデル情報初期化
	m_aModelInfo.pMesh = nullptr;
	m_aModelInfo.pBuffMat = nullptr;

	//シェーダー情報を解放
	m_pEffect = nullptr;

	//3Dオブジェクト解放処理
	CObject3D::Release();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CObjectX::Update()
{
	//角度に合わせて最大・最小の頂点座標を更新
	m_vtxMin.x = GetPos().x + sinf(GetRot().y - (D3DX_PI - m_fAngle)) * m_fLength;//X座標の最小値
	m_vtxMin.z = GetPos().z + cosf(GetRot().y + m_fAngle) * m_fLength;//Z座標の最小値
	m_vtxMax.x = GetPos().x + sinf(GetRot().y + m_fAngle) * m_fLength;//X座標の最大値
	m_vtxMax.z = GetPos().z + cosf(GetRot().y - (D3DX_PI - m_fAngle)) * m_fLength;//Z座標の最大値

	//更新された頂点座標に応じてサイズも更新
	m_Size =
	{
		abs(m_vtxMax.x - m_vtxMin.x),//x軸
		abs(m_vtxMax.y - m_vtxMin.y) * m_Scale.y,//y軸
		abs(m_vtxMax.z - m_vtxMin.z)//z軸
	};

	//座標の線上の設定
	SetStandLine();
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CObjectX::Draw()
{
	//ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//デバイスのポインタ
	D3DXMATRIX mtxWorld;//ワールドマトリックス情報
	D3DXMATRIX mtxRot, mtxTrans, mtxScale;//計算用マトリックス
	D3DMATERIAL9 matDef;//現在のマテリアル保存用
	D3DXMATERIAL* pMat;//マテリアルのデータへのポインタ
	LPDIRECT3DTEXTURE9 pTexture = GetTexture();//テクスチャ情報
	D3DXVECTOR3 Pos = GetPos();//座標情報
	D3DXVECTOR3 Rot = GetRot();//角度情報

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);

	//シェーダー情報が存在する
	if (m_pEffect != nullptr)
	{
		//シェーダーの拡大率を設定
		m_pEffect->SetFloat("Scale_X", m_Scale.x);
		m_pEffect->SetFloat("Scale_Y", m_Scale.y);
		m_pEffect->SetFloat("Scale_Z", m_Scale.z);

		//アウトラインの太さを設定
		m_pEffect->SetFloat("OutlineThickness_X", m_OutLineScale.x);
		m_pEffect->SetFloat("OutlineThickness_Y", m_OutLineScale.y);
		m_pEffect->SetFloat("OutlineThickness_Z", m_OutLineScale.z);

		//アウトラインのカラーを設定
		D3DXVECTOR4 Color = static_cast<D3DXVECTOR4>(m_OutLineColor);
		m_pEffect->SetVector("OutLineColor", &Color);
	}
	else
	{
		//拡大率を反映
		D3DXMatrixScaling(&mtxScale, m_Scale.x, m_Scale.y, m_Scale.z);
		D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxScale);
	}

	//角度を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, Rot.y, Rot.x, Rot.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	//座標を反映
	D3DXMatrixTranslation(&mtxTrans, Pos.x, Pos.y, Pos.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

	//法線の長さを1にする。
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &mtxWorld);

	//現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	//シェーダーパラメータ設定
	SetShader(mtxWorld);

	if (m_aModelInfo.pBuffMat != nullptr)
	{
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

			//シェーダー情報が存在する
			if(m_pEffect != nullptr)
			{
				//シェーダーの描画を開始
				UINT numPasses = 0;
				m_pEffect->Begin(&numPasses, 0);

				for (UINT Cnt = 0; Cnt < numPasses; Cnt++)
				{
					m_pEffect->BeginPass(Cnt);

					//モデルの描画
					m_aModelInfo.pMesh->DrawSubset(nCntMat);

					m_pEffect->EndPass();
				}

				//シェーダーの描画を終了
				m_pEffect->End();
			}
			//シェーダー情報が存在しない
			else
			{
				//モデルの描画
				m_aModelInfo.pMesh->DrawSubset(nCntMat);
			}
		}
	}

	//保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}

//===========================================================================================================
// シェーダーパラメータ設定
//===========================================================================================================
void CObjectX::SetShader(const D3DXMATRIX& mtxWorld)
{
	//シェーダー情報が存在しない
	if (m_pEffect == nullptr)
	{
		//処理を抜ける
		return;
	}

	//ローカル変数宣言
	CManager* pManager = CManager::GetInstance();//マネージャーインスタンス取得
	LPDIRECT3DDEVICE9 pDevice = pManager->GetRenderer()->GetDevice();//デバイスのポインタ
	D3DXMATRIX mtxView, mtxProj, mtxWorldViewProj;//計算用マトリックス

	/*pDevice->GetTransform(D3DTS_WORLD, &mtxWorld);*/
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);
	pDevice->GetTransform(D3DTS_PROJECTION, &mtxProj);

	D3DXMatrixMultiply(&mtxWorldViewProj, &mtxWorld, &mtxView);
	D3DXMatrixMultiply(&mtxWorldViewProj, &mtxWorldViewProj, &mtxProj);

	//シェーダーに行列を設定
	m_pEffect->SetMatrix("WorldViewProj", &mtxWorldViewProj);
	m_pEffect->SetMatrix("World", &mtxWorld);

	//ライトのベクトルを設定
	D3DXVECTOR3 lightDir(0.8f, 0.5f, -1.0f);
	D3DXVec3Normalize(&lightDir, &lightDir);
	m_pEffect->SetValue("LightDirection", &lightDir, sizeof(D3DXVECTOR3));
}

//===========================================================================================================
// 立っている線上の設定処理
//===========================================================================================================
void CObjectX::SetStandLine()
{
	//座標情報を取得
	D3DXVECTOR3 pos = GetPos();

	//Z軸の線上にいる場合
	if (pos.x > CManager::VTXMIN_FIELD.x && pos.x < CManager::VTXMAX_FIELD.x)
	{
		//立っている線上をXYに設定
		m_StandLine = STAND_LINE::XY;
	}
	//X軸の線上にいる場合
	else if (pos.z > CManager::VTXMIN_FIELD.z && pos.z < CManager::VTXMAX_FIELD.z)
	{
		//立っている線上をYZに設定
		m_StandLine = STAND_LINE::YZ;
	}
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

	//対角線の長さを求める
	m_fLength = sqrtf(powf(m_Size.x, 2.0f) + powf(m_Size.z, 2.0f)) * 0.5f;

	//対角線の角度を求める
	m_fAngle = atan2f(m_Size.x, m_Size.z);

	//頂点バッファのアンロック
	m_aModelInfo.pMesh->UnlockVertexBuffer();
}
