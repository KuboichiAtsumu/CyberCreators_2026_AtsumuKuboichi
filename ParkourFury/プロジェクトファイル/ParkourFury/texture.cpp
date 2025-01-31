//==============================================================================================================================================
//
// テクスチャに関する管理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "texture.h"
#include "manager.h"

//===========================================================================================================
// 静的メンバ変数初期化
//===========================================================================================================
CTexture* CTexture::m_pTexture = nullptr;

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CTexture::CTexture()
{
	//読込処理
	Load();
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CTexture::~CTexture()
{
}

//===========================================================================================================
// インスタンス取得
//===========================================================================================================
CTexture* CTexture::GetInstance()
{
	//インスタンスが存在しない場合
	if (m_pTexture == nullptr)
	{
		//インスタンスを生成
		m_pTexture = NEW CTexture();
	}

	return m_pTexture;
}

//===========================================================================================================
// インスタンス破棄
//===========================================================================================================
void CTexture::Release()
{
	//インスタンスが存在する場合
	if (m_pTexture != nullptr)
	{
		//インスタンスを破棄
		m_pTexture->Unload();
		delete m_pTexture;
		m_pTexture = nullptr;
	}
}

//===========================================================================================================
// 読み込み処理
//===========================================================================================================
void CTexture::Load()
{
	//ローカル変数宣言
	CTag* pTag = CManager::GetInstance()->GetTag();//タグのインスタンス
	std::string str;//ファイル検索用
	Texture aTexture = {};//モデル登録情報保存用

	//ファイルを開く
	std::ifstream file("data/TEXTURE/TextureFile.txt");

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
			pTag->SetTag(aTexture.tag, str);
		}

		//項目ごとのデータを代入
		while (1)
		{
			//カテゴリー検索
			std::getline(file, str);

			//FILEが見つかった場合
			if (str.compare("FILE") == 0)
			{
				std::stringstream ss;

				//タイプを読み込む
				std::getline(file, str);
				ss = static_cast<std::stringstream>(str);
				ss >> aTexture.nType;

				//ファイルパスを読み込む
				std::getline(file, aTexture.FileName);

				//登録処理
				Regist(aTexture.tag, aTexture.nType, aTexture.FileName);
			}
			//END_FILEが見つかった場合
			else if (str.compare("END_FILE") == 0)
			{
				//該当タグのタイプ数を設定
				if (pTag->GetNumType(aTexture.tag) < aTexture.nType + 1)
				{
					pTag->SetNumType(aTexture.nType + 1, aTexture.tag);
				}
	
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
void CTexture::Unload()
{
	//テクスチャ情報数を保存
	int nSize = m_TextureInfo.size();

	//テクスチャ保存情報破棄
	for (int nCnt = 0; nCnt < nSize; nCnt++)
	{
		//テクスチャ情報が存在する場合
		if (m_TextureInfo.back().pTexture != nullptr)
		{
			m_TextureInfo.back().pTexture->Release();
			m_TextureInfo.back().pTexture = nullptr;
		}

		//末尾の要素を削除
		m_TextureInfo.pop_back();
	}

	m_TextureInfo.clear();
}

//===========================================================================================================
// テクスチャ登録
//===========================================================================================================
int CTexture::Regist(CTag::TAG tag, int nType, std::string pTextureName)
{
	//ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//デバイスの取得
	int nIdx = 0;//テクスチャID
	bool bAlreadyRegist = false;//引数の情報の登録済判定

	for (int nCnt = 0; nCnt < static_cast<int>(m_TextureInfo.size()); nCnt++)
	{
		//ID設定
		nIdx = nCnt;

		//テクスチャ情報が存在する場合
		if (pTextureName.compare(m_TextureInfo.at(nCnt).FileName) == 0)
		{
			//登録済判定をtrueにする
			bAlreadyRegist = true;
			break;
		}
	}

	//登録されていなかった場合
	if (!bAlreadyRegist)
	{
		//ローカル変数宣言
		Texture aTexture = {nullptr};//テクスチャ情報保存用

		//情報を保存
		aTexture.nIdx = nIdx;//ID
		aTexture.tag = tag;//タグ
		aTexture.nType = nType;//タイプ
		aTexture.FileName = pTextureName;//ファイルパス

		//テクスチャ情報読込
		D3DXCreateTextureFromFile(pDevice, aTexture.FileName.c_str(), &aTexture.pTexture);

		//読み込んだテクスチャ情報を末尾に追加
		m_TextureInfo.push_back(aTexture);
	}
	
	//IDを返す
	return nIdx;
}

//===========================================================================================================
// 情報取得処理 ( カテゴリー・タイプ検索 )
//===========================================================================================================
LPDIRECT3DTEXTURE9 CTexture::GetAddress(CTag::TAG tag, int nType)
{
	//テクスチャのデータサイズを取得
	int nSize = static_cast<int>(m_TextureInfo.size());

	for (int nCnt = 0; nCnt < nSize; nCnt++)
	{
		//カテゴリーとモデルが一致した場合
		if (m_TextureInfo.at(nCnt).tag == tag && m_TextureInfo.at(nCnt).nType == nType)
		{
			//モデル情報を返す
			return m_TextureInfo.at(nCnt).pTexture;
		}
	}

	return nullptr;
}
