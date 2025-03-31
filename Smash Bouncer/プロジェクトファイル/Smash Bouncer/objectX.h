//==============================================================================================================================================
//
// 3Dオブジェクトに関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _OBJECTX_H_ //このマクロ定義がされなかったら
#define _OBJECTX_H_ //2重インクルード防止のマクロ定義

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "object3D.h"//3Dオブジェクト
#include "xfile.h"//Xファイル管理

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CObjectX : public CObject3D
{
public:
	//======================================
	// 関数
	//======================================
	CObjectX(int nPriority = 3);//コンストラクタ
	~CObjectX() override;//デストラクタ
	HRESULT Init() override;//初期化処理
	void Uninit() override;//終了処理
	void Release() override;//解放処理
	void Update() override;//更新処理
	void Draw() override;//描画処理
	void BindModel(CXfile::CModel* aModelInfo) { m_aModelInfo = *aModelInfo; }//モデル割り当て
	void SetModelIdx(int nIdx) { m_nModelIdx = nIdx; }//モデルID取得
	CXfile::CModel GetModelInfo() { return m_aModelInfo; }//モデル情報取得

	//パラメータ設定
	void SetSize();//サイズ
	void SetScale(D3DXVECTOR3 scale) { m_Scale = scale; }//拡大率
	void SetUpdateSize(D3DXVECTOR3 size) { m_Size = size; }//更新されたサイズ情報を設定

	//パラメータ取得
	D3DXVECTOR3 GetSize() { return m_Size; }//サイズ
	D3DXVECTOR3 GetScale() { return m_Scale; }//拡大率

	//======================================
	// 変数
	//======================================
	static int m_nNumType[CXfile::MAX_CATEGORY];//各カテゴリーのタイプ数

private:
	//======================================
	// 変数
	//======================================
	int m_nModelIdx;//モデルのID
	D3DXVECTOR3 m_vtxMin;//最小頂点座標
	D3DXVECTOR3 m_vtxMax;//最大頂点座標
	D3DXVECTOR3 m_Size;//サイズ
	D3DXVECTOR3 m_Scale;//拡大率
	CXfile::CModel m_aModelInfo;//各モデル情報
};

#endif