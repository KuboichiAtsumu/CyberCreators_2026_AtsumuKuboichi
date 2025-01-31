//==============================================================================================================================================
//
// �����_���[�Ɋւ��鏈��
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================

//===========================================================================================================
// �w�b�_�[�C���N���[�h
//===========================================================================================================
#include "renderer.h"
#include "debugproc.h"
#include "object.h"

//===========================================================================================================
// �R���X�g���N�^
//===========================================================================================================
CRenderer::CRenderer()
{
	//�����o�ϐ�������
	m_pD3D = nullptr;
	m_pD3DDevice = nullptr;
	m_bStop = false;//�ꎞ��~�t���O
}

//===========================================================================================================
// �f�X�g���N�^
//===========================================================================================================
CRenderer::~CRenderer()
{

}

//===========================================================================================================
// ����������
//===========================================================================================================
HRESULT CRenderer::Init(HWND hWnd, BOOL bWindow)
{
	//���[�J���ϐ��錾
	D3DDISPLAYMODE d3ddm;//�f�B�X�v���C���[�h
	D3DPRESENT_PARAMETERS d3dpp;//�v���[���e�[�V�����p�����[�^
	LPD3DXFONT pFont = nullptr;//�t�H���g�ւ̃|�C���^

	//Direct3D�I�u�W�F�N�g�̐���
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_pD3D == nullptr)
	{
		return E_FAIL;
	}

	//���݂̃f�B�X�v���C���[�h���擾
	if (FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	//�f�o�C�X�̃v���[���e�[�V�����p�����[�^�̐ݒ�
	ZeroMemory(&d3dpp, sizeof(d3dpp));//�p�����[�^�̃[���N���A
	d3dpp.BackBufferWidth = static_cast<UINT>(SCREEN_WIDTH);//�Q�[����ʃT�C�Y(����)
	d3dpp.BackBufferHeight = static_cast<UINT>(SCREEN_HEIGHT);//�Q�[����ʃT�C�Y(�c��)
	d3dpp.BackBufferFormat = d3ddm.Format;//�o�b�N�o�b�t�@�̌`��
	d3dpp.BackBufferCount = 1;//�o�b�N�o�b�t�@�̐�
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;//�_�u���o�b�t�@�؂�ւ�
	d3dpp.EnableAutoDepthStencil = TRUE;//�f�v�X�o�b�t�@�E�X�e���V�����o�b�t�@�쐬
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;//�f�v�X�o�b�t�@��24bit��
	d3dpp.Windowed = bWindow;//�E�C���h�E���[�h
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;//���t���b�V�����[�g
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;//�C���^�[�o��

	//Direct3D�f�o�C�X�̐���(�`�揈���ƒ��_�������n�[�h�E�F�A�ōs��)
	if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp,
		&m_pD3DDevice)))
	{
		//Direct3D�f�o�C�X�̐���(�`�揈���̓n�[�h�E�F�A�A���_������CPU�ōs��)
		if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp,
			&m_pD3DDevice)))
		{
			//Direct3D�f�o�C�X�̐���(�`�揈���ƒ��_������CPU�ōs��)
			if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF,
				hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&d3dpp,
				&m_pD3DDevice)))
			{
				return E_FAIL;
			}
		}
	}

	//�����_�[�X�e�[�g�̐ݒ�
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//�e�N�X�`���X�e�[�W�X�e�[�g
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	//�f�o�b�O�\��
	InitDebugProc();

	//�f�o�b�N�\���p�t�H���g�̐���
	D3DXCreateFont(m_pD3DDevice, 18, 0, 0, 0, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Terminal", &pFont);

	return S_OK;
}

//===========================================================================================================
// �I������
//===========================================================================================================
void CRenderer::Uninit()
{
	//�S�I�u�W�F�N�g�I������
	CObject::UninitAll();

	//�f�o�b�O�\��
	UninitDebugProc();
	
	//Direct3D�f�o�C�X�̔j��
	if (m_pD3DDevice != nullptr)
	{
		m_pD3DDevice->Release();
		m_pD3DDevice = nullptr;
	}
	//Direct3D�I�u�W�F�N�g�̔j��
	if (m_pD3D != nullptr)
	{
		m_pD3D->Release();
		m_pD3D = nullptr;
	}
}

//===========================================================================================================
// �X�V����
//===========================================================================================================
void CRenderer::Update()
{
	//�f�o�b�O���̂�
#ifdef _DEBUG
	//�f�o�b�O�\��
	UpdateDebugProc();

	//P�L�[�������ꂽ�ꍇ
	if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_P))
	{
		//�ꎞ��~�t���O��؂�ւ���
		m_bStop = m_bStop ? false : true;
	}
#endif // _DEBUG

	//�ꎞ��~���ł͂Ȃ��ꍇ
	if (!m_bStop)
	{
		//�S�I�u�W�F�N�g�X�V����
		CObject::UpdateAll();
	}
}

//===========================================================================================================
// �`�揈��
//===========================================================================================================
void CRenderer::Draw()
{
	//��ʃN���A(�o�b�N�o�b�t�@��Z�o�b�t�@�̃N���A)
	m_pD3DDevice->Clear(0, nullptr,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

	//�`��J�n
	if (SUCCEEDED(m_pD3DDevice->BeginScene()))
	{
		//�V�[��
		CManager::GetInstance()->GetScene()->Draw();

		//�S�Q�[���I�u�W�F�N�g
		CObject::DrawAll();

		//�t�F�[�h
		CManager::GetInstance()->GetFade()->Draw();

		//�f�o�b�O���̂�
#ifdef _DEBUG
		//�f�o�b�O�\��
		DrawDebugProc();
#endif // _DEBUG
		
		//�`��I��
		m_pD3DDevice->EndScene();
	}

	//�o�b�N�o�b�t�@�ƃt�����g�o�b�t�@����ւ�
	m_pD3DDevice->Present(nullptr, nullptr, nullptr, nullptr);
}

//===========================================================================================================
// 3D�f�o�C�X�̎擾
//===========================================================================================================
LPDIRECT3DDEVICE9 CRenderer::GetDevice()
{
	return m_pD3DDevice;
}

//===========================================================================================================
// ���C���[�t���[���̗L����
//===========================================================================================================
void CRenderer::OnWire()
{
	m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
}

//===========================================================================================================
// ���C���[�t���[���̖�����
//===========================================================================================================
void CRenderer::OffWire()
{
	m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

//===========================================================================================================
// �t�H�O�̗L����
//===========================================================================================================
void CRenderer::OnFog()
{
	//�t�H�O��L����
	m_pD3DDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);

	//�t�H�O�J���[�ݒ�
	m_pD3DDevice->SetRenderState(D3DRS_FOGCOLOR, D3DXCOLOR(0.0f, 0.0f, 0.3f, 0.1f));

	//�t�H�O�̃p�����[�^�ݒ�(�͈͎w��)
	m_pD3DDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);

	//�n�_�ݒ�
	float fFogStart = 1000.0f;
	m_pD3DDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD*)(&fFogStart));

	//�I�_�ݒ�
	float fFogEnd = 5000.0f;
	m_pD3DDevice->SetRenderState(D3DRS_FOGEND, *(DWORD*)(&fFogEnd));
}

//===========================================================================================================
// �t�H�O�̖�����
//===========================================================================================================
void CRenderer::OffFog()
{
	//�t�H�O�𖳌���
	m_pD3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
}

