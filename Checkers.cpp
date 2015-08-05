// Checkers.cpp 
//--------------------------------------------------------------------------
//************************************************************************//
#include "stdafx.h"
#include "Checkers.h"
#include "game.h"
#include <commctrl.h>
#include <Shellapi.h>
#include <richedit.h>
#define MAX_LOADSTRING 100

//************************************************************************//
//--------------------------------------------------------------------------
//		 _______ _                 _                     
//		(_______) |               | |                    
//		 _      | |__  _____  ____| |  _ _____  ____ ___ 
//		| |     |  _ \| ___ |/ ___) |_/ ) ___ |/ ___)___)
//		| |_____| | | | ____( (___|  _ (| ____| |  |___ |
//		 \______)_| |_|_____)\____)_| \_)_____)_|  (___/ 
//
//--------------------------------------------------------------------------
//************************************************************************//
//					Programmer: Sergey Borovik
//					Email: sbsergey@hotmail.com
//--------------------------------------------------------------------------
//************************************************************************//

// Global Variables declaration
HINSTANCE hInst;								
TCHAR szTitle[MAX_LOADSTRING];					
TCHAR szWindowClass[MAX_LOADSTRING];			
CGame game;
POINT active;
bool activeChecker = false;
bool MoreMoves = false;
GameStatus gameStatus = NOGAME;
GameStatus gameType = NOGAME;
HWND hStatusWnd;
HWND hwnd;
bool pcmove = false;
bool tempdraw = false;
long CurTick = 0;
int turns = 0;

// Forward declarations of functions included in this code module
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	Engine(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	Rules(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	Statistics(HWND, UINT, WPARAM, LPARAM);
void BuildStatsWindow();
void DrawBoard(HDC&);
bool SourceChecker(int,int);
bool TargetForChecker(int,int);
void MakePCTurn();
void GameOver();
void DrawMovingChecker();
void UpdateStatusBar();
void CleanDepthMenu();

//************************************************************************//
//--------------------------------------------------------------------------
//				   WinMain
//				   -------
//
//	General: The Main function of the program
//
//--------------------------------------------------------------------------
//************************************************************************//
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CHECKERS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_CHECKERS);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int) msg.wParam;
}
//************************************************************************//
//--------------------------------------------------------------------------
//				   MyRegisterClass
//				   ---------------
//
//	  General: Registers the window class
//
//--------------------------------------------------------------------------
//************************************************************************//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_CHECKERS);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_CHECKERS;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}
//************************************************************************//
//--------------------------------------------------------------------------
//				   InitInstance
//				   ------------
//
//	  General: Saves instance handle and creates main window
//
//--------------------------------------------------------------------------
//************************************************************************//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle,WS_VISIBLE|WS_SYSMENU,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}
//************************************************************************//
//--------------------------------------------------------------------------
//				   WndProc
//				   -------
//
//	  General: Processes messages for the main window
//
//--------------------------------------------------------------------------
//************************************************************************//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	int parts[2] = { 60,320 };
	switch (message) 
	{
	case WM_CREATE:
		hwnd = hWnd;
		//sets dimensions of the window
		SetWindowPos(hWnd,NULL,200,200,327,384,SWP_SHOWWINDOW);
		//loads library for the common controls
		LoadLibrary("Comctl32.dll");
		//creats status bar window
		hStatusWnd = CreateWindowEx(0L,STATUSCLASSNAME,"",
						WS_CHILD|WS_VISIBLE,0,0,0,0,hWnd,NULL,hInst,0);
		SendMessage(hStatusWnd, SB_SETPARTS,(WPARAM) 2,(LPARAM) parts);
		SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM) "Ready...");
		SendMessage(hStatusWnd,SB_SETTEXT,1,(LPARAM) "");
		CheckMenuItem(GetMenu(hwnd),ID_DEPTH_6,MF_CHECKED);
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		//menu selection swtich
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_MULTIPLAYER:
			gameStatus = MULTIPLAYER;
			game.GetPlay()->Start();
			// update the status bar
			SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM) "Turn: Blue");
			SendMessage(hStatusWnd,SB_SETTEXT,1,(LPARAM) "Move your checker");
			EnableMenuItem(GetMenu(hWnd),IDM_SINGLE,MF_GRAYED);
			EnableMenuItem(GetMenu(hWnd),IDM_MULTIPLAYER,MF_GRAYED);
			InvalidateRect(hWnd,NULL,false);
			gameType = MULTIPLAYER;
			CurTick = GetTickCount();
			game.GetPlay()->CheckMustJump(BLUE);
			break;
		case IDM_SINGLE:
			gameStatus = SINGLE;
			game.GetMind()->Start();
			game.GetPlay()->Start();
			// update the status bar
			SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM) "Turn: Blue");
			SendMessage(hStatusWnd,SB_SETTEXT,1,(LPARAM) "Move your checker");
			EnableMenuItem(GetMenu(hWnd),IDM_SINGLE,MF_GRAYED);
			EnableMenuItem(GetMenu(hWnd),IDM_MULTIPLAYER,MF_GRAYED);
			InvalidateRect(hWnd,NULL,false);
			gameType = SINGLE;
			CurTick = GetTickCount();
			break;
		case IDM_FINISHGAME:
			gameStatus = NOGAME;
			active.x = NONE;
			active.y = NONE;
			activeChecker = false;
			MoreMoves = false;
			if(game.GetPlay()->GetTurn() == RED)
				game.GetPlay()->ChangeTurn();
			
			// update the status bar
			SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM) "Ready...");
			SendMessage(hStatusWnd,SB_SETTEXT,1,(LPARAM) "");
			EnableMenuItem(GetMenu(hWnd),IDM_SINGLE,MF_ENABLED);
			EnableMenuItem(GetMenu(hWnd),IDM_MULTIPLAYER,MF_ENABLED);
			InvalidateRect(hWnd,NULL,false);
			gameType = NOGAME;
			CurTick = 0;
			turns = 0;
			break;
		case ID_DEPTH_2:
			game.GetMind()->SetDepth(2);
			CleanDepthMenu();
			CheckMenuItem(GetMenu(hWnd),ID_DEPTH_2,MF_CHECKED);
            break;
		case ID_DEPTH_4:
			game.GetMind()->SetDepth(4);
			CleanDepthMenu();
			CheckMenuItem(GetMenu(hWnd),ID_DEPTH_4,MF_CHECKED);
			break;
		case ID_DEPTH_6:
			game.GetMind()->SetDepth(6);
			CleanDepthMenu();
			CheckMenuItem(GetMenu(hWnd),ID_DEPTH_6,MF_CHECKED);
			break;
		case ID_DEPTH_8:
            game.GetMind()->SetDepth(8);
			CleanDepthMenu();
			CheckMenuItem(GetMenu(hWnd),ID_DEPTH_8,MF_CHECKED);
			break;
		case ID_DEPTH_10:
			game.GetMind()->SetDepth(10);
			CleanDepthMenu();
			CheckMenuItem(GetMenu(hWnd),ID_DEPTH_10,MF_CHECKED);
			break;
		case ID_CONFIGUREENGINE:
			DialogBox(hInst, (LPCTSTR)IDD_DIALOG1, hWnd, (DLGPROC)Engine);
			break;
		case ID_WEBSITE:
			ShellExecute(NULL,"open","http://www.wcdclan.de",NULL,NULL,1);
			break;
		case ID_EMAIL:
			ShellExecute(NULL,"open","mailto:sbsergey@hotmail.com?subject=Checkers game",
																			NULL,NULL,1);
			break;
		case ID_RULES:
			DialogBox(hInst, (LPCTSTR)IDD_DIALOG2, hWnd, (DLGPROC)Rules);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		//board
		DrawBoard(hdc);
		//checkers
		if(gameStatus)
		{
			if(!pcmove)
				game.GetPlay()->DrawCheckers(hdc);
			else
			{
				game.GetPlay()->TempDraw(hdc);
				tempdraw = true;
			}
			//possible moves
			if(activeChecker)
			{
				game.GetPlay()->DrawActiveChecker(hdc);
				game.GetPlay()->DrawPossibleMoves(hdc);
			}
		}
		EndPaint(hWnd, &ps);
		break;
	case WM_LBUTTONDOWN:
		if(gameStatus == MULTIPLAYER || 
		  (gameStatus == SINGLE && game.GetPlay()->GetTurn() == BLUE))
		{
			if(!activeChecker)
				SourceChecker(LOWORD(lParam)/SQUARE_SIZE,HIWORD(lParam)/SQUARE_SIZE);
			else 
				if(!TargetForChecker(LOWORD(lParam)/SQUARE_SIZE,
					HIWORD(lParam)/SQUARE_SIZE) && MoreMoves == false)
					   SourceChecker(LOWORD(lParam)/SQUARE_SIZE,
									 HIWORD(lParam)/SQUARE_SIZE);
		}
		break;
	case WM_DESTROY:
		game.GetMind()->Finish();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
//************************************************************************//
//--------------------------------------------------------------------------
//				   About
//				   -----
//
//	  General: Processes messages for the About dialog
//
//--------------------------------------------------------------------------
//************************************************************************//
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}
//************************************************************************//
//--------------------------------------------------------------------------
//				   Statistics
//				   ----------
//
//	  General: Processes messages for the Statistics dialog
//
//--------------------------------------------------------------------------
//************************************************************************//
LRESULT CALLBACK Statistics(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	char str[20];
	int red,blue,diff;
	game.GetPlay()->GetNumCheckers(red,blue);
	red = 12 - red; blue = 12 - blue;
	diff = (red>blue) ? red-blue:blue-red;
	switch (message)
	{
	case WM_INITDIALOG:
		if(gameType == SINGLE)
			SetWindowText(GetDlgItem(hDlg,IDC_STATIC1),"Player vs PC");
		else SetWindowText(GetDlgItem(hDlg,IDC_STATIC1),"Player vs Player");
		if(red < blue)
			SetWindowText(GetDlgItem(hDlg,IDC_STATIC2),"BLUE");
		else SetWindowText(GetDlgItem(hDlg,IDC_STATIC2),"RED");
		SetWindowText(GetDlgItem(hDlg,IDC_STATIC6),itoa(CurTick/1000,str,10));
		SetWindowText(GetDlgItem(hDlg,IDC_STATIC7),itoa(turns/2,str,10));
		SetWindowText(GetDlgItem(hDlg,IDC_STATIC3),itoa(red,str,10));
		SetWindowText(GetDlgItem(hDlg,IDC_STATIC4),itoa(blue,str,10));
		SetWindowText(GetDlgItem(hDlg,IDC_STATIC5),itoa(diff,str,10));
		InvalidateRect(hwnd,NULL,false);
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}
//************************************************************************//
//--------------------------------------------------------------------------
//				   Rules
//				   -----
//
//	  General: Processes messages for the rules dialog
//
//--------------------------------------------------------------------------
//************************************************************************//
LRESULT CALLBACK Rules(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND rich;
	char str[1500] = "";
	strcat(str,"");
	switch (message)
	{
	case WM_INITDIALOG:
		LoadLibrary("Riched20.dll");
		rich = CreateWindowEx(0L,RICHEDIT_CLASS,"",
						WS_CHILD | WS_VISIBLE |  
						WS_VSCROLL | ES_MULTILINE | ES_WANTRETURN | 
						WS_BORDER | ES_READONLY,20,15,540,320,hDlg,NULL,hInst,0);
		
		strcat(str,"ABOUT:\n\n");
		strcat(str,"Draughts is a popular checkers type game, and also known");
		strcat(str," as american-english checkers. It is played all around the world, ");
		strcat(str,"and most of the computer checkers programs play by these rules.\n\n");
		strcat(str,"MOVING:\n\n");
		strcat(str,"Moves forward-only going in diagonal position to an empty space.");
		strcat(str," If there is a jump possible (ability to jump over opposent's checker");
		strcat(str," to an empty square) it must be made and no regular moves possible.");
		strcat(str," One can jump only one piece at a time. If multiple captures are ");
		strcat(str," possible in the same position, the jump must be made till the end,");
		strcat(str," untill there is no possible jumping. Pieces are removed during ");
		strcat(str," capture moves after each jump, as such it is not possible");
		strcat(str," to jump twice over the same piece.\n\n");
		strcat(str,"KINGS:\n\n");
		strcat(str,"Piece turns into kings when move ends on the last row of the board.");
		strcat(str," Kings have the same abilities as the regular pieces, but with the ");
		strcat(str,"addition of the ability to move backwards aswell, ");
		strcat(str,"which makes their value superior over the regular pieces.\n\n");
		strcat(str,"ENDING:\n\n");
		strcat(str,"The one who succeeds to capture all the opposent's pieces and has");
		strcat(str," its own remaining on the board wins. Another win possibility is ");
		strcat(str,"that there may be a possible situation where one of the sides");
		strcat(str," cannot make a move (due to its pieces being blocked by others).");
		strcat(str," In this case this side loses, and the opposent side wins.");
		SetWindowText(rich,str);
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}
//************************************************************************//
//--------------------------------------------------------------------------
//				   Engine
//				   ------
//
//	  General: Processes messages for the engine dialog
//
//--------------------------------------------------------------------------
//************************************************************************//
LRESULT CALLBACK Engine(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	char str[6];
	int Random;
	int Exchanges;
	int Balance;
	int FirstRows;
	int Center;
	int DiffPower;
	switch (message)
	{
	case WM_INITDIALOG:
		game.GetMind()->GetEngineChanges(Random,DiffPower,Exchanges,Balance,
														  FirstRows,Center);
		
		SendMessage(GetDlgItem(hDlg,IDC_SLIDER1),TBM_SETRANGE,true,MAKELONG(1,1000));
		SendMessage(GetDlgItem(hDlg,IDC_SLIDER1),TBM_SETPOS,true,Random);
		SetWindowText(GetDlgItem(hDlg,IDC_EDIT1),itoa(Random,str,10));
    	SendMessage(GetDlgItem(hDlg,IDC_SLIDER2),TBM_SETRANGE,true,MAKELONG(0,200));
		SendMessage(GetDlgItem(hDlg,IDC_SLIDER2),TBM_SETPOS,true,DiffPower);
		SetWindowText(GetDlgItem(hDlg,IDC_EDIT2),itoa(DiffPower,str,10));
		SendMessage(GetDlgItem(hDlg,IDC_SLIDER3),TBM_SETRANGE,true,MAKELONG(0,10000));
		SendMessage(GetDlgItem(hDlg,IDC_SLIDER3),TBM_SETPOS,true,Exchanges);
		SetWindowText(GetDlgItem(hDlg,IDC_EDIT3),itoa(Exchanges,str,10));
		SendMessage(GetDlgItem(hDlg,IDC_SLIDER4),TBM_SETRANGE,true,MAKELONG(0,10000));
		SendMessage(GetDlgItem(hDlg,IDC_SLIDER4),TBM_SETPOS,true,Balance);
		SetWindowText(GetDlgItem(hDlg,IDC_EDIT4),itoa(Balance,str,10));
		SendMessage(GetDlgItem(hDlg,IDC_SLIDER5),TBM_SETRANGE,true,MAKELONG(0,10000));
		SendMessage(GetDlgItem(hDlg,IDC_SLIDER5),TBM_SETPOS,true,FirstRows);
		SetWindowText(GetDlgItem(hDlg,IDC_EDIT5),itoa(FirstRows,str,10));
		SendMessage(GetDlgItem(hDlg,IDC_SLIDER6),TBM_SETRANGE,true,MAKELONG(0,10000));
		SendMessage(GetDlgItem(hDlg,IDC_SLIDER6),TBM_SETPOS,true,Center);
		SetWindowText(GetDlgItem(hDlg,IDC_EDIT6),itoa(Center,str,10));
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			if(LOWORD(wParam) == IDOK)
			{
				Random = (int) SendMessage(GetDlgItem(hDlg,IDC_SLIDER1),TBM_GETPOS ,0,0);
				DiffPower = (int) SendMessage(GetDlgItem(hDlg,IDC_SLIDER2),TBM_GETPOS ,0,0);
				Exchanges = (int) SendMessage(GetDlgItem(hDlg,IDC_SLIDER3),TBM_GETPOS ,0,0);
				Balance = (int) SendMessage(GetDlgItem(hDlg,IDC_SLIDER4),TBM_GETPOS ,0,0);
				FirstRows = (int) SendMessage(GetDlgItem(hDlg,IDC_SLIDER5),TBM_GETPOS ,0,0);
				Center = (int) SendMessage(GetDlgItem(hDlg,IDC_SLIDER6),TBM_GETPOS ,0,0);
				game.GetMind()->SetEngineChanges(Random,DiffPower,Exchanges,Balance,
																  FirstRows,Center);
			}
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	case WM_HSCROLL:
		
		switch(GetDlgCtrlID((HWND)lParam))
		{
		case IDC_SLIDER1:	itoa((int)SendMessage(GetDlgItem(hDlg,IDC_SLIDER1),
													   TBM_GETPOS,0,0),str,10);
							SetWindowText(GetDlgItem(hDlg,IDC_EDIT1),str);
							break;
		case IDC_SLIDER2:	itoa((int)SendMessage(GetDlgItem(hDlg,IDC_SLIDER2),
													   TBM_GETPOS,0,0),str,10);
							SetWindowText(GetDlgItem(hDlg,IDC_EDIT2),str);
							break;
		case IDC_SLIDER3:	itoa((int)SendMessage(GetDlgItem(hDlg,IDC_SLIDER3),
													   TBM_GETPOS,0,0),str,10);
							SetWindowText(GetDlgItem(hDlg,IDC_EDIT3),str);
							break;
		case IDC_SLIDER4:	itoa((int)SendMessage(GetDlgItem(hDlg,IDC_SLIDER4),
													   TBM_GETPOS,0,0),str,10);
							SetWindowText(GetDlgItem(hDlg,IDC_EDIT4),str);
							break;
		case IDC_SLIDER5:	itoa((int)SendMessage(GetDlgItem(hDlg,IDC_SLIDER5),
													   TBM_GETPOS,0,0),str,10);
							SetWindowText(GetDlgItem(hDlg,IDC_EDIT5),str);
							break;
		case IDC_SLIDER6:	itoa((int)SendMessage(GetDlgItem(hDlg,IDC_SLIDER6),
													   TBM_GETPOS,0,0),str,10);
							SetWindowText(GetDlgItem(hDlg,IDC_EDIT6),str);
							break;
		}
		break;
	}
	return FALSE;
}
//--------------------------------------------------------------------------
//				   DrawBoard
//				   ---------
//	General      : Draws the board itself (squares)
//	Parameters   : opp1 - device context
//	Return Value : None
//--------------------------------------------------------------------------
void DrawBoard(HDC &hdc)
{
	int i,j;
	HBRUSH grey,white;
	white = CreateSolidBrush(RGB(220,220,220));	// for white squares
	grey = CreateSolidBrush(RGB(70,70,70));		// for black squares
	
	//draw white squares
	SelectObject(hdc,white);
	for(i=0; i<BOARD_SIZE; i++)
		for(j=i%2; j<BOARD_SIZE; j+=2)
			Rectangle(hdc,i*SQUARE_SIZE,j*SQUARE_SIZE,
					  SQUARE_SIZE+i*SQUARE_SIZE,SQUARE_SIZE+j*SQUARE_SIZE);
	
	//draw black squares
	SelectObject(hdc,grey);
	for(i=0; i<BOARD_SIZE; i++)
		for(j=(i+1)%2; j<BOARD_SIZE; j+=2)
			Rectangle(hdc,i*SQUARE_SIZE,j*SQUARE_SIZE,
					  SQUARE_SIZE+i*SQUARE_SIZE,SQUARE_SIZE+j*SQUARE_SIZE);

	//free memory of the objects
	DeleteObject(white);
	DeleteObject(grey);

}
//--------------------------------------------------------------------------
//				   SourceChecker
//				   -------------
//	General      : lets a user to select a checker. checks if 
//				   it is possible or not to select this particular checker
//				   and if it is, then selects it
//	Parameters   : opp1 - x coordinat of checker
//				   opp2 - y coordinat of checker
//	Return Value : true or false (was selected or not)
//--------------------------------------------------------------------------
bool SourceChecker(int x,int y)
{
	int Activated;
	int checker = game.GetPlay()->IsChecker(x,y);		// the kind of checker
	int turn = game.GetPlay()->GetTurn();				// current turn
	if(turn == checker || turn == checker/2)	// is the checker of this turn?
	{
		HDC hdc = GetDC(hwnd);
		Activated = game.GetPlay()->ActivateChecker(x,y,hdc,hwnd);	
		ReleaseDC(hwnd,hdc);
		if(Activated) // selected
		{
			InvalidateGameRect(active.x,active.y);
			active.x = x;
			active.y = y;
			activeChecker = true; // checker was selected
			return true;
		}
		else // checker cannot be selected
		{
			// unselect checker and update the window
			activeChecker = false;
			InvalidateGameRect(active.x,active.y);	
			active.x = NONE;
			active.y = NONE;
		}
	}
	else if(activeChecker == true) 
	{
		// if there was allready selected checker unselect it
		activeChecker = false;
		InvalidateGameRect(active.x,active.y);	
		game.GetPlay()->ClearPossibleMoves(hwnd);
		active.x = NONE;
		active.y = NONE;
	}
	return false;
}
//--------------------------------------------------------------------------
//				   TargetChecker
//				   -------------
//	General      : recieves a point where the user wants to put his 
//				   selected checker. checks if the checker can move to 
//				   to there and moves it if so
//	Parameters   : opp1 - x coordinat of checker
//				   opp2 - y coordinat of checker
//	Return Value : true or false (was moved or not)
//--------------------------------------------------------------------------
bool TargetForChecker(int x,int y)
{
	bool Ret = false;
	if(game.GetPlay()->IsChecker(x,y) == EMPTY) 	// checks if the target is empty
	{
		MoveType move;
		HDC hdc = GetDC(hwnd);
		move = game.GetPlay()->MoveToPoint(x,y,hdc,hwnd,false);   // move the checker
		ReleaseDC(hwnd,hdc);
		switch(move)
		{
		// captured
		case JUMP:  game.GetPlay()->IncCheckersNum();
					if(game.GetPlay()->IsGameOver())
					{
						 GameOver();
						 InvalidateGameRect(active.x,active.y);
						 InvalidateGameRect(x,y);
						 InvalidateGameRect((x+active.x)/2,(y+active.y)/2);
						 return true;
					 }
					InvalidateGameRect((x+active.x)/2,(y+active.y)/2);					
		// moved
		case MOVE:	 activeChecker = false;
					 MoreMoves = false;
					 InvalidateGameRect(active.x,active.y);
					 InvalidateGameRect(x,y);
					 game.GetPlay()->ChangeTurn();
					 if(gameStatus == SINGLE && game.GetPlay()->GetTurn() == RED) 
					 {
						 UpdateStatusBar();
						 SendMessage(hStatusWnd,SB_SETTEXT,1,(LPARAM) "Thinking...");
						 // pc turn
						 game.GetPlay()->CheckMustJump(game.GetPlay()->GetTurn());
						 MakePCTurn();
						 if(gameStatus != GAMEOVER)				
							game.GetPlay()->ChangeTurn();
					}	
					if(gameStatus != GAMEOVER)
						UpdateStatusBar();
					Ret = true;
					turns++;
					break;
		// captured and can capture more
		case MULTIJUMP:  
					MoreMoves = true;
					game.GetPlay()->IncCheckersNum();
					InvalidateGameRect(active.x,active.y);
					InvalidateGameRect((x+active.x)/2,(y+active.y)/2);
					InvalidateGameRect(x,y);
				    active.x = x;
				    active.y = y;
					Ret = true;
					break;
		}
		if(move!=NOMOVE && move!=MULTIJUMP && game.GetPlay()->NowhereToMove
										(game.GetPlay()->GetTurn()) == true)
			GameOver();
	}
	return Ret;
}
//--------------------------------------------------------------------------
//				   MakePCTurn
//				   ----------
//	General      : makes a move by pc
//	Parameters   : None
//	Return Value : None
//--------------------------------------------------------------------------
void MakePCTurn()
{
	pcmove = true;
	game.GetPlay()->CopyArrays();
	EnableMenuItem(GetMenu(hwnd),ID_CONFIGUREENGINE,MF_GRAYED);
	EnableMenuItem(GetMenu(hwnd),ID_DEPTH_2,MF_GRAYED);
	EnableMenuItem(GetMenu(hwnd),ID_DEPTH_4,MF_GRAYED);
	EnableMenuItem(GetMenu(hwnd),ID_DEPTH_6,MF_GRAYED);
	EnableMenuItem(GetMenu(hwnd),ID_DEPTH_8,MF_GRAYED);
	EnableMenuItem(GetMenu(hwnd),ID_DEPTH_10,MF_GRAYED);
	EnableMenuItem(GetMenu(hwnd),IDM_FINISHGAME,MF_GRAYED);

	//check if game over
	if(game.GetPlay()->NowhereToMove(game.GetPlay()->GetTurn()) == true) 
		GameOver();
	else
		game.GetMind()->Think(RED,hwnd);
	
	pcmove = false;
	EnableMenuItem(GetMenu(hwnd),ID_CONFIGUREENGINE,MF_ENABLED);
	EnableMenuItem(GetMenu(hwnd),ID_DEPTH_2,MF_ENABLED);
	EnableMenuItem(GetMenu(hwnd),ID_DEPTH_4,MF_ENABLED);
	EnableMenuItem(GetMenu(hwnd),ID_DEPTH_6,MF_ENABLED);
	EnableMenuItem(GetMenu(hwnd),ID_DEPTH_8,MF_ENABLED);
	EnableMenuItem(GetMenu(hwnd),ID_DEPTH_10,MF_ENABLED);
	EnableMenuItem(GetMenu(hwnd),IDM_FINISHGAME,MF_ENABLED);
	turns++;
}
//--------------------------------------------------------------------------
//				   GameOver
//				   --------
//	General      : updates the data for gaming over
//	Parameters   : None
//	Return Value : None
//--------------------------------------------------------------------------
void GameOver()
{
	activeChecker = false; // unselect active checker
	// update the statusbar
	SendMessage(hStatusWnd,SB_SETTEXT,1,(LPARAM) "Game Over");
	SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM) "");

	if(game.GetPlay()->GetTurn() == RED)
		game.GetPlay()->ChangeTurn();
	gameStatus = GAMEOVER; 
	CurTick = GetTickCount()-CurTick;
	DialogBox(hInst, (LPCTSTR)IDD_DIALOG3, hwnd, (DLGPROC)Statistics);
}
//--------------------------------------------------------------------------
//				   UpdateStatusBar
//				   ---------------
//	General      : updates the data in statusbar, who's turn is it
//				   and must the user jump or just move the checker
//	Parameters   : None
//	Return Value : None
//--------------------------------------------------------------------------
void UpdateStatusBar()
{
	if(game.GetPlay()->CheckMustJump(game.GetPlay()->GetTurn()))
		SendMessage(hStatusWnd,SB_SETTEXT,1,
									(LPARAM) "You must jump at this turn");
	else SendMessage(hStatusWnd,SB_SETTEXT,1,(LPARAM) "Move your checker");
	if(game.GetPlay()->GetTurn() == RED)
		SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM) "Turn: Red");
	else
		SendMessage(hStatusWnd,SB_SETTEXT,0,(LPARAM) "Turn: Blue");
}
//--------------------------------------------------------------------------
//				   CleanDepthMenu
//				   ---------------
//	General      : cleans the data of menu
//	Parameters   : None
//	Return Value : None
//--------------------------------------------------------------------------
void CleanDepthMenu()
{
	CheckMenuItem(GetMenu(hwnd),ID_DEPTH_2,MF_UNCHECKED);		
	CheckMenuItem(GetMenu(hwnd),ID_DEPTH_4,MF_UNCHECKED);		
	CheckMenuItem(GetMenu(hwnd),ID_DEPTH_6,MF_UNCHECKED);		
	CheckMenuItem(GetMenu(hwnd),ID_DEPTH_8,MF_UNCHECKED);		
	CheckMenuItem(GetMenu(hwnd),ID_DEPTH_10,MF_UNCHECKED);		
}

//--------------------------------------------------------------------------
//				   InvalidateGameRect
//				   ------------------
//	General      : recieves x,y and invalidates one square of the game
//				   board of these coordinats
//	Parameters   : opp1 - x coordinat of square
//				   opp2 - y coordinat of square
//	Return Value : None
//--------------------------------------------------------------------------
void InvalidateGameRect(int x, int y)
{
	RECT rect;
	rect.left = x*SQUARE_SIZE;
	rect.right = x*SQUARE_SIZE + SQUARE_SIZE;
	rect.top = y*SQUARE_SIZE;
	rect.bottom = y*SQUARE_SIZE + SQUARE_SIZE;
	InvalidateRect(hwnd,&rect,false);	//invalidate
}

