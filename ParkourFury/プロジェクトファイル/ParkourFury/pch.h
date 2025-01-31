//==============================================================================================================================================
//
// �v���R���p�C���w�b�_�[
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _PCH_H_
#define _PCH_H_

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#define DIRECTINPUT_VERSION (0x0800) //�r���h���̌x���Ώ��p�}�N��
#include <dinput.h>
#include <xaudio2.h>
#include <Xinput.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h> 
#include <time.h>
#include <assert.h>
#include <vector>
#include <stdexcept>
#include <cstdlib>
#include <crtdbg.h>
#include <functional>
#include <fstream>
#include <string>
#include <sstream>
#include <random>
//#include <EffekseerForDXLib_170e_324b/VC/Effekseer.h>
//#include <EffekseerForDXLib_170e_324b/VC/EffekseerRendererDX9.h>

//===========================================================================================================
// �}�N����`
//===========================================================================================================
#ifdef _DEBUG
#define FIND_MEM_LEAKS
#endif // _DEBUG

#ifdef FIND_MEM_LEAKS
#define NEW new (_NORMAL_BLOCK , __FILE__, __LINE__)
#else
#define NEW new
#endif // FIND_MEM_LEAKS

#define FVF_VERTEX_2D	(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define FVF_VERTEX_3D	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define MAX_VER			(4)//���_��
#define MAX_FRAME		(60)//�ő�t���[����
#define SCREEN_WIDTH	(1280.0f)//�E�C���h�E�̕�
#define SCREEN_HEIGHT	(720.0f)//�E�C���h�E�̍���

//===========================================================================================================
// ���C�u�����̃����N
//===========================================================================================================
#pragma comment(lib,"d3d9.lib")//�`�揈���p
#pragma comment(lib,"d3dx9.lib")//[d3d9.lib]�̊g�����C�u����
#pragma comment(lib,"dxguid.lib")//DirectX�R���|�l�[�g(���i)�g�p
#pragma comment(lib,"winmm.lib")//�V�X�e�������擾�p
#pragma comment(lib,"dinput8.lib")//���͏����p
#pragma comment(lib,"xinput.lib")//�W���C�p�b�h�����p
//#pragma comment(lib,"Effekseer.lib")
//#pragma comment(lib,"EffekseerRendererDX9.lib")

//===========================================================================================================
// �\����
//===========================================================================================================

//���_���(2D)
typedef struct
{
	D3DXVECTOR3 pos;//���_���W
	float rhw;//���W�ϊ��p�W��(1.0f�ŌŒ�)
	D3DCOLOR col;//���_�J���[
	D3DXVECTOR2 tex;//�e�N�X�`�����W
}VERTEX_2D;

//���_���(3D)
typedef struct
{
	D3DXVECTOR3 pos;//���_���W
	D3DXVECTOR3 nor;//�@���x�N�g��
	D3DCOLOR col;//���_�J���[
	D3DXVECTOR2 tex;//�e�N�X�`�����W
}VERTEX_3D;

#endif
