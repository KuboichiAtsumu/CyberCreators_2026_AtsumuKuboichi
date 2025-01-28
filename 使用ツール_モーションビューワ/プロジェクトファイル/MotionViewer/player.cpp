//=======================================================================================================================================================================================================================
//
// �v���C���[�Ɋւ��鏈��
// Author : Atsumu Kuboichi
//
//=======================================================================================================================================================================================================================
#include "player.h"
#include "camera.h"
#include "field.h"
#include "wall.h"
#include <stdio.h>

//=======================================================================================================================================================================================================================
// �O���[�o���ϐ�
//=======================================================================================================================================================================================================================
Player g_aPlayer;				//�v���C���[���
Key g_aCopyKey[MAX_PARTS];		//�L�[�Z�b�g�R�s�[���ۑ��p
int g_nParts;					//���݂̃p�[�c�ԍ�
int g_nMaxMotion;				//���[�V��������
int g_nMotion;					//���݂̃��[�V�����̎��
int g_nKeySet[MAX_MOTION];		//���݂̃L�[�Z�b�g
int g_nCntFrame[MAX_MOTION];	//�e���[�V�����̌��݂̃t���[����
int g_nTotalFrame[MAX_MOTION];	//�e���[�V�����̍��v�t���[����
bool g_bEditMotion = true;		//���[�V�����G�f�B�^�����ǂ����̔���
bool g_bUpdate = true;			//�X�V�����L�����ؑ�

//=======================================================================================================================================================================================================================
// ����������
//=======================================================================================================================================================================================================================
void InitPlayer(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�̎擾
	D3DXMATERIAL* pMat;						//�}�e���A���̃f�[�^�ւ̃|�C���^

	//�v���C���[���
	g_aPlayer.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	//�ʒu
	g_aPlayer.oldpos = g_aPlayer.pos;				//�ߋ��̈ʒu
	g_aPlayer.move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	//�ړ���
	g_aPlayer.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	//����
	g_aPlayer.nState = 0;							//���
	g_aPlayer.fSpeed = 0.0f;						//���x
	g_aPlayer.fJump = 0.0f;							//�W�����v�㏸��
	g_aPlayer.nParts = 0;							//�p�[�c��

	//�ϐ�
	g_nParts = 0;		//���݂̃p�[�c�ԍ�
	g_nMotion = 0;		//���݂̃��[�V�����̎��
	g_nMaxMotion = -1;	//�ő僂�[�V������
	
	//���[�V�������
	for (int nCntMotion = 0; nCntMotion < MAX_MOTION; nCntMotion++)
	{
		g_nKeySet[nCntMotion] = 0;//���݂̃L�[�Z�b�g
		g_nTotalFrame[nCntMotion] = 0;//�e���[�V�����̍��v�t���[����
		g_aPlayer.aMotion[nCntMotion].nLoop = 0;//���[�v����
		g_aPlayer.aMotion[nCntMotion].nMaxKeySet = 1;//���[�V�������Ƃ̃L�[�Z�b�g����

		for (int nCntKeySet = 0; nCntKeySet < MAX_KEYSET; nCntKeySet++)
		{
			g_aPlayer.aMotion[nCntMotion].aKeyset[nCntKeySet].nMaxFrame = 40;//�ő�t���[����

			for (int nCntKey = 0; nCntKey < MAX_PARTS; nCntKey++)
			{
				g_aPlayer.aMotion[nCntMotion].aKeyset[nCntKeySet].aKey[nCntKey].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);//�ʒu
				g_aPlayer.aMotion[nCntMotion].aKeyset[nCntKeySet].aKey[nCntKey].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);//����
			}
		}

		g_aPlayer.aMotion2[nCntMotion] = g_aPlayer.aMotion[nCntMotion];
	}

	//�p�[�c���̓ǂݍ���
	LoadParts();

	//���[�V�������̓ǂݍ���
	LoadMotion();

	//�e�p�[�c���f�����
	for (int nCntParts = 0; nCntParts < g_aPlayer.nParts; nCntParts++)
	{
		//X�t�@�C���̓ǂݍ���
		D3DXLoadMeshFromX(g_aPlayer.aParts[nCntParts].aPartsFile, D3DXMESH_SYSTEMMEM, pDevice, NULL, &g_aPlayer.aParts[nCntParts].pBuffMat, NULL, &g_aPlayer.aParts[nCntParts].dwNumMat, &g_aPlayer.aParts[nCntParts].pMesh);

		//�}�e���A�����ւ̃|�C���^���擾
		pMat = (D3DXMATERIAL*)g_aPlayer.aParts[nCntParts].pBuffMat->GetBufferPointer();

		//���f���̃}�e���A�����̏����l��ۑ�
		for (int nCntMat = 0; nCntMat < (int)g_aPlayer.aParts[nCntParts].dwNumMat; nCntMat++)
		{
			g_aPlayer.aParts[nCntParts].Diffuse[nCntMat] = pMat[nCntMat].MatD3D.Diffuse;
			g_aPlayer.aParts[nCntParts].FirstDiffuse[nCntMat] = g_aPlayer.aParts[nCntParts].Diffuse[nCntMat];
		}
	}

	//=====================================================
	// �e���[�V�����S�̂̃t���[�������v�Z
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
// �p�[�c���̓ǂݍ���
//============================================================================================================================================================================================================
void LoadParts(void)
{
	int nIndex = 0;//�e�p�[�c�ԍ�
	char aDataSearch[MAX_TEXT];//�f�[�^�ǂݍ��ݗp

	//�t�@�C�����J��
	FILE* pFile = fopen(PLAYER_TXT, "r");

	//�t�@�C�����J���Ȃ��ꍇ
	if (pFile == NULL)
	{
		return;
	}

	//END��������܂œǂݍ��݂��J��Ԃ�
	while (1)
	{
		//����
		int nResult = fscanf(pFile, "%s", aDataSearch);

		//END_PARTS�����������ꍇ
		if (nResult == EOF)
		{//�ǂݍ��ݏI��
			fclose(pFile);
			break;
		}

		//MOVE�����������ꍇ
		else if (!strcmp(aDataSearch, "MOVE"))
		{
			//�v���C���[�̑��x�𔽉f
			fscanf(pFile, "%f", &g_aPlayer.fSpeed);
		}

		//JUMP�����������ꍇ
		else if (!strcmp(aDataSearch, "JUMP"))
		{
			//�v���C���[�̑��x�𔽉f
			fscanf(pFile, "%f", &g_aPlayer.fJump);
		}

		//PARTSSET�����������ꍇ
		else if (!strcmp(aDataSearch, "PARTSSET"))
		{
			//�e�p�[�c�̃f�[�^������
			while (1)
			{
				//����
				fscanf(pFile, "%s", aDataSearch);

				//END_PARTSSET�����������ꍇ
				if (!strcmp(aDataSearch, "END_PARTSSET"))
				{
					//�p�[�c���J�E���g�����Z
					g_aPlayer.nParts++;
					break;
				}

				//INDEX�����������ꍇ
				else if (!strcmp(aDataSearch, "INDEX"))
				{
					//�ԍ���ۑ�
					fscanf(pFile, "%d", &nIndex);
				}

				//PATH�����������ꍇ
				else if (!strcmp(aDataSearch, "PATH"))
				{
					//�ԍ���ۑ�
					fscanf(pFile, "%s", &g_aPlayer.aParts[nIndex].aPartsFile);
				}

				//PARENT�����������ꍇ
				else if (!strcmp(aDataSearch, "PARENT"))
				{//�e�̔ԍ��𔽉f
					fscanf(pFile, "%d", &g_aPlayer.aParts[nIndex].nParent);
				}

				//HALF�����������ꍇ
				else if (!strcmp(aDataSearch, "HALF"))
				{//�㔼�g�������g���̔���𔽉f
					fscanf(pFile, "%d", &g_aPlayer.aParts[nIndex].nHalf);
				}

				//POS�����������ꍇ
				else if (!strcmp(aDataSearch, "POS"))
				{//�ʒu�𔽉f
					fscanf(pFile, "%f", &g_aPlayer.aParts[nIndex].pos.x);
					fscanf(pFile, "%f", &g_aPlayer.aParts[nIndex].pos.y);
					fscanf(pFile, "%f", &g_aPlayer.aParts[nIndex].pos.z);
				}

				//ROT�����������ꍇ
				else if (!strcmp(aDataSearch, "ROT"))
				{//�����𔽉f
					fscanf(pFile, "%f", &g_aPlayer.aParts[nIndex].rot.x);
					fscanf(pFile, "%f", &g_aPlayer.aParts[nIndex].rot.y);
					fscanf(pFile, "%f", &g_aPlayer.aParts[nIndex].rot.z);
				}

				//��L�ȊO�����������ꍇ
				else
				{//���̍s���X�L�b�v���ēǂݍ��݂𑱂���
					continue;
				}
			}
		}

		//��L�ȊO�����������ꍇ
		else
		{//�X�L�b�v���ēǂݍ��݂𑱂���
			continue;
		}
	}
}

//============================================================================================================================================================================================================
// ���[�V�������̓ǂݍ��ݏ���
//============================================================================================================================================================================================================
void LoadMotion(void)
{
	int nCntParts = 0;//�p�[�c��
	int nCntKeySet = -1;//�e���[�V�����̃L�[��
	char aDataSearch[MAX_TEXT];//�f�[�^�����p

	//�t�@�C�����J��
	FILE* pFile = fopen(PLAYER_TXT, "r");

	//�t�@�C�����J���Ȃ��ꍇ
	if (pFile == NULL)
	{//�ǂݍ��ݏ������I��
		return;
	}

	//EOF��������܂Ō������J��Ԃ�
	while (1)
	{
		//����
		int nResult = fscanf(pFile, "%s", aDataSearch);	//�������ʂ�int�^�ɑ��

		//MOTIONSET�����������ꍇ
		if (!strcmp(aDataSearch, "MOTIONSET"))
		{
			//���[�V���������J�E���g
			g_nMaxMotion++;
			g_aPlayer.aMotion[g_nMaxMotion].nMaxKeySet = -1;

			//�e���[�V�����̃f�[�^������
			while (1)
			{
				//����
				fscanf(pFile, "%s", aDataSearch);

				//END_MOTIONSET�����������ꍇ
				if (!strcmp(aDataSearch, "END_MOTIONSET"))
				{//���[�V�����ǂݍ��ݏI��
					nCntKeySet = -1;
					break;
				}

				//LOOP�����������ꍇ
				if (!strcmp(aDataSearch, "LOOP"))
				{//���[�v������擾
					fscanf(pFile, "%d", &g_aPlayer.aMotion[g_nMaxMotion].nLoop);
				}

				//KEYSET�����������ꍇ
				if (!strcmp(aDataSearch, "KEYSET"))
				{
					//���v�L�[�������Z
					g_aPlayer.aMotion[g_nMaxMotion].nMaxKeySet++;
					nCntKeySet++;
					
					//�e�L�[�Z�b�g�̃f�[�^������
					while (1)
					{
						//����
						fscanf(pFile, "%s", aDataSearch);

						//KEY�����������ꍇ
						if (!strcmp(aDataSearch, "KEY"))
						{
							//�e�L�[�̃f�[�^������
							while (1)
							{
								//����
								fscanf(pFile, "%s", aDataSearch);

								//FRAME�����������ꍇ
								if (!strcmp(aDataSearch, "FRAME"))
								{//�t���[������ǂݍ���
									fscanf(pFile, "%d", &g_aPlayer.aMotion[g_nMaxMotion].aKeyset[nCntKeySet].nMaxFrame);
								}

								//POS�����������ꍇ
								else if (!strcmp(aDataSearch, "POS"))
								{//�ʒu����ǂݍ���
									fscanf(pFile, "%f", &g_aPlayer.aMotion[g_nMaxMotion].aKeyset[nCntKeySet].aKey[nCntParts].pos.x);	//X���W
									fscanf(pFile, "%f", &g_aPlayer.aMotion[g_nMaxMotion].aKeyset[nCntKeySet].aKey[nCntParts].pos.y);	//Y���W
									fscanf(pFile, "%f", &g_aPlayer.aMotion[g_nMaxMotion].aKeyset[nCntKeySet].aKey[nCntParts].pos.z);	//Z���W
								}

								//ROT�����������ꍇ
								else if (!strcmp(aDataSearch, "ROT"))
								{//��������ǂݍ���
									fscanf(pFile, "%f", &g_aPlayer.aMotion[g_nMaxMotion].aKeyset[nCntKeySet].aKey[nCntParts].rot.x);	//X��
									fscanf(pFile, "%f", &g_aPlayer.aMotion[g_nMaxMotion].aKeyset[nCntKeySet].aKey[nCntParts].rot.y);	//Y��
									fscanf(pFile, "%f", &g_aPlayer.aMotion[g_nMaxMotion].aKeyset[nCntKeySet].aKey[nCntParts].rot.z);	//Z��
								}

								//END_KEY�Z�b�g�����������ꍇ
								else if (!strcmp(aDataSearch, "END_KEY"))
								{//���[�V�����f�[�^�̓ǂݍ��݂��I��
									nCntParts++;	//�p�[�c���J�E���g�����Z
									break;
								}
							}
						}

						//END_KEYSET�����������ꍇ
						else if (!strcmp(aDataSearch, "END_KEYSET"))
						{
							nCntParts = 0;//�p�[�c���J�E���g�����Z�b�g
							break;
						}

						//��L�ȊO�����������ꍇ
						else
						{//�X�L�b�v���ēǂݍ��݂𑱂���
							continue;
						}
					}
				}

				//��L�ȊO�����������ꍇ
				else
				{//�X�L�b�v���ēǂݍ��݂𑱂���
					continue;
				}
			}
		}

		//EOF�����������ꍇ
		else if (nResult == EOF)
		{
			//�㔼�g�̏��������g�̏��ɂ����f
			for (int nCntMotion = 0; nCntMotion < g_nMaxMotion; nCntMotion++)
			{
				g_aPlayer.aMotion2[nCntMotion] = g_aPlayer.aMotion[nCntMotion];
			}

			//�t�@�C�������
			fclose(pFile);
			break;
		}

		//��L�ȊO�����������ꍇ
		else
		{//�����𑱂���
			continue;
		}
	}
}

//============================================================================================================================================================================================================
// �p�x�␳
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
// �I������
//=======================================================================================================================================================================================================================
void UninitPlayer(void)
{
	for (int nCntParts = 0; nCntParts < g_aPlayer.nParts; nCntParts++)
	{
		//���b�V���̔j��
		if (g_aPlayer.aParts[nCntParts].pMesh != NULL)
		{
			g_aPlayer.aParts[nCntParts].pMesh->Release();
			g_aPlayer.aParts[nCntParts].pMesh = NULL;
		}
		//�}�e���A���̔j��
		if (g_aPlayer.aParts[nCntParts].pBuffMat != NULL)
		{
			g_aPlayer.aParts[nCntParts].pBuffMat->Release();
			g_aPlayer.aParts[nCntParts].pBuffMat = NULL;
		}
	}
}

//=======================================================================================================================================================================================================================
// �X�V����
//=======================================================================================================================================================================================================================
void UpdatePlayer(void)
{
	//�G�f�B�^�ƃe�X�g�v���C�̐؂�ւ�
	if (GetKeyboardTrigger(DIK_F2))
	{//F2�L�[�������ꂽ�ꍇ
		g_bEditMotion = g_bEditMotion ? false : true;
	}

	//�㔼�g�̏��������g�̏��ɂ����f
	for (int nCntMotion = 0; nCntMotion < g_nMaxMotion + 1; nCntMotion++)
	{
		g_aPlayer.aMotion2[nCntMotion] = g_aPlayer.aMotion[nCntMotion];
	}

	//���[�V�����G�f�B�^���̏ꍇ
	if (g_bEditMotion)
	{//���[�V�����̍쐬����
		EditMotion();
	}
	//�e�X�g�v���C���̏ꍇ
	else
	{//�v���C���[�̑��쏈��
		OperatePlayer();
	}

	//===============================================================================================================
	// ���[�V������ޕύX
	//===============================================================================================================
	if (GetKeyboardRepeat(DIK_3))
	{//3�L�[�������ꂽ�ꍇ
		//1�߂�
		g_nMotion--;
	}
	else if (GetKeyboardRepeat(DIK_4))
	{//4�L�[�������ꂽ�ꍇ
		//1�i��
		g_nMotion++;
	}

	if (g_nMotion < 0)
	{//0�Ԗږ����̏ꍇ
		g_nMotion = g_nMaxMotion;
	}
	else if (g_nMotion > g_nMaxMotion)
	{//�����𒴂����ꍇ
		g_nMotion = 0;
	}

	//===============================================================================================================
	// �L�[�Z�b�g�ύX
	//===============================================================================================================
	if (GetKeyboardRepeat(DIK_7))
	{//7�L�[�������ꂽ�ꍇ
		g_nKeySet[g_nMotion]--;//1�߂�
	}
	else if (GetKeyboardRepeat(DIK_8))
	{//8�L�[�������ꂽ�ꍇ
		g_nKeySet[g_nMotion]++;//1�i��
	}

	if (g_nKeySet[g_nMotion] < 0)
	{//0�Ԗږ����̏ꍇ
		g_nKeySet[g_nMotion] = g_aPlayer.aMotion[g_nMotion].nMaxKeySet;
	}
	else if (g_nKeySet[g_nMotion] > g_aPlayer.aMotion[g_nMotion].nMaxKeySet)
	{//�����𒴂����ꍇ
		g_nKeySet[g_nMotion] = 0;
	}

	//===============================================================================================================
	// �t���[�����ύX
	//===============================================================================================================
	if (GetKeyboardRepeat(DIK_I))
	{//I�L�[�������ꂽ�ꍇ
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].nMaxFrame--;
	}
	else if (GetKeyboardRepeat(DIK_K))
	{//K�L�[�������ꂽ�ꍇ
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].nMaxFrame++;
	}

	if (g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].nMaxFrame <= 0)
	{//0�ȉ��ɂȂ����ꍇ
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].nMaxFrame = 1;//1�ɒ���
	}
	else if (g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].nMaxFrame > MAX_KEYSET_FRAME)
	{//�ő吔�ɒB�����ꍇ
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].nMaxFrame = MAX_KEYSET_FRAME;//�ő�ɒ���
	}
}

//============================================================================================================================================================================================================
// ���[�V�����̍쐬
//============================================================================================================================================================================================================
void EditMotion(void)
{
	//===============================================================================================================
	// ���[�V���������ύX
	//===============================================================================================================
	if (GetKeyboardRepeat(DIK_1))
	{//1�L�[�������ꂽ�ꍇ
		//1���炷
		g_nMaxMotion--;
	}
	else if (GetKeyboardRepeat(DIK_2))
	{//2�L�[�������ꂽ�ꍇ
		//1���₷
		g_nMaxMotion++;
	}

	if (g_nMaxMotion < 0)
	{//1�����̏ꍇ
		g_nMaxMotion = 0;
	}
	else if (g_nMaxMotion > MAX_MOTION)
	{//�ő吔�𒴂����ꍇ
		g_nMaxMotion = MAX_MOTION;
	}

	//===============================================================================================================
	// �L�[�Z�b�g�����ύX
	//===============================================================================================================
	if (GetKeyboardRepeat(DIK_5))
	{//5�L�[�������ꂽ�ꍇ
		g_aPlayer.aMotion[g_nMotion].nMaxKeySet--;//1���炷
	}
	else if (GetKeyboardRepeat(DIK_6))
	{//6�L�[�������ꂽ�ꍇ
		g_aPlayer.aMotion[g_nMotion].nMaxKeySet++;//1���₷
	}

	if (g_aPlayer.aMotion[g_nMotion].nMaxKeySet < 1)
	{//1�����̏ꍇ
		g_aPlayer.aMotion[g_nMotion].nMaxKeySet = 1;
	}
	else if (g_aPlayer.aMotion[g_nMotion].nMaxKeySet > MAX_KEYSET)
	{//�ő吔�𒴂����ꍇ
		g_aPlayer.aMotion[g_nMotion].nMaxKeySet = MAX_KEYSET;
	}

	//===============================================================================================================
	// txt�t�@�C���֏o��
	//===============================================================================================================
	if (GetKeyboardTrigger(DIK_F3))
	{//F3�L�[�������ꂽ�ꍇ
		WriteMotion();
	}

	//===============================================================================================================
	// ���݂̃��[�V��������txt�t�@�C���֏o��
	//===============================================================================================================
	if (GetKeyboardTrigger(DIK_F4))
	{//F4�L�[�������ꂽ�ꍇ
		WriteNowMotion();
	}

	//===============================================================================================================
	// �L�[�R�s�[
	//===============================================================================================================
	if (GetKeyboardTrigger(DIK_F6))
	{//F6�L�[�������ꂽ�ꍇ

		//���݂̃p�[�c�̈ʒu�ƌ�����ۑ�
		for (int nCntParts = 0; nCntParts < g_aPlayer.nParts; nCntParts++)
		{
			g_aCopyKey[nCntParts].pos = g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[nCntParts].pos;
			g_aCopyKey[nCntParts].rot = g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[nCntParts].rot;
		}
	}

	//===============================================================================================================
	// �L�[�y�[�X�g
	//===============================================================================================================
	if (GetKeyboardTrigger(DIK_F7))
	{//F7�L�[�������ꂽ�ꍇ

		//�ۑ�����Ă���L�[�����y�[�X�g
		for (int nCntParts = 0; nCntParts < g_aPlayer.nParts; nCntParts++)
		{
			g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[nCntParts].pos = g_aCopyKey[nCntParts].pos;
			g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[nCntParts].rot = g_aCopyKey[nCntParts].rot;
		}
	}

	//===============================================================================================================
	// ���[�v�ؑ�
	//===============================================================================================================
	if (GetKeyboardTrigger(DIK_L))
	{//L�L�[�������ꂽ�ꍇ
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
	// �p�[�c�I��
	//===============================================================================================================
	if (GetKeyboardRepeat(DIK_UP))
	{//���L�[�������ꂽ�ꍇ
		g_nParts--;//1�߂�
	}
	else if (GetKeyboardRepeat(DIK_DOWN))
	{//���L�[�������ꂽ�ꍇ
		g_nParts++;//1�i��
	}

	if (g_nParts < 0)
	{//�p�[�c�ԍ���0�����̏ꍇ
		g_nParts = g_aPlayer.nParts - 1;
	}
	else if (g_nParts > g_aPlayer.nParts - 1)
	{//�p�[�c�ԍ����ő�p�[�c���𒴂���ꍇ
		g_nParts = 0;
	}

	//===============================================================================================================
	// �ʒu�̕ύX
	//===============================================================================================================
	
	//================================================
	// X���W
	//================================================
	if (GetKeyboardRepeat(DIK_Q))
	{//Q�L�[�������ꂽ�ꍇ
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[0].pos.x += EDIT_POS;
	}
	else if (GetKeyboardRepeat(DIK_A))
	{//A�L�[�������ꂽ�ꍇ
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[0].pos.x -= EDIT_POS;
	}
	else if (GetKeyboardRepeat(DIK_Z))
	{//Z�L�[�������ꂽ�ꍇ
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[0].pos.x = 0.0f;
	}

	//================================================
	// Y���W
	//================================================
	if (GetKeyboardRepeat(DIK_W))
	{//W�L�[�������ꂽ�ꍇ
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[0].pos.y += EDIT_POS;
	}
	else if (GetKeyboardRepeat(DIK_S))
	{//S�L�[�������ꂽ�ꍇ
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[0].pos.y -= EDIT_POS;
	}
	else if (GetKeyboardRepeat(DIK_X))
	{//X�L�[�������ꂽ�ꍇ
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[0].pos.y = 0.0f;
	}

	//================================================
	// Z���W
	//================================================
	if (GetKeyboardRepeat(DIK_E))
	{//E�L�[�������ꂽ�ꍇ
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[0].pos.z += EDIT_POS;
	}
	else if (GetKeyboardRepeat(DIK_D))
	{//D�L�[�������ꂽ�ꍇ
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[0].pos.z -= EDIT_POS;
	}
	else if (GetKeyboardRepeat(DIK_C))
	{//C�L�[�������ꂽ�ꍇ
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[0].pos.z = 0.0f;
	}

	//�v���C���[�̈ʒu���Ɍ��݂̃L�[�Z�b�g�̃��[�V�����ʒu���𔽉f
	g_aPlayer.pos = g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[0].pos;

	//===============================================================================================================
	// ��]�̕ύX
	//===============================================================================================================

	//================================================
	// X��
	//================================================
	if (GetKeyboardRepeat(DIK_T))
	{//T�L�[�������ꂽ�ꍇ
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[g_nParts].rot.x += EDIT_ROT;
	}
	else if (GetKeyboardRepeat(DIK_G))
	{//G�L�[�������ꂽ�ꍇ
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[g_nParts].rot.x -= EDIT_ROT;
	}
	else if (GetKeyboardRepeat(DIK_B))
	{//B�L�[�������ꂽ�ꍇ
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[g_nParts].rot.x = 0.0f;
	}

	//�p�x�␳
	CorrectionRot(g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[g_nParts].rot.x);

	//================================================
	// Y��
	//================================================
	if (GetKeyboardRepeat(DIK_Y))
	{//Y�L�[�������ꂽ�ꍇ
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[g_nParts].rot.y += EDIT_ROT;
	}
	else if (GetKeyboardRepeat(DIK_H))
	{//H�L�[�������ꂽ�ꍇ
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[g_nParts].rot.y -= EDIT_ROT;
	}
	else if (GetKeyboardRepeat(DIK_N))
	{//N�L�[�������ꂽ�ꍇ
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[g_nParts].rot.y = 0.0f;
	}

	//�p�x�␳
	CorrectionRot(g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[g_nParts].rot.y);

	//================================================
	// Z��
	//================================================
	if (GetKeyboardRepeat(DIK_U))
	{//U�L�[�������ꂽ�ꍇ
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[g_nParts].rot.z += EDIT_ROT;
	}
	else if (GetKeyboardRepeat(DIK_J))
	{//J�L�[�������ꂽ�ꍇ
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[g_nParts].rot.z -= EDIT_ROT;
	}
	else if (GetKeyboardRepeat(DIK_M))
	{//M�L�[�������ꂽ�ꍇ
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[g_nParts].rot.z = 0.0f;
	}

	//�p�x�␳
	CorrectionRot(g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[g_nParts].rot.z);

	//�p�[�c���Ɍ��݂̃L�[�Z�b�g�̃��[�V�������𔽉f
	for (int nCntParts = 0; nCntParts < g_aPlayer.nParts; nCntParts++)
	{
		g_aPlayer.aParts[nCntParts].rot = g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[nCntParts].rot;
	}

	//===============================================================================================================
	// �S���Z�b�g
	//===============================================================================================================
	if (GetKeyboardTrigger(DIK_RETURN))
	{
		//�ʒu���Z�b�g
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		//�������Z�b�g
		for (int nCntParts = 0; nCntParts < g_aPlayer.nParts; nCntParts++)
		{
			g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[nCntParts].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		}
	}

	//===============================================================================================================
	// �f�o�b�O�\��
	//===============================================================================================================
	PrintDebugProc("[ ���[�h�؂�ւ� ] : [ F2 ] �y Edit���[�h �z\n\n");

	PrintDebugProc("�y MOTION : %d / %d �z\n", g_nMotion, g_nMaxMotion);
	PrintDebugProc("[ ���[�V���������ύX ] : [ 1,2 ]\n");
	PrintDebugProc("[ ���[�V������ޕύX ] : [ 3,4 ]\n\n");

	PrintDebugProc("�y KEY : %d / %d �z\n", g_nKeySet[g_nMotion], g_aPlayer.aMotion[g_nMotion].nMaxKeySet);
	PrintDebugProc("[ �L�[�����ύX ] : [ 5,6 ]\n");
	PrintDebugProc("[ �L�[�ύX ] : [ 7,8 ]\n\n");

	PrintDebugProc("[ txt�t�@�C���o�� ] : [ F3 ]\n");
	PrintDebugProc("[ ���݂̃��[�V�������o�� ] : [ F4 ]\n");
	PrintDebugProc("[ �L�[���R�s�[ ] : [ F6 ]\n");
	PrintDebugProc("[ �L�[���y�[�X�g ] : [ F7 ]\n\n");

	PrintDebugProc("[ �t���[���� : %d ] : [ I,K ]\n", g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].nMaxFrame);
	PrintDebugProc("[ ���[�v : %d ] : [ L ] ( 0 : OFF , 1 : ON )\n\n", g_aPlayer.aMotion[g_nMotion].nLoop);

	PrintDebugProc(" -------------------------------------------\n");
	PrintDebugProc("| �p�[�c�I�� : [��,��]�@�@�p�[�cNo [%d / %d]\n| [ %s ]\n|\n", g_nParts, g_aPlayer.nParts - 1, g_aPlayer.aParts[g_nParts].aPartsFile);
	PrintDebugProc("| �y ���� �z\n");
	PrintDebugProc("| [ T,G ] : X����]�@[ B ] : X�����Z�b�g\n");
	PrintDebugProc("| [ Y,H ] : Y����]�@[ N ] : Y�����Z�b�g\n");
	PrintDebugProc("| [ U,J ] : Z����]�@[ M ] : Z�����Z�b�g\n|\n");
	PrintDebugProc("|  ROT ( %f, %f, %f )\n",
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[g_nParts].rot.x, 
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[g_nParts].rot.y, 
		g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].aKey[g_nParts].rot.z);
	PrintDebugProc(" -------------------------------------------\n\n");

	PrintDebugProc("�y �ʒu �z\n");
	PrintDebugProc("[ Q,A ] : X���W�@[ Z ] : X���W���Z�b�g\n");
	PrintDebugProc("[ W,S ] : Y���W�@[ X ] : Y���W���Z�b�g\n");
	PrintDebugProc("[ E,D ] : Z���W�@[ C ] : Z���W���Z�b�g\n\n");

	PrintDebugProc(" POS ( %f, %f, %f )\n\n", g_aPlayer.pos.x, g_aPlayer.pos.y, g_aPlayer.pos.z);

	PrintDebugProc("[ Enter ] : �����A�ʒu���Z�b�g\n\n");

	g_aPlayer.aMotion2[g_nMotion] = g_aPlayer.aMotion[g_nMotion];
}

//============================================================================================================================================================================================================
// ���[�V�������̏����o��
//============================================================================================================================================================================================================
void WriteMotion(void)
{
	//�t�@�C�����J��
	FILE* pFile = fopen(PLAYER_TXT, "w");

	if (pFile != NULL)
	{
		//==============================================================================================================================
		// �L�����N�^�[���
		//==============================================================================================================================
		fprintf(pFile, "#------------------------------------------------------------------------------\n");
		fprintf(pFile, "# �L�����N�^�[���\n");
		fprintf(pFile, "#------------------------------------------------------------------------------\n");
		fprintf(pFile, "MOVE	%.2f\n", g_aPlayer.fSpeed);//���x
		fprintf(pFile, "JUMP	%.2f\n\n", g_aPlayer.fJump);//�W�����v�㏸��

		for (int nCntParts = 0; nCntParts < g_aPlayer.nParts; nCntParts++)
		{//�e�p�[�c�̏��
			fprintf(pFile, "PARTSSET\n");
			fprintf(pFile, "	INDEX	%d\n", nCntParts);//�ԍ�
			fprintf(pFile, "	PATH	%s\n", g_aPlayer.aParts[nCntParts].aPartsFile);//�ԍ�
			fprintf(pFile, "	PARENT	%d\n", g_aPlayer.aParts[nCntParts].nParent);//�e�̔ԍ�
			fprintf(pFile, "	HALF	%d\n", g_aPlayer.aParts[nCntParts].nHalf);//�㔼�gor�����g
			fprintf(pFile, "	POS	%.2f %.2f %.2f\n", g_aPlayer.aParts[nCntParts].pos.x, g_aPlayer.aParts[nCntParts].pos.y, g_aPlayer.aParts[nCntParts].pos.z);//�ʒu
			fprintf(pFile, "	ROT	%.2f %.2f %.2f\n", g_aPlayer.aParts[nCntParts].rot.x, g_aPlayer.aParts[nCntParts].rot.y, g_aPlayer.aParts[nCntParts].rot.z);//����
			fprintf(pFile, "END_PARTSSET\n\n");
		}

		fprintf(pFile, "\nEND_PARTS\n\n");

		//==============================================================================================================================
		// ���[�V�������
		//==============================================================================================================================
		fprintf(pFile, "#------------------------------------------------------------------------------\n");
		fprintf(pFile, "# ���[�V�������\n");
		fprintf(pFile, "#------------------------------------------------------------------------------\n");

		for (int nCntMotion = 0; nCntMotion < g_nMaxMotion + 1; nCntMotion++)
		{
			//���[�V������
			fprintf(pFile, "\n#----------------------------------------\n");
			fprintf(pFile, "# �A�N�V����[%d]\n", nCntMotion);
			fprintf(pFile, "#----------------------------------------\n");
		
			fprintf(pFile, "MOTIONSET\n");
			fprintf(pFile, "	LOOP	%d	# ���[�v���Ȃ�(0)�A���[�v����(1)", g_aPlayer.aMotion[nCntMotion].nLoop);//���[�v����

			for (int nCntKeySet = 0; nCntKeySet < g_aPlayer.aMotion[nCntMotion].nMaxKeySet + 1; nCntKeySet++)
			{
				fprintf(pFile, "\n	KEYSET			# << KEY : %d / %d >>\n", nCntKeySet, g_aPlayer.aMotion[nCntMotion].nMaxKeySet);//�L�[�Z�b�g�ԍ�
				fprintf(pFile, "		FRAME	%d\n", g_aPlayer.aMotion[nCntMotion].aKeyset[nCntKeySet].nMaxFrame);//�t���[����

				for (int nCntKey = 0; nCntKey < g_aPlayer.nParts; nCntKey++)
				{
					//�L�[�ԍ�
					fprintf(pFile, "		KEY #----- [%d / %d] -----\n", nCntKey, g_aPlayer.nParts - 1);

					//�ʒu
					fprintf(pFile, "			POS %.2f %.2f %.2f\n", 
						g_aPlayer.aMotion[nCntMotion].aKeyset[nCntKeySet].aKey[nCntKey].pos.x, 
						g_aPlayer.aMotion[nCntMotion].aKeyset[nCntKeySet].aKey[nCntKey].pos.y, 
						g_aPlayer.aMotion[nCntMotion].aKeyset[nCntKeySet].aKey[nCntKey].pos.z);

					//����
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

		//�t�@�C�������
		fclose(pFile);
	}
}

//============================================================================================================================================================================================================
// ���݂̃��[�V�����̏����o��
//============================================================================================================================================================================================================
void WriteNowMotion(void)
{
	//�t�@�C�����J��
	FILE* pFile = fopen("data/ActionMotion.txt", "w");

	if (pFile != NULL)
	{
		//==============================================================================================================================
		// ���[�V�������
		//==============================================================================================================================
	
		//���[�V������
		fprintf(pFile, "#----------------------------------------\n");
		fprintf(pFile, "# �A�N�V����[%d]\n", g_nMotion);
		fprintf(pFile, "#----------------------------------------\n");

		fprintf(pFile, "MOTIONSET\n");
		fprintf(pFile, "	LOOP	%d	# ���[�v���Ȃ�(0)�A���[�v����(1)", g_aPlayer.aMotion[g_nMotion].nLoop);//���[�v����

		for (int nCntKeySet = 0; nCntKeySet < g_aPlayer.aMotion[g_nMotion].nMaxKeySet + 1; nCntKeySet++)
		{
			fprintf(pFile, "\n	KEYSET			# << KEY : %d / %d >>\n", nCntKeySet, g_aPlayer.aMotion[g_nMotion].nMaxKeySet);//�L�[�Z�b�g�ԍ�
			fprintf(pFile, "		FRAME	%d\n", g_aPlayer.aMotion[g_nMotion].aKeyset[nCntKeySet].nMaxFrame);//�t���[����

			for (int nCntKey = 0; nCntKey < g_aPlayer.nParts; nCntKey++)
			{
				//�L�[�ԍ�
				fprintf(pFile, "		KEY #----- [%d / %d] -----\n", nCntKey, g_aPlayer.nParts - 1);

				//�ʒu
				fprintf(pFile, "			POS %.2f %.2f %.2f\n",
					g_aPlayer.aMotion[g_nMotion].aKeyset[nCntKeySet].aKey[nCntKey].pos.x,
					g_aPlayer.aMotion[g_nMotion].aKeyset[nCntKeySet].aKey[nCntKey].pos.y,
					g_aPlayer.aMotion[g_nMotion].aKeyset[nCntKeySet].aKey[nCntKey].pos.z);

				//����
				fprintf(pFile, "			ROT %.2f %.2f %.2f\n",
					g_aPlayer.aMotion[g_nMotion].aKeyset[nCntKeySet].aKey[nCntKey].rot.x,
					g_aPlayer.aMotion[g_nMotion].aKeyset[nCntKeySet].aKey[nCntKey].rot.y,
					g_aPlayer.aMotion[g_nMotion].aKeyset[nCntKeySet].aKey[nCntKey].rot.z);

				fprintf(pFile, "		END_KEY\n");
			}

			fprintf(pFile, "	END_KEYSET\n");
		}

		fprintf(pFile, "END_MOTIONSET\n");

		//�t�@�C�������
		fclose(pFile);
	}
}

//============================================================================================================================================================================================================
// �v���C���[�̑���
//============================================================================================================================================================================================================
void OperatePlayer(void)
{
	Camera* pCamera = GetCamera();//�J�����̏��̎擾
	Field* pField = GetField();//�t�B�[���h�̏����擾
	Wall* pWall = GetWall();//�ǂ̏����擾

	if (g_bUpdate)
	{//�X�V�������L���ɂȂ��Ă���ꍇ

		//��ԕʏ���
		switch (g_aPlayer.nState)
		{
		case 0://�j���[�g����
			g_aPlayer.move.y = 0.0f;//�d�̓��Z�b�g
			break;

		case 1://�ړ�
			g_aPlayer.move.y = 0.0f;//�d�̓��Z�b�g
			break;

		case 2://�U��
			break;

		case 3://�W�����v

			//���񂾂�㏸�ʂ�0�ɋ߂Â���
			g_aPlayer.move.y -= JUMP_SPEED;

			//�㏸�ʂ�0�ɂȂ����ꍇ
			if (g_aPlayer.move.y <= 0.0f)
			{
				g_aPlayer.nState = 0;	//�j���[�g�������
			}
			break;

		default:
			break;
		}

		//��ړ�
		if (GetKeyboardPress(DIK_W))
		{
			//����ړ�
			if (GetKeyboardPress(DIK_A))
			{
				//�ړ��ʂ����Z
				g_aPlayer.move.x += sinf(pCamera->rot.y + D3DX_PI * -0.25f) * g_aPlayer.fSpeed;	//X��
				g_aPlayer.move.z += cosf(pCamera->rot.y + D3DX_PI * -0.25f) * g_aPlayer.fSpeed;	//Z��

				//�������X�V
				if (g_aPlayer.rot.y <= pCamera->rot.y + D3DX_PI * 0.75f && g_aPlayer.rot.y >= pCamera->rot.y + D3DX_PI * -0.25f)
				{
					g_aPlayer.rot.y += TURN_SPEED;	//�E��]

					if (g_aPlayer.rot.y >= pCamera->rot.y + D3DX_PI * 0.75f)
					{
						g_aPlayer.rot.y = pCamera->rot.y + D3DX_PI * 0.75f;	//�������
					}
				}
				else
				{
					g_aPlayer.rot.y -= TURN_SPEED;	//����]

					if (g_aPlayer.rot.y <= pCamera->rot.y + D3DX_PI * 0.75f)
					{
						g_aPlayer.rot.y = pCamera->rot.y + D3DX_PI * 0.75f;	//�������
					}
				}
			}
			//�E��ړ�
			else if (GetKeyboardPress(DIK_D))
			{
				//�ړ��ʂ����Z
				g_aPlayer.move.x += sinf(pCamera->rot.y + D3DX_PI * 0.25f) * g_aPlayer.fSpeed;	//X��
				g_aPlayer.move.z += cosf(pCamera->rot.y + D3DX_PI * 0.25f) * g_aPlayer.fSpeed;	//Z��

				//�������X�V
				if (g_aPlayer.rot.y <= pCamera->rot.y + D3DX_PI * 0.25f && g_aPlayer.rot.y >= pCamera->rot.y + D3DX_PI * -0.75f)
				{
					g_aPlayer.rot.y -= TURN_SPEED;	//����]

					if (g_aPlayer.rot.y <= pCamera->rot.y + D3DX_PI * -0.75f)
					{
						g_aPlayer.rot.y = pCamera->rot.y + D3DX_PI * -0.75f;//�E�����
					}
				}
				else
				{
					g_aPlayer.rot.y += TURN_SPEED;	//�E��]

					if (g_aPlayer.rot.y >= pCamera->rot.y + D3DX_PI * -0.75f)
					{
						g_aPlayer.rot.y = pCamera->rot.y + D3DX_PI * -0.75f;//�E�����
					}
				}
			}
			//�^��ړ�
			else
			{
				//�ړ��ʂ����Z
				g_aPlayer.move.x += sinf(pCamera->rot.y) * g_aPlayer.fSpeed;//X��
				g_aPlayer.move.z += cosf(pCamera->rot.y) * g_aPlayer.fSpeed;//Z��

				g_aPlayer.rot.y = pCamera->rot.y + D3DX_PI;	//�����
			}
		}

		//���ړ�
		else if (GetKeyboardPress(DIK_S))
		{
			//�����ړ�
			if (GetKeyboardPress(DIK_A))
			{
				//�ړ��ʂ����Z
				g_aPlayer.move.x += sinf(pCamera->rot.y + D3DX_PI * -0.75f) * g_aPlayer.fSpeed;	//X��
				g_aPlayer.move.z += cosf(pCamera->rot.y + D3DX_PI * -0.75f) * g_aPlayer.fSpeed;	//Z��

				//�������X�V
				if (g_aPlayer.rot.y >= pCamera->rot.y + D3DX_PI * -0.75f && g_aPlayer.rot.y <= pCamera->rot.y + D3DX_PI * 0.25f)
				{
					g_aPlayer.rot.y += TURN_SPEED;	//�E��]

					if (g_aPlayer.rot.y >= pCamera->rot.y + D3DX_PI * 0.25f)
					{
						g_aPlayer.rot.y = pCamera->rot.y + D3DX_PI * 0.25f;	//��������
					}
				}
				else
				{
					g_aPlayer.rot.y -= TURN_SPEED;	//����]

					if (g_aPlayer.rot.y <= pCamera->rot.y + D3DX_PI * 0.25f)
					{
						g_aPlayer.rot.y = pCamera->rot.y + D3DX_PI * 0.25f;	//��������
					}
				}
			}
			//�E���ړ�
			else if (GetKeyboardPress(DIK_D))
			{
				//�ړ��ʂ����Z
				g_aPlayer.move.x += sinf(pCamera->rot.y + D3DX_PI * 0.75f) * g_aPlayer.fSpeed;	//X��
				g_aPlayer.move.z += cosf(pCamera->rot.y + D3DX_PI * 0.75f) * g_aPlayer.fSpeed;	//Z��

				//�������X�V
				if (g_aPlayer.rot.y >= pCamera->rot.y + D3DX_PI * -0.25f && g_aPlayer.rot.y <= pCamera->rot.y + D3DX_PI * 0.75f)
				{
					g_aPlayer.rot.y -= TURN_SPEED;	//����]

					if (g_aPlayer.rot.y <= pCamera->rot.y + D3DX_PI * -0.25f)
					{
						g_aPlayer.rot.y = pCamera->rot.y + D3DX_PI * -0.25f;//�E������
					}
				}
				else
				{
					g_aPlayer.rot.y += TURN_SPEED;	//�E��]

					if (g_aPlayer.rot.y >= pCamera->rot.y + D3DX_PI * -0.25f)
					{
						g_aPlayer.rot.y = pCamera->rot.y + D3DX_PI * -0.25f;//�E������
					}
				}
			}
			//�^���ړ�
			else
			{
				//�ړ��ʂ����Z
				g_aPlayer.move.x += sinf(pCamera->rot.y + D3DX_PI) * g_aPlayer.fSpeed;	//X��
				g_aPlayer.move.z += cosf(pCamera->rot.y + D3DX_PI) * g_aPlayer.fSpeed;	//Z��

				g_aPlayer.rot.y = pCamera->rot.y;	//������
			}
		}

		//���ړ�
		else if (GetKeyboardPress(DIK_A))
		{
			//�ړ��ʂ����Z
			g_aPlayer.move.x += sinf(pCamera->rot.y + D3DX_PI * -0.5f) * g_aPlayer.fSpeed;	//X��
			g_aPlayer.move.z += cosf(pCamera->rot.y + D3DX_PI * -0.5f) * g_aPlayer.fSpeed;	//Z��

			//�������X�V
			if (g_aPlayer.rot.y >= pCamera->rot.y + D3DX_PI * -0.5f && g_aPlayer.rot.y <= pCamera->rot.y + D3DX_PI * 0.5f)
			{
				g_aPlayer.rot.y += TURN_SPEED;	//�E��]

				if (g_aPlayer.rot.y >= pCamera->rot.y + D3DX_PI * 0.5f)
				{
					g_aPlayer.rot.y = pCamera->rot.y + D3DX_PI * 0.5f;	//������
				}
			}
			else
			{
				g_aPlayer.rot.y -= TURN_SPEED;	//����]

				if (g_aPlayer.rot.y <= pCamera->rot.y + D3DX_PI * 0.5f)
				{
					g_aPlayer.rot.y = pCamera->rot.y + D3DX_PI * 0.5f;	//������
				}
			}
		}

		//�E�ړ�
		else if (GetKeyboardPress(DIK_D))
		{
			//�ړ��ʂ����Z
			g_aPlayer.move.x += sinf(pCamera->rot.y + D3DX_PI * 0.5f) * g_aPlayer.fSpeed;	//X��
			g_aPlayer.move.z += cosf(pCamera->rot.y + D3DX_PI * 0.5f) * g_aPlayer.fSpeed;	//Z��

			//�������X�V
			if (g_aPlayer.rot.y >= pCamera->rot.y + D3DX_PI * -0.5f && g_aPlayer.rot.y <= pCamera->rot.y + D3DX_PI * 0.5f)
			{
				g_aPlayer.rot.y -= TURN_SPEED;	//����]

				if (g_aPlayer.rot.y <= pCamera->rot.y + D3DX_PI * -0.5f)
				{
					g_aPlayer.rot.y = pCamera->rot.y + D3DX_PI * -0.5f;	//�E����
				}
			}
			else
			{
				g_aPlayer.rot.y += TURN_SPEED;	//�E��]

				if (g_aPlayer.rot.y >= pCamera->rot.y + D3DX_PI * -0.5f)
				{
					g_aPlayer.rot.y = pCamera->rot.y + D3DX_PI * -0.5f;	//�E����
				}
			}
		}

		//ENTER�L�[�������ꂽ�ꍇ
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			g_aPlayer.nState = 2;	//�U�����
		}

		//SPACE�L�[�������ꂽ�ꍇ
		if (GetKeyboardTrigger(DIK_SPACE) && g_aPlayer.pos.y == pField->pos.y)
		{
			g_aPlayer.nState = 3;	//�W�����v���
			g_aPlayer.move.y = g_aPlayer.fJump;	//�W�����v�㏸��
		}

		//�d�͂̉��Z
		g_aPlayer.move.y -= PLAYER_GRAVITY;

		//�ߋ��̈ʒu��ۑ�
		g_aPlayer.oldpos = g_aPlayer.pos;

		//�ړ��ʂ����Z
		g_aPlayer.pos += g_aPlayer.move;

		//�ړ��ʂ�����
		g_aPlayer.move.x += (0.0f - g_aPlayer.move.x) * SPEED_DERAY;
		g_aPlayer.move.z += (0.0f - g_aPlayer.move.z) * SPEED_DERAY;

		//===============================================================================================================
		// ���[�V��������
		//===============================================================================================================
		UpBodyMotion();		//�㔼�g
		DownBodyMotion();	//�����g
	}

	//===============================================================================================================
	// �����蔻��
	//===============================================================================================================
	
	//===========================================================================================
	// �n��
	//===========================================================================================
	if (g_aPlayer.pos.y < pField->pos.y)
	{
		g_aPlayer.pos.y = pField->pos.y;//�n�ʂ𒴂��Ȃ��悤�ɒ���

		//�A�N�V����or�W�����v��Ԃł͂Ȃ��ꍇ
		if (g_aPlayer.nState == 0 || g_aPlayer.nState == 1)
		{
			//�����Ă��Ȃ��ꍇ
			if ((g_aPlayer.move.x <= 0.01f && g_aPlayer.move.x >= -0.01f) && (g_aPlayer.move.z <= 0.01f && g_aPlayer.move.z >= -0.01f))
			{
				g_aPlayer.nState = 0;	//�j���[�g�������
			}
			//�����Ă���ꍇ
			else
			{
				g_aPlayer.nState = 1;	//�ړ����
			}
		}
	}
	else
	{
		g_aPlayer.nState = 3;	//�W�����v���
	}

	//===========================================================================================
	// ��
	//===========================================================================================
	for (int nCntWall = 0; nCntWall < MAX_WALL; nCntWall++, pWall++)
	{
		//1����(��)
		if (g_aPlayer.pos.z > pWall->pos.z - COLLISION_WALL && pWall->nCnt == 0)
		{
			g_aPlayer.pos.z = pWall->pos.z - COLLISION_WALL;
		}
		//2����(�E)
		if (g_aPlayer.pos.x > pWall->pos.x - COLLISION_WALL && pWall->nCnt == 1)
		{
			g_aPlayer.pos.x = pWall->pos.x - COLLISION_WALL;
		}
		//3����(��)
		if (g_aPlayer.pos.z < pWall->pos.z + COLLISION_WALL && pWall->nCnt == 2)
		{
			g_aPlayer.pos.z = pWall->pos.z + COLLISION_WALL;
		}
		//4����(��)
		if (g_aPlayer.pos.x < pWall->pos.x + COLLISION_WALL && pWall->nCnt == 3)
		{
			g_aPlayer.pos.x = pWall->pos.x + COLLISION_WALL;
		}
	}

	//===============================================================================================================
	// �ړ����x����
	//===============================================================================================================
	if (GetKeyboardRepeat(DIK_Y))
	{//Y�L�[�������ꂽ�ꍇ
		g_aPlayer.fSpeed += ADD_SPEED;
	}
	else if (GetKeyboardRepeat(DIK_H))
	{//H�L�[�������ꂽ�ꍇ
		g_aPlayer.fSpeed -= ADD_SPEED;
	}

	//===============================================================================================================
	// �W�����v�ʒ���
	//===============================================================================================================
	if (GetKeyboardRepeat(DIK_U))
	{//U�L�[�������ꂽ�ꍇ
		g_aPlayer.fJump += ADD_JUMP;
	}
	else if (GetKeyboardRepeat(DIK_J))
	{//J�L�[�������ꂽ�ꍇ
		g_aPlayer.fJump -= ADD_JUMP;
	}

	//===============================================================================================================
	// �ŏ�����Đ�
	//===============================================================================================================
	if (GetKeyboardTrigger(DIK_F4))
	{//F4�L�[�������ꂽ�ꍇ
		
		//���݂̃L�[�ƃt���[���������Z�b�g
		g_aPlayer.aMotion[g_aPlayer.nState].nCntKeySet = 0;
		g_aPlayer.aMotion[g_aPlayer.nState].nCntFrame = 0;
		g_nCntFrame[g_aPlayer.nState] = 0;
	}

	//===============================================================================================================
	// �X�V�����L�����ؑ�
	//===============================================================================================================
	if (GetKeyboardTrigger(DIK_P))
	{//P�L�[�������ꂽ�ꍇ
		g_bUpdate = g_bUpdate ? false : true;
	}

	//===============================================================================================================
	// �f�o�b�O�\��
	//===============================================================================================================
	PrintDebugProc("[ ���[�h�؂�ւ� ] : [ F2 ] �y �v���C���[�h �z\n\n");

	PrintDebugProc("�y MOTION : %d / %d �z\n", g_nMotion, g_nMaxMotion);
	PrintDebugProc("[ ���[�V������ޕύX ] : [ 3,4 ]\n\n");

	PrintDebugProc("�y KEY : %d / %d �z\n", g_nKeySet[g_nMotion], g_aPlayer.aMotion[g_nMotion].nMaxKeySet);
	PrintDebugProc("[ �L�[�ύX ] : [ 7,8 ]\n\n");

	PrintDebugProc("[ �t���[���� : %d ] : [ I,K ]\n", g_aPlayer.aMotion[g_nMotion].aKeyset[g_nKeySet[g_nMotion]].nMaxFrame);
	PrintDebugProc("[ ���[�v : %d ] : [ L ] ( 0 : OFF , 1 : ON )\n\n", g_aPlayer.aMotion[g_nMotion].nLoop);

	PrintDebugProc("[ �X�s�[�h : %f ] : [ Y,H ]\n", g_aPlayer.fSpeed);
	PrintDebugProc("[ �W�����v�� : %f ] : [ U,J ]\n\n", g_aPlayer.fJump);

	PrintDebugProc("[ �ŏ�����Đ� ] : [ F4 ]\n");
	PrintDebugProc("[ �ꎞ��~ ] : [ P ]\n\n");

	PrintDebugProc("[ ���݂̃��[�V���� ] : %d\n", g_aPlayer.nState);
	PrintDebugProc("[ ���݂̃t���[���� ] : [ %d / %d ]\n", g_nCntFrame[g_aPlayer.nState], g_nTotalFrame[g_aPlayer.nState]);
	PrintDebugProc("[ ���݂̃L�[�Z�b�g ] : [ %d / %d ]\n\n", g_aPlayer.aMotion[g_aPlayer.nState].nCntKeySet, g_aPlayer.aMotion[g_aPlayer.nState].nMaxKeySet);
}

//============================================================================================================================================================================================================
// �㔼�g�̃��[�V��������
//============================================================================================================================================================================================================
void UpBodyMotion(void)
{
	int nState = g_aPlayer.nState;//�v���C���[�̏��
	int nKeySet = g_aPlayer.aMotion[nState].nCntKeySet;//���݂̃L�[��ۑ�
	int nNextKeySet = (nKeySet + 1) % (g_aPlayer.aMotion[nState].nMaxKeySet + 1);//���̃L�[�����߂�
	float fFrame = (float)g_aPlayer.aMotion[nState].nCntFrame / g_aPlayer.aMotion[nState].aKeyset[nKeySet].nMaxFrame;//���݂̃t���[���ƑS�̃t���[���̊���

	//�p�[�c���Ƃɏ����X�V
	for (int nCntParts = 0; nCntParts < g_aPlayer.nParts; nCntParts++)
	{
		//�㔼�g�̏ꍇ
		if (g_aPlayer.aParts[nCntParts].nHalf == 0)
		{
			//==========================================================================================================================================================
			// ���̃L�[�܂ł̍��������߂�
			//==========================================================================================================================================================

			//�p�x
			float fRotx = g_aPlayer.aMotion[nState].aKeyset[nNextKeySet].aKey[nCntParts].rot.x - g_aPlayer.aMotion[nState].aKeyset[nKeySet].aKey[nCntParts].rot.x;//X��
			float fRoty = g_aPlayer.aMotion[nState].aKeyset[nNextKeySet].aKey[nCntParts].rot.y - g_aPlayer.aMotion[nState].aKeyset[nKeySet].aKey[nCntParts].rot.y;//Y��
			float fRotz = g_aPlayer.aMotion[nState].aKeyset[nNextKeySet].aKey[nCntParts].rot.z - g_aPlayer.aMotion[nState].aKeyset[nKeySet].aKey[nCntParts].rot.z;//Z��

			//�p�x�␳
			CorrectionRot(fRotx);
			CorrectionRot(fRoty);
			CorrectionRot(fRotz);

			//�ʒu
			float fPosx = g_aPlayer.aMotion[nState].aKeyset[nNextKeySet].aKey[nCntParts].pos.x - g_aPlayer.aMotion[nState].aKeyset[nKeySet].aKey[nCntParts].pos.x;//X���W
			float fPosy = g_aPlayer.aMotion[nState].aKeyset[nNextKeySet].aKey[nCntParts].pos.y - g_aPlayer.aMotion[nState].aKeyset[nKeySet].aKey[nCntParts].pos.y;//Y���W
			float fPosz = g_aPlayer.aMotion[nState].aKeyset[nNextKeySet].aKey[nCntParts].pos.z - g_aPlayer.aMotion[nState].aKeyset[nKeySet].aKey[nCntParts].pos.z;//Z���W

			//==========================================================================================================================================================
			// ���̃L�[�𔽉f
			//==========================================================================================================================================================

			//�������X�V
			g_aPlayer.aParts[nCntParts].rot.x = g_aPlayer.aMotion[nState].aKeyset[nKeySet].aKey[nCntParts].rot.x + fRotx * fFrame;	//X��
			g_aPlayer.aParts[nCntParts].rot.y = g_aPlayer.aMotion[nState].aKeyset[nKeySet].aKey[nCntParts].rot.y + fRoty * fFrame;	//Y��
			g_aPlayer.aParts[nCntParts].rot.z = g_aPlayer.aMotion[nState].aKeyset[nKeySet].aKey[nCntParts].rot.z + fRotz * fFrame;	//Z��

			//�p�x�␳
			CorrectionRot(g_aPlayer.aParts[nCntParts].rot.x);
			CorrectionRot(g_aPlayer.aParts[nCntParts].rot.y);
			CorrectionRot(g_aPlayer.aParts[nCntParts].rot.z);

			//�ʒu���X�V
			g_aPlayer.aParts[nCntParts].pos.x = g_aPlayer.aParts[nCntParts].pos.x + g_aPlayer.aMotion[nState].aKeyset[nKeySet].aKey[nCntParts].pos.x + fPosx * fFrame;	//X���W
			g_aPlayer.aParts[nCntParts].pos.y = g_aPlayer.aParts[nCntParts].pos.y + g_aPlayer.aMotion[nState].aKeyset[nKeySet].aKey[nCntParts].pos.y + fPosy * fFrame;	//Y���W
			g_aPlayer.aParts[nCntParts].pos.z = g_aPlayer.aParts[nCntParts].pos.z + g_aPlayer.aMotion[nState].aKeyset[nKeySet].aKey[nCntParts].pos.z + fPosz * fFrame;	//Z���W
		}
	}

	//�J�E���g���w��̃t���[�����ɂȂ����ꍇ
	if (g_aPlayer.aMotion[nState].nCntFrame >= g_aPlayer.aMotion[nState].aKeyset[nKeySet].nMaxFrame)
	{
		//�t���[�����J�E���g��������
		g_aPlayer.aMotion[nState].nCntFrame = 0;

		//�L�[�������Z
		g_aPlayer.aMotion[nState].nCntKeySet++;

		//�L�[�����ő吔�ɂȂ���
		if (g_aPlayer.aMotion[nState].nCntKeySet > g_aPlayer.aMotion[nState].nMaxKeySet)
		{
			//���[�v���Ȃ��ꍇ
			if (g_aPlayer.aMotion[nState].nLoop == 0)
			{
				//�j���[�g�������
				g_aPlayer.nState = 0;
			}

			//�L�[����������
			g_aPlayer.aMotion[nState].nCntKeySet = 0;
		}
	}

	//���v�t���[�������Z�b�g
	if (g_nCntFrame[nState] >= g_nTotalFrame[nState])
	{
		g_nCntFrame[nState] = 0;
	}

	//�t���[�����X�V
	g_aPlayer.aMotion[nState].nCntFrame++;

	//���v�t���[�����X�V
	g_nCntFrame[nState]++;
}

//============================================================================================================================================================================================================
// �����g�̃��[�V��������
//============================================================================================================================================================================================================
void DownBodyMotion(void)
{
	int nState = g_aPlayer.nState;	//�v���C���[�̏��

	//�����Ă���ꍇ
	if((g_aPlayer.move.x > JUDGE_MOVE && g_aPlayer.move.x < -JUDGE_MOVE) && (g_aPlayer.move.z > JUDGE_MOVE && g_aPlayer.move.z < -JUDGE_MOVE))
	{
		nState = 1;	//�ړ����
	}

	//���݂̃L�[��ۑ�
	int nKey = g_aPlayer.aMotion2[nState].nCntKeySet;

	//���̃L�[�����߂�
	int nNextKey = (nKey + 1) % (g_aPlayer.aMotion2[nState].nMaxKeySet + 1);

	//���݂̃t���[���ƑS�̃t���[���̊���
	float fFrame = (float)g_aPlayer.aMotion2[nState].nCntFrame / g_aPlayer.aMotion2[nState].aKeyset[nKey].nMaxFrame;

	//�p�[�c���Ƃɏ����X�V
	for (int nCntParts = 0; nCntParts < g_aPlayer.nParts; nCntParts++)
	{
		//�����g�̏ꍇ
		if (g_aPlayer.aParts[nCntParts].nHalf == 1)
		{
			//==========================================================================================================================================================
			// ���̃L�[�܂ł̍��������߂�
			//==========================================================================================================================================================

			//����
			float fRotx = g_aPlayer.aMotion2[nState].aKeyset[nNextKey].aKey[nCntParts].rot.x - g_aPlayer.aMotion2[nState].aKeyset[nKey].aKey[nCntParts].rot.x;	//X��
			float fRoty = g_aPlayer.aMotion2[nState].aKeyset[nNextKey].aKey[nCntParts].rot.y - g_aPlayer.aMotion2[nState].aKeyset[nKey].aKey[nCntParts].rot.y;	//Y��
			float fRotz = g_aPlayer.aMotion2[nState].aKeyset[nNextKey].aKey[nCntParts].rot.z - g_aPlayer.aMotion2[nState].aKeyset[nKey].aKey[nCntParts].rot.z;	//Z��

			//�p�x�␳
			CorrectionRot(fRotx);
			CorrectionRot(fRoty);
			CorrectionRot(fRotz);

			//�ʒu
			float fPosx = g_aPlayer.aMotion2[nState].aKeyset[nNextKey].aKey[nCntParts].pos.x - g_aPlayer.aMotion2[nState].aKeyset[nKey].aKey[nCntParts].pos.x;	//X���W
			float fPosy = g_aPlayer.aMotion2[nState].aKeyset[nNextKey].aKey[nCntParts].pos.y - g_aPlayer.aMotion2[nState].aKeyset[nKey].aKey[nCntParts].pos.y;	//Y���W
			float fPosz = g_aPlayer.aMotion2[nState].aKeyset[nNextKey].aKey[nCntParts].pos.z - g_aPlayer.aMotion2[nState].aKeyset[nKey].aKey[nCntParts].pos.z;	//Z���W

			//==========================================================================================================================================================
			// ���̃L�[�𔽉f
			//==========================================================================================================================================================
			 
			//�������X�V
			g_aPlayer.aParts[nCntParts].rot.x = g_aPlayer.aMotion2[nState].aKeyset[nKey].aKey[nCntParts].rot.x + fRotx * fFrame;//X��
			g_aPlayer.aParts[nCntParts].rot.y = g_aPlayer.aMotion2[nState].aKeyset[nKey].aKey[nCntParts].rot.y + fRoty * fFrame;//Y��
			g_aPlayer.aParts[nCntParts].rot.z = g_aPlayer.aMotion2[nState].aKeyset[nKey].aKey[nCntParts].rot.z + fRotz * fFrame;//Z��

			//�p�x�␳
			CorrectionRot(g_aPlayer.aParts[nCntParts].rot.x);
			CorrectionRot(g_aPlayer.aParts[nCntParts].rot.y);
			CorrectionRot(g_aPlayer.aParts[nCntParts].rot.z);

			//�ʒu���X�V
			g_aPlayer.aParts[nCntParts].pos.x = g_aPlayer.aParts[nCntParts].pos.x + g_aPlayer.aMotion2[nState].aKeyset[nKey].aKey[nCntParts].pos.x + fPosx * fFrame;//X��
			g_aPlayer.aParts[nCntParts].pos.y = g_aPlayer.aParts[nCntParts].pos.y + g_aPlayer.aMotion2[nState].aKeyset[nKey].aKey[nCntParts].pos.y + fPosy * fFrame;//Y��
			g_aPlayer.aParts[nCntParts].pos.z = g_aPlayer.aParts[nCntParts].pos.z + g_aPlayer.aMotion2[nState].aKeyset[nKey].aKey[nCntParts].pos.z + fPosz * fFrame;//Z��
		}
	}

	//�J�E���g���w��̃t���[�����ɂȂ����ꍇ
	if (g_aPlayer.aMotion2[nState].nCntFrame >= g_aPlayer.aMotion2[nState].aKeyset[nKey].nMaxFrame)
	{
		//�t���[�����J�E���g��������
		g_aPlayer.aMotion2[nState].nCntFrame = 0;

		//�L�[�������Z
		g_aPlayer.aMotion2[nState].nCntKeySet++;

		//�L�[�����ő吔�ɂȂ���
		if (g_aPlayer.aMotion2[nState].nCntKeySet > g_aPlayer.aMotion2[nState].nMaxKeySet)
		{
			//���[�v���Ȃ��ꍇ
			if (g_aPlayer.aMotion2[nState].nLoop == 0)
			{
				//�j���[�g�������
				g_aPlayer.nState = 0;
			}
			
			//�L�[����������
			g_aPlayer.aMotion2[nState].nCntKeySet = 0;
		}
	}

	//�t���[�����X�V
	g_aPlayer.aMotion2[nState].nCntFrame++;
}

//============================================================================================================================================================================================================
// �`�揈��
//============================================================================================================================================================================================================
void DrawPlayer(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	//�f�o�C�X�̎擾
	D3DXMATRIX mtxRot, mtxTrans, mtxParent;		//�v�Z�p�}�g���b�N�X
	D3DMATERIAL9 matDef;						//���݂̃}�e���A���ۑ��p
	D3DXMATERIAL* pMat;							//�}�e���A���̃f�[�^�ւ̃|�C���^

	//�v���C���[�̃��[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&g_aPlayer.mtxWorld);

	//�v���C���[�̌����𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aPlayer.rot.y, g_aPlayer.rot.x, g_aPlayer.rot.z);
	D3DXMatrixMultiply(&g_aPlayer.mtxWorld, &g_aPlayer.mtxWorld, &mtxRot);

	//�v���C���[�̈ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, g_aPlayer.pos.x, g_aPlayer.pos.y, g_aPlayer.pos.z);
	D3DXMatrixMultiply(&g_aPlayer.mtxWorld, &g_aPlayer.mtxWorld, &mtxTrans);

	for (int nCntParts = 0; nCntParts < g_aPlayer.nParts; nCntParts++)
	{
		//�e�p�[�c���f���̃��[���h�}�g���b�N�X�̏�����
		D3DXMatrixIdentity(&g_aPlayer.aParts[nCntParts].mtxWorld);

		//�e�p�[�c���f���̌����𔽉f
		D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aPlayer.aParts[nCntParts].rot.y, g_aPlayer.aParts[nCntParts].rot.x, g_aPlayer.aParts[nCntParts].rot.z);
		D3DXMatrixMultiply(&g_aPlayer.aParts[nCntParts].mtxWorld, &g_aPlayer.aParts[nCntParts].mtxWorld, &mtxRot);

		//�e�p�[�c���f���̈ʒu�𔽉f
		D3DXMatrixTranslation(&mtxTrans, g_aPlayer.aParts[nCntParts].pos.x, g_aPlayer.aParts[nCntParts].pos.y, g_aPlayer.aParts[nCntParts].pos.z);
		D3DXMatrixMultiply(&g_aPlayer.aParts[nCntParts].mtxWorld, &g_aPlayer.aParts[nCntParts].mtxWorld, &mtxTrans);

		//�e�̔ԍ���ۑ�
		int nParent = g_aPlayer.aParts[nCntParts].nParent;

		//�e�̃}�g���b�N�X�𔽉f
		if (nParent != -1)
		{//�e�̔ԍ������ꍇ
			mtxParent = g_aPlayer.aParts[nParent].mtxWorld;	//�e���f���̃}�g���b�N�X
		}
		else
		{//�e�̔ԍ��������Ȃ��ꍇ
			mtxParent = g_aPlayer.mtxWorld;	//�v���C���[�̃}�g���b�N�X
		}

		//�e�̃}�g���b�N�X���|�����킹��
		D3DXMatrixMultiply(&g_aPlayer.aParts[nCntParts].mtxWorld, &g_aPlayer.aParts[nCntParts].mtxWorld, &mtxParent);

		//���[���h�}�g���b�N�X�̐ݒ�
		pDevice->SetTransform(D3DTS_WORLD, &g_aPlayer.aParts[nCntParts].mtxWorld);

		//���݂̃}�e���A�����擾
		pDevice->GetMaterial(&matDef);

		//�}�e���A���f�[�^�ւ̃|�C���^���擾
		pMat = (D3DXMATERIAL*)g_aPlayer.aParts[nCntParts].pBuffMat->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)g_aPlayer.aParts[nCntParts].dwNumMat; nCntMat++)
		{
			if (!g_bEditMotion)
			{//�v���C���[�h��
				g_aPlayer.aParts[nCntParts].Diffuse[nCntMat] = g_aPlayer.aParts[nCntParts].FirstDiffuse[nCntMat];
			}
			else
			{//�G�f�B�^��
				if (nCntParts == g_nParts)
				{//���ݑI�����Ă���p�[�c�𔼓����ɂ���
					g_aPlayer.aParts[nCntParts].Diffuse[nCntMat].a = 0.3f;
				}
				else
				{//�����l�̃}�e���A�����ɂ���
					g_aPlayer.aParts[nCntParts].Diffuse[nCntMat] = g_aPlayer.aParts[nCntParts].FirstDiffuse[nCntMat];
				}
			}

			//�}�e���A�����𔽉f
			pMat[nCntMat].MatD3D.Diffuse = g_aPlayer.aParts[nCntParts].Diffuse[nCntMat];

			//�}�e���A���̐ݒ�
			pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

			//�e�N�X�`���̐ݒ�
			pDevice->SetTexture(0, NULL);

			//���f��(�p�[�c)�̕`��
			g_aPlayer.aParts[nCntParts].pMesh->DrawSubset(nCntMat);
		}

		//�ۑ����Ă����}�e���A����߂�
		pDevice->SetMaterial(&matDef);
	}
}

//============================================================================================================================================================================================================
// �v���C���[�̏����擾
//============================================================================================================================================================================================================
Player* GetPlayer(void)
{
	return &g_aPlayer;
}
