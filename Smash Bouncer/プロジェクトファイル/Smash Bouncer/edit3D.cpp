//==============================================================================================================================================
//
// 3Dエディタの処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifdef _DEBUG

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "edit3D.h"//エディタ
#include "debugproc.h"//デバッグ表示
#include "keyboard.h"//キーボード
#include "game.h"//ゲームシーン
#include "camera.h"//カメラ

//===========================================================================================================
// 静的メンバ変数初期化
//===========================================================================================================
CEdit3D* CEdit3D::m_apEditObj[MAX_EDIT3D] = {};//オブジェクト情報
CEdit3D::EDIT_MODE CEdit3D::m_mode = MODE_MOVE;//操作モード
int CEdit3D::m_nNumAll = 0;//合計データ数
int CEdit3D::m_nTarget = 0;//編集対象番号
bool CEdit3D::m_bTrigger = false;//移動方法がトリガーかどうか

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CEdit3D::CEdit3D(int nPriority) : CObjectX(nPriority)
{
	for (int nCnt = 0; nCnt < MAX_EDIT3D; nCnt++)
	{
		//データが存在しない場合
		if (m_apEditObj[nCnt] == nullptr)
		{
			//自分自身を代入
			m_apEditObj[nCnt] = this;

			//メンバー変数初期化
			m_apEditObj[nCnt]->m_pos = { 0.0f, 0.0f, 0.0f };//位置
			m_apEditObj[nCnt]->m_rot = { 0.0f, 0.0f, 0.0f };//向き
			m_apEditObj[nCnt]->m_Scale = { 1.0f, 1.0f, 1.0f };//拡大率
			m_apEditObj[nCnt]->m_apModel = {};//モデル情報
			m_apEditObj[nCnt]->m_bUse = true;//使用フラグ
			m_apEditObj[nCnt]->m_nID = nCnt;//自分自身のID
			m_apEditObj[nCnt]->m_nType = 0;//タイプ
			m_apEditObj[nCnt]->m_nCategory = 0;//カテゴリー

			//総数加算
			m_nNumAll++;

			//ループを抜ける
			break;
		}
	}

	//編集対象設定
	m_nTarget = m_apEditObj[m_nID]->m_nID;
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CEdit3D::~CEdit3D()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CEdit3D::Init()
{
	//Xファイルオブジェクト初期化処理
	if (FAILED(CObjectX::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CEdit3D::Uninit()
{
	//終了処理
	CObjectX::Uninit();
}

//===========================================================================================================
// 全オブジェクト終了処理
//===========================================================================================================
void CEdit3D::UninitAll()
{
	for (int nCnt = 0; nCnt < MAX_EDIT3D; nCnt++)
	{
		//データが存在する場合
		if (m_apEditObj[nCnt] != nullptr)
		{
			//データを解放
			m_apEditObj[nCnt]->Uninit();
			m_apEditObj[nCnt] = nullptr;
		}
	}

	//総数リセット
	m_nNumAll = 0;
}

//===========================================================================================================
// 解放処理
//===========================================================================================================
void CEdit3D::Release()
{
	//IDを保存
	int nID = m_nID;
	m_apEditObj[nID] = nullptr;
}

//===========================================================================================================
// エディタ情報更新処理
//===========================================================================================================
void CEdit3D::EditUpdate()
{
	//=======================================================================
	// 移動方法切り替え
	//=======================================================================
	if (CManager::GetKeyboard()->GetTrigger(DIK_1))
	{//1キーが押された場合
		m_bTrigger = m_bTrigger ? false : true;
	}

	//=======================================================================
	// 操作モード変更
	//=======================================================================

	//F5キーが押された場合
	if (CManager::GetKeyboard()->GetTrigger(DIK_F5))
	{
		//移動
		m_mode = MODE_MOVE;
	}
	//F6キーが押された場合
	else if (CManager::GetKeyboard()->GetTrigger(DIK_F6))
	{
		//拡大
		m_mode = MODE_SCALE;
	}

	//=======================================================================
	// 編集対象変更
	//=======================================================================

	//Nキーが押された場合
	if (CManager::GetKeyboard()->GetRepeat(DIK_N))
	{
		//左SHIFTキーも押されている場合
		if (CManager::GetKeyboard()->GetPress(DIK_LSHIFT))
		{
			//10個前のオブジェクトを対象にする
			m_nTarget -= 10;
		}
		else
		{
			//前のオブジェクトを対象にする
			m_nTarget--;
		}

		//カウントがオブジェクトの最大数に到達した場合
		if (m_nTarget < 0)
		{
			//一番最後にループする
			m_nTarget = m_nNumAll - 1;
		}

		//カメラの座標設定処理
		SetCameraPos();
	}
	//Mキーが押された場合
	else if (CManager::GetKeyboard()->GetRepeat(DIK_M))
	{
		//左SHIFTキーも押されている場合
		if (CManager::GetKeyboard()->GetPress(DIK_LSHIFT))
		{
			//10個後のオブジェクトを対象にする
			m_nTarget += 10;
		}
		else
		{
			//後のオブジェクトを対象にする
			m_nTarget++;
		}

		//カウントがオブジェクトの最大数に到達した場合
		if (m_nTarget > m_nNumAll - 1)
		{
			//一番最初にループする
			m_nTarget = 0;
		}

		//カメラの座標設定処理
		SetCameraPos();
	}

	//=======================================================================
	// 配置
	//=======================================================================
	if (CManager::GetKeyboard()->GetTrigger(DIK_B))
	{//Bキーが押された場合

		//次の配置オブジェクトを生成
		Create();

		//次のオブジェクトに前のオブジェクトの情報を反映
		m_apEditObj[m_nTarget]->m_pos = m_apEditObj[m_nTarget - 1]->m_pos;//位置
		m_apEditObj[m_nTarget]->m_rot = m_apEditObj[m_nTarget - 1]->m_rot;//角度
		m_apEditObj[m_nTarget]->m_Scale = m_apEditObj[m_nTarget - 1]->m_Scale;//拡大率
		m_apEditObj[m_nTarget]->m_nCategory = m_apEditObj[m_nTarget - 1]->m_nCategory;//カテゴリー
		m_apEditObj[m_nTarget]->m_nType = m_apEditObj[m_nTarget - 1]->m_nType;//タイプ

		//カメラの座標設定処理
		SetCameraPos();
	}

	//=======================================================================
	// 削除
	//=======================================================================
	if (CManager::GetKeyboard()->GetTrigger(DIK_DELETE))
	{//DELETEキーが押された場合

		//配置されていない状態にする
		m_apEditObj[m_nTarget]->m_bUse = false;

		//解放処理
		m_apEditObj[m_nTarget]->SetDeleteFlag(true);
		
		//対象を1つ前する
		m_nTarget--;

		//ターゲット番号が0以下にならなように調整
		if (m_nTarget < 0)
		{
			m_nTarget = 0;
		}

		//総数を減算
		m_nNumAll--;

		//カメラの座標設定処理
		SetCameraPos();
	}

	//=======================================================================
	// 削除された分のデータを詰める
	//=======================================================================
	for (int nCnt = 0; nCnt < m_nNumAll; nCnt++)
	{
		//前のデータが空いている場合
		if (m_apEditObj[nCnt] == nullptr && m_apEditObj[nCnt + 1] != nullptr)
		{
			//データを詰める
			m_apEditObj[nCnt] = m_apEditObj[nCnt + 1];

			//詰めた分のデータを解放
			m_apEditObj[nCnt + 1]->Release();

			//IDを修正する
			m_apEditObj[nCnt]->m_nID = nCnt;
		}
	}

	//=======================================================================
	// データの書き出し
	//=======================================================================
	if (CManager::GetKeyboard()->GetTrigger(DIK_F3))
	{//F3キーが押された場合

		//書き出し処理
		Save();
	}
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CEdit3D::Update()
{
	//エディタモードではない場合
	if (!CManager::GetScene()->GetEditMode())
	{//描画せずに処理を抜ける
		return;
	}

	//編集対象とIDが一致する場合
	if (m_nTarget == m_nID)
	{
		//=========================================================================================
		// 操作方法表示
		//=========================================================================================
		PrintDebugProc("==============================\n 操作( F5 : 移動　F6 : 拡大 )\n==============================\n");

		switch (m_mode)
		{
		case MODE_MOVE:
			PrintDebugProc("移動\nX軸移動:T,G　Y軸移動:Y,H　Z軸移動:U,J\n\n");
			break;
		case MODE_SCALE:
			PrintDebugProc("拡大\nX軸拡大:T,G　Y軸拡大:Y,H　Z軸拡大:U,J\n\n");
			break;
		default:
			break;
		}

		PrintDebugProc("回転\n左回転:Z　右回転:X\n\n");
		PrintDebugProc("位置と向きをリセット:R\n\n");
		PrintDebugProc("カメラ\n移動:WASD\n\n");
		PrintDebugProc("カテゴリー変更:左SHIFT + Q,E\n");
		PrintDebugProc("種類を変更:Q,E\n");
		PrintDebugProc("配置:B\n");
		PrintDebugProc("編集対象変更:N,M\n");
		PrintDebugProc("削除:DELETE\n");
		PrintDebugProc("全リセット:0\n");
		PrintDebugProc("書き出し:F3\n\n");

		//配置物データ表示
		PrintDebugProc("==============================\n 配置物データ \n==============================\n");

		//操作モードを表示
		PrintDebugProc("操作モード( 1で切り替え ) : ");
		if (!m_bTrigger)
		{//プレス
			PrintDebugProc("Press\n\n");
		}
		else
		{//トリガー
			PrintDebugProc("Trigger\n\n");
		}

		//操作モード別に拡大率を調整
		switch (m_mode)
		{
			//位置
		case MODE_MOVE:
			PrintDebugProc("POS\nX:%f　Y:%f　Z:%f\n\n", m_apEditObj[m_nTarget]->m_pos.x, m_apEditObj[m_nTarget]->m_pos.y, m_apEditObj[m_nTarget]->m_pos.z);
			break;

			//拡大率
		case MODE_SCALE:
			PrintDebugProc("SCALE\nX:%f　Y:%f　Z:%f\n\n", m_apEditObj[m_nTarget]->m_Scale.x, m_apEditObj[m_nTarget]->m_Scale.y, m_apEditObj[m_nTarget]->m_Scale.z);
			break;

		default:
			break;
		}

		PrintDebugProc("ROT\nX:%f　Y:%f  Z:%f\n\n", m_apEditObj[m_nTarget]->m_rot.x, m_apEditObj[m_nTarget]->m_rot.y, m_apEditObj[m_nTarget]->m_rot.z);//向き
		PrintDebugProc("CATEGORY:%d\n\n", m_apEditObj[m_nTarget]->m_nCategory);//カテゴリー
		PrintDebugProc("TYPE:%d\n\n", m_apEditObj[m_nTarget]->m_nType);//種類
		PrintDebugProc("配置数:%d/%d\n", m_apEditObj[m_nTarget]->m_nID + 1, m_nNumAll);//配置数

		//=========================================================================================
		// オブジェクトデータ更新
		//=========================================================================================

		//モデルデータを反映
		m_apEditObj[m_nTarget]->m_apModel = CXfile::GetSaveModelInfo(m_apEditObj[m_nTarget]->m_nCategory, m_apEditObj[m_nTarget]->m_nType);
		
		//操作モード別処理
		switch (m_mode)
		{
			//移動
		case MODE_MOVE:
			Move();
			break;

			//拡大
		case MODE_SCALE:
			Scale();
			break;

		default:
			break;
		}

		//=======================================================================
		// 回転
		//=======================================================================
		if (CManager::GetKeyboard()->GetTrigger(DIK_Z))
		{//Zキーが押された場合
			m_apEditObj[m_nTarget]->m_rot.y -= TURN;//右回転
		}
		else if (CManager::GetKeyboard()->GetTrigger(DIK_X))
		{//Xキーが押された場合
			m_apEditObj[m_nTarget]->m_rot.y += TURN;//左回転
		}

		//角度の補正
		if (m_apEditObj[m_nTarget]->m_rot.y <= -D3DX_PI)
		{
			m_apEditObj[m_nTarget]->m_rot.y = D3DX_PI;
		}
		else if (m_apEditObj[m_nTarget]->m_rot.y >= D3DX_PI)
		{
			m_apEditObj[m_nTarget]->m_rot.y = -D3DX_PI;
		}

		//=======================================================================
		// リセット
		//=======================================================================

		//Rキーが押された場合
		if (CManager::GetKeyboard()->GetTrigger(DIK_R))
		{
			//パラメータリセット
			m_apEditObj[m_nTarget]->m_pos = { 0.0f,0.0f,0.0f };//位置
			m_apEditObj[m_nTarget]->m_rot = { 0.0f,0.0f,0.0f };//角度
			m_apEditObj[m_nTarget]->m_Scale = { 1.0f,1.0f,1.0f };//拡大率

			//カメラの座標設定処理
			SetCameraPos();
		}

		//=======================================================================
		// カテゴリー・種類変更
		//=======================================================================
		if (CManager::GetKeyboard()->GetTrigger(DIK_Q))
		{//Qキーが押された場合
			if (CManager::GetKeyboard()->GetPress(DIK_LSHIFT))
			{
				//左SHIFTも押されている場合
				if (m_apEditObj[m_nTarget]->m_nCategory > 0)
				{
					//カテゴリー変更
					m_apEditObj[m_nTarget]->m_nCategory--;

					//種類を最初に戻す
					m_apEditObj[m_nTarget]->m_nType = 0;
				}
			}
			else
			{
				//左SHIFTが押されていない場合
				if (m_apEditObj[m_nTarget]->m_nType > 0)
				{
					//種類変更
					m_apEditObj[m_nTarget]->m_nType--;
				}
			}
		}
		else if (CManager::GetKeyboard()->GetTrigger(DIK_E))
		{//Eキーが押された場合
			if (CManager::GetKeyboard()->GetPress(DIK_LSHIFT))
			{//左SHIFTも押されている場合
				if (m_apEditObj[m_nTarget]->m_nCategory < CXfile::MAX_CATEGORY - 1)
				{
					//種類変更
					m_apEditObj[m_nTarget]->m_nCategory++;

					//種類を最初に戻す
					m_apEditObj[m_nTarget]->m_nType = 0;
				}
			}
			else
			{//左SHIFTが押されていない場合
				if (m_apEditObj[m_nTarget]->m_nType < (int)CObjectX::m_nNumType[m_apEditObj[m_nTarget]->m_nCategory] - 1)
				{//種類変更
					m_apEditObj[m_nTarget]->m_nType++;
				}
			}
		}
	}
}

//===========================================================================================================
// 移動処理
//===========================================================================================================
void CEdit3D::Move()
{
	//Press
	if (!m_bTrigger)
	{
		if (CManager::GetKeyboard()->GetPress(DIK_T))
		{//Tキーが押された場合
			m_apEditObj[m_nTarget]->m_pos.x -= MOVE;

			//カメラの座標設定処理
			SetCameraPos();
		}
		else if (CManager::GetKeyboard()->GetPress(DIK_G))
		{//Gキーが押された場合
			m_apEditObj[m_nTarget]->m_pos.x += MOVE;

			//カメラの座標設定処理
			SetCameraPos();
		}
		else if (CManager::GetKeyboard()->GetPress(DIK_Y))
		{//Yキーが押された場合
			m_apEditObj[m_nTarget]->m_pos.y += MOVE;

			//カメラの座標設定処理
			SetCameraPos();
		}
		else if (CManager::GetKeyboard()->GetPress(DIK_H))
		{//Hキーが押された場合
			m_apEditObj[m_nTarget]->m_pos.y -= MOVE;

			//カメラの座標設定処理
			SetCameraPos();
		}
		else if (CManager::GetKeyboard()->GetPress(DIK_U))
		{//Uキーが押された場合
			m_apEditObj[m_nTarget]->m_pos.z += MOVE;

			//カメラの座標設定処理
			SetCameraPos();
		}
		else if (CManager::GetKeyboard()->GetPress(DIK_J))
		{//Jキーが押された場合
			m_apEditObj[m_nTarget]->m_pos.z -= MOVE;

			//カメラの座標設定処理
			SetCameraPos();
		}
	}
	//Trigger
	else
	{
		if (CManager::GetKeyboard()->GetTrigger(DIK_T))
		{//Tキーが押された場合
			m_apEditObj[m_nTarget]->m_pos.x -= MOVE;

			//カメラの座標設定処理
			SetCameraPos();
		}
		else if (CManager::GetKeyboard()->GetTrigger(DIK_G))
		{//Gキーが押された場合
			m_apEditObj[m_nTarget]->m_pos.x += MOVE;

			//カメラの座標設定処理
			SetCameraPos();
		}
		else if (CManager::GetKeyboard()->GetTrigger(DIK_Y))
		{//Yキーが押された場合
			m_apEditObj[m_nTarget]->m_pos.y += MOVE;

			//カメラの座標設定処理
			SetCameraPos();
		}
		else if (CManager::GetKeyboard()->GetTrigger(DIK_H))
		{//Hキーが押された場合
			m_apEditObj[m_nTarget]->m_pos.y -= MOVE;

			//カメラの座標設定処理
			SetCameraPos();
		}
		else if (CManager::GetKeyboard()->GetTrigger(DIK_U))
		{//Uキーが押された場合
			m_apEditObj[m_nTarget]->m_pos.z += MOVE;

			//カメラの座標設定処理
			SetCameraPos();
		}
		else if (CManager::GetKeyboard()->GetTrigger(DIK_J))
		{//Jキーが押された場合
			m_apEditObj[m_nTarget]->m_pos.z -= MOVE;

			//カメラの座標設定処理
			SetCameraPos();
		}
	}
}

//===========================================================================================================
// 拡大処理
//===========================================================================================================
void CEdit3D::Scale()
{
	//Press
	if (!m_bTrigger)
	{
		if (CManager::GetKeyboard()->GetPress(DIK_T))
		{//Tキーが押された場合
			m_apEditObj[m_nTarget]->m_Scale.x -= SCALE;

			//カメラの座標設定処理
			SetCameraPos();
		}
		else if (CManager::GetKeyboard()->GetPress(DIK_G))
		{//Gキーが押された場合
			m_apEditObj[m_nTarget]->m_Scale.x += SCALE;

			//カメラの座標設定処理
			SetCameraPos();
		}
		else if (CManager::GetKeyboard()->GetPress(DIK_Y))
		{//Yキーが押された場合
			m_apEditObj[m_nTarget]->m_Scale.y += SCALE;

			//カメラの座標設定処理
			SetCameraPos();
		}
		else if (CManager::GetKeyboard()->GetPress(DIK_H))
		{//Hキーが押された場合
			m_apEditObj[m_nTarget]->m_Scale.y -= SCALE;

			//カメラの座標設定処理
			SetCameraPos();
		}
		else if (CManager::GetKeyboard()->GetPress(DIK_U))
		{//Uキーが押された場合
			m_apEditObj[m_nTarget]->m_Scale.z += SCALE;

			//カメラの座標設定処理
			SetCameraPos();
		}
		else if (CManager::GetKeyboard()->GetPress(DIK_J))
		{//Jキーが押された場合
			m_apEditObj[m_nTarget]->m_Scale.z -= SCALE;

			//カメラの座標設定処理
			SetCameraPos();
		}
	}
	//Trigger
	else
	{
		if (CManager::GetKeyboard()->GetTrigger(DIK_T))
		{//Tキーが押された場合
			m_apEditObj[m_nTarget]->m_Scale.x -= SCALE;

			//カメラの座標設定処理
			SetCameraPos();
		}
		else if (CManager::GetKeyboard()->GetTrigger(DIK_G))
		{//Gキーが押された場合
			m_apEditObj[m_nTarget]->m_Scale.x += SCALE;

			//カメラの座標設定処理
			SetCameraPos();
		}
		else if (CManager::GetKeyboard()->GetTrigger(DIK_Y))
		{//Yキーが押された場合
			m_apEditObj[m_nTarget]->m_Scale.y += SCALE;

			//カメラの座標設定処理
			SetCameraPos();
		}
		else if (CManager::GetKeyboard()->GetTrigger(DIK_H))
		{//Hキーが押された場合
			m_apEditObj[m_nTarget]->m_Scale.y -= SCALE;

			//カメラの座標設定処理
			SetCameraPos();
		}
		else if (CManager::GetKeyboard()->GetTrigger(DIK_U))
		{//Uキーが押された場合
			m_apEditObj[m_nTarget]->m_Scale.z += SCALE;

			//カメラの座標設定処理
			SetCameraPos();
		}
		else if (CManager::GetKeyboard()->GetTrigger(DIK_J))
		{//Jキーが押された場合
			m_apEditObj[m_nTarget]->m_Scale.z -= SCALE;

			//カメラの座標設定処理
			SetCameraPos();
		}
	}
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CEdit3D::Draw()
{
	//エディタモードではない場合
	if (!CManager::GetScene()->GetEditMode())
	{//描画せずに処理を抜ける
		return;
	}

	//ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();//デバイスのポインタ
	D3DXMATRIX mtxRot, mtxTrans, mtxScale;//計算用マトリックス
	D3DMATERIAL9 matDef;//現在のマテリアル保存用
	D3DXMATERIAL* pMat;//マテリアルのデータへのポインタ

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	//拡大率を反映
	D3DXMatrixScaling(&mtxScale, m_Scale.x, m_Scale.y, m_Scale.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxScale);

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	//法線の長さを1にする。
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	//マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)m_apModel->pBuffMat->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)m_apModel->dwNumMat; nCntMat++)
	{
		//編集対象のオブジェクトの場合
		if (m_nTarget == m_nID)
		{
			//モデルの色を変化
			pMat[nCntMat].MatD3D.Diffuse.a = 0.5f;
		}
		//編集対象のオブジェクトではない場合
		else
		{
			//モデルの色を元に戻す
			pMat[nCntMat].MatD3D.Diffuse = 
			{ 
				m_apModel->FirstDiffuse[nCntMat].r, 
				m_apModel->FirstDiffuse[nCntMat].g, 
				m_apModel->FirstDiffuse[nCntMat].b, 
				m_apModel->FirstDiffuse[nCntMat].a 
			};
		}

		//マテリアルの設定
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		//テクスチャの設定
		pDevice->SetTexture(0, m_apModel->m_pTexture);

		//モデル(パーツ)の描画
		m_apModel->pMesh->DrawSubset(nCntMat);
	}

	//保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}

//===========================================================================================================
// 生成処理
//===========================================================================================================
CEdit3D* CEdit3D::Create()
{
	CEdit3D* pEdit = NEW CEdit3D();

	//初期化処理
	pEdit->Init();
	
	return pEdit;
}

//===========================================================================================================
// セーブ処理
//===========================================================================================================
void CEdit3D::Save()
{
	//ローカル変数宣言
	const char* StageFile = {""};//ステージ情報ファイルパス

	//シーンに応じてファイルを変更
	switch (CManager::GetScene()->GetMode())
	{
		//チュートリアル
	case CScene::MODE_TUTORIAL:
		StageFile = CManager::TUTORIAL_STAGE;
		break;

		//ゲーム
	case CScene::MODE_GAME:
		StageFile = CManager::GAME_STAGE;
		break;

	default:
		break;
	}

	//ファイルを開く
	FILE* pFile = fopen(StageFile, "wb");

	//ファイルが見つかった場合
	if (pFile != nullptr)
	{
		//配置数を書き出す
		fwrite(&m_nNumAll, sizeof(int), 1, pFile);

		for (int nCnt = 0; nCnt < m_nNumAll; nCnt++)
		{
			//使用されている場合
			if (m_apEditObj[nCnt] != nullptr && m_apEditObj[nCnt]->m_bUse)
			{
				//データの記録
				fwrite(&m_apEditObj[nCnt]->m_pos, sizeof(D3DXVECTOR3), 1, pFile);//位置
				fwrite(&m_apEditObj[nCnt]->m_rot, sizeof(D3DXVECTOR3), 1, pFile);//向き
				fwrite(&m_apEditObj[nCnt]->m_Scale, sizeof(D3DXVECTOR3), 1, pFile);//拡大率
				fwrite(&m_apEditObj[nCnt]->m_nCategory, sizeof(int), 1, pFile);//カテゴリー
				fwrite(&m_apEditObj[nCnt]->m_nType, sizeof(int), 1, pFile);//種類
			}
		}

		//ファイルを閉じる
		fclose(pFile);
	}
}

//===========================================================================================================
// ロード処理
//===========================================================================================================
void CEdit3D::LoadInfo(const char* FileName)
{
	//ローカル変数宣言
	int nNumAll = 0;//総数

	//ファイルを開く
	FILE* pFile = fopen(FileName, "rb");

	//オブジェクトデータの読み込み
	if (pFile != nullptr)
	{
		//ファイルの読み込み
		fread(&nNumAll, sizeof(int), 1, pFile);

		for (int nCnt = 0; nCnt < nNumAll; nCnt++)
		{
			//データの取得
			Create();//生成処理
			fread(&m_apEditObj[nCnt]->m_pos, sizeof(D3DXVECTOR3), 1, pFile);//位置
			fread(&m_apEditObj[nCnt]->m_rot, sizeof(D3DXVECTOR3), 1, pFile);//向き
			fread(&m_apEditObj[nCnt]->m_Scale, sizeof(D3DXVECTOR3), 1, pFile);//拡大率
			fread(&m_apEditObj[nCnt]->m_nCategory, sizeof(int), 1, pFile);//カテゴリー
			fread(&m_apEditObj[nCnt]->m_nType, sizeof(int), 1, pFile);//種類

			//モデル情報を反映
			m_apEditObj[nCnt]->m_apModel = CXfile::GetSaveModelInfo(m_apEditObj[nCnt]->m_nCategory, m_apEditObj[nCnt]->m_nType);
		}

		//ファイルを閉じる
		fclose(pFile);
	}

	//オブジェクトデータが1つも存在しない場合
	if (nNumAll <= 0)
	{
		//オブジェクトを1つ生成
		Create();
	}
}

//===========================================================================================================
// カメラの座標設定
//===========================================================================================================
void CEdit3D::SetCameraPos()
{
	//位置情報を取得
	D3DXVECTOR3 pos = m_apEditObj[m_nTarget]->m_pos;//オブジェクト
	D3DXVECTOR3 Camerapos = CManager::GetCamera()->FirstPosV;//カメラの初期位置

	//カメラの視点と注視点をオブジェクト位置に合わせて設定
	CManager::GetCamera()->SetPosV(D3DXVECTOR3(pos.x, Camerapos.y, pos.z + Camerapos.z));//視点
	CManager::GetCamera()->SetPosR(m_apEditObj[m_nTarget]->m_pos);//注視点
}

#endif // _DEBUG