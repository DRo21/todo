#include <windows.h>
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

#define ID_BUTTON_ADD 1
#define ID_EDIT_INPUT 2
#define ID_LISTVIEW 3

HFONT hFont;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // Initialize common controls for ListView
    INITCOMMONCONTROLSEX icex = { sizeof(INITCOMMONCONTROLSEX), ICC_LISTVIEW_CLASSES };
    InitCommonControlsEx(&icex);

    const wchar_t CLASS_NAME[] = L"TodoWindowClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Reminders",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 450, 600,
        nullptr, nullptr, hInstance, nullptr
    );

    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND hEdit, hButton, hListView;
    static int taskIndex = 0;

    switch (uMsg) {
    case WM_CREATE: {
        // Create font for iOS-like appearance
        hFont = CreateFontW(18, 0, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
            VARIABLE_PITCH, L"Segoe UI");

        // Input box
        hEdit = CreateWindowEx(0, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            20, 20, 300, 30,
            hwnd, (HMENU)ID_EDIT_INPUT, nullptr, nullptr);

        // Add button
        hButton = CreateWindow(L"BUTTON", L"+",
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            330, 20, 50, 30,
            hwnd, (HMENU)ID_BUTTON_ADD, nullptr, nullptr);

        // ListView with checkboxes
        hListView = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, L"",
            WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
            20, 70, 360, 450,
            hwnd, (HMENU)ID_LISTVIEW, nullptr, nullptr);

        // Apply fonts
        SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hButton, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hListView, WM_SETFONT, (WPARAM)hFont, TRUE);

        // Enable ListView features
        ListView_SetExtendedListViewStyle(hListView,
            LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);

        // Add single column
        LVCOLUMN col = {};
        col.mask = LVCF_TEXT | LVCF_WIDTH;
        col.pszText = (LPWSTR)L"Task";
        col.cx = 320;
        ListView_InsertColumn(hListView, 0, &col);

        return 0;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == ID_BUTTON_ADD) {
            wchar_t buffer[256];
            GetWindowText(hEdit, buffer, 256);
            if (wcslen(buffer) > 0) {
                LVITEM item = {};
                item.mask = LVIF_TEXT;
                item.iItem = taskIndex++;
                item.pszText = buffer;
                ListView_InsertItem(hListView, &item);
                SetWindowText(hEdit, L"");
            }
        }
        return 0;

    case WM_DESTROY:
        DeleteObject(hFont);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
