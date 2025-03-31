//==============================================================================================================================================
//
// ブロック管理
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// ヘッダーインクルード
//===========================================================================================================
#include "blockmanager.h"//ブロックマネージャー

//===========================================================================================================
// コンストラクタ
//===========================================================================================================
CBlockManager::CBlockManager(int nPriority) : CObjectManager(nPriority)
{
	//静的メンバ変数初期化
	m_pTop = nullptr;//先頭アドレス
	m_pCur = nullptr;//終端アドレス
}

//===========================================================================================================
// デストラクタ
//===========================================================================================================
CBlockManager::~CBlockManager()
{
}

//===========================================================================================================
// 初期化処理
//===========================================================================================================
HRESULT CBlockManager::Init()
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
		//タイトル、チュートリアル
	case CScene::MODE_TITLE:
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
			if (nCategory == CXfile::BLOCK)
			{
				//ブロックを生成
				CBlock* pBlock = CBlock::Create(pos, rot, scale, nType);

				//先頭アドレスが存在しない場合
				if (m_pTop == nullptr)
				{
					//生成したブロック情報を先頭アドレスに代入
					m_pTop = pBlock;

					//前のオブジェクト情報にnullptrを代入
					pBlock->SetPrevBlock(nullptr);
				}

				//先頭アドレスが存在する場合
				else
				{
					//先頭アドレスを取得
					CBlock* pAllBlock = m_pTop;

					//オブジェクト情報がnullptrになるまで繰り返す
					while (pAllBlock != nullptr)
					{
						//次のオブジェクト情報が存在しない場合
						if (pAllBlock->GetNextBlock() == nullptr)
						{
							//次のオブジェクト情報に生成したブロック情報を保存
							pAllBlock->SetNextBlock(pBlock);

							//生成したブロック情報の前のオブジェクト情報を保存
							pBlock->SetPrevBlock(pAllBlock);

							break;
						}

						//オブジェクト情報を次の情報に変更する
						pAllBlock = pAllBlock->GetNextBlock();
					}
				}

				//自身の次のオブジェクト情報にnullptrを代入
				pBlock->SetNextBlock(nullptr);
				
				//終端アドレスを代入
				m_pCur = pBlock;
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
void CBlockManager::Uninit()
{
	//オブジェクトマネージャー終了処理
	CObjectManager::Uninit();
}

//===========================================================================================================
// 更新処理
//===========================================================================================================
void CBlockManager::Update()
{
	//先頭アドレスを取得する
	CBlock* pBlock = m_pTop;

	//オブジェクト情報がnullptrになるまで繰り返す
	while (pBlock != nullptr)
	{
		//次のオブジェクト情報を取得
		CBlock* pNext = pBlock->GetNextBlock();

		//ブロック情報が存在する場合
		//削除フラグがtrueの場合
		if (pBlock != nullptr && pBlock->GetDeleteFlag())
		{
			//前のオブジェクト情報を取得
			CBlock* pPrev = pBlock->GetPrevBlock();

			//前のオブジェクト情報が存在する場合
			if (pPrev != nullptr)
			{
				//次のオブジェクト情報を繋ぐ
				pPrev->SetNextBlock(pNext);
			}
			//前のオブジェクト情報が存在しない場合
			else
			{
				//次ののオブジェクト情報の前のオブジェクト情報をnullptrに設定
				pNext->SetPrevBlock(nullptr);

				//先頭アドレスを変更
				m_pTop = pNext;
			}

			//次のオブジェクト情報が存在する場合
			if (pNext != nullptr)
			{
				//前のオブジェクト情報を繋ぐ
				pNext->SetPrevBlock(pPrev);
			}
			//次のオブジェクト情報が存在しない場合
			else
			{
				//前のオブジェクト情報の次のオブジェクト情報をnullptrに設定
				pPrev->SetNextBlock(nullptr);

				//終端アドレスを変更
				m_pCur = pPrev;
			}
		}

		//オブジェクト情報を次の情報に変更する
		pBlock = pNext;
	}
}

//===========================================================================================================
// 描画処理
//===========================================================================================================
void CBlockManager::Draw()
{
}

//===========================================================================================================
// 当たり判定
//===========================================================================================================
void CBlockManager::Collision(D3DXVECTOR3& pos, D3DXVECTOR3& oldpos, D3DXVECTOR3& size)
{
	//先頭アドレスを取得する
	CBlock* pBlock = m_pTop;

	//オブジェクト情報がnullptrになるまで繰り返す
	while (pBlock != nullptr)
	{
		//次のオブジェクト情報を取得
		CBlock* pNext = pBlock->GetNextBlock();

		//ブロック情報が存在する場合
		if (pBlock != nullptr)
		{
			//当たり判定処理
			pBlock->Collision(pos, oldpos, size);
		}

		//オブジェクト情報を次の情報に変更する
		pBlock = pNext;
	}
}

//===========================================================================================================
// 反射処理
//===========================================================================================================
bool CBlockManager::Reflection(D3DXVECTOR3& pos, D3DXVECTOR3& oldpos, D3DXVECTOR3& move, D3DXVECTOR3& rot, D3DXVECTOR3& size)
{
	//ローカル変数宣言
	bool bReflect = false;//反射判定

	//先頭アドレスを取得する
	CBlock* pBlock = m_pTop;

	//オブジェクト情報がnullptrになるまで繰り返す
	while (pBlock != nullptr)
	{
		//次のオブジェクト情報を取得
		CBlock* pNext = pBlock->GetNextBlock();

		//ブロック情報が存在する場合
		if (pBlock != nullptr)
		{
			//当たり判定処理
			bReflect = pBlock->Reflection(pos, oldpos, move, rot, size);
		}
	
		//反射判定だった場合
		if (bReflect)
		{
			//反射処理を抜ける
			break;
		}

		//オブジェクト情報を次の情報に変更する
		pBlock = pNext;
	}

	//反射判定を返す
	return bReflect;
}
