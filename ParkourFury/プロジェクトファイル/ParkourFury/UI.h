//==============================================================================================================================================
//
// UIに関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _UI_H_ //このマクロ定義がされなかったら
#define _UI_H_ //2重インクルード防止のマクロ定義

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "object2D.h"

//===========================================================================================================
// クラス定義
//===========================================================================================================
class CUI : public CObject2D
{
public:
	//======================================
	// 列挙型定義
	//======================================

	//タイプ
	enum class TYPE
	{
		NONE = 0,
		HEAT_GAGE_FRAME,//ヒートゲージフレーム
		HEAT_GAGE,//ヒートゲージ
		TIME_NUMBER,//タイマー
		TIMER_HEADING,//タイマー見出し
		MINIGAME_BACK,//ミニゲームタイマー背景
		MINIGAME_GAGE,//ミニゲームタイマーゲージ
		MINIGAME_FRAME,//ミニゲームタイマーフレーム
		TITLE_LOGO,//タイトルロゴ
		START_BUTTON,//スタートボタン
		RESULT_HEAD,//リザルト見出し
		RESULT_TIME_HEAD,//リザルトタイム見出し
		RESULT_KILL_HEAD,//リザルトキル数見出し
		RESULT_HEAT_HEAD,//リザルトキル数見出し
		SECONDS,//秒数単位
		PERCENT,//パーセントマーク
		MAXSCORE,//スコア最大値
		RESULT_TIME,//リザルトタイム
		RESULT_KILL,//リザルトキル数
		RESULT_HEAT,//リザルト平均ヒート量
		TOTAL_SCORE,//トータルスコア
		TUTORIAL_HEAD,//チュートリアル見出し
		DESCRIPT_HEAT,//ヒート説明
		MINIGAME_HEAD,//ミニゲーム見出し
		ESCAPE,//チェイスエリア警告
		BOSS_HP,//ボスHP
		BOSS_HP_FRAME,//ボスHPのフレーム
		DESCRIPT_GAGE,//ゲージ説明
		MAX
	};

	//======================================
	// 関数
	//======================================
	CUI(int nPriority = static_cast<int>(Category::UI));//コンストラクタ
	~CUI() override;//デストラクタ
	HRESULT Init() override;//初期化処理
	void Uninit() override;//終了処理
	void Update() override;//更新処理
	void Draw() override;//描画処理
	static CUI* Create(TYPE type, const D3DXVECTOR2& pos, const D3DXVECTOR2& size, const D3DXVECTOR3& rot);//生成処理

	//前のオブジェクト情報
	void SetPrevUI(CUI* pUI) { m_pPrev = pUI; }//設定
	CUI* GetPrevUI() { return m_pPrev; }//決定

	//次のオブジェクト情報
	void SetNextUI(CUI* pUI) { m_pNext = pUI; }//設定
	CUI* GetNextUI() { return m_pNext; }//取得

	//タイプ
	void SetUIType(TYPE type) { m_Type = type; }//設定
	TYPE GetUIType() { return m_Type; }//取得

	//インデックス
	void SetIdx(int nIdx) { m_nIdx = nIdx; }//設定
	int GetIdx() { return m_nIdx; }//取得

	//エディット可能フラグ
	void SetEditFlag(bool bEdit) { m_bEdit = bEdit; }//設定
	bool GetEditFlag() { return m_bEdit; }//取得

	//======================================
	// 変数
	//======================================
	static int m_nAll;//総数

private:
	//======================================
	// 各タイプテクスチャファイルパス
	//======================================
	static constexpr const char* TEXTURE_FILE[static_cast<int>(TYPE::MAX)] =
	{
		"",//NONE
		"data/TEXTURE/UI/HeatGageFrame.png",//ヒートゲージフレーム
		"data/TEXTURE/UI/HeatGage.png",//ヒートゲージ
		"data/TEXTURE/UI/Number.png",//ゲームタイマー用ナンバー
		"data/TEXTURE/UI/TimerHeading.png",//タイマー見出し
		"data/TEXTURE/UI/MiniGameTimerGageBack.png",//ミニゲームタイマー背景
		"data/TEXTURE/UI/MiniGameTimerGage.png",//ミニゲームタイマー
		"data/TEXTURE/UI/MiniGameTimerGageFrame.png",//ミニゲームタイマーフレーム
		"data/TEXTURE/UI/TitleLogo.png",//タイトルロゴ
		"data/TEXTURE/UI/pushA.png",//スタートボタン
		"data/TEXTURE/UI/ResultHead.png",//リザルト見出し
		"data/TEXTURE/UI/ResultTimeHead.png",//リザルトタイム見出し
		"data/TEXTURE/UI/ResultKillHead.png",//リザルトキル数見出し
		"data/TEXTURE/UI/ResultHeatHead.png",//リザルト平均ヒート量見出し
		"data/TEXTURE/UI/Seconds.png",//秒数単位
		"data/TEXTURE/UI/Percent.png",//パーセントマーク
		"data/TEXTURE/UI/ResultMaxScore.png",//スコア最大値
		"data/TEXTURE/UI/Number.png",//リザルトタイマー用ナンバー
		"data/TEXTURE/UI/Number.png",//リザルトキル数用ナンバー
		"data/TEXTURE/UI/Number.png",//リザルト平均ヒート量用ナンバー
		"data/TEXTURE/UI/Number.png",//トータルスコア用ナンバー
		"data/TEXTURE/UI/TutorialHead.png",//チュートリアル見出し
		"data/TEXTURE/UI/DescriptHeat.png",//ヒート説明
		"data/TEXTURE/UI/MiniGameHead.png",//ミニゲーム見出し
		"data/TEXTURE/UI/escape.png",//チェイスエリア警告
		"",//ボスHP(テクスチャ無し)
		"data/TEXTURE/UI/BossHPFrame.png",//ボスHPフレーム
		"data/TEXTURE/UI/DescriptGage.png",//ゲージ説明
	};

	//======================================
	// 変数
	//======================================
	CUI* m_pPrev;//前のオブジェクト情報へのポインタ
	CUI* m_pNext;//次のオブジェクト情報へのポインタ
	TYPE m_Type;//タイプ
	int m_nIdx;//インデックス
	bool m_bEdit;//エディット可能フラグ
};

#endif
