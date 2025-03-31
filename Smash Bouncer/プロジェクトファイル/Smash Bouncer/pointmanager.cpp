//==============================================================================================================================================
//
// ポイント管理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "pointmanager.h"//ポイントマネージャー

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CPointManager::CPointManager() : CObjectManager()
{
	//静的メンバ変数初期化
	m_pTop = nullptr;//先頭アドレス
	m_pCur = nullptr;//終端アドレス
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CPointManager::~CPointManager()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CPointManager::Init()
{
	//ローカル変数宣言
	D3DXVECTOR3 pos = { 0.0f,0.0f,0.0f };//位置
	D3DXVECTOR3 rot = { 0.0f,0.0f,0.0f };//向き
	D3DXVECTOR3 scale = { 1.0f,1.0f,1.0f };//拡大率
	int nNumAll = 0;//総数
	int nCategory = 0;//カテゴリー
	int nType = 0;//種類
	const char* StageFile = "";//ステージ情報ファイルパス

	//シーンに応じてファイルを変更
	switch (CManager::GetScene()->GetMode())
	{
		//チュートリアル
	case CScene::MODE_TUTORIAL:
		StageFile = CManager::TUTORIAL_STAGE;
		break;

		//ゲーム
	case CScene::MODE_GAME:
		StageFile = CManager::GAME_STAGE;
		break;

	default:
		break;
	}

	//ファイルを開く
	FILE* pFile = fopen(StageFile, "rb");

	//オブジェクトデータの読み込み
	if (pFile != nullptr)
	{
		//ファイルの読み込み
		fread(&nNumAll, sizeof(int), 1, pFile);

		for (int nCnt = 0; nCnt < nNumAll; nCnt++)
		{
			//データの取得
			fread(&pos, sizeof(D3DXVECTOR3), 1, pFile);//位置
			fread(&rot, sizeof(D3DXVECTOR3), 1, pFile);//向き
			fread(&scale, sizeof(D3DXVECTOR3), 1, pFile);//拡大率
			fread(&nCategory, sizeof(int), 1, pFile);//カテゴリー
			fread(&nType, sizeof(int), 1, pFile);//種類

			//カテゴリーがブロックの場合
			if (nCategory == CXfile::POINT)
			{
				//ブロックを生成
				CPoint* pPoint = CPoint::Create(pos, rot);

				//先頭アドレスが存在しない場合
				if (m_pTop == nullptr)
				{
					//生成したブロック情報を先頭アドレスに代入
					m_pTop = pPoint;

					//前のオブジェクト情報にnullptrを代入
					pPoint->SetPrevPoint(nullptr);
				}

				//先頭アドレスが存在する場合
				else
				{
					//先頭アドレスを取得
					CPoint* pAllPoint = m_pTop;

					//オブジェクト情報がnullptrになるまで繰り返す
					while (pAllPoint != nullptr)
					{
						//次のオブジェクト情報が存在しない場合
						if (pAllPoint->GetNextPoint() == nullptr)
						{
							//次のオブジェクト情報に生成したブロック情報を保存
							pAllPoint->SetNextPoint(pPoint);

							//生成したブロック情報の前のオブジェクト情報を保存
							pPoint->SetPrevPoint(pAllPoint);

							break;
						}

						//オブジェクト情報を次の情報に変更する
						pAllPoint = pAllPoint->GetNextPoint();
					}
				}

				//自身の次のオブジェクト情報にnullptrを代入
				pPoint->SetNextPoint(nullptr);

				//終端アドレスを代入
				m_pCur = pPoint;
			}
		}

		//ファイルを閉じる
		fclose(pFile);
	}

	//オブジェクトマネージャー初期化処理
	if (FAILED(CObjectManager::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===========================================================================================================
// 終了処理
//===========================================================================================================
void CPointManager::Uninit()
{
	//オブジェクトマネージャー終了処理
	CObjectManager::Uninit();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CPointManager::Update()
{
	//先頭アドレスを取得する
	CPoint* pPoint = m_pTop;

	//オブジェクト情報がnullptrになるまで繰り返す
	while (pPoint != nullptr)
	{
		//次のオブジェクト情報を取得
		CPoint* pNext = pPoint->GetNextPoint();

		//削除フラグがtrueの場合
		if (pPoint->GetDeleteFlag())
		{
			//前のオブジェクト情報を取得
			CPoint* pPrev = pPoint->GetPrevPoint();

			//前のオブジェクト情報が存在する場合
			if (pPrev != nullptr)
			{
				//次のオブジェクト情報を繋ぐ
				pPrev->SetNextPoint(pNext);
			}
			//前のオブジェクト情報が存在しない場合
			else
			{
				//次ののオブジェクト情報の前のオブジェクト情報をnullptrに設定
				pNext->SetPrevPoint(nullptr);

				//先頭アドレスを変更
				m_pTop = pNext;
			}

			//次のオブジェクト情報が存在する場合
			if (pNext != nullptr)
			{
				//前のオブジェクト情報を繋ぐ
				pNext->SetPrevPoint(pPrev);
			}
			//次のオブジェクト情報が存在しない場合
			else
			{
				//前のオブジェクト情報の次のオブジェクト情報をnullptrに設定
				pPrev->SetNextPoint(nullptr);

				//終端アドレスを変更
				m_pCur = pPrev;
			}
		}

		//オブジェクト情報を次の情報に変更する
		pPoint = pNext;
	}
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CPointManager::Draw()
{

}
