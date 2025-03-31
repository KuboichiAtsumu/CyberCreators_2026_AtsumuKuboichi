//==============================================================================================================================================
//
// モーションに関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _MOTION_H_ //このマクロ定義がされなかったら
#define _MOTION_H_ //2重インクルード防止のマクロ定義

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "objectX.h"//Xファイルオブジェクト

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CMotionCharacter : public CObjectX
{
public:
	//=============================================================
	// 列挙型定義
	//=============================================================

	//==========================
	// 状態
	//==========================
	typedef enum
	{
		NEUTRAL = 0,//ニュートラル
		MOVE,//移動
		ACTION,//アクション
		MAX_STATE
	}MOTION_STATE;

	//==========================
	// モーションのタイプ
	//==========================
	typedef enum
	{
		PLAYER,//プレイヤー
		MAX_TYPE
	}MOTION_TYPE;

	//=============================================================
	// 関数
	//=============================================================
	CMotionCharacter(int nPriority = 3);//コンストラクタ
	~CMotionCharacter() override;//デストラクタ
	void Release() override;//解放処理
	void Update() override;//更新処理
	void Draw() override;//描画処理
	void LoadParts(MOTION_TYPE type);//パーツファイル読込処理
	void LoadMotion(MOTION_TYPE type);//モーション情報読込処理

	//情報の設定
	void SetState(MOTION_STATE state) { m_State = state; }//状態情報
	void SetPartsFlash(float a);//パーツのカラー
	void SetAllPartsSize();//全パーツを合わせたサイズ

	//情報の取得
	MOTION_STATE GetState() { return m_State; }//現在の状態
	float GetSpeed() { return m_fSpeed; }//速度の情報を取得

	//=============================================================
	// 静的メンバ変数
	//=============================================================
	static constexpr float MAX_JUDGE_MOVE = 0.1f;//移動状態判定の移動量範囲

private:
	//=============================================================
	// 列挙型定義
	//=============================================================

	//==========================
	// パーツの半身
	//==========================
	typedef enum
	{
		UP_BODY,//上半身
		DOWN_BODY,//下半身
		MAX_HALF
	}HALF_BODY;

	//==========================
	// タイプごとの情報ファイル
	//==========================
	static constexpr const char* PARTS_FILE[MAX_TYPE] =
	{
		"data/MODEL/PLAYER/SetPlayer.txt",
	};

	//=============================================================
	// 関数
	//=============================================================
	void Motion(MOTION_STATE state, int nParts, int nKeySet);//モーション処理
	void SetPartsInfo(CXfile::CModel aModelInfo, int nParts);//パーツモデル情報設定処理
	
	//=============================================================
	// 静的メンバ変数
	//=============================================================
	static constexpr int MAX_PARTS = 20;//最大パーツ数
	static constexpr int MAX_MOTION = 5;//最大モーション数
	static constexpr int MAX_KEYSET = 10;//最大キーセット数
	static constexpr int MAX_MAT = 100;//最大マテリアル数
	static constexpr int MAX_TEXT = 256;//最大テキスト読込バイト数

	//=============================================================
	// 構造体
	//=============================================================
	
	//==========================
	// 各キーの情報
	//==========================
	typedef struct
	{
		D3DXVECTOR3 pos;//位置
		D3DXVECTOR3 rot;//向き
	}Key;

	//==========================
	// 各キーセットの情報
	//==========================
	typedef struct
	{
		Key aKey[MAX_PARTS];//キーの情報の構造体
		int nMaxFrame;//最大フレーム数
	}KeySet;

	//==========================
	// 各モーションの情報
	//==========================
	typedef struct
	{
		KeySet aKeyset[MAX_KEYSET];//パーツの情報
		MOTION_TYPE type;//モーションの対象オブジェクトタイプ
		int nCntKeySet;//キー数カウンタ
		int nCntFrame;//フレーム数カウンタ
		int nMaxKeySet;//各モーションの最大キーセット数
		int nLoop;//各モーションがループするかどうか
	}MotionSet;

	//==========================
	// 各パーツの情報
	//==========================
	typedef struct
	{
		LPD3DXMESH pMesh;//メッシュの頂点情報へのポインタ
		LPD3DXBUFFER pBuffMat;//マテリアルへのポインタ
		DWORD dwNumMat;//マテリアルの数
		D3DXMATRIX mtxWorld;//ワールドマトリックス
		D3DCOLORVALUE Diffuse[MAX_MAT];//マテリアル情報
		D3DCOLORVALUE FirstDiffuse[MAX_MAT];//マテリアル情報初期値
		D3DXVECTOR3 pos;//位置
		D3DXVECTOR3 rot;//向き
		MOTION_TYPE type;//モーションの対象オブジェクトタイプ
		int nIndex;//番号
		int nParent;//親の番号
		int nHalf;//半身の判定
		char aPartsFile[MAX_TEXT];//パーツのファイル名
	}ModelParts;

	//=============================================================
	// メンバ変数
	//=============================================================
	MotionSet m_aMotion[MAX_MOTION];//モーション情報の構造体
	ModelParts m_aParts[MAX_PARTS];//各パーツ情報の構造体
	MOTION_STATE m_State;//現在の状態
	int m_nParts;//パーツ数
	int m_nMotion;//モーション数
	float m_fSpeed;//速度
};

#endif
