VERSION 5.00
Object = "{22D6F304-B0F6-11D0-94AB-0080C74C7E95}#1.0#0"; "msdxm.ocx"
Begin VB.Form frmMP3 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Open Folder & MP3 Player"
   ClientHeight    =   5100
   ClientLeft      =   150
   ClientTop       =   720
   ClientWidth     =   7305
   Icon            =   "frmMP3.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   5100
   ScaleWidth      =   7305
   StartUpPosition =   3  'Windows Default
   Begin VB.Timer Timer6 
      Enabled         =   0   'False
      Interval        =   60
      Left            =   3960
      Top             =   3480
   End
   Begin VB.Frame Frame1 
      Caption         =   "MP3 Info"
      Height          =   3495
      Left            =   120
      TabIndex        =   6
      Top             =   840
      Width           =   3615
      Begin VB.CommandButton cmdSaveInfo 
         Caption         =   "MP3 Information Save"
         Height          =   255
         Left            =   120
         TabIndex        =   18
         Top             =   3120
         Width           =   3375
      End
      Begin VB.TextBox txtGenreCode 
         Height          =   315
         Left            =   2880
         TabIndex        =   15
         Top             =   2640
         Width           =   615
      End
      Begin VB.TextBox txtComment 
         Height          =   315
         Left            =   840
         TabIndex        =   14
         Top             =   2160
         Width           =   2655
      End
      Begin VB.TextBox txtYear 
         Height          =   315
         Left            =   840
         TabIndex        =   13
         Top             =   1680
         Width           =   2655
      End
      Begin VB.TextBox txtAlbum 
         Height          =   315
         Left            =   840
         TabIndex        =   12
         Top             =   1200
         Width           =   2655
      End
      Begin VB.TextBox txtArtist 
         Height          =   315
         Left            =   840
         TabIndex        =   11
         Top             =   720
         Width           =   2655
      End
      Begin VB.TextBox txtTitle 
         Height          =   315
         Left            =   840
         TabIndex        =   10
         Top             =   240
         Width           =   2655
      End
      Begin VB.ComboBox Combo1 
         Height          =   315
         Left            =   840
         TabIndex        =   7
         Text            =   "Genres"
         Top             =   2640
         Width           =   1935
      End
      Begin VB.Label Label8 
         Caption         =   "Genres "
         Height          =   255
         Left            =   120
         TabIndex        =   24
         Top             =   2760
         Width           =   735
      End
      Begin VB.Label Label7 
         Caption         =   "Remark"
         Height          =   255
         Left            =   120
         TabIndex        =   23
         Top             =   2280
         Width           =   735
      End
      Begin VB.Label Label6 
         Caption         =   "Date"
         Height          =   255
         Left            =   120
         TabIndex        =   22
         Top             =   1800
         Width           =   735
      End
      Begin VB.Label Label5 
         Caption         =   "Album"
         Height          =   255
         Left            =   120
         TabIndex        =   21
         Top             =   1320
         Width           =   735
      End
      Begin VB.Label Label4 
         Caption         =   "Musician"
         Height          =   255
         Left            =   120
         TabIndex        =   20
         Top             =   840
         Width           =   855
      End
      Begin VB.Label Label1 
         Caption         =   "Title"
         Height          =   255
         Left            =   120
         TabIndex        =   19
         Top             =   360
         Width           =   735
      End
   End
   Begin VB.CommandButton cmdSelect 
      Caption         =   "Select Folder"
      Height          =   255
      Left            =   120
      TabIndex        =   4
      Top             =   600
      Width           =   3615
   End
   Begin VB.ListBox PlayList 
      Height          =   3180
      Left            =   3840
      TabIndex        =   1
      Top             =   360
      Width           =   3450
   End
   Begin VB.ListBox List1 
      Height          =   255
      Left            =   120
      TabIndex        =   0
      Top             =   360
      Width           =   3600
   End
   Begin VB.FileListBox File1 
      Height          =   285
      Left            =   3600
      TabIndex        =   5
      Top             =   3600
      Visible         =   0   'False
      Width           =   3615
   End
   Begin VB.Frame Frame2 
      BorderStyle     =   0  'None
      Caption         =   "Frame2"
      Height          =   4095
      Left            =   120
      TabIndex        =   9
      Top             =   360
      Width           =   7215
      Begin VB.CommandButton cmdOpen 
         Caption         =   "▲"
         Height          =   255
         Left            =   6600
         TabIndex        =   25
         ToolTipText     =   "선택된 MP3 열기"
         Top             =   3720
         Width           =   375
      End
      Begin VB.Label lblTotalTime 
         Caption         =   "Total Time:"
         Height          =   255
         Left            =   3720
         TabIndex        =   17
         Top             =   3720
         Width           =   1335
      End
      Begin VB.Label lblElapsedTime 
         Caption         =   "Time :"
         Height          =   255
         Left            =   5160
         TabIndex        =   16
         Top             =   3720
         Width           =   1335
      End
   End
   Begin VB.Label Label3 
      Caption         =   "MP3 List :"
      Height          =   360
      Left            =   3825
      TabIndex        =   2
      Top             =   135
      Width           =   1095
   End
   Begin VB.Label Label2 
      Caption         =   "MP3 Open Folder :"
      Height          =   330
      Left            =   120
      TabIndex        =   3
      Top             =   120
      Width           =   2055
   End
   Begin MediaPlayerCtl.MediaPlayer MediaPlayer1 
      Height          =   4575
      Left            =   120
      TabIndex        =   8
      Top             =   480
      Width           =   7095
      AudioStream     =   -1
      AutoSize        =   0   'False
      AutoStart       =   -1  'True
      AnimationAtStart=   -1  'True
      AllowScan       =   -1  'True
      AllowChangeDisplaySize=   -1  'True
      AutoRewind      =   0   'False
      Balance         =   0
      BaseURL         =   ""
      BufferingTime   =   5
      CaptioningID    =   ""
      ClickToPlay     =   -1  'True
      CursorType      =   0
      CurrentPosition =   -1
      CurrentMarker   =   0
      DefaultFrame    =   ""
      DisplayBackColor=   0
      DisplayForeColor=   16777215
      DisplayMode     =   0
      DisplaySize     =   4
      Enabled         =   -1  'True
      EnableContextMenu=   -1  'True
      EnablePositionControls=   -1  'True
      EnableFullScreenControls=   0   'False
      EnableTracker   =   -1  'True
      Filename        =   ""
      InvokeURLs      =   -1  'True
      Language        =   -1
      Mute            =   0   'False
      PlayCount       =   1
      PreviewMode     =   0   'False
      Rate            =   1
      SAMILang        =   ""
      SAMIStyle       =   ""
      SAMIFileName    =   ""
      SelectionStart  =   -1
      SelectionEnd    =   -1
      SendOpenStateChangeEvents=   -1  'True
      SendWarningEvents=   -1  'True
      SendErrorEvents =   -1  'True
      SendKeyboardEvents=   0   'False
      SendMouseClickEvents=   0   'False
      SendMouseMoveEvents=   0   'False
      SendPlayStateChangeEvents=   -1  'True
      ShowCaptioning  =   0   'False
      ShowControls    =   -1  'True
      ShowAudioControls=   -1  'True
      ShowDisplay     =   0   'False
      ShowGotoBar     =   0   'False
      ShowPositionControls=   -1  'True
      ShowStatusBar   =   0   'False
      ShowTracker     =   -1  'True
      TransparentAtStart=   0   'False
      VideoBorderWidth=   0
      VideoBorderColor=   0
      VideoBorder3D   =   0   'False
      Volume          =   -600
      WindowlessVideo =   0   'False
   End
   Begin VB.Menu mnuOptionEnd 
      Caption         =   "Quit"
   End
End
Attribute VB_Name = "frmMP3"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

'# **********************
'# ****MP3 PLAYER VAR****
'# **********************

Dim min As Integer
Dim sec As Integer
Dim SharedPath As String
Dim IniPath As String
Dim FileName As String
Dim FileOpen As Boolean
Dim IndexHtml As String

Private Declare Function GetComputerName Lib "kernel32" _
        Alias "GetComputerNameA" (ByVal lpBuffer As String, nSize As Long) As Long
      

Private Sub SelectedFolder()
    
    Dim ComputerName As String
    ComputerName = String(50, Chr(0))
    Call GetComputerName(ComputerName, 50)
        
    PlayList.Clear
    List1.Clear
    List1.AddItem (SharedPath)

    Dim Count As Integer, Count2 As Integer, Num As Integer ' Dim Integers I don't think that anyone will have more than 34 thousand Mp3s.
    Dim tmpString As String ' Define tmpString the string of the MP3.
    Num% = PlayList.ListCount + 1 ' Don't want it to Equal zero or the same number as the file before it.
    IndexHtml$ = "<html><body><table>"
    Call AppendFile(SharedPath & "\index.html", IndexHtml$)
    For Count% = 0 To List1.ListCount - 1 ' That nifty 0 to count - 1 again. This is for the directory
    File1.Path = List1.List(Count%) ' Make file1's path equal the directory its adding from
    For Count2% = 0 To File1.ListCount - 1 ' Number of files to add... Nifty
        If UCase(Right(File1.List(Count2%), 5)) = ".HTML" Then GoTo html
        If UCase(Right(File1.List(Count2%), 4)) = ".MP3" Then ' Find .mp3 extentions
        ' if no .mp3 file extention found, then don't add the file.
        tmpString$ = (List1.List(Count%) & "\" & File1.List(Count2%)) ' Make tmpString
        ' Equal the entire path including the filename. My Mp3 player removes the .mp3 after the next
        ' do so if the file doesn't have an ID3 then
        Do: DoEvents
            If InStr(tmpString$, "\") = False Then Exit Do ' If tmpString doesn't have a \ then
            ' exit he do. This is like impossible but to get out of the do, nessasary.
            tmpString$ = Mid(tmpString$, InStr(tmpString$, "\") + 1, Len(tmpString$) - InStr(tmpString$, "\") + 1)
            ' ^ to remove the \'s and text between.
        Loop
        PlayList.AddItem (Num% & ". " & tmpString$) ' Add to the playlist
        Num% = Num% + 1 ' Make the file number go up.
        '!!!!!!! Everything below this is the same as above.! !!!!!!
        ElseIf UCase(Right(File1.List(Count2%), 4)) = ".Mp3" Then
        tmpString$ = (List1.List(Count%) & "\" & File1.List(Count2%))
        Do: DoEvents
            If InStr(tmpString$, "\") = False Then Exit Do
            tmpString$ = Mid(tmpString$, InStr(tmpString$, "\") + 1, Len(tmpString$) - InStr(tmpString$, "\") + 1)
        Loop
        PlayList.AddItem (Num% & ". " & tmpString$)
        Num% = Num% + 1
        ElseIf UCase(Right(File1.List(Count2%), 4)) = ".mP3" Then
        tmpString$ = (List1.List(Count%) & "\" & File1.List(Count2%))
        Do: DoEvents
            If InStr(tmpString$, "\") = False Then Exit Do
            tmpString$ = Mid(tmpString$, InStr(tmpString$, "\") + 1, Len(tmpString$) - InStr(tmpString$, "\") + 1)
        Loop
        PlayList.AddItem (Num% & ". " & tmpString$)
        Num% = Num% + 1
        ElseIf UCase(Right(File1.List(Count2%), 4)) = ".mp3" Then
        tmpString$ = (List1.List(Count%) & "\" & File1.List(Count2%))
        Do: DoEvents
            If InStr(tmpString$, "\") = False Then Exit Do
            tmpString$ = Mid(tmpString$, InStr(tmpString$, "\") + 1, Len(tmpString$) - InStr(tmpString$, "\") + 1)
        Loop
        PlayList.AddItem (Num% & ". " & tmpString$)
        Num% = Num% + 1
        End If
        IndexHtml$ = "<tr>"
        IndexHtml$ = IndexHtml$ & "<td>"
        IndexHtml$ = IndexHtml$ & "<a href=" & Chr(34) & tmpString$ & Chr(34) & ">" & tmpString$ & "</a>"
        IndexHtml$ = IndexHtml$ & "</td>"
        IndexHtml$ = IndexHtml$ & "<td>"
        IndexHtml$ = IndexHtml$ & FileLen(SharedPath & "\" & tmpString$)
        IndexHtml$ = IndexHtml$ & "</td>"
        IndexHtml$ = IndexHtml$ & "<td>"
        IndexHtml$ = IndexHtml$ & Trim(Replace(ComputerName, Chr(0), "", 1, Len(ComputerName), vbTextCompare))
        IndexHtml$ = IndexHtml$ & "</td>"
        IndexHtml$ = IndexHtml$ & "</tr>"
        
        Call AppendFile(SharedPath & "\index.html", IndexHtml$)
html:
    Next Count2% ' If there is another file in the directory check it
    Next Count% ' See if there are any more directorys to cycle through.
    IndexHtml$ = "</table></body></html>"
    Call AppendFile(SharedPath & "\index.html", IndexHtml$)
End Sub


Private Sub cmdOpen_Click()
    With MediaPlayer1
        If Not FileOpen Then
            .FileName = FileName
            .AutoStart = False
            cmdOpen.Caption = "■"
            cmdOpen.ToolTipText = "Close MP3 in Playing"
            Timer6.Enabled = True
        Else
            .Stop
            cmdOpen.Caption = "▲"
            FileOpen = False
            cmdOpen.ToolTipText = "Open MP3 that You Choose"
            Timer6.Enabled = False
        End If
    End With
End Sub

Private Sub cmdSelect_Click()
    Dim ret
    ret = BrowseForFolder(Me.hwnd, "Select your shared folder.")
    If ret = "" Then Exit Sub
    SharedPath = ret
    Call SaveValue(IniPath, "Folder", "SharedFolder", SharedPath)
    ' find .MP3 So I made it get all 4 possibilities it can be.
    Call SelectedFolder
End Sub


Private Sub Form_Load()
    Dim i As Integer
    
    GenreArray = Split(sGenreMatrix, "|")   ' we fill the array with the Genre's
    For i = LBound(GenreArray) To UBound(GenreArray)
        Combo1.AddItem GenreArray(i)        ' now fill the Combobox with the array, and voila, the code you
    Next                                    ' you recieve form the Genre part of the Type, represents the combobox Listindex =)
    
    IniPath = App.Path & "\" & "P2P.ini"
    
    If FileExists(IniPath) Then
        SharedPath = ReadValue(IniPath, "Folder", "SharedFolder")
        SelectedFolder
    Else
        Call SaveValue(IniPath, "Folder", "SharedFolder", "")
    End If
    
    cmdOpen.Enabled = False
    
End Sub

Private Sub Form_Unload(Cancel As Integer)
    frmMain.mnuSysTray.Enabled = True
    frmMain.Show
End Sub

Private Sub EraseTXTBoxes()
    txtTitle = ""
    txtArtist = ""
    txtAlbum = ""
    txtYear = ""
    txtComment = ""
    txtGenreCode = ""
    Combo1.ListIndex = -1
End Sub


Private Sub mnuOptionEnd_Click()
    Me.Hide
    frmMain.mnuSysTray.Enabled = True
    frmMain.Show
End Sub

Private Sub PlayList_Click()
    Dim Temp As String
    On Error Resume Next
    
    cmdOpen.Enabled = True
    
    EraseTXTBoxes
    
    FileName = SharedPath & "\" & Right(PlayList.Text, Len(PlayList.Text) - 1 - InStr(PlayList.Text, "."))
    
    If GetId3(FileName) Then                         ' Get the filename
        txtTitle = RTrim(id3Info.Title)            ' since the fields in the type are
        txtArtist = RTrim(id3Info.Artist)                  ' fixed lenght, we use Rtrim to cut the
        txtAlbum = RTrim(id3Info.Album)                   ' trailing bytes
        txtYear = RTrim(id3Info.sYear)
        txtComment = RTrim(id3Info.Comments)
        txtGenreCode = ""
        Combo1.ListIndex = id3Info.Genre        ' fill in all the correct info.
    End If
End Sub

Private Sub cmdSaveInfo_Click()
    If FileOpen Then
        MsgBox "Can't save because the file is opeing.", vbCritical, "MP3 Tag Save Error"
        Exit Sub
    Else:
        If txtTitle.Text = "" Then
            MsgBox "Choose MP3 file or enter the Title.", vbCritical, "MP3 Tag Save Error"
            Exit Sub
        End If
    End If
    
    id3Info.Title = txtTitle           ' just filling in the information into the type
    id3Info.Artist = txtArtist
    id3Info.Album = txtAlbum
    id3Info.sYear = txtYear
    id3Info.Comments = txtComment
    id3Info.Genre = Combo1.ListIndex
    On Error GoTo ErrHandle             ' If the file is writeprotected
    SaveId3 FileName, id3Info     ' Calling the Saveid3 function
    Exit Sub

ErrHandle:
    If Err.Number = 75 Then
        MsgBox "MP3 file is using or write protected."
    Else
        MsgBox Err.Description
    End If

End Sub

Private Sub MediaPlayer1_OpenStateChange(ByVal OldState As Long, ByVal NewState As Long)
    min = MediaPlayer1.Duration \ 60
    sec = MediaPlayer1.Duration - (min * 60)
    lblTotalTime = "Total Time : " & Format(min, "0#") & ":" & Format(sec, "0#") 'format time to 00:00
    FileOpen = CBool(NewState)
    MediaPlayer1.AutoStart = False
End Sub

Private Sub Timer6_Timer()
    min = MediaPlayer1.CurrentPosition \ 60
    sec = MediaPlayer1.CurrentPosition - (min * 60)
    If min > 0 Or sec > 0 Then
        lblElapsedTime = "Time : " & Format(min, "0#") & ":" & Format(sec, "0#")
    Else
        lblElapsedTime = "Time : 00:00"
    End If
End Sub
