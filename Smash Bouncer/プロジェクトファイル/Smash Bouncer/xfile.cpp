//==============================================================================================================================================
//
// xファイル一括管理に関する管理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "xfile.h"//xファイル一括管理
#include "manager.h"//マネージャー
#include "renderer.h"//レンダラー
#include "objectX.h"//xファイルオブジェクト

//===========================================================================================================
// 静的メンバ変数初期化
//===========================================================================================================
CXfile::CModel CXfile::m_apSaveModel[MAX_CATEGORY][MAX_NUM_OBJECT] = {};

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CXfile::CXfile()
{
	//メンバ変数初期化
	for (int nCnt = 0; nCnt < MAX_MODEL; nCnt++)
	{
		m_aModel[nCnt] = nullptr;//Xファイル配列
		m_ModelFileName[nCnt] = nullptr;//作成済みXファイル保管配列
	}

	//Xファイル総数
	m_nAllModel = 0;
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CXfile::~CXfile()
{
}

//===========================================================================================================
// Xファイル破棄
//===========================================================================================================
void CXfile::Unload()
{
	//メンバ変数初期化
	for (int nCnt = 0; nCnt < m_nAllModel; nCnt++)
	{
		if (m_aModel[nCnt] != nullptr)
		{
			m_aModel[nCnt]->Release();
			delete m_aModel[nCnt];
			m_aModel[nCnt] = nullptr;//Xファイル配列
			m_ModelFileName[nCnt] = nullptr;//作成済みXファイル保管配列
		}
	}

	m_nAllModel = 0;
}

//===========================================================================================================
// モデル構造体デストラクタ
//===========================================================================================================
void CXfile::CModel::Release()
{
	//メッシュ情報が存在する場合
	if (pMesh != nullptr)
	{
		//メッシュ情報を解放
		pMesh->Release();
		pMesh = nullptr;
	}

	//マテリアル情報が存在する場合
	if (pBuffMat != nullptr)
	{
		//マテリアル情報を解放
		pBuffMat->Release();
		pBuffMat = nullptr;
	}

	//色合い情報をリセット
	for (int nCntMat = 0; nCntMat < (int)dwNumMat; nCntMat++)
	{
		Diffuse[nCntMat] = {};
		FirstDiffuse[nCntMat] = {};
	}

	//マテリアル数をリセット
	dwNumMat = {};
}

//===========================================================================================================
// Xファイル登録
//===========================================================================================================
int CXfile::Regist(const char* pModelName)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();//デバイスの取得
	int nIdx = 0;//ID

	for (int nCnt = 0; nCnt < MAX_MODEL; nCnt++)
	{
		nIdx = nCnt;//ID設定

		//情報が存在しない場合
		if (m_aModel[nIdx] == nullptr)
		{
			//情報保存用
			m_aModel[nIdx] = NEW CModel();

			//情報保管
			strcpy(m_aModel[nIdx]->aModelFile, pModelName);

			//Xファイルの読み込み
			D3DXLoadMeshFromX(m_aModel[nIdx]->aModelFile, D3DXMESH_SYSTEMMEM, pDevice, nullptr, &m_aModel[nIdx]->pBuffMat, nullptr, &m_aModel[nIdx]->dwNumMat, &m_aModel[nIdx]->pMesh);

			//マテリアル情報が存在する場合
			if (m_aModel[nIdx]->pBuffMat != nullptr)
			{
				//マテリアルデータへのポインタを取得
				D3DXMATERIAL* pMat = (D3DXMATERIAL*)m_aModel[nIdx]->pBuffMat->GetBufferPointer();

				//色合い情報を保存
				for (int nCntMat = 0; nCntMat < (int)m_aModel[nIdx]->dwNumMat; nCntMat++)
				{
					m_aModel[nIdx]->Diffuse[nCntMat] = pMat[nCntMat].MatD3D.Diffuse;
					m_aModel[nIdx]->FirstDiffuse[nCntMat] = m_aModel[nCnt]->Diffuse[nCntMat];
				}
			}

			//Xファイル総数加算
			m_nAllModel++;
			break;
		}

		//ファイルが既に登録されている場合
		else if (strcmp(pModelName, m_aModel[nIdx]->aModelFile) == 0)
		{
			break;
		}
	}

	return nIdx;//IDを返す
}

//===========================================================================================================
// モデルファイルパス読込
//===========================================================================================================
void CXfile::LoadModelFile()
{
	//ローカル変数宣言
	int nNumCategory = -1;//カテゴリー数
	int nNumFile = 0;//タイプ数
	char aDataSearch[CProcess::MAX_TEXT];//ファイル検索用

	//ファイルを開く
	FILE* pFile = fopen(MODEL_FILE, "r");

	//ファイルが開けない場合
	if (pFile == nullptr)
	{
		//処理を抜ける
		return;
	}

	//ENDが見つかるまで読み込みを繰り返す
	while (1)
	{
		//検索
		int nResult = fscanf(pFile, "%s", aDataSearch);

		//EOFが見つかった場合
		if (nResult == EOF)
		{//読み込み終了
			fclose(pFile);
			break;
		}
		//FILE_SETが見つかった場合
		else if (!strcmp(aDataSearch, "FILE_SET"))
		{
			//カテゴリー数を加算
			nNumCategory++;

			//項目ごとのデータを代入
			while (1)
			{
				//検索
				fscanf(pFile, "%s", aDataSearch);

				//END_FILEが見つかった場合
				if (!strcmp(aDataSearch, "END_FILE"))
				{
					//ファイルパスの読み込みを終了
					CObjectX::m_nNumType[nNumCategory] = nNumFile + 1;
					break;
				}
				//FILEが見つかった場合
				else if (!strcmp(aDataSearch, "FILE"))
				{
					//タイプを読み込む
					fscanf(pFile, "%d", &nNumFile);

					//ファイルパスを読み込む
					fscanf(pFile, "%s", &m_apSaveModel[nNumCategory][nNumFile].aModelFile);
				}
			}
		}
		//上記以外が見つかった場合
		else
		{
			//飛ばして検索を続ける
			continue;
		}
	}
}

//===========================================================================================================
// テクスチャファイルパス読込
//===========================================================================================================
void CXfile::LoadTextureFile()
{
	int nNumCategory = 0;
	int nNumFile = 0;
	char aDataSearch[CProcess::MAX_TEXT];

	//ファイルを開く
	FILE* pFile = fopen(TEXTURE_FILE, "r");

	//ファイルが開けない場合
	if (pFile == nullptr)
	{
		return;
	}

	//ENDが見つかるまで読み込みを繰り返す
	while (1)
	{
		int nResult = fscanf(pFile, "%s", aDataSearch);	//検索

		//EOFが見つかった場合
		if (nResult == EOF)
		{//読み込み終了
			fclose(pFile);
			break;
		}
		//BLOCKが見つかった場合
		else if (!strcmp(aDataSearch, "FILE_SET"))
		{
			//カテゴリー番号を読み込み
			fscanf(pFile, "%d", &nNumCategory);

			//項目ごとのデータを代入
			while (1)
			{
				//検索
				fscanf(pFile, "%s", aDataSearch);

				//END_BLOCKが見つかった場合
				if (!strcmp(aDataSearch, "END_FILE"))
				{//読み込みを終了
					break;
				}
				//データの読み込み
				else if (!strcmp(aDataSearch, "FILE"))
				{//ファイルパス
					fscanf(pFile, "%d", &nNumFile);//ファイルパス番号
					fscanf(pFile, "%s", &m_apSaveModel[nNumCategory][nNumFile].aTextureFile);//ファイルパス
				}
			}
		}
		//上記以外が見つかった場合
		else
		{
			continue;
		}
	}
}

//===========================================================================================================
// 登録処理
//===========================================================================================================
void CXfile::Regist()
{
	//ローカル変数宣言
	CTexture* pTex = CManager::GetTexture();
	CXfile* pModel = CManager::GetModel();
	int nTextureIdx = 0;//テクスチャ番号
	int nModelIdx = 0;//モデル番号

	//登録
	for (int nCntCategory = 0; nCntCategory < MAX_CATEGORY; nCntCategory++)
	{
		for (int nCntType = 0; nCntType < CObjectX::m_nNumType[nCntCategory] + 1; nCntType++)
		{
			//モデル
			nModelIdx = pModel->Regist(m_apSaveModel[nCntCategory][nCntType].aModelFile);
			BindModel(nCntCategory, nCntType, pModel->GetAddress(nModelIdx));

			//テクスチャ
			nTextureIdx = pTex->Regist(m_apSaveModel[nCntCategory][nCntType].aTextureFile);
			BindTexture(nCntCategory, nCntType, pTex->GetAddress(nTextureIdx));
		}
	}
}

//===========================================================================================================
// モデル情報設定処理
//===========================================================================================================
void CXfile::BindModel(int nCategory, int nType, CXfile::CModel* aModelInfo)
{
	strcpy(m_apSaveModel[nCategory][nType].aModelFile, aModelInfo->aModelFile);//ファイルパス
	m_apSaveModel[nCategory][nType].pMesh = aModelInfo->pMesh;//メッシュ情報
	m_apSaveModel[nCategory][nType].pBuffMat = aModelInfo->pBuffMat;//マテリアル情報
	m_apSaveModel[nCategory][nType].dwNumMat = aModelInfo->dwNumMat;//マテリアル数

	//色合い
	for (int nCntMat = 0; nCntMat < (int)m_apSaveModel[nCategory][nType].dwNumMat; nCntMat++)
	{
		m_apSaveModel[nCategory][nType].Diffuse[nCntMat] = aModelInfo->Diffuse[nCntMat];
		m_apSaveModel[nCategory][nType].FirstDiffuse[nCntMat] = m_apSaveModel[nCategory][nType].Diffuse[nCntMat];
	}
}