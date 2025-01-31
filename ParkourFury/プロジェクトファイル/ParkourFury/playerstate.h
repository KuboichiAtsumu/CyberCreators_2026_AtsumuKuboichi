//==============================================================================================================================================
//
// プレイヤーの状態に関する処理のヘッダーファイル
// Author : Atsumu Kuboichi
//
//==============================================================================================================================================
#ifndef _PLAYERSTATE_H_ //このマクロ定義がされなかったら
#define _PLAYERSTATE_H_ //2重インクルード防止のマクロ定義

//===========================================================================================================
// クラス定義
//===========================================================================================================

//基底クラス
class CPlayerState
{
public:
	CPlayerState() {};
	~CPlayerState() {};
};

//ニュートラル
class CNeutral : public CPlayerState
{
public:
	CNeutral() {};
	~CNeutral() {};
};

//地上移動
class CLandingMove : public CPlayerState
{
public:
	CLandingMove() {};
	~CLandingMove() {};
};

//一段ジャンプ
class CJump : public CPlayerState
{
public:
	CJump() {};
	~CJump() {};
};

//二段ジャンプ
class CDoubleJump : public CPlayerState
{
public:
	CDoubleJump() {};
	~CDoubleJump() {};
};

//落下
class CFall : public CPlayerState
{
public:
	CFall() {};
	~CFall() {};
};

//壁ずり
class CWallSlide : public CPlayerState
{
public:
	CWallSlide() {};
	~CWallSlide() {};
};

//壁キック
class CWallKick : public CPlayerState
{
public:
	CWallKick() {};
	~CWallKick() {};
};

//ダッシュ
class CDash : public CPlayerState
{
public:
	CDash() {};
	~CDash() {};
};

//崖掴まり
class CCliffGrab : public CPlayerState
{
public:
	CCliffGrab() {};
	~CCliffGrab() {};
};

#endif