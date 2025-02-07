//==============================================================================================================================================
//
// �G�l�~�[�Ǘ�
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "enemymanager.h"

//===========================================================================================================
// �ÓI�����o�ϐ�������
//===========================================================================================================
int CEnemyManager::m_nFirstAll = 0;//�������̑���
int CEnemyManager::m_nNowAll = 0;//����

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CEnemyManager::CEnemyManager() : CObjectManager(),
	m_pTop(nullptr),
	m_pCur(nullptr)
{
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CEnemyManager::~CEnemyManager()
{
}

//===========================================================================================================
// ����������
//===========================================================================================================
HRESULT CEnemyManager::Init()
{
	//���N���X����������
	if (FAILED(CObjectManager::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===========================================================================================================
// �I������
//===========================================================================================================
void CEnemyManager::Uninit()
{
	//���N���X�I������
	CObjectManager::Uninit();
}

//===========================================================================================================
// �X�V����
//===========================================================================================================
void CEnemyManager::Update()
{
	//�擪�A�h���X���擾����
	CEnemy* pEnemy = m_pTop;

	//�I�u�W�F�N�g���nullptr�ɂȂ�܂ŌJ��Ԃ�
	while (pEnemy != nullptr)
	{
		//���̃I�u�W�F�N�g�����擾
		CEnemy* pNext = pEnemy->GetNextEnemy();

		//�G�l�~�[��񂪑��݂���ꍇ
		//�폜�t���O��true�̏ꍇ
		if (pEnemy != nullptr && pEnemy->GetDeleteFlag())
		{
			//�O�̃I�u�W�F�N�g�����擾
			CEnemy* pPrev = pEnemy->GetPrevEnemy();

			//�O�̃I�u�W�F�N�g��񂪑��݂���ꍇ
			if (pPrev != nullptr)
			{
				//���̃I�u�W�F�N�g�����q��
				pPrev->SetNextEnemy(pNext);
			}
			//�O�̃I�u�W�F�N�g��񂪑��݂��Ȃ��ꍇ
			else
			{
				//���̏�񂪑��݂���ꍇ
				if (pNext != nullptr)
				{
					//���̂̃I�u�W�F�N�g���̑O�̃I�u�W�F�N�g����nullptr�ɐݒ�
					pNext->SetPrevEnemy(nullptr);
				}

				//�擪�A�h���X��ύX
				m_pTop = pNext;
			}

			//���̃I�u�W�F�N�g��񂪑��݂���ꍇ
			if (pNext != nullptr)
			{
				//�O�̃I�u�W�F�N�g�����q��
				pNext->SetPrevEnemy(pPrev);
			}
			//���̃I�u�W�F�N�g��񂪑��݂��Ȃ��ꍇ
			else
			{
				//�O�̏�񂪑��݂���ꍇ
				if (pPrev != nullptr)
				{
					//�O�̃I�u�W�F�N�g���̎��̃I�u�W�F�N�g����nullptr�ɐݒ�
					pPrev->SetNextEnemy(nullptr);
				}

				//�I�[�A�h���X��ύX
				m_pCur = pPrev;
			}

			//���������Z
			m_nNowAll++;
		}

		//�I�u�W�F�N�g�������̏��ɕύX����
		pEnemy = pNext;
	}
}

//===========================================================================================================
// �o�^����
//===========================================================================================================
void CEnemyManager::Regist(CEnemy* pEnemy)
{
	//�擪�A�h���X�����݂��Ȃ��ꍇ
	if (m_pTop == nullptr)
	{
		//���������G�l�~�[����擪�A�h���X�ɑ��
		m_pTop = pEnemy;

		//�O�̃I�u�W�F�N�g����nullptr����
		pEnemy->SetPrevEnemy(nullptr);
	}

	//�擪�A�h���X�����݂���ꍇ
	else
	{
		//�擪�A�h���X���擾
		CEnemy* pAllEnemy = m_pTop;

		//�I�u�W�F�N�g���nullptr�ɂȂ�܂ŌJ��Ԃ�
		while (pAllEnemy != nullptr)
		{
			//���̃I�u�W�F�N�g��񂪑��݂��Ȃ��ꍇ
			if (pAllEnemy->GetNextEnemy() == nullptr)
			{
				//���̃I�u�W�F�N�g���ɐ��������G�l�~�[����ۑ�
				pAllEnemy->SetNextEnemy(pEnemy);

				//���������G�l�~�[���̑O�̃I�u�W�F�N�g����ۑ�
				pEnemy->SetPrevEnemy(pAllEnemy);
				break;
			}

			//�I�u�W�F�N�g�������̏��ɕύX����
			pAllEnemy = pAllEnemy->GetNextEnemy();
		}
	}

	//���g�̎��̃I�u�W�F�N�g����nullptr����
	pEnemy->SetNextEnemy(nullptr);

	//�I�[�A�h���X����
	m_pCur = pEnemy;

	//���������Z
	m_nNowAll++;

	//�������̑������X�V
	m_nFirstAll = m_nNowAll;
}

//===========================================================================================================
// �Ǎ�����
//===========================================================================================================
void CEnemyManager::Load()
{
	//���[�J���ϐ��錾
	D3DXVECTOR3 pos = { 0.0f, 0.0f, 0.0f };//���W���ۑ��p
	D3DXVECTOR3 rot = { 0.0f, 0.0f, 0.0f };//�p�x���ۑ��p
	D3DXVECTOR3 scale = { 0.0f, 0.0f, 0.0f };//�g�嗦���ۑ��p
	int nTag = 1;//�^�O���ۑ��p
	int nType = 0;//�^�C�v���ۑ��p
	int nNumAll = 0;//����

	//�t�@�C�����J��
	FILE* pFile = fopen(CManager::GetInstance()->StageFile[static_cast<int>(CManager::GetInstance()->GetScene()->GetMode())], "rb");

	//�I�u�W�F�N�g�f�[�^�̓ǂݍ���
	if (pFile != nullptr)
	{
		//�t�@�C���̓ǂݍ���
		fread(&nNumAll, sizeof(int), 1, pFile);

		for (int nCnt = 0; nCnt < nNumAll; nCnt++)
		{
			//�f�[�^�̎擾
			fread(&pos, sizeof(D3DXVECTOR3), 1, pFile);//���W
			fread(&rot, sizeof(D3DXVECTOR3), 1, pFile);//�p�x
			fread(&scale, sizeof(D3DXVECTOR3), 1, pFile);//�g�嗦
			fread(&nTag, sizeof(int), 1, pFile);//�^�O
			fread(&nType, sizeof(int), 1, pFile);//���

			//�^�O���G�l�~�[�̏ꍇ
			if (static_cast<CTag::TAG>(nTag) == CTag::TAG::ENEMY)
			{
				//����
				CEnemy::Create(pos, rot, scale, static_cast<CEnemy::TYPE>(nType));
			}
		}

		//�t�@�C�������
		fclose(pFile);
	}
}

//===========================================================================================================
// �����蔻��
//===========================================================================================================
bool CEnemyManager::Collision(CEnemy::CollisionParam& aParam)
{
	//�擪�̏����擾
	CEnemy* pEnemy = m_pTop;

	//�I�u�W�F�N�g���nullptr�ɂȂ�܂ŌJ��Ԃ�
	while (pEnemy != nullptr)
	{
		//���̃I�u�W�F�N�g�����擾
		CEnemy* pNext = pEnemy->GetNextEnemy();

		//�����蔻�肪true�̏ꍇ
		if (pEnemy != nullptr && pEnemy->Collision(aParam))
		{
			return true;
		}

		//�I�u�W�F�N�g�������̏��ɕύX����
		pEnemy = pNext;
	}

	return false;
}

//===========================================================================================================
// ��������
//===========================================================================================================
CEnemy* CEnemyManager::FindEnemy(CEnemy::TYPE type)
{
	//�擪�̏����擾
	CEnemy* pEnemy = m_pTop;

	//���̃G�l�~�[���nullptr�ɂȂ�܂ŌJ��Ԃ�
	while (pEnemy != nullptr)
	{
		//���̃I�u�W�F�N�g�����擾
		CEnemy* pNext = pEnemy->GetNextEnemy();

		//�^�C�v����v�����ꍇ
		if (pEnemy->GetEnemyType() == type)
		{
			//�|�C���^��Ԃ�
			return pEnemy;
		}

		//���̃G�l�~�[���̃|�C���^���R�s�[
		pEnemy = pNext;
	}

	return nullptr;
}
