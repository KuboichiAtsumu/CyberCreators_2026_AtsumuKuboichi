//==============================================================================================================================================
//
// �M�~�b�N���Ǘ�
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "gimmickmanager.h"//�M�~�b�N�}�l�[�W���[

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CGimmickManager::CGimmickManager() : CObjectManager()
{
	//�ÓI�����o�ϐ�������
	m_pTop = nullptr;//�擪�A�h���X
	m_pCur = nullptr;//�I�[�A�h���X
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CGimmickManager::~CGimmickManager()
{
}

//===========================================================================================================
// ����������
//===========================================================================================================
HRESULT CGimmickManager::Init()
{
	//�I�u�W�F�N�g�}�l�[�W���[����������
	if (FAILED(CObjectManager::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===========================================================================================================
// �I������
//===========================================================================================================
void CGimmickManager::Uninit()
{
	//�I�u�W�F�N�g�}�l�[�W���[�I������
	CObjectManager::Uninit();
}

//===========================================================================================================
// �X�V����
//===========================================================================================================
void CGimmickManager::Update()
{
	//�擪�A�h���X���擾����
	CGimmick* pGimmick = m_pTop;

	//�I�u�W�F�N�g���nullptr�ɂȂ�܂ŌJ��Ԃ�
	while (pGimmick != nullptr)
	{
		//���̃I�u�W�F�N�g�����擾
		CGimmick* pNext = pGimmick->GetNext();

		//�폜�t���O��true�̏ꍇ
		if (pGimmick->GetDeleteFlag())
		{
			//�O�̃I�u�W�F�N�g�����擾
			CGimmick* pPrev = pGimmick->GetPrev();

			//�O�̃I�u�W�F�N�g��񂪑��݂���ꍇ
			if (pPrev != nullptr)
			{
				//���̃I�u�W�F�N�g�����q��
				pPrev->SetNext(pNext);
			}
			//�O�̃I�u�W�F�N�g��񂪑��݂��Ȃ��ꍇ
			else
			{
				//���̃I�u�W�F�N�g���̑O�̃I�u�W�F�N�g����nullptr�ɐݒ�
				pNext->SetPrev(nullptr);

				//�擪�A�h���X��ύX
				m_pTop = pNext;
			}

			//���̃I�u�W�F�N�g��񂪑��݂���ꍇ
			if (pNext != nullptr)
			{
				//�O�̃I�u�W�F�N�g�����q��
				pNext->SetPrev(pPrev);
			}
			//���̃I�u�W�F�N�g��񂪑��݂��Ȃ��ꍇ
			else
			{
				//�O�̃I�u�W�F�N�g���̎��̃I�u�W�F�N�g����nullptr�ɐݒ�
				pPrev->SetNext(nullptr);

				//�I�[�A�h���X��ύX
				m_pCur = pPrev;
			}
		}

		//�I�u�W�F�N�g�������̏��ɕύX����
		pGimmick = pNext;
	}
}

//===========================================================================================================
// �`�揈��
//===========================================================================================================
void CGimmickManager::Draw()
{
}

//===========================================================================================================
// �o�^����
//===========================================================================================================
void CGimmickManager::Regist(CGimmick* pGimmick)
{
	//�擪�A�h���X�����݂��Ȃ��ꍇ
	if (m_pTop == nullptr)
	{
		//���������u���b�N����擪�A�h���X�ɑ��
		m_pTop = pGimmick;

		//�O�̃I�u�W�F�N�g����nullptr����
		pGimmick->SetPrev(nullptr);
	}

	//�擪�A�h���X�����݂���ꍇ
	else
	{
		//�擪�A�h���X���擾
		CGimmick* Gimmick = m_pTop;

		//�I�u�W�F�N�g���nullptr�ɂȂ�܂ŌJ��Ԃ�
		while (Gimmick != nullptr)
		{
			//���̃I�u�W�F�N�g��񂪑��݂��Ȃ��ꍇ
			if (Gimmick->GetNext() == nullptr)
			{
				//���̃I�u�W�F�N�g���ɐ��������u���b�N����ۑ�
				Gimmick->SetNext(pGimmick);

				//���������u���b�N���̑O�̃I�u�W�F�N�g����ۑ�
				pGimmick->SetPrev(Gimmick);
				break;
			}

			//�I�u�W�F�N�g�������̏��ɕύX����
			Gimmick = Gimmick->GetNext();
		}
	}

	//���g�̎��̃I�u�W�F�N�g����nullptr����
	pGimmick->SetNext(nullptr);

	//�I�[�A�h���X����
	m_pCur = pGimmick;
}

//===========================================================================================================
// �����蔻��
//===========================================================================================================
void CGimmickManager::Collision(CGimmick::CollisionParam& aParam, CGimmick::TYPE type)
{
	//�擪�A�h���X���擾����
	CGimmick* pGimmick = m_pTop;

	//�I�u�W�F�N�g���nullptr�ɂȂ�܂ŌJ��Ԃ�
	while (pGimmick != nullptr)
	{
		//���̃I�u�W�F�N�g�����擾
		CGimmick* pNext = pGimmick->GetNext();

		//�w�肵���^�C�v�ƈ�v�����ꍇ
		if (pGimmick->GetType() == type)
		{
			//�����蔻��
			pGimmick->Collision(aParam);
		}

		//�I�u�W�F�N�g�������̏��ɕύX����
		pGimmick = pNext;
	}
}

//===========================================================================================================
// ��������
//===========================================================================================================
CGimmick* CGimmickManager::Find(CGimmick::TYPE type)
{
	//�擪�̏����擾
	CGimmick* pGimmick = m_pTop;

	//���̃G�l�~�[���nullptr�ɂȂ�܂ŌJ��Ԃ�
	while (pGimmick != nullptr)
	{
		//�^�C�v����v�����ꍇ
		if (pGimmick->GetType() == type)
		{
			//�|�C���^��Ԃ�
			return pGimmick;
		}

		//���̃G�l�~�[���̃|�C���^���R�s�[
		pGimmick = pGimmick->GetNext();
	}

	return nullptr;
}

//===========================================================================================================
// �M�~�b�N���Ǎ�����
//===========================================================================================================
void CGimmickManager::Load()
{
	//���[�J���ϐ��錾
	D3DXVECTOR3 pos = { 0.0f,0.0f,0.0f };//�ʒu
	D3DXVECTOR3 rot = { 0.0f,0.0f,0.0f };//����
	D3DXVECTOR3 scale = { 1.0f,1.0f,1.0f };//�g�嗦
	int nNumAll = 0;//����
	int nTag = 0;//�^�O
	int nType = 0;//�^�C�v

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
			fread(&pos, sizeof(D3DXVECTOR3), 1, pFile);//�ʒu
			fread(&rot, sizeof(D3DXVECTOR3), 1, pFile);//����
			fread(&scale, sizeof(D3DXVECTOR3), 1, pFile);//�g�嗦
			fread(&nTag, sizeof(int), 1, pFile);//�J�e�S���[
			fread(&nType, sizeof(int), 1, pFile);//���

			//�^�O���M�~�b�N�̏ꍇ
			if (static_cast<CTag::TAG>(nTag) == CTag::TAG::GIMMICK)
			{
				//�M�~�b�N�𐶐�
				CGimmick::Create(pos, scale, static_cast<CGimmick::TYPE>(nType));
			}
		}

		//�t�@�C�������
		fclose(pFile);
	}
}
