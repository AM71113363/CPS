#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

static char szClassName[ ] = "WindowsCPStest";

HINSTANCE ins;
HWND logger;
void CenterOnScreen(HWND);


HWND field;
UCHAR TextField[]="Click here\r\nas fast as you can\r\nFor 5 seconds";
WNDPROC OldStaticProc;


LARGE_INTEGER Pcounter[11];
LARGE_INTEGER	timerFrequency;
DWORD step=0;


void dsp(unsigned char *sms)
{
     DWORD len=GetWindowTextLength(logger);
     if(len>0)  SendMessage(logger,EM_SETSEL,(WPARAM)len,(LPARAM)len);
     SendMessage(logger,EM_REPLACESEL,(WPARAM)0,(LPARAM)sms);
     SendMessage(logger,EM_REPLACESEL,(WPARAM)0,(LPARAM)(unsigned char*)"\r\n");
     SendMessage(logger,WM_VSCROLL,(WPARAM)SB_ENDSCROLL,(LPARAM)0);
     return;
}


void PShowResult()
{
    DWORD i;
    UCHAR buffer[64];
    double result = 0,tm;
    ShowWindow(field,0);
    ShowWindow(logger,1);
    for(i=1;i<11;i++)
    {
      tm = ((double)(Pcounter[i].QuadPart-Pcounter[i-1].QuadPart)/(double)timerFrequency.QuadPart);
      sprintf(buffer,"time_%02d[ %f ] \0",i,tm);
      result+=tm;
      dsp(buffer);
    }
     sprintf(buffer,"\r\n%d clicks/second\0",(DWORD)(10/result));
    dsp(buffer); 
}


LRESULT CALLBACK StaticProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if((wParam == 1) && (lParam != 0))
    {
       if(step<11)
       {
            QueryPerformanceCounter(&Pcounter[step]);
           step++;
       }
       else if(step!=8888){ step=8888; PShowResult(); }
    
	}               
	return CallWindowProc(OldStaticProc, hwnd, message, wParam, lParam);
}

void Init()
{
   step = 0;
   SetWindowText(logger,"");
   ShowWindow(logger,0);
   ShowWindow(field,1);
  QueryPerformanceFrequency(&timerFrequency);         
}

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
       case WM_CREATE:
       {              
            logger=CreateWindow("EDIT","",WS_CHILD | WS_BORDER  | ES_MULTILINE,4,28,155,200,hwnd,(HMENU)0,ins,NULL);
            CreateWindow("BUTTON","Start",WS_VISIBLE|WS_CHILD,4,2,155,24,hwnd,(HMENU)1000,ins,NULL);
            field = CreateWindow("STATIC",TextField,WS_CHILD|SS_NOTIFY,18,78,128,55,hwnd,(HMENU)0,ins,NULL);
            OldStaticProc= (WNDPROC) SetWindowLong(field, GWL_WNDPROC, (LPARAM)StaticProc);
            CenterOnScreen(hwnd);
 
        }
        break;   

    case WM_COMMAND:
  switch(LOWORD(wParam))
  {
     case 1000:
          {  
             CreateThread(0,0,(LPTHREAD_START_ROUTINE)Init,0,0,0);
             //Init();
              
          } break;      
   }
   break;          
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}


int WINAPI WinMain(HINSTANCE _a_,HINSTANCE _b_, LPSTR _c_,int _d_)
{
    HWND hwnd;
    MSG messages;
    WNDCLASSEX wincl;
    ins = _a_;

    wincl.hInstance = _a_;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof (WNDCLASSEX);
    wincl.hIcon = LoadIcon (ins,MAKEINTRESOURCE(1111));
    wincl.hIconSm = LoadIcon (ins,MAKEINTRESOURCE(1111));
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    if (!RegisterClassEx (&wincl))
        return 0;


    hwnd = CreateWindowEx(WS_EX_TOPMOST,szClassName, "Clicks / Seconds", WS_SYSMENU,
           0,0,170,262,
           HWND_DESKTOP,NULL,_a_,NULL);

    ShowWindow (hwnd, _d_);

    while (GetMessage (&messages, NULL, 0, 0))
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }
  return messages.wParam;
}

void CenterOnScreen(HWND hnd)
{
  RECT rcClient, rcDesktop;
  int nX,nY;
  SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, 0);
  GetWindowRect(hnd, &rcClient);
  nX=((rcDesktop.right - rcDesktop.left) / 2) -((rcClient.right - rcClient.left) / 2);
  nY=((rcDesktop.bottom - rcDesktop.top) / 2) -((rcClient.bottom - rcClient.top) / 2);
  SetWindowPos(hnd, NULL, nX, nY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
return;
}


