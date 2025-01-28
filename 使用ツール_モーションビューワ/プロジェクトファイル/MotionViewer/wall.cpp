//=======================================================================================================================================================================================================================
//
// �ǂɊւ��鏈��
// Author : Atsumu Kuboichi
//
//=======================================================================================================================================================================================================================
#include "wall.h"
#include "field.h"

//=======================================================================================================================================================================================================================
//�O���[�o���ϐ�
//=======================================================================================================================================================================================================================
LPDIRECT3DTEXTURE9 g_pTextureWall = NULL;		//�e�N�X�`���ւ̃|�C���^
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffWall = NULL;	//���_�o�b�t�@�ւ̃|�C���^
D3DXMATRIX g_mtxWorldWall;						//���[���h�}�g���b�N�X
Wall g_Wall[MAX_WALL];							//�ǂ̏��

//============================================================================================================================================================================================================
// ����������
//============================================================================================================================================================================================================
void InitWall(void)
{
	VERTEX_3D* pVtx;							//���_���ւ̃|�C���^
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	//�f�o�C�X�̎擾
	Field* pField = GetField();					//�t�B�[���h�̏����擾

	//�e�N�X�`���̓ǂݍ���
	D3DXCreateTextureFromFile(pDevice,
		"data\\TEXTURE\\wall000.png",
		&g_pTextureWall);

	//���_�o�b�t�@�̐���
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * MAX_VER * MAX_WALL,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffWall,
		NULL);

	//���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	g_pVtxBuffWall->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntWall = 0; nCntWall < MAX_WALL; nCntWall++)
	{
		//���̏�����
		g_Wall[nCntWall].rot = D3DXVECTOR3(0.0f, nCntWall * (D3DX_PI / 2.0f), 0.0f);	//����
		g_Wall[nCntWall].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);						//�F
		g_Wall[nCntWall].nCnt = nCntWall;												//����

		//�ʒu
		g_Wall[0].pos = D3DXVECTOR3(pField->pos.x,						 pField->pos.y, pField->pos.z + FIELD_RADIUS / 2.0f);
		g_Wall[1].pos = D3DXVECTOR3(pField->pos.x + FIELD_RADIUS / 2.0f, pField->pos.y, pField->pos.z);
		g_Wall[2].pos = D3DXVECTOR3(pField->pos.x,						 pField->pos.y, pField->pos.z - FIELD_RADIUS / 2.0f);
		g_Wall[3].pos = D3DXVECTOR3(pField->pos.x - FIELD_RADIUS / 2.0f, pField->pos.y, pField->pos.z);

		//���_���W�̐ݒ�
		pVtx[0].pos = D3DXVECTOR3(-FIELD_RADIUS / 2.0f, WALL_HEIGHT, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(+FIELD_RADIUS / 2.0f, WALL_HEIGHT, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-FIELD_RADIUS / 2.0f, 0.0f, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(+FIELD_RADIUS / 2.0f, 0.0f, 0.0f);

		//�@���x�N�g���̐ݒ�
		pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

		//���_�J���[�̐ݒ�
		pVtx[0].col = g_Wall[nCntWall].col;
		pVtx[1].col = g_Wall[nCntWall].col;
		pVtx[2].col = g_Wall[nCntWall].col;
		pVtx[3].col = g_Wall[nCntWall].col;

		//�e�N�X�`�����W�̐ݒ�
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtx += MAX_VER;
	}

	//���_�o�b�t�@���A�����b�N����
	g_pVtxBuffWall->Unlock();
}

//============================================================================================================================================================================================================
// �I������
//============================================================================================================================================================================================================
void UninitWall(void)
{
	//�e�N�X�`���̔j��
	if (g_pTextureWall != NULL)
	{
		g_pTextureWall->Release();
		g_pTextureWall = NULL;
	}

	//���_�o�b�t�@�̔j��
	if (g_pVtxBuffWall != NULL)
	{
		g_pVtxBuffWall->Release();
		g_pVtxBuffWall = NULL;
	}
}

//============================================================================================================================================================================================================
// �`�揈��
//============================================================================================================================================================================================================
void DrawWall(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	//�f�o�C�X�̎擾
	D3DXMATRIX mtxRot, mtxTrans;				//�v�Z�p�}�g���b�N�X

	for (int nCntWall = 0; nCntWall < MAX_WALL; nCntWall++)
	{
		//���[���h�}�g���b�N�X�̏�����
		D3DXMatrixIdentity(&g_mtxWorldWall);

		//�����𔽉f
		D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Wall[nCntWall].rot.y, g_Wall[nCntWall].rot.x, g_Wall[nCntWall].rot.z);
		D3DXMatrixMultiply(&g_mtxWorldWall, &g_mtxWorldWall, &mtxRot);

		//�ʒu�𔽉f
		D3DXMatrixTranslation(&mtxTrans, g_Wall[nCntWall].pos.x, g_Wall[nCntWall].pos.y, g_Wall[nCntWall].pos.z);
		D3DXMatrixMultiply(&g_mtxWorldWall, &g_mtxWorldWall, &mtxTrans);

		//���[���h�}�g���b�N�X�̐ݒ�
		pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldWall);

		//���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
		pDevice->SetStreamSource(0, g_pVtxBuffWall, 0, sizeof(VERTEX_3D));

		//���_�t�H�[�}�b�g�̐ݒ�
		pDevice->SetFVF(FVF_VERTEX_3D);

		//�e�N�X�`���̐ݒ�
		pDevice->SetTexture(0, g_pTextureWall);

		//�|���S���̕`��
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntWall * MAX_VER, 2);
	}
}

//============================================================================================================================================================================================================
//�����擾
//============================================================================================================================================================================================================
Wall* GetWall(void)
{
	return &g_Wall[0];
}
