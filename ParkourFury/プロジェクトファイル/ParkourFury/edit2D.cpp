//==============================================================================================================================================
//
// 2D�G�f�B�^�̏���
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifdef _DEBUG

//===========================================================================================================
// �t�@�C���C���N���[�h
//===========================================================================================================
#include "edit2D.h"
#include "UImanager.h"
#include "heatgageframe.h"
#include "heatgage.h"
#include "timenumber.h"
#include "timerheading.h"
#include "mg_timerback.h"
#include "mg_timergage.h"
#include "mg_timerframe.h"
#include "titlelogo.h"
#include "startbutton.h"
#include "resulthead.h"
#include "resulttimehead.h"
#include "resultkillhead.h"
#include "resultheathead.h"
#include "seconds.h"
#include "percent.h"

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CEdit2D::CEdit2D() : 
	m_nTarget{ 0 },
	m_bTrigger{ false }
{
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CEdit2D::~CEdit2D()
{
}

//===========================================================================================================
// ����������
//===========================================================================================================
HRESULT CEdit2D::Init()
{
	return S_OK;
}

//===========================================================================================================
// �I������
//===========================================================================================================
void CEdit2D::Uninit()
{
}

//===========================================================================================================
// �������
//===========================================================================================================
void CEdit2D::Release()
{
}

//===========================================================================================================
// �X�V����
//===========================================================================================================
void CEdit2D::Update()
{
	//�}�l�[�W���[�̃C���X�^���X���擾
	CManager* pManager = CManager::GetInstance();

	//UI�J�e�S���[�̐擪�I�u�W�F�N�g�擾
	CUI* pUI = pManager->GetScene()->GetUIManager()->GetTop();

	//�擪�I�u�W�F�N�g�����݂��Ȃ��ꍇ
	if (pUI == nullptr)
	{
		//�����
		CUI::Create(static_cast<CUI::TYPE>(1), { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f }, { 10.0f, 10.0f }, { 0.0f, 0.0f, 0.0f });
	}

	//�L�[�{�[�h�̃C���X�^���X���擾
	CInputKeyboard* pKeyboard = pManager->GetKeyboard();

	//=======================================================================
	// �ړ����@�؂�ւ�
	//=======================================================================
	if (pKeyboard->GetTrigger(DIK_1))
	{//1�L�[�������ꂽ�ꍇ
		m_bTrigger = m_bTrigger ? false : true;
	}

	//=======================================================================
	// �ҏW�ΏەύX
	//=======================================================================

	//N�L�[�������ꂽ�ꍇ
	if (pKeyboard->GetRepeat(DIK_N, 10))
	{
		//�O�̃I�u�W�F�N�g��Ώۂɂ���
		m_nTarget--;

		//�J�E���g���I�u�W�F�N�g�̍ő吔�ɓ��B�����ꍇ
		if (m_nTarget < 0)
		{
			//��ԍŌ�Ƀ��[�v����
			m_nTarget = CUI::m_nAll - 1;
		}
	}
	//M�L�[�������ꂽ�ꍇ
	else if (pKeyboard->GetRepeat(DIK_M, 10))
	{
		//��̃I�u�W�F�N�g��Ώۂɂ���
		m_nTarget++;

		//�J�E���g���I�u�W�F�N�g�̍ő吔�ɓ��B�����ꍇ
		if (m_nTarget > CUI::m_nAll - 1)
		{
			//��ԍŏ��Ƀ��[�v����
			m_nTarget = 0;
		}
	}

	//=======================================================================
	// �f�[�^�̏����o��
	//=======================================================================
	if (pKeyboard->GetTrigger(DIK_F3))
	{//F3�L�[�������ꂽ�ꍇ

		//�����o������
		Save();
	}

	//�I�u�W�F�N�g���nullptr�ɂȂ�܂ŌJ��Ԃ�
	while (pUI != nullptr)
	{
		//���̃I�u�W�F�N�g�����擾
		CUI* pNext = pUI->GetNextUI();

		//�ҏW�Ώ۔ԍ��ƃC���f�b�N�X����v
		if (m_nTarget == pUI->GetIdx() && pUI->GetEditFlag())
		{
			//���\������
			PrintInfo(pUI);

			//�e�ҏW����
			Move(pUI);//���W
			Scale(pUI);//�T�C�Y
			Turn(pUI);//�p�x
			Reset(pUI);//���Z�b�g
			Change(pUI);//�^�C�v�ύX
			Delete(pUI);//�폜����
			Create(pUI);//�z�u����

			//�������ɂ���
			pUI->SetColor({ 1.0f, 1.0f, 1.0f, 0.5f });
			break;
		}
		else
		{
			//�J���[��߂�
			pUI->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		}

		//�I�u�W�F�N�g�������̏��ɕύX����
		pUI = pNext;
	}
}

//===========================================================================================================
// �ړ�����
//===========================================================================================================
void CEdit2D::Move(CUI* pUI)
{
	//�L�[�{�[�h�̃C���X�^���X���擾
	CInputKeyboard* pKeyboard = CManager::GetInstance()->GetKeyboard();

	//���݂̍��W�����擾
	D3DXVECTOR2 pos = pUI->GetPos();

	//Press
	if (!m_bTrigger)
	{
		//A�L�[�������ꂽ�ꍇ
		if (pKeyboard->GetPress(DIK_A))
		{
			//���W���X�V
			pos.x -= MOVE;
		}
		//D�L�[�������ꂽ�ꍇ
		else if (pKeyboard->GetPress(DIK_D))
		{
			//���W���X�V
			pos.x += MOVE;
		}
		//W�L�[�������ꂽ�ꍇ
		else if (pKeyboard->GetPress(DIK_W))
		{
			//���W���X�V
			pos.y -= MOVE;
		}
		//S�L�[�������ꂽ�ꍇ
		else if (pKeyboard->GetPress(DIK_S))
		{
			//���W���X�V
			pos.y += MOVE;
		}
	}
	//Trigger
	else
	{
		//A�L�[�������ꂽ�ꍇ
		if (pKeyboard->GetTrigger(DIK_A))
		{
			//���W���X�V
			pos.x -= MOVE;
		}
		//D�L�[�������ꂽ�ꍇ
		else if (pKeyboard->GetTrigger(DIK_D))
		{
			//���W���X�V
			pos.x += MOVE;
		}
		//W�L�[�������ꂽ�ꍇ
		else if (pKeyboard->GetTrigger(DIK_W))
		{
			//���W���X�V
			pos.y -= MOVE;
		}
		//S�L�[�������ꂽ�ꍇ
		else if (pKeyboard->GetTrigger(DIK_S))
		{
			//���W���X�V
			pos.y += MOVE;
		}
	}

	//�X�V���ꂽ���W����ݒ�
	pUI->SetPos(pos);
}

//===========================================================================================================
// �g�又��
//===========================================================================================================
void CEdit2D::Scale(CUI* pUI)
{
	//�L�[�{�[�h�̃C���X�^���X���擾
	CInputKeyboard* pKeyboard = CManager::GetInstance()->GetKeyboard();

	//���݂̃T�C�Y�����擾
	D3DXVECTOR2 size = pUI->GetSize();

	//Press
	if (!m_bTrigger)
	{
		//G�L�[�������ꂽ�ꍇ
		if (pKeyboard->GetPress(DIK_G))
		{
			//������Z������
			size.x -= SCALE;
		}
		//J�L�[�������ꂽ�ꍇ
		else if (pKeyboard->GetPress(DIK_J))
		{
			//�����𒷂�����
			size.x += SCALE;
		}
		//Y�L�[�������ꂽ�ꍇ
		else if (pKeyboard->GetPress(DIK_Y))
		{
			//�c���𒷂�����
			size.y += SCALE;
		}
		//H�L�[�������ꂽ�ꍇ
		else if (pKeyboard->GetPress(DIK_H))
		{
			//�c����Z������
			size.y -= SCALE;
		}
	}
	//Trigger
	else
	{
		//G�L�[�������ꂽ�ꍇ
		if (pKeyboard->GetTrigger(DIK_G))
		{
			//������Z������
			size.x -= SCALE;
		}
		//J�L�[�������ꂽ�ꍇ
		else if (pKeyboard->GetTrigger(DIK_J))
		{
			//�����𒷂�����
			size.x += SCALE;
		}
		//Y�L�[�������ꂽ�ꍇ
		else if (pKeyboard->GetTrigger(DIK_Y))
		{
			//�c���𒷂�����
			size.y += SCALE;
		}
		//H�L�[�������ꂽ�ꍇ
		else if (pKeyboard->GetTrigger(DIK_H))
		{
			//�c����Z������
			size.y -= SCALE;
		}
	}

	//�X�V���ꂽ�T�C�Y����ݒ�
	pUI->SetSize(size);
}

//===========================================================================================================
// ��]����
//===========================================================================================================
void CEdit2D::Turn(CUI* pUI)
{
	//�L�[�{�[�h�̃C���X�^���X���擾
	CInputKeyboard* pKeyboard = CManager::GetInstance()->GetKeyboard();

	//���݂̊p�x�����擾
	D3DXVECTOR3 rot = pUI->GetRot();

	//Z�L�[�������ꂽ�ꍇ
	if (pKeyboard->GetTrigger(DIK_Z))
	{
		//�E��]
		rot.z -= TURN;
	}
	//X�L�[�������ꂽ�ꍇ
	else if (pKeyboard->GetTrigger(DIK_X))
	{
		//����]
		rot.z += TURN;
	}

	//�p�x�␳
	CObject::CorrectionRot(rot.z);

	//�X�V���ꂽ�p�x����ݒ�
	pUI->SetRot(rot);
}

//===========================================================================================================
// ���Z�b�g����
//===========================================================================================================
void CEdit2D::Reset(CUI* pUI)
{
	//R�L�[�������ꂽ�ꍇ
	if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_R))
	{
		//�p�����[�^���Z�b�g
		pUI->SetPos({ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f });//���W
		pUI->SetRot({ 0.0f, 0.0f, 0.0f });//�p�x
		pUI->SetSize({ 1.0f, 1.0f });//�T�C�Y
	}
}

//===========================================================================================================
// �^�C�v�ύX����
//===========================================================================================================
void CEdit2D::Change(CUI* pUI)
{
	//�L�[�{�[�h�̃C���X�^���X���擾
	CInputKeyboard* pKeyboard = CManager::GetInstance()->GetKeyboard();

	//���݂̃^�O����ۑ�
	int nType = static_cast<int>(pUI->GetUIType());

	//Q�L�[�������ꂽ�ꍇ
	if (pKeyboard->GetTrigger(DIK_Q) && nType > 1)
	{
		//�^�C�v�ύX
		nType--;

		//UI���폜
		pUI->Uninit();

		while (1)
		{
			//�V����UI�𐶐�
			CUI* pNewUI = CUI::Create(static_cast<CUI::TYPE>(nType), pUI->GetPos(), pUI->GetSize(), pUI->GetRot());

			//�쐬���ꂽUI�̃G�f�B�^�t���O��true
			if (pNewUI->GetEditFlag())
			{
				//���[�v�𔲂���
				break;
			}
			//�쐬���ꂽUI�̃G�f�B�^�t���O��false
			else
			{
				//UI���폜
				pNewUI->Uninit();

				//����Ƀ^�C�v��ύX
				nType--;
			}
		}
	}
	//E�L�[�������ꂽ�ꍇ
	else if (pKeyboard->GetTrigger(DIK_E) && nType < static_cast<int>(CUI::TYPE::MAX) - 1)
	{
		//�^�C�v�ύX
		nType++;

		//UI���폜
		pUI->Uninit();

		while (1)
		{
			//�V����UI�𐶐�
			CUI* pNewUI = CUI::Create(static_cast<CUI::TYPE>(nType), pUI->GetPos(), pUI->GetSize(), pUI->GetRot());

			//�쐬���ꂽUI�̃G�f�B�^�t���O��true
			if (pNewUI->GetEditFlag())
			{
				//���[�v�𔲂���
				break;
			}
			//�쐬���ꂽUI�̃G�f�B�^�t���O��false
			else
			{
				//UI���폜
				pNewUI->Uninit();

				//����Ƀ^�C�v��ύX
				nType++;
			}
		}
	}
}

//===========================================================================================================
// �폜����
//===========================================================================================================
void CEdit2D::Delete(CUI* pUI)
{
	if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_DELETE)
		&& CUI::m_nAll > 1)
	{//DELETE�L�[�������ꂽ�ꍇ

		//�I������
		pUI->Uninit();

		//�Ώۂ�1�O����
		m_nTarget--;

		//�^�[�Q�b�g�ԍ���0�ȉ��ɂȂ�Ȃ悤�ɒ���
		if (m_nTarget < 0)
		{
			m_nTarget = 0;
		}
	}
}

//===========================================================================================================
// �z�u����
//===========================================================================================================
void CEdit2D::Create(CUI* pUI)
{
	//�}�l�[�W���[�̃C���X�^���X���擾
	CManager* pManager = CManager::GetInstance();

	//�L�[�{�[�h�̃C���X�^���X���擾
	CInputKeyboard* pKeyboard = pManager->GetKeyboard();

	//B�L�[
	if (pKeyboard->GetTrigger(DIK_B))
	{
		m_nTarget = CUI::Create(pUI->GetUIType(), pUI->GetPos(), pUI->GetSize(), pUI->GetRot())->GetIdx();
	}
}

//===========================================================================================================
// ���\������
//===========================================================================================================
void CEdit2D::PrintInfo(CUI* pUI)
{
	PrintDebugProc("\n==============================\n ����\n==============================\n");
	PrintDebugProc("��]\n����] : Z   �E��] : X\n\n");
	PrintDebugProc("�ʒu�Ɗp�x�����Z�b�g : R\n\n");
	PrintDebugProc("�^�C�v�ύX : Q,E\n");
	PrintDebugProc("�ҏW�ΏەύX : N,M\n");
	PrintDebugProc("�z�u : B\n");
	PrintDebugProc("�폜 : DELETE\n");
	PrintDebugProc("�����o�� : F3\n\n");

	//�z�u���f�[�^�\��
	PrintDebugProc("==============================\n �z�u���f�[�^\n==============================\n");

	//���샂�[�h��\��
	PrintDebugProc("���샂�[�h( 1�Ő؂�ւ� ) : ");
	if (!m_bTrigger)
	{
		//�v���X
		PrintDebugProc("Press\n\n");
	}
	else
	{
		//�g���K�[
		PrintDebugProc("Trigger\n\n");
	}

	D3DXVECTOR2 pos = pUI->GetPos();
	PrintDebugProc("POS X:%f, Y:%f\n\n", pos.x, pos.y);

	D3DXVECTOR2 size = pUI->GetSize();
	PrintDebugProc("SIZE X:%f, Y:%f\n\n", size.x, size.y);

	D3DXVECTOR3 rot = pUI->GetRot();
	PrintDebugProc("ROT X:%f, Y:%f, Z:%f\n\n", rot.x, rot.y, rot.z);//�p�x

	PrintDebugProc("Idx:%d\n", pUI->GetIdx());//�C���f�b�N�X
	PrintDebugProc("TYPE:%d\n", static_cast<int>(pUI->GetUIType()));//�^�C�v

	PrintDebugProc("\n�z�u��:%d/%d\n", m_nTarget + 1, CUI::m_nAll);//�z�u��
}

//===========================================================================================================
// �Z�[�u����
//===========================================================================================================
void CEdit2D::Save()
{
	//�}�l�[�W���[�̃C���X�^���X���擾
	CManager* pManager = CManager::GetInstance();

	//�L�[�{�[�h�̃C���X�^���X���擾
	CScene* pScene = pManager->GetScene();

	//�t�@�C�����J��
	FILE* pFile = fopen(pManager->UIFile[static_cast<int>(pScene->GetMode())], "wb");

	//�t�@�C�������������ꍇ
	if (pFile != nullptr)
	{
		//�z�u���������o��
		fwrite(&CUI::m_nAll, sizeof(int), 1, pFile);

		//UI�J�e�S���[�̐擪�I�u�W�F�N�g�擾
		CUI* pUI = pScene->GetUIManager()->GetTop();

		//�I�u�W�F�N�g���nullptr�ɂȂ�܂ŌJ��Ԃ�
		while (pUI != nullptr)
		{
			//���̃I�u�W�F�N�g�����擾
			CUI* pNext = pUI->GetNextUI();

			//�f�[�^�̋L�^
			int nType = static_cast<int>(pUI->GetUIType());
			fwrite(&nType, sizeof(int), 1, pFile);//�^�C�v
			fwrite(pUI->GetPos(), sizeof(D3DXVECTOR2), 1, pFile);//���W
			fwrite(pUI->GetSize(), sizeof(D3DXVECTOR2), 1, pFile);//�T�C�Y
			fwrite(pUI->GetRot(), sizeof(D3DXVECTOR3), 1, pFile);//�p�x
			bool bSaveEditFlag = pUI->GetEditFlag();
			fwrite(&bSaveEditFlag, sizeof(bool), 1, pFile);//�G�f�B�b�g�\�t���O
			
			//�I�u�W�F�N�g�������̏��ɕύX����
			pUI = pNext;
		}

		//�t�@�C�������
		fclose(pFile);
	}
}

#endif // _DEBUG
