//mind.cpp

#include "StdAfx.h"
#include "mind.h"
#include "play.h"
#include <math.h>

//--------------------------------------------------------------------------
//
// This file contains the implementation of the class CMind
//
//--------------------------------------------------------------------------
// Programmer  : Sergey Borovik
// Email	   : sbsergey@hotmail.com
// Date        : 27/04/04
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//                 				  Constructor
//--------------------------------------------------------------------------
CMind::CMind(void)
{
	m_Finish = false;
	NewGrade = false;
    DEPTH = 6;
	_Random = 15;
	_Exchanges = 100;
	_Balance = 100;
	_FirstRows = 100;
	_Center = 100;
	_DiffPower = 100;
}
//--------------------------------------------------------------------------
//      				          Destructor
//--------------------------------------------------------------------------
CMind::~CMind(void)
{
}
// acsess methods

void CMind::GetEngineChanges(int& random,int& power, int& exchanges,
							 int& balance, int& firstrows,int& center)
{
	random = _Random;
	exchanges = _Exchanges;
	balance = _Balance;
	firstrows = _FirstRows;
	center = _Center;
	power = _DiffPower;
}
// modifying methods

void CMind::Start()
{  m_Finish = false; }

void CMind::Finish() 
{  m_Finish = true;  } 

void CMind::SetPGame(CPlay *p) 
{  m_pGame = p;  }

void CMind::SetDepth(int _Depth) 
{  DEPTH = _Depth;  }

void CMind::SetEngineChanges(int random,int power,int exchanges,int balance, 
												 int firstrows, int center)
{
	_Random = random;
	_Exchanges = exchanges;
	_Balance = balance;
	_FirstRows = firstrows;
	_Center = center;
	_DiffPower = power;
}

// other methods

//--------------------------------------------------------------------------
//				   Think
//				   -----
//	General      : Sends each possible move to the BuildTree function
//				   and from recieved grades chooses the best and sends 
//				   the chosen move to a function which makes it
//	Parameters   : Opp1 - Type of checker
//				   Opp2 - handle on window
//  Return Value : None
//--------------------------------------------------------------------------
void CMind::Think(int Type,HWND &hWnd)
{	
	int count;
	POINT pt; pt.x = NONE;
	originalRed = m_pGame->numRedCheckers;
	originalBlue = m_pGame->numBlueCheckers;
	CMovesList *List = new CMovesList;
	float grade = BuildTree(Type,DEPTH,List,pt,MINUS_INFINITY,PLUS_INFINITY);
    CMovesNode *loop;
	for(count = 0,loop = List->GetHead();loop;loop = loop->GetNext())
		if(loop->GetGrade() == grade)
			count++;
	count = rand()%count + 1;
	for(loop = List->GetHead();loop && count>0;)
	{
		if(loop->GetGrade() == grade)
			count--;
		if(count != 0)
			loop = loop->GetNext();
	}
	CMovesNode *turn = NULL;
	ChooseFromMulti(&turn,loop,grade);
	m_pGame->PCMoveToPoint(turn,hWnd);
	List->DeleteList();
	if((*turn->GetNextList())!=NULL)
		(*turn->GetNextList())->DeleteList();
	delete turn;
}
//--------------------------------------------------------------------------
//				   BuildTree
//				   ---------
//	General      : Builds the lists of possible turns at current 
//				   possision depending of depth 
//	Parameters   : Opp1 - Type of side (red or blue)
//				   Opp2 - Current Depth (0 at start)
//				   Opp3 - list, to write the multi jumps into it 
//				   Opp4 - active checker
//				   Opp5 - alpha for alpha-beta pruning
//				   Opp6 - beta for alpha-beta pruning
//  Return Value : grade of the root (original move)
//--------------------------------------------------------------------------
float CMind::BuildTree(int Type, int Depth, CMovesList* List, POINT active,
													float alpha,float beta)
{
	if(!m_Finish && Depth>0 && !m_pGame->NowhereToMove(Type))
	{	
		MessageLoop();
		bool MustJump,evaluated = false;
		POINT tempMoves[4],tempCheckers[2],cur,*loop;
		int move,MultiJumps,tempType,tempMoveType,DeletedIndex;
		float min = PLUS_INFINITY,max = MINUS_INFINITY,grade;

		MustJump = m_pGame->CheckMustJump(Type);    
		loop = (Type == RED) ? m_pGame->pRedCheckers:m_pGame->pBlueCheckers;

		//scan each checker
		for(int i = 0;i<MAX_CHECKERS;i++)
		{
			if(loop[i].x != NONE)
			{
				if(active.x != NONE)
				{	cur = active; MustJump = true; }
				else cur = loop[i];
				CreatePossibleMoves(cur,MustJump,tempMoves);

				// go through possible moves for this checker
				for(move = 0;tempMoves[move].x!=NONE && move<MAX_MOVES; move++)
				{
					if(Depth == DEPTH)
						List->AddNode(new CMovesNode(cur.x,cur.y,tempMoves[move]));
					
					//saving changes
					SaveChanges(tempCheckers,tempMoves[move],Type,cur,
								MustJump,DeletedIndex,tempType,tempMoveType);
					
					//multi jump
					if(MustJump && m_pGame->CheckJump
								  (tempCheckers[0].x,tempCheckers[0].y,true))
					{
						MultiJumps = 1;
						if(Depth == DEPTH) 
						{
							CMovesList* multiList = new CMovesList();
							List->GetLast()->SetNextList(multiList);
							grade = BuildTree(Type,Depth,multiList,
														tempCheckers[0],alpha,beta);
							List->GetLast()->SetGrade(grade);
						}
						else grade = BuildTree(Type,Depth,NULL,
														tempCheckers[0],alpha,beta);
						UpdateMinMax(min,max,grade);
					}
					else  
					{
						// regular move or regular jump
						MultiJumps = 0;
						POINT noPt; noPt.x = NONE;
						grade = BuildTree(-Type,Depth-1,NULL,noPt,alpha,beta); 
						if(Depth == DEPTH)
							List->GetLast()->SetGrade(grade);
						UpdateMinMax(min,max,grade);
					}		

					//evaluate
					if(grade == MINUS_INFINITY)
					{
						if(m_pGame->numBlueCheckers + m_pGame->numRedCheckers > 14)
							int  a =4;
						evaluated = true;
						grade = Evaluate(Type);
						UpdateMinMax(min,max,grade);
						if(List != NULL)
							List->GetLast()->SetGrade(grade);
					}

					//retrieve changes
					RetrieveChanges(tempCheckers,cur,MustJump,tempType,
										DeletedIndex,Type,tempMoveType);
					
					//alpha-beta cutoff
					if(!evaluated)
					{
						float ab = CheckAlphaBeta(Depth,alpha,beta,grade);
						if(ab != MINUS_INFINITY) 
							return ab;
					}
				}
				if(active.x != NONE)
					break;
			}
		}
		return (Depth%2==0) ?  min:max;
	}
	return MINUS_INFINITY;
}
//--------------------------------------------------------------------------
//				   MessageLoop
//				   -----------
//	General      : Called from the buildtree function in order to recieve
//				   windows messages during tree building
//	Parameters   : None
//  Return Value : None
//--------------------------------------------------------------------------
void CMind::MessageLoop()
{
	MSG msg;
	if(!m_Finish && PeekMessage(&msg,NULL,0,0,PM_REMOVE))
    {
		TranslateMessage(&msg);  
		DispatchMessage(&msg);
	}
}
//--------------------------------------------------------------------------
//				   CreatePossibleMoves
//				   -------------------
//	General      : Builds and stores possible moves in current situation
//				   for a given checker
//	Parameters   : Opp1 - coordinats of the given checker
//				   Opp2 - must jump or not
//				   Opp3 - array for storing the possible moves
//  Return Value : None
//--------------------------------------------------------------------------
void CMind::CreatePossibleMoves(POINT cur, bool MustJump,POINT tempMoves[])
{
	m_pGame->ClearPossibleMoves();	
	if(MustJump == true)
		m_pGame->CheckJump(cur.x,cur.y,true);
	else m_pGame->CheckMove(cur.x,cur.y,true);
	for(int move = 0; move<MAX_MOVES; move++)  
		tempMoves[move] = m_pGame->m_Moves[move];
}
//--------------------------------------------------------------------------
//				   UpdateMinMax
//				   ------------
//	General      : Recieves references on min,max and also the grade,
//				   and updates the min and max variables if nessecery
//	Parameters   : Opp1 - min reference
//				   Opp2 - max reference
//				   Opp3 - grade
//  Return Value : None
//--------------------------------------------------------------------------
void CMind::UpdateMinMax(float& min, float& max, float grade)
{
	if(min>grade && grade!=MINUS_INFINITY)
		min = grade;
	if(max<grade && grade!=MINUS_INFINITY)
		max = grade;
}
//--------------------------------------------------------------------------
//				   CheckAlphaBeta
//				   --------------
//	General      : Recieves current depth, references on alpha,neta and 
//				   the grade and checks for alpha-beta cutoff and also
//				   updates alpha and beta variables
//	Parameters   : Opp1 - current depth
//				   Opp2 - alpha reference
//				   Opp3 - beta reference
//				   Opp4 - grade
//  Return Value : Minus infinity if there is no cutoff
//				   alpha or beta if there is
//--------------------------------------------------------------------------
float CMind::CheckAlphaBeta(int Depth,float& alpha, float& beta, float grade)
{
	float ret = MINUS_INFINITY;	
	if(Depth%2 == 0)				// min node
	{
		if(grade<beta)
			beta = grade;
		if(alpha>=beta)
			ret = beta;
	}
	else							// max node
	{	
		if(grade>alpha)
			alpha = grade;
		if(alpha>= beta) 
			ret = alpha;
	}
	return ret;
}
//--------------------------------------------------------------------------
//				   SaveChanges
//				   -----------
//	General      : saves changes made during the building of min-max tree
//	Parameters   : Opp1 - array for storing coordinats
//				   Opp2 - possible moves
//				   Opp3 - type of side (red or blue)
//				   Opp4 - current checker
//				   Opp5 - is there a jump now or not
//				   Opp6 - index of deleted checker (if there a is jump)
//				   Opp7 - type of the deleted checker (if there is a jump)
//				   Opp8 - type of moving checker
//  Return Value : None
//--------------------------------------------------------------------------
void CMind::SaveChanges(POINT tempCheckers[],POINT tempMoves,int Type,
		POINT cur,bool MustJump,int &DeletedIndex, int &tempType, int &tempMoveType)
{

	//make changes : move
	tempCheckers[0] = tempMoves;
	tempMoveType = m_pGame->m_Board[tempCheckers[0].y][tempCheckers[0].x] = 
					 m_pGame->m_Board[cur.y][cur.x];
	if(((tempCheckers[0].y == 2 && Type == BLUE) || (tempCheckers[0].y == 9 
							  && Type == RED)) && abs(tempMoveType) != KING)
		m_pGame->m_Board[tempCheckers[0].y][tempCheckers[0].x]*=2;
		
	m_pGame->m_Board[cur.y][cur.x] = EMPTY;
	m_pGame->MoveCheckerPt(Type,cur.x,cur.y,
						  tempCheckers[0] .x,tempCheckers[0].y);
	
	//make changes : jump
	if(MustJump == true)
	{
		tempCheckers[1].x = (cur.x + tempCheckers[0].x) / 2;
		tempCheckers[1].y = (cur.y + tempCheckers[0].y) / 2;
		tempType = m_pGame->m_Board[tempCheckers[1].y][tempCheckers[1].x];
		m_pGame->m_Board[tempCheckers[1].y][tempCheckers[1].x] = EMPTY;
		DeletedIndex = m_pGame->MoveCheckerPt
						(-Type,tempCheckers[1].x,tempCheckers[1].y,-1,-1);
		if(Type == 1)
			m_pGame->numBlueCheckers++;
		else
			m_pGame->numRedCheckers++;

		m_pGame->ClearPossibleMoves();
	}
}
//--------------------------------------------------------------------------
//				   RetrieveChanges
//				   ---------------
//	General      : retrieves changes saved earlier during building of 
//				   min-max tree
//	Parameters   : Opp1 - array for storing coordinats
//				   Opp2 - current checker
//				   Opp3 - is there a jump now or not
//				   Opp4 - type of deleted checker (if was a jump)
//				   Opp5 - index of deleted checker (if was a jump)
//				   Opp6 - type of side (red or blue)
//				   Opp7 - type of moving checker
//  Return Value : None
//--------------------------------------------------------------------------
void CMind::RetrieveChanges(POINT tempCheckers[],POINT cur,bool MustJump,
					 int tempType,int DeletedIndex,int Type,int tempMoveType)
{
	//retrieve changes : move
	m_pGame->m_Board[cur.y][cur.x] = tempMoveType;
	m_pGame->m_Board[tempCheckers[0].y][tempCheckers[0].x] = EMPTY;
	m_pGame->MoveCheckerPt(Type,tempCheckers[0].x,tempCheckers[0].y,
				   									  cur.x,cur.y);
	//retrieve changes : jump
	if(MustJump == true)
	{
		m_pGame->m_Board[tempCheckers[1].y]
						[tempCheckers[1].x] = tempType;
		m_pGame->MoveCheckerPt
			(-Type,DeletedIndex,tempCheckers[1].x,tempCheckers[1].y);
		if(Type == RED)
			m_pGame->numBlueCheckers--;
		else
			m_pGame->numRedCheckers--;
	}
}
//--------------------------------------------------------------------------
//				   ChooseFromMulti
//				   ---------------
//	General      : chooses the right multi jump out of possibile multi
//				   jumps that could be made by this 
//	Parameters   : Opp1 - list that contains the move
//				   Opp2 - temporary list
//				   Opp3 - grade of the 				
//  Return Value : None
//--------------------------------------------------------------------------
void CMind::ChooseFromMulti(CMovesNode** turn,CMovesNode* node,float grade)
{
	CMovesNode* loop;
	CMovesNode* original;
	CMovesList* list;
	loop = node;
	original = (*turn) = new CMovesNode;
	while(loop)
	{
		if(loop->GetGrade() == grade)
		{
			(*turn)->CopyData(loop);
			list = *(loop->GetNextList());
			if(list)
			{
				(*turn)->SetNext(new CMovesNode);
				(*turn) = (*turn)->GetNext();
				loop = list->GetHead();
			}
			else loop = NULL;
		}
		else loop = loop->GetNext();
	}
	(*turn) = original;
}
//--------------------------------------------------------------------------
//				   Evaluate
//				   --------
//	General      : calculates the grade of the current position on the
//				   board and returns it 
//	Parameters   : Opp1 - Type of side (red or blue)
//  Return Value : grade
//--------------------------------------------------------------------------
float CMind::Evaluate(int Type)
{
	int red,blue,diff;
	int numOfCheckers = MAX_CHECKERS*2 - 
						  (m_pGame->numBlueCheckers + m_pGame->numRedCheckers);
	//----------------------------------
	//  RANDOM NUMBER
	int random = rand()%_Random;
	float grade = (float) random;
	//----------------------------------
	//  POWER OF CHECKERS
	grade+= (CheckPower()*_DiffPower/100);
	//----------------------------------
	//  EXCHANGES
	diff = (Type == RED) ? m_pGame->numRedCheckers - m_pGame->numBlueCheckers :
						   m_pGame->numBlueCheckers - m_pGame->numRedCheckers ;

	int exchanges = (CountExchanges()*_Exchanges/100);
	exchanges = (diff>1) ? exchanges*3 : -exchanges*4;
	if(numOfCheckers>16 && numOfCheckers<20)
		grade+= exchanges*2;
	else if(numOfCheckers>10 && numOfCheckers<17)
		grade+= exchanges/2;
	else if(numOfCheckers<8)
		grade+= exchanges*2;
	//----------------------------------
	//  BALANCE ON BOARD
	CheckBalance(red,blue);
	red = red*_Balance/100;
	blue = blue*_Balance/100;
	if(numOfCheckers>18)
		grade-=blue*4;
	else if (numOfCheckers<19 && numOfCheckers>10)
		grade-= blue*2;
	//----------------------------------
	//  CENTER
	CheckCenter(red,blue);
	red = red*_Center/100;
	blue = blue*_Center/100;
	if(numOfCheckers>18)
		grade+=blue*6;
	else 
		grade+= blue*4;
	//----------------------------------
	//  FIRST TWO ROWS
	int first,second;
	CountFirstRows(Type,first,second);
	first = first*_FirstRows/100;
	second = second*_FirstRows/100;
	if(numOfCheckers<12)
	{
		grade-= first*6;
		grade-= second*4;
	}
	//----------------------------------
	return grade;
}
//--------------------------------------------------------------------------
//				   CountExchanges
//				   --------------
//	General      : counts and returns the number of exchanges between 
//				   checkers made, from the original to current position
//	Parameters   : None
//  Return Value : the number of exchanges
//--------------------------------------------------------------------------
int CMind::CountExchanges()
{
	int red = m_pGame->numRedCheckers - originalRed;
	int blue = m_pGame->numBlueCheckers - originalBlue;
	return (blue>red) ? blue - red: red - blue;
}
//--------------------------------------------------------------------------
//				   CheckBalance
//				   ------------
//	General      : counts and returns the number of exchanges between 
//				   checkers made, from the original to current position
//	Parameters   : None
//  Return Value : the number of exchanges
//--------------------------------------------------------------------------
void CMind::CheckBalance(int& red, int& blue)
{
	red = blue = 0;
	int redLeft = 0,redRight = 0;
	int blueLeft = 0,blueRight = 0;
	for(int i = 0; i<MAX_CHECKERS; i++)
	{
		if(m_pGame->pRedCheckers[i].x != NONE)
		{
			if(m_pGame->pRedCheckers[i].x <=5)
				redLeft++;
			else
				redRight++;
		}
		if(m_pGame->pBlueCheckers[i].x != NONE)
		{
			if(m_pGame->pBlueCheckers[i].x <=5)
				blueLeft++;
			else
				blueRight++;
		}
	}
	red = abs(redLeft-redRight);
	blue = abs(blueLeft-blueRight);
}
//--------------------------------------------------------------------------
//				   CountFirstRows
//				   --------------
//	General      : counts and returns the number of checkers in the
//				   first and in the second rows
//	Parameters   : Opp1 - type of side (red or blue)
//				   Opp2 - pointer on integer indicading first row
//				   Opp3 - pointer on integer indicading second row
//  Return Value : None
//--------------------------------------------------------------------------
void CMind::CountFirstRows(int Type,int &first, int &second)
{
	first = second = 0;
	POINT* loop;
	loop = (Type == 1) ? m_pGame->pRedCheckers:m_pGame->pBlueCheckers;
	for(int i = 0; i<MAX_CHECKERS;i++,loop++)
	{
		if((Type == RED && loop->y == 2) || (Type == BLUE) && loop->y == 9)
			first++;
		if((Type == RED && loop->y == 3) || (Type == BLUE) && loop->y == 8)
			second++;
	}
}
//--------------------------------------------------------------------------
//				   CheckCenter
//				   --------------
//	General      : counts and returns the number of checkers in the center
//				   for both sides
//	Parameters   : Opp1 - pointer on integer indicading red
//				   Opp2 - pointer on integer indicading blue
//  Return Value : None
//--------------------------------------------------------------------------
void CMind::CheckCenter(int &red,int &blue)
{
	red = blue = 0;
	POINT center[4] = {4,5,6,5,5,6,7,6};
	bool found = false;
	for(int i = 0; i<MAX_CHECKERS; i++)
	{
		for(int c = 0, found = false; c<4 && !found; c++)
			if(center[c].x == m_pGame->pRedCheckers[i].x &&
			   center[c].y == m_pGame->pRedCheckers[i].y)
			{
				red++;
				found = true;
			}
		
		for(c = 0, found = false; c<4 && !found; c++)
			if(center[c].x == m_pGame->pBlueCheckers[i].x &&
			   center[c].y == m_pGame->pBlueCheckers[i].y)
			{
				red++;
				found = true;
			}
	}
}
//--------------------------------------------------------------------------
//				   CheckPower
//				   ----------
//	General      : returns the power of the checkers minus the power of 
//				   the opposent's power of checkers
//	Parameters   : None
//  Return Value : power
//--------------------------------------------------------------------------
int CMind::CheckPower()
{
	int grade = 0;
	for(int i = 0; i<MAX_CHECKERS; i++)
	{
		// calculates blue power
		if(m_pGame->pBlueCheckers[i].x != NONE)
		{
			if(m_pGame->m_Board[m_pGame->pBlueCheckers[i].y]
							   [m_pGame->pBlueCheckers[i].x] == BLUECHECKER)
			{
				grade+=100;
				grade+= (10 - m_pGame->pBlueCheckers[i].y)*
						(10 - m_pGame->pBlueCheckers[i].y);
			}
			else 
			{
				grade+=200;
				if(m_pGame->pBlueCheckers[i].y == 2 || m_pGame->pBlueCheckers[i].y == 9)
					grade-=30;
				if(m_pGame->pBlueCheckers[i].x == 2 || m_pGame->pBlueCheckers[i].x == 9)
					grade-=30;
			}
		}
		//calculates red power
		if(m_pGame->pRedCheckers[i].x != NONE)
		{
			if(m_pGame->m_Board[m_pGame->pRedCheckers[i].y]
							   [m_pGame->pRedCheckers[i].x] == REDCHECKER)
			{
				grade-=100;
				grade-= (m_pGame->pRedCheckers[i].y - 1)*
						(m_pGame->pRedCheckers[i].y - 1);
			}
			else 
			{
				grade-=200;
				if(m_pGame->pRedCheckers[i].y == 2 || m_pGame->pRedCheckers[i].y == 9)
					grade+=30;
				if(m_pGame->pRedCheckers[i].x == 2 || m_pGame->pRedCheckers[i].x == 9)
					grade+=30;
			}
		}
	}
	return grade;
}
