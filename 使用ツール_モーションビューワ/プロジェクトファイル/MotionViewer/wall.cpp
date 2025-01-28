//=======================================================================================================================================================================================================================
//
// 壁に関する処理
// Author : Atsumu Kuboichi
//
//=======================================================================================================================================================================================================================
#include "wall.h"
#include "field.h"

//=======================================================================================================================================================================================================================
//グローバル変数
//=======================================================================================================================================================================================================================
LPDIRECT3DTEXTURE9 g_pTextureWall = NULL;		//テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffWall = NULL;	//頂点バッファへのポインタ
D3DXMATRIX g_mtxWorldWall;						//ワールドマトリックス
Wall g_Wall[MAX_WALL];							//壁の情報

//============================================================================================================================================================================================================
// 初期化処理
//============================================================================================================================================================================================================
void InitWall(void)
{
	VERTEX_3D* pVtx;							//頂点情報へのポインタ
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	//デバイスの取得
	Field* pField = GetField();					//フィールドの情報を取得

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data\\TEXTURE\\wall000.png",
		&g_pTextureWall);

	//頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * MAX_VER * MAX_WALL,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffWall,
		NULL);

	//頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffWall->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntWall = 0; nCntWall < MAX_WALL; nCntWall++)
	{
		//情報の初期化
		g_Wall[nCntWall].rot = D3DXVECTOR3(0.0f, nCntWall * (D3DX_PI / 2.0f), 0.0f);	//向き
		g_Wall[nCntWall].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);						//色
		g_Wall[nCntWall].nCnt = nCntWall;												//枚数

		//位置
		g_Wall[0].pos = D3DXVECTOR3(pField->pos.x,						 pField->pos.y, pField->pos.z + FIELD_RADIUS / 2.0f);
		g_Wall[1].pos = D3DXVECTOR3(pField->pos.x + FIELD_RADIUS / 2.0f, pField->pos.y, pField->pos.z);
		g_Wall[2].pos = D3DXVECTOR3(pField->pos.x,						 pField->pos.y, pField->pos.z - FIELD_RADIUS / 2.0f);
		g_Wall[3].pos = D3DXVECTOR3(pField->pos.x - FIELD_RADIUS / 2.0f, pField->pos.y, pField->pos.z);

		//頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(-FIELD_RADIUS / 2.0f, WALL_HEIGHT, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(+FIELD_RADIUS / 2.0f, WALL_HEIGHT, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-FIELD_RADIUS / 2.0f, 0.0f, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(+FIELD_RADIUS / 2.0f, 0.0f, 0.0f);

		//法線ベクトルの設定
		pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

		//頂点カラーの設定
		pVtx[0].col = g_Wall[nCntWall].col;
		pVtx[1].col = g_Wall[nCntWall].col;
		pVtx[2].col = g_Wall[nCntWall].col;
		pVtx[3].col = g_Wall[nCntWall].col;

		//テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtx += MAX_VER;
	}

	//頂点バッファをアンロックする
	g_pVtxBuffWall->Unlock();
}

//============================================================================================================================================================================================================
// 終了処理
//============================================================================================================================================================================================================
void UninitWall(void)
{
	//テクスチャの破棄
	if (g_pTextureWall != NULL)
	{
		g_pTextureWall->Release();
		g_pTextureWall = NULL;
	}

	//頂点バッファの破棄
	if (g_pVtxBuffWall != NULL)
	{
		g_pVtxBuffWall->Release();
		g_pVtxBuffWall = NULL;
	}
}

//============================================================================================================================================================================================================
// 描画処理
//============================================================================================================================================================================================================
void DrawWall(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	//デバイスの取得
	D3DXMATRIX mtxRot, mtxTrans;				//計算用マトリックス

	for (int nCntWall = 0; nCntWall < MAX_WALL; nCntWall++)
	{
		//ワールドマトリックスの初期化
		D3DXMatrixIdentity(&g_mtxWorldWall);

		//向きを反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Wall[nCntWall].rot.y, g_Wall[nCntWall].rot.x, g_Wall[nCntWall].rot.z);
		D3DXMatrixMultiply(&g_mtxWorldWall, &g_mtxWorldWall, &mtxRot);

		//位置を反映
		D3DXMatrixTranslation(&mtxTrans, g_Wall[nCntWall].pos.x, g_Wall[nCntWall].pos.y, g_Wall[nCntWall].pos.z);
		D3DXMatrixMultiply(&g_mtxWorldWall, &g_mtxWorldWall, &mtxTrans);

		//ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldWall);

		//頂点バッファをデータストリームに設定
		pDevice->SetStreamSource(0, g_pVtxBuffWall, 0, sizeof(VERTEX_3D));

		//頂点フォーマットの設定
		pDevice->SetFVF(FVF_VERTEX_3D);

		//テクスチャの設定
		pDevice->SetTexture(0, g_pTextureWall);

		//ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntWall * MAX_VER, 2);
	}
}

//============================================================================================================================================================================================================
//情報を取得
//============================================================================================================================================================================================================
Wall* GetWall(void)
{
	return &g_Wall[0];
}
