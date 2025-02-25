//==============================================================================================================================================
//
// 3D�G�f�B�^�̏���
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifdef _DEBUG

//===========================================================================================================
// �t�@�C���C���N���[�h
//===========================================================================================================
#include "edit3D.h"
#include "editmanager.h"

//===========================================================================================================
// �ÓI�����o�ϐ�������
//===========================================================================================================
CEdit3D::MODE CEdit3D::m_mode = CEdit3D::MODE::MODE_MOVE;//���샂�[�h
int CEdit3D::m_nAll = 0;//�z�u��
int CEdit3D::m_nTarget = 0;//�ҏW�Ώ۔ԍ�
bool CEdit3D::m_bTrigger = false;//�ړ����@���g���K�[���ǂ���

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CEdit3D::CEdit3D(int nPriority) : CObjectX(nPriority),
	m_pPrev(nullptr),
	m_pNext(nullptr),
	m_tag(CTag::TAG::BLOCK),
	m_nType(1)
{
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CEdit3D::~CEdit3D()
{
}

//===========================================================================================================
// ����������
//===========================================================================================================
HRESULT CEdit3D::Init()
{
	//�����o�ϐ�������
	m_nIdx = m_nAll;//ID
	m_nAll++;//�z�u�������Z

	//�ҏW�Ώېݒ�
	m_nTarget = m_nIdx;

	//X�t�@�C���Ǘ��̃C���X�^���X�擾
	CXfile* pXfile = CXfile::GetInstance();

	//���f����񔽉f
	CXfile::ModelInfo* pModel = pXfile->GetAddress(m_tag, m_nType);
	if (pModel == nullptr)
	{
		pModel = pXfile->GetAddress(m_tag, 1);
	}
	BindModel(pModel);

	//�e�N�X�`���Ǘ��̃C���X�^���X�擾
	CTexture* pTexManager = CTexture::GetInstance();

	//�e�N�X�`����񔽉f
	LPDIRECT3DTEXTURE9 pTexture = pTexManager->GetAddress(m_tag, m_nType);
	if (pTexture == nullptr)
	{
		pTexture = pTexManager->GetAddress(m_tag, 1);
	}
	BindTexture(pTexture);

	//X�t�@�C���I�u�W�F�N�g����������
	if (FAILED(CObjectX::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===========================================================================================================
// �I������
//===========================================================================================================
void CEdit3D::Uninit()
{
	//X�t�@�C���I�u�W�F�N�g�I������
	CObjectX::Uninit();

	//�z�u����1���炷
	m_nAll--;
}

//===========================================================================================================
// �������
//===========================================================================================================
void CEdit3D::Release()
{
	//X�t�@�C���I�u�W�F�N�g�������
	CObjectX::Release();
}

//===========================================================================================================
// �G�f�B�^���X�V����
//===========================================================================================================
void CEdit3D::EditUpdate()
{
	//=======================================================================
	// �ړ����@�؂�ւ�
	//=======================================================================
	if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_1))
	{//1�L�[�������ꂽ�ꍇ
		m_bTrigger = m_bTrigger ? false : true;
	}

	//=======================================================================
	// ���샂�[�h�ύX
	//=======================================================================

	//F5�L�[�������ꂽ�ꍇ
	if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_F5))
	{
		//�ړ�
		m_mode = CEdit3D::MODE::MODE_MOVE;
	}
	//F6�L�[�������ꂽ�ꍇ
	else if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_F6))
	{
		//�g��
		m_mode = CEdit3D::MODE::MODE_SCALE;
	}

	//=======================================================================
	// �z�u
	//=======================================================================
	if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_B))
	{//B�L�[�������ꂽ�ꍇ

		//���ݑI�����Ă���G�f�B�^�I�u�W�F�N�g�����擾
		CEdit3D* pEdit = CManager::GetInstance()->GetScene()->GetEditManager()->Find(m_nTarget);

		//�擾�������ŐV���ɐ���
		Create(*pEdit);
	}

	//=======================================================================
	// �ҏW�ΏەύX
	//=======================================================================

	//N�L�[�������ꂽ�ꍇ
	if (CManager::GetInstance()->GetKeyboard()->GetRepeat(DIK_N, 10))
	{
		//��SHIFT�L�[��������Ă���ꍇ
		if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_LSHIFT))
		{
			//10�O�̃I�u�W�F�N�g��Ώۂɂ���
			m_nTarget -= 10;
		}
		else
		{
			//�O�̃I�u�W�F�N�g��Ώۂɂ���
			m_nTarget--;
		}

		//�J�E���g���I�u�W�F�N�g�̍ő吔�ɓ��B�����ꍇ
		if (m_nTarget < 0)
		{
			//��ԍŌ�Ƀ��[�v����
			m_nTarget = m_nAll - 1;
		}
	}
	//M�L�[�������ꂽ�ꍇ
	else if (CManager::GetInstance()->GetKeyboard()->GetRepeat(DIK_M, 10))
	{
		//��SHIFT�L�[��������Ă���ꍇ
		if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_LSHIFT))
		{
			//10��̃I�u�W�F�N�g��Ώۂɂ���
			m_nTarget += 10;
		}
		else
		{
			//��̃I�u�W�F�N�g��Ώۂɂ���
			m_nTarget++;
		}

		//�J�E���g���I�u�W�F�N�g�̍ő吔�ɓ��B�����ꍇ
		if (m_nTarget > m_nAll - 1)
		{
			//��ԍŏ��Ƀ��[�v����
			m_nTarget = 0;
		}
	}

	//=======================================================================
	// �f�[�^�̏����o��
	//=======================================================================
	if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_F3))
	{//F3�L�[�������ꂽ�ꍇ

		//�����o������
		Save();
	}
}

//===========================================================================================================
// �X�V����
//===========================================================================================================
void CEdit3D::Update()
{
	//�G�f�B�^���[�h�̏ꍇ
	if (CManager::GetInstance()->GetScene()->GetSceneState() != CScene::SCENE::EDIT_3D)
	{//�`�悹���ɏ����𔲂���
		return;
	}

	//�ҏW�Ώۂ�ID����v���Ȃ��ꍇ
	if (m_nTarget != m_nIdx)
	{
		//�����𔲂���
		return;
	}

	//=========================================================================================
	// ������@�\��
	//=========================================================================================
	PrintDebugProc("==============================\n ����( F5 : �ړ��@F6 : �g�� )\n==============================\n");

	switch (m_mode)
	{
		//�ړ����[�h�̏ꍇ
	case CEdit3D::MODE::MODE_MOVE:
		PrintDebugProc("�ړ�\nX���ړ�:T,G Y���ړ�:Y,H Z���ړ�:U,J\n\n");
		break;
		//�g�k���[�h�̏ꍇ
	case CEdit3D::MODE::MODE_SCALE:
		PrintDebugProc("�g��\nX���g��:T,G Y���g��:Y,H Z���g��:U,J\n\n");
		break;
	default:
		break;
	}

	PrintDebugProc("��]\n����] : Z   �E��] : X\n\n");
	PrintDebugProc("�ʒu�Ɗp�x�����Z�b�g : R\n\n");
	PrintDebugProc("�^�O�ύX : ��SHIFT + Q,E\n");
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

	//���샂�[�h�ʂɏ���\��
	switch (m_mode)
	{
	//���W
	case CEdit3D::MODE::MODE_MOVE:
	{
		//���W�����擾
		D3DXVECTOR3 pos = GetPos();
		PrintDebugProc("POS X:%f Y:%f Z:%f\n\n", pos.x, pos.y, pos.z);
		break;
	}
	//�g�嗦
	case CEdit3D::MODE::MODE_SCALE:
	{
		//�g�嗦�����擾
		D3DXVECTOR3 scale = GetScale();
		PrintDebugProc("SCALE X:%f Y:%f Z:%f\n\n", scale.x, scale.y, scale.z);
		break;
	}
	default:
		break;
	}

	//�p�x�����擾
	D3DXVECTOR3 rot = GetRot();
	PrintDebugProc("ROT X:%f Y:%f Z:%f\n\n", rot.x, rot.y, rot.z);//�p�x

	//�t�@�C���p�X��\��
	CXfile::Model aModel = CXfile::GetInstance()->GetModel(m_tag, m_nType);
	PrintDebugProc("FILE_NAME : %s\n\n", aModel.FileName.c_str());

	PrintDebugProc("TAG:%d\n", static_cast<int>(m_tag));//�^�O
	PrintDebugProc("TYPE:%d\n", m_nType);//�^�C�v

	PrintDebugProc("\n�z�u��:%d/%d\n", m_nTarget + 1, m_nAll);//�z�u��

	//=========================================================================================
	// �I�u�W�F�N�g�f�[�^�X�V
	//=========================================================================================

	//���f����񔽉f
	CXfile::ModelInfo* pModel = CXfile::GetInstance()->GetAddress(m_tag, m_nType);
	if (pModel != nullptr)
	{
		BindModel(pModel);
	}

	//�e�N�X�`����񔽉f
	LPDIRECT3DTEXTURE9 pTexture = CTexture::GetInstance()->GetAddress(m_tag, m_nType);
	if (pTexture != nullptr)
	{
		BindTexture(pTexture);
	}

	//���샂�[�h�ʏ���
	switch (m_mode)
	{
		//�ړ�
	case CEdit3D::MODE::MODE_MOVE:
		Move();
		break;

		//�g��
	case CEdit3D::MODE::MODE_SCALE:
		Scale();
		break;

	default:
		break;
	}

	//=======================================================================
	// �폜
	//=======================================================================
	if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_DELETE)
		&& m_nAll > 1)
	{//DELETE�L�[�������ꂽ�ꍇ

		//�I������
		Uninit();

		//�Ώۂ�1�O����
		m_nTarget--;

		//�^�[�Q�b�g�ԍ���0�ȉ��ɂȂ�Ȃ悤�ɒ���
		if (m_nTarget < 0)
		{
			m_nTarget = 0;
		}
	}

	//��]����
	Turn();

	//���Z�b�g����
	Reset();

	//�^�O�E��ޕύX
	Change();

	//�J�����̍��W�ݒ菈��
	SetCameraPos();
}

//===========================================================================================================
// �ړ�����
//===========================================================================================================
void CEdit3D::Move()
{
	//���݂̍��W�����擾
	D3DXVECTOR3 pos = GetPos();

	//Press
	if (!m_bTrigger)
	{
		//T�L�[�������ꂽ�ꍇ
		if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_T))
		{
			//���W���X�V
			pos.x -= MOVE;
		}
		//G�L�[�������ꂽ�ꍇ
		else if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_G))
		{
			//���W���X�V
			pos.x += MOVE;
		}
		//Y�L�[�������ꂽ�ꍇ
		else if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_Y))
		{
			//���W���X�V
			pos.y += MOVE;
		}
		//H�L�[�������ꂽ�ꍇ
		else if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_H))
		{
			//���W���X�V
			pos.y -= MOVE;
		}
		//U�L�[�������ꂽ�ꍇ
		else if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_U))
		{
			//���W���X�V
			pos.z += MOVE;
		}
		//J�L�[�������ꂽ�ꍇ
		else if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_J))
		{
			//���W���X�V
			pos.z -= MOVE;
		}
	}
	//Trigger
	else
	{
		//T�L�[�������ꂽ�ꍇ
		if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_T))
		{
			//���W���X�V
			pos.x -= MOVE;
		}
		//G�L�[�������ꂽ�ꍇ
		else if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_G))
		{
			//���W���X�V
			pos.x += MOVE;
		}
		//Y�L�[�������ꂽ�ꍇ
		else if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_Y))
		{
			//���W���X�V
			pos.y += MOVE;
		}
		//H�L�[�������ꂽ�ꍇ
		else if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_H))
		{
			//���W���X�V
			pos.y -= MOVE;
		}
		//U�L�[�������ꂽ�ꍇ
		else if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_U))
		{
			//���W���X�V
			pos.z += MOVE;
		}
		//J�L�[�������ꂽ�ꍇ
		else if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_J))
		{
			//���W���X�V
			pos.z -= MOVE;
		}
	}

	//�X�V���ꂽ���W����ݒ�
	SetPos(pos);
}

//===========================================================================================================
// �g�又��
//===========================================================================================================
void CEdit3D::Scale()
{
	//���݂̊g�嗦�����擾
	D3DXVECTOR3 Scale = GetScale();

	//Press
	if (!m_bTrigger)
	{
		//T�L�[�������ꂽ�ꍇ
		if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_T))
		{
			//�g�嗦���X�V
			Scale.x -= SCALE;
		}
		//G�L�[�������ꂽ�ꍇ
		else if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_G))
		{
			//�g�嗦���X�V
			Scale.x += SCALE;
		}
		//Y�L�[�������ꂽ�ꍇ
		else if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_Y))
		{
			//�g�嗦���X�V
			Scale.y += SCALE;
		}
		//H�L�[�������ꂽ�ꍇ
		else if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_H))
		{
			//�g�嗦���X�V
			Scale.y -= SCALE;
		}
		//U�L�[�������ꂽ�ꍇ
		else if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_U))
		{
			//�g�嗦���X�V
			Scale.z += SCALE;
		}
		//J�L�[�������ꂽ�ꍇ
		else if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_J))
		{
			//�g�嗦���X�V
			Scale.z -= SCALE;
		}
	}
	//Trigger
	else
	{
		//T�L�[�������ꂽ�ꍇ
		if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_T))
		{
			//�g�嗦���X�V
			Scale.x -= SCALE;
		}
		//G�L�[�������ꂽ�ꍇ
		else if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_G))
		{
			//�g�嗦���X�V
			Scale.x += SCALE;
		}
		//Y�L�[�������ꂽ�ꍇ
		else if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_Y))
		{
			//�g�嗦���X�V
			Scale.y += SCALE;
		}
		//H�L�[�������ꂽ�ꍇ
		else if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_H))
		{
			//�g�嗦���X�V
			Scale.y -= SCALE;
		}
		//U�L�[�������ꂽ�ꍇ
		else if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_U))
		{
			//�g�嗦���X�V
			Scale.z += SCALE;
		}
		//J�L�[�������ꂽ�ꍇ
		else if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_J))
		{
			//�g�嗦���X�V
			Scale.z -= SCALE;
		}
	}

	//�X�V���ꂽ�g�嗦����ݒ�
	SetScale(Scale);
}

//===========================================================================================================
// ��]����
//===========================================================================================================
void CEdit3D::Turn()
{
	//���݂̊p�x�����擾
	D3DXVECTOR3 rot = GetRot();

	//Z�L�[�������ꂽ�ꍇ
	if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_Z))
	{
		//�E��]
		rot.y -= TURN;
	}
	//X�L�[�������ꂽ�ꍇ
	else if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_X))
	{
		//����]
		rot.y += TURN;
	}

	//�p�x�␳
	CorrectionRot(rot.y);

	//�X�V���ꂽ�p�x����ݒ�
	SetRot(rot);
}

//===========================================================================================================
// ���Z�b�g����
//===========================================================================================================
void CEdit3D::Reset()
{
	//R�L�[�������ꂽ�ꍇ
	if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_R))
	{
		//�p�����[�^���Z�b�g
		SetPos({ 0.0f, 0.0f, 0.0f });//���W
		SetRot({ 0.0f, 0.0f, 0.0f });//�p�x
		SetScale({ 1.0f, 1.0f, 1.0f });//�g�嗦
	}
}

//===========================================================================================================
// �^�O�E�^�C�v�ύX����
//===========================================================================================================
void CEdit3D::Change()
{
	//���݂̃^�O����ۑ�
	int nTag = static_cast<int>(m_tag);

	//Q�L�[�������ꂽ�ꍇ
	if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_Q))
	{
		//��SHIFT��������Ă���ꍇ
		if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_LSHIFT) && 
			nTag > 1)
		{
			while (1)
			{
				//��ޕύX
				nTag--;

				//�L�����N�^�p�[�c�ƃe�N�X�`���݂̂̃^�O���΂�
				if (static_cast<CTag::TAG>(nTag) != CTag::TAG::BOSS &&
					static_cast<CTag::TAG>(nTag) != CTag::TAG::BG &&
					static_cast<CTag::TAG>(nTag) != CTag::TAG::EFFECT &&
					static_cast<CTag::TAG>(nTag) != CTag::TAG::UI)
				{
					break;
				}
			}

			//�^�C�v������������
			m_nType = 1;
		}
		//��SHIFT��������Ă��Ȃ��ꍇ
		else
		{
			if (m_nType > 1)
			{
				//�^�C�v�ύX
				m_nType--;
			}
		}
	}
	//E�L�[�������ꂽ�ꍇ
	else if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_E))
	{
		//��SHIFT��������Ă���ꍇ
		if (CManager::GetInstance()->GetKeyboard()->GetPress(DIK_LSHIFT) &&
			nTag < static_cast<int>(CTag::TAG::MAX) - 1)
		{
			while (1)
			{
				//��ޕύX
				nTag++;

				//�L�����N�^�p�[�c�ƃe�N�X�`���݂̂̃^�O���΂�
				if (static_cast<CTag::TAG>(nTag) != CTag::TAG::BOSS &&
					static_cast<CTag::TAG>(nTag) != CTag::TAG::BG &&
					static_cast<CTag::TAG>(nTag) != CTag::TAG::EFFECT &&
					static_cast<CTag::TAG>(nTag) != CTag::TAG::UI)
				{
					break;
				}
			}

			//�^�C�v������������
			m_nType = 1;
		}
		//��SHIFT��������Ă��Ȃ��ꍇ
		else
		{
			if (m_nType < CManager::GetInstance()->GetTag()->GetNumType(m_tag) - 1)
			{
				//�^�C�v�ύX
				m_nType++;
			}
		}
	}

	//�X�V���ꂽ�^�O����ݒ�
	m_tag = static_cast<CTag::TAG>(nTag);
}

//===========================================================================================================
// �`�揈��
//===========================================================================================================
void CEdit3D::Draw()
{
	//�G�f�B�^���[�h�ł͂Ȃ��ꍇ
	if (CManager::GetInstance()->GetScene()->GetSceneState() != CScene::SCENE::EDIT_3D)
	{//�`�悹���ɏ����𔲂���
		return;
	}

	//���[�J���ϐ��錾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//�f�o�C�X�̃|�C���^
	D3DXMATRIX mtxRot, mtxTrans, mtxScale;//�v�Z�p�}�g���b�N�X
	D3DMATERIAL9 matDef;//���݂̃}�e���A���ۑ��p
	D3DXMATERIAL* pMat;//�}�e���A���̃f�[�^�ւ̃|�C���^
	D3DXMATRIX mtxWorld;//�}�g���b�N�X���
	D3DXVECTOR3 Scale = GetScale();//�g�嗦���
	D3DXVECTOR3 rot = GetRot();//�p�x���
	D3DXVECTOR3 pos = GetPos();//���W���
	CXfile::ModelInfo aModel = GetModelInfo();//���f�����

	//���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&mtxWorld);

	//�g�嗦�𔽉f
	D3DXMatrixScaling(&mtxScale, Scale.x, Scale.y, Scale.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxScale);

	//�p�x�𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	//�ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

	//�@���̒�����1�ɂ���B
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	//���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &mtxWorld);

	//���݂̃}�e���A�����擾
	pDevice->GetMaterial(&matDef);

	//�}�e���A���f�[�^�ւ̃|�C���^���擾
	pMat = (D3DXMATERIAL*)aModel.pBuffMat->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < static_cast<int>(aModel.dwNumMat); nCntMat++)
	{
		//�ҏW�Ώۂ̃I�u�W�F�N�g�̏ꍇ
		if (m_nTarget == m_nIdx)
		{
			//���f���̐F��ω�
			pMat[nCntMat].MatD3D.Diffuse.a = 0.5f;
		}
		//�ҏW�Ώۂ̃I�u�W�F�N�g�ł͂Ȃ��ꍇ
		else
		{
			//���f���̐F�����ɖ߂�
			pMat[nCntMat].MatD3D.Diffuse =
			{
				aModel.FirstDiffuse.at(nCntMat).r,
				aModel.FirstDiffuse.at(nCntMat).g,
				aModel.FirstDiffuse.at(nCntMat).b,
				aModel.FirstDiffuse.at(nCntMat).a
			};
		}

		//�}�e���A���̐ݒ�
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		//�e�N�X�`���̐ݒ�
		pDevice->SetTexture(0, GetTexture());

		//���f��(�p�[�c)�̕`��
		aModel.pMesh->DrawSubset(nCntMat);
	}

	//�ۑ����Ă����}�e���A����߂�
	pDevice->SetMaterial(&matDef);
}

//===========================================================================================================
// ��������
//===========================================================================================================
CEdit3D* CEdit3D::Create(CEdit3D aEdit)
{
	//�������𓮓I�m��
	CEdit3D* pEdit = NEW CEdit3D(static_cast<int>(Category::EDIT));

	//�p�����[�^�ݒ�
	pEdit->SetPos(aEdit.GetPos());//���W
	pEdit->SetRot(aEdit.GetRot());//�p�x
	pEdit->SetScale(aEdit.GetScale());//�g�嗦
	pEdit->m_tag = aEdit.m_tag;//�^�O
	pEdit->m_nType = aEdit.m_nType;//�^�C�v

	//����������
	pEdit->Init();

	//�}�l�[�W���[�o�^����
	CManager::GetInstance()->GetScene()->GetEditManager()->Regist(pEdit);

	return pEdit;
}

//===========================================================================================================
// �Z�[�u����
//===========================================================================================================
void CEdit3D::Save()
{
	//�t�@�C�����J��
	FILE* pFile = fopen(CManager::GetInstance()->StageFile[static_cast<int>(CManager::GetInstance()->GetScene()->GetMode())], "wb");
	
	//�t�@�C�������������ꍇ
	if (pFile != nullptr)
	{
		//�z�u���������o��
		fwrite(&m_nAll, sizeof(int), 1, pFile);

		//�擪�̃G�f�B�^�I�u�W�F�N�g�����擾
		CObject* pObj = GetTop(static_cast<int>(Category::EDIT));
		CEdit3D* pEdit3D = dynamic_cast<CEdit3D*>(pObj);

		//�I�u�W�F�N�g���nullptr�ɂȂ�܂ŌJ��Ԃ�
		while (pEdit3D != nullptr)
		{
			//���̃I�u�W�F�N�g�����擾
			CEdit3D* pNext = pEdit3D->GetNextEdit();

			//����ۑ�
			D3DXVECTOR3 pos = pEdit3D->GetPos();//���W
			D3DXVECTOR3 rot = pEdit3D->GetRot();//�p�x
			D3DXVECTOR3 scale = pEdit3D->GetScale();//�g�嗦
			int nTag = static_cast<int>(pEdit3D->m_tag);//�^�O
			
			//�f�[�^�̋L�^
			fwrite(&pos, sizeof(D3DXVECTOR3), 1, pFile);//���W
			fwrite(&rot, sizeof(D3DXVECTOR3), 1, pFile);//�p�x
			fwrite(&scale, sizeof(D3DXVECTOR3), 1, pFile);//�g�嗦
			fwrite(&nTag, sizeof(int), 1, pFile);//�^�O
			fwrite(&pEdit3D->m_nType, sizeof(int), 1, pFile);//���

			//�I�u�W�F�N�g�������̏��ɕύX����
			pEdit3D = pNext;
		}

		//�t�@�C�������
		fclose(pFile);
	}
}

//===========================================================================================================
// �J�����̍��W�ݒ�
//===========================================================================================================
void CEdit3D::SetCameraPos()
{
	//�ʒu�����擾
	D3DXVECTOR3 pos = GetPos();//�I�u�W�F�N�g

	//�J�����̎��_�ƒ����_���I�u�W�F�N�g�ʒu�ɍ��킹�Đݒ�
	CCamera* pCamera = CManager::GetInstance()->GetCamera();
	pCamera->SetPosR(pos);//�����_
}

#endif // _DEBUG
