//mind.h

#pragma once
#include "MovesList.h"

//-----------------------------------------------------------------------
//							CMind
//							=====
//
// General :	This class represents the thinking part of the program,
//			    the computer choosing the best turn
//
// Methods :	SetPGame		    - sets the CPlay class pointer
//				SetDepth		    - sets the maximal depth of the tree
//				SetEngineChanges    - sets the changes made by user for
//								  	  the grade picking weights
//				GetEngineChanges    - returns all the engine parametres
//									  through references
//				Finish              - sets the finish variable to true
//									  makes the tree building to stop
//				Start               - sets the finish variable to false
//
//				BuildTree		    - builds the min-max tree
//				Think			    - calls buildtree function, chooses
//									  the right turn and sends it to 
//									  pc moving function
//				SaveChanges         - when building a tree saves the 
//									  changes made on the board
//				RetrieveChanges     - when building a tree retrieves
//									  the changes saved before
//				ChooseFromMulti     - chooses the correct multi jump
//									  from number of possible jumps of 
//									  the same checker
//				Evaluate		    - returns a grade based on the 
//									  position made on the board
//				CountExchanges      - returns number of exchanges made
//				CheckBalance        - returns the difference in the number
//									  of checkers in left and right side 
//									  of the board
//				CountFirstRows      - returns the number of checkers in
//									  the first and second rows
//				CheckCenter         - returns number of checkers standing
//									  at the middle of the board
//				CheckPower		    - returns the summed power of all 
//									  checkers (the power of checker grows
//									  as the checker advances in the board)
//				MessageLoop         - recieves windows messages furing
//									  tree building
//				UpdateMinMax        - updates the min and max variables
//				CheckAlphaBeta      - checks if there is a cutoff and
//									  updates alpha and beta variables
//			    CreatePossibleMoves - creates and stores possible moves
//									  for given checker
//
//-----------------------------------------------------------------------
// Programmer  : Sergey Borovik
// Email       : sbsergey@hotmail.com
// Date        : 27/04/04
//-----------------------------------------------------------------------

class CPlay;
class CMind
{
	CPlay *m_pGame;
	int CurMove;
	bool Must_Jump;
	POINT m_Root;
	int DEPTH;
	bool NewGrade;
	int originalRed;
	int originalBlue;
	bool m_Finish;
	int _Random;
	int _Exchanges;
	int _Balance;
	int _FirstRows;
	int _Center;
	int _DiffPower;
public:
	CMind(void);
	~CMind(void);
	void Finish();
	void Start();
	void SetPGame(CPlay*);
	float BuildTree(int,int,CMovesList*,POINT,float,float); 
	void Think(int,HWND&);
	float Evaluate(int);
	void SaveChanges(POINT[],POINT,int,POINT,bool,int&,int&,int&);
	void RetrieveChanges(POINT[],POINT,bool,int,int,int,int);
	void ChooseFromMulti(CMovesNode**,CMovesNode*,float);
	int CountExchanges();
	void CheckBalance(int&,int&);
	void CountFirstRows(int,int&,int&);
	void CheckCenter(int&,int&);
	int CheckPower();
	void SetDepth(int);
	void SetEngineChanges(int,int,int,int,int,int);
	void GetEngineChanges(int&,int&,int&,int&,int&,int&);
	void MessageLoop();
	void CreatePossibleMoves(POINT,bool,POINT[]);
	void UpdateMinMax(float&,float&,float);
	float CheckAlphaBeta(int,float&,float&,float);
};
