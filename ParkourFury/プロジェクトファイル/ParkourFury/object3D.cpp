//==============================================================================================================================================
//
// 3Dオブジェクトに関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "object3D.h"

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CObject3D::CObject3D(int nPriority) : CObject(nPriority),
	m_pos( 0.0f, 0.0f, 0.0f ),
	m_move( 0.0f, 0.0f, 0.0f ),
	m_rot( 0.0f, 0.0f, 0.0f ),
	m_size( 0.0f, 0.0f, 0.0f ),
	m_col( 1.0f, 1.0f, 1.0f, 1.0f ),
	m_nDivisionWidth(1),
	m_nDivisionHeight(1),
	m_nDelayAnim(1),
	m_nPatternAnimWidth(1),
	m_nPatternAnimHeight(1),
	m_nCounterAnim(0)
{
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CObject3D::~CObject3D()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CObject3D::Init()
{
	//ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//デバイスのポインタ
	VERTEX_3D* pVtx;//頂点情報へのポインタ
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();//頂点バッファへのポインタ

	//頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * MAX_VER,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&pVtxBuff,
		nullptr);

	//頂点バッファの設定
	SetVtxBuff(pVtxBuff);

	//頂点バッファをロックし、頂点情報へのポインタを取得
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点座標の設定
	pVtx[0].pos = { -m_size.x * 0.5f, m_size.y * 0.5f, m_size.z * 0.5f };
	pVtx[1].pos = { m_size.x * 0.5f, m_size.y * 0.5f, m_size.z * 0.5f };
	pVtx[2].pos = { -m_size.x * 0.5f, -m_size.y * 0.5f, -m_size.z * 0.5f };
	pVtx[3].pos = { m_size.x * 0.5f, -m_size.y * 0.5f, -m_size.z * 0.5f };

	//テクスチャ座標の設定
	pVtx[0].tex = { 0.0f, 0.0f };
	pVtx[1].tex = { 1.0f / (float)m_nDivisionWidth, 0.0f };
	pVtx[2].tex = { 0.0f, 1.0f / (float)m_nDivisionHeight };
	pVtx[3].tex = { 1.0f / (float)m_nDivisionWidth, 1.0f / (float)m_nDivisionHeight };

	for (int nCntVer = 0; nCntVer < MAX_VER; nCntVer++)
	{
		//法線ベクトルの設定
		pVtx[nCntVer].nor = { 0.0f, 1.0f, 0.0f };

		//頂点カラーの設定
		pVtx[nCntVer].col = m_col;
	}

	//頂点バッファをアンロックする
	pVtxBuff->Unlock();

	//基底クラス初期化処理
	if (FAILED(CObject::Init())) return E_FAIL;
	
	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CObject3D::Uninit()
{
	//削除フラグをtrueに設定
	SetDeleteFlag(true);
}

//===========================================================================================================
// 解放処理
//===========================================================================================================
void CObject3D::Release()
{
	//基底クラス解放処理
	CObject::Release();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CObject3D::Update()
{
	//ローカル変数宣言
	VERTEX_3D* pVtx; //頂点情報へのポインタ
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();//頂点バッファへのポインタ

	//頂点バッファをロックし、頂点情報へのポインタを取得
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(-m_size.x * 0.5f, m_size.y * 0.5f, m_size.z * 0.5f);
	pVtx[1].pos = D3DXVECTOR3(m_size.x * 0.5f, m_size.y * 0.5f, m_size.z * 0.5f);
	pVtx[2].pos = D3DXVECTOR3(-m_size.x * 0.5f, -m_size.y * 0.5f, -m_size.z * 0.5f);
	pVtx[3].pos = D3DXVECTOR3(m_size.x * 0.5f, -m_size.y * 0.5f, -m_size.z * 0.5f);

	for (int nCntVer = 0; nCntVer < MAX_VER; nCntVer++)
	{
		//頂点カラーの設定
		pVtx[nCntVer].col = m_col;
	}

	//頂点バッファをアンロックする
	pVtxBuff->Unlock();
}

//===========================================================================================================
// アニメーション処理
//===========================================================================================================
void CObject3D::UpdateAnim()
{
	//ローカル変数宣言
	VERTEX_3D* pVtx;
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();//頂点バッファへのポインタ

	//頂点バッファをロックし、頂点情報へのポインタを取得
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//アニメーションカウンタを加算
	m_nCounterAnim++;

	//テクスチャ更新処理
	if (m_nCounterAnim % m_nDelayAnim == 0)
	{
		//横のアニメーションパターンを加算
		m_nPatternAnimWidth++;

		//横のテクスチャパターンが最大まで達した場合
		if (m_nPatternAnimWidth > m_nDivisionWidth)
		{
			//横のアニメーションパターンをリセット
			m_nPatternAnimWidth = 1;

			//縦のアニメーションパターンを加算
			m_nPatternAnimHeight++;

			//縦のアニメーション
			if (m_nPatternAnimHeight > m_nDivisionHeight)
			{
				//縦のアニメーションパターンをリセット
				m_nPatternAnimHeight = 1;
			}
		}

		//テクスチャ座標を移動
		pVtx[0].tex.x = 1.0f / (float)(m_nDivisionWidth) * (float)(m_nPatternAnimWidth - 1);
		pVtx[1].tex.x = 1.0f / (float)(m_nDivisionWidth) * (float)m_nPatternAnimWidth;
		pVtx[2].tex.x = 1.0f / (float)(m_nDivisionWidth) * (float)(m_nPatternAnimWidth - 1);
		pVtx[3].tex.x = 1.0f / (float)(m_nDivisionWidth) * (float)m_nPatternAnimWidth;

		pVtx[0].tex.y = 1.0f / (float)(m_nDivisionHeight) * (float)(m_nPatternAnimHeight - 1);
		pVtx[1].tex.y = 1.0f / (float)(m_nDivisionHeight) * (float)(m_nPatternAnimHeight - 1);
		pVtx[2].tex.y = 1.0f / (float)(m_nDivisionHeight) * (float)m_nPatternAnimHeight;
		pVtx[3].tex.y = 1.0f / (float)(m_nDivisionHeight) * (float)m_nPatternAnimHeight;

		//アニメーションカウンタをリセット
		m_nCounterAnim = 0;
	}

	//頂点バッファをアンロックする
	pVtxBuff->Unlock();
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CObject3D::Draw()
{
	//ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//デバイスのポインタ
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();//頂点バッファへのポインタ
	LPDIRECT3DTEXTURE9 pTexture = GetTexture();//テクスチャ情報へのポインタ
	D3DXMATRIX mtxRot, mtxTrans;//計算用マトリックス
	D3DXMATRIX mtxWorld;//ワールドマトリックス

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);

	//角度を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	//座標を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &mtxWorld);

	//頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, pVtxBuff, 0, sizeof(VERTEX_3D));

	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	//テクスチャの設定
	pDevice->SetTexture(0, pTexture);

	//ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}
