Attribute VB_Name = "modSysTray"
Private nid As NOTIFYICONDATA
Private Declare Function Shell_NotifyIcon Lib "shell32" Alias "Shell_NotifyIconA" (ByVal dwMessage As Long, pnid As NOTIFYICONDATA) As Boolean
Private Const WM_CHAR = &H102
Private Const WM_SETTEXT = &HC
Private Const WM_USER = &H400
Private Const WM_KEYDOWN = &H100
Private Const WM_KEYUP = &H101
Private Const WM_LBUTTONDOWN = &H201
Private Const WM_CLOSE = &H10
Private Const WM_COMMAND = &H111
Private Const WM_CLEAR = &H303
Private Const WM_DESTROY = &H2
Private Const WM_GETTEXT = &HD
Private Const WM_GETTEXTLENGTH = &HE
Private Type NOTIFYICONDATA
    cbSize As Long
    hwnd As Long
    uId As Long
    uFlags As Long
    uCallBackMessage As Long
    hIcon As Long
    szTip As String * 64
    End Type
    
    Private Const NIM_ADD = &H0
    Private Const NIM_MODIFY = &H1
    Private Const NIM_DELETE = &H2
    Private Const NIF_MESSAGE = &H1
    Private Const NIF_ICON = &H2
    Private Const NIF_TIP = &H4
    Private Const WM_MOUSEMOVE = &H200

Public Declare Function SetForegroundWindow Lib "user32" (ByVal hwnd As Long) As Long

Function AddIcon(frm As Form, tip As String)

    With nid
        .cbSize = Len(nid)
        .hwnd = frm.hwnd
        .uId = vbNull
        .uFlags = NIF_ICON Or NIF_TIP Or NIF_MESSAGE
        .uCallBackMessage = WM_MOUSEMOVE
        .szTip = tip$ & vbNullChar
        .hIcon = frm.Icon
    End With
    
    Shell_NotifyIcon NIM_ADD, nid
End Function

Function DeleteIcon()
    Shell_NotifyIcon NIM_DELETE, nid
End Function

