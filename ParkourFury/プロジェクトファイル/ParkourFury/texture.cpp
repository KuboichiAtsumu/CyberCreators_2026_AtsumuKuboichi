//==============================================================================================================================================
//
// �e�N�X�`���Ɋւ���Ǘ�
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "texture.h"
#include "manager.h"

//===========================================================================================================
// �ÓI�����o�ϐ�������
//===========================================================================================================
CTexture* CTexture::m_pTexture = nullptr;

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CTexture::CTexture()
{
	//�Ǎ�����
	Load();
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CTexture::~CTexture()
{
}

//===========================================================================================================
// �C���X�^���X�擾
//===========================================================================================================
CTexture* CTexture::GetInstance()
{
	//�C���X�^���X�����݂��Ȃ��ꍇ
	if (m_pTexture == nullptr)
	{
		//�C���X�^���X�𐶐�
		m_pTexture = NEW CTexture();
	}

	return m_pTexture;
}

//===========================================================================================================
// �C���X�^���X�j��
//===========================================================================================================
void CTexture::Release()
{
	//�C���X�^���X�����݂���ꍇ
	if (m_pTexture != nullptr)
	{
		//�C���X�^���X��j��
		m_pTexture->Unload();
		delete m_pTexture;
		m_pTexture = nullptr;
	}
}

//===========================================================================================================
// �ǂݍ��ݏ���
//===========================================================================================================
void CTexture::Load()
{
	//���[�J���ϐ��錾
	CTag* pTag = CManager::GetInstance()->GetTag();//�^�O�̃C���X�^���X
	std::string str;//�t�@�C�������p
	Texture aTexture = {};//���f���o�^���ۑ��p

	//�t�@�C�����J��
	std::ifstream file("data/TEXTURE/TextureFile.txt");

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
			pTag->SetTag(aTexture.tag, str);
		}

		//���ڂ��Ƃ̃f�[�^����
		while (1)
		{
			//�J�e�S���[����
			std::getline(file, str);

			//FILE�����������ꍇ
			if (str.compare("FILE") == 0)
			{
				std::stringstream ss;

				//�^�C�v��ǂݍ���
				std::getline(file, str);
				ss = static_cast<std::stringstream>(str);
				ss >> aTexture.nType;

				//�t�@�C���p�X��ǂݍ���
				std::getline(file, aTexture.FileName);

				//�o�^����
				Regist(aTexture.tag, aTexture.nType, aTexture.FileName);
			}
			//END_FILE�����������ꍇ
			else if (str.compare("END_FILE") == 0)
			{
				//�Y���^�O�̃^�C�v����ݒ�
				if (pTag->GetNumType(aTexture.tag) < aTexture.nType + 1)
				{
					pTag->SetNumType(aTexture.nType + 1, aTexture.tag);
				}
	
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
void CTexture::Unload()
{
	//�e�N�X�`����񐔂�ۑ�
	int nSize = m_TextureInfo.size();

	//�e�N�X�`���ۑ����j��
	for (int nCnt = 0; nCnt < nSize; nCnt++)
	{
		//�e�N�X�`����񂪑��݂���ꍇ
		if (m_TextureInfo.back().pTexture != nullptr)
		{
			m_TextureInfo.back().pTexture->Release();
			m_TextureInfo.back().pTexture = nullptr;
		}

		//�����̗v�f���폜
		m_TextureInfo.pop_back();
	}

	m_TextureInfo.clear();
}

//===========================================================================================================
// �e�N�X�`���o�^
//===========================================================================================================
int CTexture::Regist(CTag::TAG tag, int nType, std::string pTextureName)
{
	//���[�J���ϐ��錾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();//�f�o�C�X�̎擾
	int nIdx = 0;//�e�N�X�`��ID
	bool bAlreadyRegist = false;//�����̏��̓o�^�ϔ���

	for (int nCnt = 0; nCnt < static_cast<int>(m_TextureInfo.size()); nCnt++)
	{
		//ID�ݒ�
		nIdx = nCnt;

		//�e�N�X�`����񂪑��݂���ꍇ
		if (pTextureName.compare(m_TextureInfo.at(nCnt).FileName) == 0)
		{
			//�o�^�ϔ����true�ɂ���
			bAlreadyRegist = true;
			break;
		}
	}

	//�o�^����Ă��Ȃ������ꍇ
	if (!bAlreadyRegist)
	{
		//���[�J���ϐ��錾
		Texture aTexture = {nullptr};//�e�N�X�`�����ۑ��p

		//����ۑ�
		aTexture.nIdx = nIdx;//ID
		aTexture.tag = tag;//�^�O
		aTexture.nType = nType;//�^�C�v
		aTexture.FileName = pTextureName;//�t�@�C���p�X

		//�e�N�X�`�����Ǎ�
		D3DXCreateTextureFromFile(pDevice, aTexture.FileName.c_str(), &aTexture.pTexture);

		//�ǂݍ��񂾃e�N�X�`�����𖖔��ɒǉ�
		m_TextureInfo.push_back(aTexture);
	}
	
	//ID��Ԃ�
	return nIdx;
}

//===========================================================================================================
// ���擾���� ( �J�e�S���[�E�^�C�v���� )
//===========================================================================================================
LPDIRECT3DTEXTURE9 CTexture::GetAddress(CTag::TAG tag, int nType)
{
	//�e�N�X�`���̃f�[�^�T�C�Y���擾
	int nSize = static_cast<int>(m_TextureInfo.size());

	for (int nCnt = 0; nCnt < nSize; nCnt++)
	{
		//�J�e�S���[�ƃ��f������v�����ꍇ
		if (m_TextureInfo.at(nCnt).tag == tag && m_TextureInfo.at(nCnt).nType == nType)
		{
			//���f������Ԃ�
			return m_TextureInfo.at(nCnt).pTexture;
		}
	}

	return nullptr;
}
