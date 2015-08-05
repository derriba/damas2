//play.cpp

#include "StdAfx.h"
#include "play.h"
#include "Checkers.h"

//--------------------------------------------------------------------------
//
// This file contains the implementation of the class CPlay
//
//--------------------------------------------------------------------------
// Programmer  : Sergey Borovik
// Email	   : sbsergey@hotmail.com
// Date        : 27/04/04
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//                 				  Constructor
//--------------------------------------------------------------------------
CPlay::CPlay(void):m_Turn(BLUE)
				   
{
}
//--------------------------------------------------------------------------
//      				          Destructor
//--------------------------------------------------------------------------
CPlay::~CPlay(void)
{
}

//Acsess methods

int CPlay::GetTurn() 
{  return m_Turn;  }
	
int CPlay::IsChecker(int x,int y)
{  return (m_Board[y+WALL_SIZE][x+WALL_SIZE]);  }

void CPlay::GetNumCheckers(int &red, int &blue)
{
	red = numRedCheckers;
	blue = numBlueCheckers;
}
//Modify methods

void CPlay::SetTurn(int _turn) 
{  m_Turn = _turn;  }

void CPlay::SetActiveChecker(int x, int y)
{
	m_Active.x = x;
	m_Active.y = y;
}

void CPlay::ChangeTurn() 
{  m_Turn = -m_Turn;  }

void CPlay::IncCheckersNum()
{ 
	if(m_Turn == BLUE)
		numRedCheckers++;
	else numBlueCheckers++;
}

//Other methods

bool CPlay::IsGameOver()
{
	return ((numRedCheckers == MAX_CHECKERS) || 
		    (numBlueCheckers == MAX_CHECKERS) );
}

//--------------------------------------------------------------------------
//				   Start
//				   -----
//	General      : Initializes all the class members
//                 (including setting checkers)
//	Parameters   : None
//  Return Value : None
//--------------------------------------------------------------------------
void CPlay::Start()
{
	int i,j;
	int redCount,blueCount;
	for (redCount = 0, blueCount = 0,i = 0; i<GAMEBOARD_SIZE; i++)
		for (j = 0; j<GAMEBOARD_SIZE; j++)
		{
			if(i>=GAMEBOARD_SIZE-WALL_SIZE || j>=GAMEBOARD_SIZE-WALL_SIZE ||
			   i < WALL_SIZE || j < WALL_SIZE)
				m_Board[i][j] = WALL;
			else if(i<3+WALL_SIZE)
			{
				if((i+j)%2)
				{
					pRedCheckers[redCount].x = j;
					pRedCheckers[redCount++].y = i;
				}
				m_Board[i][j] = ((i+j)%2);
			}
			else if(i>BOARD_SIZE-2)
			{
				if((i+j)%2)
				{
					pBlueCheckers[blueCount].x = j;
					pBlueCheckers[blueCount++].y = i;
				}
				m_Board[i][j] = -1*(i+j)%2;
			}
			else
				m_Board[i][j] = 0;
		}	

	ClearPossibleMoves();
	m_Active.x = NONE;
	m_Active.y = NONE;
	m_MustJump = 0;
	numBlueCheckers = 0;
	numRedCheckers = 0;
}

//--------------------------------------------------------------------------
//				   ClearPossibleMoves
//				   ------------------
//	General      : Clears the moves array
//	Parameters   : None
//  Return Value : None
//--------------------------------------------------------------------------
inline void CPlay::ClearPossibleMoves()
{
	for (int i = 0; i<MAX_MOVES; i++)
	{
		m_Moves[i].x = NONE;
		m_Moves[i].y = NONE;
	}
}
//--------------------------------------------------------------------------
//				   DrawCheckers
//				   ------------
//	General      : Draws checkers on the screen
//	Parameters   : opp1 - HDC (device context)
//  Return Value : None
//--------------------------------------------------------------------------
void CPlay::DrawCheckers(HDC& hdc)
{
	CHECKERS brushFlag = REDCHECKER;

	//creating brushes for coloring
	HBRUSH red,blue;
	red = CreateSolidBrush(RGB(200,30,30));
	blue = CreateSolidBrush(RGB(80,80,250));
	SelectObject(hdc,red);

	//scanning the matrix
	for(int i = WALL_SIZE - 1; i<BOARD_SIZE+WALL_SIZE; i++)
		for (int j = WALL_SIZE - 1; j<BOARD_SIZE+WALL_SIZE; j++)
		if(m_Board[i][j]!= EMPTY)
		{
			if(m_Board[i][j] == BLUECHECKER || m_Board[i][j] == BLUEKING)
			{
				if(brushFlag == REDCHECKER)
				{
					//pick blue color				
					brushFlag = BLUECHECKER;
					SelectObject(hdc,blue);
				}
			}
			else
				if(brushFlag == BLUECHECKER)
				{
					//pick red color
					brushFlag = REDCHECKER;
					SelectObject(hdc,red);
				}
			
			//draw checker
			Ellipse(hdc,(j-WALL_SIZE)*SQUARE_SIZE,(i-WALL_SIZE)*SQUARE_SIZE,
						 SQUARE_SIZE+(j-WALL_SIZE)*SQUARE_SIZE,SQUARE_SIZE+
												 (i-WALL_SIZE)*SQUARE_SIZE);
			
			//draw smaller ellipse in middle if king
			if(m_Board[i][j]%2 == 0)    
				Ellipse(hdc,(j-WALL_SIZE)*SQUARE_SIZE+10,(i-WALL_SIZE)*SQUARE_SIZE+10,
						 30+(j-WALL_SIZE)*SQUARE_SIZE,30+(i-WALL_SIZE)*SQUARE_SIZE);
		}
	DeleteObject(red);
	DeleteObject(blue);
}
//--------------------------------------------------------------------------
//				   DrawActiveChecker
//				   -----------------
//	General      : Draws currently active checker
//	Parameters   : opp1 - HDC (device context)
//  Return Value : None
//--------------------------------------------------------------------------
void CPlay::DrawActiveChecker(HDC &hdc)
{
	//brushes
	HBRUSH red,blue;
	red = CreateSolidBrush(RGB(200,130,130));
	blue = CreateSolidBrush(RGB(180,180,250));
	
	//choose brush
	if((m_Board[m_Active.y][m_Active.x] - m_Board[m_Active.y][m_Active.x]/2) == RED)
		SelectObject(hdc,red);
	else SelectObject(hdc,blue);

	//draw the checker
	Ellipse(hdc,(m_Active.x - WALL_SIZE)*SQUARE_SIZE,
				(m_Active.y - WALL_SIZE)*SQUARE_SIZE,
			    (m_Active.x - WALL_SIZE)*SQUARE_SIZE + SQUARE_SIZE,
				(m_Active.y - WALL_SIZE)*SQUARE_SIZE + SQUARE_SIZE);

	DeleteObject(red);
	DeleteObject(blue);

}
//--------------------------------------------------------------------------
//				   DrawActiveChecker
//				   -----------------
//	General      : Draws possible moves for currently 
//                 active checker
//	Parameters   : opp1 - HDC (device context)
//  Return Value : None
//--------------------------------------------------------------------------
void CPlay::DrawPossibleMoves(HDC &hdc)
{
	//brush
	HBRUSH white;
	white = CreateSolidBrush(RGB(220,220,220));
	SelectObject(hdc,white);
	
	//draw possible moves
	for (int i = 0; i<MAX_MOVES && m_Moves[i].x != NONE; i++)
	{
        Ellipse(hdc,(m_Moves[i].x - WALL_SIZE) *SQUARE_SIZE + 15,
					(m_Moves[i].y - WALL_SIZE) *SQUARE_SIZE + 15,
					(m_Moves[i].x - WALL_SIZE) *SQUARE_SIZE + 25,
					(m_Moves[i].y - WALL_SIZE) *SQUARE_SIZE + 25);
	}
	DeleteObject(white);
}
//--------------------------------------------------------------------------
//				   TempDraw
//				   --------
//	General      : Draws the checkers on the board from the temporary 
//                 copy of the checkers arrays
//	Parameters   : HDC (device context)
//	Return Value : None
//--------------------------------------------------------------------------
void CPlay::TempDraw(HDC &hdc)
{
	int i;
	CHECKERS brushFlag = REDCHECKER;

	//creating brushes for coloring
	HBRUSH red,blue;
	red = CreateSolidBrush(RGB(200,30,30));
	blue = CreateSolidBrush(RGB(80,80,250));
	SelectObject(hdc,red);

	for(i = 0; i<MAX_CHECKERS; i++)
	{
		if(tempRed[0][i] != REDCHECKER)
		{
			//draw checker
			Ellipse(hdc,(tempRed[0][i]-WALL_SIZE)*SQUARE_SIZE,
						(tempRed[1][i]-WALL_SIZE)*SQUARE_SIZE,
						SQUARE_SIZE+(tempRed[0][i]-WALL_SIZE)*SQUARE_SIZE,
						SQUARE_SIZE+(tempRed[1][i]-WALL_SIZE)*SQUARE_SIZE);
		}
		if(tempRed[2][i] == REDKING)
		{
			Ellipse(hdc,(tempRed[0][i]-WALL_SIZE)*SQUARE_SIZE+10,
						(tempRed[1][i]-WALL_SIZE)*SQUARE_SIZE+10,
						30+(tempRed[0][i]-WALL_SIZE)*SQUARE_SIZE,
						30+(tempRed[1][i]-WALL_SIZE)*SQUARE_SIZE);
		}

	}	
	SelectObject(hdc,blue);
	for(i = 0; i<MAX_CHECKERS; i++)
	{
		if(tempBlue[0][i] != BLUECHECKER)
		{
			//draw checker
			Ellipse(hdc,(tempBlue[0][i]-WALL_SIZE)*SQUARE_SIZE,
						(tempBlue[1][i]-WALL_SIZE)*SQUARE_SIZE,
						SQUARE_SIZE+(tempBlue[0][i]-WALL_SIZE)*SQUARE_SIZE,
						SQUARE_SIZE+(tempBlue[1][i]-WALL_SIZE)*SQUARE_SIZE);
		}
		if(tempBlue[2][i] == BLUEKING)
		{
			Ellipse(hdc,(tempBlue[0][i]-WALL_SIZE)*SQUARE_SIZE+10,
						(tempBlue[1][i]-WALL_SIZE)*SQUARE_SIZE+10,
						30+(tempBlue[0][i]-WALL_SIZE)*SQUARE_SIZE,
						30+(tempBlue[1][i]-WALL_SIZE)*SQUARE_SIZE);
		}
	}
	DeleteObject(red);
	DeleteObject(blue);
}
//--------------------------------------------------------------------------
//				   ClearPossibleMoves
//				   ------------------
//	General      : Clears from screen the possible moves 
//	Parameters   : opp1 - hWnd (handle to window)
//  Return Value : None
//--------------------------------------------------------------------------
void CPlay::ClearPossibleMoves(HWND &hWnd)
{
	RECT rect;
	for (int i = 0; i<MAX_MOVES && m_Moves[i].x != NONE; i++)
	{
		rect.left = (m_Moves[i].x - WALL_SIZE) * SQUARE_SIZE + 15;
		rect.right = (m_Moves[i].x - WALL_SIZE) * SQUARE_SIZE + 25;
		rect.bottom = (m_Moves[i].y - WALL_SIZE) * SQUARE_SIZE + 25;
		rect.top = (m_Moves[i].y - WALL_SIZE) * SQUARE_SIZE + 15;
		InvalidateRect(hWnd,&rect,false);
	}
}
//--------------------------------------------------------------------------
//				   ActivateChecker
//				   ---------------
//	General      : Runs checks in order to determine if
//                 a checker the user wants to select is 
//				   able to make a move
//	Parameters   : opp1 - x coordinate of chosen checker
//                 opp2 - y coordinate of chosen checker
//				   opp3 - HDC (device context)
//  Return Value : Possible to move this checker or not
//--------------------------------------------------------------------------
int CPlay::ActivateChecker(int x, int y, HDC &hdc,HWND &hWnd)
{

	//increasing to match the game board
	x = x + WALL_SIZE;
	y = y + WALL_SIZE;
	
	ClearPossibleMoves(hWnd);
	ClearPossibleMoves();
	
	int Moving = 0;
	if(m_MustJump == 1)
		Moving = CheckJump(x,y,true);
	else
		Moving = CheckMove(x,y,true);

	if(Moving)
	{
		//draws moves possibilites
		DrawPossibleMoves(hdc);

		//creates brushes for coloring
		HBRUSH red,blue;
		red = CreateSolidBrush(RGB(200,130,130));
		blue = CreateSolidBrush(RGB(180,180,250));
	
		//choosing color
		if(m_Board[y][x]==REDCHECKER || m_Board[y][x]==REDKING)
			SelectObject(hdc,red);
		else SelectObject(hdc,blue);

		//drawing the checker
		Ellipse(hdc,(x - WALL_SIZE)*SQUARE_SIZE,(y - WALL_SIZE)*SQUARE_SIZE,
									SQUARE_SIZE + (x - WALL_SIZE)*SQUARE_SIZE,
									SQUARE_SIZE + (y - WALL_SIZE)*SQUARE_SIZE);

		DeleteObject(red);
		DeleteObject(blue);
		m_Active.x = x;
		m_Active.y = y;
		return Moving;
	}
	return Moving;
}
//--------------------------------------------------------------------------
//				   MoveToPoint
//				   -----------
//	General      : Checks if active target can move to the
//                 given target, and moves the checker 
//                 there if its true
//	Parameters   : opp1 - x coordinate of chosen target
//                 opp2 - y coordinate of chosen target
//				   opp3 - must jump in this turn or not
//				   opp4 - handle to window
//  Return Value : Moved to the target point or not
//--------------------------------------------------------------------------
MoveType CPlay::MoveToPoint(int destX, int destY,HDC &hdc,HWND &hWnd,bool pc)
{
	MoveType moved = NOMOVE;
	//increasing to match the game board
	
	if(!pc)
	{
		destY = destY + WALL_SIZE;
		destX = destX + WALL_SIZE;
	}

	if(pc || IsPossibleMove(destX,destY)) //checks if possible to move
	{
		ClearPossibleMoves(hWnd);
		//moves the checker
        if( (destY == WALL_SIZE && m_Board[m_Active.y][m_Active.x] == BLUECHECKER) ||
			(destY == GAMEBOARD_SIZE - WALL_SIZE - 1 && 
			    					m_Board[m_Active.y][m_Active.x] == REDCHECKER) )
			m_Board[destY][destX] = m_Board[m_Active.y][m_Active.x]*2;
		else 
			m_Board[destY][destX] = m_Board[m_Active.y][m_Active.x];
		m_Board[m_Active.y][m_Active.x] = EMPTY;
		MoveCheckerPt(m_Turn,m_Active.x,m_Active.y,destX,destY);
		moved = MOVE;
	
		if (m_MustJump !=0) //jump was made
		{
		   //captured checker = 0
		   m_Board[m_Active.y + (destY - m_Active.y)/2]
			       [m_Active.x + (destX - m_Active.x)/2] = EMPTY;
		   MoveCheckerPt(-m_Turn, m_Active.x + (destX - m_Active.x)/2,
						   m_Active.y + (destY - m_Active.y)/2,-1,-1);
		   moved = JUMP;
		}
		ClearPossibleMoves();
		
		if(!pc && m_MustJump) 
		{
			//checks for more possible moves		
			m_MustJump = CheckJump(destX,destY,true);
			if(m_MustJump)
			{
				moved = MULTIJUMP;
				m_Active.x = destX;
				m_Active.y = destY;
				DrawPossibleMoves(hdc);
			}
		}
		else
		{
			m_Active.x = NONE;
			m_Active.y = NONE;
		}
	}
	return moved;
}
//--------------------------------------------------------------------------
//				   CheckMove
//				   ---------
//	General      : Checks in which directions can a checker
//                 in given coordinats move
//	Parameters   : opp1 - x coordinate of checker
//                 opp2 - y coordinate of checker
//				   WriteMove - possible moves array should 
//                             be updated or not
//  Return Value : Checker can be moved or not
//--------------------------------------------------------------------------
int CPlay::CheckMove(int x, int y, bool WriteMove)
{
	int move = 0;
	int P = 0;
	int direction = m_Board[y][x] - m_Board[y][x]/2;
	POINT moves[4] = { x-1, y+direction, x+1, y+direction, x+1, y-direction,
														 x-1, y-direction };

	for(int i = 0; i<MAX_MOVES; i++)
	{
		if((i<2 || abs(m_Board[y][x]) == KING) && 
							m_Board[moves[i].y][moves[i].x] == 0)
		{
			if(WriteMove)
			{
				m_Moves[P].x = moves[i].x;
				m_Moves[P++].y = moves[i].y;
			}
			move = 1;
		}
	}
	return move;
}
//--------------------------------------------------------------------------
//				   CheckJump
//				   ---------
//	General      : Checks in which directions can a checker
//                 in given coordinats Jump
//	Parameters   : opp1 - x coordinate of checker
//                 opp2 - y coordinate of checker
//  Return Value : Checker can jump or not
//--------------------------------------------------------------------------
int CPlay::CheckJump(int x, int y, bool save)
{
	int jump = 0;
	int P = 0;
	int dir = m_Board[y][x] - m_Board[y][x]/2;
	POINT moves[4] = {x-2,y+dir*2,x+2,y+dir*2,x-2,y-dir*2,x+2,y-dir*2};
	POINT opposite[4] = {x-1,y+dir,x+1,y+dir,x-1,y-dir,x+1,y-dir};
	
	for (int i = 0; i<MAX_MOVES; i++)
	{
		if((i<2 || abs(m_Board[y][x]) == KING) && 
		   m_Board[moves[i].y][moves[i].x] == EMPTY &&
		   OppositeCheckers(x,y,opposite[i].x,opposite[i].y))
		{
			if(save)
			{
				m_Moves[P].x = moves[i].x;
				m_Moves[P++].y = moves[i].y;
			}
			jump = 1;
		}
	}
	return jump;
}
//--------------------------------------------------------------------------
//				   OppositeCheckers
//				   ----------------
//	General      : Checks if the two given checkers are 
//			       from different players
//	Parameters   : opp1 - x coordinate of checker 1
//                 opp2 - y coordinate of checker 1
//	             : opp3 - x coordinate of checker 2
//                 opp4 - y coordinate of checker 2
//  Return Value : Checkers are opposite or not
//--------------------------------------------------------------------------
bool CPlay::OppositeCheckers(int x1,int y1,int x2,int y2)
{
	//if one of these spotes isnt checker
	if (m_Board[y1][x1] * m_Board[y2][x2] == 0)
		return false;

	//if checkers are checkers and they are different
	if( (m_Board[y1][x1] - m_Board[y1][x1]/2) != 
		(m_Board[y2][x2] - m_Board[y2][x2]/2))
			  return true;
	return false;
}
//--------------------------------------------------------------------------
//				   IsPossibleMove
//				   --------------
//	General      : Checks if a target point is in moves list
//	Parameters   : opp1 - x coordinate of target 
//                 opp2 - y coordinate of target 
//  Return Value : Target in possible moves list or not
//--------------------------------------------------------------------------
bool CPlay::IsPossibleMove(int destX, int destY)
{
	for(int i = 0; i<MAX_MOVES && m_Moves[i].x != NONE; i++)
        if(m_Moves[i].x == destX && m_Moves[i].y == destY)
			return true;
	return false;
}
//--------------------------------------------------------------------------
//				   CheckMustJump
//				   -------------
//	General      : Checks if in current turn there has to 
//                 be jump made by checker
//	Parameters   : opp1 - Type of checker(red or blue)
//	Return Value : Must be a jump or not
//--------------------------------------------------------------------------
bool CPlay::CheckMustJump(int CheckerType)
{
	int x,y;
	for (y = WALL_SIZE; y<BOARD_SIZE + WALL_SIZE; y++)
		for (x = WALL_SIZE; x<BOARD_SIZE + WALL_SIZE; x++)
		 if( (m_Board[y][x] == CheckerType || m_Board[y][x] == CheckerType*2)
														&& CheckJump(x,y,false)!=0)
		 {
			 m_MustJump = 1;
			 return true;
		 }

	m_MustJump = 0;
	return false;
}
//--------------------------------------------------------------------------
//				   NowhereToMove
//				   -------------
//	General      : Checks if a player cannot move any of 
//                 his checkers which are left (lose case)
//	Parameters   : opp1 - Type of checker(red or blue)
//	Return Value : Possible to move a checker or not
//--------------------------------------------------------------------------
bool CPlay::NowhereToMove(int CheckerType)
{
	int x,y;
	for (y = WALL_SIZE; y<BOARD_SIZE + WALL_SIZE; y++)
		for (x = WALL_SIZE; x<BOARD_SIZE + WALL_SIZE; x++)
		 if( (m_Board[y][x] == CheckerType || m_Board[y][x] == CheckerType*2)
			 && (CheckMove(x,y,false)!=0 || CheckJump(x,y,false)!=0))
				return false;
	return true;
}
//--------------------------------------------------------------------------
//				   MoveCheckerPt
//				   -------------
//	General      : moves checker's coordinats in array of checkers
//				   or puts -1 in coordinats of those which are needed 
//				   to be erased
//	Parameters   : opp1 - Type of checker(red or blue)
//				   opp2 - X coordinat of checker
//				   opp3 - Y coordinat of checker
//				   opp4 - X destination coordinat
//				   opp5 - Y destination coordinat
//	Return Value : the index of the movied checker in the checkers array
//--------------------------------------------------------------------------
int CPlay::MoveCheckerPt(int type, int x, int y,int destX,int destY)
{
	POINT *loop;
	//choose the right array (red or blue checkers)
	if(type == RED)
		loop = pRedCheckers;
	else
		loop = pBlueCheckers;
	
	//go through the array
	for (int i = 0; i<MAX_CHECKERS; i++)
	{
		if(loop[i].x == x && loop[i].y == y) // if found
		{
			loop[i].x = destX;
			loop[i].y = destY;
			return i;
		}
	}
	return -1;
}
//--------------------------------------------------------------------------
//				   MoveCheckerPt
//				   -------------
//	General      : moves checker's coordinats in array of checkers
//				   or puts -1 in coordinats of those which are needed 
//				   to be erased
//	Parameters   : opp1 - Type of checker(red or blue)
//				   opp2 - index in checkers array
//				   opp3 - X destination coordinat
//				   opp4 - Y destination coordinat
//	Return Value : None
//--------------------------------------------------------------------------
void CPlay::MoveCheckerPt(int type, int i, int x, int y)
{
	POINT *loop;
	//choose the right array (red or blue checkers)
	if(type == RED)
		loop = pRedCheckers;
	else
		loop = pBlueCheckers;
	loop[i].x = x;
	loop[i].y = y;
}
//--------------------------------------------------------------------------
//				   CopyArrays
//				   ----------
//	General      : Makes a copt of the checkers arrays
//	Parameters   : None
//	Return Value : None
//--------------------------------------------------------------------------
void CPlay::CopyArrays()
{
	for (int i = 0; i<MAX_CHECKERS; i++)
	{
		tempRed[0][i] = pRedCheckers[i].x;
		tempRed[1][i] = pRedCheckers[i].y;
		tempRed[2][i] = m_Board[pRedCheckers[i].y][pRedCheckers[i].x];
		
		tempBlue[0][i] = pBlueCheckers[i].x;
		tempBlue[1][i] = pBlueCheckers[i].y;
		tempBlue[2][i] = m_Board[pBlueCheckers[i].y][pBlueCheckers[i].x];
	}
}
//--------------------------------------------------------------------------
//				   PCMoveToPoint
//				   -------------
//	General      : moves the checker turn made by computer
//	Parameters   : opp1 - X destination coordinat
//				   opp2 - Y destination coordinat
//	Return Value : Moved to the target point or not
//--------------------------------------------------------------------------
int CPlay::PCMoveToPoint(CMovesNode* node,HWND &hWnd)
{
	int moved;
	int destX,destY,x,y;
	HDC hdc = GetDC(hWnd);
	moved = 0;
	int jump = CheckMustJump(RED);

	while(node)
	{
		node->GetCoordinats(x,y,destX,destY);
		m_Active.x = x; m_Active.y = y;
		MoveToPoint(destX,destY,hdc,hWnd,true);
		InvalidateGameRect(destX - 2,destY - 2);
		InvalidateGameRect(x - 2,y - 2);
		if(jump != 0)
		{
			InvalidateGameRect(x + (destX - x)/2 - 2,
							   y + (destY - y)/2 - 2);
			numBlueCheckers++;
		}
		node = node->GetNext();
	}
	ReleaseDC(hWnd,hdc);
	return moved;
}