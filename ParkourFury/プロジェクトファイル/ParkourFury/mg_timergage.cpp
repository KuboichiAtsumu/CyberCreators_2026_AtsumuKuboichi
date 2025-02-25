//==============================================================================================================================================
//
// ミニゲームタイマーゲージに関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "mg_timergage.h"
#include "game.h"
#include "minigame.h"

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CMG_TimerGage::CMG_TimerGage() : CUI()
{
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CMG_TimerGage::~CMG_TimerGage()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CMG_TimerGage::Init()
{
	//ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//デバイスのポインタ
	VERTEX_2D* pVtx; //頂点情報へのポインタ
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();//頂点バッファへのポインタ

	//テクスチャ生成
	CTexture* pTex = CTexture::GetInstance();//取得
	BindTexture(pTex->GetAddress(CTag::TAG::UI, static_cast<int>(GetUIType())));//設定

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

	//座標設定
	D3DXVECTOR2 pos(GetPos());

	//サイズ設定
	D3DXVECTOR2 size(GetSize());
	m_MaxSize = size;

	//頂点座標の設定
	pVtx[0].pos.x = pos.x;
	pVtx[0].pos.y = pos.y - size.y * 0.5f;
	pVtx[1].pos.x = pos.x + size.x;
	pVtx[1].pos.y = pos.y - size.y * 0.5f;
	pVtx[2].pos.x = pos.x;
	pVtx[2].pos.y = pos.y + size.y * 0.5f;
	pVtx[3].pos.x = pos.x + size.x;
	pVtx[3].pos.y = pos.y + size.y * 0.5f;

	//テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	//カラー情報取得
	D3DXCOLOR col = GetColor();

	//透過させる
	col.a = 0.8f;
	SetColor(col);

	for (int nCntVtx = 0; nCntVtx < MAX_VER; nCntVtx++)
	{
		//rhwの設定
		pVtx[nCntVtx].rhw = 1.0f;

		//頂点カラーを赤色に設定
		pVtx[nCntVtx].col = col;
	}

	//頂点バッファをアンロックする
	pVtxBuff->Unlock();

	//エディット可能フラグ
	SetEditFlag(false);

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
void CMG_TimerGage::Uninit()
{
	//基底クラス終了処理
	CUI::Uninit();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CMG_TimerGage::Update()
{
	//ゲームシーンのインスタンス取得
	CGame* pGame = CGame::GetInstance();

	//ミニゲーム中ではない場合
	if (pGame != nullptr &&
		pGame->GetArea() != CGame::GAME_AREA::MINI_GAME)
	{
		//終了処理
		Uninit();

		//処理を抜ける
		return;
	}

	//基底クラス更新処理
	CUI::Update();

	//プレイヤーオブジェクトを検索
	CObject* pFindObj = CObject::FindObject(Category::EVENT);
	CMiniGame* pMiniGame = nullptr;

	//検索結果が一致
	if (pFindObj != nullptr)
	{
		//プレイヤークラスにダウンキャスト
		pMiniGame = CObject::DownCast<CMiniGame, CObject>(pFindObj);
	}
	else
	{
		return;
	}

	//サイズ情報取得
	D3DXVECTOR2 size = GetSize();

	//サイズを減算する
	size.x -= m_MaxSize.x / static_cast<float>(pMiniGame->TIMER) / MAX_FRAME;

	//サイズを設定
	SetSize(size);

	//頂点バッファをロックし、頂点情報へのポインタを取得
	VERTEX_2D* pVtx;
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//座標情報を取得
	D3DXVECTOR2 pos = GetPos();

	//頂点座標の設定
	pVtx[0].pos.x = pos.x;
	pVtx[0].pos.y = pos.y - size.y * 0.5f;
	pVtx[1].pos.x = pos.x + size.x;
	pVtx[1].pos.y = pos.y - size.y * 0.5f;
	pVtx[2].pos.x = pos.x;
	pVtx[2].pos.y = pos.y + size.y * 0.5f;
	pVtx[3].pos.x = pos.x + size.x;
	pVtx[3].pos.y = pos.y + size.y * 0.5f;

	//テクスチャ座標の設定
	pVtx[0].tex = { 0.0f, 0.0f };
	pVtx[1].tex = { size.x / m_MaxSize.x, 0.0f };
	pVtx[2].tex = { 0.0f, 1.0f };
	pVtx[3].tex = { size.x / m_MaxSize.x, 1.0f };

	//頂点バッファをアンロックする
	pVtxBuff->Unlock();
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CMG_TimerGage::Draw()
{
	//基底クラス描画処理
	CUI::Draw();
}
