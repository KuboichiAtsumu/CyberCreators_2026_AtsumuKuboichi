//==============================================================================================================================================
//
// モーションに関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "motion.h"

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CMotionCharacter::CMotionCharacter(int nPriority) : CObjectX{ nPriority },
	m_CenterPos( 0.0f, 0.0f, 0.0f ),
	m_nState(0),
	m_fSpeed(1.0f),
	m_fJump(1.0f),
	m_bLanding(true),
	m_bMotion(true)
{
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CMotionCharacter::~CMotionCharacter()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CMotionCharacter::Init()
{
	//パーツファイル読込処理
	LoadParts();
	SetAllPartsSize();//サイズ設定

	//モーション情報読込処理
	LoadMotion();

	//基底クラス初期化処理
	if (FAILED(CObjectX::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===========================================================================================================
// 解放処理
//===========================================================================================================
void CMotionCharacter::Release()
{
	//パーツ情報をクリア
	m_aParts.clear();

	//モーション数を保存
	int nMotion = m_aMotion.size();

	//モーション情報を破棄
	for (int nCntMotion = 0; nCntMotion < nMotion; nCntMotion++)
	{
		//キーセット数を保存
		int nKeySet = m_aMotion.back().aKeyset.size();

		//キーセット情報を破棄
		for (int nCntKeySet = 0; nCntKeySet < nKeySet; nCntKeySet++)
		{
			//キー情報をクリア
			m_aMotion.back().aKeyset.back().aKey.clear();
		}

		//キーセット情報をクリア
		m_aMotion.back().aKeyset.clear();
	}

	//モーション情報をクリア
	m_aMotion.clear();

	//Xファイルオブジェクト解放処理
	CObjectX::Release();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CMotionCharacter::Update()
{
	//現在の座標を取得
	D3DXVECTOR3 pos = GetPos();

	//中心座標を更新
	m_CenterPos = { pos.x, pos.y + GetSize().y * 0.5f, pos.z };

	//Xファイルオブジェクト更新処理
	CObjectX::Update();

	//座標の補正
	CorrectionPos();
}

//===========================================================================================================
// 座標の補正処理
//===========================================================================================================
void CMotionCharacter::CorrectionPos()
{
	//座標情報を取得
	D3DXVECTOR3 pos = GetPos();

	//カメラの向きを取得
	float fCameraRotY = CManager::GetInstance()->GetCamera()->GetRot().y;

	//カメラの向きに合わせて座標を修正
	switch (GetStandLine())
	{
		//XY上
	case CMotionCharacter::STAND_LINE::XY:
		if (fCameraRotY == 0.0f)
		{
			pos.z = CManager::VTXMIN_FIELD.z;
		}
		else if (fCameraRotY == D3DX_PI || fCameraRotY == -D3DX_PI)
		{
			pos.z = CManager::VTXMAX_FIELD.z;
		}
		break;

		//YZ上
	case CMotionCharacter::STAND_LINE::YZ:
		if (fCameraRotY == D3DX_PI * 0.5f)
		{
			pos.x = CManager::VTXMIN_FIELD.x;
		}
		else if (fCameraRotY == -D3DX_PI * 0.5f)
		{
			pos.x = CManager::VTXMAX_FIELD.x;
		}
		break;

	default:
		break;
	}

	//フィールド外に出ないように調整
	if (pos.x > CManager::VTXMAX_FIELD.x)
	{
		pos.x = CManager::VTXMAX_FIELD.x;
	}
	else if(pos.x < CManager::VTXMIN_FIELD.x)
	{
		pos.x = CManager::VTXMIN_FIELD.x;
	}
	if (pos.z > CManager::VTXMAX_FIELD.z)
	{
		pos.z = CManager::VTXMAX_FIELD.z;
	}
	else if (pos.z < CManager::VTXMIN_FIELD.z)
	{
		pos.z = CManager::VTXMIN_FIELD.z;
	}

	//チュートリアルの場合
	if (CManager::GetInstance()->GetScene()->GetMode() == CScene::MODE::MODE_TUTORIAL)
	{
		//Z座標を0に固定
		pos.z = 0.0f;
	}

	//更新された座標を設定
	SetPos(pos);
}

//===========================================================================================================
// モーション状態設定処理
//===========================================================================================================
void CMotionCharacter::SetMotionState(int nState)
{
	//現在と違う状態の場合
	if (m_nState != nState)
	{
		//カウントをリセット
		m_aMotion.at(nState).nCntKeySet = 0;//キーセット
		m_aMotion.at(nState).nCntFrame = 0;//フレーム

		//モーションの判定をtrueにする
		m_bMotion = true;

		//現在の状態を保存
		m_nState = nState;
	}

	//モーションの判定がfalseの場合
	if (!m_bMotion)
	{
		//処理を抜ける
		return;
	}

	//フレーム数カウント
	m_aMotion.at(m_nState).nCntFrame++;

	//カウントが指定のフレーム数になった場合
	if (m_aMotion.at(m_nState).nCntFrame > m_aMotion.at(m_nState).aKeyset.at(m_aMotion.at(m_nState).nCntKeySet).nMaxFrame)
	{
		//フレーム数カウントを初期化
		m_aMotion.at(m_nState).nCntFrame = 0;

		//キーセット数を加算
		m_aMotion.at(m_nState).nCntKeySet++;

		//キー数が最大数になった場合
		if (m_aMotion.at(m_nState).nCntKeySet >= m_aMotion.at(m_nState).nMaxKeySet + 1 && 
			static_cast<bool>(m_aMotion.at(m_nState).nLoop))
		{//ループする場合

			//キーセット数を初期化
			m_aMotion.at(m_nState).nCntKeySet = 0;
		}
		else if (m_aMotion.at(m_nState).nCntKeySet >= m_aMotion.at(m_nState).nMaxKeySet && 
			!static_cast<bool>(m_aMotion.at(m_nState).nLoop))
		{//ループしない場合

			//モーションをしないように設定
			m_bMotion = false;

			m_aMotion.at(m_nState).nCntKeySet = m_aMotion.at(m_nState).nMaxKeySet;
		}
	}
}

//===========================================================================================================
// パーツごとの状態設定処理
//===========================================================================================================
void CMotionCharacter::SetPartsState()
{
	//パーツ数を保存
	int nNumParts = m_aParts.size();

	for (int nCntParts = 0; nCntParts < nNumParts; nCntParts++)
	{
		//モーション処理
		Motion(m_nState, nCntParts);
	}
}

//===========================================================================================================
// モーション処理
//===========================================================================================================
void CMotionCharacter::Motion(int nState, int nParts)
{
	//ローカル変数宣言
	int nKeySet = m_aMotion.at(nState).nCntKeySet;//現在のキーセットカウンタ
	int nNextKeySet = (nKeySet + 1) % (m_aMotion.at(nState).nMaxKeySet + 1);//次のキー
	float fFrame = static_cast<float>(m_aMotion.at(nState).nCntFrame) / static_cast<float>(m_aMotion.at(nState).aKeyset.at(nKeySet).nMaxFrame);//現在のフレームと全体フレームの割合

	//============================
	// 次のキーまでの差分を求める
	//============================

	//角度
	D3DXVECTOR3 Rot = m_aMotion.at(nState).aKeyset.at(nNextKeySet).aKey.at(nParts).rot - m_aParts.at(nParts).rot;

	//角度補正
	CorrectionRot(Rot.x);//X軸
	CorrectionRot(Rot.y);//Y軸
	CorrectionRot(Rot.z);//Z軸

	//============================
	// 次のキーを反映
	//============================

	//角度を更新
	m_aParts.at(nParts).rot += Rot * fFrame;

	//角度補正
	CorrectionRot(m_aParts.at(nParts).rot.x);//X軸
	CorrectionRot(m_aParts.at(nParts).rot.y);//Y軸
	CorrectionRot(m_aParts.at(nParts).rot.z);//Z軸
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CMotionCharacter::Draw()
{
	//ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//デバイスのポインタ
	D3DXMATRIX mtxRot, mtxTrans, mtxParent;//計算用マトリックス
	D3DMATERIAL9 matDef;//現在のマテリアル保存用
	D3DXMATERIAL* pMat;//マテリアルのデータへのポインタ
	D3DXMATRIX mtxWorld;//ワールドマトリックス情報
	D3DXVECTOR3 pos = GetPos();//座標
	D3DXVECTOR3 rot = GetRot();//角度

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);

	//角度を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	//座標を反映
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

	//パーツ数を取得
	int nNumParts = static_cast<int>(m_aParts.size());

	for (int nCntParts = 0; nCntParts < nNumParts; nCntParts++)
	{
		////アウトライン描画処理
		//DrawOutLine(nCntParts, mtxWorld);

		//各パーツモデルのワールドマトリックスの初期化
		D3DXMatrixIdentity(&m_aParts.at(nCntParts).mtxWorld);

		//各パーツモデルの角度を反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, m_aParts.at(nCntParts).rot.y, m_aParts.at(nCntParts).rot.x, m_aParts.at(nCntParts).rot.z);
		D3DXMatrixMultiply(&m_aParts.at(nCntParts).mtxWorld, &m_aParts.at(nCntParts).mtxWorld, &mtxRot);

		//各パーツモデルの座標を反映
		D3DXMatrixTranslation(&mtxTrans, m_aParts.at(nCntParts).pos.x, m_aParts.at(nCntParts).pos.y, m_aParts.at(nCntParts).pos.z);
		D3DXMatrixMultiply(&m_aParts.at(nCntParts).mtxWorld, &m_aParts.at(nCntParts).mtxWorld, &mtxTrans);

		//親の番号を保存
		int nParent = m_aParts.at(nCntParts).nParent;

		//親のマトリックスを反映
		if (nParent != -1)
		{//親の番号を持つ場合
			mtxParent = m_aParts.at(nParent).mtxWorld;//親モデルのマトリックス
		}
		else
		{//親の番号を持たない場合
			mtxParent = mtxWorld;//キャラクター自身のマトリックス
		}

		//親のマトリックスを掛け合わせる
		D3DXMatrixMultiply(&m_aParts.at(nCntParts).mtxWorld, &m_aParts.at(nCntParts).mtxWorld, &mtxParent);

		//ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &m_aParts.at(nCntParts).mtxWorld);

		//ワールド座標を保存
		D3DXVECTOR3 WorldPos = { m_aParts.at(nCntParts).mtxWorld._41, m_aParts.at(nCntParts).mtxWorld._42, m_aParts.at(nCntParts).mtxWorld._43 };
		SetWorldPos(nCntParts, WorldPos);

		//現在のマテリアルを取得
		pDevice->GetMaterial(&matDef);

		//マテリアルデータへのポインタを取得
		pMat = (D3DXMATERIAL*)m_aParts.at(nCntParts).m_aModelInfo.pBuffMat->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < static_cast<int>(m_aParts.at(nCntParts).m_aModelInfo.dwNumMat); nCntMat++)
		{
			//マテリアル情報を反映
			pMat[nCntMat].MatD3D.Diffuse = m_aParts.at(nCntParts).m_aModelInfo.Diffuse.at(nCntMat);

			//マテリアルの設定
			pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

			//テクスチャの設定
			pDevice->SetTexture(0, nullptr);

			//モデル(パーツ)の描画
			m_aParts.at(nCntParts).m_aModelInfo.pMesh->DrawSubset(nCntMat);
		}

		//保存していたマテリアルを戻す
		pDevice->SetMaterial(&matDef);
	}
}

//===========================================================================================================
// アウトライン描画処理
//===========================================================================================================
void CMotionCharacter::DrawOutLine(int nParts, D3DXMATRIX mtxWorld)
{
	//ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//デバイスのポインタ
	D3DXMATRIX mtxRot, mtxTrans, mtxParent;//計算用マトリックス
	
	//各パーツモデルのワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_aParts.at(nParts).mtxWorld);

	//各パーツモデルの拡大率を反映
	D3DXMatrixScaling(&m_aParts.at(nParts).mtxWorld, OUTLINE_SCALE, OUTLINE_SCALE, OUTLINE_SCALE);//元のパーツより大きく設定
	pDevice->SetTransform(D3DTS_WORLD, &m_aParts.at(nParts).mtxWorld);

	//各パーツモデルの角度を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_aParts.at(nParts).rot.y, m_aParts.at(nParts).rot.x, m_aParts.at(nParts).rot.z);
	D3DXMatrixMultiply(&m_aParts.at(nParts).mtxWorld, &m_aParts.at(nParts).mtxWorld, &mtxRot);

	//各パーツモデルの座標を反映
	D3DXMatrixTranslation(&mtxTrans, m_aParts.at(nParts).pos.x, m_aParts.at(nParts).pos.y, m_aParts.at(nParts).pos.z);
	D3DXMatrixMultiply(&m_aParts.at(nParts).mtxWorld, &m_aParts.at(nParts).mtxWorld, &mtxTrans);

	//親の番号を保存
	int nParent = m_aParts.at(nParts).nParent;

	//親のマトリックスを反映
	if (nParent != -1)
	{//親の番号を持つ場合
		mtxParent = m_aParts.at(nParent).mtxWorld;//親モデルのマトリックス
	}
	else
	{//親の番号を持たない場合
		mtxParent = mtxWorld;//キャラクター自身のマトリックス
	}

	//親のマトリックスを掛け合わせる
	D3DXMatrixMultiply(&m_aParts.at(nParts).mtxWorld, &m_aParts.at(nParts).mtxWorld, &mtxParent);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_aParts.at(nParts).mtxWorld);

	//アウトライン描画設定
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);//裏面だけ描画
	pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);//Zバッファ有効化

	//アウトライン色の設定
	D3DMATERIAL9 OutlineMaterial = {};
	OutlineMaterial.Diffuse.r = 1.0f;
	OutlineMaterial.Diffuse.g = 1.0f;
	OutlineMaterial.Diffuse.b = 1.0f;
	OutlineMaterial.Diffuse.a = 1.0f;
	pDevice->SetMaterial(&OutlineMaterial);

	//アウトライン描画
	for (int nCnt = 0; nCnt < static_cast<int>(m_aParts.at(nParts).m_aModelInfo.dwNumMat); nCnt++)
	{
		m_aParts.at(nParts).m_aModelInfo.pMesh->DrawSubset(nCnt);
	}

	//描画状態を元に戻す
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//===========================================================================================================
// パーツファイル読込処理
//===========================================================================================================
void CMotionCharacter::LoadParts()
{
	//ローカル変数宣言
	std::string str;//ファイル検索用
	std::stringstream ss;//文字列保存用
	ModelParts aParts;//パーツ情報保存用

	//ファイルを開く
	std::ifstream file(PARTS_FILE[static_cast<int>(m_Type)]);

	//ファイルが開けない場合
	if (!file)
	{
		assert(false);
		return;
	}

	//ENDが見つかるまで読み込みを繰り返す
	while (1)
	{
		//検索
		std::getline(file, str);

		//END_PARTSが見つかった場合
		if (str.compare("END_PARTS") == 0)
		{
			//ファイルを閉じる
			file.close();

			break;
		}
		//MOVEが見つかった場合
		else if (str.find("MOVE") != std::string::npos)
		{
			//読み込んだ文字列を保存
			ss = static_cast<std::stringstream>(str);

			//移動量を読み込む
			ss >> str >> m_fSpeed;
		}
		//JUMPが見つかった場合
		else if (str.find("JUMP") != std::string::npos)
		{
			//読み込んだ文字列を保存
			ss = static_cast<std::stringstream>(str);

			//移動量を読み込む
			ss >> str >> m_fJump;
		}
		//PARTSSETが見つかった場合
		else if (str.find("PARTSSET") != std::string::npos)
		{
			//各パーツのデータを検索
			while (1)
			{
				//検索
				std::getline(file, str);

				//END_PARTSSETが見つかった場合
				if (str.find("END_PARTSSET") != std::string::npos)
				{
					//パーツ情報を追加
					m_aParts.push_back(aParts);

					//パーツのデータ検索を終了
					break;
				}
				//INDEXが見つかった場合
				else if (str.find("INDEX") != std::string::npos)
				{
					//読み込んだ文字列を保存
					ss = static_cast<std::stringstream>(str);

					//インデックスを読み込む
					ss >> str >> aParts.nIndex;
				}
				//PATHが見つかった場合
				else if (str.find("PATH") != std::string::npos)
				{
					//読み込んだ文字列を保存
					ss = static_cast<std::stringstream>(str);

					//パーツのファイル名を反映
					ss >> str >> aParts.aPartsFile;
				}
				//PARENTが見つかった場合
				else if (str.find("PARENT") != std::string::npos)
				{
					//読み込んだ文字列を保存
					ss = static_cast<std::stringstream>(str);

					//親のインデックスを読み込む
					ss >> str >> aParts.nParent;
				}
				//HALFが見つかった場合
				else if (str.find("HALF") != std::string::npos)
				{
					//読み込んだ文字列を保存
					ss = static_cast<std::stringstream>(str);

					//上半身か下半身かの情報を読み込む
					int nHalf;
					ss >> str >> nHalf;

					//読み込んだ数字を反映
					aParts.Half = static_cast<HALF_BODY>(nHalf);
				}
				//POSが見つかった場合
				else if (str.find("POS") != std::string::npos)
				{
					//読み込んだ文字列を保存
					ss = static_cast<std::stringstream>(str);

					//座標を読み込む
					ss >> str >> aParts.pos.x >> aParts.pos.y >> aParts.pos.z;

					//初期位置保存
					aParts.FirstPos = aParts.pos;
				}
				//ROTが見つかった場合
				else if (str.find("ROT") != std::string::npos)
				{
					//読み込んだ文字列を保存
					ss = static_cast<std::stringstream>(str);

					//角度を読み込む
					ss >> str >> aParts.rot.x >> aParts.rot.y >> aParts.rot.z;
				}
				//上記以外が見つかった場合
				else
				{
					continue;
				}
			}
		}
		else
		{
			continue;
		}
	}

	//タイプごとにタグを設定
	CTag::TAG tag = {};
	switch (m_Type)
	{
		//プレイヤー
	case CMotionCharacter::TYPE::PLAYER:
		tag = CTag::TAG::PLAYER;
		break;

		//ボス
	case CMotionCharacter::TYPE::BOSS:
		tag = CTag::TAG::BOSS;
		break;

	default:
		break;
	}

	//Xファイル管理のインスタンスを取得
	CXfile* pModel = CXfile::GetInstance();

	//パーツ数を保存
	int nNumParts = m_aParts.size();

	for (int nCnt = 0; nCnt < nNumParts; nCnt++)
	{
		//モデル情報管理
		int nModelIdx = pModel->Regist(tag, m_aParts.at(nCnt).nIndex, m_aParts.at(nCnt).aPartsFile);//登録
		BindModel(pModel->GetAddress(nModelIdx));//設定
	
		//パーツモデル情報設定処理
		SetPartsInfo(GetModelInfo(), nCnt);
	}
}

//===========================================================================================================
// モーション情報読込処理
//===========================================================================================================
void CMotionCharacter::LoadMotion()
{
	//ローカル変数宣言
	std::string str;//ファイル検索用
	std::stringstream ss;//文字列保存用
	MotionSet aMotion;//モーション情報
	KeySet aKeySet;//キーセット情報
	Key aKey;//キー情報
	int nMotion = -1;//モーション数

	//ファイルを開く
	std::ifstream file(PARTS_FILE[static_cast<int>(m_Type)]);

	//ファイルが開けない場合
	if (!file)
	{
		assert(false);
		return;
	}

	//EOFが見つかるまで検索を繰り返す
	while (1)
	{
		//検索
		std::getline(file, str);

		//EOFが見つかった場合
		if (str.find("END_MOTION") != std::string::npos)
		{
			//ファイルを閉じる
			file.close();
			break;
		}
		//MOTIONSETが見つかった場合
		else if (str.find("MOTIONSET") != std::string::npos)
		{
			m_aMotion.push_back({});

			//モーション数を加算
			nMotion++;

			//モーション情報を初期化
			m_aMotion.at(nMotion).nCntFrame = 0;
			m_aMotion.at(nMotion).nCntKeySet = 0;
			m_aMotion.at(nMotion).nMaxKeySet = -1;

			//各モーションのデータを検索
			while (1)
			{
				//検索
				std::getline(file, str);

				//END_MOTIONSETが見つかった場合
				if (str.find("END_MOTIONSET") != std::string::npos)
				{
					//モーション読み込み終了
					break;
				}

				//LOOPが見つかった場合
				if (str.find("LOOP") != std::string::npos)
				{
					//読み込んだ文字列を保存
					ss = static_cast<std::stringstream>(str);

					//角度を読み込む
					ss >> str >> m_aMotion.at(nMotion).nLoop;
				}

				//KEYSETが見つかった場合
				if (str.find("KEYSET") != std::string::npos)
				{
					m_aMotion.at(nMotion).aKeyset.push_back({});

					//合計キー数を加算
					m_aMotion.at(nMotion).nMaxKeySet++;

					//各キーセットのデータを検索
					while (1)
					{
						//検索
						std::getline(file, str);

						//END_KEYSETが見つかった場合
						if (str.find("END_KEYSET") != std::string::npos)
						{
							//各キーセットのデータの検索を終了する
							break;
						}
						//FRAMEが見つかった場合
						else if (str.find("FRAME") != std::string::npos)
						{
							//読み込んだ文字列を保存
							ss = static_cast<std::stringstream>(str);

							//角度を読み込む
							ss >> str >> m_aMotion.at(nMotion).aKeyset.at(m_aMotion.at(nMotion).nMaxKeySet).nMaxFrame;
						}
						//KEYが見つかった場合
						else if (str.find("KEY") != std::string::npos)
						{
							//各キーのデータを検索
							while (1)
							{
								//検索
								std::getline(file, str);

								//END_KEYセットが見つかった場合
								if (str.find("END_KEY") != std::string::npos)
								{
									//キー情報を登録
									m_aMotion.at(nMotion).aKeyset.at(m_aMotion.at(nMotion).nMaxKeySet).aKey.push_back(aKey);

									//モーションデータの読み込みを終了
									break;
								}
								//POSが見つかった場合
								else if (str.find("POS") != std::string::npos)
								{
									//読み込んだ文字列を保存
									ss = static_cast<std::stringstream>(str);

									//座標を読み込む
									ss >> str >> aKey.pos.x >> aKey.pos.y >> aKey.pos.z;
								}
								//ROTが見つかった場合
								else if (str.find("ROT") != std::string::npos)
								{
									//読み込んだ文字列を保存
									ss = static_cast<std::stringstream>(str);

									//座標を読み込む
									ss >> str >> aKey.rot.x >> aKey.rot.y >> aKey.rot.z;
								}
							}
						}
						//上記以外が見つかった場合
						else
						{
							//スキップして読み込みを続ける
							continue;
						}
					}
				}
				//上記以外が見つかった場合
				else
				{
					//スキップして読み込みを続ける
					continue;
				}
			}
		}
		//上記以外が見つかった場合
		else
		{
			//検索を続ける
			continue;
		}
	}
}

//===========================================================================================================
// パーツモデル情報設定処理
//===========================================================================================================
void CMotionCharacter::SetPartsInfo(CXfile::ModelInfo aModelInfo, int nParts)
{
	//マテリアル数
	m_aParts.at(nParts).m_aModelInfo.dwNumMat = aModelInfo.dwNumMat;

	//マテリアルへのポインタ
	m_aParts.at(nParts).m_aModelInfo.pBuffMat = aModelInfo.pBuffMat;

	//メッシュの頂点情報へのポインタ
	m_aParts.at(nParts).m_aModelInfo.pMesh = aModelInfo.pMesh;

	//色
	for (int nCntMat = 0; nCntMat < static_cast<int>(m_aParts.at(nParts).m_aModelInfo.dwNumMat); nCntMat++)
	{
		m_aParts.at(nParts).m_aModelInfo.Diffuse.push_back(aModelInfo.Diffuse.at(nCntMat));
		m_aParts.at(nParts).m_aModelInfo.FirstDiffuse.push_back(m_aParts.at(nParts).m_aModelInfo.Diffuse.at(nCntMat));
	}
}

//===========================================================================================================
// パーツのカラー設定処理
//===========================================================================================================
void CMotionCharacter::SetPartsFlash(float a)
{
	//パーツ数を取得
	int nNumParts = m_aParts.size();

	//パーツ数分設定
	for (int nCntParts = 0; nCntParts < nNumParts; nCntParts++)
	{
		//各パーツに指定されたカラー情報を設定
		for (int nCntMat = 0; nCntMat < (int)m_aParts[nCntParts].m_aModelInfo.dwNumMat; nCntMat++)
		{
			m_aParts[nCntParts].m_aModelInfo.Diffuse[nCntMat].a = a;
		}
	}
}

//===========================================================================================================
// //全パーツを合わせたサイズ設定処理
//===========================================================================================================
void CMotionCharacter::SetAllPartsSize()
{
	//ローカル変数宣言
	BYTE* pVtxBuff;//頂点バッファへのポインタ
	D3DXVECTOR3 size(0.0f, 0.0f, 0.0f);//求められたサイズ
	D3DXVECTOR3 vtxMin(0.0f, 0.0f, 0.0f);//各座標の最小の値
	D3DXVECTOR3 vtxMax(0.0f, 0.0f, 0.0f);//各座標の最大の値
	int nNumParts = m_aParts.size();//パーツ数

	for (int nCnt = 0; nCnt < nNumParts; nCnt++)
	{
		//頂点数の取得
		int nNumVtx = m_aParts.at(nCnt).m_aModelInfo.pMesh->GetNumVertices();

		//頂点フォーマットのサイズを取得
		DWORD sizeFVF = D3DXGetFVFVertexSize(m_aParts.at(nCnt).m_aModelInfo.pMesh->GetFVF());

		//頂点バッファのロック
		m_aParts.at(nCnt).m_aModelInfo.pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

		for (int nIdxVtx = 0; nIdxVtx < nNumVtx; nIdxVtx++)
		{
			//頂点座標の代入
			D3DXVECTOR3 vtx = *(D3DXVECTOR3*)pVtxBuff;
			vtx.y += m_aParts.at(nCnt).pos.y;

			//モデルの最小値と最大値を取得
			if (vtx.x < vtxMin.x)
			{//X座標の最小値
				vtxMin.x = vtx.x;
			}
			if (vtx.y < vtxMin.y)
			{//Y座標の最小値
				vtxMin.y = vtx.y;
			}
			if (vtx.z < vtxMin.z)
			{//Z座標の最小値
				vtxMin.z = vtx.z;
			}
			if (vtx.x > vtxMax.x)
			{//X座標の最大値
				vtxMax.x = vtx.x;
			}
			if (vtx.y > vtxMax.y)
			{//Y座標の最大値
				vtxMax.y = vtx.y;
			}
			if (vtx.z > vtxMax.z)
			{//Z座標の最大値
				vtxMax.z = vtx.z;
			}

			//頂点フォーマットのサイズ分ポインタを進める
			pVtxBuff += sizeFVF;
		}

		//頂点バッファのアンロック
		m_aParts.at(nCnt).m_aModelInfo.pMesh->UnlockVertexBuffer();
	}

	//頂点情報設定
	SetVtxMax(vtxMax);//最大
	SetVtxMin(vtxMin);//最小

	//サイズを求める
	size =
	{
		abs(vtxMax.x - vtxMin.x),//x軸
		abs(vtxMax.y - vtxMin.y),//y軸
		abs(vtxMax.z - vtxMin.z)//z軸
	};

	//対角線の長さを求める
	SetLength(sqrtf(powf(size.x, 2.0f) + powf(size.z, 2.0f)) * 0.5f);

	//対角線の角度を求める
	SetAngle(atan2f(size.x, size.z));

	//サイズ情報を設定
	SetUpdateSize(size);
}