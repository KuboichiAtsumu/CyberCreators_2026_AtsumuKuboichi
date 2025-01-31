//==============================================================================================================================================
//
// パーティクルに関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "particle.h"
#include "normalparticle.h"
#include "ascendparticle.h"

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CParticle::CParticle(int nPriority) : CBillboardEffect{ nPriority },
	m_MaxSize( 0.0f, 0.0f, 0.0f ),
	m_type(TYPE::NONE),
	m_nLife(10),
	m_nMaxLife(m_nLife)
{
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CParticle::~CParticle()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CParticle::Init()
{
	//基底クラス初期化処理
	if (FAILED(CBillboardEffect::Init())) return E_FAIL;
	
	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CParticle::Uninit()
{
	//基底クラス終了処理
	CBillboardEffect::Uninit();
}

//===========================================================================================================
// 解放処理
//===========================================================================================================
void CParticle::Release()
{
	//基底クラス解放処理
	CBillboardEffect::Release();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CParticle::Update()
{
	//座標更新
	SetPos(GetPos() + GetMove());

	//寿命減少
	m_nLife--;

	//寿命が0
	if (m_nLife <= 0)
	{
		//終了処理
		Uninit();
	}

	//寿命に合わせて薄くする
	D3DXVECTOR3 size = GetSize();
	size.x = m_MaxSize.x * (static_cast<float>(m_nLife) / static_cast<float>(m_nMaxLife));
	size.y = m_MaxSize.y * (static_cast<float>(m_nLife) / static_cast<float>(m_nMaxLife));
	SetSize(size);

	//基底クラス更新処理
	CBillboardEffect::Update();
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CParticle::Draw()
{
	//ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//デバイスのポインタ
	D3DXMATRIX mtxTrans;//計算用マトリックス
	D3DXMATRIX mtxView;//ビューマトリックス取得用
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();//頂点バッファへのポインタ
	LPDIRECT3DTEXTURE9 pTexture = GetTexture();//テクスチャ情報へのポインタ
	D3DXMATRIX mtxWorld;//ワールドマトリックス情報
	D3DXVECTOR3 pos = GetPos();//座標情報

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
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

	//ライトを無効化
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	//Zバッファ書き込み無効化
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	//アルファテスト有効化
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &mtxWorld);

	//頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, pVtxBuff, 0, sizeof(VERTEX_3D));

	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	//アルファブレンディングを加算合成に設定
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	//テクスチャの設定
	pDevice->SetTexture(0, pTexture);

	//ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	//アルファブレンディングを元に戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//ライトを有効化
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	//Zバッファ書き込み有効化
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	//アルファテスト無効化
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}

//===========================================================================================================
// 生成処理
//===========================================================================================================
CParticle* CParticle::Create(TYPE type, const D3DXVECTOR3& pos, const D3DXVECTOR3& size, int nLife, const D3DXCOLOR& col)
{
	//ローカル変数宣言
	CParticle* pParticle = nullptr;//パーティクル情報のポインタ

	switch (type)
	{
		//ノーマル
	case TYPE::NORMAL:
		pParticle = NEW CNormalParticle();
		break;

		//上昇
	case TYPE::ASCEND:
		pParticle = NEW CAscendParticle();
		break;

	default:
		break;
	}

	//ターゲット情報が存在する場合
	if (pParticle != nullptr)
	{
		//パラメータ設定
		pParticle->SetType(CBillboardEffect::TYPE::PARTICLE);//ビルボードタイプ
		pParticle->SetPos(pos);//座標
		pParticle->SetColor(col);//カラー
		pParticle->SetSize(size);//サイズ
		pParticle->m_MaxSize = size;//最大サイズ
		pParticle->m_nLife = nLife;//ライフ
		pParticle->m_nMaxLife = nLife;//最大ライフ
		pParticle->m_type = type;//パーティクルタイプ

		//移動量設定
		pParticle->SetMoveValue();

		//初期化処理
		pParticle->Init();
	}

	return pParticle;
}

