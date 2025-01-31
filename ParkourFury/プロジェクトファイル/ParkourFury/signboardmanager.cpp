//==============================================================================================================================================
//
// �ŔI�u�W�F�N�g�Ǘ�
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "signboardmanager.h"

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CSignBoardManager::CSignBoardManager() : CObjectManager()
{
	//�����o�ϐ�������
	m_pTop = nullptr;//�擪�A�h���X
	m_pCur = nullptr;//�I�[�A�h���X
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CSignBoardManager::~CSignBoardManager()
{
}

//===========================================================================================================
// ����������
//===========================================================================================================
HRESULT CSignBoardManager::Init()
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
void CSignBoardManager::Uninit()
{
	//���N���X�I������
	CObjectManager::Uninit();
}

//===========================================================================================================
// �X�V����
//===========================================================================================================
void CSignBoardManager::Update()
{
	//�擪�A�h���X���擾����
	CSignBoard* pSignBoard = m_pTop;

	//�I�u�W�F�N�g���nullptr�ɂȂ�܂ŌJ��Ԃ�
	while (pSignBoard != nullptr)
	{
		//���̃I�u�W�F�N�g�����擾
		CSignBoard* pNext = pSignBoard->GetNext();

		//�G���A��񂪑��݂���ꍇ
		//�폜�t���O��true�̏ꍇ
		if (pSignBoard != nullptr && pSignBoard->GetDeleteFlag())
		{
			//�O�̃I�u�W�F�N�g�����擾
			CSignBoard* pPrev = pSignBoard->GetPrev();

			//�O�̃I�u�W�F�N�g��񂪑��݂���ꍇ
			if (pPrev != nullptr)
			{
				//���̃I�u�W�F�N�g�����q��
				pPrev->SetNext(pNext);
			}
			//�O�̃I�u�W�F�N�g��񂪑��݂��Ȃ��ꍇ
			else
			{
				//���̂̃I�u�W�F�N�g���̑O�̃I�u�W�F�N�g����nullptr�ɐݒ�
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
		pSignBoard = pNext;
	}
}

//===========================================================================================================
// �`�揈��
//===========================================================================================================
void CSignBoardManager::Draw()
{
}

//===========================================================================================================
// �o�^����
//===========================================================================================================
void CSignBoardManager::Regist(CSignBoard* pSignBoard)
{
	//�擪�A�h���X�����݂��Ȃ��ꍇ
	if (m_pTop == nullptr)
	{
		//���������G���A����擪�A�h���X�ɑ��
		m_pTop = pSignBoard;

		//�O�̃I�u�W�F�N�g����nullptr����
		pSignBoard->SetPrev(nullptr);
	}

	//�擪�A�h���X�����݂���ꍇ
	else
	{
		//�擪�A�h���X���擾
		CSignBoard* pAllSignBoard = m_pTop;

		//�I�u�W�F�N�g���nullptr�ɂȂ�܂ŌJ��Ԃ�
		while (pAllSignBoard != nullptr)
		{
			//���̃I�u�W�F�N�g��񂪑��݂��Ȃ��ꍇ
			if (pAllSignBoard->GetNext() == nullptr)
			{
				//���̃I�u�W�F�N�g���ɐ��������G���A����ۑ�
				pAllSignBoard->SetNext(pSignBoard);

				//���������G���A���̑O�̃I�u�W�F�N�g����ۑ�
				pSignBoard->SetPrev(pAllSignBoard);
				break;
			}

			//�I�u�W�F�N�g�������̏��ɕύX����
			pAllSignBoard = pAllSignBoard->GetNext();
		}
	}

	//���g�̎��̃I�u�W�F�N�g����nullptr����
	pSignBoard->SetNext(nullptr);

	//�I�[�A�h���X����
	m_pCur = pSignBoard;
}

//===========================================================================================================
// �Ǎ�����
//===========================================================================================================
void CSignBoardManager::Load()
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

	//�t�@�C�����J���Ȃ������ꍇ�����𔲂���
	if (pFile == nullptr) return;
	
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

		//�^�O����v
		if (static_cast<CTag::TAG>(nTag) == CTag::TAG::SIGN_BOARD)
		{
			//�G���A�𐶐�
			CSignBoard::Create(static_cast<CSignBoard::TYPE>(nType), pos, scale, rot);
		}
	}

	//�t�@�C�������
	fclose(pFile);
}

//===========================================================================================================
// ��������
//===========================================================================================================
CSignBoard* CSignBoardManager::Find(CSignBoard::TYPE type)
{
	//�擪�̃|�C���^���R�s�[
	CSignBoard* pSignBoard = m_pTop;

	//���̃u���b�N���nullptr�ɂȂ�܂ŌJ��Ԃ�
	while (pSignBoard != nullptr)
	{
		//�^�C�v����v
		if (pSignBoard->GetType() == type) return pSignBoard;
		
		//���̃u���b�N���̃|�C���^���R�s�[
		pSignBoard = pSignBoard->GetNext();
	}

	return nullptr;
}
