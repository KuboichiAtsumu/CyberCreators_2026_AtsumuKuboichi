//==============================================================================================================================================
//
// ミニゲームに関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "minigame.h"
#include "game.h"
#include "areamanager.h"
#include "enemymanager.h"
#include "player.h"
#include "UI.h"
#include "minigameclear.h"
#include "minigamefailed.h"

//===========================================================================================================
// 静的メンバ変数初期化
//===========================================================================================================
int CMiniGame::m_nTimer = 0;

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CMiniGame::CMiniGame(int nPriority) : CObject(nPriority),
	m_nCntFrame(0)
{
	m_nTimer = TIMER;//タイマー
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CMiniGame::~CMiniGame()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CMiniGame::Init()
{
	//基底クラス初期化処理
	if (FAILED(CObject::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CMiniGame::Uninit()
{
	//削除フラグをtrueに設定
	SetDeleteFlag(true);
}

//===========================================================================================================
// 解放処理
//===========================================================================================================
void CMiniGame::Release()
{
	//基底クラス解放処理
	CObject::Release();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CMiniGame::Update()
{
	//ゲームシーンのインスタンス取得
	CGame* pGame = CGame::GetInstance();

	//ミニゲーム中ではない場合
	if (pGame != nullptr && pGame->GetArea() != CGame::GAME_AREA::MINI_GAME)
	{
		//終了処理
		Uninit();

		//処理を抜ける
		return;
	}

	//タイマーカウント処理
	TimerCount();
}

//===========================================================================================================
// タイマーカウント処理
//===========================================================================================================
void CMiniGame::TimerCount()
{
	//フレームカウンタを加算
	m_nCntFrame++;

	//フレームカウンタが60フレーム
	if (m_nCntFrame == MAX_FRAME)
	{
		//カウンタをリセット
		m_nCntFrame = 0;
	}
	//フレームカウンタが60フレームではない
	else
	{
		//処理を抜ける
		return;
	}

	//シーンのインスタンスを取得
	CScene* pScene = CManager::GetInstance()->GetScene();

	//ミニゲームエリアを検索
	CArea* pArea = pScene->GetAreaManager()->FindArea(CArea::TYPE::MINI_GAME);

	//エリア情報がnullptrではない場合
	if (pArea == nullptr)
	{
		assert(false);
	}

	//エネミーカウンタ
	int nCntEnemy = 0;
	std::vector<CEnemy*> aEnemy;

	//エネミーの先頭オブジェクトを取得
	CEnemy* pEnemy = pScene->GetEnemyManager()->GetTop();

	//オブジェクト情報がnullptrになるまで繰り返す
	while (pEnemy != nullptr)
	{
		//次のオブジェクト情報を取得
		CEnemy* pNext = pEnemy->GetNextEnemy();

		//当たり判定がtrueの場合
		if (pArea->Collision(pEnemy->GetPos()))
		{
			//エネミーカウンタ加算
			nCntEnemy++;

			//対象のエネミー情報を保存
			aEnemy.push_back(pEnemy);
		}

		//オブジェクト情報を次の情報に変更する
		pEnemy = pNext;
	}

	//プレイヤーカテゴリーのオブジェクト情報を取得
	CObject* pFindObj = CObject::FindObject(CObject::Category::PLAYER);
	CPlayer* pPlayer = nullptr;

	//オブジェクト情報が存在する場合
	if (pFindObj != nullptr)
	{
		//プレイヤークラスにダウンキャスト
		pPlayer = CObject::DownCast<CPlayer, CObject>(pFindObj);
	}
	//オブジェクト情報が存在しない場合
	else
	{
		assert(false);
	}

	//エフェクト生成座標を設定
	D3DXVECTOR3 CreatePos = pPlayer->GetPos();
	CreatePos.y += CREATE_EFFECT_HEIGHT;

	//エリア内のエネミーが0
	if (nCntEnemy == 0)
	{
		//プレイヤーの上にクリアエフェクトを表示
		CMiniGameClear::Create(CreatePos, pPlayer);

		//SE再生
		CManager::GetInstance()->GetSound()->Play(CSound::SOUND::MINIGAME_CLEAR);

		//エリアを削除
		pArea->Uninit();

		//現在のエリアを通常エリアに設定
		CGame::GetInstance()->SetArea(CGame::GAME_AREA::NORMAL);

		//終了処理
		Uninit();
	}

	//タイマーが0ではない場合
	if (m_nTimer > 0)
	{
		//タイマーを減らす
		m_nTimer--;
	}

	//エネミー数を保存
	int nNumEnemy = aEnemy.size();

	//タイマーが0の時にエネミーが残っている場合
	if (m_nTimer == 0 && nCntEnemy > 0)
	{
		//エリア内のエネミーを削除
		for (int nCnt = 0; nCnt < nNumEnemy; nCnt++)
		{
			aEnemy.at(nCnt)->Uninit();
		}

		//プレイヤーの上に失敗エフェクトを表示
		CMiniGameFailed::Create(CreatePos, pPlayer);

		//エリアを削除
		pArea->Uninit();

		//現在のエリアを通常エリアに設定
		CGame::GetInstance()->SetArea(CGame::GAME_AREA::NORMAL);

		//終了処理
		Uninit();
	}

	//エリア内のエネミーを削除
	for (int nCnt = 0; nCnt < nNumEnemy; nCnt++)
	{
		aEnemy.at(nCnt) = nullptr;
	}

	//配列を解放
	aEnemy.clear();
}

//===========================================================================================================
// 生成処理
//===========================================================================================================
CMiniGame* CMiniGame::Create()
{
	//メモリを動的確保
	CMiniGame* pMiniGame = NEW CMiniGame(static_cast<int>(Category::EVENT));

	//ポインタがnullptrではない場合
	if (pMiniGame != nullptr)
	{
		//初期化処理
		pMiniGame->Init();
	}

	//ローカル変数宣言
	int nNum = m_nTimer;//現在のタイマーを保存
	int nDigit = 0;//桁数

	//桁数を求める
	while (nNum != 0)
	{//nNumが0になるまで
		nNum /= 10;//タイマーを10で割る
		nDigit++;//桁数を加算
	}

	//ミニゲーム用タイマー生成
	CUI::Create(CUI::TYPE::MINIGAME_HEAD, { 950.0f, 570.0f }, { 400.0f, 60.0f }, { 0.0f, 0.0f, 0.0f });//ミニゲーム見出し
	CUI::Create(CUI::TYPE::MINIGAME_BACK, { 950.0f, 640.0f }, { 500.0f, 50.0f }, { 0.0f, 0.0f, 0.0f });//ミニゲームタイマーゲージ背景
	CUI::Create(CUI::TYPE::MINIGAME_GAGE, { 950.0f - 490.0f * 0.5f, 640.0f }, { 490.0f, 40.0f }, { 0.0f, 0.0f, 0.0f });//ミニゲームタイマーゲージ
	CUI::Create(CUI::TYPE::MINIGAME_FRAME, { 950.0f, 659.0f }, { 500.0f, 88.0f }, { 0.0f, 0.0f, 0.0f });//ミニゲームタイマーゲージフレーム

	return pMiniGame;
}
