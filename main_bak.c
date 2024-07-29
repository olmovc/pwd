#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <windows.h>
#include <WINDOWSX.H>
#include <commctrl.h>

#define ID_BTN_OPEN     1001
#define ID_BTN_RUN      1002
#define ID_EDIT_ZIP     1003
#define ID_EDIT_PASSW   1004
#define ID_EDIT_ABC     1005


static HWND hOpen, hRun, heditPassword, heditZipExe;
/*  Declare Windows procedure  */
static LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
static void Main_OnCommand(HWND, int, HWND, UINT);
void create_ui(HWND hwnd);
static HANDLE ghInstance;

int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,   int nCmdShow)
{
    INITCOMMONCONTROLSEX icex;


    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */
    ghInstance = hThisInstance;
    //--------
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC  = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);
    //--------
    /* The Window structure */
    wincl.hInstance = ghInstance;
    wincl.lpszClassName = L"7zip_password";
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           wincl.lpszClassName,         /* Classname */
           L"Code::Blocks Template Windows App",       /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           544,                 /* The programs width */
           375,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

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
      HANDLE_MSG(hwnd, WM_COMMAND, Main_OnCommand);

      case WM_CREATE:
      {
        create_ui(hwnd);
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

void create_ui(HWND hwnd)
{
  hOpen = CreateWindowW(L"Button", L"Open",
                WS_VISIBLE | WS_CHILD ,
                10, 10, 80, 20, hwnd, (HMENU) ID_BTN_OPEN, NULL, NULL);

  hRun = CreateWindowW(L"Button", L"Quit",
                WS_VISIBLE | WS_CHILD ,
                10, 35, 80, 20, hwnd, (HMENU) ID_BTN_RUN, NULL, NULL);

  heditZipExe = CreateWindowW(L"Edit", L"pat to 7zip", WS_CHILD | WS_VISIBLE | WS_BORDER,
                95, 10, 150, 20, hwnd, (HMENU) ID_EDIT_ZIP, NULL, NULL);

  heditPassword = CreateWindowW(L"Edit", L"password", WS_VISIBLE | WS_CHILD,
                95, 35, 150, 20, hwnd, (HMENU) ID_EDIT_PASSW, NULL, NULL);
}

static void Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
 // MessageBoxW(0,L"dfasfs",L"dddd",MB_OK);
    switch (id)
    {
        case ID_BTN_OPEN:
            MessageBoxW(0,L"dfasfs",L"dddd",MB_OK);
            //DialogBox(ghInstance, MAKEINTRESOURCE(DLG_ABOUT), hwnd, (DLGPROC)AboutDlgProc);

        /* TODO: Enter more commands here */
    }
}
