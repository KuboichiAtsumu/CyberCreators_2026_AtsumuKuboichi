//==============================================================================================================================================
//
// ビルボードエフェクトに関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "billboardeffect.h"
#include "particle.h"
#include "target.h"
#include "smoke.h"
#include "minigameclear.h"
#include "minigamefailed.h"

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CBillboardEffect::CBillboardEffect(int nPriority) :	CBillboard{ nPriority },
	m_type(TYPE::NONE)
{
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CBillboardEffect::~CBillboardEffect()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CBillboardEffect::Init()
{
	//タイプを保存
	int nType = static_cast<int>(m_type);

	//テクスチャ生成
	CTexture* pTex = CTexture::GetInstance();//取得
	int nTexIdx = pTex->Regist(CTag::TAG::EFFECT, nType, TEXTURE_FILE[nType]);//登録
	BindTexture(pTex->GetAddress(nTexIdx));//設定

	//基底クラス初期化処理
	if (FAILED(CBillboard::Init())) return E_FAIL;

	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CBillboardEffect::Uninit()
{
	//基底クラス終了処理
	CBillboard::Uninit();
}

//===========================================================================================================
// 解放処理
//===========================================================================================================
void CBillboardEffect::Release()
{
	//基底クラス解放処理
	CBillboard::Release();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CBillboardEffect::Update()
{
	//基底クラス更新処理
	CBillboard::Update();
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CBillboardEffect::Draw()
{
	//基底クラス描画処理
	CBillboard::Draw();
}

//===========================================================================================================
// 生成処理
//===========================================================================================================
CBillboardEffect* CBillboardEffect::Create(TYPE type, const D3DXVECTOR3& pos, const D3DXVECTOR3& size)
{
	//メモリを動的確保
	CBillboardEffect* pBillboardEffect = nullptr;
	switch (type)
	{
		//パーティクル
	case TYPE::PARTICLE:
		pBillboardEffect = NEW CParticle();
		break;

		//ターゲット
	case TYPE::TARGET:
		pBillboardEffect = NEW CTarget();
		break;

		//煙
	case TYPE::SMOKE:
		pBillboardEffect = NEW CSmoke();
		break;

		//ミニゲームクリア
	case TYPE::MINIGAME_CLEAR:
		pBillboardEffect = NEW CMiniGameClear();
		break;

		//ミニゲーム失敗
	case TYPE::MINIGAME_FAILED:
		pBillboardEffect = NEW CMiniGameFailed();
		break;

	default:
		break;
	}

	//ターゲット情報が存在する場合
	if (pBillboardEffect != nullptr)
	{
		//パラメータ設定
		pBillboardEffect->SetPos(pos);//座標
		pBillboardEffect->SetSize(size);//サイズ
		pBillboardEffect->m_type = type;//タイプ
	}

	return pBillboardEffect;
}
