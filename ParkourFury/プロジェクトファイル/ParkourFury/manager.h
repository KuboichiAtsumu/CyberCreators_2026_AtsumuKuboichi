//==============================================================================================================================================
//
// �}�l�[�W���[�Ɋւ��鏈���̃w�b�_�[�t�@�C��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _MANAGER_H_ //���̃}�N����`������Ȃ�������
#define _MANAGER_H_ //2�d�C���N���[�h�h�~�̃}�N����`

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "scene.h"
#include "tag.h"
#include "debugproc.h"
#include "renderer.h"
#include "keyboard.h"
#include "mouse.h"
#include "joypad.h"
#include "camera.h"
#include "light.h"
#include "fade.h"
#include "sound.h"
#include "collision.h"
#include "calculate.h"

//===========================================================================================================
// �N���X��`
//===========================================================================================================
class CManager
{
private:
	//======================================
	// �֐�
	//======================================
	CManager();//�R���X�g���N�^
	~CManager();//�f�X�g���N�^

public:
	//======================================
	// �X�e�[�W���t�@�C���p�X
	//======================================
	static constexpr const char* StageFile[static_cast<int>(CScene::MODE::MODE_MAX)] =
	{
		"",//NONE
		"data/STAGE/GameStage.bin",//TITLE
		"data/STAGE/TutorialStage.bin",//TUTORIAL
		"data/STAGE/GameStage.bin",//GAME
		"",//RESULT
		"",//RANKING
	};

	//======================================
	// UI���t�@�C���p�X
	//======================================
	static constexpr const char* UIFile[static_cast<int>(CScene::MODE::MODE_MAX)] =
	{
		"",//NONE
		"data/UI/TitleUI.bin",//TITLE
		"data/UI/TutorialUI.bin",//TUTORIAL
		"data/UI/GameUI.bin",//GAME
		"data/UI/ResultUI.bin",//RESULT
		"data/UI/RankingUI.bin",//RANKING
	};

	//======================================
	// �֐�
	//======================================
	static CManager* GetInstance();//�C���X�^���X�̎擾
	static void Release();//�C���X�^���X�̔j��

	HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);//����������
	void Uninit();//�I������
	void Update();//�X�V����
	void Draw();//�`�揈��
	void SetMode(CScene::MODE mode);//���[�h�̐ݒ�

	//�C���X�^���X�擾
	CTag* GetTag() { return m_pTag; }//�^�O
	CRenderer* GetRenderer() { return m_pRenderer; }//�����_���[
	CInputKeyboard* GetKeyboard() { return m_pKeyboard; }//�L�[�{�[�h
	CInputJoypad* GetJoypad() { return m_pJoypad; }//�W���C�p�b�h
	CInputMouse* GetMouse() { return m_pMouse; }//�}�E�X
	CSound* GetSound() { return m_pSound; }//�T�E���h
	CCamera* GetCamera() { return m_pCamera; }//�J����
	CLight* GetLight() { return m_pLight; }//���C�g
	CFade* GetFade() { return m_pFade; }//�t�F�[�h
	CScene* GetScene() { return m_pScene; }//�V�[��
	CCollision* GetCollision() { return m_pCollision; }//�����蔻��
	CCalculate* GetCalculate() { return m_pCalculate; }//�v�Z����

	//======================================
	// �萔
	//======================================
	static inline float FIELD_RANGE = 3200.0f;//�t�B�[���h�͈̔�
	static inline D3DXVECTOR3 VTXMIN_FIELD = { -FIELD_RANGE * 0.5f, 0.0f, -FIELD_RANGE * 0.5f };//�t�B�[���h�͈̔͂̍��W�ŏ��l
	static inline D3DXVECTOR3 VTXMAX_FIELD = { FIELD_RANGE * 0.5f, 0.0f, FIELD_RANGE * 0.5f };//�t�B�[���h�͈̔͂̍��W�ő�l

private:
	//======================================
	// �ϐ�
	//======================================
	static CManager* m_pManager;//�C���X�^���X
	CTag* m_pTag;//�^�O
	CRenderer* m_pRenderer;//�����_���[
	CInputKeyboard* m_pKeyboard;//�L�[�{�[�h
	CInputJoypad* m_pJoypad;//Pad
	CInputMouse* m_pMouse;//�}�E�X
	CSound* m_pSound;//�T�E���h
	CCamera* m_pCamera;//�J����
	CLight* m_pLight;//���C�g
	CScene* m_pScene;//���݂̉��
	CFade* m_pFade;//�t�F�[�h
	CCollision* m_pCollision;//�����蔻��
	CCalculate* m_pCalculate;//�v�Z����
};

#endif