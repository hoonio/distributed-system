Attribute VB_Name = "modMain"
Option Explicit

Public doingDL As Boolean
Public doingPLAY As Boolean
Public currIndex As Long

Private Type BrowseInfo
       hWndOwner As Long
       pIDLRoot As Long
       pszDisplayName As Long
       lpszTitle As Long
       ulFlags As Long
       lpfnCallback As Long
       lParam As Long
       iImage As Long
End Type

Global Const MAX_PATH = 260

Private Const BIF_RETURNONLYFSDIRS = 1

Private Declare Sub CoTaskMemFree Lib "ole32.dll" (ByVal hMem As Long)

Private Declare Function lstrcat Lib "kernel32" Alias "lstrcatA" _
       (ByVal lpString1 As String, ByVal lpString2 As String) As Long

Private Declare Function SHBrowseForFolder Lib "shell32" _
       (lpbi As BrowseInfo) As Long
       
Private Declare Function SHGetPathFromIDList Lib "shell32" _
       (ByVal pidList As Long, ByVal lpBuffer As String) As Long
       
Public Function BrowseForFolder(hWndOwner As Long, sPrompt As String) As String

    Dim iNull As Integer
    Dim lpIDList As Long
    Dim lResult As Long
    Dim sPath As String
    Dim udtBI As BrowseInfo
    With udtBI
        .hWndOwner = hWndOwner
        .lpszTitle = lstrcat(sPrompt, "")
        .ulFlags = BIF_RETURNONLYFSDIRS
    End With

    lpIDList = SHBrowseForFolder(udtBI)

    If lpIDList Then
        sPath = String$(MAX_PATH, 0)
        lResult = SHGetPathFromIDList(lpIDList, sPath)
        Call CoTaskMemFree(lpIDList)
        iNull = InStr(sPath, vbNullChar)

        If iNull Then
            sPath = Left$(sPath, iNull - 1)
        End If
    End If

    BrowseForFolder = sPath
End Function


Public Function FileExists(FullFileName As String) As Boolean
    On Error GoTo MakeF
        'If file does Not exist, there will be an Error
        Open FullFileName For Input As #1
        Close #1
        'no error, file exists
        FileExists = True
    Exit Function
MakeF:
    'error, file does Not exist
    FileExists = False
End Function

Public Function LoadFile(FileName As String) As String
    Dim Data As String
    On Error GoTo hell
    Open FileName For Binary Access Read As #1 'Opens Requested file
    Data = Space(LOF(1))
    Get #1, , Data
    LoadFile = Data
    Close #1

hell:
    If Err.Number = 76 Then LoadFile = "파일을 찾을 수 없습니다."
End Function

Public Sub AppendFile(FileName As String, myText As String)
    Dim FileNum As Integer
    FileNum = FreeFile
    Open FileName For Append As FileNum
    Print #FileNum, myText
    Close FileNum
End Sub

Public Sub DeleteFile(FileName As String)
    On Error GoTo DelError
    Kill FileName
    Exit Sub
DelError:
    MsgBox "Error deleting File"
    Resume
End Sub
