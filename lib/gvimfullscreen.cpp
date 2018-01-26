/*
 cl /LD gvimfullscreen.c user32.lib
 ------------------------------
 :call libcallnr("gvimfullscreen.dll", "EnableFullScreen", 1)
*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

BOOL CALLBACK FindWindowProc(HWND hwnd, LPARAM lParam);
void FixBackgroundColor(HWND hwndTextArea);

struct WindowInfo;
void ReadWindowInfo(char* serialized_state, WindowInfo* window_info);
void WriteWindowInfo(WindowInfo* window_info);

struct WindowInfo {
    BOOL maximized;
    BOOL fullscreen;
    LONG style;
    LONG exstyle;
    RECT rc;
};

// string buffer to return back to vim with the state information to persist
char g_serialized_window_info[2048] = "";

extern "C"
__declspec(dllexport)
char* ToggleFullScreen(char* prev_state) {
    HWND hwnd = NULL;
    EnumThreadWindows(GetCurrentThreadId(), FindWindowProc, (LPARAM)&hwnd);

    HWND hwndTextArea = FindWindowEx(hwnd, NULL, "VimTextArea",
                                     "Vim text area");

    WindowInfo window_info;
    ReadWindowInfo(prev_state, &window_info);

    if (hwnd) {
        if (!window_info.fullscreen) {
            FixBackgroundColor(hwndTextArea);

            // Save original window state, position, and size
            window_info.maximized = IsZoomed(hwnd);
            window_info.style = GetWindowLong(hwnd, GWL_STYLE);
            window_info.exstyle = GetWindowLong(hwnd, GWL_EXSTYLE);
            GetWindowRect(hwnd, &window_info.rc);

            // Get the monitor it is on
            MONITORINFO mi;
            mi.cbSize = sizeof(MONITORINFO);
            GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST),
                           &mi);

            // Fullscreen it
#if 0 // appears to not be a problem on Windows 10
            if (window_info.maximized) {
                // Restore the window if it was maximized first, because
                // Windows seems to have trouble hiding the taskbar when
                // changing the size of a maximized window.
                SendMessage(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
            }
#endif
            SetWindowLong(hwnd, GWL_STYLE,
                          window_info.style & ~(WS_CAPTION |
                              WS_THICKFRAME));
            SetWindowLong(hwnd, GWL_EXSTYLE,
                          window_info.exstyle & ~(WS_EX_DLGMODALFRAME |
                              WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE |
                              WS_EX_STATICEDGE));
            SetWindowPos(hwnd, NULL,
                         mi.rcMonitor.left,
                         mi.rcMonitor.top,
                         mi.rcMonitor.right - mi.rcMonitor.left,
                         mi.rcMonitor.bottom - mi.rcMonitor.top,
                         SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

            window_info.fullscreen = TRUE;
        } else {
            // Already full screen, so restore all the previous styles
            SetWindowLong(hwnd, GWL_STYLE, window_info.style);
            SetWindowLong(hwnd, GWL_EXSTYLE, window_info.exstyle);
            SetWindowPos(hwnd, NULL,
                         window_info.rc.left,
                         window_info.rc.top,
                         window_info.rc.right - window_info.rc.left,
                         window_info.rc.bottom - window_info.rc.top,
                         SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
#if 0 // again appears to not be a problem on Windows 10
            if (window_info.maximized) {
                SendMessage(hwnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
            }
#endif
            window_info.fullscreen = FALSE;
        }
    }

    WriteWindowInfo(&window_info);
    return g_serialized_window_info;
}

BOOL CALLBACK FindWindowProc(HWND hwnd, LPARAM lParam) {
    HWND* pphWnd = (HWND*)lParam;

    if (GetParent(hwnd)) {
        *pphWnd = NULL;
        return TRUE;
    }

    *pphWnd = hwnd;
    return FALSE;
}

void FixBackgroundColor(HWND hwndTextArea) {
    // Get lower right corner color and use that as stock brush color
    // for the background color
    if (hwndTextArea != NULL) {
        COLORREF rgb = RGB(255,0,255);
        HDC hdc = GetDC(hwndTextArea);
        if (hdc != NULL) {
            RECT rc;
            GetWindowRect(hwndTextArea, &rc);
            rgb = GetPixel(hdc, rc.right - rc.left - 2,
                                    rc.bottom - rc.top - 2);
            if (rgb != CLR_INVALID) {
                SetDCBrushColor(hdc, rgb);
            }
            ReleaseDC(hwndTextArea, hdc);
        }

        SetClassLongPtr(hwndTextArea, GCLP_HBRBACKGROUND,
                        (LONG_PTR)GetStockObject(DC_BRUSH));

        // Setting the parent background removes flicker when resizing the
        // window up to fullscreen.  However it doesn't appear to work for
        // the first toggle.  Not sure how to remedy that.
        HWND hwnd = GetParent(hwndTextArea);
        HBRUSH hbr = CreateSolidBrush(rgb);
        SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)hbr);
    }
}

void ReadWindowInfo(char* serialized_state, WindowInfo* window_info) {
    ZeroMemory(window_info, sizeof(WindowInfo));

    if (!serialized_state || !serialized_state[0]) {
        window_info->fullscreen = FALSE;
        return;
    }

    sscanf_s(serialized_state,
             "%d,%d,%d,%d,%d,%d,%d,%d",
             &window_info->fullscreen,
             &window_info->maximized,
             &window_info->style,
             &window_info->exstyle,
             &window_info->rc.left,
             &window_info->rc.top,
             &window_info->rc.right,
             &window_info->rc.bottom);
}

void WriteWindowInfo(WindowInfo* window_info) {
    sprintf_s(g_serialized_window_info,
              "%d,%d,%d,%d,%d,%d,%d,%d",
              window_info->fullscreen,
              window_info->maximized,
              window_info->style,
              window_info->exstyle,
              window_info->rc.left,
              window_info->rc.top,
              window_info->rc.right,
              window_info->rc.bottom);
}
