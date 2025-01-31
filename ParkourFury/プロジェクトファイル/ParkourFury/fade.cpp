//==============================================================================================================================================
//
// フェードに関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "fade.h"
#include "manager.h"

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CFade::CFade()
{
	//メンバ変数初期化
	m_state = CFade::STATE::FADE_NONE;//現在の状態
	m_NextMode = CScene::MODE::MODE_TITLE;//次のモード
	m_size = { SCREEN_WIDTH, SCREEN_HEIGHT };//サイズ
	m_pos = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };//座標
	m_rot = { 0.0f, 0.0f, 0.0f };//角度
	m_col = { 1.0f, 1.0f, 1.0f, 0.0f };//カラー
	m_fLength = sqrtf(powf(m_size.x, 2.0f) + powf(m_size.y, 2.0f)) * 0.5f;//対角線の長さ
	m_fAngle = atan2f(m_size.x, m_size.y);//対角線の角度
	m_pVtxBuff = nullptr;//頂点バッファ
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CFade::~CFade()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CFade::Init()
{
	//ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//デバイスのポインタ
	VERTEX_2D* pVtx; //頂点情報へのポインタ
	
	//頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * MAX_VER,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		nullptr);

	//頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点座標の設定
	pVtx[0].pos.x = m_pos.x + sinf(m_rot.z - (D3DX_PI - m_fAngle)) * m_fLength;
	pVtx[0].pos.y = m_pos.y + cosf(m_rot.z - (D3DX_PI - m_fAngle)) * m_fLength;
	pVtx[1].pos.x = m_pos.x + sinf(m_rot.z + (D3DX_PI - m_fAngle)) * m_fLength;
	pVtx[1].pos.y = m_pos.y + cosf(m_rot.z + (D3DX_PI - m_fAngle)) * m_fLength;
	pVtx[2].pos.x = m_pos.x + sinf(m_rot.z - m_fAngle) * m_fLength;
	pVtx[2].pos.y = m_pos.y + cosf(m_rot.z - m_fAngle) * m_fLength;
	pVtx[3].pos.x = m_pos.x + sinf(m_rot.z + m_fAngle) * m_fLength;
	pVtx[3].pos.y = m_pos.y + cosf(m_rot.z + m_fAngle) * m_fLength;

	//テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	for (int nCntVtx = 0; nCntVtx < MAX_VER; nCntVtx++)
	{
		//頂点座標のZ軸を0.0fで固定
		pVtx[nCntVtx].pos.z = 0.0f;

		//rhwの設定
		pVtx[nCntVtx].rhw = 1.0f;

		//頂点カラーの設定
		pVtx[nCntVtx].col = m_col;
	}

	//頂点バッファをアンロックする
	m_pVtxBuff->Unlock();

	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CFade::Uninit()
{
	//頂点バッファの破棄
	if (m_pVtxBuff != nullptr)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = nullptr;
	}
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CFade::Update()
{
	//ローカル変数宣言
	VERTEX_2D* pVtx;//頂点情報へのポインタ

	//フェードイン
	if (m_state == CFade::STATE::FADE_IN)
	{
		//ポリゴンを透明にしていく
		m_col.a -= FADE_SPEED;

		//α値が0.0f以下の場合
		if (m_col.a <= 0.0f)
		{
			//0.0f以下にならないように調整
			m_col.a = 0.0f;

			//フェードしない状態にする
			m_state = CFade::STATE::FADE_NONE;
		}
	}

	//フェードアウト
	else if (m_state == CFade::STATE::FADE_OUT)
	{
		//ポリゴンを不透明にしていく
		m_col.a += FADE_SPEED;

		//α値が1.0f以上の場合
		if (m_col.a >= 1.0f)
		{
			//1.0f以上にならないように調整
			m_col.a = 1.0f;

			//フェードイン状態にする
			m_state = CFade::STATE::FADE_IN;

			//次の画面へ移行
			CManager::GetInstance()->SetMode(m_NextMode);
		}
	}

	//頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点カラーの設定
	for (int nCntVtx = 0; nCntVtx < MAX_VER; nCntVtx++)
	{
		pVtx[nCntVtx].col = m_col;
	}

	//頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CFade::Draw()
{
	//フェードしていない場合
	if (m_state == STATE::FADE_NONE)
	{
		//描画せずに処理を抜ける
		return;
	}

	//ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//デバイスへのポインタ

	//頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	//テクスチャの設定
	pDevice->SetTexture(0, nullptr);

	//ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//===========================================================================================================
// 生成処理
//===========================================================================================================
CFade* CFade::Create()
{
	//メモリを動的確保
	CFade* pFade = NEW CFade();

	//初期化処理
	pFade->Init();

	return pFade;
}

//===========================================================================================================
// 状態設定処理
//===========================================================================================================
void CFade::SetFade(CScene::MODE mode)
{
	//次のシーンを設定
	m_NextMode = mode;

	//フェードアウト状態にする
	m_state = CFade::STATE::FADE_OUT;
}
