//==============================================================================================================================================
//
// �V�[���Ɋւ��鏈���̃w�b�_�[�t�@�C��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _SCENE_H_ //���̃}�N����`������Ȃ�������
#define _SCENE_H_ //2�d�C���N���[�h�h�~�̃}�N����`

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "main.h"

//===========================================================================================================
// �O���錾
//===========================================================================================================
class CEdit2D;
class CEdit3DManager;
class CBlockManager;
class CGimmickManager;
class CBulletManager;
class CEnemyManager;
class CAreaManager;
class CUIManager;
class CSignBoardManager;

//===========================================================================================================
// �N���X��`
//===========================================================================================================
class CScene
{
public:
	//======================================
	// �񋓌^��`
	//======================================

	//���[�h
	enum class MODE
	{
		MODE_NONE = 0,
		MODE_TITLE,//�^�C�g��
		MODE_TUTORIAL,//�`���[�g���A��
		MODE_GAME,//�Q�[��
		MODE_RESULT,//���U���g
		MODE_RANKING,//�����L���O
		MODE_MAX
	};

	//�V�[��
	enum class SCENE
	{
		GAME = 0,//�Q�[��
		EDIT_3D,//3D�G�f�B�^
		EDIT_2D,//2D�G�f�B�^
	};

	//======================================
	// �\���̒�`
	//======================================
	struct ResultScore
	{
		int nGoalTime;//�S�[���^�C��
		float fAverageHeat;//���σq�[�g��
		float fNumKillenemy;//�|�����G�̐�(����)
	};

	//======================================
	// �֐�
	//======================================
	CScene();//�R���X�g���N�^
	virtual ~CScene();//�f�X�g���N�^
	virtual HRESULT Init();//����������
	virtual void Uninit();//�I������
	virtual void Update();//�X�V����
	virtual void Draw();//�`�揈��
	static CScene* Create(MODE mode);//��������

	//3D�G�f�B�^�}�l�[�W���[
	void SetEditManager(CEdit3DManager* pManager) { m_pEditManager = pManager; }//�ݒ�
	CEdit3DManager* GetEditManager() { return m_pEditManager; }//�擾

	//�u���b�N�}�l�[�W���[
	void SetBlockManager(CBlockManager* pManager) { m_pBlockManager = pManager; }//�ݒ�
	CBlockManager* GetBlockManager() { return m_pBlockManager; }//�擾

	//�G�l�~�[�}�l�[�W���[
	void SetEnemyManager(CEnemyManager* pManager) { m_pEnemyManager = pManager; }//�ݒ�
	CEnemyManager* GetEnemyManager() { return m_pEnemyManager; }//�擾

	//�M�~�b�N�}�l�[�W���[
	void SetGimmickManager(CGimmickManager* pManager) { m_pGimmickManager = pManager; }//�ݒ�
	CGimmickManager* GetGimmickManager() { return m_pGimmickManager; }//�擾

	//�o���b�g�}�l�[�W���[
	void SetBulletManager(CBulletManager* pManager) { m_pBulletManager = pManager; }//�ݒ�
	CBulletManager* GetBulletManager() { return m_pBulletManager; }//�擾

	//�G���A�}�l�[�W���[
	void SetAreaManager(CAreaManager* pManager) { m_pAreaManager = pManager; }//�ݒ�
	CAreaManager* GetAreaManager() { return m_pAreaManager; }//�擾

	//UI�}�l�[�W���[
	void SetUIManager(CUIManager* pManager) { m_pUIManager = pManager; }//�ݒ�
	CUIManager* GetUIManager() { return m_pUIManager; }//�擾

	//�Ŕ}�l�[�W���[
	void SetSignBoardManager(CSignBoardManager* pManager) { m_pSignBoardManager = pManager; }//�ݒ�
	CSignBoardManager* GetSignBoardManager() { return m_pSignBoardManager; }//�擾

	//���U���g�p�X�R�A
	void SetResultScore(ResultScore aResultScore) { m_aResultScore = aResultScore; }//�ݒ�
	ResultScore GetResultScore() { return m_aResultScore; }//�擾

	//���[�h���
	void SetMode(MODE mode) { m_mode = mode; }//�ݒ�
	MODE GetMode() { return m_mode; }//�擾

	//���݂̃V�[��
	void SetScene(SCENE scene) { m_Scene = scene; }//�ݒ�
	SCENE GetSceneState() { return m_Scene; }//�擾

private:
	//======================================
	// �ϐ�
	//======================================

	//�C���X�^���X
	CEdit2D* m_pEdit2D;//2D�G�f�B�^�̃C���X�^���X
	CEdit3DManager* m_pEditManager;//�G�f�B�^�}�l�[�W���[
	CBlockManager* m_pBlockManager;//�u���b�N�}�l�[�W���[
	CGimmickManager* m_pGimmickManager;//�M�~�b�N�}�l�[�W���[
	CBulletManager* m_pBulletManager;//�o���b�g�}�l�[�W���[
	CEnemyManager* m_pEnemyManager;//�G�l�~�[�}�l�[�W���[
	CAreaManager* m_pAreaManager;//�G���A�}�l�[�W���[
	CUIManager* m_pUIManager;//UI�}�l�[�W���[
	CSignBoardManager* m_pSignBoardManager;//�Ŕ}�l�[�W���[

	static ResultScore m_aResultScore;//���U���g�p�X�R�A
	MODE m_mode;//���[�h
	SCENE m_Scene;//���݂̃V�[��
};

#endif
