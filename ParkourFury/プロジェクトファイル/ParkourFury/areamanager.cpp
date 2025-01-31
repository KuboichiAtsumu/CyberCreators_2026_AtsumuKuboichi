//==============================================================================================================================================
//
// �G���A�Ǘ�
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "areamanager.h"
#include "game.h"

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CAreaManager::CAreaManager() : CObjectManager()
{
	//�����o�ϐ�������
	m_pTop = nullptr;//�擪�A�h���X
	m_pCur = nullptr;//�I�[�A�h���X
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CAreaManager::~CAreaManager()
{
}

//===========================================================================================================
// ����������
//===========================================================================================================
HRESULT CAreaManager::Init()
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
void CAreaManager::Uninit()
{
	//�I�u�W�F�N�g�}�l�[�W���[�I������
	CObjectManager::Uninit();
}

//===========================================================================================================
// �X�V����
//===========================================================================================================
void CAreaManager::Update()
{
	//�擪�A�h���X���擾����
	CArea* pArea = m_pTop;

	//�I�u�W�F�N�g���nullptr�ɂȂ�܂ŌJ��Ԃ�
	while (pArea != nullptr)
	{
		//���̃I�u�W�F�N�g�����擾
		CArea* pNext = pArea->GetNextArea();

		//�G���A��񂪑��݂���ꍇ
		//�폜�t���O��true�̏ꍇ
		if (pArea != nullptr && pArea->GetDeleteFlag())
		{
			//�O�̃I�u�W�F�N�g�����擾
			CArea* pPrev = pArea->GetPrevArea();

			//�O�̃I�u�W�F�N�g��񂪑��݂���ꍇ
			if (pPrev != nullptr)
			{
				//���̃I�u�W�F�N�g�����q��
				pPrev->SetNextArea(pNext);
			}
			//�O�̃I�u�W�F�N�g��񂪑��݂��Ȃ��ꍇ
			else
			{
				//���̂̃I�u�W�F�N�g���̑O�̃I�u�W�F�N�g����nullptr�ɐݒ�
				pNext->SetPrevArea(nullptr);

				//�擪�A�h���X��ύX
				m_pTop = pNext;
			}

			//���̃I�u�W�F�N�g��񂪑��݂���ꍇ
			if (pNext != nullptr)
			{
				//�O�̃I�u�W�F�N�g�����q��
				pNext->SetPrevArea(pPrev);
			}
			//���̃I�u�W�F�N�g��񂪑��݂��Ȃ��ꍇ
			else
			{
				//�O�̃I�u�W�F�N�g���̎��̃I�u�W�F�N�g����nullptr�ɐݒ�
				pPrev->SetNextArea(nullptr);

				//�I�[�A�h���X��ύX
				m_pCur = pPrev;
			}
		}

		//�I�u�W�F�N�g�������̏��ɕύX����
		pArea = pNext;
	}
}

//===========================================================================================================
// �`�揈��
//===========================================================================================================
void CAreaManager::Draw()
{
}

//===========================================================================================================
// �o�^����
//===========================================================================================================
void CAreaManager::Regist(CArea* pArea)
{
	//�擪�A�h���X�����݂��Ȃ��ꍇ
	if (m_pTop == nullptr)
	{
		//���������G���A����擪�A�h���X�ɑ��
		m_pTop = pArea;

		//�O�̃I�u�W�F�N�g����nullptr����
		pArea->SetPrevArea(nullptr);
	}

	//�擪�A�h���X�����݂���ꍇ
	else
	{
		//�擪�A�h���X���擾
		CArea* pAllArea = m_pTop;

		//�I�u�W�F�N�g���nullptr�ɂȂ�܂ŌJ��Ԃ�
		while (pAllArea != nullptr)
		{
			//���̃I�u�W�F�N�g��񂪑��݂��Ȃ��ꍇ
			if (pAllArea->GetNextArea() == nullptr)
			{
				//���̃I�u�W�F�N�g���ɐ��������G���A����ۑ�
				pAllArea->SetNextArea(pArea);

				//���������G���A���̑O�̃I�u�W�F�N�g����ۑ�
				pArea->SetPrevArea(pAllArea);
				break;
			}

			//�I�u�W�F�N�g�������̏��ɕύX����
			pAllArea = pAllArea->GetNextArea();
		}
	}

	//���g�̎��̃I�u�W�F�N�g����nullptr����
	pArea->SetNextArea(nullptr);

	//�I�[�A�h���X����
	m_pCur = pArea;
}

//===========================================================================================================
// �Ǎ�����
//===========================================================================================================
void CAreaManager::Load()
{
	//���[�J���ϐ��錾
	CManager* pManager = CManager::GetInstance();//�}�l�[�W���[�̃C���X�^���X
	D3DXVECTOR3 pos = { 0.0f, 0.0f, 0.0f };//���W���ۑ��p
	D3DXVECTOR3 rot = { 0.0f, 0.0f, 0.0f };//�p�x���ۑ��p
	D3DXVECTOR3 scale = { 0.0f, 0.0f, 0.0f };//�g�嗦���ۑ��p
	int nTag = 1;//�^�O���ۑ��p
	int nType = 0;//�^�C�v���ۑ��p
	int nNumAll = 0;//����

	//�t�@�C�����J��
	FILE* pFile = fopen(pManager->StageFile[static_cast<int>(pManager->GetScene()->GetMode())], "rb");

	//�t�@�C�����J���Ȃ�����
	if (pFile == nullptr)
	{
		//�������󂯂�
		return;
	}

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

		//�^�O���G���A�ł͂Ȃ��ꍇ
		if (static_cast<CTag::TAG>(nTag) != CTag::TAG::AREA)
		{
			//���������ɓǍ��𑱂���
			continue;
		}

		//�G���A�𐶐�
		CArea::Create(pos, rot, scale, static_cast<CArea::TYPE>(nType));
	}

	//�t�@�C�������
	fclose(pFile);
}

//===========================================================================================================
// �����蔻��
//===========================================================================================================
bool CAreaManager::Collision(const D3DXVECTOR3& pos)
{
	//�擪�A�h���X���擾����
	CArea* pArea = m_pTop;

	//�I�u�W�F�N�g���nullptr�ɂȂ�܂ŌJ��Ԃ�
	while (pArea != nullptr)
	{
		//���̃I�u�W�F�N�g�����擾
		CArea* pNext = pArea->GetNextArea();

		//�G���A��񂪑��݂���ꍇ
		if (pArea != nullptr && pArea->Collision(pos))
		{
			return true;
		}

		//�I�u�W�F�N�g�������̏��ɕύX����
		pArea = pNext;
	}

	return false;
}

//===========================================================================================================
// ��������
//===========================================================================================================
CArea* CAreaManager::FindArea(CArea::TYPE type)
{
	//�擪�̃|�C���^���R�s�[
	CArea* pArea = m_pTop;

	//���̃u���b�N���nullptr�ɂȂ�܂ŌJ��Ԃ�
	while (pArea != nullptr)
	{
		//�^�C�v����v�����ꍇ
		if (pArea->GetAreaType() == type)
		{
			//�|�C���^��Ԃ�
			return pArea;
		}

		//���̃u���b�N���̃|�C���^���R�s�[
		pArea = pArea->GetNextArea();
	}

	return nullptr;
}
