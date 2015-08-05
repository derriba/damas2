//play.h

#pragma once
#include "mind.h"

//-----------------------------------------------------------------------
//							CPlay
//							=====
//
// General :	This class represents game rules and holds the whole
//				information and different checks for the checkers game
//
// Methods :	IsChecker		   - returns the number in matrix 
//									 from given x,y
//				GetTurn			   - returns current turn
//				GetNumCheckers     - gets the numbers of eaten checkers
//
//				ChangeTurn		   - changes the turn from one player 
//									 to another
//				SetTurn            - sets new turn
//				IncCheckersNum     - increases the number of eated 
//									 checkers on the opposite to turn's 
//									 side
//				IsGameOver         - checks and returns if this is
//									 a game over position on the board
//				SetActiveChecker   - sets active checker 
//
//				Start              - initalizes all the members and
//									 prepares everything for the game
//				ClearPossibleMoves - clears the possible moves array
//									 (sets -1,-1 in everyone)
//				DrawCheckers	   - draws checkers on the screen
//				TempDraw           - draws checkers from temp arrays
//				DrawActiveChecker  - draws active checker 
//									 (using different color)
//				DrawPossibleMoves  - draws possible moves to particular 
//									 checker on the screen
//				ActivateChecker    - activates (selects) checker if its
//									 possible
//				MoveToPoint        - makes a move 
//				PCMoveToPoint      - makes a computer move
//				CheckMove          - checks for moves for given checker,
//									 and fills the possible moves array
//				CheckJump          - checks for jumps for given checker,
//									 and fills the possible moves array
//				CheckMustJump	   - checks if there must be ajump in 
//									 this turn
//				OppositeCheckers   - recieves two checkers and returns
//									 if they are in the opposite colors
//				NowhereToMove      - checks if a given side can make 
//									 a move now
//				MoveCheckerPt      - moves checker in checkers array
//				CopyArrays         - makes a copy of the checkers arrays
//
//
//-----------------------------------------------------------------------
// Programmer  : Sergey Borovik
// Email       : sbsergey@hotmail.com
// Date        : 27/04/04
//-----------------------------------------------------------------------

class CPlay
{
	int numRedCheckers;
	int numBlueCheckers;
	int m_Turn;
	POINT pRedCheckers[12];
	POINT pBlueCheckers[12];
	int tempRed[3][12];
	int tempBlue[3][12];
	POINT m_Moves[4];
	int m_Board[GAMEBOARD_SIZE][GAMEBOARD_SIZE];
	POINT m_Active;	
	int m_MustJump;
public:
	CPlay(void);
	~CPlay(void);
	friend class CMind;
	void CopyArrays();
	void TempDraw(HDC&);
	int GetTurn();
	void ChangeTurn();
	int MoveCheckerPt(int,int,int,int,int);
	void MoveCheckerPt(int,int,int,int);
	void SetTurn(int);
	void IncCheckersNum();
	bool IsGameOver();
	void SetActiveChecker(int,int);
	void Start();
	void ClearPossibleMoves();
	void GetNumCheckers(int&,int&);
	void DrawCheckers(HDC&);
	void DrawActiveChecker(HDC&);
	void DrawPossibleMoves(HDC&);
	void ClearPossibleMoves(HWND&);
	int ActivateChecker(int,int,HDC&,HWND&);
	MoveType MoveToPoint(int,int,HDC&,HWND&,bool);
	int PCMoveToPoint(CMovesNode*,HWND&);
	int CheckMove(int,int,bool);
	int CheckJump(int,int,bool);
	bool CheckMustJump(int);
	bool OppositeCheckers(int,int,int,int);
	bool NowhereToMove(int);
	bool IsPossibleMove(int,int);
	int IsChecker(int,int);
};
