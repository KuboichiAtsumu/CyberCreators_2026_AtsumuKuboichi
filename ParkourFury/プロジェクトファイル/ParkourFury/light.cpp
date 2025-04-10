//==============================================================================================================================================
//
// ライトの処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "light.h"
#include "manager.h"
#include "renderer.h"

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CLight::CLight()
{
	//ライトの情報をクリアする
	ZeroMemory(&m_aLight, sizeof(D3DLIGHT9));
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CLight::~CLight()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CLight::Init()
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//デバイスのポインタ
	D3DXVECTOR3 vecDir[MAX_LIGHT];//設定用方向ベクトル

	//ライトの方向を設定
	vecDir[0] = D3DXVECTOR3(0.3f, 0.2f, -0.4f);
	vecDir[1] = D3DXVECTOR3(-0.5f, -0.3f, -1.0f);
	vecDir[2] = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	for (int nCntLight = 0; nCntLight < MAX_LIGHT; nCntLight++)
	{
		//ライトの種類を設定
		m_aLight[nCntLight].Type = D3DLIGHT_DIRECTIONAL;

		//ライトの拡散光を設定
		m_aLight[nCntLight].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		//ベクトルを正規化する
		D3DXVec3Normalize(&vecDir[nCntLight], &vecDir[nCntLight]);
		m_aLight[nCntLight].Direction = vecDir[nCntLight];

		//ライトを設定する
		pDevice->SetLight(nCntLight, &m_aLight[nCntLight]);

		//ライトを有効にする
		pDevice->LightEnable(nCntLight, TRUE);
	}

	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CLight::Uninit()
{
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CLight::Update()
{
}
