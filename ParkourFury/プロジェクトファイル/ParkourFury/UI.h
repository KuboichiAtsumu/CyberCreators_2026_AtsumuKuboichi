//==============================================================================================================================================
//
// UI�Ɋւ��鏈���̃w�b�_�[�t�@�C��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _UI_H_ //���̃}�N����`������Ȃ�������
#define _UI_H_ //2�d�C���N���[�h�h�~�̃}�N����`

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "object2D.h"

//===========================================================================================================
// �N���X��`
//===========================================================================================================
class CUI : public CObject2D
{
public:
	//======================================
	// �񋓌^��`
	//======================================

	//�^�C�v
	enum class TYPE
	{
		NONE = 0,
		HEAT_GAGE_FRAME,//�q�[�g�Q�[�W�t���[��
		HEAT_GAGE,//�q�[�g�Q�[�W
		TIME_NUMBER,//�^�C�}�[
		TIMER_HEADING,//�^�C�}�[���o��
		MINIGAME_BACK,//�~�j�Q�[���^�C�}�[�w�i
		MINIGAME_GAGE,//�~�j�Q�[���^�C�}�[�Q�[�W
		MINIGAME_FRAME,//�~�j�Q�[���^�C�}�[�t���[��
		TITLE_LOGO,//�^�C�g�����S
		START_BUTTON,//�X�^�[�g�{�^��
		RESULT_HEAD,//���U���g���o��
		RESULT_TIME_HEAD,//���U���g�^�C�����o��
		RESULT_KILL_HEAD,//���U���g�L�������o��
		RESULT_HEAT_HEAD,//���U���g�L�������o��
		SECONDS,//�b���P��
		PERCENT,//�p�[�Z���g�}�[�N
		MAXSCORE,//�X�R�A�ő�l
		RESULT_TIME,//���U���g�^�C��
		RESULT_KILL,//���U���g�L����
		RESULT_HEAT,//���U���g���σq�[�g��
		TOTAL_SCORE,//�g�[�^���X�R�A
		TUTORIAL_HEAD,//�`���[�g���A�����o��
		DESCRIPT_HEAT,//�q�[�g����
		MINIGAME_HEAD,//�~�j�Q�[�����o��
		ESCAPE,//�`�F�C�X�G���A�x��
		BOSS_HP,//�{�XHP
		BOSS_HP_FRAME,//�{�XHP�̃t���[��
		DESCRIPT_GAGE,//�Q�[�W����
		MAX
	};

	//======================================
	// �֐�
	//======================================
	CUI(int nPriority = static_cast<int>(Category::UI));//�R���X�g���N�^
	~CUI() override;//�f�X�g���N�^
	HRESULT Init() override;//����������
	void Uninit() override;//�I������
	void Update() override;//�X�V����
	void Draw() override;//�`�揈��
	static CUI* Create(TYPE type, const D3DXVECTOR2& pos, const D3DXVECTOR2& size, const D3DXVECTOR3& rot);//��������

	//�O�̃I�u�W�F�N�g���
	void SetPrevUI(CUI* pUI) { m_pPrev = pUI; }//�ݒ�
	CUI* GetPrevUI() { return m_pPrev; }//����

	//���̃I�u�W�F�N�g���
	void SetNextUI(CUI* pUI) { m_pNext = pUI; }//�ݒ�
	CUI* GetNextUI() { return m_pNext; }//�擾

	//�^�C�v
	void SetUIType(TYPE type) { m_Type = type; }//�ݒ�
	TYPE GetUIType() { return m_Type; }//�擾

	//�C���f�b�N�X
	void SetIdx(int nIdx) { m_nIdx = nIdx; }//�ݒ�
	int GetIdx() { return m_nIdx; }//�擾

	//�G�f�B�b�g�\�t���O
	void SetEditFlag(bool bEdit) { m_bEdit = bEdit; }//�ݒ�
	bool GetEditFlag() { return m_bEdit; }//�擾

	//======================================
	// �ϐ�
	//======================================
	static int m_nAll;//����

private:
	//======================================
	// �e�^�C�v�e�N�X�`���t�@�C���p�X
	//======================================
	static constexpr const char* TEXTURE_FILE[static_cast<int>(TYPE::MAX)] =
	{
		"",//NONE
		"data/TEXTURE/UI/HeatGageFrame.png",//�q�[�g�Q�[�W�t���[��
		"data/TEXTURE/UI/HeatGage.png",//�q�[�g�Q�[�W
		"data/TEXTURE/UI/Number.png",//�Q�[���^�C�}�[�p�i���o�[
		"data/TEXTURE/UI/TimerHeading.png",//�^�C�}�[���o��
		"data/TEXTURE/UI/MiniGameTimerGageBack.png",//�~�j�Q�[���^�C�}�[�w�i
		"data/TEXTURE/UI/MiniGameTimerGage.png",//�~�j�Q�[���^�C�}�[
		"data/TEXTURE/UI/MiniGameTimerGageFrame.png",//�~�j�Q�[���^�C�}�[�t���[��
		"data/TEXTURE/UI/TitleLogo.png",//�^�C�g�����S
		"data/TEXTURE/UI/pushA.png",//�X�^�[�g�{�^��
		"data/TEXTURE/UI/ResultHead.png",//���U���g���o��
		"data/TEXTURE/UI/ResultTimeHead.png",//���U���g�^�C�����o��
		"data/TEXTURE/UI/ResultKillHead.png",//���U���g�L�������o��
		"data/TEXTURE/UI/ResultHeatHead.png",//���U���g���σq�[�g�ʌ��o��
		"data/TEXTURE/UI/Seconds.png",//�b���P��
		"data/TEXTURE/UI/Percent.png",//�p�[�Z���g�}�[�N
		"data/TEXTURE/UI/ResultMaxScore.png",//�X�R�A�ő�l
		"data/TEXTURE/UI/Number.png",//���U���g�^�C�}�[�p�i���o�[
		"data/TEXTURE/UI/Number.png",//���U���g�L�����p�i���o�[
		"data/TEXTURE/UI/Number.png",//���U���g���σq�[�g�ʗp�i���o�[
		"data/TEXTURE/UI/Number.png",//�g�[�^���X�R�A�p�i���o�[
		"data/TEXTURE/UI/TutorialHead.png",//�`���[�g���A�����o��
		"data/TEXTURE/UI/DescriptHeat.png",//�q�[�g����
		"data/TEXTURE/UI/MiniGameHead.png",//�~�j�Q�[�����o��
		"data/TEXTURE/UI/escape.png",//�`�F�C�X�G���A�x��
		"",//�{�XHP(�e�N�X�`������)
		"data/TEXTURE/UI/BossHPFrame.png",//�{�XHP�t���[��
		"data/TEXTURE/UI/DescriptGage.png",//�Q�[�W����
	};

	//======================================
	// �ϐ�
	//======================================
	CUI* m_pPrev;//�O�̃I�u�W�F�N�g���ւ̃|�C���^
	CUI* m_pNext;//���̃I�u�W�F�N�g���ւ̃|�C���^
	TYPE m_Type;//�^�C�v
	int m_nIdx;//�C���f�b�N�X
	bool m_bEdit;//�G�f�B�b�g�\�t���O
};

#endif
