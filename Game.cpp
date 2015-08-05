//game.cpp

#include "StdAfx.h"
#include "game.h"

//--------------------------------------------------------------------------
//
// This file contains the implementation of the class CGame
//
//--------------------------------------------------------------------------
// Programmer  : Sergey Borovik
// Email	   : sbsergey@hotmail.com
// Date        : 27/04/04
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//                 				  Constructor
//--------------------------------------------------------------------------
CGame::CGame(void)
{
	m_Mind.SetPGame(&m_Play);
}
//--------------------------------------------------------------------------
//      				          Destructor
//--------------------------------------------------------------------------
CGame::~CGame(void)
{
}

// Access methods

CPlay* CGame::GetPlay() 
{  return &m_Play;  }

CMind* CGame::GetMind() 
{	return &m_Mind;  }