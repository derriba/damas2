//moveslist.h

#pragma once

#include "stdafx.h"

#define SQUARE_SIZE 40
#define BOARD_SIZE 8
#define WALL_SIZE 2
#define RED 1
#define BLUE -1
#define KING 2
#define MAX_CHECKERS 12
#define WALL -9
#define GAMEBOARD_SIZE (BOARD_SIZE + WALL_SIZE * 2)
#define PLUS_INFINITY 100000
#define MINUS_INFINITY -100000
#define NONE -1
#define INIT_GRADE -999
#define MAX_MOVES 4
enum MoveType { NOMOVE,MOVE,JUMP,MULTIJUMP };
enum CHECKERS {BLUEKING = -2,BLUECHECKER,EMPTY,REDCHECKER,REDKING};
enum GameStatus { GAMEOVER = -1, NOGAME, SINGLE, MULTIPLAYER };

//-----------------------------------------------------------------------
//							CMovesList
//							==========
//
// General :	This class represents the list which contains nodes
//				for storing multiple jumps and possibilites
//
// Methods :	GetHead    - returns the pointer on the head of the list
//			    GetLast    - returns the pointer on the tail of the list	
//				AddNode    - adds recieved node to the list
//				DeleteList - deletes the list and all the nodes in it
//
//-----------------------------------------------------------------------
// Programmer  : Sergey Borovik
// Email       : sbsergey@hotmail.com
// Date        : 27/04/04
//-----------------------------------------------------------------------

class CMovesNode;
class CMovesList
{
	CMovesNode *head;
	CMovesNode *last;
public:
	int count;
	CMovesList(void);
	~CMovesList(void);
	CMovesNode* GetHead();
	CMovesNode* GetLast();
	void AddNode(CMovesNode*);
	void DeleteList();
};

//-----------------------------------------------------------------------
//							CMovesNode
//							==========
//
// General :	This class represents the Node from which list is made
//				It contains the pointer to the next Node, and to the 
//			    next list
//
// Methods :	GetNext           - returns the next node
//			    GetNextList       - returns the next list
//				GetGrade          - returns the grade
//				GetCoordinats     -	returns all the coordinats info
//
//				SetNext           - sets new next node
//				SetNextList		  - sets new next list
//				SetGrade          - sets new grade
//				SetDestCoordinats - sets new destination coordinats
//				SetCoordinats     - sets new coordinats info
//
//				IsFree			  - returns if the node has info or not
//				CopyData          - copies data from one node to another
//
//-----------------------------------------------------------------------
// Programmer  : Sergey Borovik
// Email       : sbsergey@hotmail.com
// Date        : 27/04/04
//-----------------------------------------------------------------------

class CMovesNode
{
	int pointX,pointY;
	int destX,destY;
	CMovesNode *next;
	CMovesList *nextList;
	float grade;
public:
	CMovesNode(int,int,POINT);
	CMovesNode();
	~CMovesNode(void);
	CMovesList** GetNextList();
	CMovesNode* GetNext();
	bool IsFree();
	void SetNext(CMovesNode*);
	void GetCoordinats(int&,int&,int&, int&);
	void SetCoordinats(int,int,POINT);
	void SetNextList(CMovesList*);
	float GetGrade();
	void SetGrade(float);
	void SetDestCoordinats(POINT);
	void CopyData(CMovesNode*);
};
