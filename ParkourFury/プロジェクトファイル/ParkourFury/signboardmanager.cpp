//==============================================================================================================================================
//
// 看板オブジェクト管理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "signboardmanager.h"

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CSignBoardManager::CSignBoardManager() : CObjectManager(),
	m_pTop(nullptr),
	m_pCur(nullptr)
{
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CSignBoardManager::~CSignBoardManager()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CSignBoardManager::Init()
{
	//基底クラス初期化処理
	if (FAILED(CObjectManager::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CSignBoardManager::Uninit()
{
	//基底クラス終了処理
	CObjectManager::Uninit();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CSignBoardManager::Update()
{
	//先頭アドレスを取得する
	CSignBoard* pSignBoard = m_pTop;

	//オブジェクト情報がnullptrになるまで繰り返す
	while (pSignBoard != nullptr)
	{
		//次のオブジェクト情報を取得
		CSignBoard* pNext = pSignBoard->GetNext();

		//エリア情報が存在する場合
		//削除フラグがtrueの場合
		if (pSignBoard != nullptr && pSignBoard->GetDeleteFlag())
		{
			//前のオブジェクト情報を取得
			CSignBoard* pPrev = pSignBoard->GetPrev();

			//前のオブジェクト情報が存在する場合
			if (pPrev != nullptr)
			{
				//次のオブジェクト情報を繋ぐ
				pPrev->SetNext(pNext);
			}
			//前のオブジェクト情報が存在しない場合
			else
			{
				//次ののオブジェクト情報の前のオブジェクト情報をnullptrに設定
				pNext->SetPrev(nullptr);

				//先頭アドレスを変更
				m_pTop = pNext;
			}

			//次のオブジェクト情報が存在する場合
			if (pNext != nullptr)
			{
				//前のオブジェクト情報を繋ぐ
				pNext->SetPrev(pPrev);
			}
			//次のオブジェクト情報が存在しない場合
			else
			{
				//前のオブジェクト情報の次のオブジェクト情報をnullptrに設定
				pPrev->SetNext(nullptr);

				//終端アドレスを変更
				m_pCur = pPrev;
			}
		}

		//オブジェクト情報を次の情報に変更する
		pSignBoard = pNext;
	}
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CSignBoardManager::Draw()
{
}

//===========================================================================================================
// 登録処理
//===========================================================================================================
void CSignBoardManager::Regist(CSignBoard* pSignBoard)
{
	//先頭アドレスが存在しない場合
	if (m_pTop == nullptr)
	{
		//生成したエリア情報を先頭アドレスに代入
		m_pTop = pSignBoard;

		//前のオブジェクト情報にnullptrを代入
		pSignBoard->SetPrev(nullptr);
	}

	//先頭アドレスが存在する場合
	else
	{
		//先頭アドレスを取得
		CSignBoard* pAllSignBoard = m_pTop;

		//オブジェクト情報がnullptrになるまで繰り返す
		while (pAllSignBoard != nullptr)
		{
			//次のオブジェクト情報が存在しない場合
			if (pAllSignBoard->GetNext() == nullptr)
			{
				//次のオブジェクト情報に生成したエリア情報を保存
				pAllSignBoard->SetNext(pSignBoard);

				//生成したエリア情報の前のオブジェクト情報を保存
				pSignBoard->SetPrev(pAllSignBoard);
				break;
			}

			//オブジェクト情報を次の情報に変更する
			pAllSignBoard = pAllSignBoard->GetNext();
		}
	}

	//自身の次のオブジェクト情報にnullptrを代入
	pSignBoard->SetNext(nullptr);

	//終端アドレスを代入
	m_pCur = pSignBoard;
}

//===========================================================================================================
// 読込処理
//===========================================================================================================
void CSignBoardManager::Load()
{
	//ローカル変数宣言
	CManager* pManager = CManager::GetInstance();//マネージャーのインスタンス
	D3DXVECTOR3 pos = { 0.0f, 0.0f, 0.0f };//座標情報保存用
	D3DXVECTOR3 rot = { 0.0f, 0.0f, 0.0f };//角度情報保存用
	D3DXVECTOR3 scale = { 0.0f, 0.0f, 0.0f };//拡大率情報保存用
	int nTag = 1;//タグ情報保存用
	int nType = 0;//タイプ情報保存用
	int nNumAll = 0;//総数

	//ファイルを開く
	FILE* pFile = fopen(pManager->StageFile[static_cast<int>(pManager->GetScene()->GetMode())], "rb");

	//ファイルが開けなかった場合処理を抜ける
	if (pFile == nullptr) return;
	
	//ファイルの読み込み
	fread(&nNumAll, sizeof(int), 1, pFile);

	for (int nCnt = 0; nCnt < nNumAll; nCnt++)
	{
		//データの取得
		fread(&pos, sizeof(D3DXVECTOR3), 1, pFile);//座標
		fread(&rot, sizeof(D3DXVECTOR3), 1, pFile);//角度
		fread(&scale, sizeof(D3DXVECTOR3), 1, pFile);//拡大率
		fread(&nTag, sizeof(int), 1, pFile);//タグ
		fread(&nType, sizeof(int), 1, pFile);//種類

		//タグが一致
		if (static_cast<CTag::TAG>(nTag) == CTag::TAG::SIGN_BOARD)
		{
			//エリアを生成
			CSignBoard::Create(static_cast<CSignBoard::TYPE>(nType), pos, scale, rot);
		}
	}

	//ファイルを閉じる
	fclose(pFile);
}

//===========================================================================================================
// 検索処理
//===========================================================================================================
CSignBoard* CSignBoardManager::Find(CSignBoard::TYPE type)
{
	//先頭のポインタをコピー
	CSignBoard* pSignBoard = m_pTop;

	//次のブロック情報がnullptrになるまで繰り返す
	while (pSignBoard != nullptr)
	{
		//タイプが一致
		if (pSignBoard->GetType() == type) return pSignBoard;
		
		//次のブロック情報のポインタをコピー
		pSignBoard = pSignBoard->GetNext();
	}

	return nullptr;
}
