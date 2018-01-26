/*
  cl /LD gvimtweak.c user32.lib
  ------------------------------
  :call libcallnr("gvimtweak.dll", "SetAlpha", 200)
  :call libcallnr("gvimtweak.dll", "EnableMaximize", 1)
  :call libcallnr("gvimtweak.dll", "EnableTopMost", 1)
  :call libcallnr("gvimtweak.dll", "ToggleFullScreen", 1)
*/

#include <windows.h>

#ifndef LWA_ALPHA
#define LWA_ALPHA 2
#endif

#ifndef WS_EX_LAYERD
#define WS_EX_LAYERED 0x00080000
#endif

int g_x, g_y, g_dx, g_dy;

BOOL CALLBACK FindWindowProc(HWND hwnd, LPARAM lParam)
{
  HWND* pphWnd = (HWND*)lParam;

  if (GetParent(hwnd))
  {
    *pphWnd = NULL;
    return TRUE;
  }
  *pphWnd = hwnd;
  return FALSE;
}

LONG _declspec(dllexport) SetAlpha(LONG nTrans)
{
  HMODULE hDllUser32;

  hDllUser32 = LoadLibrary("user32");
  if (hDllUser32)
  {
    BOOL (WINAPI *pfnSetLayeredWindowAttributes)(HWND,DWORD,BYTE,DWORD);

    pfnSetLayeredWindowAttributes
      = (BOOL (WINAPI *)(HWND,DWORD,BYTE,DWORD))
      GetProcAddress(hDllUser32, "SetLayeredWindowAttributes");

    if (pfnSetLayeredWindowAttributes)
    {
      HWND hTop = NULL;
      DWORD dwThreadID;

      dwThreadID = GetCurrentThreadId();
      EnumThreadWindows(dwThreadID, FindWindowProc, (LPARAM)&hTop);

      if (hTop)
      {
        if (nTrans == 255)
        {
          SetWindowLong(hTop, GWL_EXSTYLE,
              GetWindowLong(hTop, GWL_EXSTYLE) & ~WS_EX_LAYERED);
        }
        else
        {
          SetWindowLong(hTop, GWL_EXSTYLE,
              GetWindowLong(hTop, GWL_EXSTYLE) | WS_EX_LAYERED);
          pfnSetLayeredWindowAttributes(
              hTop, 0, (BYTE)nTrans, LWA_ALPHA);
        }
      }
    }
    FreeLibrary(hDllUser32);
  }
  return GetLastError();
}

LONG _declspec(dllexport) EnableCaption(LONG bCaption)
{
  HWND hTop = NULL;
  DWORD dwThreadID;

  dwThreadID = GetCurrentThreadId();
  EnumThreadWindows(dwThreadID, FindWindowProc, (LPARAM)&hTop);

  if (hTop)
  {
    if (bCaption == 0)
      SetWindowLong(hTop, GWL_STYLE,
          GetWindowLong(hTop, GWL_STYLE) & ~WS_CAPTION);
    else
      SetWindowLong(hTop, GWL_STYLE,
          GetWindowLong(hTop, GWL_STYLE) | WS_CAPTION);
  }
  return GetLastError();
}

LONG _declspec(dllexport) EnableMaximize(LONG bEnable)
{
  HWND hTop = NULL;
  DWORD dwThreadID;

  dwThreadID = GetCurrentThreadId();
  EnumThreadWindows(dwThreadID, FindWindowProc, (LPARAM)&hTop);

  if (hTop)
  {
    if (bEnable == 0)
      SendMessage(hTop, WM_SYSCOMMAND, SC_RESTORE, 0);
    else
      SendMessage(hTop, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
  }
  return GetLastError();
}

LONG _declspec(dllexport) EnableTopMost(LONG bEnable)
{
  HWND hTop = NULL;
  DWORD dwThreadID;

  dwThreadID = GetCurrentThreadId();
  EnumThreadWindows(dwThreadID, FindWindowProc, (LPARAM)&hTop);

  if (hTop)
  {
    if (bEnable == 0)
      SetWindowPos(hTop, HWND_NOTOPMOST, 0, 0, 0, 0,
          SWP_NOSIZE | SWP_NOMOVE);
    else
      SetWindowPos(hTop, HWND_TOPMOST, 0, 0, 0, 0,
          SWP_NOSIZE | SWP_NOMOVE);
  }
  return GetLastError();
}

BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam);

LONG _declspec(dllexport) ToggleFullScreen()
{
  HWND hTop = NULL;
  DWORD dwThreadID;

  dwThreadID = GetCurrentThreadId();
  EnumThreadWindows(dwThreadID, FindWindowProc, (LPARAM)&hTop);

  if (hTop)
  {
    /* Determine the current state of the window */

    if ( GetWindowLong(hTop, GWL_STYLE) & WS_CAPTION )
    {
      /* Has a caption, so isn't maximised */

      MONITORINFO mi;
      RECT rc;
      HMONITOR hMonitor;

      GetWindowRect(hTop, &rc);
      hMonitor = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);

      //
      // get the work area or entire monitor rect.
      //
      mi.cbSize = sizeof(mi);
      GetMonitorInfo(hMonitor, &mi);

      g_x = mi.rcMonitor.left;
      g_y = mi.rcMonitor.top;
      g_dx = mi.rcMonitor.right - g_x;
      g_dy = mi.rcMonitor.bottom - g_y;
      //cx = GetSystemMetrics(SM_CXSCREEN);
      //cy = GetSystemMetrics(SM_CYSCREEN);

      /* Remove border, caption, and edges */
      SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_EXSTYLE) & ~WS_BORDER);
      SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_STYLE) & ~WS_CAPTION);
      SetWindowLong(hTop, GWL_EXSTYLE, GetWindowLong(hTop, GWL_STYLE) & ~WS_EX_CLIENTEDGE);
      SetWindowLong(hTop, GWL_EXSTYLE, GetWindowLong(hTop, GWL_STYLE) & ~WS_EX_WINDOWEDGE);

      SetWindowPos(hTop, HWND_TOP, g_x, g_y, g_dx, g_dy, SWP_SHOWWINDOW);

      /* Now need to find the child text area window
       * and set it's size accordingly
       */
      EnumChildWindows(hTop, EnumChildProc, 0);
    }
    else
    {
      /* Already full screen, so restore all the previous styles */
      SetWindowLong(hTop, GWL_EXSTYLE, GetWindowLong(hTop, GWL_EXSTYLE) | WS_BORDER);
      SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_STYLE) | WS_CAPTION);
      SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_STYLE) | WS_SYSMENU);
      SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_STYLE) | WS_MINIMIZEBOX);
      SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_STYLE) | WS_MAXIMIZEBOX);
      SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_STYLE) | WS_SYSMENU);
      SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_STYLE) | WS_EX_CLIENTEDGE);
      SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_STYLE) | WS_EX_WINDOWEDGE);
      SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_STYLE) | WS_THICKFRAME);
      SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_STYLE) | WS_DLGFRAME);


      SendMessage(hTop, WM_SYSCOMMAND, SC_RESTORE, 0);
      SendMessage(hTop, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
    }
  }
  return GetLastError();
}

BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam)
{
  char lpszClassName[100];
  HBRUSH b;

  UNREFERENCED_PARAMETER(lParam);

  GetClassName(hwnd, lpszClassName, 100);
  if ( strcmp(lpszClassName, "VimTextArea") == 0 )
  {
    //int cx, cy;
    //cx = GetSystemMetrics(SM_CXSCREEN);
    //cy = GetSystemMetrics(SM_CYSCREEN);

    SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_EX_CLIENTEDGE);
    SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_EX_WINDOWEDGE);
    SetWindowPos(hwnd, HWND_TOP, 0, 0, g_dx, g_dy, SWP_SHOWWINDOW);

    b = CreateSolidBrush(RGB(0,0,0));
    SetClassLongPtr (hwnd, GCLP_HBRBACKGROUND, (LONG_PTR) b);
  }
  return TRUE;
}
