//=======================================================================================================================================================================================================================
//
// プレイヤーに関する処理
// Author : Atsumu Kuboichi
//
//=======================================================================================================================================================================================================================
#include "player.h"
#include "camera.h"
#include "field.h"
#include "wall.h"
#include <stdio.h>

//=======================================================================================================================================================================================================================
// グローバル変数
//=======================================================================================================================================================================================================================
Player g_aPlayer;				//プレイヤー情報
Key g_aCopyKey[MAX_PARTS];		//キーセットコピー情報保存用
int g_nParts;					//現在のパーツ番号
int g_nMaxMotion;				//モーション総数
int g_nMotion;					//現在のモーションの種類
int g_nKeySet[MAX_MOTION];		//現在のキーセット
int g_nCntFrame[MAX_MOTION];	//各モーションの現在のフレーム数
int g_nTotalFrame[MAX_MOTION];	//各モーションの合計フレーム数
bool g_bEditMotion = true;		//モーションエディタ中かどうかの判定
bool g_bUpdate = true;			//更新処理有効化切替

//=======================================================================================================================================================================================================================
// 初期化処理
//=======================================================================================================================================================================================================================
void InitPlayer(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイスの取得
	D3DXMATERIAL* pMat;						//マテリアルのデータへのポインタ

	//プレイヤー情報
	g_aPlayer.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	//位置
	g_aPlayer.oldpos = g_aPlayer.pos;				//過去の位置
	g_aPlayer.move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	//移動量
	g_aPlayer.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	//向き
	g_aPlayer.nState = 0;							//状態
	g_aPlayer.fSpeed = 0.0f;						//速度
	g_aPlayer.fJump = 0.0f;							//ジャンプ上昇量
	g_aPlayer.nParts = 0;							//パーツ数

	//変数
	g_nParts = 0;		//現在のパーツ番号
	g_nMotion = 0;		//現在のモーションの種類
	g_nMaxMotion = -1;	//最大モーション数
	
	//モーション情報
	for (int nCntMotion = 0; nCntMotion < MAX_MOTION; nCntMotion++)
	{
		g_nKeySet[nCntMotion] = 0;//現在のキーセット
		g_nTotalFrame[nCntMotion] = 0;//各モーションの合計フレーム数
		g_aPlayer.aMotion[nCntMotion].nLoop = 0;//ループ判定
		g_aPlayer.aMotion[nCntMotion].nMaxKeySet = 1;//モーションごとのキーセット総数

		for (int nCntKeySet = 0; nCntKeySet < MAX_KEYSET; nCntKeySet++)
		{
			g_aPlayer.aMotion[nCntMotion].aKeyset[nCntKeySet].nMaxFrame = 40;//最大フレーム数

			for (int nCntKey = 0; nCntKey < MAX_PARTS; nCntKey++)
			{
				g_aPlayer.aMotion[nCntMotion].aKeyset[nCntKeySet].aKey[nCntKey].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);//位置
				g_aPlayer.aMotion[nCntMotion].aKeyset[nCntKeySet].aKey[nCntKey].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);//向き
			}
		}

		g_aPlayer.aMotion2[nCntMotion] = g_aPlayer.aMotion[nCntMotion];
	}

	//パーツ情報の読み込み
	LoadParts();

	//モーション情報の読み込み
	LoadMotion();

	//各パーツモデル情報
	for (int nCntParts = 0; nCntParts < g_aPlayer.nParts; nCntParts++)
	{
		//Xファイルの読み込み
		D3DXLoadMeshFromX(g_aPlayer.aParts[nCntParts].aPartsFile, D3DXMESH_SYSTEMMEM, pDevice, NULL, &g_aPlayer.aParts[nCntParts].pBuffMat, NULL, &g_aPlayer.aParts[nCntParts].dwNumMat, &g_aPlayer.aParts[nCntParts].pMesh);

		//マテリアル情報へのポインタを取得
		pMat = (D3DXMATERIAL*)g_aPlayer.aParts[nCntParts].pBuffMat->GetBufferPointer();

		//モデルのマテリアル情報の初期値を保存
		for (int nCntMat = 0; nCntMat < (int)g_aPlayer.aParts[nCntParts].dwNumMat; nCntMat++)
		{
			g_aPlayer.aParts[nCntParts].Diffuse[nCntMat] = pMat[nCntMat].MatD3D.Diffuse;
			g_aPlayer.aParts[nCntParts].FirstDiffuse[nCntMat] = g_aPlayer.aParts[nCntParts].Diffuse[nCntMat];
		}
	}

	//=====================================================
	// 各モーション全体のフレーム数を計算
	//=====================================================     
	for (int nCntMotion = 0; nCntMotion < g_nMaxMotion + 1; nCntMotion++)
	{
		for (int nCntKeySet = 0; nCntKeySet < g_aPlayer.aMotion[nCntMotion].nMaxKeySet + 1; nCntKeySet++)
		{
			g_nTotalFrame[nCntMotion] += g_aPlayer.aMotion[nCntMotion].aKeyset[nCntKeySet].nMaxFrame;
		}
	}
}

//============================================================================================================================================================================================================
// パーツ情報の読み込み
//============================================================================================================================================================================================================
void LoadParts(void)
{
	int nIndex = 0;//各パーツ番号
	char aDataSearch[MAX_TEXT];//データ読み込み用

	//ファイルを開く
	FILE* pFile = fopen(PLAYER_TXT, "r");

	//ファイルが開けない場合
	if (pFile == NULL)
	{
		return;
	}

	//ENDが見つかるまで読み込みを繰り返す
	while (1)
	{
		//検索
		int nResult = fscanf(pFile, "%s", aDataSearch);

		//END_PARTSが見つかった場合
		if (nResult == EOF)
		{//読み込み終了
			fclose(pFile);
			break;
		}

		//MOVEが見つかった場合
		else if (!strcmp(aDataSearch, "MOVE"))
		{
			//プレイヤーの速度を反映
			fscanf(pFile, "%f", &g_aPlayer.fSpeed);
		}

		//JUMPが見つかった場合
		else if (!strcmp(aDataSearch, "JUMP"))
		{
			//プレイヤーの速度を反映
			fscanf(pFile, "%f", &g_aPlayer.fJump);
		}

		//PARTSSETが見つかった場合
		else if (!strcmp(aDataSearch, "PARTSSET"))
		{
			//各パーツのデータを検索
			while (1)
			{
				//検索
				fscanf(pFile, "%s", aDataSearch);

				//END_PARTSSETが見つかった場合
				if (!strcmp(aDataSearch, "END_PARTSSET"))
				{
					//パーツ数カウントを加算
					g_aPlayer.nParts++;
					break;
				}

				//INDEXが見つかった場合
				else if (!strcmp(aDataSearch, "INDEX"))
				{
					//番号を保存
					fscanf(pFile, "%d", &nIndex);
				}

				//PATHが見つかった場合
				else if (!strcmp(aDataSearch, "PATH"))
				{
					//番号を保存
					fscanf(pFile, "%s", &g_aPlayer.aParts[nIndex].aPartsFile);
				}

				//PARENTが見つかった場合
				else if (!strcmp(aDataSearch, "PARENT"))
				{//親の番号を反映
					fscanf(pFile, "%d", &g_aPlayer.aParts[nIndex].nParent);
				}

				//HALFが見つかった場合
				else if (!strcmp(aDataSearch, "HALF"))
				{//上半身か下半身かの判定を反映
					fscanf(pFile, "%d", &g_aPlayer.aParts[nIndex].nHalf);
				}

				//POSが見つかった場合
				else if (!strcmp(aDataSearch, "POS"))
				{//位置を反映
					fscanf(pFile, "%f", &g_aPlayer.aParts[nIndex].pos.x);
					fscanf(pFile, "%f", &g_aPlayer.aParts[nIndex].pos.y);
					fscanf(pFile, "%f", &g_aPlayer.aParts[nIndex].pos.z);
				}

				//ROTが見つかった場合
				else if (!strcmp(aDataSearch, "ROT"))
				{//向きを反映
					fscanf(pFile, "%f", &g_aPlayer.aParts[nIndex].rot.x);
					fscanf(pFile, "%f", &g_aPlayer.aParts[nIndex].rot.y);
					fscanf(pFile, "%f", &g_aPlayer.aParts[nIndex].rot.z);
				}

				//上記以外が見つかった場合
				else
				{//その行をスキップして読み込みを続ける
					continue;
				}
			}
		}

		//上記以外が見つかった場合
		else
		{//スキップして読み込みを続ける
			continue;
		}
	}
}

//============================================================================================================================================================================================================
// モーション情報の読み込み処理
//============================================================================================================================================================================================================
void LoadMotion(void)
{
	int nCntParts = 0;//パーツ数
	int nCntKeySet = -1;//各モーションのキー数
	char aDataSearch[MAX_TEXT];//データ検索用

	//ファイルを開く
	FILE* pFile = fopen(PLAYER_TXT, "r");

	//ファイルが開けない場合
	if (pFile == NULL)
	{//読み込み処理を終了
		return;
	}

	//EOFが見つかるまで検索を繰り返す
	while (1)
	{
		//検索
		int nResult = fscanf(pFile, "%s", aDataSearch);	//検索結果をint型に代入

		//MOTIONSETが見つかった場合
		if (!strcmp(aDataSearch, "MOTIONSET"))
		{
			//モーション数をカウント
			g_nMaxMotion++;
			g_aPlayer.aMotion[g_nMaxMotion].nMaxKeySet = -1;

			//各モーションのデータを検索
			while (1)
			{
				//検索
				fscanf(pFile, "%s", aDataSearch);

				//END_MOTIONSETが見つかった場合
				if (!strcmp(aDataSearch, "END_MOTIONSET"))
				{//モーション読み込み終了
					nCntKeySet = -1;
					break;
				}

				//LOOPが見つかった場合
				if (!strcmp(aDataSearch, "LOOP"))
				{//ループ判定を取得
					fscanf(pFile, "%d", &g_aPlayer.aMotion[g_nMaxMotion].nLoop);
				}

				//KEYSETが見つかった場合
				if (!strcmp(aDataSearch, "KEYSET"))
				{
					//合計キー数を加算
					g_aPlayer.aMotion[g_nMaxMotion].nMaxKeySet++;
					nCntKeySet++;
					
					//各キーセットのデータを検索
					while (1)
					{
						//検索
						fscanf(pFile, "%s", aDataSearch);

						//KEYが見つかった場合
						if (!strcmp(aDataSearch, "KEY"))
						{
							//各キーのデータを検索
							while (1)
							{
								//検索
								fscanf(pFile, "%s", aDataSearch);

								//FRAMEが見つかった場合
								if (!strcmp(aDataSearch, "FRAME"))
								{//フレーム数を読み込む
									fscanf(pFile, "%d", &g_aPlayer.aMotion[g_nMaxMotion].aKeyset[nCntKeySet].nMaxFrame);
								}

								//POSが見つかった場合
								else if (!strcmp(aDataSearch, "POS"))
								{//位置情報を読み込む
									fscanf(pFile, "%f", &g_aPlayer.aMotion[g_nMaxMotion].aKeyset[nCntKeySet].aKey[nCntParts].pos.x);	//X座標
									fscanf(pFile, "%f", &g_aPlayer.aMotion[g_nMaxMotion].aKeyset[nCntKeySet].aKey[nCntParts].pos.y);	//Y座標
									fscanf(pFile, "%f", &g_aPlayer.aMotion[g_nMaxMotion].aKeyset[nCntKeySet].aKey[nCntParts].pos.z);	//Z座標
								}

								//ROTが見つかった場合
								else if (!strcmp(aDataSearch, "ROT"))
								{//向き情報を読み込む
									fscanf(pFile, "%f", &g_aPlayer.aMotion[g_nMaxMotion].aKeyset[nCntKeySet].aKey[nCntParts].rot.x);	//X軸
									fscanf(pFile, "%f", &g_aPlayer.aMotion[g_nMaxMotion].aKeyset[nCntKeySet].aKey[nCntParts].rot.y);	//Y軸
									fscanf(pFile, "%f", &g_aPlayer.aMotion[g_nMaxMotion].aKeyset[nCntKeySet].aKey[nCntParts].rot.z);	//Z軸
								}

								//END_KEYセットが見つかった場合
								else if (!strcmp(aDataSearch, "END_KEY"))
								{//モーションデータの読み込みを終了
									nCntParts++;	//パーツ数カウントを加算
									break;
								}
							}
						}

						//END_KEYSETが見つかった場合
						else if (!strcmp(aDataSearch, "END_KEYSET"))
						{
							nCntParts = 0;//パーツ数カウントをリセット
							break;
						}

						//上記以外が見つかった場合
						else
						{//スキップして読み込みを続ける
							continue;
						}
					}
				}

				//上記以外が見つかった場合
				else
				{//スキップして読み込みを続ける
					continue;
				}
			}
		}

		//EOFが見つかった場合
		else if (nResult == EOF)
		{
			//上半身の情報を下半身の情報にも反映
			for (int nCntMotion = 0; nCntMotion < g_nMaxMotion; nCntMotion++)
			{
				g_aPlayer.aMotion2[nCntMotion] = g_aPlayer.aMotion[nCntMotion];
			}

			//ファイルを閉じる
			fclose(pFile);
			break;
		}

		//上記以外が見つかった場合
		else
		{//検索を続ける
			continue;
		}
	}
}

//============================================================================================================================================================================================================
// 角度補正
//============================================================================================================================================================================================================
void CorrectionRot(float& fRot)
{
	if (fRot > D3DX_PI)
	{
		fRot = -D3DX_PI + (fRot - D3DX_PI);
	}
	else if (fRot < -D3DX_PI)
	{
		fRot = D3DX_PI + (fRot + D3DX_PI);
	}
}

//=======================================================================================================================================================================================================================
// 終了処理
//=======================================================================================================================================================================================================================
void UninitPlayer(void)
{
	for (int nCntParts = 0; nCntParts < g_aPlayer.nParts; nCntParts++)
	{
		//メッシュの破棄
		if (g_aPlayer.aParts[nCntParts].pMesh != NULL)
		{
			g_aPlayer.aParts[nCntParts].pMesh->Release();
			g_aPlayer.aParts[nCntParts].pMesh = NULL;
		}
		//マテリアルの破棄
		if (g_aPlayer.aParts[nCntParts].pBuffMat != NULL)
		{
			g_aPlayer.aParts[nCntParts].pBuffMat->Release();
			g_aPlayer.aParts[nCntParts].pBuffMat = NULL;
		}
	}
}

//=======================================================================================================================================================================================================================
// 更新処理
//=======================================================================================================================================================================================================================
void UpdatePlayer(void)
{
	//エディタとテストプレイの切り替え
	if (GetKeyboardTrigger(DIK_F2))
	{//F2キーが押された場合
		g_bEditMotion = g_bEditMotion ? false : true;
	}

	//上半身の情報を下半身の情報にも反映
	for (int nCntMotion = 0; nCntMotion < g_nMaxMotion + 1; nCntMotion++)
	{
		g_aPlayer.aMotion2[nCntMotion] = g_aPlayer.aMotion[nCntMotion];
	}

	//モーションエディタ中の場合
	if (g_bEditMotion)
	{//モーションの作成処理
		EditMotion();
	}
	//テストプレイ中の場合
	else
	{//プレイヤーの操作処理
		OperatePlayer();
	}

	//===============================================================================================================
	// モーション種類変更
	//===============================================================================================================
	if (GetKeyboardRepeat(DIK_3))
	{//3キーが押された場合
		//1つ戻る
		g_nMotion--;
	}
	else if (GetKeyboardRepeat(DIK_4))
	{//4キーが押された場合
		//1つ進む
		g_nMotion++;
	}

	if (g_nMotion < 0)
	{//0番目未満の場合
		g_nMotion = g_nMaxMotion;
	}
	else if (g_nMotion > g_nMaxMotion)
	{//総数を超えた場合
		g_nMotion = 0;
	}

	//===============================================================================================================
	// キーセット変更
	//===============================================================================================================
	if (GetKeyboardRepeat(DIK_7))
	{//7キーが押された場合
		g_nKeySet[g_nMotion]--;//1つ戻る
	}
	else if (GetKeyboardRepeat(DIK_8))
	{//8キーが押された場合
		g_nKeySet[g_nMotion]++;//1つ進む
	}

	if (g_nKeySet[g_nMotion] < 0)
	{//0番目未満の場合
		g_nKeySet[g_nMotion] = g_aPlayer.aMotion[g_nMotion].nMaxKeySet;
	}
	else if (g_nKeySet[g_nMotion] > g_aPlayer.aMotion[g_nMotion].nMaxKeySet)
	{//総数を超えた場合
		g_nKeySet[g_nMotion] = 0;
	}

	//===============================================================================================================
	// フレーム数変更
	//===============================================================================================================
	if (GetKeyboardRepeat(DIK_I))
	{//Iキーが押された場合
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].nMaxFrame--;
	}
	else if (GetKeyboardRepeat(DIK_K))
	{//Kキーが押された場合
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].nMaxFrame++;
	}

	if (g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].nMaxFrame <= 0)
	{//0以下になった場合
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].nMaxFrame = 1;//1に調整
	}
	else if (g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].nMaxFrame > MAX_KEYSET_FRAME)
	{//最大数に達した場合
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].nMaxFrame = MAX_KEYSET_FRAME;//最大に調整
	}
}

//============================================================================================================================================================================================================
// モーションの作成
//============================================================================================================================================================================================================
void EditMotion(void)
{
	//===============================================================================================================
	// モーション総数変更
	//===============================================================================================================
	if (GetKeyboardRepeat(DIK_1))
	{//1キーが押された場合
		//1つ減らす
		g_nMaxMotion--;
	}
	else if (GetKeyboardRepeat(DIK_2))
	{//2キーが押された場合
		//1つ増やす
		g_nMaxMotion++;
	}

	if (g_nMaxMotion < 0)
	{//1つ未満の場合
		g_nMaxMotion = 0;
	}
	else if (g_nMaxMotion > MAX_MOTION)
	{//最大数を超えた場合
		g_nMaxMotion = MAX_MOTION;
	}

	//===============================================================================================================
	// キーセット総数変更
	//===============================================================================================================
	if (GetKeyboardRepeat(DIK_5))
	{//5キーが押された場合
		g_aPlayer.aMotion[g_nMotion].nMaxKeySet--;//1つ減らす
	}
	else if (GetKeyboardRepeat(DIK_6))
	{//6キーが押された場合
		g_aPlayer.aMotion[g_nMotion].nMaxKeySet++;//1つ増やす
	}

	if (g_aPlayer.aMotion[g_nMotion].nMaxKeySet < 1)
	{//1つ未満の場合
		g_aPlayer.aMotion[g_nMotion].nMaxKeySet = 1;
	}
	else if (g_aPlayer.aMotion[g_nMotion].nMaxKeySet > MAX_KEYSET)
	{//最大数を超えた場合
		g_aPlayer.aMotion[g_nMotion].nMaxKeySet = MAX_KEYSET;
	}

	//===============================================================================================================
	// txtファイルへ出力
	//===============================================================================================================
	if (GetKeyboardTrigger(DIK_F3))
	{//F3キーが押された場合
		WriteMotion();
	}

	//===============================================================================================================
	// 現在のモーション情報をtxtファイルへ出力
	//===============================================================================================================
	if (GetKeyboardTrigger(DIK_F4))
	{//F4キーが押された場合
		WriteNowMotion();
	}

	//===============================================================================================================
	// キーコピー
	//===============================================================================================================
	if (GetKeyboardTrigger(DIK_F6))
	{//F6キーが押された場合

		//現在のパーツの位置と向きを保存
		for (int nCntParts = 0; nCntParts < g_aPlayer.nParts; nCntParts++)
		{
			g_aCopyKey[nCntParts].pos = g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[nCntParts].pos;
			g_aCopyKey[nCntParts].rot = g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[nCntParts].rot;
		}
	}

	//===============================================================================================================
	// キーペースト
	//===============================================================================================================
	if (GetKeyboardTrigger(DIK_F7))
	{//F7キーが押された場合

		//保存されているキー情報をペースト
		for (int nCntParts = 0; nCntParts < g_aPlayer.nParts; nCntParts++)
		{
			g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[nCntParts].pos = g_aCopyKey[nCntParts].pos;
			g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[nCntParts].rot = g_aCopyKey[nCntParts].rot;
		}
	}

	//===============================================================================================================
	// ループ切替
	//===============================================================================================================
	if (GetKeyboardTrigger(DIK_L))
	{//Lキーが押された場合
		if (g_aPlayer.aMotion[g_nMotion].nLoop == 0)
		{
			g_aPlayer.aMotion[g_nMotion].nLoop = 1;
		}
		else
		{
			g_aPlayer.aMotion[g_nMotion].nLoop = 0;
		}
	}

	//===============================================================================================================
	// パーツ選択
	//===============================================================================================================
	if (GetKeyboardRepeat(DIK_UP))
	{//↑キーが押された場合
		g_nParts--;//1つ戻る
	}
	else if (GetKeyboardRepeat(DIK_DOWN))
	{//↓キーが押された場合
		g_nParts++;//1つ進む
	}

	if (g_nParts < 0)
	{//パーツ番号が0未満の場合
		g_nParts = g_aPlayer.nParts - 1;
	}
	else if (g_nParts > g_aPlayer.nParts - 1)
	{//パーツ番号が最大パーツ数を超える場合
		g_nParts = 0;
	}

	//===============================================================================================================
	// 位置の変更
	//===============================================================================================================
	
	//================================================
	// X座標
	//================================================
	if (GetKeyboardRepeat(DIK_Q))
	{//Qキーが押された場合
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[0].pos.x += EDIT_POS;
	}
	else if (GetKeyboardRepeat(DIK_A))
	{//Aキーが押された場合
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[0].pos.x -= EDIT_POS;
	}
	else if (GetKeyboardRepeat(DIK_Z))
	{//Zキーが押された場合
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[0].pos.x = 0.0f;
	}

	//================================================
	// Y座標
	//================================================
	if (GetKeyboardRepeat(DIK_W))
	{//Wキーが押された場合
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[0].pos.y += EDIT_POS;
	}
	else if (GetKeyboardRepeat(DIK_S))
	{//Sキーが押された場合
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[0].pos.y -= EDIT_POS;
	}
	else if (GetKeyboardRepeat(DIK_X))
	{//Xキーが押された場合
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[0].pos.y = 0.0f;
	}

	//================================================
	// Z座標
	//================================================
	if (GetKeyboardRepeat(DIK_E))
	{//Eキーが押された場合
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[0].pos.z += EDIT_POS;
	}
	else if (GetKeyboardRepeat(DIK_D))
	{//Dキーが押された場合
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[0].pos.z -= EDIT_POS;
	}
	else if (GetKeyboardRepeat(DIK_C))
	{//Cキーが押された場合
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[0].pos.z = 0.0f;
	}

	//プレイヤーの位置情報に現在のキーセットのモーション位置情報を反映
	g_aPlayer.pos = g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[0].pos;

	//===============================================================================================================
	// 回転の変更
	//===============================================================================================================

	//================================================
	// X軸
	//================================================
	if (GetKeyboardRepeat(DIK_T))
	{//Tキーが押された場合
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[g_nParts].rot.x += EDIT_ROT;
	}
	else if (GetKeyboardRepeat(DIK_G))
	{//Gキーが押された場合
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[g_nParts].rot.x -= EDIT_ROT;
	}
	else if (GetKeyboardRepeat(DIK_B))
	{//Bキーが押された場合
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[g_nParts].rot.x = 0.0f;
	}

	//角度補正
	CorrectionRot(g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[g_nParts].rot.x);

	//================================================
	// Y軸
	//================================================
	if (GetKeyboardRepeat(DIK_Y))
	{//Yキーが押された場合
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[g_nParts].rot.y += EDIT_ROT;
	}
	else if (GetKeyboardRepeat(DIK_H))
	{//Hキーが押された場合
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[g_nParts].rot.y -= EDIT_ROT;
	}
	else if (GetKeyboardRepeat(DIK_N))
	{//Nキーが押された場合
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[g_nParts].rot.y = 0.0f;
	}

	//角度補正
	CorrectionRot(g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[g_nParts].rot.y);

	//================================================
	// Z軸
	//================================================
	if (GetKeyboardRepeat(DIK_U))
	{//Uキーが押された場合
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[g_nParts].rot.z += EDIT_ROT;
	}
	else if (GetKeyboardRepeat(DIK_J))
	{//Jキーが押された場合
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[g_nParts].rot.z -= EDIT_ROT;
	}
	else if (GetKeyboardRepeat(DIK_M))
	{//Mキーが押された場合
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[g_nParts].rot.z = 0.0f;
	}

	//角度補正
	CorrectionRot(g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[g_nParts].rot.z);

	//パーツ情報に現在のキーセットのモーション情報を反映
	for (int nCntParts = 0; nCntParts < g_aPlayer.nParts; nCntParts++)
	{
		g_aPlayer.aParts[nCntParts].rot = g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[nCntParts].rot;
	}

	//===============================================================================================================
	// 全リセット
	//===============================================================================================================
	if (GetKeyboardTrigger(DIK_RETURN))
	{
		//位置リセット
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		//向きリセット
		for (int nCntParts = 0; nCntParts < g_aPlayer.nParts; nCntParts++)
		{
			g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[nCntParts].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		}
	}

	//===============================================================================================================
	// デバッグ表示
	//===============================================================================================================
	PrintDebugProc("[ モード切り替え ] : [ F2 ] 【 Editモード 】\n\n");

	PrintDebugProc("【 MOTION : %d / %d 】\n", g_nMotion, g_nMaxMotion);
	PrintDebugProc("[ モーション総数変更 ] : [ 1,2 ]\n");
	PrintDebugProc("[ モーション種類変更 ] : [ 3,4 ]\n\n");

	PrintDebugProc("【 KEY : %d / %d 】\n", g_nKeySet[g_nMotion], g_aPlayer.aMotion[g_nMotion].nMaxKeySet);
	PrintDebugProc("[ キー総数変更 ] : [ 5,6 ]\n");
	PrintDebugProc("[ キー変更 ] : [ 7,8 ]\n\n");

	PrintDebugProc("[ txtファイル出力 ] : [ F3 ]\n");
	PrintDebugProc("[ 現在のモーションを出力 ] : [ F4 ]\n");
	PrintDebugProc("[ キーをコピー ] : [ F6 ]\n");
	PrintDebugProc("[ キーをペースト ] : [ F7 ]\n\n");

	PrintDebugProc("[ フレーム数 : %d ] : [ I,K ]\n", g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].nMaxFrame);
	PrintDebugProc("[ ループ : %d ] : [ L ] ( 0 : OFF , 1 : ON )\n\n", g_aPlayer.aMotion[g_nMotion].nLoop);

	PrintDebugProc(" -------------------------------------------\n");
	PrintDebugProc("| パーツ選択 : [↑,↓]　　パーツNo [%d / %d]\n| [ %s ]\n|\n", g_nParts, g_aPlayer.nParts - 1, g_aPlayer.aParts[g_nParts].aPartsFile);
	PrintDebugProc("| 【 向き 】\n");
	PrintDebugProc("| [ T,G ] : X軸回転　[ B ] : X軸リセット\n");
	PrintDebugProc("| [ Y,H ] : Y軸回転　[ N ] : Y軸リセット\n");
	PrintDebugProc("| [ U,J ] : Z軸回転　[ M ] : Z軸リセット\n|\n");
	PrintDebugProc("|  ROT ( %f, %f, %f )\n",
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[g_nParts].rot.x, 
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[g_nParts].rot.y, 
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[g_nParts].rot.z);
	PrintDebugProc(" -------------------------------------------\n\n");

	PrintDebugProc("【 位置 】\n");
	PrintDebugProc("[ Q,A ] : X座標　[ Z ] : X座標リセット\n");
	PrintDebugProc("[ W,S ] : Y座標　[ X ] : Y座標リセット\n");
	PrintDebugProc("[ E,D ] : Z座標　[ C ] : Z座標リセット\n\n");

	PrintDebugProc(" POS ( %f, %f, %f )\n\n", g_aPlayer.pos.x, g_aPlayer.pos.y, g_aPlayer.pos.z);

	PrintDebugProc("[ Enter ] : 向き、位置リセット\n\n");

	g_aPlayer.aMotion2[g_nMotion] = g_aPlayer.aMotion[g_nMotion];
}

//============================================================================================================================================================================================================
// モーション情報の書き出し
//============================================================================================================================================================================================================
void WriteMotion(void)
{
	//ファイルを開く
	FILE* pFile = fopen(PLAYER_TXT, "w");

	if (pFile != NULL)
	{
		//==============================================================================================================================
		// キャラクター情報
		//==============================================================================================================================
		fprintf(pFile, "#------------------------------------------------------------------------------\n");
		fprintf(pFile, "# キャラクター情報\n");
		fprintf(pFile, "#------------------------------------------------------------------------------\n");
		fprintf(pFile, "MOVE	%.2f\n", g_aPlayer.fSpeed);//速度
		fprintf(pFile, "JUMP	%.2f\n\n", g_aPlayer.fJump);//ジャンプ上昇量

		for (int nCntParts = 0; nCntParts < g_aPlayer.nParts; nCntParts++)
		{//各パーツの情報
			fprintf(pFile, "PARTSSET\n");
			fprintf(pFile, "	INDEX	%d\n", nCntParts);//番号
			fprintf(pFile, "	PATH	%s\n", g_aPlayer.aParts[nCntParts].aPartsFile);//番号
			fprintf(pFile, "	PARENT	%d\n", g_aPlayer.aParts[nCntParts].nParent);//親の番号
			fprintf(pFile, "	HALF	%d\n", g_aPlayer.aParts[nCntParts].nHalf);//上半身or下半身
			fprintf(pFile, "	POS	%.2f %.2f %.2f\n", g_aPlayer.aParts[nCntParts].pos.x, g_aPlayer.aParts[nCntParts].pos.y, g_aPlayer.aParts[nCntParts].pos.z);//位置
			fprintf(pFile, "	ROT	%.2f %.2f %.2f\n", g_aPlayer.aParts[nCntParts].rot.x, g_aPlayer.aParts[nCntParts].rot.y, g_aPlayer.aParts[nCntParts].rot.z);//向き
			fprintf(pFile, "END_PARTSSET\n\n");
		}

		fprintf(pFile, "\nEND_PARTS\n\n");

		//==============================================================================================================================
		// モーション情報
		//==============================================================================================================================
		fprintf(pFile, "#------------------------------------------------------------------------------\n");
		fprintf(pFile, "# モーション情報\n");
		fprintf(pFile, "#------------------------------------------------------------------------------\n");

		for (int nCntMotion = 0; nCntMotion < g_nMaxMotion + 1; nCntMotion++)
		{
			//モーション名
			fprintf(pFile, "\n#----------------------------------------\n");
			fprintf(pFile, "# アクション[%d]\n", nCntMotion);
			fprintf(pFile, "#----------------------------------------\n");
		
			fprintf(pFile, "MOTIONSET\n");
			fprintf(pFile, "	LOOP	%d	# ループしない(0)、ループする(1)", g_aPlayer.aMotion[nCntMotion].nLoop);//ループ判定

			for (int nCntKeySet = 0; nCntKeySet < g_aPlayer.aMotion[nCntMotion].nMaxKeySet + 1; nCntKeySet++)
			{
				fprintf(pFile, "\n	KEYSET			# << KEY : %d / %d >>\n", nCntKeySet, g_aPlayer.aMotion[nCntMotion].nMaxKeySet);//キーセット番号
				fprintf(pFile, "		FRAME	%d\n", g_aPlayer.aMotion[nCntMotion].aKeyset[nCntKeySet].nMaxFrame);//フレーム数

				for (int nCntKey = 0; nCntKey < g_aPlayer.nParts; nCntKey++)
				{
					//キー番号
					fprintf(pFile, "		KEY #----- [%d / %d] -----\n", nCntKey, g_aPlayer.nParts - 1);

					//位置
					fprintf(pFile, "			POS %.2f %.2f %.2f\n", 
						g_aPlayer.aMotion[nCntMotion].aKeyset[nCntKeySet].aKey[nCntKey].pos.x, 
						g_aPlayer.aMotion[nCntMotion].aKeyset[nCntKeySet].aKey[nCntKey].pos.y, 
						g_aPlayer.aMotion[nCntMotion].aKeyset[nCntKeySet].aKey[nCntKey].pos.z);

					//向き
					fprintf(pFile, "			ROT %.2f %.2f %.2f\n",
						g_aPlayer.aMotion[nCntMotion].aKeyset[nCntKeySet].aKey[nCntKey].rot.x,
						g_aPlayer.aMotion[nCntMotion].aKeyset[nCntKeySet].aKey[nCntKey].rot.y,
						g_aPlayer.aMotion[nCntMotion].aKeyset[nCntKeySet].aKey[nCntKey].rot.z);

					fprintf(pFile, "		END_KEY\n");
				}

				fprintf(pFile, "	END_KEYSET\n");
			}

			fprintf(pFile, "END_MOTIONSET\n");
		}

		fprintf(pFile, "\nEND_MOTION\n");

		//ファイルを閉じる
		fclose(pFile);
	}
}

//============================================================================================================================================================================================================
// 現在のモーションの書き出し
//============================================================================================================================================================================================================
void WriteNowMotion(void)
{
	//ファイルを開く
	FILE* pFile = fopen("data/ActionMotion.txt", "w");

	if (pFile != NULL)
	{
		//==============================================================================================================================
		// モーション情報
		//==============================================================================================================================
	
		//モーション名
		fprintf(pFile, "#----------------------------------------\n");
		fprintf(pFile, "# アクション[%d]\n", g_nMotion);
		fprintf(pFile, "#----------------------------------------\n");

		fprintf(pFile, "MOTIONSET\n");
		fprintf(pFile, "	LOOP	%d	# ループしない(0)、ループする(1)", g_aPlayer.aMotion[g_nMotion].nLoop);//ループ判定

		for (int nCntKeySet = 0; nCntKeySet < g_aPlayer.aMotion[g_nMotion].nMaxKeySet + 1; nCntKeySet++)
		{
			fprintf(pFile, "\n	KEYSET			# << KEY : %d / %d >>\n", nCntKeySet, g_aPlayer.aMotion[g_nMotion].nMaxKeySet);//キーセット番号
			fprintf(pFile, "		FRAME	%d\n", g_aPlayer.aMotion[g_nMotion].aKeyset[nCntKeySet].nMaxFrame);//フレーム数

			for (int nCntKey = 0; nCntKey < g_aPlayer.nParts; nCntKey++)
			{
				//キー番号
				fprintf(pFile, "		KEY #----- [%d / %d] -----\n", nCntKey, g_aPlayer.nParts - 1);

				//位置
				fprintf(pFile, "			POS %.2f %.2f %.2f\n",
					g_aPlayer.aMotion[g_nMotion].aKeyset[nCntKeySet].aKey[nCntKey].pos.x,
					g_aPlayer.aMotion[g_nMotion].aKeyset[nCntKeySet].aKey[nCntKey].pos.y,
					g_aPlayer.aMotion[g_nMotion].aKeyset[nCntKeySet].aKey[nCntKey].pos.z);

				//向き
				fprintf(pFile, "			ROT %.2f %.2f %.2f\n",
					g_aPlayer.aMotion[g_nMotion].aKeyset[nCntKeySet].aKey[nCntKey].rot.x,
					g_aPlayer.aMotion[g_nMotion].aKeyset[nCntKeySet].aKey[nCntKey].rot.y,
					g_aPlayer.aMotion[g_nMotion].aKeyset[nCntKeySet].aKey[nCntKey].rot.z);

				fprintf(pFile, "		END_KEY\n");
			}

			fprintf(pFile, "	END_KEYSET\n");
		}

		fprintf(pFile, "END_MOTIONSET\n");

		//ファイルを閉じる
		fclose(pFile);
	}
}

//============================================================================================================================================================================================================
// プレイヤーの操作
//============================================================================================================================================================================================================
void OperatePlayer(void)
{
	Camera* pCamera = GetCamera();//カメラの情報の取得
	Field* pField = GetField();//フィールドの情報を取得
	Wall* pWall = GetWall();//壁の情報を取得

	if (g_bUpdate)
	{//更新処理が有効になっている場合

		//状態別処理
		switch (g_aPlayer.nState)
		{
		case 0://ニュートラル
			g_aPlayer.move.y = 0.0f;//重力リセット
			break;

		case 1://移動
			g_aPlayer.move.y = 0.0f;//重力リセット
			break;

		case 2://攻撃
			break;

		case 3://ジャンプ

			//だんだん上昇量を0に近づける
			g_aPlayer.move.y -= JUMP_SPEED;

			//上昇量が0になった場合
			if (g_aPlayer.move.y <= 0.0f)
			{
				g_aPlayer.nState = 0;	//ニュートラル状態
			}
			break;

		default:
			break;
		}

		//上移動
		if (GetKeyboardPress(DIK_W))
		{
			//左上移動
			if (GetKeyboardPress(DIK_A))
			{
				//移動量を加算
				g_aPlayer.move.x += sinf(pCamera->rot.y + D3DX_PI * -0.25f) * g_aPlayer.fSpeed;	//X軸
				g_aPlayer.move.z += cosf(pCamera->rot.y + D3DX_PI * -0.25f) * g_aPlayer.fSpeed;	//Z軸

				//向きを更新
				if (g_aPlayer.rot.y <= pCamera->rot.y + D3DX_PI * 0.75f && g_aPlayer.rot.y >= pCamera->rot.y + D3DX_PI * -0.25f)
				{
					g_aPlayer.rot.y += TURN_SPEED;	//右回転

					if (g_aPlayer.rot.y >= pCamera->rot.y + D3DX_PI * 0.75f)
					{
						g_aPlayer.rot.y = pCamera->rot.y + D3DX_PI * 0.75f;	//左上向き
					}
				}
				else
				{
					g_aPlayer.rot.y -= TURN_SPEED;	//左回転

					if (g_aPlayer.rot.y <= pCamera->rot.y + D3DX_PI * 0.75f)
					{
						g_aPlayer.rot.y = pCamera->rot.y + D3DX_PI * 0.75f;	//左上向き
					}
				}
			}
			//右上移動
			else if (GetKeyboardPress(DIK_D))
			{
				//移動量を加算
				g_aPlayer.move.x += sinf(pCamera->rot.y + D3DX_PI * 0.25f) * g_aPlayer.fSpeed;	//X軸
				g_aPlayer.move.z += cosf(pCamera->rot.y + D3DX_PI * 0.25f) * g_aPlayer.fSpeed;	//Z軸

				//向きを更新
				if (g_aPlayer.rot.y <= pCamera->rot.y + D3DX_PI * 0.25f && g_aPlayer.rot.y >= pCamera->rot.y + D3DX_PI * -0.75f)
				{
					g_aPlayer.rot.y -= TURN_SPEED;	//左回転

					if (g_aPlayer.rot.y <= pCamera->rot.y + D3DX_PI * -0.75f)
					{
						g_aPlayer.rot.y = pCamera->rot.y + D3DX_PI * -0.75f;//右上向き
					}
				}
				else
				{
					g_aPlayer.rot.y += TURN_SPEED;	//右回転

					if (g_aPlayer.rot.y >= pCamera->rot.y + D3DX_PI * -0.75f)
					{
						g_aPlayer.rot.y = pCamera->rot.y + D3DX_PI * -0.75f;//右上向き
					}
				}
			}
			//真上移動
			else
			{
				//移動量を加算
				g_aPlayer.move.x += sinf(pCamera->rot.y) * g_aPlayer.fSpeed;//X軸
				g_aPlayer.move.z += cosf(pCamera->rot.y) * g_aPlayer.fSpeed;//Z軸

				g_aPlayer.rot.y = pCamera->rot.y + D3DX_PI;	//上向き
			}
		}

		//下移動
		else if (GetKeyboardPress(DIK_S))
		{
			//左下移動
			if (GetKeyboardPress(DIK_A))
			{
				//移動量を加算
				g_aPlayer.move.x += sinf(pCamera->rot.y + D3DX_PI * -0.75f) * g_aPlayer.fSpeed;	//X軸
				g_aPlayer.move.z += cosf(pCamera->rot.y + D3DX_PI * -0.75f) * g_aPlayer.fSpeed;	//Z軸

				//向きを更新
				if (g_aPlayer.rot.y >= pCamera->rot.y + D3DX_PI * -0.75f && g_aPlayer.rot.y <= pCamera->rot.y + D3DX_PI * 0.25f)
				{
					g_aPlayer.rot.y += TURN_SPEED;	//右回転

					if (g_aPlayer.rot.y >= pCamera->rot.y + D3DX_PI * 0.25f)
					{
						g_aPlayer.rot.y = pCamera->rot.y + D3DX_PI * 0.25f;	//左下向き
					}
				}
				else
				{
					g_aPlayer.rot.y -= TURN_SPEED;	//左回転

					if (g_aPlayer.rot.y <= pCamera->rot.y + D3DX_PI * 0.25f)
					{
						g_aPlayer.rot.y = pCamera->rot.y + D3DX_PI * 0.25f;	//左下向き
					}
				}
			}
			//右下移動
			else if (GetKeyboardPress(DIK_D))
			{
				//移動量を加算
				g_aPlayer.move.x += sinf(pCamera->rot.y + D3DX_PI * 0.75f) * g_aPlayer.fSpeed;	//X軸
				g_aPlayer.move.z += cosf(pCamera->rot.y + D3DX_PI * 0.75f) * g_aPlayer.fSpeed;	//Z軸

				//向きを更新
				if (g_aPlayer.rot.y >= pCamera->rot.y + D3DX_PI * -0.25f && g_aPlayer.rot.y <= pCamera->rot.y + D3DX_PI * 0.75f)
				{
					g_aPlayer.rot.y -= TURN_SPEED;	//左回転

					if (g_aPlayer.rot.y <= pCamera->rot.y + D3DX_PI * -0.25f)
					{
						g_aPlayer.rot.y = pCamera->rot.y + D3DX_PI * -0.25f;//右下向き
					}
				}
				else
				{
					g_aPlayer.rot.y += TURN_SPEED;	//右回転

					if (g_aPlayer.rot.y >= pCamera->rot.y + D3DX_PI * -0.25f)
					{
						g_aPlayer.rot.y = pCamera->rot.y + D3DX_PI * -0.25f;//右下向き
					}
				}
			}
			//真下移動
			else
			{
				//移動量を加算
				g_aPlayer.move.x += sinf(pCamera->rot.y + D3DX_PI) * g_aPlayer.fSpeed;	//X軸
				g_aPlayer.move.z += cosf(pCamera->rot.y + D3DX_PI) * g_aPlayer.fSpeed;	//Z軸

				g_aPlayer.rot.y = pCamera->rot.y;	//下向き
			}
		}

		//左移動
		else if (GetKeyboardPress(DIK_A))
		{
			//移動量を加算
			g_aPlayer.move.x += sinf(pCamera->rot.y + D3DX_PI * -0.5f) * g_aPlayer.fSpeed;	//X軸
			g_aPlayer.move.z += cosf(pCamera->rot.y + D3DX_PI * -0.5f) * g_aPlayer.fSpeed;	//Z軸

			//向きを更新
			if (g_aPlayer.rot.y >= pCamera->rot.y + D3DX_PI * -0.5f && g_aPlayer.rot.y <= pCamera->rot.y + D3DX_PI * 0.5f)
			{
				g_aPlayer.rot.y += TURN_SPEED;	//右回転

				if (g_aPlayer.rot.y >= pCamera->rot.y + D3DX_PI * 0.5f)
				{
					g_aPlayer.rot.y = pCamera->rot.y + D3DX_PI * 0.5f;	//左向き
				}
			}
			else
			{
				g_aPlayer.rot.y -= TURN_SPEED;	//左回転

				if (g_aPlayer.rot.y <= pCamera->rot.y + D3DX_PI * 0.5f)
				{
					g_aPlayer.rot.y = pCamera->rot.y + D3DX_PI * 0.5f;	//左向き
				}
			}
		}

		//右移動
		else if (GetKeyboardPress(DIK_D))
		{
			//移動量を加算
			g_aPlayer.move.x += sinf(pCamera->rot.y + D3DX_PI * 0.5f) * g_aPlayer.fSpeed;	//X軸
			g_aPlayer.move.z += cosf(pCamera->rot.y + D3DX_PI * 0.5f) * g_aPlayer.fSpeed;	//Z軸

			//向きを更新
			if (g_aPlayer.rot.y >= pCamera->rot.y + D3DX_PI * -0.5f && g_aPlayer.rot.y <= pCamera->rot.y + D3DX_PI * 0.5f)
			{
				g_aPlayer.rot.y -= TURN_SPEED;	//左回転

				if (g_aPlayer.rot.y <= pCamera->rot.y + D3DX_PI * -0.5f)
				{
					g_aPlayer.rot.y = pCamera->rot.y + D3DX_PI * -0.5f;	//右向き
				}
			}
			else
			{
				g_aPlayer.rot.y += TURN_SPEED;	//右回転

				if (g_aPlayer.rot.y >= pCamera->rot.y + D3DX_PI * -0.5f)
				{
					g_aPlayer.rot.y = pCamera->rot.y + D3DX_PI * -0.5f;	//右向き
				}
			}
		}

		//ENTERキーが押された場合
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			g_aPlayer.nState = 2;	//攻撃状態
		}

		//SPACEキーが押された場合
		if (GetKeyboardTrigger(DIK_SPACE) && g_aPlayer.pos.y == pField->pos.y)
		{
			g_aPlayer.nState = 3;	//ジャンプ状態
			g_aPlayer.move.y = g_aPlayer.fJump;	//ジャンプ上昇量
		}

		//重力の加算
		g_aPlayer.move.y -= PLAYER_GRAVITY;

		//過去の位置を保存
		g_aPlayer.oldpos = g_aPlayer.pos;

		//移動量を加算
		g_aPlayer.pos += g_aPlayer.move;

		//移動量を減衰
		g_aPlayer.move.x += (0.0f - g_aPlayer.move.x) * SPEED_DERAY;
		g_aPlayer.move.z += (0.0f - g_aPlayer.move.z) * SPEED_DERAY;

		//===============================================================================================================
		// モーション処理
		//===============================================================================================================
		UpBodyMotion();		//上半身
		DownBodyMotion();	//下半身
	}

	//===============================================================================================================
	// 当たり判定
	//===============================================================================================================
	
	//===========================================================================================
	// 地面
	//===========================================================================================
	if (g_aPlayer.pos.y < pField->pos.y)
	{
		g_aPlayer.pos.y = pField->pos.y;//地面を超えないように調整

		//アクションorジャンプ状態ではない場合
		if (g_aPlayer.nState == 0 || g_aPlayer.nState == 1)
		{
			//動いていない場合
			if ((g_aPlayer.move.x <= 0.01f && g_aPlayer.move.x >= -0.01f) && (g_aPlayer.move.z <= 0.01f && g_aPlayer.move.z >= -0.01f))
			{
				g_aPlayer.nState = 0;	//ニュートラル状態
			}
			//動いている場合
			else
			{
				g_aPlayer.nState = 1;	//移動状態
			}
		}
	}
	else
	{
		g_aPlayer.nState = 3;	//ジャンプ状態
	}

	//===========================================================================================
	// 壁
	//===========================================================================================
	for (int nCntWall = 0; nCntWall < MAX_WALL; nCntWall++, pWall++)
	{
		//1枚目(上)
		if (g_aPlayer.pos.z > pWall->pos.z - COLLISION_WALL && pWall->nCnt == 0)
		{
			g_aPlayer.pos.z = pWall->pos.z - COLLISION_WALL;
		}
		//2枚目(右)
		if (g_aPlayer.pos.x > pWall->pos.x - COLLISION_WALL && pWall->nCnt == 1)
		{
			g_aPlayer.pos.x = pWall->pos.x - COLLISION_WALL;
		}
		//3枚目(下)
		if (g_aPlayer.pos.z < pWall->pos.z + COLLISION_WALL && pWall->nCnt == 2)
		{
			g_aPlayer.pos.z = pWall->pos.z + COLLISION_WALL;
		}
		//4枚目(左)
		if (g_aPlayer.pos.x < pWall->pos.x + COLLISION_WALL && pWall->nCnt == 3)
		{
			g_aPlayer.pos.x = pWall->pos.x + COLLISION_WALL;
		}
	}

	//===============================================================================================================
	// 移動速度調整
	//===============================================================================================================
	if (GetKeyboardRepeat(DIK_Y))
	{//Yキーが押された場合
		g_aPlayer.fSpeed += ADD_SPEED;
	}
	else if (GetKeyboardRepeat(DIK_H))
	{//Hキーが押された場合
		g_aPlayer.fSpeed -= ADD_SPEED;
	}

	//===============================================================================================================
	// ジャンプ量調整
	//===============================================================================================================
	if (GetKeyboardRepeat(DIK_U))
	{//Uキーが押された場合
		g_aPlayer.fJump += ADD_JUMP;
	}
	else if (GetKeyboardRepeat(DIK_J))
	{//Jキーが押された場合
		g_aPlayer.fJump -= ADD_JUMP;
	}

	//===============================================================================================================
	// 最初から再生
	//===============================================================================================================
	if (GetKeyboardTrigger(DIK_F4))
	{//F4キーが押された場合
		
		//現在のキーとフレーム数をリセット
		g_aPlayer.aMotion[g_aPlayer.nState].nCntKeySet = 0;
		g_aPlayer.aMotion[g_aPlayer.nState].nCntFrame = 0;
		g_nCntFrame[g_aPlayer.nState] = 0;
	}

	//===============================================================================================================
	// 更新処理有効化切替
	//===============================================================================================================
	if (GetKeyboardTrigger(DIK_P))
	{//Pキーが押された場合
		g_bUpdate = g_bUpdate ? false : true;
	}

	//===============================================================================================================
	// デバッグ表示
	//===============================================================================================================
	PrintDebugProc("[ モード切り替え ] : [ F2 ] 【 プレイモード 】\n\n");

	PrintDebugProc("【 MOTION : %d / %d 】\n", g_nMotion, g_nMaxMotion);
	PrintDebugProc("[ モーション種類変更 ] : [ 3,4 ]\n\n");

	PrintDebugProc("【 KEY : %d / %d 】\n", g_nKeySet[g_nMotion], g_aPlayer.aMotion[g_nMotion].nMaxKeySet);
	PrintDebugProc("[ キー変更 ] : [ 7,8 ]\n\n");

	PrintDebugProc("[ フレーム数 : %d ] : [ I,K ]\n", g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].nMaxFrame);
	PrintDebugProc("[ ループ : %d ] : [ L ] ( 0 : OFF , 1 : ON )\n\n", g_aPlayer.aMotion[g_nMotion].nLoop);

	PrintDebugProc("[ スピード : %f ] : [ Y,H ]\n", g_aPlayer.fSpeed);
	PrintDebugProc("[ ジャンプ量 : %f ] : [ U,J ]\n\n", g_aPlayer.fJump);

	PrintDebugProc("[ 最初から再生 ] : [ F4 ]\n");
	PrintDebugProc("[ 一時停止 ] : [ P ]\n\n");

	PrintDebugProc("[ 現在のモーション ] : %d\n", g_aPlayer.nState);
	PrintDebugProc("[ 現在のフレーム数 ] : [ %d / %d ]\n", g_nCntFrame[g_aPlayer.nState], g_nTotalFrame[g_aPlayer.nState]);
	PrintDebugProc("[ 現在のキーセット ] : [ %d / %d ]\n\n", g_aPlayer.aMotion[g_aPlayer.nState].nCntKeySet, g_aPlayer.aMotion[g_aPlayer.nState].nMaxKeySet);
}

//============================================================================================================================================================================================================
// 上半身のモーション処理
//============================================================================================================================================================================================================
void UpBodyMotion(void)
{
	int nState = g_aPlayer.nState;//プレイヤーの状態
	int nKeySet = g_aPlayer.aMotion[nState].nCntKeySet;//現在のキーを保存
	int nNextKeySet = (nKeySet + 1) % (g_aPlayer.aMotion[nState].nMaxKeySet + 1);//次のキーを求める
	float fFrame = (float)g_aPlayer.aMotion[nState].nCntFrame / g_aPlayer.aMotion[nState].aKeyset[nKeySet].nMaxFrame;//現在のフレームと全体フレームの割合

	//パーツごとに情報を更新
	for (int nCntParts = 0; nCntParts < g_aPlayer.nParts; nCntParts++)
	{
		//上半身の場合
		if (g_aPlayer.aParts[nCntParts].nHalf == 0)
		{
			//==========================================================================================================================================================
			// 次のキーまでの差分を求める
			//==========================================================================================================================================================

			//角度
			float fRotx = g_aPlayer.aMotion[nState].aKeyset[nNextKeySet].aKey[nCntParts].rot.x - g_aPlayer.aMotion[nState].aKeyset[nKeySet].aKey[nCntParts].rot.x;//X軸
			float fRoty = g_aPlayer.aMotion[nState].aKeyset[nNextKeySet].aKey[nCntParts].rot.y - g_aPlayer.aMotion[nState].aKeyset[nKeySet].aKey[nCntParts].rot.y;//Y軸
			float fRotz = g_aPlayer.aMotion[nState].aKeyset[nNextKeySet].aKey[nCntParts].rot.z - g_aPlayer.aMotion[nState].aKeyset[nKeySet].aKey[nCntParts].rot.z;//Z軸

			//角度補正
			CorrectionRot(fRotx);
			CorrectionRot(fRoty);
			CorrectionRot(fRotz);

			//位置
			float fPosx = g_aPlayer.aMotion[nState].aKeyset[nNextKeySet].aKey[nCntParts].pos.x - g_aPlayer.aMotion[nState].aKeyset[nKeySet].aKey[nCntParts].pos.x;//X座標
			float fPosy = g_aPlayer.aMotion[nState].aKeyset[nNextKeySet].aKey[nCntParts].pos.y - g_aPlayer.aMotion[nState].aKeyset[nKeySet].aKey[nCntParts].pos.y;//Y座標
			float fPosz = g_aPlayer.aMotion[nState].aKeyset[nNextKeySet].aKey[nCntParts].pos.z - g_aPlayer.aMotion[nState].aKeyset[nKeySet].aKey[nCntParts].pos.z;//Z座標

			//==========================================================================================================================================================
			// 次のキーを反映
			//==========================================================================================================================================================

			//向きを更新
			g_aPlayer.aParts[nCntParts].rot.x = g_aPlayer.aMotion[nState].aKeyset[nKeySet].aKey[nCntParts].rot.x + fRotx * fFrame;	//X軸
			g_aPlayer.aParts[nCntParts].rot.y = g_aPlayer.aMotion[nState].aKeyset[nKeySet].aKey[nCntParts].rot.y + fRoty * fFrame;	//Y軸
			g_aPlayer.aParts[nCntParts].rot.z = g_aPlayer.aMotion[nState].aKeyset[nKeySet].aKey[nCntParts].rot.z + fRotz * fFrame;	//Z軸

			//角度補正
			CorrectionRot(g_aPlayer.aParts[nCntParts].rot.x);
			CorrectionRot(g_aPlayer.aParts[nCntParts].rot.y);
			CorrectionRot(g_aPlayer.aParts[nCntParts].rot.z);

			//位置を更新
			g_aPlayer.aParts[nCntParts].pos.x = g_aPlayer.aParts[nCntParts].pos.x + g_aPlayer.aMotion[nState].aKeyset[nKeySet].aKey[nCntParts].pos.x + fPosx * fFrame;	//X座標
			g_aPlayer.aParts[nCntParts].pos.y = g_aPlayer.aParts[nCntParts].pos.y + g_aPlayer.aMotion[nState].aKeyset[nKeySet].aKey[nCntParts].pos.y + fPosy * fFrame;	//Y座標
			g_aPlayer.aParts[nCntParts].pos.z = g_aPlayer.aParts[nCntParts].pos.z + g_aPlayer.aMotion[nState].aKeyset[nKeySet].aKey[nCntParts].pos.z + fPosz * fFrame;	//Z座標
		}
	}

	//カウントが指定のフレーム数になった場合
	if (g_aPlayer.aMotion[nState].nCntFrame >= g_aPlayer.aMotion[nState].aKeyset[nKeySet].nMaxFrame)
	{
		//フレーム数カウントを初期化
		g_aPlayer.aMotion[nState].nCntFrame = 0;

		//キー数を加算
		g_aPlayer.aMotion[nState].nCntKeySet++;

		//キー数が最大数になった
		if (g_aPlayer.aMotion[nState].nCntKeySet > g_aPlayer.aMotion[nState].nMaxKeySet)
		{
			//ループしない場合
			if (g_aPlayer.aMotion[nState].nLoop == 0)
			{
				//ニュートラル状態
				g_aPlayer.nState = 0;
			}

			//キー数を初期化
			g_aPlayer.aMotion[nState].nCntKeySet = 0;
		}
	}

	//合計フレームをリセット
	if (g_nCntFrame[nState] >= g_nTotalFrame[nState])
	{
		g_nCntFrame[nState] = 0;
	}

	//フレーム数更新
	g_aPlayer.aMotion[nState].nCntFrame++;

	//合計フレーム数更新
	g_nCntFrame[nState]++;
}

//============================================================================================================================================================================================================
// 下半身のモーション処理
//============================================================================================================================================================================================================
void DownBodyMotion(void)
{
	int nState = g_aPlayer.nState;	//プレイヤーの状態

	//動いている場合
	if((g_aPlayer.move.x > JUDGE_MOVE && g_aPlayer.move.x < -JUDGE_MOVE) && (g_aPlayer.move.z > JUDGE_MOVE && g_aPlayer.move.z < -JUDGE_MOVE))
	{
		nState = 1;	//移動状態
	}

	//現在のキーを保存
	int nKey = g_aPlayer.aMotion2[nState].nCntKeySet;

	//次のキーを求める
	int nNextKey = (nKey + 1) % (g_aPlayer.aMotion2[nState].nMaxKeySet + 1);

	//現在のフレームと全体フレームの割合
	float fFrame = (float)g_aPlayer.aMotion2[nState].nCntFrame / g_aPlayer.aMotion2[nState].aKeyset[nKey].nMaxFrame;

	//パーツごとに情報を更新
	for (int nCntParts = 0; nCntParts < g_aPlayer.nParts; nCntParts++)
	{
		//下半身の場合
		if (g_aPlayer.aParts[nCntParts].nHalf == 1)
		{
			//==========================================================================================================================================================
			// 次のキーまでの差分を求める
			//==========================================================================================================================================================

			//向き
			float fRotx = g_aPlayer.aMotion2[nState].aKeyset[nNextKey].aKey[nCntParts].rot.x - g_aPlayer.aMotion2[nState].aKeyset[nKey].aKey[nCntParts].rot.x;	//X軸
			float fRoty = g_aPlayer.aMotion2[nState].aKeyset[nNextKey].aKey[nCntParts].rot.y - g_aPlayer.aMotion2[nState].aKeyset[nKey].aKey[nCntParts].rot.y;	//Y軸
			float fRotz = g_aPlayer.aMotion2[nState].aKeyset[nNextKey].aKey[nCntParts].rot.z - g_aPlayer.aMotion2[nState].aKeyset[nKey].aKey[nCntParts].rot.z;	//Z軸

			//角度補正
			CorrectionRot(fRotx);
			CorrectionRot(fRoty);
			CorrectionRot(fRotz);

			//位置
			float fPosx = g_aPlayer.aMotion2[nState].aKeyset[nNextKey].aKey[nCntParts].pos.x - g_aPlayer.aMotion2[nState].aKeyset[nKey].aKey[nCntParts].pos.x;	//X座標
			float fPosy = g_aPlayer.aMotion2[nState].aKeyset[nNextKey].aKey[nCntParts].pos.y - g_aPlayer.aMotion2[nState].aKeyset[nKey].aKey[nCntParts].pos.y;	//Y座標
			float fPosz = g_aPlayer.aMotion2[nState].aKeyset[nNextKey].aKey[nCntParts].pos.z - g_aPlayer.aMotion2[nState].aKeyset[nKey].aKey[nCntParts].pos.z;	//Z座標

			//==========================================================================================================================================================
			// 次のキーを反映
			//==========================================================================================================================================================
			 
			//向きを更新
			g_aPlayer.aParts[nCntParts].rot.x = g_aPlayer.aMotion2[nState].aKeyset[nKey].aKey[nCntParts].rot.x + fRotx * fFrame;//X軸
			g_aPlayer.aParts[nCntParts].rot.y = g_aPlayer.aMotion2[nState].aKeyset[nKey].aKey[nCntParts].rot.y + fRoty * fFrame;//Y軸
			g_aPlayer.aParts[nCntParts].rot.z = g_aPlayer.aMotion2[nState].aKeyset[nKey].aKey[nCntParts].rot.z + fRotz * fFrame;//Z軸

			//角度補正
			CorrectionRot(g_aPlayer.aParts[nCntParts].rot.x);
			CorrectionRot(g_aPlayer.aParts[nCntParts].rot.y);
			CorrectionRot(g_aPlayer.aParts[nCntParts].rot.z);

			//位置を更新
			g_aPlayer.aParts[nCntParts].pos.x = g_aPlayer.aParts[nCntParts].pos.x + g_aPlayer.aMotion2[nState].aKeyset[nKey].aKey[nCntParts].pos.x + fPosx * fFrame;//X軸
			g_aPlayer.aParts[nCntParts].pos.y = g_aPlayer.aParts[nCntParts].pos.y + g_aPlayer.aMotion2[nState].aKeyset[nKey].aKey[nCntParts].pos.y + fPosy * fFrame;//Y軸
			g_aPlayer.aParts[nCntParts].pos.z = g_aPlayer.aParts[nCntParts].pos.z + g_aPlayer.aMotion2[nState].aKeyset[nKey].aKey[nCntParts].pos.z + fPosz * fFrame;//Z軸
		}
	}

	//カウントが指定のフレーム数になった場合
	if (g_aPlayer.aMotion2[nState].nCntFrame >= g_aPlayer.aMotion2[nState].aKeyset[nKey].nMaxFrame)
	{
		//フレーム数カウントを初期化
		g_aPlayer.aMotion2[nState].nCntFrame = 0;

		//キー数を加算
		g_aPlayer.aMotion2[nState].nCntKeySet++;

		//キー数が最大数になった
		if (g_aPlayer.aMotion2[nState].nCntKeySet > g_aPlayer.aMotion2[nState].nMaxKeySet)
		{
			//ループしない場合
			if (g_aPlayer.aMotion2[nState].nLoop == 0)
			{
				//ニュートラル状態
				g_aPlayer.nState = 0;
			}
			
			//キー数を初期化
			g_aPlayer.aMotion2[nState].nCntKeySet = 0;
		}
	}

	//フレーム数更新
	g_aPlayer.aMotion2[nState].nCntFrame++;
}

//============================================================================================================================================================================================================
// 描画処理
//============================================================================================================================================================================================================
void DrawPlayer(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	//デバイスの取得
	D3DXMATRIX mtxRot, mtxTrans, mtxParent;		//計算用マトリックス
	D3DMATERIAL9 matDef;						//現在のマテリアル保存用
	D3DXMATERIAL* pMat;							//マテリアルのデータへのポインタ

	//プレイヤーのワールドマトリックスの初期化
	D3DXMatrixIdentity(&g_aPlayer.mtxWorld);

	//プレイヤーの向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aPlayer.rot.y, g_aPlayer.rot.x, g_aPlayer.rot.z);
	D3DXMatrixMultiply(&g_aPlayer.mtxWorld, &g_aPlayer.mtxWorld, &mtxRot);

	//プレイヤーの位置を反映
	D3DXMatrixTranslation(&mtxTrans, g_aPlayer.pos.x, g_aPlayer.pos.y, g_aPlayer.pos.z);
	D3DXMatrixMultiply(&g_aPlayer.mtxWorld, &g_aPlayer.mtxWorld, &mtxTrans);

	for (int nCntParts = 0; nCntParts < g_aPlayer.nParts; nCntParts++)
	{
		//各パーツモデルのワールドマトリックスの初期化
		D3DXMatrixIdentity(&g_aPlayer.aParts[nCntParts].mtxWorld);

		//各パーツモデルの向きを反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aPlayer.aParts[nCntParts].rot.y, g_aPlayer.aParts[nCntParts].rot.x, g_aPlayer.aParts[nCntParts].rot.z);
		D3DXMatrixMultiply(&g_aPlayer.aParts[nCntParts].mtxWorld, &g_aPlayer.aParts[nCntParts].mtxWorld, &mtxRot);

		//各パーツモデルの位置を反映
		D3DXMatrixTranslation(&mtxTrans, g_aPlayer.aParts[nCntParts].pos.x, g_aPlayer.aParts[nCntParts].pos.y, g_aPlayer.aParts[nCntParts].pos.z);
		D3DXMatrixMultiply(&g_aPlayer.aParts[nCntParts].mtxWorld, &g_aPlayer.aParts[nCntParts].mtxWorld, &mtxTrans);

		//親の番号を保存
		int nParent = g_aPlayer.aParts[nCntParts].nParent;

		//親のマトリックスを反映
		if (nParent != -1)
		{//親の番号を持つ場合
			mtxParent = g_aPlayer.aParts[nParent].mtxWorld;	//親モデルのマトリックス
		}
		else
		{//親の番号を持たない場合
			mtxParent = g_aPlayer.mtxWorld;	//プレイヤーのマトリックス
		}

		//親のマトリックスを掛け合わせる
		D3DXMatrixMultiply(&g_aPlayer.aParts[nCntParts].mtxWorld, &g_aPlayer.aParts[nCntParts].mtxWorld, &mtxParent);

		//ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &g_aPlayer.aParts[nCntParts].mtxWorld);

		//現在のマテリアルを取得
		pDevice->GetMaterial(&matDef);

		//マテリアルデータへのポインタを取得
		pMat = (D3DXMATERIAL*)g_aPlayer.aParts[nCntParts].pBuffMat->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)g_aPlayer.aParts[nCntParts].dwNumMat; nCntMat++)
		{
			if (!g_bEditMotion)
			{//プレイモード中
				g_aPlayer.aParts[nCntParts].Diffuse[nCntMat] = g_aPlayer.aParts[nCntParts].FirstDiffuse[nCntMat];
			}
			else
			{//エディタ中
				if (nCntParts == g_nParts)
				{//現在選択しているパーツを半透明にする
					g_aPlayer.aParts[nCntParts].Diffuse[nCntMat].a = 0.3f;
				}
				else
				{//初期値のマテリアル情報にする
					g_aPlayer.aParts[nCntParts].Diffuse[nCntMat] = g_aPlayer.aParts[nCntParts].FirstDiffuse[nCntMat];
				}
			}

			//マテリアル情報を反映
			pMat[nCntMat].MatD3D.Diffuse = g_aPlayer.aParts[nCntParts].Diffuse[nCntMat];

			//マテリアルの設定
			pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

			//テクスチャの設定
			pDevice->SetTexture(0, NULL);

			//モデル(パーツ)の描画
			g_aPlayer.aParts[nCntParts].pMesh->DrawSubset(nCntMat);
		}

		//保存していたマテリアルを戻す
		pDevice->SetMaterial(&matDef);
	}
}

//============================================================================================================================================================================================================
// プレイヤーの情報を取得
//============================================================================================================================================================================================================
Player* GetPlayer(void)
{
	return &g_aPlayer;
}
