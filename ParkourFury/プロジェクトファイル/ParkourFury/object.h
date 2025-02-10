//==============================================================================================================================================
//
// オブジェクトに関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _OBJECT_H_ //このマクロ定義がされなかったら
#define _OBJECT_H_ //2重インクルード防止のマクロ定義

//===========================================================================================================
// ファイルインクルード
//===========================================================================================================
#include "manager.h"
#include "texture.h"

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CObject
{
public:
	//======================================
	// 列挙型定義
	//======================================
	
	//カテゴリー
	enum class Category
	{
		NONE = 0,
		BG,//背景
		GIMMICK,//ギミック
		BULLET,//弾
		FIELD,//フィールド
		EDIT,//エディタオブジェクト
		BOSS,//ボス
		BLOCK,//ブロック
		PLAYER,//プレイヤー
		ENEMY,//エネミー
		AREA,//エリア
		BOSS_ATTACK,//ボス攻撃
		BACK_3D,//3D背景
		SIGN_BOARD,//看板
		EVENT,//イベント
		EFFECT,//エフェクト
		UI,//UI
		EFFECT_2D,//2Dエフェクト
		MANAGER,//マネージャー
		MAX_CATEGORY
	};

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
	static CObject* FindObject(Category category);//オブジェクト検索処理
	static void CorrectionRot(float& fRot);//角度補正処理
	static bool MovingJudgeXZ(const D3DXVECTOR3& move);//動いている判定処理
	template <typename T1, typename T2> static T1* DownCast(T2* pBase);//ダウンキャスト処理

	//頂点バッファ
	void SetVtxBuff(LPDIRECT3DVERTEXBUFFER9 pVtxBuff) { m_pVtxBuff = pVtxBuff; }//設定
	LPDIRECT3DVERTEXBUFFER9 GetVtxBuff() { return m_pVtxBuff; }//取得
	
	//テクスチャ情報
	void BindTexture(LPDIRECT3DTEXTURE9 pTexture) { m_pTexture = pTexture; }//設定
	LPDIRECT3DTEXTURE9 GetTexture() { return m_pTexture; }//取得

	//削除フラグ
	void SetDeleteFlag(bool bDelete) { m_bDelete = bDelete; }//設定
	bool GetDeleteFlag() { return m_bDelete; }//取得
	
	//先頭オブジェクト
	static CObject* GetTop(int nPriority) { return m_pTop[nPriority]; }//取得

	//次のオブジェクト情報
	CObject* GetNext() { return m_pNext; }//取得

	//カテゴリー
	Category GetCategory() { return m_category; }//取得

private:
	//======================================
	// 変数
	//======================================
	static CObject* m_pTop[static_cast<int>(Category::MAX_CATEGORY)];//先頭オブジェクトのポインタ
	static CObject* m_pCur[static_cast<int>(Category::MAX_CATEGORY)];//終端オブジェクトのポインタ
	CObject* m_pPrev;//前のオブジェクトのポインタ
	CObject* m_pNext;//次のオブジェクトのポインタ
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;//頂点バッファ
	LPDIRECT3DTEXTURE9 m_pTexture;//テクスチャ情報へのポインタ
	Category m_category;//種類
	int m_nPriority;//描画優先度順
	bool m_bDelete;//削除フラグ
};

//===========================================================================================================
// ダウンキャスト処理
//===========================================================================================================
template<typename T1, typename T2> inline T1* CObject::DownCast(T2* pBase)
{
	//引数のオブジェクトを指定されたクラスにダウンキャスト
	T1* pObj = dynamic_cast<T1*>(pBase) ;

	//ダウンキャスト失敗
	if (!pObj)
	{
		assert(false);
	}

	return pObj;
}

#endif
