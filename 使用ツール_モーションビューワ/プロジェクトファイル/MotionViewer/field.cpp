//=======================================================================================================================================================================================================================
//
// フィールドの処理
// Author : Atsumu Kuboichi
//
//=======================================================================================================================================================================================================================
#include "field.h"

#define TEST_MESHWALL	(0)

//=======================================================================================================================================================================================================================
// グローバル変数
//=======================================================================================================================================================================================================================
LPDIRECT3DTEXTURE9 g_pTextureField = NULL;		//テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffField = NULL;	//頂点バッファへのポインタ
LPDIRECT3DINDEXBUFFER9 g_pldxBuffField = NULL;	//インデックスバッファへのポインタ
Field g_field;									//メッシュフィールドの情報

//============================================================================================================================================================================================================
// 初期化処理
//============================================================================================================================================================================================================
void InitField(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	//デバイスの取得
	VERTEX_3D* pVtx;							//頂点情報へのポインタ
	WORD* pldx;									//インデックス情報へのポインタ

	//頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * FIELD_VER,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffField,
		NULL);

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data\\TEXTURE\\field000.jpg",
		&g_pTextureField);

	//インデックスバッファの生成
	pDevice->CreateIndexBuffer(sizeof(WORD) * FIELD_INDEX,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&g_pldxBuffField,
		NULL);

	//情報の初期化
	g_field.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		//位置
	g_field.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		//向き
	g_field.col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	//色

	//頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffField->Lock(0, 0, (void**)&pVtx, 0);

	//インデックスバッファをロックし、インデックス情報へのポインタを取得
	g_pldxBuffField->Lock(0, 0, (void**)&pldx, 0);

	int nCntX = 0;	//X軸の頂点数
	int nCntZ = 0;	//Z軸の頂点数
	
	// 平面に配置
	for (int nCntField = 0; nCntField < FIELD_VER; nCntField++)
	{
		//頂点座標の設定
		pVtx[nCntField].pos = D3DXVECTOR3(FIELD_RADIUS * (nCntX / (float)(NUM_BLOCK_X)) - (FIELD_RADIUS * 0.5f), 0.0f, -FIELD_RADIUS * (nCntZ / (float)(NUM_BLOCK_Z)) + (FIELD_RADIUS * 0.5f));

		//テクスチャ座標の設定
		pVtx[nCntField].tex = D3DXVECTOR2(1.0f * (nCntX / (float)(NUM_BLOCK_X)), 1.0f * (nCntZ / (float)(NUM_BLOCK_Z)));

		nCntX++;

		if (nCntX == NUM_BLOCK_X + 1)
		{
			nCntX = 0;
			nCntZ++;
		}

		//法線ベクトルの設定
		pVtx[nCntField].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		//頂点カラーの設定
		pVtx[nCntField].col = g_field.col;
	}

	//インデックスの設定
	for (int nCntIndex = 0; nCntIndex < NUM_BLOCK_Z; nCntIndex++)
	{
		//Zのブロック数分配列を加算
		int nCntZ2 = nCntIndex * ((NUM_BLOCK_X + 1) * 2 + 2);

		//Zのブロック数分データを加算
		int nCntX2 = nCntIndex * (NUM_BLOCK_X + 1);

		for (int nCntIndex2 = 0; nCntIndex2 < NUM_BLOCK_X + 1; nCntIndex2++)
		{
			pldx[nCntIndex2 * 2 + nCntZ2] = (WORD)(nCntIndex2 + NUM_BLOCK_X + 1 + nCntX2);
			pldx[nCntIndex2 * 2 + 1 + nCntZ2] = (WORD)(nCntIndex2 + nCntX2);
		}

		//縮退の計算
		if (nCntIndex != NUM_BLOCK_Z - 1)
		{
			pldx[(NUM_BLOCK_X + 1) * 2 + 1 + nCntZ2] = pldx[(NUM_BLOCK_X + 1) * 2 + 1 + nCntZ2];
			pldx[(NUM_BLOCK_X + 1) * 2 + 2 + nCntZ2] = (WORD)(NUM_BLOCK_X + 1 + (NUM_BLOCK_X + 1) * (nCntIndex + 1));
		}
	}

	//頂点バッファをアンロックする
	g_pVtxBuffField->Unlock();

	//インデックスバッファをアンロックする
	g_pldxBuffField->Unlock();
}

//============================================================================================================================================================================================================
// 終了処理
//============================================================================================================================================================================================================
void UninitField(void)
{
	//テクスチャの破棄
	if (g_pTextureField != NULL)
	{
		g_pTextureField->Release();
		g_pTextureField = NULL;
	}
	//頂点バッファの破棄
	if (g_pVtxBuffField != NULL)
	{
		g_pVtxBuffField->Release();
		g_pVtxBuffField = NULL;
	}
	//インデックスバッファの解放
	if (g_pldxBuffField != NULL)
	{
		g_pldxBuffField->Release();
		g_pldxBuffField = NULL;
	}
}

//============================================================================================================================================================================================================
// 描画処理
//============================================================================================================================================================================================================
void DrawField(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	//デバイスの取得
	D3DXMATRIX mtxRot, mtxTrans;				//計算用マトリックス
	D3DXMATRIX mtxView;							//ビューマトリックス取得用

	for (int nCntCylinder = 0; nCntCylinder < NUM_BLOCK_X; nCntCylinder++)
	{
		//ワールドマトリックスの初期化
		D3DXMatrixIdentity(&g_field.mtxWorld);

		//向きを反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, g_field.rot.y, g_field.rot.x, g_field.rot.z);
		D3DXMatrixMultiply(&g_field.mtxWorld, &g_field.mtxWorld, &mtxRot);

		//位置を反映
		D3DXMatrixTranslation(&mtxTrans, g_field.pos.x, g_field.pos.y, g_field.pos.z);
		D3DXMatrixMultiply(&g_field.mtxWorld, &g_field.mtxWorld, &mtxTrans);

		//ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &g_field.mtxWorld);

		//頂点バッファをデータストリームに設定
		pDevice->SetStreamSource(0, g_pVtxBuffField, 0, sizeof(VERTEX_3D));

		//インデックスバッファをデータストリームに設定
		pDevice->SetIndices(g_pldxBuffField);

		//頂点フォーマットの設定
		pDevice->SetFVF(FVF_VERTEX_3D);

		//テクスチャの設定
		pDevice->SetTexture(0, g_pTextureField);

		//ポリゴンの描画
		pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, FIELD_VER, 0, FIELD_POLYGON);
	}
}

//============================================================================================================================================================================================================
// 情報の取得
//============================================================================================================================================================================================================
Field* GetField(void)
{
	return &g_field;
}