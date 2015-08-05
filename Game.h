//game.h

#pragma once
#include "Play.h"

//-----------------------------------------------------------------------
//							CGame
//							=====
//
// General :	This class represents the game which unites 
//				all the elemenths needed the game itself to work
//
// Methods :	GetPlay - returns the pointer on CPlay member
//			    GetMind - returns the pointer on CMind member			
//
//-----------------------------------------------------------------------
// Programmer  : Sergey Borovik
// Email       : sbsergey@hotmail.com
// Date        : 27/04/04
//-----------------------------------------------------------------------

class CGame
{
	CPlay m_Play;
	CMind m_Mind;
public:
	CGame(void);
	~CGame(void);
	CPlay* GetPlay();
	CMind* GetMind();
};
