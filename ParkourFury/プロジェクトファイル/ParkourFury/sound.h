//==============================================================================================================================================
//
// �T�E���h�����Ɋւ���w�b�_�[�t�@�C��
// Author : Yuudai Takaya
//
//==============================================================================================================================================
#ifndef _SOUND_H_
#define _SOUND_H_

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "main.h"//���C���w�b�_�[

//===========================================================================================================
// �N���X��`
//===========================================================================================================
class CSound
{
public:
	//======================================
	// �T�E���h�ꗗ
	//======================================
	enum class SOUND
	{
		//BGM
		BGM_TITLE,//�^�C�g��BGM
		BGM_TUTORIAL,//�`���[�g���A��BGM
		BGM_GAME,//�Q�[��BGM
		BGM_RESULT,//���U���gBGM

		//SE
		SHOT,//����
		NICE,//NICE
		GREAT,//GREAT
		EXCELLENT,//EXCELLENT
		HIT,//���j
		JUMP,//�W�����v
		DOUBLE_JUMP,//�_�u���W�����v
		SLIDING,//�X���C�f�B���O
		SELECT,//���艹
		MINIGAME_CLEAR,//�~�j�Q�[���N���A

		SOUND_LABEL_MAX,
	};

	//======================================
	// �T�E���h���̍\���̒�`
	//======================================
	typedef struct
	{
		const char* pFilename;// �t�@�C����
		int nCntLoop;// ���[�v�J�E���g
		float Volume;// ����
	}SOUNDINFO;

	//======================================
	// �T�E���h�̏��
	//======================================
	CSound::SOUNDINFO m_aSoundInfo[static_cast<int>(SOUND::SOUND_LABEL_MAX)] =
	{
		//BGM
		"data/SOUND/BGM/Title.wav", 1, 1.0f,//�^�C�g��
		"data/SOUND/BGM/Tutorial.wav", 1, 1.0f,//�`���[�g���A��
		"data/SOUND/BGM/Game.wav", 1, 1.0f,//�Q�[��
		"data/SOUND/BGM/Result.wav", 1, 1.0f,//���U���g

		//SE
		"data/SOUND/SE/Shot.wav", 0, 1.0f,//����
		"data/SOUND/SE/Nice.wav", 0, 0.7f,//NICE
		"data/SOUND/SE/Great.wav", 0, 0.7f,//GREAT
		"data/SOUND/SE/Excellent.wav", 0, 0.7f,//EXCELLENT
		"data/SOUND/SE/Hit.wav", 0, 1.0f,//���j
		"data/SOUND/SE/Jump.wav", 0, 2.0f,//�W�����v
		"data/SOUND/SE/DoubleJump.wav", 0, 2.0f,//�_�u���W�����v
		"data/SOUND/SE/Sliding.wav", 0, 3.0f,//�X���C�f�B���O
		"data/SOUND/SE/Select.wav", 0, 1.0f,//���艹
		"data/SOUND/SE/MiniGameClear.wav", 0, 0.7f,//�~�j�Q�[���N���A
	};

	//======================================
	// �֐�
	//======================================
	HRESULT Init(HWND hWnd);//����������
	void Uninit();//�I������
	HRESULT Play(SOUND label);//�Đ�����
	void Stop(SOUND label);//��~����(�I��)
	void Stop();//�S��~����

private:
	//======================================
	// �֐�
	//======================================
	HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD* pChunkSize, DWORD* pChunkDataPosition);
	HRESULT ReadChunkData(HANDLE hFile, void* pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);

	//======================================
	// �ϐ�
	//======================================
	IXAudio2* m_pXAudio2 = nullptr;// XAudio2�I�u�W�F�N�g�ւ̃C���^�[�t�F�C�X
	IXAudio2MasteringVoice* m_pMasteringVoice = nullptr;// �}�X�^�[�{�C�X
	IXAudio2SourceVoice* m_apSourceVoice[static_cast<int>(SOUND::SOUND_LABEL_MAX)];// �\�[�X�{�C�X
	BYTE* m_apDataAudio[static_cast<int>(SOUND::SOUND_LABEL_MAX)];// �I�[�f�B�I�f�[�^
	DWORD m_aSizeAudio[static_cast<int>(SOUND::SOUND_LABEL_MAX)];// �I�[�f�B�I�f�[�^�T�C�Y
};

#endif
