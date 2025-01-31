//==============================================================================================================================================
//
// xファイルに関する管理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _XFILE_H_ //このマクロ定義がされなかったら
#define _XFILE_H_ //2重インクルード防止のマクロ定義

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "tag.h"

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CXfile
{
private:
	CXfile();//コンストラクタ
	~CXfile();//デストラクタ

public:
	//モデル情報
	struct ModelInfo
	{
		//変数
		LPD3DXMESH pMesh;//メッシュ情報
		LPD3DXBUFFER pBuffMat;//マテリアル情報
		DWORD dwNumMat;//マテリアルの数
		std::vector<D3DCOLORVALUE> Diffuse;//モデルのカラー情報
		std::vector<D3DCOLORVALUE> FirstDiffuse;//モデルのカラー情報
	};

	//モデル登録情報
	struct Model
	{
		ModelInfo* aModel;//モデル情報
		CTag::TAG tag;//タグ
		int nType;//タイプ
		int nIdx;//ID
		std::string FileName;//ファイルパス
	};

	//======================================
	// 関数
	//======================================
	static CXfile* GetInstance();//インスタンスの取得
	static void Release();//インスタンスの破棄
	void Load();//読み込み処理
	void Unload();//削除処理
	int Regist(CTag::TAG tag, int nType, std::string pModelName);//登録処理
	Model GetModel(CTag::TAG tag, int nType);//モデル登録情報取得
	ModelInfo* GetAddress(int nIdx) { return m_ModelInfo.at(nIdx).aModel; }//情報取得処理 ( インデックス検索 )
	ModelInfo* GetAddress(CTag::TAG tag, int nType);//情報取得処理 ( タグ・タイプ検索 )

private:
	//======================================
	// 変数
	//======================================
	static CXfile* m_pXfile;//インスタンス
	std::vector<Model> m_ModelInfo;//モデル情報
};

#endif
