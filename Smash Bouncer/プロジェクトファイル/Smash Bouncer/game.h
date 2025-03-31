//==============================================================================================================================================
//
// ゲーム画面に関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _GAME_H_ //このマクロ定義がされなかったら
#define _GAME_H_ //2重インクルード防止のマクロ定義

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "manager.h"//マネージャー

//===========================================================================================================
// 前方宣言
//===========================================================================================================
class CBlockManager;//ブロックマネージャー
class CEnemyManager;//エネミーマネージャー
class CItemManager;//アイテムマネージャー
class CPointManager;//ポイントマネージャー
class CGimmickManager;//ギミックマネージャー
class CBulletManager;//バレットマネージャー
class CScore;//スコア情報

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CGame : public CScene
{
public:
	//======================================
	// 列挙型定義
	//======================================

	//ゲームの状態
	typedef enum
	{
		STATE_GAME,//ゲーム中
		STATE_MOVE,//ステージ移動中
		MAX_STATE
	}GAMESTATE;

	//ステージ
	typedef enum
	{
		STAGE_1,//ステージ1
		STAGE_2,//ステージ2
		STAGE_3,//ステージ3
		STAGE_4,//ステージ4
		STAGE_5,//ステージ5
		STAGE_6,//ステージ6
		STAGE_7,//ステージ7
		STAGE_8,//ステージ8
		STAGE_9,//ステージ9
		STAGE_10,//ステージ10
		MAX_STAGE
	}STAGE;

	//======================================
	// 関数
	//======================================
	CGame();//コンストラクタ
	~CGame() override;//デストラクタ
	HRESULT Init() override;//初期化処理
	void Uninit() override;//終了処理
	void Update() override;//更新処理
	void Draw() override;//描画処理
	static CGame* Create();//生成処理

	//情報の取得
	static CBlockManager* GetBlockManager() { return m_pBlockManager; }//ブロックマネージャー情報
	static CEnemyManager* GetEnemyManager() { return m_pEnemyManager; }//エネミーマネージャー情報
	static CItemManager* GetItemManager() { return m_pItemManager; }//アイテムマネージャー情報
	static CPointManager* GetPointManager() { return m_pPointManager; }//ポイントマネージャー情報
	static CGimmickManager* GetGimmickManager() { return m_pGimmickManager; }//ギミックマネージャー情報
	static CBulletManager* GetBulletManager() { return m_pBulletManager; }//バレットマネージャー情報
	static CScore* GetScore() { return m_pScore; }//スコア情報
	static STAGE GetNumStage() { return m_Stage; }//現在のステージ
	static GAMESTATE GetGameState() { return m_GameState; }//ゲームの状態

	//情報の設定
	static void SetNumStage(int m_nStage) { m_Stage = (STAGE)m_nStage; }//現在のステージ
	static void SetGameState(GAMESTATE state);//ゲームの状態

private:
	//======================================
	// 変数
	//======================================
#ifdef _DEBUG
	static constexpr int SPAWN_STAGE = STAGE_1;//スポーンするステージ
#endif // _DEBUG
	static constexpr float FIRST_SPAWN = -160.0f;//初期スポーン位置
	static CBlockManager* m_pBlockManager;//ブロックマネージャー情報
	static CEnemyManager* m_pEnemyManager;//エネミーマネージャー情報
	static CItemManager* m_pItemManager;//アイテムマネージャー情報
	static CPointManager* m_pPointManager;//ポイントマネージャー情報
	static CGimmickManager* m_pGimmickManager;//ギミックマネージャー情報
	static CBulletManager* m_pBulletManager;//バレットマネージャー情報
	static CScore* m_pScore;//スコア情報
	static STAGE m_Stage;//現在のステージ
	static GAMESTATE m_GameState;//ゲームの状態
};

#endif
