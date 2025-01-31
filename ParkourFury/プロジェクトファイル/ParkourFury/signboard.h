//==============================================================================================================================================
//
// �Ŕ�U�Ɋւ��鏈���̃w�b�_�[�t�@�C��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _SIGNBOARD_H_ //���̃}�N����`������Ȃ�������
#define _SIGNBOARD_H_ //2�d�C���N���[�h�h�~�̃}�N����`

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "objectX.h"

//===========================================================================================================
// �N���X��`
//===========================================================================================================
class CSignBoard : public CObjectX
{
public:
	//======================================
	// �񋓌^��`
	//======================================

	//�^�C�v
	enum class TYPE
	{
		NONE = 0,

		//�`���[�g���A��
		TUTORIAL_JUMP,//�W�����v����
		TUTORIAL_DASH,//�_�b�V������
		TUTORIAL_WALLKICK,//�ǃL�b�N����
		TUTORIAL_SHOT,//���ː���
		TUTORIAL_SLIDING,//�X���C�f�B���O����

		//�Q�[��
		GUIDANCE_MINIGAME,//�~�j�Q�[���G���A�ē�
		GUIDANCE_BOSS,//�{�X�G���A�ē�

		MAX
	};

	//======================================
	// �֐�
	//======================================
	CSignBoard(int nPriority = static_cast<int>(Category::SIGN_BOARD));//�R���X�g���N�^
	~CSignBoard() override;//�f�X�g���N�^
	HRESULT Init() override;//����������
	void Uninit() override;//�I������
	void Update() override;//�X�V����
	void Draw() override;//�`�揈��
	static CSignBoard* Create(TYPE type, D3DXVECTOR3 pos, D3DXVECTOR3 scale, D3DXVECTOR3 rot);//��������

	//�O�̃I�u�W�F�N�g���
	void SetPrev(CSignBoard* pUI) { m_pPrev = pUI; }//�ݒ�
	CSignBoard* GetPrev() { return m_pPrev; }//����

	//���̃I�u�W�F�N�g���
	void SetNext(CSignBoard* pUI) { m_pNext = pUI; }//�ݒ�
	CSignBoard* GetNext() { return m_pNext; }//�擾

	//�^�C�v
	void SetType(TYPE type) { m_Type = type; }
	TYPE GetType() { return m_Type; }//�擾

private:
	//======================================
	// �e�^�C�v�e�N�X�`���t�@�C���p�X
	//======================================
	static constexpr const char* TEXTURE_FILE[static_cast<int>(TYPE::MAX)] =
	{
		"",//NONE

		//�`���[�g���A��
		"data/TEXTURE/SIGN_BOARD/TutorialJump.png",//�W�����v����
		"data/TEXTURE/SIGN_BOARD/TutorialDash.png",//�_�b�V������
		"data/TEXTURE/SIGN_BOARD/TutorialWallKick.png",//�ǃL�b�N����
		"data/TEXTURE/SIGN_BOARD/TutorialShot.png",//���ː���
		"data/TEXTURE/SIGN_BOARD/TutorialSliding.png",//�X���C�f�B���O����

		//�Q�[��
		"data/TEXTURE/SIGN_BOARD/GuidanceMiniGame.png",//�~�j�Q�[���G���A�ē�
		"data/TEXTURE/SIGN_BOARD/GuidanceBoss.png",//�{�X�G���A�ē�
	};

	//======================================
	// �ϐ�
	//======================================
	CSignBoard* m_pPrev;//�O�̃I�u�W�F�N�g���ւ̃|�C���^
	CSignBoard* m_pNext;//���̃I�u�W�F�N�g���ւ̃|�C���^
	TYPE m_Type;//�^�C�v
};

#endif
