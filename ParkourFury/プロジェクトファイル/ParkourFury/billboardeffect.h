//==============================================================================================================================================
//
// �r���{�[�h�G�t�F�N�g�Ɋւ��鏈���̃w�b�_�[�t�@�C��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _BILLBOARDEFFECT_H_ //���̃}�N����`������Ȃ�������
#define _BILLBOARDEFFECT_H_ //2�d�C���N���[�h�h�~�̃}�N����`

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "billboard.h"

//===========================================================================================================
// �N���X��`
//===========================================================================================================
class CBillboardEffect : public CBillboard
{
public:
	//======================================
	// �񋓌^��`
	//======================================
	enum class TYPE
	{
		NONE = 0,
		PARTICLE,//�p�[�e�B�N��
		TARGET,//�^�[�Q�b�g
		SMOKE,//��
		MAX
	};

	//======================================
	// �֐�
	//======================================
	CBillboardEffect(int nPriority = static_cast<int>(Category::EFFECT));//�R���X�g���N�^
	~CBillboardEffect() override;//�f�X�g���N�^
	HRESULT Init() override;//����������
	void Uninit() override;//�I������
	void Release() override;//�������
	void Update() override;//�X�V����
	void Draw() override;//�`�揈��
	static CBillboardEffect* Create(TYPE type, const D3DXVECTOR3& pos, const D3DXVECTOR3& size);//��������

	//�^�C�v
	void SetType(TYPE type) { m_type = type; };//�ݒ�

private:
	//======================================
	// �e�^�C�v�e�N�X�`���t�@�C���p�X
	//======================================
	static constexpr const char* TEXTURE_FILE[static_cast<int>(TYPE::MAX)] =
	{
		"",//NONE
		"data/TEXTURE/EFFECT/Particle.jpg",//�p�[�e�B�N��
		"data/TEXTURE/EFFECT/Target.png",//�^�[�Q�b�g
		"data/TEXTURE/EFFECT/Smoke.png",//��
	};

	//======================================
	// �ϐ�
	//======================================
	TYPE m_type;//�^�C�v
};

#endif