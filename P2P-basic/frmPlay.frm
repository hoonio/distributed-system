VERSION 5.00
Object = "{22D6F304-B0F6-11D0-94AB-0080C74C7E95}#1.0#0"; "msdxm.ocx"
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form frmPlay 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "JukeBox"
   ClientHeight    =   5100
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   3570
   ControlBox      =   0   'False
   Icon            =   "frmPlay.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   NegotiateMenus  =   0   'False
   ScaleHeight     =   5100
   ScaleWidth      =   3570
   Begin VB.CommandButton cmdPlay 
      Caption         =   "▶ 연주"
      Height          =   300
      Left            =   2600
      TabIndex        =   1
      Top             =   4800
      Width           =   975
   End
   Begin VB.CommandButton cmdClear 
      Caption         =   "Clear"
      Height          =   300
      Left            =   1600
      TabIndex        =   0
      Top             =   4800
      Width           =   975
   End
   Begin VB.Timer Timer1 
      Interval        =   500
      Left            =   240
      Top             =   4680
   End
   Begin VB.Timer Timer6 
      Enabled         =   0   'False
      Interval        =   60
      Left            =   0
      Top             =   0
   End
   Begin MSComctlLib.ListView lvPlays 
      Height          =   4095
      Left            =   0
      TabIndex        =   4
      Top             =   0
      Width           =   3150
      _ExtentX        =   5556
      _ExtentY        =   7223
      View            =   3
      LabelEdit       =   1
      LabelWrap       =   -1  'True
      HideSelection   =   -1  'True
      _Version        =   393217
      ForeColor       =   -2147483640
      BackColor       =   -2147483643
      BorderStyle     =   1
      Appearance      =   1
      NumItems        =   1
      BeginProperty ColumnHeader(1) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         Text            =   "연주곡"
         Object.Width           =   5434
      EndProperty
   End
   Begin VB.Image Image1 
      Height          =   4050
      Left            =   3100
      Picture         =   "frmPlay.frx":2372
      Stretch         =   -1  'True
      Top             =   10
      Width           =   555
   End
   Begin VB.Label lblElapsedTime 
      Height          =   255
      Left            =   120
      TabIndex        =   3
      Top             =   4800
      Width           =   1335
   End
   Begin MediaPlayerCtl.MediaPlayer MediaPlayer1 
      Height          =   735
      Left            =   0
      TabIndex        =   2
      Top             =   3960
      Width           =   3650
      AudioStream     =   -1
      AutoSize        =   0   'False
      AutoStart       =   0   'False
      AnimationAtStart=   0   'False
      AllowScan       =   -1  'True
      AllowChangeDisplaySize=   0   'False
      AutoRewind      =   -1  'True
      Balance         =   0
      BaseURL         =   ""
      BufferingTime   =   5
      CaptioningID    =   ""
      ClickToPlay     =   0   'False
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
End
Attribute VB_Name = "frmPlay"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Dim FileOpen As Boolean
Dim sec As Integer
Dim min As Integer

Sub CreateClearWaveFile(ByRef pstrFileName)
    
    Dim llngIndex As Long
    Dim lbytTransferAry(0) As Byte
    Dim llngFileNum As Long
    Dim llngMaxIndex As Long
    Dim lstrData As String
    Dim lstrByte As String
    Dim llngPosition As Long
    
    
   
    lstrData = _
    "52 49 46 46 A0 04 00 00 57 41 56 45 66 6D 74 20" & _
    "32 00 00 00 02 00 02 00 22 56 00 00 27 57 00 00" & _
    "00 04 04 00 20 00 F4 03 07 00 00 01 00 00 00 02" & _
    "00 FF 00 00 00 00 C0 00 40 00 F0 00 00 00 CC 01" & _
    "30 FF 88 01 18 FF 66 61 63 74 04 00 00 00 A7 02" & _
    "00 00 64 61 74 61 00 04 00 00 02 02 28 00 37 00" & _
    "89 FF 6F 01 3D 01 76 00 4E 64 05 BF 1C C2 0B 70" & _
    "37 80 78 E7 FD 50 D4 EB 2F 22 71 F4 ED FD 4E 16" & _
    "F0 0F D0 4D 34 C1 FC 20 22 F1 CF 0D 20 42 E4 ED" & _
    "2E 02 00 21 02 2F 92 08 00 F0 00 00 20 F2 0F 10" & _
    "01 50 24 F1 FE EF 0D 00 00 F0 1E 03 F0 6F 27 E1" & _
    "0E 30 E4 DE 0F E0 3F 82 E8 5F 04 10 11 F1 0F 00" & _
    "20 D2 DC 0F 50 F4 BE 1C 33 14 F0 0F 11 F1 0E 10" & _
    "B1 EA 1F 31 D2 ED 7F 17 00 E0 1F 11 00 00 E0 CE" & _
    "ED 5E 16 F0 0F 40 34 F2 FE EF 3E F3 1F D1 8D 0A" & _
    "20 23 E2 1C 32 02 00 30 D3 BC ED 0E 12 D1 5C 06" & _
    "E0 0E 60 15 E1 EE 1F 21 E2 FE FF 0F 30 73 F7 DE" & _
    "2E 43 04 D0 FD 00 00 F0 0F 11 11 41 F4 0F 11 01" & _
    "C0 EC 1F 02 B0 EA 2F 42 23 E2 EE 7E 47 03 DF EB" & _
    "2F 02 E0 FE 4F 33 03 E0 0E 51 35 03 9F 08 10 01" & _
    "E0 0E 10 31 12 F1 FF 10 72 05 F0 CF 0B 20 12 F1" & _
    "DF 4D 44 33 F3 0E 00 00 F0 BF EA 0E 30 03 FF 10" & _
    "71 27 01 F0 1F 01 E0 DE AE 0B 22 02 F1 0F 61 37"

    lstrData = lstrData & _
    "03 D0 0D 00 F0 CF EC FE 40 45 02 00 30 64 05 F0" & _
    "0F 00 01 E0 ED 0E 31 32 43 04 20 73 07 E0 FE 00" & _
    "10 D1 ED 2F 63 36 13 00 10 62 06 F0 E0 0E 00 00" & _
    "F0 EF 4F 55 34 F3 0F 10 21 E2 FD D0 DE 2F 21 32" & _
    "C3 7C 37 23 02 20 23 D2 AE FA 30 F4 00 F0 5F 35" & _
    "32 F3 CE 3D 34 02 80 09 00 10 01 10 02 F0 2F 54" & _
    "14 C0 0B 20 22 11 00 00 00 70 16 01 FF 3F 04 10" & _
    "31 11 C1 2E 73 07 00 00 51 15 00 F0 10 02 10 12" & _
    "12 D1 EF 70 07 00 00 40 34 12 11 F1 10 F2 70 07" & _
    "00 10 31 54 13 10 22 13 01 20 13 12 03 61 F5 0F" & _
    "01 10 73 07 01 00 50 06 10 01 00 31 43 33 F3 1E" & _
    "71 17 F1 0F 30 66 F3 FE 30 24 F1 0F 00 00 41 04" & _
    "00 E0 20 22 C1 0F 20 15 D0 5E 45 03 00 42 44 26" & _
    "F1 00 00 00 20 01 90 FB 60 07 00 01 00 01 00 30" & _
    "14 E1 DF 5F 24 E2 0E 31 43 15 32 E2 CF FD 50 F7" & _
    "BF 0E 10 03 40 56 24 F2 FF 50 26 01 E0 2F 12 01" & _
    "50 24 E1 4E 55 04 E0 0D 61 F5 EE 0E F0 20 03 20" & _
    "04 00 40 64 15 01 20 13 01 F0 0F 12 02 3F 05 F1" & _
    "30 72 35 E3 0C 60 06 E0 BE 0D 12 02 F0 ED 70 37" & _
    "14 01 00 00 00 10 F2 EE 0E 32 05 10 72 26 32 32" & _
    "13 01 E0 EF 80 DB 0C 01 11 12 21 42 03 11 13 13" & _
    "D1 FE 10 D1 0E 11 32 04 51 66 14 11 12 01 00 10" & _
    "21 F3 E0 40 46 02 D0 5F 35 22 F1 CF 0E 51 35 12" & _
    "00 00 42 45 02 B0 EC 1F 23 02 10 43 76 56 13 01"
    
    lstrData = lstrData & _
    "00 20 23 F2 DE 0D 21 12 12 12 23 02 21 42 25 01" & _
    "20 12 E0 2F 44 53 25 02 10 31 75 45 04 00 00 00" & _
    "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" & _
    "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" & _
    "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" & _
    "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" & _
    "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" & _
    "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" & _
    "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" & _
    "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" & _
    "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" & _
    "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" & _
    "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" & _
    "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" & _
    "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" & _
    "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" & _
    "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" & _
    "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" & _
    "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" & _
    "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" & _
    "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" & _
    "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" & _
    "00 00 00 00 00 00 00 00 00 00 4C 49 53 54 46 00" & _
    "00 00 49 4E 46 4F 49 43 52 44 0B 00 00 00 31 39"
    
    lstrData = lstrData & _
    "39 36 2D 30 32 2D 30 37 00 00 49 45 4E 47 0D 00" & _
    "00 00 42 69 6C 6C 20 57 6F 6C 66 6F 72 64 00 00" & _
    "49 53 46 54 10 00 00 00 53 6F 75 6E 64 20 46 6F" & _
    "72 67 65 20 34 2E 30 00"

    ' Remove white space (only there for readablity)
    lstrData = Replace(lstrData, " ", "")
    
    ' Determine max number of hex characters
    '
    llngMaxIndex = Len(lstrData)
    
    ' Ignore errors
    On Error Resume Next
    
    ' Attempt to delete existing file (causes an error if not exists)
    FileSystem.Kill pstrFileName
    
    ' stop ignoring errors
    On Error GoTo 0
    
    ' Get a reference number to use for conn
    '     ecting to file
    llngFileNum = FreeFile
    
    ' Open file to be written
    Open pstrFileName For Binary As llngFileNum
    
    ' Loop through each hex byte value (byte
    '     = 2 hex characters)


    For llngIndex = 1 To llngMaxIndex Step 2
        
        ' Parse hex byte
        lstrByte = Mid(lstrData, llngIndex, 2)
        
        ' Convert data type to byte and store wi
        '     thin array
        lbytTransferAry(0) = CByte("&H" & lstrByte)
        
        ' Determine where the position of the by
        '     te is in the file
        llngPosition = ((llngIndex - 1) / 2) + 1
        
        ' Save byte in proper position
        Put #llngFileNum, llngPosition, lbytTransferAry
        
    Next
    
    ' Close file
    Close llngFileNum
    
End Sub


Private Sub cmdClear_Click()
    MediaPlayer1.FileName = App.Path & "\clear.wave"
    MediaPlayer1.AutoStart = True
    lvPlays.ListItems.Clear
    If FileExists(App.Path & "\play.asx") Then
        Call DeleteFile(App.Path & "\play.asx")
    End If
    MediaPlayer1.AutoStart = False
End Sub

Private Sub cmdPlay_Click()
    
    If lvPlays.ListItems.Count = 0 Then
        MsgBox "먼저 연주될 곡을 선택하세요", vbInformation
        Exit Sub
    End If
    With MediaPlayer1
        If Not FileOpen Then
    
            Dim playAsx As String
            Dim cnt As Integer
    
            playAsx$ = "<asx version=" & Chr(34) & "3.0" & Chr(34) & " >"
            Call AppendFile(App.Path & "\play.asx", playAsx$)
            For cnt = 1 To lvPlays.ListItems.Count
                Set lvPlays.SelectedItem = lvPlays.ListItems(cnt)
                playAsx$ = "<Entry>"
                playAsx$ = playAsx$ & "<Title>" & lvPlays.SelectedItem.Text & "</Title>"
                playAsx$ = playAsx$ & "<Ref href = " & Chr(34) & lvPlays.SelectedItem.Key & Chr(34) & "/>"
                playAsx$ = playAsx$ & "</Entry>"
                Call AppendFile(App.Path & "\play.asx", playAsx$)
            Next
            playAsx$ = "</asx>"
            Call AppendFile(App.Path & "\play.asx", playAsx$)
    
            .FileName = App.Path & "\play.asx"
            .AutoStart = True
            cmdPlay.Caption = "■ 중지"
            FileOpen = True
            cmdClear.Enabled = False
            Timer6.Enabled = True
        Else
            .Stop
            cmdPlay.Caption = "▶ 연주"
            .AutoStart = False
            FileOpen = False
            cmdClear.Enabled = True
            Timer6.Enabled = False
        End If
    End With
  
End Sub

Private Sub Form_Load()
    doingPLAY = True
    If FileExists(App.Path & "\clear.wav") Then
    Else
        Call CreateClearWaveFile(App.Path & "\clear.wav")
    End If
End Sub

Private Sub Timer1_Timer()
    If Me.Top <> frmMain.Top Or Me.Left <> frmMain.Left + frmMain.Width Then
        Me.Top = frmMain.Top
        Me.Left = frmMain.Left + frmMain.Width
    End If
End Sub

Private Sub Timer6_Timer()
    min = MediaPlayer1.CurrentPosition \ 60
    sec = MediaPlayer1.CurrentPosition - (min * 60)
    If min > 0 Or sec > 0 Then
        lblElapsedTime = "진행시간 : " & Format(min, "0#") & ":" & Format(sec, "0#")
    Else
        lblElapsedTime = "진행시간 : 00:00"
    End If
End Sub
