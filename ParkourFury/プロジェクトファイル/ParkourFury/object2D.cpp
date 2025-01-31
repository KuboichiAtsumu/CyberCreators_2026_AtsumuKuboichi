//==============================================================================================================================================
//
// 2Dオブジェクトに関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "object2D.h"//2Dオブジェクト

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CObject2D::CObject2D(int nPriority) : CObject(nPriority)
{
	//メンバ変数初期化
	m_pos = { 0.0f, 0.0f };//座標
	m_size = { 0.0f, 0.0f };//サイズ
	m_move = { 0.0f, 0.0f };//移動量
	m_rot = { 0.0f, 0.0f, 0.0f };//角度
	m_col = { 1.0f, 1.0f, 1.0f, 1.0f };//色

	//テクスチャアニメーション
	m_nDivisionWidth = 1;//横のテクスチャ分割数
	m_nDivisionHeight = 1;//縦のテクスチャ分割数
	m_nDelayAnim = 1;//アニメーション遅延
	m_nPatternAnimWidth = 1;//横のアニメーションパターン
	m_nPatternAnimHeight = 1;//縦のアニメーションパターン
	m_nCounterAnim = 0;//アニメーションカウンタ

	m_fAngle = 0.0f;//対角線の角度
	m_fLength = 0.0f;//対角線の長さ
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CObject2D::~CObject2D()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CObject2D::Init()
{
	//ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//デバイスのポインタ
	VERTEX_2D* pVtx; //頂点情報へのポインタ
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();//頂点バッファへのポインタ

	//メンバ変数初期化
	m_fLength = sqrtf(powf(m_size.x, 2.0f) + powf(m_size.y, 2.0f)) * 0.5f;//対角線の長さ
	m_fAngle = atan2f(m_size.x, m_size.y);//対角線の角度

	//頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * MAX_VER,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&pVtxBuff,
		nullptr);

	//頂点バッファの設定
	SetVtxBuff(pVtxBuff);

	//頂点バッファをロックし、頂点情報へのポインタを取得
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点座標の設定
	pVtx[0].pos.x = m_pos.x + sinf(m_rot.z - (D3DX_PI - m_fAngle)) * m_fLength;
	pVtx[0].pos.y = m_pos.y + cosf(m_rot.z - (D3DX_PI - m_fAngle)) * m_fLength;

	pVtx[1].pos.x = m_pos.x + sinf(m_rot.z + (D3DX_PI - m_fAngle)) * m_fLength;
	pVtx[1].pos.y = m_pos.y + cosf(m_rot.z + (D3DX_PI - m_fAngle)) * m_fLength;

	pVtx[2].pos.x = m_pos.x + sinf(m_rot.z - m_fAngle) * m_fLength;
	pVtx[2].pos.y = m_pos.y + cosf(m_rot.z - m_fAngle) * m_fLength;

	pVtx[3].pos.x = m_pos.x + sinf(m_rot.z + m_fAngle) * m_fLength;
	pVtx[3].pos.y = m_pos.y + cosf(m_rot.z + m_fAngle) * m_fLength;

	//分割数保存
	float fDivisionWidth = static_cast<float>(m_nDivisionWidth);
	float fDivisionHeight = static_cast<float>(m_nDivisionHeight);

	//テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f / fDivisionWidth, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f / fDivisionHeight);
	pVtx[3].tex = D3DXVECTOR2(1.0f / fDivisionWidth, 1.0f / fDivisionHeight);

	for (int nCntVtx = 0; nCntVtx < MAX_VER; nCntVtx++)
	{
		//rhwの設定
		pVtx[nCntVtx].rhw = 1.0f;

		//頂点カラーの設定
		pVtx[nCntVtx].col = m_col;
	}

	//頂点バッファをアンロックする
	pVtxBuff->Unlock();

	//基底クラス初期化処理
	if (FAILED(CObject::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CObject2D::Uninit()
{
	//削除フラグをtrueに設定
	SetDeleteFlag(true);
}

//===========================================================================================================
// 解放処理
//===========================================================================================================
void CObject2D::Release()
{
	//基底クラス解放処理
	CObject::Release();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CObject2D::Update()
{
	//ローカル変数宣言
	VERTEX_2D* pVtx;//頂点情報へのポインタ
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();//頂点バッファへのポインタ

	//対角線
	m_fLength = sqrtf(powf(m_size.x, 2.0f) + powf(m_size.y, 2.0f)) * 0.5f;//長さ
	m_fAngle = atan2f(m_size.x, m_size.y);//角度

	//頂点バッファをロックし、頂点情報へのポインタを取得
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点座標の設定
	pVtx[0].pos.x = m_pos.x + sinf(m_rot.z - (D3DX_PI - m_fAngle)) * m_fLength;
	pVtx[0].pos.y = m_pos.y + cosf(m_rot.z - (D3DX_PI - m_fAngle)) * m_fLength;

	pVtx[1].pos.x = m_pos.x + sinf(m_rot.z + (D3DX_PI - m_fAngle)) * m_fLength;
	pVtx[1].pos.y = m_pos.y + cosf(m_rot.z + (D3DX_PI - m_fAngle)) * m_fLength;

	pVtx[2].pos.x = m_pos.x + sinf(m_rot.z - m_fAngle) * m_fLength;
	pVtx[2].pos.y = m_pos.y + cosf(m_rot.z - m_fAngle) * m_fLength;

	pVtx[3].pos.x = m_pos.x + sinf(m_rot.z + m_fAngle) * m_fLength;
	pVtx[3].pos.y = m_pos.y + cosf(m_rot.z + m_fAngle) * m_fLength;

	//ローカル変数に情報を保存
	float fDivisionWidth = static_cast<float>(m_nDivisionWidth);//横のテクスチャ分割数
	float fDivisionHeight = static_cast<float>(m_nDivisionHeight);//縦のテクスチャ分割数
	float fPatternAnimWidth = static_cast<float>(m_nPatternAnimWidth);//横のアニメーションパターン
	float fPatternAnimHeight = static_cast<float>(m_nPatternAnimHeight);//縦のアニメーションパターン

	//テクスチャ座標の設定
	pVtx[0].tex.x = 1.0f / fDivisionWidth * fPatternAnimWidth;
	pVtx[1].tex.x = 1.0f / fDivisionWidth * (fPatternAnimWidth + 1.0f);
	pVtx[2].tex.x = 1.0f / fDivisionWidth * fPatternAnimWidth;
	pVtx[3].tex.x = 1.0f / fDivisionWidth * (fPatternAnimWidth + 1.0f);

	pVtx[0].tex.y = 1.0f / fDivisionHeight * (fPatternAnimHeight - 1.0f);
	pVtx[1].tex.y = 1.0f / fDivisionHeight * (fPatternAnimHeight - 1.0f);
	pVtx[2].tex.y = 1.0f / fDivisionHeight * fPatternAnimHeight;
	pVtx[3].tex.y = 1.0f / fDivisionHeight * fPatternAnimHeight;

	//頂点カラーの設定
	for (int nCntVtx = 0; nCntVtx < MAX_VER; nCntVtx++)
	{
		pVtx[nCntVtx].col = m_col;
	}

	//頂点バッファをアンロックする
	pVtxBuff->Unlock();
}

//===========================================================================================================
// アニメーション処理
//===========================================================================================================
void CObject2D::UpdateAnim()
{
	//ローカル変数宣言
	VERTEX_2D* pVtx;
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

		//アニメーションカウンタをリセット
		m_nCounterAnim = 0;
	}

	//頂点バッファをアンロックする
	pVtxBuff->Unlock();
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CObject2D::Draw()
{
	//エディタモードの場合
	if (CManager::GetInstance()->GetScene()->GetSceneState() == CScene::SCENE::EDIT_3D)
	{//描画せずに処理を抜ける
		return;
	}

	//ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//デバイスへのポインタ
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();//頂点バッファへのポインタ
	LPDIRECT3DTEXTURE9 pTexture = GetTexture();//テクスチャ情報へのポインタ

	//頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, pVtxBuff, 0, sizeof(VERTEX_2D));

	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	//テクスチャの設定
	pDevice->SetTexture(0, pTexture);

	//ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}
