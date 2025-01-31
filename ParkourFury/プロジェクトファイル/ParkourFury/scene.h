//==============================================================================================================================================
//
// シーンに関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _SCENE_H_ //このマクロ定義がされなかったら
#define _SCENE_H_ //2重インクルード防止のマクロ定義

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "main.h"

//===========================================================================================================
// 前方宣言
//===========================================================================================================
class CEdit2D;
class CEdit3DManager;
class CBlockManager;
class CGimmickManager;
class CBulletManager;
class CEnemyManager;
class CAreaManager;
class CUIManager;
class CSignBoardManager;

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CScene
{
public:
	//======================================
	// 列挙型定義
	//======================================

	//モード
	enum class MODE
	{
		MODE_NONE = 0,
		MODE_TITLE,//タイトル
		MODE_TUTORIAL,//チュートリアル
		MODE_GAME,//ゲーム
		MODE_RESULT,//リザルト
		MODE_RANKING,//ランキング
		MODE_MAX
	};

	//シーン
	enum class SCENE
	{
		GAME = 0,//ゲーム
		EDIT_3D,//3Dエディタ
		EDIT_2D,//2Dエディタ
	};

	//======================================
	// 構造体定義
	//======================================
	struct ResultScore
	{
		int nGoalTime;//ゴールタイム
		float fAverageHeat;//平均ヒート量
		float fNumKillenemy;//倒した敵の数(割合)
	};

	//======================================
	// 関数
	//======================================
	CScene();//コンストラクタ
	virtual ~CScene();//デストラクタ
	virtual HRESULT Init();//初期化処理
	virtual void Uninit();//終了処理
	virtual void Update();//更新処理
	virtual void Draw();//描画処理
	static CScene* Create(MODE mode);//生成処理

	//3Dエディタマネージャー
	void SetEditManager(CEdit3DManager* pManager) { m_pEditManager = pManager; }//設定
	CEdit3DManager* GetEditManager() { return m_pEditManager; }//取得

	//ブロックマネージャー
	void SetBlockManager(CBlockManager* pManager) { m_pBlockManager = pManager; }//設定
	CBlockManager* GetBlockManager() { return m_pBlockManager; }//取得

	//エネミーマネージャー
	void SetEnemyManager(CEnemyManager* pManager) { m_pEnemyManager = pManager; }//設定
	CEnemyManager* GetEnemyManager() { return m_pEnemyManager; }//取得

	//ギミックマネージャー
	void SetGimmickManager(CGimmickManager* pManager) { m_pGimmickManager = pManager; }//設定
	CGimmickManager* GetGimmickManager() { return m_pGimmickManager; }//取得

	//バレットマネージャー
	void SetBulletManager(CBulletManager* pManager) { m_pBulletManager = pManager; }//設定
	CBulletManager* GetBulletManager() { return m_pBulletManager; }//取得

	//エリアマネージャー
	void SetAreaManager(CAreaManager* pManager) { m_pAreaManager = pManager; }//設定
	CAreaManager* GetAreaManager() { return m_pAreaManager; }//取得

	//UIマネージャー
	void SetUIManager(CUIManager* pManager) { m_pUIManager = pManager; }//設定
	CUIManager* GetUIManager() { return m_pUIManager; }//取得

	//看板マネージャー
	void SetSignBoardManager(CSignBoardManager* pManager) { m_pSignBoardManager = pManager; }//設定
	CSignBoardManager* GetSignBoardManager() { return m_pSignBoardManager; }//取得

	//リザルト用スコア
	void SetResultScore(ResultScore aResultScore) { m_aResultScore = aResultScore; }//設定
	ResultScore GetResultScore() { return m_aResultScore; }//取得

	//モード情報
	void SetMode(MODE mode) { m_mode = mode; }//設定
	MODE GetMode() { return m_mode; }//取得

	//現在のシーン
	void SetScene(SCENE scene) { m_Scene = scene; }//設定
	SCENE GetSceneState() { return m_Scene; }//取得

private:
	//======================================
	// 変数
	//======================================

	//インスタンス
	CEdit2D* m_pEdit2D;//2Dエディタのインスタンス
	CEdit3DManager* m_pEditManager;//エディタマネージャー
	CBlockManager* m_pBlockManager;//ブロックマネージャー
	CGimmickManager* m_pGimmickManager;//ギミックマネージャー
	CBulletManager* m_pBulletManager;//バレットマネージャー
	CEnemyManager* m_pEnemyManager;//エネミーマネージャー
	CAreaManager* m_pAreaManager;//エリアマネージャー
	CUIManager* m_pUIManager;//UIマネージャー
	CSignBoardManager* m_pSignBoardManager;//看板マネージャー

	static ResultScore m_aResultScore;//リザルト用スコア
	MODE m_mode;//モード
	SCENE m_Scene;//現在のシーン
};

#endif
