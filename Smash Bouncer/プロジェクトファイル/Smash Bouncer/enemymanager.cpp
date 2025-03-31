//==============================================================================================================================================
//
// エネミーマネージャーに関する処理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "enemymanager.h"//エネミーマネージャー

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CEnemyManager::CEnemyManager(int nPriority) : CObjectManager(nPriority)
{
	//静的メンバ変数初期化
	m_pTop = nullptr;//先頭アドレス
	m_pCur = nullptr;//終端アドレス
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CEnemyManager::~CEnemyManager()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CEnemyManager::Init()
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

			//カテゴリーがエネミーの場合
			if (nCategory == CXfile::ENEMY)
			{
				//エネミーを生成
				CEnemy* pEnemy = CEnemy::Create(pos, rot, nType);
				
				//先頭アドレスが存在しない場合
				if (m_pTop == nullptr)
				{
					//生成したエネミー情報を先頭アドレスに代入
					m_pTop = pEnemy;

					//前のオブジェクト情報にnullptrを代入
					pEnemy->SetPrevEnemy(nullptr);
				}

				//先頭アドレスが存在する場合
				else
				{
					//先頭アドレスを取得
					CEnemy* pAllEnemy = m_pTop;

					//オブジェクト情報がnullptrになるまで繰り返す
					while (pAllEnemy != nullptr)
					{
						//次のオブジェクト情報が存在しない場合
						if (pAllEnemy->GetNextEnemy() == nullptr)
						{
							//次のオブジェクト情報に生成したエネミー情報を保存
							pAllEnemy->SetNextEnemy(pEnemy);

							//生成したエネミー情報の前のオブジェクト情報を保存
							pEnemy->SetPrevEnemy(pAllEnemy);

							break;
						}

						//オブジェクト情報を次の情報に変更する
						pAllEnemy = pAllEnemy->GetNextEnemy();
					}
				}

				//自身の次のオブジェクト情報にnullptrを代入
				pEnemy->SetNextEnemy(nullptr);

				//終端アドレスを代入
				m_pCur = pEnemy;
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
void CEnemyManager::Uninit()
{
	//オブジェクトマネージャー終了処理
	CObjectManager::Uninit();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CEnemyManager::Update()
{
	//先頭アドレスを取得する
	CEnemy* pEnemy = m_pTop;

	//オブジェクト情報がnullptrになるまで繰り返す
	while (pEnemy != nullptr)
	{
		//次のオブジェクト情報を取得
		CEnemy* pNext = pEnemy->GetNextEnemy();

		//削除フラグがtrueの場合
		if (pEnemy->GetDeleteFlag())
		{
			//前のオブジェクト情報を取得
			CEnemy* pPrev = pEnemy->GetPrevEnemy();

			//前のオブジェクト情報が存在する場合
			if (pPrev != nullptr)
			{
				//次のオブジェクト情報を繋ぐ
				pPrev->SetNextEnemy(pNext);
			}
			//前のオブジェクト情報が存在しない場合
			else
			{
				//次のオブジェクト情報が存在する場合
				if (pNext != nullptr)
				{
					//次のオブジェクト情報の前のオブジェクト情報をnullptrに設定
					pNext->SetPrevEnemy(nullptr);
				}

				//先頭アドレスを変更
				m_pTop = pNext;
			}

			//次のオブジェクト情報が存在する場合
			if (pNext != nullptr)
			{
				//前のオブジェクト情報を繋ぐ
				pNext->SetPrevEnemy(pPrev);
			}
			//次のオブジェクト情報が存在しない場合
			else
			{
				//前のオブジェクト情報が存在する場合
				if (pPrev != nullptr)
				{
					//前のオブジェクト情報の次のオブジェクト情報をnullptrに設定
					pPrev->SetNextEnemy(nullptr);
				}

				//終端アドレスを変更
				m_pCur = pPrev;
			}
		}

		//オブジェクト情報を次の情報に変更する
		pEnemy = pNext;
	}
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CEnemyManager::Draw()
{
}

//===========================================================================================================
// 当たり判定
//===========================================================================================================
bool CEnemyManager::Collision(D3DXVECTOR3& pos, D3DXVECTOR3& size)
{
	//ローカル変数宣言
	bool bCollsion = false;//当たり判定フラグ

	//先頭アドレスを取得する
	CEnemy* pEnemy = m_pTop;

	//オブジェクト情報がnullptrになるまで繰り返す
	while (pEnemy != nullptr)
	{
		//次のオブジェクト情報を取得
		CEnemy* pNext = pEnemy->GetNextEnemy();

		//敵に当たった場合
		if (!pEnemy->GetDeleteFlag() && pEnemy->Collision(pos, size))
		{
			//ヒット処理
			pEnemy->Hit();

			//当たり判定フラグをtrueにする
			bCollsion = true;
		}

		//オブジェクト情報を次の情報に変更する
		pEnemy = pNext;
	}

	return bCollsion;
}

//===========================================================================================================
// 登録処理
//===========================================================================================================
void CEnemyManager::Regist(CEnemy* pEnemy)
{
	//先頭アドレスが存在しない場合
	if (m_pTop == nullptr)
	{
		//生成したバレット情報を先頭アドレスに代入
		m_pTop = pEnemy;

		//前のオブジェクト情報にnullptrを代入
		m_pTop->SetPrevEnemy(nullptr);
	}

	//先頭アドレスが存在する場合
	else
	{
		//先頭アドレスを取得
		CEnemy* pAllEnemy = m_pTop;

		//オブジェクト情報がnullptrになるまで繰り返す
		while (pAllEnemy != nullptr)
		{
			//次のオブジェクト情報が存在しない場合
			if (pAllEnemy->GetNextEnemy() == nullptr)
			{
				//次のオブジェクト情報に生成したバレット情報を保存
				pAllEnemy->SetNextEnemy(pEnemy);

				//生成したバレット情報の前のオブジェクト情報を保存
				pEnemy->SetPrevEnemy(pAllEnemy);

				break;
			}

			//オブジェクト情報を次の情報に変更する
			pAllEnemy = pAllEnemy->GetNextEnemy();
		}
	}

	//自身の次のオブジェクト情報にnullptrを代入
	pEnemy->SetNextEnemy(nullptr);

	//終端アドレスを代入
	m_pCur = pEnemy;
}
