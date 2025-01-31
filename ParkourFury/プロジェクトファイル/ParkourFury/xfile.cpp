//==============================================================================================================================================
//
// X�t�@�C���ꊇ�Ǘ��Ɋւ���Ǘ�
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "xfile.h"
#include "manager.h"

//===========================================================================================================
// �ÓI�����o�ϐ�������
//===========================================================================================================
CXfile* CXfile::m_pXfile = nullptr;

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CXfile::CXfile()
{
	//�����o�ϐ�������
	m_ModelInfo.clear();//���f�����

	//�Ǎ���������
	Load();
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CXfile::~CXfile()
{
}

//===========================================================================================================
// �C���X�^���X�擾
//===========================================================================================================
CXfile* CXfile::GetInstance()
{
	//�C���X�^���X�����݂��Ȃ��ꍇ
	if (m_pXfile == nullptr)
	{
		//�C���X�^���X�𐶐�
		m_pXfile = NEW CXfile();
	}

	return m_pXfile;
}

//===========================================================================================================
// �C���X�^���X�j��
//===========================================================================================================
void CXfile::Release()
{
	//�C���X�^���X�����݂���ꍇ
	if (m_pXfile != nullptr)
	{
		m_pXfile->Unload();
		delete m_pXfile;
		m_pXfile = nullptr;
	}
}

//===========================================================================================================
// �ǂݍ��ݏ���
//===========================================================================================================
void CXfile::Load()
{
	//���[�J���ϐ��錾
	CTag* pTag = CManager::GetInstance()->GetTag();//�^�O�̃C���X�^���X
	std::string str;//�t�@�C�������p
	Model aModel = {};//���f���o�^���ۑ��p

	//�t�@�C�����J��
	std::ifstream file("data/MODEL/ModelFile.txt");

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

		//END�����������ꍇ
		if (str.compare("END") == 0)
		{
			break;
		}
		//�R�����g�A�E�g�����������ꍇ
		else if (!str.empty() && str.front() == '#')
		{
			//�ǂݍ��݂𑱂���
			continue;
		}
		//�󔒂̏ꍇ
		else if (str.empty())
		{
			//�ǂݍ��݂𑱂���
			continue;
		}
		else
		{
			//�^�O��ݒ�
			pTag->SetTag(aModel.tag, str);
		}

		//���ڂ��Ƃ̃f�[�^����
		while (1)
		{
			//�^�O����
			std::getline(file, str);

			//FILE�����������ꍇ
			if (str.compare("FILE") == 0)
			{
				std::stringstream ss;

				//�^�C�v��ǂݍ���
				std::getline(file, str);
				ss = static_cast<std::stringstream>(str);
				ss >> aModel.nType;

				//�t�@�C���p�X��ǂݍ���
				std::getline(file, aModel.FileName);

				//�o�^����
				Regist(aModel.tag, aModel.nType, aModel.FileName);
			}
			//END_FILE�����������ꍇ
			else if (str.compare("END_FILE") == 0)
			{
				//�Y���^�O�̃^�C�v����ݒ�
				if (pTag->GetNumType(aModel.tag) < aModel.nType + 1)
				{
					pTag->SetNumType(aModel.nType + 1, aModel.tag);
				}

				//�t�@�C���p�X�̓ǂݍ��݂��I��
				break;
			}
		}
	}

	//�t�@�C�������
	file.close();
}

//===========================================================================================================
// �폜����
//===========================================================================================================
void CXfile::Unload()
{
	//���f����񐔂�ۑ�
	int nSize = m_ModelInfo.size();

	//���f���ۑ����j��
	for (int nCnt = 0; nCnt < nSize; nCnt++)
	{
		//���b�V����񂪑��݂���ꍇ
		if (m_ModelInfo.back().aModel->pMesh != nullptr)
		{
			//���b�V���������
			m_ModelInfo.back().aModel->pMesh->Release();
			m_ModelInfo.back().aModel->pMesh = nullptr;
		}

		//�}�e���A����񂪑��݂���ꍇ
		if (m_ModelInfo.back().aModel->pBuffMat != nullptr)
		{
			//�}�e���A���������
			m_ModelInfo.back().aModel->pBuffMat->Release();
			m_ModelInfo.back().aModel->pBuffMat = nullptr;
		}

		//�J���[���𖖔�����j��
		for (int nCntMat = 0; nCntMat < static_cast<int>(m_ModelInfo.back().aModel->dwNumMat); nCntMat++)
		{
			m_ModelInfo.back().aModel->Diffuse.pop_back();
			m_ModelInfo.back().aModel->FirstDiffuse.pop_back();
		}

		//�J���[�����N���A
		m_ModelInfo.back().aModel->Diffuse.clear();
		m_ModelInfo.back().aModel->FirstDiffuse.clear();

		//�}�e���A���������Z�b�g
		m_ModelInfo.back().aModel->dwNumMat = static_cast<DWORD>(0);

		delete m_ModelInfo.back().aModel;
		m_ModelInfo.back().aModel = nullptr;

		//�����̗v�f���폜
		m_ModelInfo.pop_back();
	}

	//���f�������N���A
	m_ModelInfo.clear();
}

//===========================================================================================================
// �o�^����
//===========================================================================================================
int CXfile::Regist(CTag::TAG tag, int nType, std::string pModelName)
{
	//���[�J���ϐ��錾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//�f�o�C�X�̎擾
	int nIdx = 0;//���f���C���f�b�N�X
	int nNumParts = m_ModelInfo.size();//�p�[�c���擾
	bool bAlreadyRegist = false;//�����̏��̓o�^�ϔ���

	for (int nCnt = 0; nCnt < nNumParts; nCnt++)
	{
		//���f����񂪑��݂���ꍇ
		if (pModelName.compare(m_ModelInfo.at(nCnt).FileName) == 0)
		{
			//�o�^�ϔ����true�ɂ���
			bAlreadyRegist = true;
			break;
		}

		//�C���f�b�N�X�ݒ�
		nIdx++;
	}

	//�o�^����Ă��Ȃ������ꍇ
	if (!bAlreadyRegist)
	{
		//���[�J���ϐ��錾
		Model modelinfo = {};//���f�����ۑ��p

		//����ۑ�
		modelinfo.aModel = NEW ModelInfo();
		modelinfo.nIdx = nIdx;//�C���f�b�N�X
		modelinfo.FileName = pModelName;//�t�@�C���p�X
		modelinfo.tag = tag;//�^�O
		modelinfo.nType = nType;//�^�C�v

		//X�t�@�C���̓ǂݍ���
		D3DXLoadMeshFromX(modelinfo.FileName.c_str(), D3DXMESH_SYSTEMMEM, pDevice, nullptr, &modelinfo.aModel->pBuffMat, nullptr, &modelinfo.aModel->dwNumMat, &modelinfo.aModel->pMesh);

		//�}�e���A����񂪑��݂���ꍇ
		if (modelinfo.aModel->pBuffMat != nullptr)
		{
			//�}�e���A���f�[�^�ւ̃|�C���^���擾
			D3DXMATERIAL* pMat = (D3DXMATERIAL*)modelinfo.aModel->pBuffMat->GetBufferPointer();

			//�F��������ۑ�
			for (int nCntMat = 0; nCntMat < (int)modelinfo.aModel->dwNumMat; nCntMat++)
			{
				modelinfo.aModel->Diffuse.push_back(pMat[nCntMat].MatD3D.Diffuse);
				modelinfo.aModel->FirstDiffuse.push_back(pMat[nCntMat].MatD3D.Diffuse);
			}
		}

		//�ǂݍ��񂾃��f�����𖖔��ɒǉ�
		m_ModelInfo.push_back(modelinfo);
	}

	//�C���f�b�N�X��Ԃ�
	return nIdx;
}

//===========================================================================================================
// ���擾���� ( �^�O�E�^�C�v���� )
//===========================================================================================================
CXfile::ModelInfo* CXfile::GetAddress(CTag::TAG tag, int nType)
{
	for (int nCnt = 0; nCnt < static_cast<int>(m_ModelInfo.size()); nCnt++)
	{
		//�^�O�ƃ��f������v�����ꍇ
		if (m_ModelInfo.at(nCnt).tag == tag
			&& m_ModelInfo.at(nCnt).nType == nType)
		{
			//���f������Ԃ�
			return m_ModelInfo.at(nCnt).aModel;
		}
	}

	return nullptr;
}

//===========================================================================================================
// ���f���o�^���擾���� ( �^�O�E�^�C�v���� )
//===========================================================================================================
CXfile::Model CXfile::GetModel(CTag::TAG tag, int nType)
{
	for (int nCnt = 0; nCnt < static_cast<int>(m_ModelInfo.size()); nCnt++)
	{
		//�^�O�ƃ��f������v�����ꍇ
		if (m_ModelInfo.at(nCnt).tag == tag
			&& m_ModelInfo.at(nCnt).nType == nType)
		{
			//���f������Ԃ�
			return m_ModelInfo.at(nCnt);
		}
	}

	return { nullptr };
}
