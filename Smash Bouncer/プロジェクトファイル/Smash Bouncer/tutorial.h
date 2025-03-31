//==============================================================================================================================================
//
// チュートリアル画面に関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _TUTORIAL_H_ //このマクロ定義がされなかったら
#define _TUTORIAL_H_ //2重インクルード防止のマクロ定義

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "manager.h"//マネージャー

//===========================================================================================================
// 前方宣言
//===========================================================================================================
class CBlockManager;//ブロックマネージャー
class CEnemyManager;//エネミーマネージャー
class CBulletManager;//バレットマネージャー

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CTutorial : public CScene
{
public:
	//======================================
	// 関数
	//======================================
	CTutorial();//コンストラクタ
	~CTutorial() override;//デストラクタ
	HRESULT Init() override;//初期化処理
	void Uninit() override;//終了処理
	void Update() override;//更新処理
	void Draw() override;//描画処理
	static CTutorial* Create();//生成処理
	void SetSpawnTime() { m_nSpawnTime = SPAWN_INTERVAL; }//エネミースポーンまでの時間設定処理
	static CBlockManager* GetBlockManager() { return m_pBlockManager; }//ブロックマネージャー情報
	static CEnemyManager* GetEnemyManager() { return m_pEnemyManager; }//エネミーマネージャー情報
	static CBulletManager* GetBulletManager() { return m_pBulletManager; }//バレットマネージャー情報

private:
	//======================================
	// 変数
	//======================================
	const D3DXVECTOR3 ENEMY_SPAWN_POS = { 120.0f, 0.0f, 0.0f };//エネミーのスポーン位置
	const D3DXVECTOR3 ENEMY_SPAWN_ROT = { 0.0f, D3DX_PI * 0.5f, 0.0f };//エネミーのスポーン角度
	const int SPAWN_INTERVAL = 30;//エネミースポーンまでの時間
	static constexpr float FIRST_SPAWN = -160.0f;//初期スポーン位置
	static CBlockManager* m_pBlockManager;//ブロックマネージャー情報
	static CEnemyManager* m_pEnemyManager;//エネミーマネージャー情報
	static CBulletManager* m_pBulletManager;//バレットマネージャー情報
	int m_nSpawnTime;//エネミースポーンまでのカウント
};

#endif
