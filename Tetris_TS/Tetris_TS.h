#pragma once
#include "BaseApp.h"
#include <thread>
#include <stdlib.h>

#define WIN_HEIGHT 22
#define WIN_WIDTH  30
#define SHAPE_NUM  6


class Tetris_TS : public BaseApp
{
	typedef BaseApp Parent;
	string mShapes[SHAPE_NUM] ;
	wchar_t buffer[WIN_HEIGHT][WIN_WIDTH];
	bool mCollision;
	bool mDownForce; // Down arrow
	bool mGameOver;
	int mCurrentPiece;
	int mNextPiece;
	int mNextOrient;
	int mCurX; //Current column
	int mCurY; //Current line
	int mScore;
	int rotation;	
public:
	Tetris_TS();
	virtual void KeyPressed(int btnCode);
	virtual void UpdateF(float deltaTime);
	int  Rotate(int sx, int sy, int angle); // returns the rotated coordinate
	bool CollisionTest(int nx, int ny, int rot=-1);
	void LineCheck();
	void Draw();
	void InitBuffer();
	void UpdateScore();
	void UpdateNPiece();
	void WriteToBuffer();
};

