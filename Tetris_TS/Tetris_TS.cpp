#include "Tetris_TS.h"
#define _CRT_SECURE_NO_WARNINGS 1
#pragma warning(disable:4996)
wchar_t SCORE[7] = L"Score:";
wchar_t ISOCRE[7] = L"0";
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
	memset(buffer, 0, sizeof(buffer));
	srand(time(0));
	mCollision = mDownForce = mGameOver = false;
	mCurrentPiece = rand() % 5;
	mNextPiece = (rand() + rand() / 7) % 5;
	mNextOrient = (rand() + 2018) % 4;
	rotation = rand() % 4;
	mScore = 0;
	mCurY = 1;
	mCurX = 7;
	//Initial buffer
	for (int y = 0; y < WIN_HEIGHT; y++)
		for (int x = 0; x < WIN_WIDTH - 13; x++)
			if (x == 0 || x == 16 || y == 21) // 22*15 boarders
				SetChar(x, y, '#');
			else
				SetChar(x, y, '.');
	WriteToBuffer();
	UpdateScore();
	UpdateNPiece();
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
		if (!CollisionTest(mCurX, mCurY, ((rotation + 1) % 4)))
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

bool Tetris_TS::CollisionTest(int nx, int ny, int rot) {
	int si(0);
	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++)
		{
			if (rot>=0)
				 si = Rotate(x, y, rotation); //index of the piece
			else 
				 si = Rotate(x, y, rotation);
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
						if ((mShapes[mCurrentPiece][si] == 'X') && ((buffer[ny + y][nx + x] == '.')))
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

void Tetris_TS::Draw() {

	//Write buffer to console
	for (int y = 0; y < WIN_HEIGHT; y++)
		for (int x = 0; x < WIN_WIDTH; x++)
			SetChar(x, y, buffer[y][x]);

	//Draw next tetris piece//
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

	//Lock pieces if collision detected and create a new one
	if (mCollision) {
		WriteToBuffer(); // Save scene to buffer
		mCollision = mDownForce = false;
		mCurY = 0;
		mCurX = 7;
		rotation = mNextOrient;
		mCurrentPiece = mNextPiece;
		srand(time(0));
		mNextOrient = rand() % 4;
		mNextPiece = rand() % 5;
		UpdateNPiece();
		LineCheck(); // check for complete lines
	}
}

void Tetris_TS::LineCheck() {
	int oldscore = mScore;
	int lines(0), counter(0);
	for (int y = 0; y < WIN_HEIGHT; y++) {
		for (int x = 0; x < 16; x++) {
			if (GetChar(x, y) == 'X')
				counter++;
		}
		if (counter == 15)
		{
			lines++;
			//if a completed line found
				for (int x = 1; x < 16; x++)
					buffer[y][x] = '.';
				for (int i = y - 1; i > 0; i--)
					for (int x = 1; x < 16; x++) {
						if (buffer[i][x] == 'X')
						{
							buffer[i + 1][x] = buffer[i][x];
							buffer[i][x] = '.';
						}
					}
		}
		counter = 0;
	}
	mScore += lines * 100;
	if (oldscore < mScore)
	{
		UpdateScore();
	}
}

void Tetris_TS::UpdateScore(){
	_itow(mScore, ISOCRE, 10);
	for (int i = 0; i < 7; i++) {
		buffer[1][18 + i] = SCORE[i];
		if (iswdigit(ISOCRE[i]))
			buffer[2][18 + i] = ISOCRE[i];
	}
}

void Tetris_TS::UpdateNPiece(){
	//Update next piece
	for (int y = 0; y < 4; y++)
		for (int x = 0; x < 4; x++)
			buffer[7 + y][WIN_HEIGHT + x] = ((mShapes[mNextPiece][Rotate(x, y, mNextOrient)] == '.') ? ' ' : 'X');
}

void Tetris_TS::WriteToBuffer(){
	for (int y = 0; y < WIN_HEIGHT; y++)
		for (int x = 0; x < WIN_WIDTH; x++)
			buffer[y][x] = GetChar(x, y);
}
