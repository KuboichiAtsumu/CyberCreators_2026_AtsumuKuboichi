//==============================================================================================================================================
//
// 3Dエディタの処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifdef _DEBUG

//===========================================================================================================
// ファイルインクルード
//===========================================================================================================
#include "edit3D.h"
#include "editmanager.h"

//===========================================================================================================
// 静的メンバ変数初期化
//===========================================================================================================
CEdit3D::MODE CEdit3D::m_mode = CEdit3D::MODE::MODE_MOVE;//操作モード
int CEdit3D::m_nAll = 0;//配置数
int CEdit3D::m_nTarget = 0;//編集対象番号
bool CEdit3D::m_bTrigger = false;//移動方法がトリガーかどうか

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CEdit3D::CEdit3D(int nPriority) : CObjectX(nPriority),
	m_pPrev(nullptr),
	m_pNext(nullptr),
	m_tag(CTag::TAG::BLOCK),
	m_nType(1)
{
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
	//メンバ変数初期化
	m_nIdx = m_nAll;//ID
	m_nAll++;//配置数を加算

	//編集対象設定
	m_nTarget = m_nIdx;

	//Xファイル管理のインスタンス取得
	CXfile* pXfile = CXfile::GetInstance();

	//モデル情報反映
	CXfile::ModelInfo* pModel = pXfile->GetAddress(m_tag, m_nType);
	if (pModel == nullptr)
	{
		pModel = pXfile->GetAddress(m_tag, 1);
	}
	BindModel(pModel);

	//テクスチャ管理のインスタンス取得
	CTexture* pTexManager = CTexture::GetInstance();

	//テクスチャ情報反映
	LPDIRECT3DTEXTURE9 pTexture = pTexManager->GetAddress(m_tag, m_nType);
	if (pTexture == nullptr)
	{
		pTexture = pTexManager->GetAddress(m_tag, 1);
	}
	BindTexture(pTexture);

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
	//Xファイルオブジェクト終了処理
	CObjectX::Uninit();

	//配置数を1つ減らす
	m_nAll--;
}

//===========================================================================================================
// 解放処理
//===========================================================================================================
void CEdit3D::Release()
{
	//Xファイルオブジェクト解放処理
	CObjectX::Release();
}

//===========================================================================================================
// エディタ情報更新処理
//===========================================================================================================
void CEdit3D::EditUpdate()
{
	//=======================================================================
	// 移動方法切り替え
	//=======================================================================
	if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_1))
	{//1キーが押された場合
		m_bTrigger = m_bTrigger ? false : true;
	}

	//=======================================================================
	// 操作モード変更
	//=======================================================================

	//F5キーが押された場合
	if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_F5))
	{
		//移動
		m_mode = CEdit3D::MODE::MODE_MOVE;
	}
	//F6キーが押された場合
	else if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_F6))
	{
		//拡大
		m_mode = CEdit3D::MODE::MODE_SCALE;
	}

	//=======================================================================
	// 配置
	//=======================================================================
	if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_B))
	{//Bキーが押された場合

		//現在選択しているエディタオブジェクト情報を取得
		CEdit3D* pEdit = CManager::GetInstance()->GetScene()->GetEditManager()->Find(m_nTarget);

		//取得した情報で新たに生成
		Create(*pEdit);
	}

	//=======================================================================
	// 編集対象変更
	//=======================================================================

	//Nキーが押された場合
	if (CManager::GetInstance()->GetKeyboard()->GetRepeat(DIK_N, 10))
	{
		//左SHIFTキーも押されている場合
		if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_LSHIFT))
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
			m_nTarget = m_nAll - 1;
		}
	}
	//Mキーが押された場合
	else if (CManager::GetInstance()->GetKeyboard()->GetRepeat(DIK_M, 10))
	{
		//左SHIFTキーも押されている場合
		if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_LSHIFT))
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
		if (m_nTarget > m_nAll - 1)
		{
			//一番最初にループする
			m_nTarget = 0;
		}
	}

	//=======================================================================
	// データの書き出し
	//=======================================================================
	if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_F3))
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
	//エディタモードの場合
	if (CManager::GetInstance()->GetScene()->GetSceneState() != CScene::SCENE::EDIT_3D)
	{//描画せずに処理を抜ける
		return;
	}

	//編集対象とIDが一致しない場合
	if (m_nTarget != m_nIdx)
	{
		//処理を抜ける
		return;
	}

	//=========================================================================================
	// 操作方法表示
	//=========================================================================================
	PrintDebugProc("==============================\n 操作( F5 : 移動　F6 : 拡大 )\n==============================\n");

	switch (m_mode)
	{
		//移動モードの場合
	case CEdit3D::MODE::MODE_MOVE:
		PrintDebugProc("移動\nX軸移動:T,G Y軸移動:Y,H Z軸移動:U,J\n\n");
		break;
		//拡縮モードの場合
	case CEdit3D::MODE::MODE_SCALE:
		PrintDebugProc("拡大\nX軸拡大:T,G Y軸拡大:Y,H Z軸拡大:U,J\n\n");
		break;
	default:
		break;
	}

	PrintDebugProc("回転\n左回転 : Z   右回転 : X\n\n");
	PrintDebugProc("位置と角度をリセット : R\n\n");
	PrintDebugProc("タグ変更 : 左SHIFT + Q,E\n");
	PrintDebugProc("タイプ変更 : Q,E\n");
	PrintDebugProc("編集対象変更 : N,M\n");
	PrintDebugProc("配置 : B\n");
	PrintDebugProc("削除 : DELETE\n");
	PrintDebugProc("書き出し : F3\n\n");

	//配置物データ表示
	PrintDebugProc("==============================\n 配置物データ\n==============================\n");

	//操作モードを表示
	PrintDebugProc("操作モード( 1で切り替え ) : ");
	if (!m_bTrigger)
	{
		//プレス
		PrintDebugProc("Press\n\n");
	}
	else
	{
		//トリガー
		PrintDebugProc("Trigger\n\n");
	}

	//操作モード別に情報を表示
	switch (m_mode)
	{
	//座標
	case CEdit3D::MODE::MODE_MOVE:
	{
		//座標情報を取得
		D3DXVECTOR3 pos = GetPos();
		PrintDebugProc("POS X:%f Y:%f Z:%f\n\n", pos.x, pos.y, pos.z);
		break;
	}
	//拡大率
	case CEdit3D::MODE::MODE_SCALE:
	{
		//拡大率情報を取得
		D3DXVECTOR3 scale = GetScale();
		PrintDebugProc("SCALE X:%f Y:%f Z:%f\n\n", scale.x, scale.y, scale.z);
		break;
	}
	default:
		break;
	}

	//角度情報を取得
	D3DXVECTOR3 rot = GetRot();
	PrintDebugProc("ROT X:%f Y:%f Z:%f\n\n", rot.x, rot.y, rot.z);//角度

	//ファイルパスを表示
	CXfile::Model aModel = CXfile::GetInstance()->GetModel(m_tag, m_nType);
	PrintDebugProc("FILE_NAME : %s\n\n", aModel.FileName.c_str());

	PrintDebugProc("TAG:%d\n", static_cast<int>(m_tag));//タグ
	PrintDebugProc("TYPE:%d\n", m_nType);//タイプ

	PrintDebugProc("\n配置数:%d/%d\n", m_nTarget + 1, m_nAll);//配置数

	//=========================================================================================
	// オブジェクトデータ更新
	//=========================================================================================

	//モデル情報反映
	CXfile::ModelInfo* pModel = CXfile::GetInstance()->GetAddress(m_tag, m_nType);
	if (pModel != nullptr)
	{
		BindModel(pModel);
	}

	//テクスチャ情報反映
	LPDIRECT3DTEXTURE9 pTexture = CTexture::GetInstance()->GetAddress(m_tag, m_nType);
	if (pTexture != nullptr)
	{
		BindTexture(pTexture);
	}

	//操作モード別処理
	switch (m_mode)
	{
		//移動
	case CEdit3D::MODE::MODE_MOVE:
		Move();
		break;

		//拡大
	case CEdit3D::MODE::MODE_SCALE:
		Scale();
		break;

	default:
		break;
	}

	//=======================================================================
	// 削除
	//=======================================================================
	if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_DELETE)
		&& m_nAll > 1)
	{//DELETEキーが押された場合

		//終了処理
		Uninit();

		//対象を1つ前する
		m_nTarget--;

		//ターゲット番号が0以下にならなように調整
		if (m_nTarget < 0)
		{
			m_nTarget = 0;
		}
	}

	//回転処理
	Turn();

	//リセット処理
	Reset();

	//タグ・種類変更
	Change();

	//カメラの座標設定処理
	SetCameraPos();
}

//===========================================================================================================
// 移動処理
//===========================================================================================================
void CEdit3D::Move()
{
	//現在の座標情報を取得
	D3DXVECTOR3 pos = GetPos();

	//Press
	if (!m_bTrigger)
	{
		//Tキーが押された場合
		if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_T))
		{
			//座標を更新
			pos.x -= MOVE;
		}
		//Gキーが押された場合
		else if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_G))
		{
			//座標を更新
			pos.x += MOVE;
		}
		//Yキーが押された場合
		else if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_Y))
		{
			//座標を更新
			pos.y += MOVE;
		}
		//Hキーが押された場合
		else if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_H))
		{
			//座標を更新
			pos.y -= MOVE;
		}
		//Uキーが押された場合
		else if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_U))
		{
			//座標を更新
			pos.z += MOVE;
		}
		//Jキーが押された場合
		else if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_J))
		{
			//座標を更新
			pos.z -= MOVE;
		}
	}
	//Trigger
	else
	{
		//Tキーが押された場合
		if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_T))
		{
			//座標を更新
			pos.x -= MOVE;
		}
		//Gキーが押された場合
		else if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_G))
		{
			//座標を更新
			pos.x += MOVE;
		}
		//Yキーが押された場合
		else if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_Y))
		{
			//座標を更新
			pos.y += MOVE;
		}
		//Hキーが押された場合
		else if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_H))
		{
			//座標を更新
			pos.y -= MOVE;
		}
		//Uキーが押された場合
		else if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_U))
		{
			//座標を更新
			pos.z += MOVE;
		}
		//Jキーが押された場合
		else if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_J))
		{
			//座標を更新
			pos.z -= MOVE;
		}
	}

	//更新された座標情報を設定
	SetPos(pos);
}

//===========================================================================================================
// 拡大処理
//===========================================================================================================
void CEdit3D::Scale()
{
	//現在の拡大率情報を取得
	D3DXVECTOR3 Scale = GetScale();

	//Press
	if (!m_bTrigger)
	{
		//Tキーが押された場合
		if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_T))
		{
			//拡大率を更新
			Scale.x -= SCALE;
		}
		//Gキーが押された場合
		else if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_G))
		{
			//拡大率を更新
			Scale.x += SCALE;
		}
		//Yキーが押された場合
		else if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_Y))
		{
			//拡大率を更新
			Scale.y += SCALE;
		}
		//Hキーが押された場合
		else if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_H))
		{
			//拡大率を更新
			Scale.y -= SCALE;
		}
		//Uキーが押された場合
		else if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_U))
		{
			//拡大率を更新
			Scale.z += SCALE;
		}
		//Jキーが押された場合
		else if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_J))
		{
			//拡大率を更新
			Scale.z -= SCALE;
		}
	}
	//Trigger
	else
	{
		//Tキーが押された場合
		if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_T))
		{
			//拡大率を更新
			Scale.x -= SCALE;
		}
		//Gキーが押された場合
		else if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_G))
		{
			//拡大率を更新
			Scale.x += SCALE;
		}
		//Yキーが押された場合
		else if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_Y))
		{
			//拡大率を更新
			Scale.y += SCALE;
		}
		//Hキーが押された場合
		else if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_H))
		{
			//拡大率を更新
			Scale.y -= SCALE;
		}
		//Uキーが押された場合
		else if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_U))
		{
			//拡大率を更新
			Scale.z += SCALE;
		}
		//Jキーが押された場合
		else if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_J))
		{
			//拡大率を更新
			Scale.z -= SCALE;
		}
	}

	//更新された拡大率情報を設定
	SetScale(Scale);
}

//===========================================================================================================
// 回転処理
//===========================================================================================================
void CEdit3D::Turn()
{
	//現在の角度情報を取得
	D3DXVECTOR3 rot = GetRot();

	//Zキーが押された場合
	if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_Z))
	{
		//右回転
		rot.y -= TURN;
	}
	//Xキーが押された場合
	else if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_X))
	{
		//左回転
		rot.y += TURN;
	}

	//角度補正
	CorrectionRot(rot.y);

	//更新された角度情報を設定
	SetRot(rot);
}

//===========================================================================================================
// リセット処理
//===========================================================================================================
void CEdit3D::Reset()
{
	//Rキーが押された場合
	if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_R))
	{
		//パラメータリセット
		SetPos({ 0.0f, 0.0f, 0.0f });//座標
		SetRot({ 0.0f, 0.0f, 0.0f });//角度
		SetScale({ 1.0f, 1.0f, 1.0f });//拡大率
	}
}

//===========================================================================================================
// タグ・タイプ変更処理
//===========================================================================================================
void CEdit3D::Change()
{
	//現在のタグ情報を保存
	int nTag = static_cast<int>(m_tag);

	//Qキーが押された場合
	if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_Q))
	{
		//左SHIFTも押されている場合
		if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_LSHIFT) && 
			nTag > 1)
		{
			while (1)
			{
				//種類変更
				nTag--;

				//キャラクタパーツとテクスチャのみのタグを飛ばす
				if (static_cast<CTag::TAG>(nTag) != CTag::TAG::BOSS &&
					static_cast<CTag::TAG>(nTag) != CTag::TAG::BG &&
					static_cast<CTag::TAG>(nTag) != CTag::TAG::EFFECT &&
					static_cast<CTag::TAG>(nTag) != CTag::TAG::UI)
				{
					break;
				}
			}

			//タイプを初期化する
			m_nType = 1;
		}
		//左SHIFTが押されていない場合
		else
		{
			if (m_nType > 1)
			{
				//タイプ変更
				m_nType--;
			}
		}
	}
	//Eキーが押された場合
	else if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_E))
	{
		//左SHIFTも押されている場合
		if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_LSHIFT) &&
			nTag < static_cast<int>(CTag::TAG::MAX) - 1)
		{
			while (1)
			{
				//種類変更
				nTag++;

				//キャラクタパーツとテクスチャのみのタグを飛ばす
				if (static_cast<CTag::TAG>(nTag) != CTag::TAG::BOSS &&
					static_cast<CTag::TAG>(nTag) != CTag::TAG::BG &&
					static_cast<CTag::TAG>(nTag) != CTag::TAG::EFFECT &&
					static_cast<CTag::TAG>(nTag) != CTag::TAG::UI)
				{
					break;
				}
			}

			//タイプを初期化する
			m_nType = 1;
		}
		//左SHIFTが押されていない場合
		else
		{
			if (m_nType < CManager::GetInstance()->GetTag()->GetNumType(m_tag) - 1)
			{
				//タイプ変更
				m_nType++;
			}
		}
	}

	//更新されたタグ情報を設定
	m_tag = static_cast<CTag::TAG>(nTag);
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CEdit3D::Draw()
{
	//エディタモードではない場合
	if (CManager::GetInstance()->GetScene()->GetSceneState() != CScene::SCENE::EDIT_3D)
	{//描画せずに処理を抜ける
		return;
	}

	//ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//デバイスのポインタ
	D3DXMATRIX mtxRot, mtxTrans, mtxScale;//計算用マトリックス
	D3DMATERIAL9 matDef;//現在のマテリアル保存用
	D3DXMATERIAL* pMat;//マテリアルのデータへのポインタ
	D3DXMATRIX mtxWorld;//マトリックス情報
	D3DXVECTOR3 Scale = GetScale();//拡大率情報
	D3DXVECTOR3 rot = GetRot();//角度情報
	D3DXVECTOR3 pos = GetPos();//座標情報
	CXfile::ModelInfo aModel = GetModelInfo();//モデル情報

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);

	//拡大率を反映
	D3DXMatrixScaling(&mtxScale, Scale.x, Scale.y, Scale.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxScale);

	//角度を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

	//法線の長さを1にする。
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &mtxWorld);

	//現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	//マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)aModel.pBuffMat->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < static_cast<int>(aModel.dwNumMat); nCntMat++)
	{
		//編集対象のオブジェクトの場合
		if (m_nTarget == m_nIdx)
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
				aModel.FirstDiffuse.at(nCntMat).r,
				aModel.FirstDiffuse.at(nCntMat).g,
				aModel.FirstDiffuse.at(nCntMat).b,
				aModel.FirstDiffuse.at(nCntMat).a
			};
		}

		//マテリアルの設定
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		//テクスチャの設定
		pDevice->SetTexture(0, GetTexture());

		//モデル(パーツ)の描画
		aModel.pMesh->DrawSubset(nCntMat);
	}

	//保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}

//===========================================================================================================
// 生成処理
//===========================================================================================================
CEdit3D* CEdit3D::Create(CEdit3D aEdit)
{
	//メモリを動的確保
	CEdit3D* pEdit = NEW CEdit3D(static_cast<int>(Category::EDIT));

	//パラメータ設定
	pEdit->SetPos(aEdit.GetPos());//座標
	pEdit->SetRot(aEdit.GetRot());//角度
	pEdit->SetScale(aEdit.GetScale());//拡大率
	pEdit->m_tag = aEdit.m_tag;//タグ
	pEdit->m_nType = aEdit.m_nType;//タイプ

	//初期化処理
	pEdit->Init();

	//マネージャー登録処理
	CManager::GetInstance()->GetScene()->GetEditManager()->Regist(pEdit);

	return pEdit;
}

//===========================================================================================================
// セーブ処理
//===========================================================================================================
void CEdit3D::Save()
{
	//ファイルを開く
	FILE* pFile = fopen(CManager::GetInstance()->StageFile[static_cast<int>(CManager::GetInstance()->GetScene()->GetMode())], "wb");
	
	//ファイルが見つかった場合
	if (pFile != nullptr)
	{
		//配置数を書き出す
		fwrite(&m_nAll, sizeof(int), 1, pFile);

		//先頭のエディタオブジェクト情報を取得
		CObject* pObj = GetTop(static_cast<int>(Category::EDIT));
		CEdit3D* pEdit3D = dynamic_cast<CEdit3D*>(pObj);

		//オブジェクト情報がnullptrになるまで繰り返す
		while (pEdit3D != nullptr)
		{
			//次のオブジェクト情報を取得
			CEdit3D* pNext = pEdit3D->GetNextEdit();

			//情報を保存
			D3DXVECTOR3 pos = pEdit3D->GetPos();//座標
			D3DXVECTOR3 rot = pEdit3D->GetRot();//角度
			D3DXVECTOR3 scale = pEdit3D->GetScale();//拡大率
			int nTag = static_cast<int>(pEdit3D->m_tag);//タグ
			
			//データの記録
			fwrite(&pos, sizeof(D3DXVECTOR3), 1, pFile);//座標
			fwrite(&rot, sizeof(D3DXVECTOR3), 1, pFile);//角度
			fwrite(&scale, sizeof(D3DXVECTOR3), 1, pFile);//拡大率
			fwrite(&nTag, sizeof(int), 1, pFile);//タグ
			fwrite(&pEdit3D->m_nType, sizeof(int), 1, pFile);//種類

			//オブジェクト情報を次の情報に変更する
			pEdit3D = pNext;
		}

		//ファイルを閉じる
		fclose(pFile);
	}
}

//===========================================================================================================
// カメラの座標設定
//===========================================================================================================
void CEdit3D::SetCameraPos()
{
	//位置情報を取得
	D3DXVECTOR3 pos = GetPos();//オブジェクト

	//カメラの視点と注視点をオブジェクト位置に合わせて設定
	CCamera* pCamera = CManager::GetInstance()->GetCamera();
	pCamera->SetPosR(pos);//注視点
}

#endif // _DEBUG
