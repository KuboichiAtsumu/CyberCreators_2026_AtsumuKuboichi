//==============================================================================================================================================
//
// 3Dオブジェクトに関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "object3D.h"//3Dオブジェクト
#include "zone.h"//ゾーン

//===========================================================================================================
// 静的メンバ変数初期化
//===========================================================================================================
int CObject3D::m_nAllEachStageID[CGame::MAX_STAGE][MAX_OBJECT_TYPE]{};//各ステージの各オブジェクトの合計数

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CObject3D::CObject3D(int nPriority) : CObject(nPriority)
{
	//メンバ変数初期化
	m_pos = { 0.0f, 0.0f, 0.0f };//位置
	m_move = { 0.0f, 0.0f, 0.0f };//移動量
	m_rot = { 0.0f, 0.0f, 0.0f };//向き
	m_size = { 0.0f, 0.0f, 0.0f };//サイズ
	m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);//色
	m_nLife = 0;//寿命
	m_nPostStageID = 0;//対象オブジェクトがいるステージのID
	m_nEachStageID = 0;//対象オブジェクトがいるステージのオブジェクトID
	m_nDivisionWidth = 1;//横のテクスチャ分割数
	m_nDivisionHeight = 1;//縦のテクスチャ分割数
	m_nDelayAnim = 1;//アニメーション遅延
	m_nPatternAnimWidth = 1;//横のアニメーションパターン
	m_nPatternAnimHeight = 1;//縦のアニメーションパターン
	m_nCounterAnim = 0;//アニメーションカウンタ
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
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();//デバイスのポインタ
	VERTEX_3D* pVtx;//頂点情報へのポインタ
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();//頂点バッファへのポインタ

	//頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * CProcess::MAX_VER,
		D3DUSAGE_WRITEONLY,
		CProcess::FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&pVtxBuff,
		nullptr);

	//頂点バッファの設定
	SetVtxBuff(pVtxBuff);

	//頂点バッファをロックし、頂点情報へのポインタを取得
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(-m_size.x * 0.5f, m_size.y * 0.5f, m_size.z * 0.5f);
	pVtx[1].pos = D3DXVECTOR3(m_size.x * 0.5f, m_size.y * 0.5f, m_size.z * 0.5f);
	pVtx[2].pos = D3DXVECTOR3(-m_size.x * 0.5f, -m_size.y * 0.5f, -m_size.z * 0.5f);
	pVtx[3].pos = D3DXVECTOR3(m_size.x * 0.5f, -m_size.y * 0.5f, -m_size.z * 0.5f);

	//テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f / (float)m_nDivisionWidth, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f / (float)m_nDivisionHeight);
	pVtx[3].tex = D3DXVECTOR2(1.0f / (float)m_nDivisionWidth, 1.0f / (float)m_nDivisionHeight);

	for (int nCntVer = 0; nCntVer < CProcess::MAX_VER; nCntVer++)
	{
		//法線ベクトルの設定
		pVtx[nCntVer].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		//頂点カラーの設定
		pVtx[nCntVer].col = m_col;
	}

	//頂点バッファをアンロックする
	pVtxBuff->Unlock();

	//オブジェクトの初期化処理
	if (FAILED(CObject::Init()))
	{
		return E_FAIL;
	}

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
	//該当ステージの該当タイプのオブジェクト総数を減算
	m_nAllEachStageID[m_nPostStageID][GetType()]--;

	//オブジェクト解放処理
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

	for (int nCntVer = 0; nCntVer < CProcess::MAX_VER; nCntVer++)
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
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();//デバイスのポインタ
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();//頂点バッファへのポインタ
	LPDIRECT3DTEXTURE9 pTexture = GetTexture();//テクスチャ情報へのポインタ
	D3DXMATRIX mtxRot, mtxTrans;//計算用マトリックス

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, pVtxBuff, 0, sizeof(VERTEX_3D));

	//頂点フォーマットの設定
	pDevice->SetFVF(CProcess::FVF_VERTEX_3D);

	//テクスチャの設定
	pDevice->SetTexture(0, pTexture);

	//ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//===========================================================================================================
// 自分がいるステージIDの登録処理
//===========================================================================================================
void CObject3D::SetPostStageID()
{
	//指定した順番にオブジェクト情報を取得する
	for (int nCnt = 0; nCnt < MAX_PRIORITY; nCnt++)
	{
		//先頭のオブジェクト情報を取得
		CObject* pObj = CObject::GetTop(nCnt);

		//オブジェクト情報がnullptrになるまで繰り返す
		while (pObj != nullptr)
		{
			//次のオブジェクト情報を取得
			CObject* pNext = pObj->GetNext();

			//オブジェクトタイプがゾーンの場合
			if (pObj->GetType() == CObject::ZONE)
			{
				//オブジェクト情報をエネミー情報に変換
				CZone* pZone = (CZone*)pObj;

				//位置情報を取得
				D3DXVECTOR3 pos = GetPos();

				//範囲内だった場合
				if (pZone->Judge(pos) && pZone->GetZoneType() == CZone::STAGE)
				{
					//プレイヤーがいるステージの番号を保存する
					m_nPostStageID = pZone->GetID();

					//該当ステージの該当オブジェクトIDを設定
					m_nEachStageID = m_nAllEachStageID[m_nPostStageID][GetType()];

					//該当ステージの該当オブジェクトの合計数を加算
					m_nAllEachStageID[m_nPostStageID][GetType()]++;
				}
			}

			//オブジェクト情報を次の情報に変更する
			pObj = pNext;
		}
	}
}
