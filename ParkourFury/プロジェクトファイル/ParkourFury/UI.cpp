//==============================================================================================================================================
//
// UIに関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "UI.h"
#include "UImanager.h"
#include "heatgageframe.h"
#include "heatgage.h"
#include "timenumber.h"
#include "timerheading.h"
#include "mg_timerback.h"
#include "mg_timergage.h"
#include "mg_timerframe.h"
#include "titlelogo.h"
#include "startbutton.h"
#include "resulthead.h"
#include "resulttimehead.h"
#include "resultkillhead.h"
#include "resultheathead.h"
#include "seconds.h"
#include "percent.h"
#include "maxscore.h"
#include "resulttime.h"
#include "resultkill.h"
#include "resultheat.h"
#include "totalscore.h"
#include "tutorialhead.h"
#include "descriptheat.h"
#include "mg_head.h"
#include "escape.h"
#include "bosshp.h"
#include "bosshpframe.h"
#include "descriptgage.h"

//===========================================================================================================
// 静的メンバ変数初期化
//===========================================================================================================
int CUI::m_nAll = 0;

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CUI::CUI(int nPriority) : CObject2D{ nPriority },
	m_pPrev{ nullptr },
	m_pNext{ nullptr },
	m_Type{ TYPE::NONE },
	m_bEdit{ true }

{
	//インデックス設定
	m_nIdx = m_nAll;

	//総数加算
	m_nAll++;
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CUI::~CUI()
{
	//総数減算
	m_nAll--;
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CUI::Init()
{
	//自身のタイプをint型で保存
	int nType = static_cast<int>(m_Type);

	//テクスチャ生成
	CTexture* pTex = CTexture::GetInstance();//取得
	int nTexIdx = pTex->Regist(CTag::TAG::UI, nType, CUI::TEXTURE_FILE[nType]);//登録
	BindTexture(pTex->GetAddress(nTexIdx));//設定

	//基底クラス初期化処理
	if (FAILED(CObject2D::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CUI::Uninit()
{
	//基底クラス終了処理
	CObject2D::Uninit();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CUI::Update()
{
	//基底クラス更新処理
	CObject2D::Update();
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CUI::Draw()
{
	//基底クラス描画処理
	CObject2D::Draw();
}

//===========================================================================================================
// 生成処理
//===========================================================================================================
CUI* CUI::Create(TYPE type, const D3DXVECTOR2& pos, const D3DXVECTOR2& size, const D3DXVECTOR3& rot)
{
	//タイプに応じてメモリを動的確保
	CUI* pUI = nullptr;
	switch (type)
	{
		//ヒートゲージフレーム
	case TYPE::HEAT_GAGE_FRAME:
		pUI = NEW CHeatGageFrame();
		break;

		//ヒートゲージ
	case TYPE::HEAT_GAGE:
		pUI = NEW CHeatGage();
		break;

		//タイマー
	case TYPE::TIME_NUMBER:
		pUI = NEW CTimeNumber();
		break;

		//タイマー見出し
	case TYPE::TIMER_HEADING:
		pUI = NEW CTimeHeading();
		break;

		//ミニゲームタイマー背景
	case TYPE::MINIGAME_BACK:
		pUI = NEW CMG_TimerBack();
		break;

		//ミニゲームタイマーゲージ
	case TYPE::MINIGAME_GAGE:
		pUI = NEW CMG_TimerGage();
		break;

		//ミニゲームタイマーフレーム
	case TYPE::MINIGAME_FRAME:
		pUI = NEW CMG_TimerFrame();
		break;

		//タイトルロゴ
	case TYPE::TITLE_LOGO:
		pUI = NEW CTitleLogo();
		break;

		//スタートボタン
	case TYPE::START_BUTTON:
		pUI = NEW CStartButton();
		break;

		//リザルト見出し
	case TYPE::RESULT_HEAD:
		pUI = NEW CResultHead();
		break;

		//リザルトタイム見出し
	case TYPE::RESULT_TIME_HEAD:
		pUI = NEW CResultTimeHead();
		break;

		//リザルトキル数見出し
	case TYPE::RESULT_KILL_HEAD:
		pUI = NEW CResultKillHead();
		break;

		//リザルトヒート量見出し
	case TYPE::RESULT_HEAT_HEAD:
		pUI = NEW CResultHeatHead();
		break;

		//秒数単位
	case TYPE::SECONDS:
		pUI = NEW CSeconds();
		break;

		//パーセントマーク
	case TYPE::PERCENT:
		pUI = NEW CPercent();
		break;

		//スコア最大値
	case TYPE::MAXSCORE:
		pUI = NEW CMaxScore();
		break;

		//リザルトタイム
	case TYPE::RESULT_TIME:
		pUI = NEW CResultTime();
		break;

		//リザルトキル数
	case TYPE::RESULT_KILL:
		pUI = NEW CResultKill();
		break;

		//リザルト平均ヒート量
	case TYPE::RESULT_HEAT:
		pUI = NEW CResultHeat();
		break;

		//トータルスコア
	case TYPE::TOTAL_SCORE:
		pUI = NEW CTotalScore();
		break;

		//チュートリアル見出し
	case TYPE::TUTORIAL_HEAD:
		pUI = NEW CTutorialHead();
		break;

		//ヒート説明
	case TYPE::DESCRIPT_HEAT:
		pUI = NEW CDescriptHeat();
		break;

		//ミニゲーム見出し
	case TYPE::MINIGAME_HEAD:
		pUI = NEW CMG_Head();
		break;

		//チェイスエリア警告
	case TYPE::ESCAPE:
		pUI = NEW CEscape();
		break;

		//ボスHP
	case TYPE::BOSS_HP:
		pUI = NEW CBossHP();
		break;

		//ボスHPフレーム
	case TYPE::BOSS_HP_FRAME:
		pUI = NEW CBossHPFrame();
		break;

		//ゲージ説明
	case TYPE::DESCRIPT_GAGE:
		pUI = NEW CDescriptGage();
		break;

	default:
		break;
	}

	//ポインタがnullptrではない場合
	if (pUI != nullptr)
	{
		//パラメータ設定
		pUI->m_Type = type;//タイプ
		pUI->SetPos(pos);//座標
		pUI->SetSize(size);//サイズ
		pUI->SetRot(rot);//角度

		//初期化処理
		pUI->Init();

		//マネージャー登録
		CManager::GetInstance()->GetScene()->GetUIManager()->Regist(pUI);
	}

	return pUI;
}
