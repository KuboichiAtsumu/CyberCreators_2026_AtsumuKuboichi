#ifdef _DEBUG
//==============================================================================================================================================
//
// 3Dエディタに関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _EDIT3D_H_
#define _EDIT3D_H_

//===========================================================================================================
// ファイルインクルード
//===========================================================================================================
#include "objectx.h"

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CEdit3D : public CObjectX
{
public:
	//======================================
	// 関数
	//======================================
	CEdit3D(int nPriority = 0);//コンストラクタ
	~CEdit3D();//デストラクタ
	HRESULT Init();//初期化処理
	void Uninit();//終了処理
	void Release();//解放処理
	void Update();//更新処理
	void Draw();//描画処理
	static CEdit3D* Create(CEdit3D aEdit);//生成処理
	static void EditUpdate();//エディタ情報更新処理

	//前のオブジェクト情報
	void SetPrevEdit(CEdit3D* pEdit) { m_pPrev = pEdit; }//設定
	CEdit3D* GetPrevEdit() { return m_pPrev; }//決定

	//次のオブジェクト情報
	void SetNextEdit(CEdit3D* pEdit) { m_pNext = pEdit; }//設定
	CEdit3D* GetNextEdit() { return m_pNext; }//取得

	//タグ
	void SetTag(CTag::TAG tag) { m_tag = tag; }//設定

	//タイプ
	void SetType(int nType) { m_nType = nType; }//設定

	//インデックス
	void SetIdx(int nIdx) { m_nIdx = nIdx; }//設定
	int GetIdx() { return m_nIdx; }//取得

private:
	//======================================
	// 列挙型定義
	//======================================

	//操作モード
	enum class MODE
	{
		MODE_MOVE,//移動
		MODE_SCALE,//拡大
	};

	//======================================
	// 関数
	//======================================
	static void Save();//セーブ処理
	void SetCameraPos();//カメラの設定
	void Move();//移動処理
	void Scale();//拡大処理
	void Turn();//回転処理
	void Reset();//リセット処理
	void Change();//カテゴリー・タイプ変更処理

	//======================================
	// 定数
	//======================================
	const int TURN_DIRECTION = 4;//回転方向分割数
	const float MOVE = 1.0f;//移動量
	const float SCALE = 0.1f;//拡大量
	const float TURN = D3DX_PI * (1.0f / static_cast<float>(TURN_DIRECTION));//回転量

	//======================================
	// 変数
	//======================================
	static MODE m_mode;//操作モード
	static int m_nAll;//配置数
	static int m_nTarget;//編集対象番号
	static bool m_bTrigger;//移動方法がトリガーかどうか
	CEdit3D* m_pPrev;//前のオブジェクト情報へのポインタ
	CEdit3D* m_pNext;//次のオブジェクト情報へのポインタ
	CTag::TAG m_tag;//タグ
	int m_nIdx;//インデックス
	int m_nType;//タイプ
};

#endif
#endif // _DEBUG
