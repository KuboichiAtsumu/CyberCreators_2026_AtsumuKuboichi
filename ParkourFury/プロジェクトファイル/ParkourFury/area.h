//==============================================================================================================================================
//
// �G���A�Ɋւ��鏈���̃w�b�_�[�t�@�C��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _AREA_H_ //���̃}�N����`������Ȃ�������
#define _AREA_H_ //2�d�C���N���[�h�h�~�̃}�N����`

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "objectX.h"

//===========================================================================================================
// �N���X��`
//===========================================================================================================
class CArea : public CObjectX
{
public:
	//======================================
	// �񋓌^��`
	//======================================

	//�^�C�v
	enum class TYPE
	{
		NONE = 0,
		BOSS,//�{�X
		MINI_GAME,//�~�j�Q�[��
		CHASE,//�`�F�C�X
		MAX
	};

	//======================================
	// �֐�
	//======================================
	CArea(int nPriority = static_cast<int>(Category::AREA));//�R���X�g���N�^
	~CArea() override;//�f�X�g���N�^
	HRESULT Init() override;//����������
	void Uninit() override;//�I������
	void Release() override;//�������
	void Update() override;//�X�V����
	void Draw() override;//�`�揈��
	static CArea* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, TYPE type);//��������
	virtual bool Collision(D3DXVECTOR3 pos);//�����蔻��

	//�O�̃I�u�W�F�N�g���
	void SetPrevArea(CArea* pArea) { m_pPrev = pArea; }//�ݒ�
	CArea* GetPrevArea() { return m_pPrev; }//����

	//���̃I�u�W�F�N�g���
	void SetNextArea(CArea* pArea) { m_pNext = pArea; }//�ݒ�
	CArea* GetNextArea() { return m_pNext; }//�擾

	//�u���b�N�^�C�v
	void SetAreaType(TYPE type) { m_Type = type; }//�ݒ�
	TYPE GetAreaType() { return m_Type; }//�擾

private:
	//======================================
	// �ϐ�
	//======================================
	CArea* m_pPrev;//�O�̃I�u�W�F�N�g���ւ̃|�C���^
	CArea* m_pNext;//���̃I�u�W�F�N�g���ւ̃|�C���^
	TYPE m_Type;//���
};

#endif
