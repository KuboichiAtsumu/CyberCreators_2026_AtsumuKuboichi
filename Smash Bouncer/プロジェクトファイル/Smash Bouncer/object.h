//==============================================================================================================================================
//
// オブジェクトに関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _OBJECT_H_ //このマクロ定義がされなかったら
#define _OBJECT_H_ //2重インクルード防止のマクロ定義

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "manager.h"//マネージャー

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CObject
{
public:
	//======================================
	// 列挙型定義
	//======================================

	//種類
	typedef enum
	{
		NONE = 0,
		EDIT,//エディタオブジェクト
		PLAYER,//プレイヤー
		ENEMY,//敵
		BULLET,//弾
		BILLBOARD,//ビルボード
		SCORE,//スコア
		BLOCK,//ブロック
		ITEM,//アイテム
		FIELD,//フィールド
		FADE,//フェード
		ZONE,//ゾーン
		SHOTPOINT,//ポイント
		GIMMICK,//ギミック
		UI,//UI
		EFFECT,//エフェクト
		MAX_OBJECT_TYPE
	}TYPE;

	//======================================
	// 関数
	//======================================
	CObject(int nPriority = 3);//コンストラクタ
	virtual ~CObject();//デストラクタ
	virtual HRESULT Init();//初期化処理
	virtual void Uninit() = 0;//終了処理
	virtual void Release();//解放処理
	virtual void Update() = 0;//更新処理
	virtual void Draw() = 0;//描画処理
	static void UninitAll();//全オブジェクト終了処理
	static void ReleaseAll();//全オブジェクト解放処理
	static void UpdateAll();//全オブジェクト更新処理
	static void DrawAll();//全オブジェクト描画処理

	//情報の設定
	void SetVtxBuff(LPDIRECT3DVERTEXBUFFER9 pVtxBuff) { m_pVtxBuff = pVtxBuff; }//頂点バッファ
	void BindTexture(LPDIRECT3DTEXTURE9 pTexture) { m_pTexture = pTexture; }//テクスチャ割り当て
	void SetType(TYPE type) { m_type = type; }//種類
	void SetTextureIdx(int nIdx) { m_nTextureIdx = nIdx; }//テクスチャ番号
	void SetDeleteFlag(bool bDelete) { m_bDelete = bDelete; }//削除フラグ

	//情報の取得
	LPDIRECT3DVERTEXBUFFER9 GetVtxBuff() { return m_pVtxBuff; }//頂点バッファ
	LPDIRECT3DTEXTURE9 GetTexture() { return m_pTexture; }//テクスチャ情報
	TYPE GetType() { return m_type; }//種類
	static CObject* GetTop(int nPriority) { return m_pTop[nPriority]; }//先頭オブジェクト
	CObject* GetNext() { return m_pNext; }//次のオブジェクト情報
	bool GetDeleteFlag() { return m_bDelete; }//削除フラグ

private:
	//======================================
	// 変数
	//======================================
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;//頂点バッファ
	LPDIRECT3DTEXTURE9 m_pTexture;//テクスチャ情報へのポインタ
	static CObject* m_pTop[MAX_PRIORITY];//先頭オブジェクトのポインタ
	static CObject* m_pCur[MAX_PRIORITY];//終端オブジェクトのポインタ
	CObject* m_pPrev;//前のオブジェクトのポインタ
	CObject* m_pNext;//次のオブジェクトのポインタ
	TYPE m_type;//種類
	int m_nPriority;//描画優先度順
	int m_nTextureIdx;//テクスチャID
	bool m_bDelete;//削除フラグ
};

#endif