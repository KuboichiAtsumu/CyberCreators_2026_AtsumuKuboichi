#ifdef _DEBUG
//==============================================================================================================================================
//
// �G�f�B�^�I�u�W�F�N�g�Ǘ�
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "editmanager.h"

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CEdit3DManager::CEdit3DManager() : CObjectManager()
{
	//�����o�ϐ�������
	m_pTop = nullptr;//�擪�A�h���X
	m_pCur = nullptr;//�I�[�A�h���X
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CEdit3DManager::~CEdit3DManager()
{
}

//===========================================================================================================
// ����������
//===========================================================================================================
HRESULT CEdit3DManager::Init()
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
void CEdit3DManager::Uninit()
{
	//�I�u�W�F�N�g�}�l�[�W���[�I������
	CObjectManager::Uninit();
}

//===========================================================================================================
// �X�V����
//===========================================================================================================
void CEdit3DManager::Update()
{
	//�擪�A�h���X���擾����
	CEdit3D* pEdit = m_pTop;

	//�I�u�W�F�N�g���nullptr�ɂȂ�܂ŌJ��Ԃ�
	while (pEdit != nullptr)
	{
		//���̃I�u�W�F�N�g�����擾
		CEdit3D* pNext = pEdit->GetNextEdit();

		//�u���b�N��񂪑��݂���ꍇ
		//�폜�t���O��true�̏ꍇ
		if (pEdit != nullptr && pEdit->GetDeleteFlag())
		{
			//�O�̃I�u�W�F�N�g�����擾
			CEdit3D* pPrev = pEdit->GetPrevEdit();

			//�O�̃I�u�W�F�N�g��񂪑��݂���ꍇ
			if (pPrev != nullptr)
			{
				//���̃I�u�W�F�N�g�����q��
				pPrev->SetNextEdit(pNext);
			}
			//�O�̃I�u�W�F�N�g��񂪑��݂��Ȃ��ꍇ
			else
			{
				//���̂̃I�u�W�F�N�g���̑O�̃I�u�W�F�N�g����nullptr�ɐݒ�
				pNext->SetPrevEdit(nullptr);

				//�擪�A�h���X��ύX
				m_pTop = pNext;
			}

			//���̃I�u�W�F�N�g��񂪑��݂���ꍇ
			if (pNext != nullptr)
			{
				//�O�̃I�u�W�F�N�g�����q��
				pNext->SetPrevEdit(pPrev);

				//�C���f�b�N�X���l�߂�
				pNext->SetIdx(pEdit->GetIdx());
			}
			//���̃I�u�W�F�N�g��񂪑��݂��Ȃ��ꍇ
			else
			{
				//�O�̃I�u�W�F�N�g���̎��̃I�u�W�F�N�g����nullptr�ɐݒ�
				pPrev->SetNextEdit(nullptr);

				//�I�[�A�h���X��ύX
				m_pCur = pPrev;
			}
		}

		//�I�u�W�F�N�g�������̏��ɕύX����
		pEdit = pNext;
	}
}

//===========================================================================================================
// �`�揈��
//===========================================================================================================
void CEdit3DManager::Draw()
{
}

//===========================================================================================================
// �o�^����
//===========================================================================================================
void CEdit3DManager::Regist(CEdit3D* pEdit)
{
	//�擪�A�h���X�����݂��Ȃ��ꍇ
	if (m_pTop == nullptr)
	{
		//���������u���b�N����擪�A�h���X�ɑ��
		m_pTop = pEdit;

		//�O�̃I�u�W�F�N�g����nullptr����
		pEdit->SetPrevEdit(nullptr);
	}

	//�擪�A�h���X�����݂���ꍇ
	else
	{
		//�擪�A�h���X���擾
		CEdit3D* pAllEdit = m_pTop;

		//�I�u�W�F�N�g���nullptr�ɂȂ�܂ŌJ��Ԃ�
		while (pAllEdit != nullptr)
		{
			//���̃I�u�W�F�N�g��񂪑��݂��Ȃ��ꍇ
			if (pAllEdit->GetNextEdit() == nullptr)
			{
				//���̃I�u�W�F�N�g���ɐ��������u���b�N����ۑ�
				pAllEdit->SetNextEdit(pEdit);

				//���������u���b�N���̑O�̃I�u�W�F�N�g����ۑ�
				pEdit->SetPrevEdit(pAllEdit);
				break;
			}

			//�I�u�W�F�N�g�������̏��ɕύX����
			pAllEdit = pAllEdit->GetNextEdit();
		}
	}

	//���g�̎��̃I�u�W�F�N�g����nullptr����
	pEdit->SetNextEdit(nullptr);

	//�I�[�A�h���X����
	m_pCur = pEdit;
}

//===========================================================================================================
// �Ǎ�����
//===========================================================================================================
void CEdit3DManager::Load()
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

			//�G�f�B�^�I�u�W�F�N�g���ݒ�
			CEdit3D aEdit = { static_cast<int>(Category::EDIT) };
			aEdit.SetPos(pos);//���W
			aEdit.SetRot(rot);//�p�x
			aEdit.SetScale(scale);//�g�嗦
			aEdit.SetTag(static_cast<CTag::TAG>(nTag));//�^�O
			aEdit.SetType(nType);//�^�C�v

			//��������
			CEdit3D::Create(aEdit);
		}

		//�t�@�C�������
		fclose(pFile);
	}

	//�I�u�W�F�N�g�f�[�^��1�����݂��Ȃ��ꍇ
	if (nNumAll == 0)
	{
		//�G�f�B�^�I�u�W�F�N�g���ݒ�
		CEdit3D aEdit = { static_cast<int>(Category::EDIT) };
		aEdit.SetPos({ 0.0f, 0.0f, 0.0f });//���W�������ݒ�
		aEdit.SetRot({ 0.0f, 0.0f, 0.0f });//�p�x�������ݒ�
		aEdit.SetScale({ 1.0f, 1.0f, 1.0f });//�g�嗦�������ݒ�
		aEdit.SetTag(static_cast<CTag::TAG>(nTag));//�^�O
		aEdit.SetType(1);//�^�C�v

		//�I�u�W�F�N�g��1����
		CEdit3D::Create(aEdit);
	}
}

//===========================================================================================================
// �C���f�b�N�X��������
//===========================================================================================================
CEdit3D* CEdit3DManager::Find(int nIdx)
{
	//�擪�̏����擾
	CEdit3D* pEdit = m_pTop;

	//���̏��nullptr�ɂȂ�܂ŌJ��Ԃ�
	while (pEdit != nullptr)
	{
		//�C���f�b�N�X����v�����ꍇ
		if (pEdit->GetIdx() == nIdx)
		{
			//�|�C���^��Ԃ�
			return pEdit;
		}

		//���̏����擾
		pEdit = pEdit->GetNextEdit();
	}

	return nullptr;
}
#endif // _DEBUG