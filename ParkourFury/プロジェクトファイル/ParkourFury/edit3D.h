#ifdef _DEBUG
//==============================================================================================================================================
//
// 3D�G�f�B�^�Ɋւ��鏈���̃w�b�_�[�t�@�C��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _EDIT3D_H_
#define _EDIT3D_H_

//===========================================================================================================
// �t�@�C���C���N���[�h
//===========================================================================================================
#include "objectx.h"

//===========================================================================================================
// �N���X��`
//===========================================================================================================
class CEdit3D : public CObjectX
{
public:
	//======================================
	// �֐�
	//======================================
	CEdit3D(int nPriority = 0);//�R���X�g���N�^
	~CEdit3D();//�f�X�g���N�^
	HRESULT Init();//����������
	void Uninit();//�I������
	void Release();//�������
	void Update();//�X�V����
	static void EditUpdate();//�G�f�B�^���X�V����
	void Draw();//�`�揈��
	static CEdit3D* Create(CEdit3D aEdit);//��������

	//�O�̃I�u�W�F�N�g���
	void SetPrevEdit(CEdit3D* pEdit) { m_pPrev = pEdit; }//�ݒ�
	CEdit3D* GetPrevEdit() { return m_pPrev; }//����

	//���̃I�u�W�F�N�g���
	void SetNextEdit(CEdit3D* pEdit) { m_pNext = pEdit; }//�ݒ�
	CEdit3D* GetNextEdit() { return m_pNext; }//�擾

	//�^�O
	void SetTag(CTag::TAG tag) { m_tag = tag; }//�ݒ�

	//�^�C�v
	void SetType(int nType) { m_nType = nType; }//�ݒ�

	//�C���f�b�N�X
	void SetIdx(int nIdx) { m_nIdx = nIdx; }//�ݒ�
	int GetIdx() { return m_nIdx; }//�擾

private:
	//======================================
	// �񋓌^��`
	//======================================

	//���샂�[�h
	enum class MODE
	{
		MODE_MOVE,//�ړ�
		MODE_SCALE,//�g��
	};

	//======================================
	// �֐�
	//======================================
	static void Save();//�Z�[�u����
	void SetCameraPos();//�J�����̐ݒ�
	void Move();//�ړ�����
	void Scale();//�g�又��
	void Turn();//��]����
	void Reset();//���Z�b�g����
	void Change();//�J�e�S���[�E�^�C�v�ύX����

	//======================================
	// �ϐ�
	//======================================
	static constexpr int TURN_DIRECTION = 4;//��]����������
	static constexpr float MOVE = 5.0f;//�ړ���
	static constexpr float SCALE = 0.1f;//�g���
	static constexpr float TURN = D3DX_PI * (1.0f / static_cast<float>(TURN_DIRECTION));//��]��
	static MODE m_mode;//���샂�[�h
	static int m_nAll;//�z�u��
	static int m_nTarget;//�ҏW�Ώ۔ԍ�
	static bool m_bTrigger;//�ړ����@���g���K�[���ǂ���
	CEdit3D* m_pPrev;//�O�̃I�u�W�F�N�g���ւ̃|�C���^
	CEdit3D* m_pNext;//���̃I�u�W�F�N�g���ւ̃|�C���^
	CTag::TAG m_tag;//�^�O
	int m_nIdx;//�C���f�b�N�X
	int m_nType;//�^�C�v
};

#endif
#endif // _DEBUG
