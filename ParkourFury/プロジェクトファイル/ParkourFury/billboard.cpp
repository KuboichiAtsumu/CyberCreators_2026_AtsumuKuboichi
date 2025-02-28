//==============================================================================================================================================
//
// ビルボードに関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "billboard.h"

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CBillboard::CBillboard(int nPriority) : CObject3D(nPriority)
{
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CBillboard::~CBillboard()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CBillboard::Init()
{
	//ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//デバイスのポインタ
	VERTEX_3D* pVtx;//頂点情報へのポインタ
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();//頂点バッファへのポインタ
	D3DXVECTOR3 size = GetSize();//サイズ情報

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
	pVtx[0].pos = D3DXVECTOR3(-size.x * 0.5f, size.y * 0.5f, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(size.x * 0.5f, size.y * 0.5f, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(-size.x * 0.5f, -size.y * 0.5f, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(size.x * 0.5f, -size.y * 0.5f, 0.0f);

	//テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f / static_cast<float>(GetDivisionWidth()), 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f / static_cast<float>(GetDivisionHeight()));
	pVtx[3].tex = D3DXVECTOR2(1.0f / static_cast<float>(GetDivisionWidth()), 1.0f / static_cast<float>(GetDivisionHeight()));

	for (int nCnt = 0; nCnt < MAX_VER; nCnt++)
	{
		//法線ベクトルの設定
		pVtx[nCnt].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		
		//頂点カラーの設定
		pVtx[nCnt].col = GetColor();
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
void CBillboard::Uninit()
{
	//基底クラス終了処理
	CObject3D::Uninit();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CBillboard::Update()
{
	//ローカル変数宣言
	VERTEX_3D* pVtx; //頂点情報へのポインタ
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();//頂点バッファへのポインタ
	D3DXVECTOR3 size = GetSize();//サイズ情報

	//頂点バッファをロックし、頂点情報へのポインタを取得
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(-size.x * 0.5f, size.y * 0.5f, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(size.x * 0.5f, size.y * 0.5f, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(-size.x * 0.5f, -size.y * 0.5f, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(size.x * 0.5f, -size.y * 0.5f, 0.0f);

	for (int nCntVer = 0; nCntVer < MAX_VER; nCntVer++)
	{
		//頂点カラーの設定
		pVtx[nCntVer].col = GetColor();
	}

	//頂点バッファをアンロックする
	pVtxBuff->Unlock();
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CBillboard::Draw()
{
	//ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//デバイスのポインタ
	D3DXMATRIX mtxTrans;//計算用マトリックス
	D3DXMATRIX mtxView;//ビューマトリックス取得用
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();//頂点バッファへのポインタ
	LPDIRECT3DTEXTURE9 pTexture = GetTexture();//テクスチャ情報へのポインタ
	D3DXMATRIX mtxWorld;//ワールドマトリックス情報
	D3DXVECTOR3 Pos = GetPos();//座標情報

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);

	//ビューマトリックスを取得
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	//ポリゴンをカメラに対して正面に向ける
	D3DXMatrixInverse(&mtxWorld, nullptr, &mtxView);//逆行列を求める
	mtxWorld._41 = 0.0f;
	mtxWorld._42 = 0.0f;
	mtxWorld._43 = 0.0f;

	//座標を反映
	D3DXMatrixTranslation(&mtxTrans, Pos.x, Pos.y, Pos.z);
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
