//==============================================================================================================================================
//
// x�t�@�C���Ɋւ���Ǘ��̃w�b�_�[�t�@�C��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _XFILE_H_ //���̃}�N����`������Ȃ�������
#define _XFILE_H_ //2�d�C���N���[�h�h�~�̃}�N����`

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "tag.h"

//===========================================================================================================
// �N���X��`
//===========================================================================================================
class CXfile
{
private:
	CXfile();//�R���X�g���N�^
	~CXfile();//�f�X�g���N�^

public:
	//���f�����
	struct ModelInfo
	{
		//�ϐ�
		LPD3DXMESH pMesh;//���b�V�����
		LPD3DXBUFFER pBuffMat;//�}�e���A�����
		DWORD dwNumMat;//�}�e���A���̐�
		std::vector<D3DCOLORVALUE> Diffuse;//���f���̃J���[���
		std::vector<D3DCOLORVALUE> FirstDiffuse;//���f���̃J���[���
	};

	//���f���o�^���
	struct Model
	{
		ModelInfo* aModel;//���f�����
		CTag::TAG tag;//�^�O
		int nType;//�^�C�v
		int nIdx;//ID
		std::string FileName;//�t�@�C���p�X
	};

	//======================================
	// �֐�
	//======================================
	static CXfile* GetInstance();//�C���X�^���X�̎擾
	static void Release();//�C���X�^���X�̔j��
	void Load();//�ǂݍ��ݏ���
	void Unload();//�폜����
	int Regist(CTag::TAG tag, int nType, std::string pModelName);//�o�^����
	Model GetModel(CTag::TAG tag, int nType);//���f���o�^���擾
	ModelInfo* GetAddress(int nIdx) { return m_ModelInfo.at(nIdx).aModel; }//���擾���� ( �C���f�b�N�X���� )
	ModelInfo* GetAddress(CTag::TAG tag, int nType);//���擾���� ( �^�O�E�^�C�v���� )

private:
	//======================================
	// �ϐ�
	//======================================
	static CXfile* m_pXfile;//�C���X�^���X
	std::vector<Model> m_ModelInfo;//���f�����
};

#endif
