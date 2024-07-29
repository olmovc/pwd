#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <wchar.h>
#include <windows.h>
#include <WINDOWSX.H>
#include <commctrl.h>

#define ID_BTN_OPEN         1001
#define ID_BTN_RUN          1002
#define ID_EDIT_ABC         1003
#define ID_EDIT_7ZIP        1004
#define ID_EDIT_ZIP         1005
#define ID_EDIT_PASSWORD     1005
#define ID_CB_MIN           1006
#define ID_STAT_MIN           1009
#define ID_CB_MAX           1007
#define ID_STAT_MAX           1008
#define ID_CB_LEN           1010
#define ID_STAT_LEN           1011
#define ID_STAT_CURRENT        1012
#define ID_EDIT_CURRENT        1013
#define ID_STAT_ABC           1014
#define ID_BTN_ZIP         1015
#define ID_BTN_START         1016
#define TIMER1           1017

#define MAX_ABC            40
#define BUFF_LEN           50
//#define MAX_PASSWORD_LEN   40

static char alphabet[MAX_ABC] = {'\0'};// = "abcde";
static char zip_exe_path[MAX_PATH] = {'\0'};
static char archive_path[MAX_PATH] = {'\0'};
static char command_str[MAX_PATH] =  {'\0'};
static size_t password_count = 0;
static long long paswd_min_len, paswd_max_len, paswd_len;

//static size_t password_count = 0;
static size_t time_show = 20;
static int alphabet_len = 0;

static clock_t clock_time;

char* convert_value(int num, int diclen);
int generate(int min_len, int max_len, int len);
int run(char * cmdstr);

static void DoEvents();

static HWND hOpen, hZip, hZipExe, hABC;
static HWND hStatABC, hEditZip, hStart;
static HWND hMin, hStatMin, hStatMax, hMax,hStatLen, hLen;//, hZipExe, hABC;
static HWND hCurrent, hStatCur;//, hStatMax, hMax,hStatLen, hLen;
static HWND hwin, hLabel;

/*  Declare Windows procedure  */
static LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
static void Main_OnCommand(HWND, int, HWND, UINT);
BOOL _OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
static void OpenDialog(HWND, int);
static int check(char * txt);
static HANDLE ghInstance;

//int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance,
//                   LPSTR lpszArgument,   int nCmdShow)
int WINAPI wWinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance,
                    PWSTR lpszArgument, int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl = {0};        /* Data structure for the windowclass */
    ghInstance = hThisInstance;
    /* The Window structure */
    wincl.hInstance = ghInstance;
    wincl.lpszClassName = L"7zip_password";
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    //wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = GetSysColorBrush(COLOR_3DFACE);//(HBRUSH) COLOR_WINDOW;// BTNFACE;// 3DFACE;// BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
               0,                   /* Extended possibilites for variation */
               wincl.lpszClassName,         /* Classname */
               L"recovering a forgotten password",       /* Title Text */
               WS_SYSMENU | WS_MINIMIZEBOX, /* default window */
               100,       /* Windows decides the position */
               50,       /* where the window ends up on the screen */
               465,                 /* The programs width */
               240,                 /* and height in pixels */
               HWND_DESKTOP,        /* The window is a child-window to desktop */
               NULL,                /* No menu */
               hThisInstance,       /* Program Instance handler */
               NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);
    hwin = hwnd;

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

/*  This function is called by the Windows function DispatchMessage()  */

static LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{


    switch (message)                  /* handle the messages */
    {
        HANDLE_MSG (hwnd, WM_CREATE, _OnCreate);
        HANDLE_MSG(hwnd, WM_COMMAND, Main_OnCommand);
        break;
//case WM_TIMER:
//       //if (codeNotify == WM_TIMER) {
//       InvalidateRect(hwnd, NULL, TRUE);
//       UpdateWindow(hCurrent);

    //break;
    case WM_DESTROY:
        PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
        break;

    default:                      /* for messages that we don't deal with */
        return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

static void Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
// MessageBoxW(0,L"dfasfs",L"dddd",MB_OK);
    wchar_t buffer[3];// = {'\0'};
    char szbuffer[100] = {'\0'};
    //char buffer[3];
//char * str = NULL;

    switch (id)
    {
    case ID_BTN_START:
        if (!check(alphabet))
          {
            MessageBoxW(0,L"field ",L"Error",MB_OK);
          }
          clock_time = clock();
          int ret = -1;
          ret = generate(paswd_min_len, paswd_max_len, paswd_len);
          if (ret == -1)
            SetWindowTextA(hCurrent, "password not found");

          clock_time = clock() - clock_time;
         swprintf(szbuffer,60, L"s=%lu", clock_time);
         MessageBoxA(hwnd,szbuffer,"dgd",0);
         SetWindowTextA(hLabel,szbuffer);
          //SetWindowTextA(hLabel,buffer);
        break;
    case ID_EDIT_ABC:
        if (codeNotify == EN_KILLFOCUS || codeNotify == EN_CHANGE)
        {
            GetWindowTextA(hABC, alphabet, MAX_PATH);
            SetWindowTextA(hCurrent, alphabet);
            alphabet_len = lstrlenA(alphabet);
        }
        break;
    case ID_BTN_ZIP:
        OpenDialog(hwnd, 1);
        SetWindowTextA(hEditZip, archive_path);
        break;
    case ID_BTN_OPEN:
        //MessageBoxW(0,L"dfasfs",L"dddd",MB_OK);
        OpenDialog(hwnd, 0);
        SetWindowTextA(hZipExe, zip_exe_path);
        //MessageBoxA(0,"dfasfs",zip_exe_path,MB_OK);
        break;
    //SendMessageW(hMin,CB_ADDSTRING,0,(LPARAM)L"items[i]");
    //DialogBox(ghInstance, MAKEINTRESOURCE(DLG_ABOUT), hwnd, (DLGPROC)AboutDlgProc);
    case ID_CB_MIN:
        if (codeNotify == CBN_SELCHANGE)
        {
            LRESULT sel = SendMessage(hMin, CB_GETCURSEL, 0, 0);
            swprintf(buffer,3, L"%lu", sel);
            paswd_min_len = sel;
            //MessageBoxW(0,buffer,L"dddd",MB_OK);
            //ultoa(sel, buffer, 10);
            SetWindowTextW(hCurrent, buffer);
        }
        //MessageBoxW(0,L"ID_CB_MIN",L"ID_CB_MIN",MB_OK);
        break;
    case ID_CB_MAX:
        if (codeNotify == CBN_SELCHANGE)
        {
            LRESULT sel = SendMessage(hMax, CB_GETCURSEL, 0, 0);
            swprintf(buffer,3, L"%lu", sel);
            paswd_max_len = sel;
            //MessageBoxW(0,buffer,L"dddd",MB_OK);
            //ultoa(sel, buffer, 10);
            SetWindowTextW(hCurrent, buffer);
        }
        //MessageBoxW(0,L"ID_CB_MIN",L"ID_CB_MIN",MB_OK);
        break;
    case ID_CB_LEN:
        if (codeNotify == CBN_SELCHANGE)
        {
            LRESULT sel = SendMessage(hLen, CB_GETCURSEL, 0, 0);
            swprintf(buffer,3, L"%lu", sel);
            paswd_len = sel;
            //MessageBoxW(0,buffer,L"dddd",MB_OK);
            //ultoa(sel, buffer, 10);
            SetWindowTextW(hCurrent, buffer);
        }
        //MessageBoxW(0,L"ID_CB_MIN",L"ID_CB_MIN",MB_OK);
        break;
        /* TODO: Enter more commands here */
    }
}

BOOL _OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    paswd_len = 0;
    paswd_max_len = paswd_min_len = 1;
    password_count = 0;

    // TODO: Add your message processing code here...
    const wchar_t *items[] = { L"0", L"1", L"2", L"3", L"4", L"5", L"6", L"7",
                               L"8", L"9"
                             };

    hOpen = CreateWindowW(L"Button", L"Open",
                          WS_VISIBLE | WS_CHILD,
                          10, 10, 80, 20, hwnd, (HMENU) ID_BTN_OPEN, NULL, NULL);

    hZipExe = CreateWindowW(L"Edit", NULL,
                            WS_VISIBLE | WS_CHILD,
                            95, 10, 350, 20, hwnd, (HMENU) ID_EDIT_7ZIP, NULL, NULL);

    hZip = CreateWindowW(L"Button", L"Open",
                         WS_VISIBLE | WS_CHILD,
                         10, 35, 80, 20, hwnd, (HMENU) ID_BTN_ZIP, NULL, NULL);

    hEditZip = CreateWindowW(L"Edit", NULL,
                             WS_VISIBLE | WS_CHILD,
                             95, 35, 350, 20, hwnd, (HMENU) ID_EDIT_ZIP, NULL, NULL);

    hStatABC = CreateWindowW(L"Static", L"Alphabet",
                             WS_VISIBLE | WS_CHILD,
                             10, 60, 80, 20, hwnd, (HMENU) ID_STAT_ABC, NULL, NULL);

    hABC = CreateWindowW(L"Edit", NULL,
                         WS_VISIBLE | WS_CHILD,
                         95, 60, 350, 20, hwnd, (HMENU) ID_EDIT_ABC, NULL, NULL);
    //------------------------------------------------------------------
    hStatMin = CreateWindowW(L"Static", L"Min length",
                             WS_VISIBLE | WS_CHILD | SS_RIGHT,// | SS_CENTER,
                             10, 85, 80, 20, hwnd, (HMENU) ID_STAT_MIN, ghInstance, NULL);
    //ghInstance
    hMin = CreateWindowW(L"Combobox", L"",
                         WS_VISIBLE | WS_CHILD | CBS_DROPDOWN | CBS_HASSTRINGS | WS_OVERLAPPED,
                         95, 85, 350, 220, hwnd, (HMENU) ID_CB_MIN, ghInstance, NULL);

    hStatMax = CreateWindowW(L"Static", L"Max length",
                             WS_VISIBLE | WS_CHILD | SS_RIGHT,// | SS_CENTER,
                             10, 110, 80, 20, hwnd, (HMENU) ID_STAT_MAX, ghInstance, NULL);

    hMax = CreateWindowW(L"Combobox", L"",
                         WS_VISIBLE | WS_CHILD | CBS_DROPDOWN | CBS_HASSTRINGS | WS_OVERLAPPED,
                         95, 110, 350, 220, hwnd, (HMENU) ID_CB_MAX, ghInstance, NULL);

    hStatLen = CreateWindowW(L"Static", L"Len",
                             WS_VISIBLE | WS_CHILD | SS_RIGHT,// | SS_CENTER,
                             10, 135, 80, 20, hwnd, (HMENU) ID_STAT_LEN, ghInstance, NULL);

    hLen = CreateWindowW(L"Combobox", L"",
                         WS_VISIBLE | WS_CHILD | CBS_DROPDOWN | CBS_HASSTRINGS | WS_OVERLAPPED,
                         95, 135, 350, 220, hwnd, (HMENU) ID_CB_LEN, ghInstance, NULL);

    for(int i = 0; i < 10; i++)
    {
        SendMessageW(hMin,CB_ADDSTRING,0,(LPARAM) items[i]);
        SendMessageW(hMax,CB_ADDSTRING,0,(LPARAM) items[i]);
        SendMessageW(hLen,CB_ADDSTRING,0,(LPARAM) items[i]);
    }
    SendMessageW(hMin,CB_SETCURSEL,1,0);
    SendMessageW(hMax,CB_SETCURSEL,2,0);
    SendMessageW(hLen,CB_SETCURSEL,0,0);
    hStatCur = CreateWindowW(L"Static", L"Current password",
                             WS_VISIBLE | WS_CHILD | SS_RIGHT,// | SS_CENTER,
                             10, 162, 80, 20, hwnd, (HMENU) ID_STAT_CURRENT, ghInstance, NULL);

    hCurrent = CreateWindowW(L"Edit", NULL,
                             WS_VISIBLE | WS_CHILD,
                             95, 162, 350, 20, hwnd, (HMENU) ID_EDIT_CURRENT, NULL, NULL);

    hStart = CreateWindowW(L"Button", L"Start",
                           WS_VISIBLE | WS_CHILD,
                           10, 180, 80, 20, hwnd, (HMENU) ID_BTN_START, NULL, NULL);

    hLabel = CreateWindowW(L"Static", L"sec.",
                           WS_VISIBLE | WS_CHILD,
                           95, 183, 80, 20, hwnd, (HMENU) ID_BTN_START, NULL, NULL);

    return TRUE;

}

void OpenDialog(HWND hwnd, int n)
{

    OPENFILENAMEA ofn;
    char szFile[MAX_PATH] = {0};
    char * ret; //[MAX_PATH] = {'\0'};
    ret = malloc(sizeof(char) * MAX_PATH);
    //ret[MAX_PATH] = '\0'

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.hwndOwner = hwnd;
    ofn.nMaxFile = sizeof(szFile);
    if (n == 0)
        ofn.lpstrFilter = "Exe files(*.exe)\0*.exe\0";
    else
        ofn.lpstrFilter = "All files(*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrFileTitle = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if(GetOpenFileNameA(&ofn))
    {
        if (n == 0)
        {
            strcpy(zip_exe_path,ofn.lpstrFile);//LoadFile(ofn.lpstrFile);
        }
        else
        {
            strcpy(archive_path,ofn.lpstrFile);
        }
        //SetWindowTextA(hCurrent, ret);
        return;
    }
    else
    {
        return;
    }
}

static int check(char * txt)
{
    int len = strlen(txt);
    if (len < 2)
      return 0;
    char * buffer = malloc((len + 1) * sizeof(char));
    strcpy(buffer, txt);
    char * pch;
    for (int i = 0; i < len; i++)
    {
        pch = strchr(txt + i + 1, buffer[i]);
        if (pch != NULL)
        {
            free(buffer);
            return 0;
        }
    }
    free(buffer);
    return 1;
}

char * convert_value(int num, int diclen)
{
    int len_array = 0;
    if (num == 0)
    {
        len_array = 1;
    }
    else
    {
        len_array = (int) floor(log(num)/log(diclen))+1;
    }
    char * buffer;
    int buffer_size = sizeof(char) * (len_array + 1);
    buffer = malloc(buffer_size);
    buffer[buffer_size - 1] = '\0';
    int i = buffer_size - 2;
    //printf("\nlenar=%d,bufsize=%d,i=%d",len_array,buffer_size,i);
    do
    {
        int m = (int) floor((double)(num % diclen));
        buffer[i--] = alphabet[m];
        num = (int) floor( (double)(num / diclen) );
    }
    while(num > 0);
    //string s = String.Join(string.Empty,buffer);
    return buffer;
}

char * pad_left(int length, char ch, char* in_str)
{
    int in_str_len;
    in_str_len = strlen(in_str);
    int pad_len = length - in_str_len ;// 1;

    char* ret;

    if (pad_len < 1) {
        return in_str;
    }

    ret = malloc((length + 1) * sizeof(char));
    memset(ret, '\0', (length + 1) * sizeof(char));
    memset(ret, ch, pad_len*sizeof(char));
    //memcpy(ret+length,in_str,in_str_len);
    strcat(ret, in_str);
    return ret;
}


int generate(int min_len, int max_len, int len)
{
    //char password[100] = {'\0'};
    //int alphabet_len = strlen(alphabet);
    int max_value = 0;// floor(pow(alphabet_len, len));
    char* pas_tmp = NULL;
    char* pas_pad = NULL;
    char buffer[BUFF_LEN] = {0};
    //char pas_pad_str[MAX_PASSWORD_LEN];
    int found = -1;
    if(len > 0)
    {
        max_value = floor(pow(alphabet_len, len));
        for(int n = 0; n < max_value; n++)
        {
            pas_tmp = convert_value(n, alphabet_len);
            pas_pad = pad_left(len,alphabet[0],pas_tmp);
            //padLeft(len,alphabet[0],pas_tmp,pas_pad_str);// pas_pad_str//int length, char pad, char* inStr,char* outStr)
            //strcat_s(command_str,FILENAME_MAX,pas_pad)
            sprintf_s(command_str, FILENAME_MAX, "%s t -p%s %s",zip_exe_path,pas_pad,archive_path);
            //printf("%-8llu password = %20s\n",password_count, pas_pad);
            password_count++;
            if (password_count % time_show == 0)
            {
                //gotoxy(1,7);
                sprintf_s(buffer,BUFF_LEN,"%llu password = %s\n",password_count, pas_pad);
                SetWindowTextA(hCurrent, buffer);
                InvalidateRect(hwin, NULL, TRUE);
                UpdateWindow(hCurrent);
                DoEvents();
                //printf("%-8llu password = %20s\n",password_count, pas_pad);
            }
            if (run(command_str) == 0)
            {
                //printf("Found password: %s\n", pas_pad);
                //free(pas_tmp);
                //free(pas_pad);
                //return 0;c
                //printf("Found password: %s\n", pas_pad);
                sprintf_s(buffer,BUFF_LEN,"Found password: %s\n", pas_pad);
                SetWindowTextA(hCurrent, buffer);
                //InvalidateRect(a);
                InvalidateRect(hwin, NULL, TRUE);
                UpdateWindow(hCurrent);
                DoEvents();
                found = 1;
                //break;
                free(pas_tmp);
                return found;
            }
        }
                        InvalidateRect(hwin, NULL, TRUE);
                UpdateWindow(hwin);
    }
    else
    {
        for(int j = min_len; j <= max_len; j++)
        {
            max_value = (int)floor(pow(alphabet_len, j));
            for(int k = 0; k < max_value; k++)
            {
                pas_tmp = convert_value(k, alphabet_len);
                //padLeft(len,alphabet[0],pas_tmp,pas_pad_str);
                pas_pad = pad_left(j,alphabet[0],pas_tmp);
                //strcat_s(command_str,FILENAME_MAX,pas_pad)
                sprintf_s(command_str, FILENAME_MAX, "%s t -p%s %s",zip_exe_path,pas_pad,archive_path);
                password_count++;
                if (password_count % time_show == 0)
                {
                    sprintf_s(buffer,BUFF_LEN,"%llu password = %s\n",password_count, pas_pad);
                    SetWindowTextA(hCurrent, buffer);
                    InvalidateRect(hwin, NULL, TRUE);                  //gotoxy(1,15);
                    UpdateWindow(hCurrent);
                    DoEvents();
                    //printf("%-8llu password = %20s\n",password_count, pas_pad);
                }
                //printf("%-8llu password = %20s\n",password_count, pas_pad);
                //MSG msg;
                if (run(command_str) == 0)
                {
                    sprintf_s(buffer,BUFF_LEN,"Found password: %s\n", pas_pad);
                    SetWindowTextA(hCurrent, buffer);
                    InvalidateRect(hwin, NULL, TRUE);
                    UpdateWindow(hCurrent);
                    //PeekMessageW(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
                    //PeekMessageW(NULL, NULL, 0, 0, PM_NOREMOVE );
                    DoEvents();
                    //printf("Found password: %s\n", pas_pad);
                    found = 1;
                    free(pas_tmp);
                    return found;
                    //break;
                }
            }
            // printf("\n----------BREAK-------------------\n");
           // if (found = 1)
             // break;
                InvalidateRect(hwin, NULL, TRUE);
                UpdateWindow(hwin);
        }
    }
    //printf("Password Not Found\n");
    if (pas_tmp != NULL)
    free(pas_tmp);

    //if(pas_pad != NULL)
     // free(pas_pad);
    return found;
}

int run(char * cmdstr)
{
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    DWORD exit_code;
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    si.wShowWindow = SW_SHOWMINIMIZED;
    ZeroMemory( &pi, sizeof(pi) );
    // Start the child process.
    if( !CreateProcessA( NULL,   // No module name (use command line)
                         cmdstr,   // Command line
                         NULL,           // Process handle not inheritable
                         NULL,           // Thread handle not inheritable
                         FALSE,          // Set handle inheritance to FALSE
                         CREATE_NO_WINDOW,              // No creation flags
                         NULL,           // Use parent's environment block
                         NULL,           // Use parent's starting directory
                         &si,            // Pointer to STARTUPINFO structure
                         &pi )           // Pointer to PROCESS_INFORMATION structure
      )
    {
        printf("CreateProcess failed (%lu).\n", GetLastError() );
        return -1;
    }

    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );

    GetExitCodeProcess(pi.hProcess, &exit_code);

    //printf("the execution of: \"%s\"\nreturns: %d\n", cmdstr, exit_code);

    // Close process and thread handles.
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
    return exit_code;
}


static void DoEvents()
{
    MSG msg;
    BOOL result;

    while ( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE ) )
    {
        result = GetMessage(&msg, NULL, 0, 0);
        if (result == 0) // WM_QUIT
        {
            PostQuitMessage(msg.wParam);
            break;
        }
        else if (result == -1)
        {
             // Handle errors/exit application, etc.
             PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

