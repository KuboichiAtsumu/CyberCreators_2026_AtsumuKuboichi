//==============================================================================================================================================
//
// �M�~�b�N�Ɋւ��鏈���̃w�b�_�[�t�@�C��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _GIMMICK_H_ //���̃}�N����`������Ȃ�������
#define _GIMMICK_H_ //2�d�C���N���[�h�h�~�̃}�N����`

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "objectX.h"

//===========================================================================================================
// �N���X��`
//===========================================================================================================
class CGimmick : public CObjectX
{
public:
	//======================================
	// �\����
	//======================================

	//�����蔻��p�p�����[�^
	struct CollisionParam
	{
		D3DXVECTOR3 pos;//���W
		D3DXVECTOR3 oldpos;//�X�V�O�̍��W
		D3DXVECTOR3 rot;//�p�x
	};

	//======================================
	// �񋓌^��`
	//======================================

	//�^�C�v
	enum class TYPE
	{
		NONE = 0,
		PLAYER_SPAWNER,//�v���C���[�X�|�[���ʒu
		ROTATION_ZONE,//�X�e�[�W��]�ʒu
		BOSS_SPAWNER,//�{�X�X�|�[���ʒu
		BOSS_MOVE,//�{�X�̈ړ��|�C���g
		BALL_SPAWNER,//�{�[���X�|�[���ʒu
		BALL_MOVE,//�{�[���ړ��|�C���g
		MAX
	};

	//======================================
	// �֐�
	//======================================
	CGimmick(int nPriority = static_cast<int>(Category::GIMMICK));//�R���X�g���N�^
	~CGimmick() override;//�f�X�g���N�^
	HRESULT Init() override;//����������
	void Uninit() override;//�I������
	void Release() override;//�������
	void Update() override;//�X�V����
	void Draw() override;//�`�揈��
	static CGimmick* Create(D3DXVECTOR3 pos, D3DXVECTOR3 scale, TYPE type);//��������
	virtual bool Collision(CollisionParam& aParam);//�����蔻��

	//�O�̃M�~�b�N���
	void SetPrev(CGimmick* pPrev) { m_pPrev = pPrev; }//�ݒ�
	CGimmick* GetPrev() { return m_pPrev; }//�擾

	//���̃M�~�b�N���
	void SetNext(CGimmick* pNext) { m_pNext = pNext; }//�ݒ�
	CGimmick* GetNext() { return m_pNext; }//�擾

	//�^�C�v���
	TYPE GetType() { return m_Type; }//�擾

private:
	//======================================
	// �ϐ�
	//======================================
	CGimmick* m_pPrev;//�����̑O�̃M�~�b�N���
	CGimmick* m_pNext;//�����̎��̃M�~�b�N���
	TYPE m_Type;//�^�C�v
};

#endif
