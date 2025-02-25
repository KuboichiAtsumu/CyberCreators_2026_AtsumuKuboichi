//==============================================================================================================================================
//
// �o���b�g�Ɋւ��鏈���̃w�b�_�[�t�@�C��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _BULLET_H_
#define _BULLET_H_

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "objectX.h"

//===========================================================================================================
// �N���X��`
//===========================================================================================================
class CBullet : public CObjectX
{
public:
	//======================================
	// �񋓌^��`
	//======================================

	//�e�̃^�C�v
	enum class TYPE
	{
		NONE = 0,
		PLAYER,//�v���C���[
		MAX
	};

	//======================================
	// �֐�
	//======================================
	CBullet(int nPriority = 3);//�R���X�g���N�^
	~CBullet() override;//�f�X�g���N�^
	HRESULT Init() override;//����������
	void Uninit() override;//�I������
	void Update() override;//�X�V����
	void Draw() override;//�`�揈��
	static CBullet* Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& move, const D3DXVECTOR3& rot, TYPE m_type);//��������
	
	//�O�̃o���b�g���
	void SetPrevBullet(CBullet* pBullet) { m_pPrev = pBullet; }//�ݒ�
	CBullet* GetPrevBullet() { return m_pPrev; }//�擾

	//���̃o���b�g���
	void SetNextBullet(CBullet* pBullet) { m_pNext = pBullet; }//�ݒ�
	CBullet* GetNextBullet() { return m_pNext; }//�擾

	//�^�C�v
	void SetBulletType(TYPE Type) { m_Type = Type; }//�ݒ�
	TYPE GetBulletType() { return m_Type; }//�擾

	//����
	void SetLife(int nLife) { m_nLife = nLife; }//�ݒ�

private:
	//======================================
	// �֐�
	//======================================
	void CollisionEnemy();//�G�l�~�[�Ƃ̓����蔻��
	void CollisionBoss();//�{�X�Ƃ̓����蔻��
	void CollisionBlock();//�u���b�N�Ƃ̓����蔻��

	//======================================
	// �ϐ�
	//======================================
	CBullet* m_pPrev;//�O�̏��̃|�C���^
	CBullet* m_pNext;//���̏��̃|�C���^
	TYPE m_Type;//�^�C�v
	int m_nLife;//����
};

#endif
