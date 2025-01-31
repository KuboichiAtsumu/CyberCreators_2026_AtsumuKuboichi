//==============================================================================================================================================
//
// Xファイル一括管理に関する管理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "xfile.h"
#include "manager.h"

//===========================================================================================================
// 静的メンバ変数初期化
//===========================================================================================================
CXfile* CXfile::m_pXfile = nullptr;

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CXfile::CXfile()
{
	//メンバ変数初期化
	m_ModelInfo.clear();//モデル情報

	//読込処理処理
	Load();
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CXfile::~CXfile()
{
}

//===========================================================================================================
// インスタンス取得
//===========================================================================================================
CXfile* CXfile::GetInstance()
{
	//インスタンスが存在しない場合
	if (m_pXfile == nullptr)
	{
		//インスタンスを生成
		m_pXfile = NEW CXfile();
	}

	return m_pXfile;
}

//===========================================================================================================
// インスタンス破棄
//===========================================================================================================
void CXfile::Release()
{
	//インスタンスが存在する場合
	if (m_pXfile != nullptr)
	{
		m_pXfile->Unload();
		delete m_pXfile;
		m_pXfile = nullptr;
	}
}

//===========================================================================================================
// 読み込み処理
//===========================================================================================================
void CXfile::Load()
{
	//ローカル変数宣言
	CTag* pTag = CManager::GetInstance()->GetTag();//タグのインスタンス
	std::string str;//ファイル検索用
	Model aModel = {};//モデル登録情報保存用

	//ファイルを開く
	std::ifstream file("data/MODEL/ModelFile.txt");

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

		//ENDが見つかった場合
		if (str.compare("END") == 0)
		{
			break;
		}
		//コメントアウトが見つかった場合
		else if (!str.empty() && str.front() == '#')
		{
			//読み込みを続ける
			continue;
		}
		//空白の場合
		else if (str.empty())
		{
			//読み込みを続ける
			continue;
		}
		else
		{
			//タグを設定
			pTag->SetTag(aModel.tag, str);
		}

		//項目ごとのデータを代入
		while (1)
		{
			//タグ検索
			std::getline(file, str);

			//FILEが見つかった場合
			if (str.compare("FILE") == 0)
			{
				std::stringstream ss;

				//タイプを読み込む
				std::getline(file, str);
				ss = static_cast<std::stringstream>(str);
				ss >> aModel.nType;

				//ファイルパスを読み込む
				std::getline(file, aModel.FileName);

				//登録処理
				Regist(aModel.tag, aModel.nType, aModel.FileName);
			}
			//END_FILEが見つかった場合
			else if (str.compare("END_FILE") == 0)
			{
				//該当タグのタイプ数を設定
				if (pTag->GetNumType(aModel.tag) < aModel.nType + 1)
				{
					pTag->SetNumType(aModel.nType + 1, aModel.tag);
				}

				//ファイルパスの読み込みを終了
				break;
			}
		}
	}

	//ファイルを閉じる
	file.close();
}

//===========================================================================================================
// 削除処理
//===========================================================================================================
void CXfile::Unload()
{
	//モデル情報数を保存
	int nSize = m_ModelInfo.size();

	//モデル保存情報破棄
	for (int nCnt = 0; nCnt < nSize; nCnt++)
	{
		//メッシュ情報が存在する場合
		if (m_ModelInfo.back().aModel->pMesh != nullptr)
		{
			//メッシュ情報を解放
			m_ModelInfo.back().aModel->pMesh->Release();
			m_ModelInfo.back().aModel->pMesh = nullptr;
		}

		//マテリアル情報が存在する場合
		if (m_ModelInfo.back().aModel->pBuffMat != nullptr)
		{
			//マテリアル情報を解放
			m_ModelInfo.back().aModel->pBuffMat->Release();
			m_ModelInfo.back().aModel->pBuffMat = nullptr;
		}

		//カラー情報を末尾から破棄
		for (int nCntMat = 0; nCntMat < static_cast<int>(m_ModelInfo.back().aModel->dwNumMat); nCntMat++)
		{
			m_ModelInfo.back().aModel->Diffuse.pop_back();
			m_ModelInfo.back().aModel->FirstDiffuse.pop_back();
		}

		//カラー情報をクリア
		m_ModelInfo.back().aModel->Diffuse.clear();
		m_ModelInfo.back().aModel->FirstDiffuse.clear();

		//マテリアル数をリセット
		m_ModelInfo.back().aModel->dwNumMat = static_cast<DWORD>(0);

		delete m_ModelInfo.back().aModel;
		m_ModelInfo.back().aModel = nullptr;

		//末尾の要素を削除
		m_ModelInfo.pop_back();
	}

	//モデル情報をクリア
	m_ModelInfo.clear();
}

//===========================================================================================================
// 登録処理
//===========================================================================================================
int CXfile::Regist(CTag::TAG tag, int nType, std::string pModelName)
{
	//ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//デバイスの取得
	int nIdx = 0;//モデルインデックス
	int nNumParts = m_ModelInfo.size();//パーツ数取得
	bool bAlreadyRegist = false;//引数の情報の登録済判定

	for (int nCnt = 0; nCnt < nNumParts; nCnt++)
	{
		//モデル情報が存在する場合
		if (pModelName.compare(m_ModelInfo.at(nCnt).FileName) == 0)
		{
			//登録済判定をtrueにする
			bAlreadyRegist = true;
			break;
		}

		//インデックス設定
		nIdx++;
	}

	//登録されていなかった場合
	if (!bAlreadyRegist)
	{
		//ローカル変数宣言
		Model modelinfo = {};//モデル情報保存用

		//情報を保存
		modelinfo.aModel = NEW ModelInfo();
		modelinfo.nIdx = nIdx;//インデックス
		modelinfo.FileName = pModelName;//ファイルパス
		modelinfo.tag = tag;//タグ
		modelinfo.nType = nType;//タイプ

		//Xファイルの読み込み
		D3DXLoadMeshFromX(modelinfo.FileName.c_str(), D3DXMESH_SYSTEMMEM, pDevice, nullptr, &modelinfo.aModel->pBuffMat, nullptr, &modelinfo.aModel->dwNumMat, &modelinfo.aModel->pMesh);

		//マテリアル情報が存在する場合
		if (modelinfo.aModel->pBuffMat != nullptr)
		{
			//マテリアルデータへのポインタを取得
			D3DXMATERIAL* pMat = (D3DXMATERIAL*)modelinfo.aModel->pBuffMat->GetBufferPointer();

			//色合い情報を保存
			for (int nCntMat = 0; nCntMat < (int)modelinfo.aModel->dwNumMat; nCntMat++)
			{
				modelinfo.aModel->Diffuse.push_back(pMat[nCntMat].MatD3D.Diffuse);
				modelinfo.aModel->FirstDiffuse.push_back(pMat[nCntMat].MatD3D.Diffuse);
			}
		}

		//読み込んだモデル情報を末尾に追加
		m_ModelInfo.push_back(modelinfo);
	}

	//インデックスを返す
	return nIdx;
}

//===========================================================================================================
// 情報取得処理 ( タグ・タイプ検索 )
//===========================================================================================================
CXfile::ModelInfo* CXfile::GetAddress(CTag::TAG tag, int nType)
{
	for (int nCnt = 0; nCnt < static_cast<int>(m_ModelInfo.size()); nCnt++)
	{
		//タグとモデルが一致した場合
		if (m_ModelInfo.at(nCnt).tag == tag
			&& m_ModelInfo.at(nCnt).nType == nType)
		{
			//モデル情報を返す
			return m_ModelInfo.at(nCnt).aModel;
		}
	}

	return nullptr;
}

//===========================================================================================================
// モデル登録情報取得処理 ( タグ・タイプ検索 )
//===========================================================================================================
CXfile::Model CXfile::GetModel(CTag::TAG tag, int nType)
{
	for (int nCnt = 0; nCnt < static_cast<int>(m_ModelInfo.size()); nCnt++)
	{
		//タグとモデルが一致した場合
		if (m_ModelInfo.at(nCnt).tag == tag
			&& m_ModelInfo.at(nCnt).nType == nType)
		{
			//モデル情報を返す
			return m_ModelInfo.at(nCnt);
		}
	}

	return { nullptr };
}
