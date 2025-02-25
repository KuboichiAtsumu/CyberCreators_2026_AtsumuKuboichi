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
#include "object3D.h"
#include "xfile.h"

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CObjectX : public CObject3D
{
public:
	//=============================================================
	// 列挙型定義
	//=============================================================

	//座標の線上
	enum class STAND_LINE
	{
		XY,
		YZ,
	};

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

	//モデル情報
	void BindModel(CXfile::ModelInfo* pModelInfo) { m_aModelInfo = *pModelInfo; }//設定
	CXfile::ModelInfo GetModelInfo() { return m_aModelInfo; }//取得

	//シェーダー情報
	void SetEffect(ID3DXEffect* pEffect) { m_pEffect = pEffect; }//設定
	ID3DXEffect* GetEffect() { return m_pEffect; }//取得

	//最小頂点情報
	void SetVtxMin(const D3DXVECTOR3& vtxMin) { m_vtxMin = vtxMin; }//設定
	D3DXVECTOR3& GetVtxMin() { return m_vtxMin; }//取得

	//最大頂点情報
	void SetVtxMax(const D3DXVECTOR3& vtxMax) { m_vtxMax = vtxMax; }//設定
	D3DXVECTOR3& GetVtxMax() { return m_vtxMax; }//取得

	//サイズ情報
	void SetSize();//設定
	void SetUpdateSize(const D3DXVECTOR3& size) { m_Size = size; }//更新されたサイズ情報を設定
	D3DXVECTOR3& GetSize() { return m_Size; }//取得

	//拡大率情報
	void SetScale(const D3DXVECTOR3& scale) { m_Scale = scale; }//設定
	D3DXVECTOR3& GetScale() { return m_Scale; }//取得

	//アウトライン拡大率
	void SetOutLineScale(const D3DXVECTOR3& OutLineScale) { m_OutLineScale = OutLineScale; }//設定

	//アウトラインカラー
	void SetOutLineColor(const D3DXCOLOR& OutLineColor) { m_OutLineColor = OutLineColor; }//設定

	//座標の線上
	STAND_LINE GetStandLine() { return m_StandLine; }//取得

	//対角線の長さ
	void SetLength(float fLength) { m_fLength = fLength; }//設定

	//対角線の角度
	void SetAngle(float fAngle) { m_fAngle = fAngle; }//設定

	//アウトライン描画フラグ
	void SetOutLineFlag(bool bOutLine) { m_bOutLine = bOutLine; }//設定

private:
	//======================================
	// 関数
	//======================================
	void SetShader(const D3DXMATRIX& mtxWorld);//シェーダーパラメータ設定
	void SetStandLine();//座標の線上の設定

	//======================================
	// 変数
	//======================================
	ID3DXEffect* m_pEffect;//シェーダー情報
	CXfile::ModelInfo m_aModelInfo;//各モデル情報
	D3DXVECTOR3 m_vtxMin;//最小頂点座標
	D3DXVECTOR3 m_vtxMax;//最大頂点座標
	D3DXVECTOR3 m_Size;//サイズ
	D3DXVECTOR3 m_Scale;//拡大率
	D3DXVECTOR3 m_OutLineScale;//アウトライン拡大率
	D3DXCOLOR m_OutLineColor;//アウトラインカラー
	STAND_LINE m_StandLine;//座標の線上
	float m_fLength;//対角線の長さ
	float m_fAngle;//対角線の角度
	bool m_bOutLine;//アウトライン描画フラグ
};

#endif