#include "Tetris_TS.h"
#define _CRT_SECURE_NO_WARNINGS 1
#pragma warning(disable:4996)

Tetris_TS::Tetris_TS() : Parent(WIN_WIDTH, WIN_HEIGHT)
{
	// 4x4 size shapes
	mShapes[0].append(".X...XX...X.....");	/*.X..
											  .XX.
											  ..X.
											  ....*/

	mShapes[1].append("..X...X...X...X.");	/*..X.
											  ..X.
											  ..X.
											  ..X.*/
	mShapes[2].append(".....XX..XX.....");	/*....
											  .XX.
											  .XX.
											  ....*/
	mShapes[3].append(".X...X...XX.....");  /*.X..
											  .X..
											  .XX.
											  ....*/
	mShapes[4].append(".X...XX..X......");	/*.X..
											  .XX.
											  .X..
											  ....*/
	mShapes[5].append("......X...X..XX.");	/*....
											  ..X.
											  ..X.
											  .XX.*/
	srand(time(0));
	mCollision = mDownForce = mGameOver = false;
	mCurrentPiece = rand() % 5;
	mNextPiece = (rand() +rand()/7 ) % 5;
	mNextOrient = (rand() + 2018) % 4;
	rotation = rand() % 4;
	mScore =0;
	mCurY = 1;
	mCurX = 7;
}

void Tetris_TS::KeyPressed(int btnCode) {
	switch (btnCode)
	{
	case 80: //Down
		if (!CollisionTest(mCurX, mCurY + 1)) {
			mCurY++;
			mDownForce = true;
		}
		else
			mDownForce = false;
		break;
	case 75: //Left
		if (!CollisionTest(mCurX - 1, mCurY))
			mCurX--;
		break;
	case 77: //Right
		if (!CollisionTest(mCurX + 1, mCurY))
			mCurX++;
		break;
	case 32: //Space
		if (!CollisionTest(mCurX, mCurY))
			((rotation += 1) % 4);
		break;
	default:

		break;
	}
}

void Tetris_TS::UpdateF(float deltaTime) {
	if (!mGameOver) {
		Draw();
		static float sum = 0;
		sum += deltaTime;
		if (mDownForce) {
			KeyPressed(80);
		}
		if (!mDownForce)
			Sleep(40); //  Increase Game Tick, decrese if down arrow pressed
		else
			Sleep(50);
		if (!CollisionTest(mCurX, mCurY + 1) && (sum > 0.4))
		{
			mCurY++;
			sum = 0;
		}
	}
	else
		exit(0);
}
int  Tetris_TS::Rotate(int sx, int sy, int angle) {
	int si(0); //block index
	switch (angle % 4) {
	case 0: // 0 degrees		
		si = sy * 4 + sx;				// 0  1  2  3
		break;							// 4  5  6  7
										// 8  9 10 11
										//12 13 14 15
	case 1: // 90 degrees		
		si = 12 + sy - (sx * 4);		//12  8  4  0
		break;							//13  9  5  1
										//14 10  6  2
										//15 11  7  3
	case 2: // 180 degrees		
		si = 15 - (sy * 4) - sx;		//15 14 13 12
		break;							//11 10  9  8
										// 7  6  5  4
										// 3  2  1  0
	case 3: // 270 degrees		
		si = 3 - sy + (sx * 4);			// 3  7 11 15
		break;							// 2  6 10 14
	}									// 1  5  9 13
	return si;							// 0  4  8 12
}

bool Tetris_TS::CollisionTest(int nx, int ny) {

	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++)
		{
			int si = Rotate(x, y, rotation); //index of the piece
			if (nx + x >= 0 && nx + x < WIN_WIDTH - 13)
			{
				if (ny + y >= 0 && ny + y < WIN_HEIGHT)
				{
					GetChar(nx + x, (ny + y));
					// Inside shape bounds collision check
					if ((mShapes[mCurrentPiece][si] == 'X') && (buffer[ny + y][nx + x] != '.')) {
						if (ny == 1)	// end game
							mGameOver = true;
						if ((mShapes[mCurrentPiece][si] == 'X') && (buffer[ny + y][nx + x] == '#') && (ny + y != WIN_HEIGHT - 1))
						{
							return true;
						}
						mCollision = true;
						return true; // collision detected
					}
				}
			}
		}
	}
	return false;
}

void Tetris_TS::Draw(int completeLine) {
	wchar_t score[7] = L"score:";
	wchar_t iscore[7] = L"0";
	static bool firstIteration = true; //start reading from buffer flag
	_itow(mScore, iscore, 10);

	//if complete line found
	if (completeLine >= 0) {
		for (int x = 1; x < 16; x++)
			buffer[completeLine][x] = '.';
		for (int y = completeLine - 1; y > 0; y--)
			for (int x = 1; x < 16; x++) {
				if (buffer[y][x] == 'X')
				{
					buffer[y + 1][x] = buffer[y][x];
					buffer[y][x] = '.';
				}
			}
	}
	//Draw score//
	for (int i = 0; i < 7; i++) {
		buffer[1][18 + i] = score[i];
		if (iswdigit(iscore[i]))
			buffer[2][25 + i] = iscore[i];
	}

	//Display next piece
	for (int y = 0; y < 4; y++)
		for (int x = 0; x < 4; x++)
			buffer[7 + y][WIN_HEIGHT + x] = ((mShapes[mNextPiece][Rotate(x, y, mNextOrient)] == '.') ? ' ' : 'X');

	//Draw field//
	if (firstIteration) { // one-time iteration
		for (int y = 0; y < WIN_HEIGHT; y++)
			for (int x = 0; x < WIN_WIDTH - 13; x++)
				if (x == 0 || x == 16 || y == 21) // boarders
					SetChar(x, y, '#');
				else
					SetChar(x, y, '.');
		mCollision = true;
	}
	else {	//Draw from buffer
		for (int y = 0; y < WIN_HEIGHT; y++)
			for (int x = 0; x < WIN_WIDTH; x++)
				SetChar(x, y, buffer[y][x]);
	}
	//Draw piece//
	if (!firstIteration) // dont draw at first iteration
		for (int y = 0; y < 4; y++) {
			for (int x = 0; x < 4; x++)
			{
				if ((mShapes[mCurrentPiece][Rotate(x, y, rotation)] == '.') && (GetChar(mCurX + x, mCurY + y) == '#'))
					SetChar(mCurX + x, mCurY + y, '#');
				else
					if ((mShapes[mCurrentPiece][Rotate(x, y, rotation)] == '.') && (GetChar(mCurX + x, mCurY + y) == 'X'))
						SetChar(mCurX + x, mCurY + y, 'X');
					else
						SetChar(mCurX + x, mCurY + y, ((mShapes[mCurrentPiece][Rotate(x, y, rotation)] == '.') ? '.' : 'X'));
			}
		}

	//Lock pieces if collision detected and create a new piece
	if (mCollision) {
		for (int y = 0; y < WIN_HEIGHT; y++)
			for (int x = 0; x < WIN_WIDTH; x++)
				buffer[y][x] = GetChar(x, y);
		mCollision = mDownForce = false;
		mCurY = 0;
		mCurX = 7;
		rotation = mNextOrient;
		mCurrentPiece = mNextPiece;
		srand(time(0));
		mNextOrient = rand() % 4;
		mNextPiece = rand() % 5;
		firstIteration = false;
		LineCheck();
	}
}

void Tetris_TS::LineCheck() {
	int lines(0), counter(0);
	for (int y = 0; y < WIN_HEIGHT; y++) {
		for (int x = 0; x < 16; x++) {
			if (GetChar(x, y) == 'X')
				counter++;
		}
		if (counter == 15)
		{
			lines++;
			Draw(y);
		}
		counter = 0;
	}
	mScore += lines * 100;
}