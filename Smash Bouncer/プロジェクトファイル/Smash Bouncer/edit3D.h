//==============================================================================================================================================
//
// 3Dエディタに関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _EDIT3D_H_
#define _EDIT3D_H_

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "manager.h"
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
	static void UninitAll();//全オブジェクト解放処理
	void Uninit();//終了処理
	void Release();//解放処理
	void Update();//更新処理
	static void EditUpdate();//エディタ情報更新処理
	void Draw();//描画処理
	static CEdit3D* Create();//生成処理
	static void LoadInfo(const char* FileName);//ロード処理
	
	//======================================
	// 変数
	//======================================
	static constexpr int MAX_EDIT3D = 1000;//オブジェクトの最大数

private:
	//======================================
	// 列挙型定義
	//======================================

	//操作モード
	typedef enum
	{
		MODE_MOVE,//移動
		MODE_SCALE,//拡大
	}EDIT_MODE;

	//======================================
	// 関数
	//======================================
	static void Save();//セーブ処理
	static void SetCameraPos();//カメラの設定
	void Move();//移動処理
	void Scale();//拡大処理
	
	//======================================
	// 変数
	//======================================

	//静的メンバ変数
	static constexpr int TURN_DIRECTION = 8;//回転方向分割数
	static constexpr float MOVE = 5.0f;//移動量
	static constexpr float SCALE = 0.1f;//拡大量
	static constexpr float TURN = D3DX_PI * (1.0f / (float)TURN_DIRECTION);//回転量
	static CEdit3D* m_apEditObj[MAX_EDIT3D];//オブジェクト情報
	static EDIT_MODE m_mode;//操作モード
	static int m_nTarget;//編集対象番号
	static int m_nNumAll;//合計データ数
	static bool m_bTrigger;//移動方法がトリガーかどうか

	//メンバ変数
	CXfile::CModel* m_apModel;//モデルの情報
	D3DXMATRIX m_mtxWorld;//ワールドマトリックス
	D3DXVECTOR3 m_pos;//位置
	D3DXVECTOR3 m_rot;//向き
	D3DXVECTOR3 m_Scale;//拡大率
	int m_nCategory;//カテゴリー
	int m_nType;//種類
	int m_nID;//各オブジェクトのアドレス
	bool m_bUse;//使用しているかどうか
};

#endif
