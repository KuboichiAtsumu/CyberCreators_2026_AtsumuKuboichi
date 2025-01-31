//==============================================================================================================================================
//
// ���[�V�����Ɋւ��鏈��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "motion.h"

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CMotionCharacter::CMotionCharacter(int nPriority) : CObjectX{ nPriority },
	m_CenterPos( 0.0f, 0.0f, 0.0f ),
	m_nState(0),
	m_fSpeed(1.0f),
	m_fJump(1.0f),
	m_bLanding(true),
	m_bMotion(true)
{
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CMotionCharacter::~CMotionCharacter()
{
}

//===========================================================================================================
// ����������
//===========================================================================================================
HRESULT CMotionCharacter::Init()
{
	//�p�[�c�t�@�C���Ǎ�����
	LoadParts();
	SetAllPartsSize();//�T�C�Y�ݒ�

	//���[�V�������Ǎ�����
	LoadMotion();

	//���N���X����������
	if (FAILED(CObjectX::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===========================================================================================================
// �������
//===========================================================================================================
void CMotionCharacter::Release()
{
	//�p�[�c�����N���A
	m_aParts.clear();

	//���[�V��������ۑ�
	int nMotion = m_aMotion.size();

	//���[�V��������j��
	for (int nCntMotion = 0; nCntMotion < nMotion; nCntMotion++)
	{
		//�L�[�Z�b�g����ۑ�
		int nKeySet = m_aMotion.back().aKeyset.size();

		//�L�[�Z�b�g����j��
		for (int nCntKeySet = 0; nCntKeySet < nKeySet; nCntKeySet++)
		{
			//�L�[�����N���A
			m_aMotion.back().aKeyset.back().aKey.clear();
		}

		//�L�[�Z�b�g�����N���A
		m_aMotion.back().aKeyset.clear();
	}

	//���[�V���������N���A
	m_aMotion.clear();

	//X�t�@�C���I�u�W�F�N�g�������
	CObjectX::Release();
}

//===========================================================================================================
// �X�V����
//===========================================================================================================
void CMotionCharacter::Update()
{
	//���݂̍��W���擾
	D3DXVECTOR3 pos = GetPos();

	//���S���W���X�V
	m_CenterPos = { pos.x, pos.y + GetSize().y * 0.5f, pos.z };

	//X�t�@�C���I�u�W�F�N�g�X�V����
	CObjectX::Update();

	//���W�̕␳
	CorrectionPos();
}

//===========================================================================================================
// ���W�̕␳����
//===========================================================================================================
void CMotionCharacter::CorrectionPos()
{
	//���W�����擾
	D3DXVECTOR3 pos = GetPos();

	//�J�����̌������擾
	float fCameraRotY = CManager::GetInstance()->GetCamera()->GetRot().y;

	//�J�����̌����ɍ��킹�č��W���C��
	switch (GetStandLine())
	{
		//XY��
	case CMotionCharacter::STAND_LINE::XY:
		if (fCameraRotY == 0.0f)
		{
			pos.z = CManager::VTXMIN_FIELD.z;
		}
		else if (fCameraRotY == D3DX_PI || fCameraRotY == -D3DX_PI)
		{
			pos.z = CManager::VTXMAX_FIELD.z;
		}
		break;

		//YZ��
	case CMotionCharacter::STAND_LINE::YZ:
		if (fCameraRotY == D3DX_PI * 0.5f)
		{
			pos.x = CManager::VTXMIN_FIELD.x;
		}
		else if (fCameraRotY == -D3DX_PI * 0.5f)
		{
			pos.x = CManager::VTXMAX_FIELD.x;
		}
		break;

	default:
		break;
	}

	//�t�B�[���h�O�ɏo�Ȃ��悤�ɒ���
	if (pos.x > CManager::VTXMAX_FIELD.x)
	{
		pos.x = CManager::VTXMAX_FIELD.x;
	}
	else if(pos.x < CManager::VTXMIN_FIELD.x)
	{
		pos.x = CManager::VTXMIN_FIELD.x;
	}
	if (pos.z > CManager::VTXMAX_FIELD.z)
	{
		pos.z = CManager::VTXMAX_FIELD.z;
	}
	else if (pos.z < CManager::VTXMIN_FIELD.z)
	{
		pos.z = CManager::VTXMIN_FIELD.z;
	}

	//�`���[�g���A���̏ꍇ
	if (CManager::GetInstance()->GetScene()->GetMode() == CScene::MODE::MODE_TUTORIAL)
	{
		//Z���W��0�ɌŒ�
		pos.z = 0.0f;
	}

	//�X�V���ꂽ���W��ݒ�
	SetPos(pos);
}

//===========================================================================================================
// ���[�V������Ԑݒ菈��
//===========================================================================================================
void CMotionCharacter::SetMotionState(int nState)
{
	//���݂ƈႤ��Ԃ̏ꍇ
	if (m_nState != nState)
	{
		//�J�E���g�����Z�b�g
		m_aMotion.at(nState).nCntKeySet = 0;//�L�[�Z�b�g
		m_aMotion.at(nState).nCntFrame = 0;//�t���[��

		//���[�V�����̔����true�ɂ���
		m_bMotion = true;

		//���݂̏�Ԃ�ۑ�
		m_nState = nState;
	}

	//���[�V�����̔��肪false�̏ꍇ
	if (!m_bMotion)
	{
		//�����𔲂���
		return;
	}

	//�t���[�����J�E���g
	m_aMotion.at(m_nState).nCntFrame++;

	//�J�E���g���w��̃t���[�����ɂȂ����ꍇ
	if (m_aMotion.at(m_nState).nCntFrame > m_aMotion.at(m_nState).aKeyset.at(m_aMotion.at(m_nState).nCntKeySet).nMaxFrame)
	{
		//�t���[�����J�E���g��������
		m_aMotion.at(m_nState).nCntFrame = 0;

		//�L�[�Z�b�g�������Z
		m_aMotion.at(m_nState).nCntKeySet++;

		//�L�[�����ő吔�ɂȂ����ꍇ
		if (m_aMotion.at(m_nState).nCntKeySet >= m_aMotion.at(m_nState).nMaxKeySet + 1 && 
			static_cast<bool>(m_aMotion.at(m_nState).nLoop))
		{//���[�v����ꍇ

			//�L�[�Z�b�g����������
			m_aMotion.at(m_nState).nCntKeySet = 0;
		}
		else if (m_aMotion.at(m_nState).nCntKeySet >= m_aMotion.at(m_nState).nMaxKeySet && 
			!static_cast<bool>(m_aMotion.at(m_nState).nLoop))
		{//���[�v���Ȃ��ꍇ

			//���[�V���������Ȃ��悤�ɐݒ�
			m_bMotion = false;

			m_aMotion.at(m_nState).nCntKeySet = m_aMotion.at(m_nState).nMaxKeySet;
		}
	}
}

//===========================================================================================================
// �p�[�c���Ƃ̏�Ԑݒ菈��
//===========================================================================================================
void CMotionCharacter::SetPartsState()
{
	//�p�[�c����ۑ�
	int nNumParts = m_aParts.size();

	for (int nCntParts = 0; nCntParts < nNumParts; nCntParts++)
	{
		//���[�V��������
		Motion(m_nState, nCntParts);
	}
}

//===========================================================================================================
// ���[�V��������
//===========================================================================================================
void CMotionCharacter::Motion(int nState, int nParts)
{
	//���[�J���ϐ��錾
	int nKeySet = m_aMotion.at(nState).nCntKeySet;//���݂̃L�[�Z�b�g�J�E���^
	int nNextKeySet = (nKeySet + 1) % (m_aMotion.at(nState).nMaxKeySet + 1);//���̃L�[
	float fFrame = static_cast<float>(m_aMotion.at(nState).nCntFrame) / static_cast<float>(m_aMotion.at(nState).aKeyset.at(nKeySet).nMaxFrame);//���݂̃t���[���ƑS�̃t���[���̊���

	//============================
	// ���̃L�[�܂ł̍��������߂�
	//============================

	//�p�x
	D3DXVECTOR3 Rot = m_aMotion.at(nState).aKeyset.at(nNextKeySet).aKey.at(nParts).rot - m_aParts.at(nParts).rot;

	//�p�x�␳
	CorrectionRot(Rot.x);//X��
	CorrectionRot(Rot.y);//Y��
	CorrectionRot(Rot.z);//Z��

	//============================
	// ���̃L�[�𔽉f
	//============================

	//�p�x���X�V
	m_aParts.at(nParts).rot += Rot * fFrame;

	//�p�x�␳
	CorrectionRot(m_aParts.at(nParts).rot.x);//X��
	CorrectionRot(m_aParts.at(nParts).rot.y);//Y��
	CorrectionRot(m_aParts.at(nParts).rot.z);//Z��
}

//===========================================================================================================
// �`�揈��
//===========================================================================================================
void CMotionCharacter::Draw()
{
	//���[�J���ϐ��錾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//�f�o�C�X�̃|�C���^
	D3DXMATRIX mtxRot, mtxTrans, mtxParent;//�v�Z�p�}�g���b�N�X
	D3DMATERIAL9 matDef;//���݂̃}�e���A���ۑ��p
	D3DXMATERIAL* pMat;//�}�e���A���̃f�[�^�ւ̃|�C���^
	D3DXMATRIX mtxWorld;//���[���h�}�g���b�N�X���
	D3DXVECTOR3 pos = GetPos();//���W
	D3DXVECTOR3 rot = GetRot();//�p�x

	//���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&mtxWorld);

	//�p�x�𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	//���W�𔽉f
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

	//�p�[�c�����擾
	int nNumParts = static_cast<int>(m_aParts.size());

	for (int nCntParts = 0; nCntParts < nNumParts; nCntParts++)
	{
		////�A�E�g���C���`�揈��
		//DrawOutLine(nCntParts, mtxWorld);

		//�e�p�[�c���f���̃��[���h�}�g���b�N�X�̏�����
		D3DXMatrixIdentity(&m_aParts.at(nCntParts).mtxWorld);

		//�e�p�[�c���f���̊p�x�𔽉f
		D3DXMatrixRotationYawPitchRoll(&mtxRot, m_aParts.at(nCntParts).rot.y, m_aParts.at(nCntParts).rot.x, m_aParts.at(nCntParts).rot.z);
		D3DXMatrixMultiply(&m_aParts.at(nCntParts).mtxWorld, &m_aParts.at(nCntParts).mtxWorld, &mtxRot);

		//�e�p�[�c���f���̍��W�𔽉f
		D3DXMatrixTranslation(&mtxTrans, m_aParts.at(nCntParts).pos.x, m_aParts.at(nCntParts).pos.y, m_aParts.at(nCntParts).pos.z);
		D3DXMatrixMultiply(&m_aParts.at(nCntParts).mtxWorld, &m_aParts.at(nCntParts).mtxWorld, &mtxTrans);

		//�e�̔ԍ���ۑ�
		int nParent = m_aParts.at(nCntParts).nParent;

		//�e�̃}�g���b�N�X�𔽉f
		if (nParent != -1)
		{//�e�̔ԍ������ꍇ
			mtxParent = m_aParts.at(nParent).mtxWorld;//�e���f���̃}�g���b�N�X
		}
		else
		{//�e�̔ԍ��������Ȃ��ꍇ
			mtxParent = mtxWorld;//�L�����N�^�[���g�̃}�g���b�N�X
		}

		//�e�̃}�g���b�N�X���|�����킹��
		D3DXMatrixMultiply(&m_aParts.at(nCntParts).mtxWorld, &m_aParts.at(nCntParts).mtxWorld, &mtxParent);

		//���[���h�}�g���b�N�X�̐ݒ�
		pDevice->SetTransform(D3DTS_WORLD, &m_aParts.at(nCntParts).mtxWorld);

		//���[���h���W��ۑ�
		D3DXVECTOR3 WorldPos = { m_aParts.at(nCntParts).mtxWorld._41, m_aParts.at(nCntParts).mtxWorld._42, m_aParts.at(nCntParts).mtxWorld._43 };
		SetWorldPos(nCntParts, WorldPos);

		//���݂̃}�e���A�����擾
		pDevice->GetMaterial(&matDef);

		//�}�e���A���f�[�^�ւ̃|�C���^���擾
		pMat = (D3DXMATERIAL*)m_aParts.at(nCntParts).m_aModelInfo.pBuffMat->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < static_cast<int>(m_aParts.at(nCntParts).m_aModelInfo.dwNumMat); nCntMat++)
		{
			//�}�e���A�����𔽉f
			pMat[nCntMat].MatD3D.Diffuse = m_aParts.at(nCntParts).m_aModelInfo.Diffuse.at(nCntMat);

			//�}�e���A���̐ݒ�
			pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

			//�e�N�X�`���̐ݒ�
			pDevice->SetTexture(0, nullptr);

			//���f��(�p�[�c)�̕`��
			m_aParts.at(nCntParts).m_aModelInfo.pMesh->DrawSubset(nCntMat);
		}

		//�ۑ����Ă����}�e���A����߂�
		pDevice->SetMaterial(&matDef);
	}
}

//===========================================================================================================
// �A�E�g���C���`�揈��
//===========================================================================================================
void CMotionCharacter::DrawOutLine(int nParts, D3DXMATRIX mtxWorld)
{
	//���[�J���ϐ��錾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//�f�o�C�X�̃|�C���^
	D3DXMATRIX mtxRot, mtxTrans, mtxParent;//�v�Z�p�}�g���b�N�X
	
	//�e�p�[�c���f���̃��[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_aParts.at(nParts).mtxWorld);

	//�e�p�[�c���f���̊g�嗦�𔽉f
	D3DXMatrixScaling(&m_aParts.at(nParts).mtxWorld, OUTLINE_SCALE, OUTLINE_SCALE, OUTLINE_SCALE);//���̃p�[�c���傫���ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &m_aParts.at(nParts).mtxWorld);

	//�e�p�[�c���f���̊p�x�𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_aParts.at(nParts).rot.y, m_aParts.at(nParts).rot.x, m_aParts.at(nParts).rot.z);
	D3DXMatrixMultiply(&m_aParts.at(nParts).mtxWorld, &m_aParts.at(nParts).mtxWorld, &mtxRot);

	//�e�p�[�c���f���̍��W�𔽉f
	D3DXMatrixTranslation(&mtxTrans, m_aParts.at(nParts).pos.x, m_aParts.at(nParts).pos.y, m_aParts.at(nParts).pos.z);
	D3DXMatrixMultiply(&m_aParts.at(nParts).mtxWorld, &m_aParts.at(nParts).mtxWorld, &mtxTrans);

	//�e�̔ԍ���ۑ�
	int nParent = m_aParts.at(nParts).nParent;

	//�e�̃}�g���b�N�X�𔽉f
	if (nParent != -1)
	{//�e�̔ԍ������ꍇ
		mtxParent = m_aParts.at(nParent).mtxWorld;//�e���f���̃}�g���b�N�X
	}
	else
	{//�e�̔ԍ��������Ȃ��ꍇ
		mtxParent = mtxWorld;//�L�����N�^�[���g�̃}�g���b�N�X
	}

	//�e�̃}�g���b�N�X���|�����킹��
	D3DXMatrixMultiply(&m_aParts.at(nParts).mtxWorld, &m_aParts.at(nParts).mtxWorld, &mtxParent);

	//���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &m_aParts.at(nParts).mtxWorld);

	//�A�E�g���C���`��ݒ�
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);//���ʂ����`��
	pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);//Z�o�b�t�@�L����

	//�A�E�g���C���F�̐ݒ�
	D3DMATERIAL9 OutlineMaterial = {};
	OutlineMaterial.Diffuse.r = 1.0f;
	OutlineMaterial.Diffuse.g = 1.0f;
	OutlineMaterial.Diffuse.b = 1.0f;
	OutlineMaterial.Diffuse.a = 1.0f;
	pDevice->SetMaterial(&OutlineMaterial);

	//�A�E�g���C���`��
	for (int nCnt = 0; nCnt < static_cast<int>(m_aParts.at(nParts).m_aModelInfo.dwNumMat); nCnt++)
	{
		m_aParts.at(nParts).m_aModelInfo.pMesh->DrawSubset(nCnt);
	}

	//�`���Ԃ����ɖ߂�
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//===========================================================================================================
// �p�[�c�t�@�C���Ǎ�����
//===========================================================================================================
void CMotionCharacter::LoadParts()
{
	//���[�J���ϐ��錾
	std::string str;//�t�@�C�������p
	std::stringstream ss;//������ۑ��p
	ModelParts aParts;//�p�[�c���ۑ��p

	//�t�@�C�����J��
	std::ifstream file(PARTS_FILE[static_cast<int>(m_Type)]);

	//�t�@�C�����J���Ȃ��ꍇ
	if (!file)
	{
		assert(false);
		return;
	}

	//END��������܂œǂݍ��݂��J��Ԃ�
	while (1)
	{
		//����
		std::getline(file, str);

		//END_PARTS�����������ꍇ
		if (str.compare("END_PARTS") == 0)
		{
			//�t�@�C�������
			file.close();

			break;
		}
		//MOVE�����������ꍇ
		else if (str.find("MOVE") != std::string::npos)
		{
			//�ǂݍ��񂾕������ۑ�
			ss = static_cast<std::stringstream>(str);

			//�ړ��ʂ�ǂݍ���
			ss >> str >> m_fSpeed;
		}
		//JUMP�����������ꍇ
		else if (str.find("JUMP") != std::string::npos)
		{
			//�ǂݍ��񂾕������ۑ�
			ss = static_cast<std::stringstream>(str);

			//�ړ��ʂ�ǂݍ���
			ss >> str >> m_fJump;
		}
		//PARTSSET�����������ꍇ
		else if (str.find("PARTSSET") != std::string::npos)
		{
			//�e�p�[�c�̃f�[�^������
			while (1)
			{
				//����
				std::getline(file, str);

				//END_PARTSSET�����������ꍇ
				if (str.find("END_PARTSSET") != std::string::npos)
				{
					//�p�[�c����ǉ�
					m_aParts.push_back(aParts);

					//�p�[�c�̃f�[�^�������I��
					break;
				}
				//INDEX�����������ꍇ
				else if (str.find("INDEX") != std::string::npos)
				{
					//�ǂݍ��񂾕������ۑ�
					ss = static_cast<std::stringstream>(str);

					//�C���f�b�N�X��ǂݍ���
					ss >> str >> aParts.nIndex;
				}
				//PATH�����������ꍇ
				else if (str.find("PATH") != std::string::npos)
				{
					//�ǂݍ��񂾕������ۑ�
					ss = static_cast<std::stringstream>(str);

					//�p�[�c�̃t�@�C�����𔽉f
					ss >> str >> aParts.aPartsFile;
				}
				//PARENT�����������ꍇ
				else if (str.find("PARENT") != std::string::npos)
				{
					//�ǂݍ��񂾕������ۑ�
					ss = static_cast<std::stringstream>(str);

					//�e�̃C���f�b�N�X��ǂݍ���
					ss >> str >> aParts.nParent;
				}
				//HALF�����������ꍇ
				else if (str.find("HALF") != std::string::npos)
				{
					//�ǂݍ��񂾕������ۑ�
					ss = static_cast<std::stringstream>(str);

					//�㔼�g�������g���̏���ǂݍ���
					int nHalf;
					ss >> str >> nHalf;

					//�ǂݍ��񂾐����𔽉f
					aParts.Half = static_cast<HALF_BODY>(nHalf);
				}
				//POS�����������ꍇ
				else if (str.find("POS") != std::string::npos)
				{
					//�ǂݍ��񂾕������ۑ�
					ss = static_cast<std::stringstream>(str);

					//���W��ǂݍ���
					ss >> str >> aParts.pos.x >> aParts.pos.y >> aParts.pos.z;

					//�����ʒu�ۑ�
					aParts.FirstPos = aParts.pos;
				}
				//ROT�����������ꍇ
				else if (str.find("ROT") != std::string::npos)
				{
					//�ǂݍ��񂾕������ۑ�
					ss = static_cast<std::stringstream>(str);

					//�p�x��ǂݍ���
					ss >> str >> aParts.rot.x >> aParts.rot.y >> aParts.rot.z;
				}
				//��L�ȊO�����������ꍇ
				else
				{
					continue;
				}
			}
		}
		else
		{
			continue;
		}
	}

	//�^�C�v���ƂɃ^�O��ݒ�
	CTag::TAG tag = {};
	switch (m_Type)
	{
		//�v���C���[
	case CMotionCharacter::TYPE::PLAYER:
		tag = CTag::TAG::PLAYER;
		break;

		//�{�X
	case CMotionCharacter::TYPE::BOSS:
		tag = CTag::TAG::BOSS;
		break;

	default:
		break;
	}

	//X�t�@�C���Ǘ��̃C���X�^���X���擾
	CXfile* pModel = CXfile::GetInstance();

	//�p�[�c����ۑ�
	int nNumParts = m_aParts.size();

	for (int nCnt = 0; nCnt < nNumParts; nCnt++)
	{
		//���f�����Ǘ�
		int nModelIdx = pModel->Regist(tag, m_aParts.at(nCnt).nIndex, m_aParts.at(nCnt).aPartsFile);//�o�^
		BindModel(pModel->GetAddress(nModelIdx));//�ݒ�
	
		//�p�[�c���f�����ݒ菈��
		SetPartsInfo(GetModelInfo(), nCnt);
	}
}

//===========================================================================================================
// ���[�V�������Ǎ�����
//===========================================================================================================
void CMotionCharacter::LoadMotion()
{
	//���[�J���ϐ��錾
	std::string str;//�t�@�C�������p
	std::stringstream ss;//������ۑ��p
	MotionSet aMotion;//���[�V�������
	KeySet aKeySet;//�L�[�Z�b�g���
	Key aKey;//�L�[���
	int nMotion = -1;//���[�V������

	//�t�@�C�����J��
	std::ifstream file(PARTS_FILE[static_cast<int>(m_Type)]);

	//�t�@�C�����J���Ȃ��ꍇ
	if (!file)
	{
		assert(false);
		return;
	}

	//EOF��������܂Ō������J��Ԃ�
	while (1)
	{
		//����
		std::getline(file, str);

		//EOF�����������ꍇ
		if (str.find("END_MOTION") != std::string::npos)
		{
			//�t�@�C�������
			file.close();
			break;
		}
		//MOTIONSET�����������ꍇ
		else if (str.find("MOTIONSET") != std::string::npos)
		{
			m_aMotion.push_back({});

			//���[�V�����������Z
			nMotion++;

			//���[�V��������������
			m_aMotion.at(nMotion).nCntFrame = 0;
			m_aMotion.at(nMotion).nCntKeySet = 0;
			m_aMotion.at(nMotion).nMaxKeySet = -1;

			//�e���[�V�����̃f�[�^������
			while (1)
			{
				//����
				std::getline(file, str);

				//END_MOTIONSET�����������ꍇ
				if (str.find("END_MOTIONSET") != std::string::npos)
				{
					//���[�V�����ǂݍ��ݏI��
					break;
				}

				//LOOP�����������ꍇ
				if (str.find("LOOP") != std::string::npos)
				{
					//�ǂݍ��񂾕������ۑ�
					ss = static_cast<std::stringstream>(str);

					//�p�x��ǂݍ���
					ss >> str >> m_aMotion.at(nMotion).nLoop;
				}

				//KEYSET�����������ꍇ
				if (str.find("KEYSET") != std::string::npos)
				{
					m_aMotion.at(nMotion).aKeyset.push_back({});

					//���v�L�[�������Z
					m_aMotion.at(nMotion).nMaxKeySet++;

					//�e�L�[�Z�b�g�̃f�[�^������
					while (1)
					{
						//����
						std::getline(file, str);

						//END_KEYSET�����������ꍇ
						if (str.find("END_KEYSET") != std::string::npos)
						{
							//�e�L�[�Z�b�g�̃f�[�^�̌������I������
							break;
						}
						//FRAME�����������ꍇ
						else if (str.find("FRAME") != std::string::npos)
						{
							//�ǂݍ��񂾕������ۑ�
							ss = static_cast<std::stringstream>(str);

							//�p�x��ǂݍ���
							ss >> str >> m_aMotion.at(nMotion).aKeyset.at(m_aMotion.at(nMotion).nMaxKeySet).nMaxFrame;
						}
						//KEY�����������ꍇ
						else if (str.find("KEY") != std::string::npos)
						{
							//�e�L�[�̃f�[�^������
							while (1)
							{
								//����
								std::getline(file, str);

								//END_KEY�Z�b�g�����������ꍇ
								if (str.find("END_KEY") != std::string::npos)
								{
									//�L�[����o�^
									m_aMotion.at(nMotion).aKeyset.at(m_aMotion.at(nMotion).nMaxKeySet).aKey.push_back(aKey);

									//���[�V�����f�[�^�̓ǂݍ��݂��I��
									break;
								}
								//POS�����������ꍇ
								else if (str.find("POS") != std::string::npos)
								{
									//�ǂݍ��񂾕������ۑ�
									ss = static_cast<std::stringstream>(str);

									//���W��ǂݍ���
									ss >> str >> aKey.pos.x >> aKey.pos.y >> aKey.pos.z;
								}
								//ROT�����������ꍇ
								else if (str.find("ROT") != std::string::npos)
								{
									//�ǂݍ��񂾕������ۑ�
									ss = static_cast<std::stringstream>(str);

									//���W��ǂݍ���
									ss >> str >> aKey.rot.x >> aKey.rot.y >> aKey.rot.z;
								}
							}
						}
						//��L�ȊO�����������ꍇ
						else
						{
							//�X�L�b�v���ēǂݍ��݂𑱂���
							continue;
						}
					}
				}
				//��L�ȊO�����������ꍇ
				else
				{
					//�X�L�b�v���ēǂݍ��݂𑱂���
					continue;
				}
			}
		}
		//��L�ȊO�����������ꍇ
		else
		{
			//�����𑱂���
			continue;
		}
	}
}

//===========================================================================================================
// �p�[�c���f�����ݒ菈��
//===========================================================================================================
void CMotionCharacter::SetPartsInfo(CXfile::ModelInfo aModelInfo, int nParts)
{
	//�}�e���A����
	m_aParts.at(nParts).m_aModelInfo.dwNumMat = aModelInfo.dwNumMat;

	//�}�e���A���ւ̃|�C���^
	m_aParts.at(nParts).m_aModelInfo.pBuffMat = aModelInfo.pBuffMat;

	//���b�V���̒��_���ւ̃|�C���^
	m_aParts.at(nParts).m_aModelInfo.pMesh = aModelInfo.pMesh;

	//�F
	for (int nCntMat = 0; nCntMat < static_cast<int>(m_aParts.at(nParts).m_aModelInfo.dwNumMat); nCntMat++)
	{
		m_aParts.at(nParts).m_aModelInfo.Diffuse.push_back(aModelInfo.Diffuse.at(nCntMat));
		m_aParts.at(nParts).m_aModelInfo.FirstDiffuse.push_back(m_aParts.at(nParts).m_aModelInfo.Diffuse.at(nCntMat));
	}
}

//===========================================================================================================
// �p�[�c�̃J���[�ݒ菈��
//===========================================================================================================
void CMotionCharacter::SetPartsFlash(float a)
{
	//�p�[�c�����擾
	int nNumParts = m_aParts.size();

	//�p�[�c�����ݒ�
	for (int nCntParts = 0; nCntParts < nNumParts; nCntParts++)
	{
		//�e�p�[�c�Ɏw�肳�ꂽ�J���[����ݒ�
		for (int nCntMat = 0; nCntMat < (int)m_aParts[nCntParts].m_aModelInfo.dwNumMat; nCntMat++)
		{
			m_aParts[nCntParts].m_aModelInfo.Diffuse[nCntMat].a = a;
		}
	}
}

//===========================================================================================================
// //�S�p�[�c�����킹���T�C�Y�ݒ菈��
//===========================================================================================================
void CMotionCharacter::SetAllPartsSize()
{
	//���[�J���ϐ��錾
	BYTE* pVtxBuff;//���_�o�b�t�@�ւ̃|�C���^
	D3DXVECTOR3 size(0.0f, 0.0f, 0.0f);//���߂�ꂽ�T�C�Y
	D3DXVECTOR3 vtxMin(0.0f, 0.0f, 0.0f);//�e���W�̍ŏ��̒l
	D3DXVECTOR3 vtxMax(0.0f, 0.0f, 0.0f);//�e���W�̍ő�̒l
	int nNumParts = m_aParts.size();//�p�[�c��

	for (int nCnt = 0; nCnt < nNumParts; nCnt++)
	{
		//���_���̎擾
		int nNumVtx = m_aParts.at(nCnt).m_aModelInfo.pMesh->GetNumVertices();

		//���_�t�H�[�}�b�g�̃T�C�Y���擾
		DWORD sizeFVF = D3DXGetFVFVertexSize(m_aParts.at(nCnt).m_aModelInfo.pMesh->GetFVF());

		//���_�o�b�t�@�̃��b�N
		m_aParts.at(nCnt).m_aModelInfo.pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

		for (int nIdxVtx = 0; nIdxVtx < nNumVtx; nIdxVtx++)
		{
			//���_���W�̑��
			D3DXVECTOR3 vtx = *(D3DXVECTOR3*)pVtxBuff;
			vtx.y += m_aParts.at(nCnt).pos.y;

			//���f���̍ŏ��l�ƍő�l���擾
			if (vtx.x < vtxMin.x)
			{//X���W�̍ŏ��l
				vtxMin.x = vtx.x;
			}
			if (vtx.y < vtxMin.y)
			{//Y���W�̍ŏ��l
				vtxMin.y = vtx.y;
			}
			if (vtx.z < vtxMin.z)
			{//Z���W�̍ŏ��l
				vtxMin.z = vtx.z;
			}
			if (vtx.x > vtxMax.x)
			{//X���W�̍ő�l
				vtxMax.x = vtx.x;
			}
			if (vtx.y > vtxMax.y)
			{//Y���W�̍ő�l
				vtxMax.y = vtx.y;
			}
			if (vtx.z > vtxMax.z)
			{//Z���W�̍ő�l
				vtxMax.z = vtx.z;
			}

			//���_�t�H�[�}�b�g�̃T�C�Y���|�C���^��i�߂�
			pVtxBuff += sizeFVF;
		}

		//���_�o�b�t�@�̃A�����b�N
		m_aParts.at(nCnt).m_aModelInfo.pMesh->UnlockVertexBuffer();
	}

	//���_���ݒ�
	SetVtxMax(vtxMax);//�ő�
	SetVtxMin(vtxMin);//�ŏ�

	//�T�C�Y�����߂�
	size =
	{
		abs(vtxMax.x - vtxMin.x),//x��
		abs(vtxMax.y - vtxMin.y),//y��
		abs(vtxMax.z - vtxMin.z)//z��
	};

	//�Ίp���̒��������߂�
	SetLength(sqrtf(powf(size.x, 2.0f) + powf(size.z, 2.0f)) * 0.5f);

	//�Ίp���̊p�x�����߂�
	SetAngle(atan2f(size.x, size.z));

	//�T�C�Y����ݒ�
	SetUpdateSize(size);
}