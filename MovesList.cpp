//moveslist.cpp

#include "StdAfx.h"
#include "moveslist.h"

//--------------------------------------------------------------------------
//
// This is the implementation of the class CMovesList
//
//--------------------------------------------------------------------------
// Programmer  : Sergey Borovik
// Email	   : sbsergey@hotmail.com
// Date        : 27/04/04
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//                 				CList - Constructor
//--------------------------------------------------------------------------
CMovesList::CMovesList(void)
{
	head = NULL;
	count = 0;
}
//--------------------------------------------------------------------------
//                 				CList - Destructor
//--------------------------------------------------------------------------
CMovesList::~CMovesList(void)
{
	head = NULL;
}

// Access methods

CMovesNode* CMovesList::GetHead() 
{  return head;  }

CMovesNode* CMovesList::GetLast() 
{  return last;  }

// other methods

//--------------------------------------------------------------------------
//				   AddNode
//				   -------
//	General      : revieves a node and adds it to the list
//	Parameters   : Opp1 - node
//  Return Value : None
//--------------------------------------------------------------------------
void CMovesList::AddNode(CMovesNode* node)
{
	if(!head)
		head = last = node;
	else
	{
		last->SetNext(node);
		last = last->GetNext();
	}
}
//--------------------------------------------------------------------------
//				   DeleteList
//				   ----------
//	General      : deletes the list and all the nodes in it
//  Return Value : None
//--------------------------------------------------------------------------
void CMovesList::DeleteList()
{
	CMovesNode *node,*temp;
	node = head;
	while(node)
	{
		if((*node->GetNextList()) != NULL)
			(*node->GetNextList())->DeleteList();
		temp = node->GetNext();
		delete node;
		node = temp;
	}
	delete this;
}
//--------------------------------------------------------------------------
//
// This is the implementation of the class CMovesNode
//
//--------------------------------------------------------------------------
// Programmer  : Sergey Borovik
// Email	   : sbsergey@hotmail.com
// Date        : 13/04/04
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//                 				CNode - Constructors
//--------------------------------------------------------------------------
CMovesNode::CMovesNode(int x,int y,POINT dest)
{
	next = NULL;
	nextList = NULL;
	pointX = x;
	pointY = y;
	destX = (char) dest.x;
	destY = (char) dest.y;
	grade = INIT_GRADE;
}
CMovesNode::CMovesNode() 
{ 
	next = NULL;
	nextList = NULL; 
	pointX = pointY = destX = destY = NONE; 
	grade = INIT_GRADE;
}
//--------------------------------------------------------------------------
//                 				CNode - Destructor
//--------------------------------------------------------------------------
CMovesNode::~CMovesNode(void)
{
	next = NULL;
	nextList = NULL;
}

// Acsess methods

CMovesList** CMovesNode::GetNextList() 
{  return &nextList;  }

CMovesNode* CMovesNode::GetNext() 
{  return next;  }

void CMovesNode::GetCoordinats(int &x,int &y,int &dX, int &dY)
{
	x = pointX;
	y = pointY;
	dX = destX;
	dY = destY;
}

float CMovesNode::GetGrade() 
{  return grade;  }

// Modify methods

void CMovesNode::SetNext(CMovesNode *node) { next = node; }

void CMovesNode::SetCoordinats(int _x, int _y, POINT _dest)
{
	pointX = _x;
	pointY = _y;
	destX = (char) _dest.x;
	destY = (char) _dest.y;
}
void CMovesNode::SetNextList(CMovesList* _nextList) 
{  nextList = _nextList;  }

void CMovesNode::SetGrade(float _grade) 
{  grade = _grade;  }

void CMovesNode::SetDestCoordinats(POINT _dest) 
{ 
	destX = (char) _dest.x; destY = (char) _dest.y; 
}

//Other methods

bool CMovesNode::IsFree() 
{  return (pointX == NONE);  }

void CMovesNode::CopyData(CMovesNode* node)
{
	pointX = node->pointX;
	pointY = node->pointY;
	destX = node->destX;
	destY = node->destY;
}
