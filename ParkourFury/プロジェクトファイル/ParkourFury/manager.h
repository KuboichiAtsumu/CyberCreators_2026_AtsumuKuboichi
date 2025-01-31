//==============================================================================================================================================
//
// マネージャーに関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _MANAGER_H_ //このマクロ定義がされなかったら
#define _MANAGER_H_ //2重インクルード防止のマクロ定義

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "scene.h"
#include "tag.h"
#include "debugproc.h"
#include "renderer.h"
#include "keyboard.h"
#include "mouse.h"
#include "joypad.h"
#include "camera.h"
#include "light.h"
#include "fade.h"
#include "sound.h"
#include "collision.h"
#include "calculate.h"

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CManager
{
private:
	//======================================
	// 関数
	//======================================
	CManager();//コンストラクタ
	~CManager();//デストラクタ

public:
	//======================================
	// ステージ情報ファイルパス
	//======================================
	static constexpr const char* StageFile[static_cast<int>(CScene::MODE::MODE_MAX)] =
	{
		"",//NONE
		"data/STAGE/GameStage.bin",//TITLE
		"data/STAGE/TutorialStage.bin",//TUTORIAL
		"data/STAGE/GameStage.bin",//GAME
		"",//RESULT
		"",//RANKING
	};

	//======================================
	// UI情報ファイルパス
	//======================================
	static constexpr const char* UIFile[static_cast<int>(CScene::MODE::MODE_MAX)] =
	{
		"",//NONE
		"data/UI/TitleUI.bin",//TITLE
		"data/UI/TutorialUI.bin",//TUTORIAL
		"data/UI/GameUI.bin",//GAME
		"data/UI/ResultUI.bin",//RESULT
		"data/UI/RankingUI.bin",//RANKING
	};

	//======================================
	// 関数
	//======================================
	static CManager* GetInstance();//インスタンスの取得
	static void Release();//インスタンスの破棄

	HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);//初期化処理
	void Uninit();//終了処理
	void Update();//更新処理
	void Draw();//描画処理
	void SetMode(CScene::MODE mode);//モードの設定

	//インスタンス取得
	CTag* GetTag() { return m_pTag; }//タグ
	CRenderer* GetRenderer() { return m_pRenderer; }//レンダラー
	CInputKeyboard* GetKeyboard() { return m_pKeyboard; }//キーボード
	CInputJoypad* GetJoypad() { return m_pJoypad; }//ジョイパッド
	CInputMouse* GetMouse() { return m_pMouse; }//マウス
	CSound* GetSound() { return m_pSound; }//サウンド
	CCamera* GetCamera() { return m_pCamera; }//カメラ
	CLight* GetLight() { return m_pLight; }//ライト
	CFade* GetFade() { return m_pFade; }//フェード
	CScene* GetScene() { return m_pScene; }//シーン
	CCollision* GetCollision() { return m_pCollision; }//当たり判定
	CCalculate* GetCalculate() { return m_pCalculate; }//計算処理

	//======================================
	// 定数
	//======================================
	static inline float FIELD_RANGE = 3200.0f;//フィールドの範囲
	static inline D3DXVECTOR3 VTXMIN_FIELD = { -FIELD_RANGE * 0.5f, 0.0f, -FIELD_RANGE * 0.5f };//フィールドの範囲の座標最小値
	static inline D3DXVECTOR3 VTXMAX_FIELD = { FIELD_RANGE * 0.5f, 0.0f, FIELD_RANGE * 0.5f };//フィールドの範囲の座標最大値

private:
	//======================================
	// 変数
	//======================================
	static CManager* m_pManager;//インスタンス
	CTag* m_pTag;//タグ
	CRenderer* m_pRenderer;//レンダラー
	CInputKeyboard* m_pKeyboard;//キーボード
	CInputJoypad* m_pJoypad;//Pad
	CInputMouse* m_pMouse;//マウス
	CSound* m_pSound;//サウンド
	CCamera* m_pCamera;//カメラ
	CLight* m_pLight;//ライト
	CScene* m_pScene;//現在の画面
	CFade* m_pFade;//フェード
	CCollision* m_pCollision;//当たり判定
	CCalculate* m_pCalculate;//計算処理
};

#endif