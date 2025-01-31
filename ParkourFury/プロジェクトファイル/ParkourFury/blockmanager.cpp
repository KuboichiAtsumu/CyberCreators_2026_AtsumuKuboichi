//==============================================================================================================================================
//
// �u���b�N�Ǘ�
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "blockmanager.h"

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CBlockManager::CBlockManager() : CObjectManager()
{
	//�����o�ϐ�������
	m_pTop = nullptr;//�擪�A�h���X
	m_pCur = nullptr;//�I�[�A�h���X
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CBlockManager::~CBlockManager()
{
}

//===========================================================================================================
// ����������
//===========================================================================================================
HRESULT CBlockManager::Init()
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
void CBlockManager::Uninit()
{
	//���N���X�I������
	CObjectManager::Uninit();
}

//===========================================================================================================
// �w�肳�ꂽ�^�C�v�݂̂̏I������
//===========================================================================================================
void CBlockManager::UninitTypeAll(CBlock::TYPE type)
{
	//�擪�̃|�C���^���R�s�[
	CBlock* pBlock = m_pTop;

	//���̃u���b�N���nullptr�ɂȂ�܂ŌJ��Ԃ�
	while (pBlock != nullptr)
	{
		//���̃I�u�W�F�N�g�����擾
		CBlock* pNext = pBlock->GetNextBlock();

		//�^�C�v����v�����ꍇ
		if (pBlock->GetBlockType() == type)
		{
			//�I������
			pBlock->Uninit();
			pBlock = nullptr;
		}

		//���̃u���b�N���̃|�C���^���R�s�[
		pBlock = pNext;
	}
}

//===========================================================================================================
// �X�V����
//===========================================================================================================
void CBlockManager::Update()
{
	//�擪�A�h���X���擾����
	CBlock* pBlock = m_pTop;

	//�I�u�W�F�N�g���nullptr�ɂȂ�܂ŌJ��Ԃ�
	while (pBlock != nullptr)
	{
		//���̃I�u�W�F�N�g�����擾
		CBlock* pNext = pBlock->GetNextBlock();

		//�u���b�N��񂪑��݂���ꍇ
		//�폜�t���O��true�̏ꍇ
		if (pBlock != nullptr && pBlock->GetDeleteFlag())
		{
			//�O�̃I�u�W�F�N�g�����擾
			CBlock* pPrev = pBlock->GetPrevBlock();

			//�O�̃I�u�W�F�N�g��񂪑��݂���ꍇ
			if (pPrev != nullptr)
			{
				//���̃I�u�W�F�N�g�����q��
				pPrev->SetNextBlock(pNext);
			}
			//�O�̃I�u�W�F�N�g��񂪑��݂��Ȃ��ꍇ
			else
			{
				//���̃I�u�W�F�N�g��񂪑��݂���ꍇ
				if (pNext != nullptr)
				{
					//���̃I�u�W�F�N�g���̑O�̃I�u�W�F�N�g����nullptr�ɐݒ�
					pNext->SetPrevBlock(nullptr);
				}

				//�擪�A�h���X��ύX
				m_pTop = pNext;
			}

			//���̃I�u�W�F�N�g��񂪑��݂���ꍇ
			if (pNext != nullptr)
			{
				//�O�̃I�u�W�F�N�g�����q��
				pNext->SetPrevBlock(pPrev);
			}
			//���̃I�u�W�F�N�g��񂪑��݂��Ȃ��ꍇ
			else
			{
				//�O�̃I�u�W�F�N�g�����݂���ꍇ
				if (pPrev != nullptr)
				{
					//�O�̃I�u�W�F�N�g���̎��̃I�u�W�F�N�g����nullptr�ɐݒ�
					pPrev->SetNextBlock(nullptr);
				}

				//�I�[�A�h���X��ύX
				m_pCur = pPrev;
			}
		}

		//�I�u�W�F�N�g�������̏��ɕύX����
		pBlock = pNext;
	}
}

//===========================================================================================================
// �`�揈��
//===========================================================================================================
void CBlockManager::Draw()
{
}

//===========================================================================================================
// �o�^����
//===========================================================================================================
void CBlockManager::Regist(CBlock* pBlock)
{
	//�擪�A�h���X�����݂��Ȃ��ꍇ
	if (m_pTop == nullptr)
	{
		//���������u���b�N����擪�A�h���X�ɑ��
		m_pTop = pBlock;

		//�O�̃I�u�W�F�N�g����nullptr����
		pBlock->SetPrevBlock(nullptr);
	}

	//�擪�A�h���X�����݂���ꍇ
	else
	{
		//�擪�A�h���X���擾
		CBlock* pAllBlock = m_pTop;

		//�I�u�W�F�N�g���nullptr�ɂȂ�܂ŌJ��Ԃ�
		while (pAllBlock != nullptr)
		{
			//���̃I�u�W�F�N�g��񂪑��݂��Ȃ��ꍇ
			if (pAllBlock->GetNextBlock() == nullptr)
			{
				//���̃I�u�W�F�N�g���ɐ��������u���b�N����ۑ�
				pAllBlock->SetNextBlock(pBlock);

				//���������u���b�N���̑O�̃I�u�W�F�N�g����ۑ�
				pBlock->SetPrevBlock(pAllBlock);
				break;
			}

			//�I�u�W�F�N�g�������̏��ɕύX����
			pAllBlock = pAllBlock->GetNextBlock();
		}
	}

	//���g�̎��̃I�u�W�F�N�g����nullptr����
	pBlock->SetNextBlock(nullptr);

	//�I�[�A�h���X����
	m_pCur = pBlock;
}

//===========================================================================================================
// �Ǎ�����
//===========================================================================================================
void CBlockManager::Load()
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

			//�^�O���u���b�N�ł͂Ȃ��ꍇ
			if (static_cast<CTag::TAG>(nTag) != CTag::TAG::BLOCK)
			{
				//���������ɓǍ��𑱂���
				continue;
			}

			//�u���b�N�𐶐�
			CBlock::Create(pos, rot, scale, static_cast<CBlock::TYPE>(nType));
		}

		//�t�@�C�������
		fclose(pFile);
	}
}

//===========================================================================================================
// �����蔻��
//===========================================================================================================
CBlock::CollisionFlag CBlockManager::Collision(CBlock::CollisionParam& aParam)
{
	//�擪�A�h���X���擾����
	CBlock::CollisionFlag aFlag = {};//�e���W�̔���
	CBlock::CollisionFlag aSaveFlag = {};//����擾�p
	CBlock* pBlock = m_pTop;

	//�I�u�W�F�N�g���nullptr�ɂȂ�܂ŌJ��Ԃ�
	while (pBlock != nullptr)
	{
		//���̃I�u�W�F�N�g�����擾
		CBlock* pNext = pBlock->GetNextBlock();

		//�u���b�N��񂪑��݂���ꍇ
		//�^�C�v����v�����ꍇ
		if (pBlock != nullptr)
		{
			//�����蔻�菈��
			aSaveFlag = pBlock->Collision(aParam);

			//���̔��肪�܂�false�̏ꍇ
			if (!aFlag.X && aSaveFlag.X)
			{
				//false�̔����true�ɍX�V
				aFlag.X = aSaveFlag.X;
			}
			//��̔��肪�܂�false�̏ꍇ
			if (!aFlag.Y_UP && aSaveFlag.Y_UP)
			{
				//false�̔����true�ɍX�V
				aFlag.Y_UP = aSaveFlag.Y_UP;
			}
			//���̔��肪�܂�false�̏ꍇ
			if (!aFlag.Y_DOWN && aSaveFlag.Y_DOWN)
			{
				//false�̔����true�ɍX�V
				aFlag.Y_DOWN = aSaveFlag.Y_DOWN;
			}
			//���̔��肪�܂�false�̏ꍇ
			if (!aFlag.Z && aSaveFlag.Z)
			{
				//false�̔����true�ɍX�V
				aFlag.Z = aSaveFlag.Z;
			}
		}

		//�I�u�W�F�N�g�������̏��ɕύX����
		pBlock = pNext;
	}

	return aFlag;
}

//===========================================================================================================
// �u���b�N��������
//===========================================================================================================
CBlock* CBlockManager::FindBlock(CBlock::TYPE type)
{
	//�擪�̃|�C���^���R�s�[
	CBlock* pBlock = m_pTop;

	//���̃u���b�N���nullptr�ɂȂ�܂ŌJ��Ԃ�
	while (pBlock != nullptr)
	{
		//�^�C�v����v�����ꍇ
		if (pBlock->GetBlockType() == type)
		{
			//�|�C���^��Ԃ�
			return pBlock;
		}

		//���̃u���b�N���̃|�C���^���R�s�[
		pBlock = pBlock->GetNextBlock();
	}

	return nullptr;
}
