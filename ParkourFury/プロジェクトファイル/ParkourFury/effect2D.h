//==============================================================================================================================================
//
// 2D�G�t�F�N�g�Ɋւ��鏈���̃w�b�_�[�t�@�C��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _EFFECT2D_H_ //���̃}�N����`������Ȃ�������
#define _EFFECT2D_H_ //2�d�C���N���[�h�h�~�̃}�N����`

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "object2D.h"

//===========================================================================================================
// �N���X��`
//===========================================================================================================
class CEffect2D : public CObject2D
{
public:
	//======================================
	// �񋓌^��`
	//======================================

	//�^�C�v
	enum class TYPE
	{
		NONE = 0,
		NICE,//NICE
		GREAT,//GREAT
		EXCELLENT,//EXCELLENT
		BAD,//BAD
		MAX
	};

	//======================================
	// �֐�
	//======================================
	CEffect2D(int nPriority = static_cast<int>(Category::EFFECT_2D));//�R���X�g���N�^
	~CEffect2D() override;//�f�X�g���N�^
	HRESULT Init() override;//����������
	void Uninit() override;//�I������
	void Update() override;//�X�V����
	void Draw() override;//�`�揈��
	static CEffect2D* Create(TYPE type, const D3DXVECTOR2& pos, const D3DXVECTOR3& rot);//��������

	//�������g�̃|�C���^
	static CEffect2D* GetEffect2D() { return m_pEffect2D; }//�擾

	//�^�C�v
	void SetEffect2DType(TYPE type) { m_Type = type; }//�ݒ�
	TYPE GetEffect2DType() { return m_Type; }//�擾

	//�ڕW�̃T�C�Y
	void SetTargetSize(const D3DXVECTOR2& TargetSize) { m_TargetSize = TargetSize; }//�ݒ�

private:
	//======================================
	// �֐�
	//======================================
	void ColorChange();//�J���[�ύX����
	void ScaleDownSize();//�T�C�Y�k������

	//======================================
	// �e�^�C�v�e�N�X�`���t�@�C���p�X
	//======================================
	static constexpr const char* TEXTURE_FILE[static_cast<int>(TYPE::MAX)] =
	{
		"",//NONE
		"data/TEXTURE/EFFECT/Nice.png",//NICE
		"data/TEXTURE/EFFECT/Great.png",//GREAT
		"data/TEXTURE/EFFECT/Excellent.png",//EXCELLENT
		"data/TEXTURE/EFFECT/Bad.png",//BAD
	};

	//======================================
	// �萔
	//======================================
	const int SCALE_DOWN_FRAME = 10;//�k������t���[����
	const float COLOR_SPEED = 0.1f;//�J���[�ύX�X�s�[�h
	const float MAG_CREATE_SIZE = 4.0f;//�������̃T�C�Y�{��

	//======================================
	// �ϐ�
	//======================================
	static CEffect2D* m_pEffect2D;//�������g�̃|�C���^
	D3DXVECTOR2 m_TargetSize;//�ڕW�̃T�C�Y
	D3DXVECTOR2 m_ScaleDown;//�k������l
	TYPE m_Type;//�^�C�v
	int m_nScaleDownFrame;//�k���t���[��
};

#endif
