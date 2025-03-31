//==============================================================================================================================================
//
// モーションに関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "motion.h"//モーション

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CMotionCharacter::CMotionCharacter(int nPriority) : CObjectX(nPriority)
{
	//メンバ変数初期化
	m_State = NEUTRAL;//状態
	m_nParts = 0;//パーツ数
	m_nMotion = -1;//モーション数
	m_fSpeed = 0.0f;//速度倍率

	//パーツ情報
	for (int nCnt = 0; nCnt < MAX_PARTS; nCnt++)
	{
		m_aParts[nCnt] = {};
	}
	//モーション情報
	for (int nCnt = 0; nCnt < MAX_MOTION; nCnt++)
	{
		m_aMotion[nCnt] = {};
	}
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CMotionCharacter::~CMotionCharacter()
{
}

//===========================================================================================================
// 解放処理
//===========================================================================================================
void CMotionCharacter::Release()
{
	//パーツ情報を破棄
	for (int nCnt = 0; nCnt < m_nParts; nCnt++)
	{
		m_aParts[nCnt] = {};
	}
	//モーション情報を破棄
	for (int nCnt = 0; nCnt < m_nMotion; nCnt++)
	{
		m_aMotion[nCnt] = {};
	}

	//Xファイルオブジェクト解放処理
	CObjectX::Release();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CMotionCharacter::Update()
{
	//ローカル変数宣言
	MOTION_STATE State = {};//現在の状態
	int nKeySet = 0;//現在のキー

	for (int nCntParts = 0; nCntParts < m_nParts; nCntParts++)
	{
		//上半身パーツの場合
		if (m_aParts[nCntParts].nHalf == UP_BODY)
		{
			//現在の状態を保存
			State = m_State;
		}

		//下半身パーツの場合
		else if (m_aParts[nCntParts].nHalf == DOWN_BODY)
		{
			//対象オブジェクトの移動量情報取得
			D3DXVECTOR3 move = GetMove();

			//動いていない場合
			if ((move.x <= MAX_JUDGE_MOVE && move.x >= -MAX_JUDGE_MOVE) && (move.z <= MAX_JUDGE_MOVE && move.z >= -MAX_JUDGE_MOVE))
			{
				//現在の状態を保存
				State = m_State;
			}
			//動いている場合
			else
			{
				//移動状態
				State = MOVE;
			}
		}

		//現在のキーを保存
		nKeySet = m_aMotion[State].nCntKeySet;

		//モーション処理
		Motion(State, nCntParts, nKeySet);
	}

	//カウントが指定のフレーム数になった場合
	if (m_aMotion[State].nCntFrame >= m_aMotion[State].aKeyset[nKeySet].nMaxFrame)
	{
		//フレーム数カウントを初期化
		m_aMotion[State].nCntFrame = 0;

		//キー数を加算
		m_aMotion[State].nCntKeySet++;

		//キー数が最大数になった
		if (m_aMotion[State].nCntKeySet > m_aMotion[State].nMaxKeySet)
		{
			//ループしない場合
			if (!(bool)m_aMotion[State].nLoop)
			{
				//ニュートラル状態
				m_State = NEUTRAL;
			}

			//キー数を初期化
			m_aMotion[State].nCntKeySet = 0;
		}
	}

	//フレーム数更新
	m_aMotion[State].nCntFrame++;
}

//===========================================================================================================
// モーション処理
//===========================================================================================================
void CMotionCharacter::Motion(MOTION_STATE state, int nParts, int nKeySet)
{
	//ローカル変数宣言
	int nNextKeySet = (nKeySet + 1) % (m_aMotion[state].nMaxKeySet + 1);//次のキー
	float fFrame = (float)m_aMotion[state].nCntFrame / m_aMotion[state].aKeyset[nKeySet].nMaxFrame;//現在のフレームと全体フレームの割合

	//============================
	// 次のキーまでの差分を求める
	//============================

	//向き
	float fRotx = m_aMotion[state].aKeyset[nNextKeySet].aKey[nParts].rot.x - m_aMotion[state].aKeyset[nKeySet].aKey[nParts].rot.x;//X軸
	float fRoty = m_aMotion[state].aKeyset[nNextKeySet].aKey[nParts].rot.y - m_aMotion[state].aKeyset[nKeySet].aKey[nParts].rot.y;//Y軸
	float fRotz = m_aMotion[state].aKeyset[nNextKeySet].aKey[nParts].rot.z - m_aMotion[state].aKeyset[nKeySet].aKey[nParts].rot.z;//Z軸

	//位置
	float fPosx = m_aMotion[state].aKeyset[nNextKeySet].aKey[nParts].pos.x - m_aMotion[state].aKeyset[nKeySet].aKey[nParts].pos.x;//X座標
	float fPosy = m_aMotion[state].aKeyset[nNextKeySet].aKey[nParts].pos.y - m_aMotion[state].aKeyset[nKeySet].aKey[nParts].pos.y;//Y座標
	float fPosz = m_aMotion[state].aKeyset[nNextKeySet].aKey[nParts].pos.z - m_aMotion[state].aKeyset[nKeySet].aKey[nParts].pos.z;//Z座標

	//============================
	// 次のキーを反映
	//============================

	//向きを更新
	m_aParts[nParts].rot.x = m_aMotion[state].aKeyset[nKeySet].aKey[nParts].rot.x + fRotx * fFrame;//X軸
	m_aParts[nParts].rot.y = m_aMotion[state].aKeyset[nKeySet].aKey[nParts].rot.y + fRoty * fFrame;//Y軸
	m_aParts[nParts].rot.z = m_aMotion[state].aKeyset[nKeySet].aKey[nParts].rot.z + fRotz * fFrame;//Z軸

	//位置を更新
	m_aParts[nParts].pos.x = m_aParts[nParts].pos.x + m_aMotion[state].aKeyset[nKeySet].aKey[nParts].pos.x + fPosx * fFrame;//X座標
	m_aParts[nParts].pos.y = m_aParts[nParts].pos.y + m_aMotion[state].aKeyset[nKeySet].aKey[nParts].pos.y + fPosy * fFrame;//Y座標
	m_aParts[nParts].pos.z = m_aParts[nParts].pos.z + m_aMotion[state].aKeyset[nKeySet].aKey[nParts].pos.z + fPosz * fFrame;//Z座標
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CMotionCharacter::Draw()
{
	//ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();//デバイスのポインタ
	D3DXMATRIX mtxRot, mtxTrans, mtxParent;//計算用マトリックス
	D3DMATERIAL9 matDef;//現在のマテリアル保存用
	D3DXMATERIAL* pMat;//マテリアルのデータへのポインタ
	D3DXMATRIX mtxWorld = GetMatrix();//ワールドマトリックス情報
	D3DXVECTOR3 pos = GetPos();//位置
	D3DXVECTOR3 rot = GetRot();//向き

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

	for (int nCntParts = 0; nCntParts < m_nParts; nCntParts++)
	{
		//各パーツモデルのワールドマトリックスの初期化
		D3DXMatrixIdentity(&m_aParts[nCntParts].mtxWorld);

		//各パーツモデルの向きを反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, m_aParts[nCntParts].rot.y, m_aParts[nCntParts].rot.x, m_aParts[nCntParts].rot.z);
		D3DXMatrixMultiply(&m_aParts[nCntParts].mtxWorld, &m_aParts[nCntParts].mtxWorld, &mtxRot);

		//各パーツモデルの位置を反映
		D3DXMatrixTranslation(&mtxTrans, m_aParts[nCntParts].pos.x, m_aParts[nCntParts].pos.y, m_aParts[nCntParts].pos.z);
		D3DXMatrixMultiply(&m_aParts[nCntParts].mtxWorld, &m_aParts[nCntParts].mtxWorld, &mtxTrans);

		//親の番号を保存
		int nParent = m_aParts[nCntParts].nParent;

		//親のマトリックスを反映
		if (nParent != -1)
		{//親の番号を持つ場合
			mtxParent = m_aParts[nParent].mtxWorld;//親モデルのマトリックス
		}
		else
		{//親の番号を持たない場合
			mtxParent = mtxWorld;//キャラクター自身のマトリックス
		}

		//親のマトリックスを掛け合わせる
		D3DXMatrixMultiply(&m_aParts[nCntParts].mtxWorld, &m_aParts[nCntParts].mtxWorld, &mtxParent);

		//ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &m_aParts[nCntParts].mtxWorld);

		//現在のマテリアルを取得
		pDevice->GetMaterial(&matDef);

		//マテリアルデータへのポインタを取得
		pMat = (D3DXMATERIAL*)m_aParts[nCntParts].pBuffMat->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)m_aParts[nCntParts].dwNumMat; nCntMat++)
		{
			//マテリアル情報を反映
			pMat[nCntMat].MatD3D.Diffuse = m_aParts[nCntParts].Diffuse[nCntMat];

			//マテリアルの設定
			pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

			//テクスチャの設定
			pDevice->SetTexture(0, nullptr);

			//モデル(パーツ)の描画
			m_aParts[nCntParts].pMesh->DrawSubset(nCntMat);
		}

		//保存していたマテリアルを戻す
		pDevice->SetMaterial(&matDef);
	}
}

//===========================================================================================================
// パーツファイル読込処理
//===========================================================================================================
void CMotionCharacter::LoadParts(MOTION_TYPE type)
{
	//ローカル変数宣言
	int nIndex = 0;//各パーツ番号
	char aDataSearch[MAX_TEXT];//データ読み込み用

	//ファイルを開く
	FILE* pFile = fopen(PARTS_FILE[type], "r");

	//ファイルが開けない場合
	if (pFile == nullptr)
	{
		//処理を終了
		return;
	}

	//ENDが見つかるまで読み込みを繰り返す
	while (1)
	{
		//検索
		int nResult = fscanf(pFile, "%s", aDataSearch);

		//END_PARTSが見つかった場合
		if (nResult == EOF)
		{
			//ファイルを閉じる
			fclose(pFile);

			//検索終了
			break;
		}
		//PARTS_FILENAMEが見つかった場合
		else if (!strcmp(aDataSearch, "PARTS_FILENAME"))
		{
			//パーツのファイル名を反映
			fscanf(pFile, "%s", &m_aParts[m_nParts].aPartsFile);

			//パーツ数カウントを加算
			m_nParts++;
		}
		//MOVEが見つかった場合
		else if (!strcmp(aDataSearch, "MOVE"))
		{
			//プレイヤーの速度を反映
			fscanf(pFile, "%f", &m_fSpeed);
		}
		//PARTSSETが見つかった場合
		else if (!strcmp(aDataSearch, "PARTSSET"))
		{
			//各パーツのデータを検索
			while (1)
			{
				//検索
				fscanf(pFile, "%s", aDataSearch);

				//END_PARTSSETが見つかった場合
				if (!strcmp(aDataSearch, "END_PARTSSET"))
				{
					//各パーツごとのデータを検索を終了
					break;
				}
				//INDEXが見つかった場合
				else if (!strcmp(aDataSearch, "INDEX"))
				{
					//番号を保存
					fscanf(pFile, "%d", &nIndex);

					//パーツの番号がパーツ数を超えている場合
					if (nIndex > m_nParts)
					{//各パーツごとのデータ検索を終了
						break;
					}

					//番号を反映
					fscanf(pFile, "%d", &m_aParts[nIndex].nIndex);
				}
				//PARENTが見つかった場合
				else if (!strcmp(aDataSearch, "PARENT"))
				{
					//親の番号を反映
					fscanf(pFile, "%d", &m_aParts[nIndex].nParent);
				}
				//HALFが見つかった場合
				else if (!strcmp(aDataSearch, "HALF"))
				{
					//上半身か下半身かの判定を反映
					fscanf(pFile, "%d", &m_aParts[nIndex].nHalf);
				}
				//POSが見つかった場合
				else if (!strcmp(aDataSearch, "POS"))
				{
					//位置を反映
					fscanf(pFile, "%f", &m_aParts[nIndex].pos.x);
					fscanf(pFile, "%f", &m_aParts[nIndex].pos.y);
					fscanf(pFile, "%f", &m_aParts[nIndex].pos.z);
				}
				//ROTが見つかった場合
				else if (!strcmp(aDataSearch, "ROT"))
				{
					//向きを反映
					fscanf(pFile, "%f", &m_aParts[nIndex].rot.x);
					fscanf(pFile, "%f", &m_aParts[nIndex].rot.y);
					fscanf(pFile, "%f", &m_aParts[nIndex].rot.z);
				}
				//上記以外が見つかった場合
				else
				{
					//その行をスキップして読み込みを続ける
					continue;
				}
			}
		}
		//上記以外が見つかった場合
		else
		{
			//その行をスキップして読み込みを続ける
			continue;
		}
	}

	//モデル生成
	CXfile* pModel = CManager::GetModel();
	for (int nCnt = 0; nCnt < m_nParts; nCnt++)
	{
		int nModelIdx = pModel->Regist(m_aParts[nCnt].aPartsFile);//登録
		BindModel(pModel->GetAddress(nModelIdx));//設定
		SetModelIdx(nModelIdx);//ID設定
		SetSize();//サイズ設定

		//パーツモデル情報設定処理
		SetPartsInfo(GetModelInfo(), nCnt);
	}
}

//===========================================================================================================
// パーツモデル情報設定処理
//===========================================================================================================
void CMotionCharacter::SetPartsInfo(CXfile::CModel aModelInfo, int nParts)
{
	//マテリアル数
	m_aParts[nParts].dwNumMat = aModelInfo.dwNumMat;

	//マテリアルへのポインタ
	m_aParts[nParts].pBuffMat = aModelInfo.pBuffMat;

	//メッシュの頂点情報へのポインタ
	m_aParts[nParts].pMesh = aModelInfo.pMesh;

	//色
	for (int nCntMat = 0; nCntMat < (int)m_aParts[nParts].dwNumMat; nCntMat++)
	{
		m_aParts[nParts].Diffuse[nCntMat] = aModelInfo.Diffuse[nCntMat];
		m_aParts[nParts].FirstDiffuse[nCntMat] = m_aParts[nParts].Diffuse[nCntMat];
	}
}

//===========================================================================================================
// モーション情報読込処理
//===========================================================================================================
void CMotionCharacter::LoadMotion(MOTION_TYPE type)
{
	//ローカル変数宣言
	int nCntParts = 0;//パーツ数
	char aDataSearch[MAX_TEXT];//データ検索用

	//ファイルを開く
	FILE* pFile = fopen(PARTS_FILE[type], "r");

	//ファイルが開けない場合
	if (pFile == nullptr)
	{
		//読み込み処理を終了
		return;
	}

	//EOFが見つかるまで検索を繰り返す
	while (1)
	{
		//検索
		int nResult = fscanf(pFile, "%s", aDataSearch);//検索結果をint型に代入

		//MOTIONSETが見つかった場合
		if (!strcmp(aDataSearch, "MOTIONSET"))
		{
			//モーション数をカウント
			m_nMotion++;
			m_aMotion[m_nMotion].nMaxKeySet = -1;

			//各モーションのデータを検索
			while (1)
			{
				//検索
				fscanf(pFile, "%s", aDataSearch);

				//END_MOTIONSETが見つかった場合
				if (!strcmp(aDataSearch, "END_MOTIONSET"))
				{
					//モーション読み込み終了
					break;
				}

				//LOOPが見つかった場合
				if (!strcmp(aDataSearch, "LOOP"))
				{
					//ループ判定を取得
					fscanf(pFile, "%d", &m_aMotion[m_nMotion].nLoop);
				}

				//KEYSETが見つかった場合
				if (!strcmp(aDataSearch, "KEYSET"))
				{
					//合計キー数を加算
					m_aMotion[m_nMotion].nMaxKeySet++;
					
					//各キーセットのデータを検索
					while (1)
					{
						//検索
						fscanf(pFile, "%s", aDataSearch);

						//KEYが見つかった場合
						if (!strcmp(aDataSearch, "KEY"))
						{
							//各キーのデータを検索
							while (1)
							{
								//検索
								fscanf(pFile, "%s", aDataSearch);

								//FRAMEが見つかった場合
								if (!strcmp(aDataSearch, "FRAME"))
								{
									//フレーム数を読み込む
									fscanf(pFile, "%d", &m_aMotion[m_nMotion].aKeyset[m_aMotion[m_nMotion].nMaxKeySet].nMaxFrame);
								}
								//POSが見つかった場合
								else if (!strcmp(aDataSearch, "POS"))
								{
									//位置情報を読み込む
									fscanf(pFile, "%f", &m_aMotion[m_nMotion].aKeyset[m_aMotion[m_nMotion].nMaxKeySet].aKey[nCntParts].pos.x);	//X座標
									fscanf(pFile, "%f", &m_aMotion[m_nMotion].aKeyset[m_aMotion[m_nMotion].nMaxKeySet].aKey[nCntParts].pos.y);	//Y座標
									fscanf(pFile, "%f", &m_aMotion[m_nMotion].aKeyset[m_aMotion[m_nMotion].nMaxKeySet].aKey[nCntParts].pos.z);	//Z座標
								}
								//ROTが見つかった場合
								else if (!strcmp(aDataSearch, "ROT"))
								{
									//向き情報を読み込む
									fscanf(pFile, "%f", &m_aMotion[m_nMotion].aKeyset[m_aMotion[m_nMotion].nMaxKeySet].aKey[nCntParts].rot.x);	//X軸
									fscanf(pFile, "%f", &m_aMotion[m_nMotion].aKeyset[m_aMotion[m_nMotion].nMaxKeySet].aKey[nCntParts].rot.y);	//Y軸
									fscanf(pFile, "%f", &m_aMotion[m_nMotion].aKeyset[m_aMotion[m_nMotion].nMaxKeySet].aKey[nCntParts].rot.z);	//Z軸
								}
								//END_KEYセットが見つかった場合
								else if (!strcmp(aDataSearch, "END_KEY"))
								{
									//パーツ数を加算
									nCntParts++;

									//モーションデータの読み込みを終了
									break;
								}
							}
						}
						//END_KEYSETが見つかった場合
						else if (!strcmp(aDataSearch, "END_KEYSET"))
						{
							//パーツ数カウントをリセット
							nCntParts = 0;

							//各キーセットのデータの検索を終了する
							break;
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
		//EOFが見つかった場合
		else if (nResult == EOF)
		{
			//ファイルを閉じる
			fclose(pFile);
			break;
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
// パーツのカラー設定処理
//===========================================================================================================
void CMotionCharacter::SetPartsFlash(float a)
{
	//パーツ数分設定
	for (int nCntParts = 0; nCntParts < m_nParts; nCntParts++)
	{
		//各パーツに指定されたカラー情報を設定
		for (int nCntMat = 0; nCntMat < (int)m_aParts[nCntParts].dwNumMat; nCntMat++)
		{
			m_aParts[nCntParts].Diffuse[nCntMat].a = a;
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
	D3DXVECTOR3 size = { 0.0f, 0.0f, 0.0f };//求められたサイズ
	D3DXVECTOR3 vtxMin = { 0.0f, 0.0f, 0.0f };//各座標の最小の値
	D3DXVECTOR3 vtxMax = { 0.0f, 0.0f, 0.0f };//各座標の最大の値

	for (int nCnt = 0; nCnt < m_nParts; nCnt++)
	{
		//頂点数の取得
		int nNumVtx = m_aParts[nCnt].pMesh->GetNumVertices();

		//頂点フォーマットのサイズを取得
		DWORD sizeFVF = D3DXGetFVFVertexSize(m_aParts[nCnt].pMesh->GetFVF());

		//頂点バッファのロック
		m_aParts[nCnt].pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

		for (int nIdxVtx = 0; nIdxVtx < nNumVtx; nIdxVtx++)
		{
			//頂点座標の代入
			D3DXVECTOR3 vtx = *(D3DXVECTOR3*)pVtxBuff;

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
		m_aParts[nCnt].pMesh->UnlockVertexBuffer();
	}

	//サイズを求める
	size =
	{
		vtxMax.x - vtxMin.x,//x軸
		vtxMax.y - vtxMin.y,//y軸
		vtxMax.z - vtxMin.z//z軸
	};

	//サイズ情報を設定
	SetUpdateSize(size);
}
