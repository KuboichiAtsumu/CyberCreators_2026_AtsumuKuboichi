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
#include "main.h"

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CXfile
{
public:
	//======================================
	// 変数
	//======================================
	static const int MAX_MAT = 100;//マテリアル最大数
	static constexpr int MAX_NUM_OBJECT = 100;//種類の最大数

	//======================================
	// 列挙型定義
	//======================================
	struct CModel
	{
		//関数
		void Release();//削除処理

		//変数
		static const int MAX_MAT = 100;//マテリアル最大数
		char aTextureFile[CProcess::MAX_TEXT];//ファイルパス
		char aModelFile[CProcess::MAX_TEXT];//ファイルパス
		LPDIRECT3DTEXTURE9 m_pTexture;//テクスチャへのポインタ
		LPD3DXMESH pMesh;//メッシュ情報
		LPD3DXBUFFER pBuffMat;//マテリアル情報
		DWORD dwNumMat;//マテリアルの数
		D3DCOLORVALUE Diffuse[MAX_MAT];//色合い情報
		D3DCOLORVALUE FirstDiffuse[MAX_MAT];//色合い情報初期値
	};

	//======================================
	// カテゴリー
	//======================================
	typedef enum
	{
		BLOCK,//オブジェクト
		ENEMY,//敵
		ITEM,//アイテム
		POINT,//ポイント
		GIMMICK,//ギミック
		MAX_CATEGORY
	}CATEGORY;

	//======================================
	// 関数
	//======================================
	CXfile();//コンストラクタ
	~CXfile();//デストラクタ
	void Unload();//テクスチャ破棄
	int Regist(const char* pModelName);//モデル登録
	static void BindModel(int nCategory, int nType, CXfile::CModel* aModelInfo);//モデル情報割り当て
	static void BindTexture(int nCategory, int nType, LPDIRECT3DTEXTURE9 pTexture) { m_apSaveModel[nCategory][nType].m_pTexture = pTexture; }//テクスチャ割り当て
	static void LoadModelFile();//モデルファイルパス読込
	static void LoadTextureFile();//テクスチャファイルパス読込
	static void Regist();//登録処理
	static CModel* GetSaveModelInfo(int nCategory, int nType) { return &m_apSaveModel[nCategory][nType]; }//モデル保存情報取得
	CModel* GetAddress(int nIdx) { return m_aModel[nIdx]; }//モデル情報取得

private:
	//======================================
	// 変数
	//======================================
	static constexpr const char* MODEL_FILE = "data/ModelFile.txt";//モデルパス保存ファイル
	static constexpr const char* TEXTURE_FILE = "data/TextureFile.txt";//テクスチャパス保存ファイル
	static const int MAX_MODEL = 1000;//モデル情報最大数
	static CModel m_apSaveModel[MAX_CATEGORY][MAX_NUM_OBJECT];//保存用モデル情報
	const char* m_ModelFileName[MAX_MODEL];//作成済みXファイルパス保管用
	CModel* m_aModel[MAX_MODEL];//モデル情報
	int m_nAllModel = 0;//モデル総数
};

#endif
