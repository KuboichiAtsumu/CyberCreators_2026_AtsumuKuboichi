//=======================================================================================================================================================================================================================
//
// �X�N���[���Ɋւ��鏈��
// Author : Atsumu Kuboichi
//
//=======================================================================================================================================================================================================================
#include "main.h"
#include <cstdio>
#include "light.h"
#include "game.h"

//=======================================================================================================================================================================================================================
// �O���[�o���ϐ�
//=======================================================================================================================================================================================================================
LPDIRECT3D9 g_pD3D = NULL;				//Direct3D�I�u�W�F�N�g�ւ̃|�C���^
LPDIRECT3DDEVICE9 g_pD3DDevice = NULL;	//Direct3D�f�o�C�X�ւ̃|�C���^
LPD3DXFONT g_pFont = NULL;				//�t�H���g�ւ̃|�C���^
MODE g_mode = MODE_GAME;				//���݂̃��[�h
int g_nCountFPS = 0;					//FPS�J�E���^
bool g_isFullscreen = false;			//�E�B���h�E��؂�ւ��邽�߂̃t���O
RECT g_windowRect;						//�E�B���h�E��؂�ւ��邽�߂̕ϐ�

//=======================================================================================================================================================================================================================
// ���C���֐�
//=======================================================================================================================================================================================================================
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hInstancePrev*/, _In_ LPSTR /*lpCmdLine*/, _In_ int nCmdShow)
{
	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),				//WNDCLASSEX�̃������T�C�Y
		CS_CLASSDC,						//�E�C���h�E�̃X�^�C��
		WindowProc,						//�E�C���h�E�v���V�[�W��
		0,								//0�ɂ���
		0,								//0�ɂ���
		hInstance,						//�C���X�^���X�n���h��
		LoadIcon(NULL,IDI_ASTERISK),	//�^�X�N�o�[�̃A�C�R��
		LoadCursor(NULL,IDC_CROSS),		//�}�E�X�J�[�\��
		(HBRUSH)(COLOR_ACTIVECAPTION),	//�N���C�A���g�̈�̔w�i�F
		NULL,							//���j���[�o�[
		CLASS_NAME,						//�E�C���h�E�N���X�̖��O
		LoadIcon(NULL,IDI_APPLICATION)	//�t�@�C���̃A�C�R��
	};

	HWND hWnd;				//�E�C���h�E�n���h��(���ʎq)
	MSG msg;				//���b�Z�[�W���i�[����ϐ�
	RECT rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
	DWORD dwCurrentTime;	//���ݎ���
	DWORD dwExecLastTime;	//�Ō�ɏ�����������
	DWORD dwFrameCount;		//�t���[���J�E���g
	DWORD dwFPSLastTime;	//�Ō��FPS���v����������

	RegisterClassEx(&wcex);	//�E�C���h�E�N���X�̓o�^

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);	//�N���C�A���g�̈���w��̃T�C�Y�ɒ���

	//�E�C���h�E�𐶐�
	hWnd = CreateWindowEx(0,		//�g���E�C���h�E�X�^�C��
		CLASS_NAME,					//�E�C���h�E�N���X�̖��O
		WINDOW_NAME,				//�E�C���h�E�̖��O
		WS_OVERLAPPEDWINDOW,		//�E�C���h�E�X�^�C��
		CW_USEDEFAULT,				//�E�C���h�E����X���W
		CW_USEDEFAULT,				//�E�C���h�E����Y���W
		(rect.right-rect.left),		//�E�C���h�E�̕�
		(rect.bottom - rect.top),	//�E�C���h�E�̍���
		NULL,						//�e�E�C���h�E�̃n���h��
		NULL,						//���j���[�n���h���܂��͎q�E�C���h�EID
		hInstance,					//�C���X�^���X�n���h��
		NULL);						//�E�C���h�E�̍쐬�f�[�^

	//����������
	if (FAILED(Init(hInstance, hWnd, TRUE)))
	{
		return-1;//���������������s�����ꍇ
	}

	//�E�C���h�E�̕\��
	ShowWindow(hWnd, nCmdShow);	//�E�C���h�E�̕\����Ԃ̐ݒ�
	UpdateWindow(hWnd);			//�N���C�A���g�̈���X�V

	//����\��ݒ�
	timeBeginPeriod(1);

	dwCurrentTime = 0;				//����������
	dwExecLastTime = timeGetTime();	//���ݎ������擾(�ۑ�)
	dwFrameCount = 0;
	dwFPSLastTime = timeGetTime();

	//���b�Z�[�W���[�v
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
		{
			//windows�̏���
			if (msg.message == WM_QUIT) //�I��
			{
				break;//WM_QUIT���b�Z�[�W���󂯎�����烁�b�Z�[�W���[�v�𔲂���
			}
			else
			{
				//���b�Z�[�W�̐ݒ�
				TranslateMessage(&msg);	//���z�L�[���b�Z�[�W�𕶎����b�Z�[�W�֕ϊ�
				DispatchMessage(&msg);	//�E�C���h�E�v���V�[�W���փ��b�Z�[�W�𑗏o
			}
		}
		else
		{
			//DirectX�̏���
			dwCurrentTime = timeGetTime(); //���ݎ������擾

			if ((dwCurrentTime - dwFPSLastTime) >= MAX_TIMEF) //0.5�b�o��
			{
				g_nCountFPS = (dwFrameCount * 1000) / (dwCurrentTime - dwFPSLastTime);	//FPS���v��
				dwFPSLastTime = dwCurrentTime;											//FPS�𑪒肵��������ۑ�
				dwFrameCount = 0;														//�t���[���J�E���g���N���A
			}
			if ((dwCurrentTime - dwExecLastTime) >= (1000 / MAX_FRAME))
			{
				//���Ԍo��
				dwExecLastTime = dwCurrentTime; //���ݎ�����ۑ�

				Update();	//�X�V����
				Draw();		//�`�揈��

				dwFrameCount++; //�t���[���J�E���g�����Z
			}
		}
	}
	
	Uninit(); //�I������
	
	UnregisterClass(CLASS_NAME, wcex.hInstance);//�E�C���h�E�N���X�̓o�^������

	return(int)msg.wParam;
}

//=======================================================================================================================================================================================================================
// �E�C���h�E�v���V�[�W��
//=======================================================================================================================================================================================================================
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nID; //�Ԃ�l�i�[�p

	switch (uMsg)
	{
	case WM_DESTROY:	//�E�C���h�E�j���̃��b�Z�[�W
		PostQuitMessage(0);	//WM_QUIT���b�Z�[�W�𑗂�
		break;

	case WM_KEYDOWN:	//�L�[�����̃��b�Z�[�W
		
		switch (wParam)
		{
		case VK_ESCAPE:	//[ESC]�L�[����

			nID = MessageBox(hWnd, "�I�����܂����H", "�I�����b�Z�[�W", MB_YESNOCANCEL| MB_ICONQUESTION);	//�I�����b�Z�[�W

			if (nID == IDYES) //�͂�
			{
				DestroyWindow(hWnd);//�E�C���h�E��j������(WM_DESTROY���b�Z�[�W�𑗂�)
				break;
			}
			break;
		case VK_F11:
			ToggleFullscreen(hWnd);
			break;
		}
		break;

	case WM_CLOSE: //����{�^���̃��b�Z�[�W

		nID = MessageBox(hWnd, "�I�����܂����H", "�I�����b�Z�[�W", MB_YESNOCANCEL | MB_ICONQUESTION);	//�I�����b�Z�[�W

		if (nID == IDYES) //�͂�
		{
			DestroyWindow(hWnd);//�E�C���h�E��j������(WM_DESTROY���b�Z�[�W�𑗂�)
		}
		else
		{
			return 0; //0��Ԃ��Ȃ��ƏI��
		}
		break;

	case WM_LBUTTONDOWN:	//�}�E�X���N���b�N�̃��b�Z�[�W
		SetFocus(hWnd);	//�E�C���h�E�Ƀt�H�[�J�X�����킹��
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//=======================================================================================================================================================================================================
// ����������
//=======================================================================================================================================================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	D3DDISPLAYMODE d3ddm;			//�f�B�X�v���C���[�h
	D3DPRESENT_PARAMETERS d3dpp;	//�v���[���e�[�V�����p�����[�^

	//Direct3D�I�u�W�F�N�g�̐���
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (g_pD3D == NULL)
	{
		return E_FAIL;
	}

	//���݂̃f�B�X�v���C���[�h���擾
	if (FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	//�f�o�C�X�̃v���[���e�[�V�����p�����[�^�̐ݒ�
	ZeroMemory(&d3dpp, sizeof(d3dpp));							//�p�����[�^�̃[���N���A
	d3dpp.BackBufferWidth = SCREEN_WIDTH;						//�Q�[����ʃT�C�Y(��)
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;						//�Q�[����ʃT�C�Y(����)
	d3dpp.BackBufferFormat = d3ddm.Format;						//�o�b�N�o�b�t�@�̌`��
	d3dpp.BackBufferCount = 1;									//�o�b�N�o�b�t�@�̐�
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;					//�_�u���o�b�t�@�؂�ւ�
	d3dpp.EnableAutoDepthStencil = TRUE;						//�f�v�X�o�b�t�@�E�X�e���V�����o�b�t�@�쐬
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;					//�f�v�X�o�b�t�@��16bit��
	d3dpp.Windowed = bWindow;									//�E�C���h�E���[�h
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;	//���t���b�V�����[�g
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;	//�C���^�[�o��

	//Direct3D�f�o�C�X�̐���(�`�揈���ƒ��_�������n�[�h�E�F�A�ōs��)
	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp,
		&g_pD3DDevice)))
	{
		//Direct3D�f�o�C�X�̐���(�`�揈���̓n�[�h�E�F�A�A���_������CPU�ōs��)
		if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp,
			&g_pD3DDevice)))
		{
			//Direct3D�f�o�C�X�̐���(�`�揈���ƒ��_������CPU�ōs��)
			if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF,
				hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&d3dpp,
				&g_pD3DDevice)))
			{
				return E_FAIL;
			}
		}
	}

	//�����_�[�X�e�[�g�̐ݒ�
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//�e�N�X�`���X�e�[�W�X�e�[�g
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	//�L�[�{�[�h
	if (FAILED(InitKeyboard(hInstance, hWnd)))
	{
		return E_FAIL;
	}

	//�}�E�X
	if (FAILED(InitMouse(hInstance, hWnd)))
	{
		return E_FAIL;
	}

	//�W���C�p�b�h
	if (FAILED(InitJoypad()))
	{
		return E_FAIL;
	}

	InitLight();	//���C�g
	InitDebugProc();//�f�o�b�O�\��
	SetMode(g_mode);//���[�h�̐ݒ�

	//�f�o�b�N�\���p�t�H���g�̐���
	D3DXCreateFont(g_pD3DDevice, 18, 0, 0, 0, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Terminal", &g_pFont);

	return S_OK;
}

//=======================================================================================================================================================================================================
// �I������
//=======================================================================================================================================================================================================
void Uninit(void)
{
	UninitKeyboard();	//�L�[�{�[�h
	UninitMouse();		//�}�E�X
	UninitJoypad();		//�W���C�p�b�h
	UninitLight();		//���C�g
	UninitDebugProc();	//�f�o�b�O�\��

	//Direct3D�f�o�C�X�̔j��
	if (g_pD3DDevice != NULL)
	{
		g_pD3DDevice->Release();
		g_pD3DDevice = NULL;
	}
	//Direct3D�I�u�W�F�N�g�̔j��
	if (g_pD3D != NULL)
	{
		g_pD3D->Release();
		g_pD3D = NULL;
	}
}

//=======================================================================================================================================================================================================
// �X�V����
//=======================================================================================================================================================================================================
void Update(void)
{
	UpdateKeyboard();	//�L�[�{�[�h
	UpdateMouse();		//�}�E�X
	UpdateJoypad();		//�R���g���[���[
	UpdateStick();		//�R���g���[���[�X�e�B�b�N
	UpdateLight();		//���C�g
	UpdateDebugProc();	//�f�o�b�O�\��

	switch (g_mode)
	{
	case MODE_GAME: //�Q�[�����
		UpdateGame();
		break;

	default:
		break;
	}
}

//=======================================================================================================================================================================================================
// �`�揈��
//=======================================================================================================================================================================================================
void Draw(void)
{
	//��ʃN���A(�o�b�N�o�b�t�@��Z�o�b�t�@�̃N���A)
	g_pD3DDevice->Clear(0, NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		D3DCOLOR_RGBA(30, 144, 255, 0), 1.0f, 0);

	//�`��J�n
	if (SUCCEEDED(g_pD3DDevice->BeginScene()))
	{
		switch (g_mode)
		{
		case MODE_GAME: //�Q�[�����
			DrawGame();
			break;

		default:
			break;
		}

		DrawDebugProc();	//�f�o�b�O�\��

		g_pD3DDevice->EndScene();	//�`��I��
	}

	g_pD3DDevice->Present(NULL, NULL, NULL, NULL);	//�o�b�N�o�b�t�@�ƃt�����g�o�b�t�@����ւ�
}

//=======================================================================================================================================================================================================
// �f�o�C�X�̎擾
//=======================================================================================================================================================================================================
LPDIRECT3DDEVICE9 GetDevice(void)
{
	return g_pD3DDevice;
}

//=======================================================================================================================================================================================================
// ���[�h�̐ݒ�
//=======================================================================================================================================================================================================
void SetMode(MODE mode)
{
	//���݂̉�ʂ̏I������
	switch (g_mode)
	{
	case MODE_GAME: //�Q�[�����
		UninitGame();
		break;

	default:
		break;
	}

	g_mode = mode; //���݂̉�ʂ�؂�ւ���

	//�V������ʂ̏���������
	switch (g_mode)
	{
	case MODE_GAME: //�Q�[�����
		InitGame();
		break;

	default:
		break;
	}
}

//=======================================================================================================================================================================================================
// ���C���[�t���[���̗L����
//=======================================================================================================================================================================================================
void onwire(void)
{
	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
}

//=======================================================================================================================================================================================================
// ���C���[�t���[���̖�����
//=======================================================================================================================================================================================================
void offwire(void)
{
	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

//=======================================================================================================================================================================================================
// �E�B���h�E���t���X�N���[���ɕς��鏈��
//=======================================================================================================================================================================================================
void ToggleFullscreen(HWND hWnd)
{
	// ���݂̃E�B���h�E�X�^�C�����擾
	DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);

	if (g_isFullscreen)
	{
		// �E�B���h�E���[�h�ɐ؂�ւ�
		SetWindowLong(hWnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPos(hWnd, HWND_TOP, g_windowRect.left, g_windowRect.top,
			g_windowRect.right - g_windowRect.left, g_windowRect.bottom - g_windowRect.top,
			SWP_FRAMECHANGED | SWP_NOACTIVATE);
		ShowWindow(hWnd, SW_NORMAL);
	}
	else
	{
		// �t���X�N���[�����[�h�ɐ؂�ւ�
		GetWindowRect(hWnd, &g_windowRect);
		SetWindowLong(hWnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
		SetWindowPos(hWnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
			SWP_FRAMECHANGED | SWP_NOACTIVATE);
		ShowWindow(hWnd, SW_MAXIMIZE);
	}

	g_isFullscreen = !g_isFullscreen;
}
