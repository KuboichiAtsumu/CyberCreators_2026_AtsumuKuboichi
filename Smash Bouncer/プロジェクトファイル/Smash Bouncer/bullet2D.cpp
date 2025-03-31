//==============================================================================================================================================
//
// 2Dバレットに関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "bullet2D.h"//2Dバレット
#include "trajectory2D.h"//軌跡(2D)
#include "titleframe.h"//タイトルフレーム

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CBullet2D::CBullet2D() : CObject2D()
{
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CBullet2D::~CBullet2D()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CBullet2D::Init()
{
	//パラメータ設定
	SetSize(BULLET2D_SIZE);//サイズ
	SetColor({ 0.0f,0.0f,1.0f,1.0f });

	//テクスチャ生成
	CTexture* pTex = CManager::GetTexture();//取得
	int nTextureIdx = pTex->Regist(TEXTURE_FILE);//登録
	BindTexture(pTex->GetAddress(nTextureIdx));//設定
	SetTextureIdx(nTextureIdx);//ID設定

	//2Dオブジェクト初期化処理
	if (FAILED(CObject2D::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CBullet2D::Uninit()
{
	//2Dオブジェクト終了処理
	CObject2D::Uninit();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CBullet2D::Update()
{
	//削除フラグがtrueの場合
	if (GetDeleteFlag())
	{
		//処理を抜ける
		return;
	}

	//ローカル変数宣言
	D3DXVECTOR2 pos = GetPos();//位置情報
	D3DXVECTOR2 move = GetMove();//移動量情報

	//移動量を加算
	pos += move;

	//X座標が範囲外の場合
	if (pos.x <= CTitleFrame::FRAME_SIZE.x || pos.x >= CProcess::SCREEN_WIDTH - CTitleFrame::FRAME_SIZE.x)
	{
		//X軸の移動方向を反転させる
		move.x *= -1.0f;
	}

	//Y座標が範囲外の場合
	if (pos.y <= CTitleFrame::FRAME_SIZE.y || pos.y >= CProcess::SCREEN_HEIGHT - CTitleFrame::FRAME_SIZE.y)
	{
		//Y軸の移動方向を反転させる
		move.y *= -1.0f;
	}

	//軌跡を生成
	CTrajectory2D::Create(pos, GetSize() * 0.5f, { 0.0f, 1.0f, 1.0f, 0.3f }, TEXTURE_FILE);

	//更新情報設定
	SetPos(pos);//位置
	CObject2D::SetMove(move);//移動量

	//2Dオブジェクト更新処理
	CObject2D::Update();
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CBullet2D::Draw()
{
	//ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();//デバイスの取得

	//αブレンディングを加算合成に設定
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	//2Dオブジェクト描画処理
	CObject2D::Draw();

	//αブレンディングを元に戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

//===========================================================================================================
// 生成処理
//===========================================================================================================
CBullet2D* CBullet2D::Create(D3DXVECTOR2 pos)
{
	CBullet2D* pBullet2D = NEW CBullet2D();

	pBullet2D->SetType(CObject::BULLET);//オブジェクトタイプ
	pBullet2D->SetPos(pos);//生成位置設定
	pBullet2D->SetMove();//移動量情報設定
	pBullet2D->Init();//初期化処理

	return pBullet2D;
}

//===========================================================================================================
// 移動量設定
//===========================================================================================================
void CBullet2D::SetMove()
{
	//パラメータ設定
	float fRandAngle = float((rand() % 629 + 1) - 314) / 100.0f; //角度
	float fMoveX = sinf(fRandAngle) * BULLET2D_SPEED;//X座標の移動量
	float fMoveY = cosf(fRandAngle) * BULLET2D_SPEED;//Y座標の移動量

	//移動量情報を設定
	CObject2D::SetMove({ fMoveX, fMoveY });
}

