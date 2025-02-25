//==============================================================================================================================================
//
// ヒートゲージに関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "heatgage.h"
#include "player.h"

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CHeatGage::CHeatGage() : CUI()
{
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CHeatGage::~CHeatGage()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CHeatGage::Init()
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

	//ゲーム中
	if (CManager::GetInstance()->GetScene()->GetSceneState() == CScene::SCENE::GAME)
	{
		//X軸のサイズを0に設定
		SetSize({ 0.0f, size.y });
	}

	//頂点座標の設定
	pVtx[0].pos.x = pos.x;
	pVtx[0].pos.y = pos.y - size.y * 0.5f;
	pVtx[1].pos.x = pos.x;
	pVtx[1].pos.y = pos.y - size.y * 0.5f;
	pVtx[2].pos.x = pos.x;
	pVtx[2].pos.y = pos.y + size.y * 0.5f;
	pVtx[3].pos.x = pos.x;
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
void CHeatGage::Uninit()
{
	//基底クラス終了処理
	CUI::Uninit();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CHeatGage::Update()
{
	//基底クラス更新処理
	CUI::Update();

	//プレイヤーオブジェクトを検索
	CObject* pFindObj = CObject::FindObject(Category::PLAYER);
	CPlayer* pPlayer = nullptr;

	//オブジェクト情報が存在する場合
	if (pFindObj != nullptr)
	{
		//プレイヤークラスにダウンキャスト
		pPlayer = CObject::DownCast<CPlayer, CObject>(pFindObj);

		//拡大・縮小処理
		Scaling(pPlayer->GetHeat());
	}
}

//===========================================================================================================
// 拡大・縮小処理
//===========================================================================================================
void CHeatGage::Scaling(float fHeat)
{
	//エディタ判定用フラグ
	static bool bEdit;

	//サイズ情報取得
	D3DXVECTOR2 size = GetSize();

	//座標情報を取得
	D3DXVECTOR2 pos = GetPos();

	//頂点バッファをロックし、頂点情報へのポインタを取得
	VERTEX_2D* pVtx;
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//エディタ中はサイズの最大値を更新
	if (CManager::GetInstance()->GetScene()->GetSceneState() != CScene::SCENE::GAME)
	{
		//エディタモードになった時にサイズ最大値に戻す
		if (!bEdit)
		{
			size = m_MaxSize;
			SetSize(size);
			bEdit = true;
		}

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
		pVtx[1].tex = { 1.0f, 0.0f };
		pVtx[2].tex = { 0.0f, 1.0f };
		pVtx[3].tex = { 1.0f, 1.0f };

		return;
	}

	//エディタ判定
	bEdit = false;

	//目標のサイズ
	D3DXVECTOR2 TargetSize(0.0f, size.y);

	//現在のヒート量の割合を求める
	float fRatioHeat = fHeat / CPlayer::MAX_HEAT;

	//割合が1.0を超えないように調整
	if (fRatioHeat > 1.0f)
	{
		fRatioHeat = 1.0f;
	}

	//ヒート量に合わせて目標のサイズを設定
	TargetSize.x = m_MaxSize.x * fRatioHeat;

	//目標のサイズが現在のサイズより大きい場合
	if (TargetSize.x - size.x > 0.0f)
	{
		//サイズを加算する
		size.x += GAGE_MOVE_SPEED;

		//目標サイズを超えた場合
		if (TargetSize.x - size.x < 0.0f)
		{
			//目標のサイズに調整
			size.x = TargetSize.x;
		}
	}
	//目標のサイズ現在のサイズより小さい場合
	else if (TargetSize.x - size.x < 0.0f)
	{
		//サイズを減算する
		size.x -= 0.7f;

		//目標サイズを超えた場合
		if (TargetSize.x - size.x > 0.0f)
		{
			//目標のサイズに調整
			size.x = TargetSize.x;
		}
	}

	//サイズを設定
	SetSize(size);

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
void CHeatGage::Draw()
{
	//基底クラス描画処理
	CUI::Draw();
}
