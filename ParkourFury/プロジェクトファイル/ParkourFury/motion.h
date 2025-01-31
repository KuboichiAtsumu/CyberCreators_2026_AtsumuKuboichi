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
	
	//モーションタイプ
	enum class TYPE
	{
		PLAYER,//プレイヤー
		BOSS,//ボス
		MAX_TYPE
	};

	//パーツ半身
	enum class HALF_BODY
	{
		UP_BODY,//上半身
		DOWN_BODY,//下半身
	};

	//=============================================================
	// 構造体
	//=============================================================
	
	//キー情報
	typedef struct
	{
		D3DXVECTOR3 pos;//座標
		D3DXVECTOR3 rot;//角度
	}Key;

	//キーセット情報
	typedef struct
	{
		std::vector<Key> aKey;//キーの情報の構造体
		int nMaxFrame;//最大フレーム数
	}KeySet;

	//モーション情報
	typedef struct
	{
		std::vector<KeySet> aKeyset;//パーツの情報
		TYPE type;//モーションの対象オブジェクトタイプ
		int nCntKeySet;//キー数カウンタ
		int nCntFrame;//フレーム数カウンタ
		int nMaxKeySet;//各モーションの最大キーセット数
		int nLoop;//各モーションがループするかどうか
	}MotionSet;

	//パーツ情報
	typedef struct
	{
		CXfile::ModelInfo m_aModelInfo;//各モデル情報
		D3DXMATRIX mtxWorld;//ワールドマトリックス
		D3DXVECTOR3 pos;//座標
		D3DXVECTOR3 FirstPos;//初期位置
		D3DXVECTOR3 WorldPos;//ワールド座標
		D3DXVECTOR3 rot;//角度
		HALF_BODY Half;//半身の判定
		int nIndex;//番号
		int nParent;//親の番号
		std::string aPartsFile;//パーツのファイル名
	}ModelParts;

	//=============================================================
	// 関数
	//=============================================================
	CMotionCharacter(int nPriority);//コンストラクタ
	~CMotionCharacter() override;//デストラクタ
	HRESULT Init() override;//初期化処理
	void Release() override;//解放処理
	void Update() override;//更新処理
	void Draw() override;//描画処理
	void DrawOutLine(int nParts, D3DXMATRIX mtxWorld);//アウトライン描画処理
	virtual void SetMotionState(int nState);//モーション状態設定処理
	virtual void SetPartsState();//パーツごとの状態設定処理
	void Motion(int nState, int nParts);//モーション処理
	void LoadParts();//パーツファイル読込処理
	void LoadMotion();//モーション情報読込処理

	//モーション情報
	void SetMotionSet(int nIdx, MotionSet motionset) { m_aMotion.at(nIdx) = motionset; }//設定
	MotionSet GetMotionSet(int nIdx) { return m_aMotion.at(nIdx); }//取得

	//モーションフラグ
	void SetMotionFlag(bool bMotion) { m_bMotion = bMotion; }//設定
	bool GetMotionFlag() { return m_bMotion; }//取得

	//パーツ情報
	void SetLocalPos(int nIdx, D3DXVECTOR3 pos) { m_aParts.at(nIdx).pos = pos; }//ローカル座標設定
	void SetWorldPos(int nIdx, D3DXVECTOR3 WorldPos) { m_aParts.at(nIdx).WorldPos = WorldPos; };//ワールド座標
	void SetPartsInfo(int nIdx, ModelParts aParts) { m_aParts.at(nIdx) = aParts; }//設定
	void SetPartsFlash(float a);//カラーα値設定
	void SetAllPartsSize();//全パーツを合わせたサイズ
	ModelParts GetPartsInfo(int nIdx) { return m_aParts.at(nIdx); }//取得
	int GetPartsSize() { return m_aParts.size(); }//パーツの数

	//タイプ
	void SetCharacterType(TYPE type) { m_Type = type; }//設定

	//中心座標
	D3DXVECTOR3& GetCenterPos() { return m_CenterPos; }//取得

	//状態
	int GetMotionState() { return m_nState; }//取得

	//速度
	float GetSpeed() { return m_fSpeed; }//取得

	//ジャンプ力
	float GetJump() { return m_fJump; }//取得

	//着地判定
	void SetLandingFlag(bool bLanding) { m_bLanding = bLanding; }//設定
	bool GetLandingFlag() { return m_bLanding; }//取得

private:
	//==========================
	// タイプごとの情報ファイル
	//==========================
	static constexpr const char* PARTS_FILE[static_cast<int>(TYPE::MAX_TYPE)] =
	{
		"data/MODEL/PLAYER/SetPlayer.txt",
		"data/MODEL/BOSS/SetBoss.txt",
	};

	//=============================================================
	// 関数
	//=============================================================
	void SetPartsInfo(CXfile::ModelInfo aModelInfo, int nParts);//パーツモデル情報設定処理
	void CorrectionPos();//座標の補正

	//=============================================================
	// 定数
	//=============================================================
	const float MAX_JUDGE_MOVE = 0.01f;//移動状態判定の移動量範囲
	const float OUTLINE_SCALE = 1.1f;//アウトラインの拡大率(太さ)

	//=============================================================
	// 変数
	//=============================================================
	std::vector<MotionSet> m_aMotion;//モーション情報
	std::vector<ModelParts> m_aParts;//各パーツ情報
	D3DXVECTOR3 m_CenterPos;//中心座標
	TYPE m_Type;//タイプ
	int m_nState;//現在のモーション
	float m_fSpeed;//速度
	float m_fJump;//ジャンプ力
	bool m_bLanding;//着地判定
	bool m_bMotion;//モーションするかどうかの判定
};

#endif
