//==============================================================================================================================================
//
// 2Dエディタの処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifdef _DEBUG

//===========================================================================================================
// ファイルインクルード
//===========================================================================================================
#include "edit2D.h"
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

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CEdit2D::CEdit2D() : 
	m_nTarget{ 0 },
	m_bTrigger{ false }
{
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CEdit2D::~CEdit2D()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CEdit2D::Init()
{
	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CEdit2D::Uninit()
{
}

//===========================================================================================================
// 解放処理
//===========================================================================================================
void CEdit2D::Release()
{
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CEdit2D::Update()
{
	//マネージャーのインスタンスを取得
	CManager* pManager = CManager::GetInstance();

	//UIカテゴリーの先頭オブジェクト取得
	CUI* pUI = pManager->GetScene()->GetUIManager()->GetTop();

	//先頭オブジェクトが存在しない場合
	if (pUI == nullptr)
	{
		//一つ生成
		CUI::Create(static_cast<CUI::TYPE>(1), { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f }, { 10.0f, 10.0f }, { 0.0f, 0.0f, 0.0f });
	}

	//キーボードのインスタンスを取得
	CInputKeyboard* pKeyboard = pManager->GetKeyboard();

	//=======================================================================
	// 移動方法切り替え
	//=======================================================================
	if (pKeyboard->GetTrigger(DIK_1))
	{//1キーが押された場合
		m_bTrigger = m_bTrigger ? false : true;
	}

	//=======================================================================
	// 編集対象変更
	//=======================================================================

	//Nキーが押された場合
	if (pKeyboard->GetRepeat(DIK_N, 10))
	{
		//前のオブジェクトを対象にする
		m_nTarget--;

		//カウントがオブジェクトの最大数に到達した場合
		if (m_nTarget < 0)
		{
			//一番最後にループする
			m_nTarget = CUI::m_nAll - 1;
		}
	}
	//Mキーが押された場合
	else if (pKeyboard->GetRepeat(DIK_M, 10))
	{
		//後のオブジェクトを対象にする
		m_nTarget++;

		//カウントがオブジェクトの最大数に到達した場合
		if (m_nTarget > CUI::m_nAll - 1)
		{
			//一番最初にループする
			m_nTarget = 0;
		}
	}

	//=======================================================================
	// データの書き出し
	//=======================================================================
	if (pKeyboard->GetTrigger(DIK_F3))
	{//F3キーが押された場合

		//書き出し処理
		Save();
	}

	//オブジェクト情報がnullptrになるまで繰り返す
	while (pUI != nullptr)
	{
		//次のオブジェクト情報を取得
		CUI* pNext = pUI->GetNextUI();

		//編集対象番号とインデックスが一致
		if (m_nTarget == pUI->GetIdx() && pUI->GetEditFlag())
		{
			//情報表示処理
			PrintInfo(pUI);

			//各編集処理
			Move(pUI);//座標
			Scale(pUI);//サイズ
			Turn(pUI);//角度
			Reset(pUI);//リセット
			Change(pUI);//タイプ変更
			Delete(pUI);//削除処理
			Create(pUI);//配置処理

			//半透明にする
			pUI->SetColor({ 1.0f, 1.0f, 1.0f, 0.5f });
			break;
		}
		else
		{
			//カラーを戻す
			pUI->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		}

		//オブジェクト情報を次の情報に変更する
		pUI = pNext;
	}
}

//===========================================================================================================
// 移動処理
//===========================================================================================================
void CEdit2D::Move(CUI* pUI)
{
	//キーボードのインスタンスを取得
	CInputKeyboard* pKeyboard = CManager::GetInstance()->GetKeyboard();

	//現在の座標情報を取得
	D3DXVECTOR2 pos = pUI->GetPos();

	//Press
	if (!m_bTrigger)
	{
		//Aキーが押された場合
		if (pKeyboard->GetPress(DIK_A))
		{
			//座標を更新
			pos.x -= MOVE;
		}
		//Dキーが押された場合
		else if (pKeyboard->GetPress(DIK_D))
		{
			//座標を更新
			pos.x += MOVE;
		}
		//Wキーが押された場合
		else if (pKeyboard->GetPress(DIK_W))
		{
			//座標を更新
			pos.y -= MOVE;
		}
		//Sキーが押された場合
		else if (pKeyboard->GetPress(DIK_S))
		{
			//座標を更新
			pos.y += MOVE;
		}
	}
	//Trigger
	else
	{
		//Aキーが押された場合
		if (pKeyboard->GetTrigger(DIK_A))
		{
			//座標を更新
			pos.x -= MOVE;
		}
		//Dキーが押された場合
		else if (pKeyboard->GetTrigger(DIK_D))
		{
			//座標を更新
			pos.x += MOVE;
		}
		//Wキーが押された場合
		else if (pKeyboard->GetTrigger(DIK_W))
		{
			//座標を更新
			pos.y -= MOVE;
		}
		//Sキーが押された場合
		else if (pKeyboard->GetTrigger(DIK_S))
		{
			//座標を更新
			pos.y += MOVE;
		}
	}

	//更新された座標情報を設定
	pUI->SetPos(pos);
}

//===========================================================================================================
// 拡大処理
//===========================================================================================================
void CEdit2D::Scale(CUI* pUI)
{
	//キーボードのインスタンスを取得
	CInputKeyboard* pKeyboard = CManager::GetInstance()->GetKeyboard();

	//現在のサイズ情報を取得
	D3DXVECTOR2 size = pUI->GetSize();

	//Press
	if (!m_bTrigger)
	{
		//Gキーが押された場合
		if (pKeyboard->GetPress(DIK_G))
		{
			//横幅を短くする
			size.x -= SCALE;
		}
		//Jキーが押された場合
		else if (pKeyboard->GetPress(DIK_J))
		{
			//横幅を長くする
			size.x += SCALE;
		}
		//Yキーが押された場合
		else if (pKeyboard->GetPress(DIK_Y))
		{
			//縦幅を長くする
			size.y += SCALE;
		}
		//Hキーが押された場合
		else if (pKeyboard->GetPress(DIK_H))
		{
			//縦幅を短くする
			size.y -= SCALE;
		}
	}
	//Trigger
	else
	{
		//Gキーが押された場合
		if (pKeyboard->GetTrigger(DIK_G))
		{
			//横幅を短くする
			size.x -= SCALE;
		}
		//Jキーが押された場合
		else if (pKeyboard->GetTrigger(DIK_J))
		{
			//横幅を長くする
			size.x += SCALE;
		}
		//Yキーが押された場合
		else if (pKeyboard->GetTrigger(DIK_Y))
		{
			//縦幅を長くする
			size.y += SCALE;
		}
		//Hキーが押された場合
		else if (pKeyboard->GetTrigger(DIK_H))
		{
			//縦幅を短くする
			size.y -= SCALE;
		}
	}

	//更新されたサイズ情報を設定
	pUI->SetSize(size);
}

//===========================================================================================================
// 回転処理
//===========================================================================================================
void CEdit2D::Turn(CUI* pUI)
{
	//キーボードのインスタンスを取得
	CInputKeyboard* pKeyboard = CManager::GetInstance()->GetKeyboard();

	//現在の角度情報を取得
	D3DXVECTOR3 rot = pUI->GetRot();

	//Zキーが押された場合
	if (pKeyboard->GetTrigger(DIK_Z))
	{
		//右回転
		rot.z -= TURN;
	}
	//Xキーが押された場合
	else if (pKeyboard->GetTrigger(DIK_X))
	{
		//左回転
		rot.z += TURN;
	}

	//角度補正
	CObject::CorrectionRot(rot.z);

	//更新された角度情報を設定
	pUI->SetRot(rot);
}

//===========================================================================================================
// リセット処理
//===========================================================================================================
void CEdit2D::Reset(CUI* pUI)
{
	//Rキーが押された場合
	if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_R))
	{
		//パラメータリセット
		pUI->SetPos({ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f });//座標
		pUI->SetRot({ 0.0f, 0.0f, 0.0f });//角度
		pUI->SetSize({ 1.0f, 1.0f });//サイズ
	}
}

//===========================================================================================================
// タイプ変更処理
//===========================================================================================================
void CEdit2D::Change(CUI* pUI)
{
	//キーボードのインスタンスを取得
	CInputKeyboard* pKeyboard = CManager::GetInstance()->GetKeyboard();

	//現在のタグ情報を保存
	int nType = static_cast<int>(pUI->GetUIType());

	//Qキーが押された場合
	if (pKeyboard->GetTrigger(DIK_Q) && nType > 1)
	{
		//タイプ変更
		nType--;

		//UIを削除
		pUI->Uninit();

		while (1)
		{
			//新たにUIを生成
			CUI* pNewUI = CUI::Create(static_cast<CUI::TYPE>(nType), pUI->GetPos(), pUI->GetSize(), pUI->GetRot());

			//作成されたUIのエディタフラグがtrue
			if (pNewUI->GetEditFlag())
			{
				//ループを抜ける
				break;
			}
			//作成されたUIのエディタフラグがfalse
			else
			{
				//UIを削除
				pNewUI->Uninit();

				//さらにタイプを変更
				nType--;
			}
		}
	}
	//Eキーが押された場合
	else if (pKeyboard->GetTrigger(DIK_E) && nType < static_cast<int>(CUI::TYPE::MAX) - 1)
	{
		//タイプ変更
		nType++;

		//UIを削除
		pUI->Uninit();

		while (1)
		{
			//新たにUIを生成
			CUI* pNewUI = CUI::Create(static_cast<CUI::TYPE>(nType), pUI->GetPos(), pUI->GetSize(), pUI->GetRot());

			//作成されたUIのエディタフラグがtrue
			if (pNewUI->GetEditFlag())
			{
				//ループを抜ける
				break;
			}
			//作成されたUIのエディタフラグがfalse
			else
			{
				//UIを削除
				pNewUI->Uninit();

				//さらにタイプを変更
				nType++;
			}
		}
	}
}

//===========================================================================================================
// 削除処理
//===========================================================================================================
void CEdit2D::Delete(CUI* pUI)
{
	if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_DELETE)
		&& CUI::m_nAll > 1)
	{//DELETEキーが押された場合

		//終了処理
		pUI->Uninit();

		//対象を1つ前する
		m_nTarget--;

		//ターゲット番号が0以下にならなように調整
		if (m_nTarget < 0)
		{
			m_nTarget = 0;
		}
	}
}

//===========================================================================================================
// 配置処理
//===========================================================================================================
void CEdit2D::Create(CUI* pUI)
{
	//マネージャーのインスタンスを取得
	CManager* pManager = CManager::GetInstance();

	//キーボードのインスタンスを取得
	CInputKeyboard* pKeyboard = pManager->GetKeyboard();

	//Bキー
	if (pKeyboard->GetTrigger(DIK_B))
	{
		m_nTarget = CUI::Create(pUI->GetUIType(), pUI->GetPos(), pUI->GetSize(), pUI->GetRot())->GetIdx();
	}
}

//===========================================================================================================
// 情報表示処理
//===========================================================================================================
void CEdit2D::PrintInfo(CUI* pUI)
{
	PrintDebugProc("\n==============================\n 操作\n==============================\n");
	PrintDebugProc("回転\n左回転 : Z   右回転 : X\n\n");
	PrintDebugProc("位置と角度をリセット : R\n\n");
	PrintDebugProc("タイプ変更 : Q,E\n");
	PrintDebugProc("編集対象変更 : N,M\n");
	PrintDebugProc("配置 : B\n");
	PrintDebugProc("削除 : DELETE\n");
	PrintDebugProc("書き出し : F3\n\n");

	//配置物データ表示
	PrintDebugProc("==============================\n 配置物データ\n==============================\n");

	//操作モードを表示
	PrintDebugProc("操作モード( 1で切り替え ) : ");
	if (!m_bTrigger)
	{
		//プレス
		PrintDebugProc("Press\n\n");
	}
	else
	{
		//トリガー
		PrintDebugProc("Trigger\n\n");
	}

	D3DXVECTOR2 pos = pUI->GetPos();
	PrintDebugProc("POS X:%f, Y:%f\n\n", pos.x, pos.y);

	D3DXVECTOR2 size = pUI->GetSize();
	PrintDebugProc("SIZE X:%f, Y:%f\n\n", size.x, size.y);

	D3DXVECTOR3 rot = pUI->GetRot();
	PrintDebugProc("ROT X:%f, Y:%f, Z:%f\n\n", rot.x, rot.y, rot.z);//角度

	PrintDebugProc("Idx:%d\n", pUI->GetIdx());//インデックス
	PrintDebugProc("TYPE:%d\n", static_cast<int>(pUI->GetUIType()));//タイプ

	PrintDebugProc("\n配置数:%d/%d\n", m_nTarget + 1, CUI::m_nAll);//配置数
}

//===========================================================================================================
// セーブ処理
//===========================================================================================================
void CEdit2D::Save()
{
	//マネージャーのインスタンスを取得
	CManager* pManager = CManager::GetInstance();

	//キーボードのインスタンスを取得
	CScene* pScene = pManager->GetScene();

	//ファイルを開く
	FILE* pFile = fopen(pManager->UIFile[static_cast<int>(pScene->GetMode())], "wb");

	//ファイルが見つかった場合
	if (pFile != nullptr)
	{
		//配置数を書き出す
		fwrite(&CUI::m_nAll, sizeof(int), 1, pFile);

		//UIカテゴリーの先頭オブジェクト取得
		CUI* pUI = pScene->GetUIManager()->GetTop();

		//オブジェクト情報がnullptrになるまで繰り返す
		while (pUI != nullptr)
		{
			//次のオブジェクト情報を取得
			CUI* pNext = pUI->GetNextUI();

			//データの記録
			int nType = static_cast<int>(pUI->GetUIType());
			fwrite(&nType, sizeof(int), 1, pFile);//タイプ
			fwrite(pUI->GetPos(), sizeof(D3DXVECTOR2), 1, pFile);//座標
			fwrite(pUI->GetSize(), sizeof(D3DXVECTOR2), 1, pFile);//サイズ
			fwrite(pUI->GetRot(), sizeof(D3DXVECTOR3), 1, pFile);//角度
			bool bSaveEditFlag = pUI->GetEditFlag();
			fwrite(&bSaveEditFlag, sizeof(bool), 1, pFile);//エディット可能フラグ
			
			//オブジェクト情報を次の情報に変更する
			pUI = pNext;
		}

		//ファイルを閉じる
		fclose(pFile);
	}
}

#endif // _DEBUG
