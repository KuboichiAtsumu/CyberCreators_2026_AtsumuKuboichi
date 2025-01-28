//=======================================================================================================================================================================================================================
//
// �t�B�[���h�̏���
// Author : Atsumu Kuboichi
//
//=======================================================================================================================================================================================================================
#include "field.h"

#define TEST_MESHWALL	(0)

//=======================================================================================================================================================================================================================
// �O���[�o���ϐ�
//=======================================================================================================================================================================================================================
LPDIRECT3DTEXTURE9 g_pTextureField = NULL;		//�e�N�X�`���ւ̃|�C���^
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffField = NULL;	//���_�o�b�t�@�ւ̃|�C���^
LPDIRECT3DINDEXBUFFER9 g_pldxBuffField = NULL;	//�C���f�b�N�X�o�b�t�@�ւ̃|�C���^
Field g_field;									//���b�V���t�B�[���h�̏��

//============================================================================================================================================================================================================
// ����������
//============================================================================================================================================================================================================
void InitField(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	//�f�o�C�X�̎擾
	VERTEX_3D* pVtx;							//���_���ւ̃|�C���^
	WORD* pldx;									//�C���f�b�N�X���ւ̃|�C���^

	//���_�o�b�t�@�̐���
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * FIELD_VER,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffField,
		NULL);

	//�e�N�X�`���̓ǂݍ���
	D3DXCreateTextureFromFile(pDevice,
		"data\\TEXTURE\\field000.jpg",
		&g_pTextureField);

	//�C���f�b�N�X�o�b�t�@�̐���
	pDevice->CreateIndexBuffer(sizeof(WORD) * FIELD_INDEX,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&g_pldxBuffField,
		NULL);

	//���̏�����
	g_field.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		//�ʒu
	g_field.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		//����
	g_field.col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	//�F

	//���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	g_pVtxBuffField->Lock(0, 0, (void**)&pVtx, 0);

	//�C���f�b�N�X�o�b�t�@�����b�N���A�C���f�b�N�X���ւ̃|�C���^���擾
	g_pldxBuffField->Lock(0, 0, (void**)&pldx, 0);

	int nCntX = 0;	//X���̒��_��
	int nCntZ = 0;	//Z���̒��_��
	
	// ���ʂɔz�u
	for (int nCntField = 0; nCntField < FIELD_VER; nCntField++)
	{
		//���_���W�̐ݒ�
		pVtx[nCntField].pos = D3DXVECTOR3(FIELD_RADIUS * (nCntX / (float)(NUM_BLOCK_X)) - (FIELD_RADIUS * 0.5f), 0.0f, -FIELD_RADIUS * (nCntZ / (float)(NUM_BLOCK_Z)) + (FIELD_RADIUS * 0.5f));

		//�e�N�X�`�����W�̐ݒ�
		pVtx[nCntField].tex = D3DXVECTOR2(1.0f * (nCntX / (float)(NUM_BLOCK_X)), 1.0f * (nCntZ / (float)(NUM_BLOCK_Z)));

		nCntX++;

		if (nCntX == NUM_BLOCK_X + 1)
		{
			nCntX = 0;
			nCntZ++;
		}

		//�@���x�N�g���̐ݒ�
		pVtx[nCntField].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		//���_�J���[�̐ݒ�
		pVtx[nCntField].col = g_field.col;
	}

	//�C���f�b�N�X�̐ݒ�
	for (int nCntIndex = 0; nCntIndex < NUM_BLOCK_Z; nCntIndex++)
	{
		//Z�̃u���b�N�����z������Z
		int nCntZ2 = nCntIndex * ((NUM_BLOCK_X + 1) * 2 + 2);

		//Z�̃u���b�N�����f�[�^�����Z
		int nCntX2 = nCntIndex * (NUM_BLOCK_X + 1);

		for (int nCntIndex2 = 0; nCntIndex2 < NUM_BLOCK_X + 1; nCntIndex2++)
		{
			pldx[nCntIndex2 * 2 + nCntZ2] = (WORD)(nCntIndex2 + NUM_BLOCK_X + 1 + nCntX2);
			pldx[nCntIndex2 * 2 + 1 + nCntZ2] = (WORD)(nCntIndex2 + nCntX2);
		}

		//�k�ނ̌v�Z
		if (nCntIndex != NUM_BLOCK_Z - 1)
		{
			pldx[(NUM_BLOCK_X + 1) * 2 + 1 + nCntZ2] = pldx[(NUM_BLOCK_X + 1) * 2 + 1 + nCntZ2];
			pldx[(NUM_BLOCK_X + 1) * 2 + 2 + nCntZ2] = (WORD)(NUM_BLOCK_X + 1 + (NUM_BLOCK_X + 1) * (nCntIndex + 1));
		}
	}

	//���_�o�b�t�@���A�����b�N����
	g_pVtxBuffField->Unlock();

	//�C���f�b�N�X�o�b�t�@���A�����b�N����
	g_pldxBuffField->Unlock();
}

//============================================================================================================================================================================================================
// �I������
//============================================================================================================================================================================================================
void UninitField(void)
{
	//�e�N�X�`���̔j��
	if (g_pTextureField != NULL)
	{
		g_pTextureField->Release();
		g_pTextureField = NULL;
	}
	//���_�o�b�t�@�̔j��
	if (g_pVtxBuffField != NULL)
	{
		g_pVtxBuffField->Release();
		g_pVtxBuffField = NULL;
	}
	//�C���f�b�N�X�o�b�t�@�̉��
	if (g_pldxBuffField != NULL)
	{
		g_pldxBuffField->Release();
		g_pldxBuffField = NULL;
	}
}

//============================================================================================================================================================================================================
// �`�揈��
//============================================================================================================================================================================================================
void DrawField(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	//�f�o�C�X�̎擾
	D3DXMATRIX mtxRot, mtxTrans;				//�v�Z�p�}�g���b�N�X
	D3DXMATRIX mtxView;							//�r���[�}�g���b�N�X�擾�p

	for (int nCntCylinder = 0; nCntCylinder < NUM_BLOCK_X; nCntCylinder++)
	{
		//���[���h�}�g���b�N�X�̏�����
		D3DXMatrixIdentity(&g_field.mtxWorld);

		//�����𔽉f
		D3DXMatrixRotationYawPitchRoll(&mtxRot, g_field.rot.y, g_field.rot.x, g_field.rot.z);
		D3DXMatrixMultiply(&g_field.mtxWorld, &g_field.mtxWorld, &mtxRot);

		//�ʒu�𔽉f
		D3DXMatrixTranslation(&mtxTrans, g_field.pos.x, g_field.pos.y, g_field.pos.z);
		D3DXMatrixMultiply(&g_field.mtxWorld, &g_field.mtxWorld, &mtxTrans);

		//���[���h�}�g���b�N�X�̐ݒ�
		pDevice->SetTransform(D3DTS_WORLD, &g_field.mtxWorld);

		//���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
		pDevice->SetStreamSource(0, g_pVtxBuffField, 0, sizeof(VERTEX_3D));

		//�C���f�b�N�X�o�b�t�@���f�[�^�X�g���[���ɐݒ�
		pDevice->SetIndices(g_pldxBuffField);

		//���_�t�H�[�}�b�g�̐ݒ�
		pDevice->SetFVF(FVF_VERTEX_3D);

		//�e�N�X�`���̐ݒ�
		pDevice->SetTexture(0, g_pTextureField);

		//�|���S���̕`��
		pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, FIELD_VER, 0, FIELD_POLYGON);
	}
}

//============================================================================================================================================================================================================
// ���̎擾
//============================================================================================================================================================================================================
Field* GetField(void)
{
	return &g_field;
}