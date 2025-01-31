//==============================================================================================================================================
//
// �G�l�~�[�Ɋւ��鏈���̃w�b�_�[�t�@�C��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _ENEMY_H_ //���̃}�N����`������Ȃ�������
#define _ENEMY_H_ //2�d�C���N���[�h�h�~�̃}�N����`

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "objectX.h"

//===========================================================================================================
// �N���X��`
//===========================================================================================================
class CEnemy : public CObjectX
{
public:
	//======================================
	// �񋓌^��`
	//======================================

	//�^�C�v
	enum class TYPE
	{
		NONE = 0,
		NORMAL,//�m�[�}��
		BALL,//�{�[��
		MOVE,//���[�u
		BULLET,//�o���b�g
		MAX
	};

	//======================================
	// �\���̒�`
	//======================================

	//�����蔻��p�p�����[�^
	struct CollisionParam
	{
		D3DXVECTOR3 pos;//���W
		D3DXVECTOR3 size;//�T�C�Y
		CEnemy* pEnemy;//���������G�l�~�[�̏��
	};

	//======================================
	// �֐�
	//======================================
	CEnemy(int nPriority = static_cast<int>(Category::ENEMY));//�R���X�g���N�^
	~CEnemy() override;//�f�X�g���N�^
	HRESULT Init() override;//����������
	void Uninit() override;//�I������
	void Release() override;//�������
	void Update() override;//�X�V����
	void Draw() override;//�`�揈��
	virtual bool Collision(CollisionParam& aParam);//�����蔻��
	static CEnemy* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, TYPE type);//��������

	//�O�̃I�u�W�F�N�g���
	void SetPrevEnemy(CEnemy* pEnemy) { m_pPrev = pEnemy; }//�ݒ�
	CEnemy* GetPrevEnemy() { return m_pPrev; }//����

	//���̃I�u�W�F�N�g���
	void SetNextEnemy(CEnemy* pEnemy) { m_pNext = pEnemy; }//�ݒ�
	CEnemy* GetNextEnemy() { return m_pNext; }//�擾

	//�^�C�v
	void SetEnemyType(TYPE type) { m_Type = type; }//�ݒ�
	TYPE GetEnemyType() { return m_Type; }//�擾

	//����
	static void ResetAll() { m_nAll = 0; }//���Z�b�g
	static int GetAll() { return m_nAll; }//�擾

private:
	//======================================
	// �֐�
	//======================================
	bool Collision_X(CollisionParam aParam);//X���̓����蔻��
	bool Collision_Y(CollisionParam aParam);//Y���̓����蔻��
	bool Collision_Z(CollisionParam aParam);//Z���̓����蔻��

	//======================================
	// ���f���t�@�C���p�X
	//======================================
	static constexpr const char* MODEL_FILE[static_cast<int>(TYPE::MAX)] =
	{
		"",
		"data/MODEL/ENEMY/NormalEnemy.x",//�m�[�}��
		"data/MODEL/ENEMY/ball.x",//�{�[��
		"data/MODEL/ENEMY/MoveEnemy.x",//�����G
		"data/MODEL/ENEMY/BulletEnemy.x",//�e�����G
	};

	//======================================
	// �ϐ�
	//======================================
	static int m_nAll;//����
	CEnemy* m_pPrev;//�O�̃I�u�W�F�N�g���ւ̃|�C���^
	CEnemy* m_pNext;//���̃I�u�W�F�N�g���ւ̃|�C���^
	TYPE m_Type;//���
};

#endif
