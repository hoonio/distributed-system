VERSION 5.00
Object = "{248DD890-BB45-11CF-9ABC-0080C7E7B78D}#1.0#0"; "MSWINSCK.OCX"
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form frmMain 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "현대종합상사 MP3 파일공유"
   ClientHeight    =   4815
   ClientLeft      =   150
   ClientTop       =   720
   ClientWidth     =   7305
   Icon            =   "frmMain.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   Picture         =   "frmMain.frx":038A
   ScaleHeight     =   4815
   ScaleWidth      =   7305
   ShowInTaskbar   =   0   'False
   StartUpPosition =   3  'Windows Default
   Begin MSComctlLib.ImageList imgFiles 
      Left            =   120
      Top             =   2880
      _ExtentX        =   1005
      _ExtentY        =   1005
      BackColor       =   -2147483643
      ImageWidth      =   16
      ImageHeight     =   16
      MaskColor       =   12632256
      _Version        =   393216
      BeginProperty Images {2C247F25-8591-11D1-B16A-00C0F0283628} 
         NumListImages   =   8
         BeginProperty ListImage1 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":06CC
            Key             =   ""
         EndProperty
         BeginProperty ListImage2 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":0C66
            Key             =   ""
         EndProperty
         BeginProperty ListImage3 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":10B8
            Key             =   ""
         EndProperty
         BeginProperty ListImage4 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":150A
            Key             =   ""
         EndProperty
         BeginProperty ListImage5 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":388C
            Key             =   ""
         EndProperty
         BeginProperty ListImage6 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":4B0E
            Key             =   ""
         EndProperty
         BeginProperty ListImage7 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":72C0
            Key             =   ""
         EndProperty
         BeginProperty ListImage8 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":9642
            Key             =   ""
         EndProperty
      EndProperty
   End
   Begin VB.TextBox Text2 
      Height          =   3015
      Left            =   480
      MultiLine       =   -1  'True
      ScrollBars      =   2  'Vertical
      TabIndex        =   6
      Text            =   "frmMain.frx":9A94
      Top             =   240
      Visible         =   0   'False
      Width           =   6375
   End
   Begin VB.Timer tmrTimeout 
      Enabled         =   0   'False
      Interval        =   60
      Left            =   120
      Top             =   840
   End
   Begin MSWinsockLib.Winsock wscHttp 
      Left            =   120
      Top             =   360
      _ExtentX        =   741
      _ExtentY        =   741
      _Version        =   393216
   End
   Begin VB.CommandButton cmdList 
      Caption         =   "리스트"
      Height          =   495
      Left            =   6120
      TabIndex        =   4
      Top             =   3600
      Width           =   1095
   End
   Begin VB.Frame Frame1 
      Height          =   615
      Left            =   120
      TabIndex        =   0
      Top             =   4080
      Width           =   7095
      Begin VB.Label Lbl_Status2 
         Height          =   255
         Left            =   240
         TabIndex        =   1
         Top             =   240
         Width           =   5415
      End
   End
   Begin MSWinsockLib.Winsock HttpSvr 
      Left            =   6840
      Top             =   0
      _ExtentX        =   741
      _ExtentY        =   741
      _Version        =   393216
      LocalPort       =   821
   End
   Begin MSWinsockLib.Winsock Hoe 
      Index           =   0
      Left            =   6840
      Top             =   480
      _ExtentX        =   741
      _ExtentY        =   741
      _Version        =   393216
      LocalPort       =   80
   End
   Begin MSComctlLib.ListView lvFiles 
      Height          =   3495
      Left            =   0
      TabIndex        =   7
      Top             =   0
      Width           =   7335
      _ExtentX        =   12938
      _ExtentY        =   6165
      View            =   3
      LabelEdit       =   1
      LabelWrap       =   -1  'True
      HideSelection   =   0   'False
      GridLines       =   -1  'True
      _Version        =   393217
      SmallIcons      =   "imgFiles"
      ForeColor       =   -2147483640
      BackColor       =   16777215
      BorderStyle     =   1
      Appearance      =   1
      NumItems        =   3
      BeginProperty ColumnHeader(1) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         Text            =   "파일이름"
         Object.Width           =   10398
      EndProperty
      BeginProperty ColumnHeader(2) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         SubItemIndex    =   1
         Text            =   "크기 (bytes):"
         Object.Width           =   2540
      EndProperty
      BeginProperty ColumnHeader(3) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         SubItemIndex    =   2
         Text            =   "컴퓨터이름"
         Object.Width           =   1411
      EndProperty
   End
   Begin VB.Label lblMsg 
      Alignment       =   2  'Center
      Height          =   255
      Left            =   120
      TabIndex        =   5
      Top             =   3555
      Width           =   5895
   End
   Begin VB.Label current 
      Caption         =   "0"
      Height          =   255
      Left            =   5400
      TabIndex        =   3
      Top             =   3840
      Width           =   615
   End
   Begin VB.Label curr 
      Caption         =   "Connections:"
      Height          =   255
      Left            =   4320
      TabIndex        =   2
      Top             =   3840
      Width           =   975
   End
   Begin VB.Menu mnuOption 
      Caption         =   "설정"
      Begin VB.Menu mnuFolder 
         Caption         =   "공유폴더"
      End
      Begin VB.Menu mnuSetIP 
         Caption         =   "IP검색범위"
      End
   End
   Begin VB.Menu mnuMp3P 
      Caption         =   "MP3플레이어"
   End
   Begin VB.Menu mnuSysTray 
      Caption         =   "SysTray"
   End
   Begin VB.Menu mnuEnd 
      Caption         =   "종료"
   End
End
Attribute VB_Name = "frmMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim SharedPath As String
Dim IniPath As String
Dim FileName As String

Private m_strRemoteHost As String    'Who do we connect to
Private m_strFilePath As String        'Remote file path. In this case always /
Private m_strHttpResponse As String    'Response from server. Used for parsing
Private m_bResponseReceived As Boolean 'Recieved response?
Private StillExecuting As Boolean      'Set to True when socket is active

Private ConnectCnt As Integer

Private Sub cmdList_Click()
On Error Resume Next
    Dim i As Integer
        
    frmHostScan.Show vbModal
    
    lvFiles.ListItems.Clear

    For i = 1 To frmHostScan.lvUsers.ListItems.Count
        
        m_strRemoteHost = Trim(CStr(frmHostScan.lvUsers.ListItems(i).Text))
    
        m_strFilePath = "/"
    
        m_strHttpResponse = ""
        m_bResponseReceived = False
    
        lblMsg.Caption = "Connecting to " & m_strRemoteHost & "..."

        With wscHttp
            .Close
            .LocalPort = 0
            .Connect m_strRemoteHost, 821
            tmrTimeout.Enabled = True
            StillExecuting = True
        End With

        While StillExecuting
            DoEvents
        Wend
    Next i
   
End Sub


Private Sub Form_Paint()
    frmPlay.Top = Me.Top
    frmPlay.Left = Me.Left + Me.Width
    frmPlay.Show , Me
End Sub

Private Sub lvFiles_Click()
    Dim listx As ListItem
    Dim i As Integer
    
    On Error Resume Next
    
    For i = 1 To frmPlay.lvPlays.ListItems.Count
        If frmPlay.lvPlays.ListItems(i).Text = lvFiles.SelectedItem.Text Then
            MsgBox "이미 선택된 곡 입니다.", vbInformation, "중복파일"
            Exit Sub
        End If
    Next
    
    Set listx = frmPlay.lvPlays.ListItems.Add()
    listx.Text = lvFiles.SelectedItem.Text
    listx.Key = lvFiles.SelectedItem.Key
    
End Sub

Private Sub mnuEnd_Click()
    End
End Sub

Private Sub mnuSetIP_Click()
    frmSetIP.Show
End Sub

Private Sub tmrTimeout_Timer()
    tmrTimeout.Enabled = False
    StillExecuting = False
End Sub

Private Sub Form_Load()

    Dim i As Integer
    
    If App.PrevInstance = True Then
        MsgBox "HDC P2P 는 이미 실행되어 있습니다.", vbExclamation, "안내"
        End
    End If
    
    Me.Left = (Screen.Width - (Me.Width + frmPlay.Width)) / 2
    Me.Top = (Screen.Height - Me.Height) / 2
    
    IniPath = App.Path & "\" & "P2P.ini"
    
    If FileExists(IniPath) Then
    Else
        Dim ret
        Me.Show
        ret = BrowseForFolder(Me.hwnd, "공유할 디렉토리를 선택하세요.")
        If ret = "" Then Exit Sub
        SharedPath = ret
        Call SaveValue(IniPath, "Folder", "SharedFolder", SharedPath)
    End If
    SharedPath = ReadValue(IniPath, "Folder", "SharedFolder")
    If FileExists(SharedPath & "\index.html") Then
        Call DeleteFile(SharedPath & "\index.html")
    End If
    
    frmMP3.Show
    frmMP3.Hide
    
    For i = 1 To 200
        Load Hoe(i)
    Next i

    'The following are necessary to reduce the chance of errors
        
    If HttpSvr.State <> sckClosed Then          '# If the Winsock is not closed, it closes the winsock.
        HttpSvr.Close
    End If
    HttpSvr.Protocol = sckTCPProtocol
    HttpSvr.LocalPort = 821
    HttpSvr.Listen
    If HttpSvr.State = 0 Then
        Lbl_Status2.Caption = "Connection closed on port: " & HttpSvr.LocalPort
    ElseIf HttpSvr.State = 1 Then
        Lbl_Status2.Caption = "Connection in use on port: " & HttpSvr.LocalPort
    ElseIf HttpSvr.State = 2 Then
        Lbl_Status2.Caption = "Winsock listening on port: " & HttpSvr.LocalPort
    ElseIf HttpSvr.State = 3 Then
        Lbl_Status2.Caption = "Connection pending on port: " & HttpSvr.LocalPort
    ElseIf HttpSvr.State = 4 Then
        Lbl_Status2.Caption = "Connection pending on port: " & HttpSvr.LocalPort
    ElseIf HttpSvr.State = 5 Then
        Lbl_Status2.Caption = "Host resolved on port: " & HttpSvr.LocalPort
    ElseIf HttpSvr.State = 6 Then
        Lbl_Status2.Caption = "Connecting on port: " & HttpSvr.LocalPort
    ElseIf HttpSvr.State = 7 Then
        Lbl_Status2.Caption = "Connected on port: " & HttpSvr.LocalPort
    ElseIf HttpSvr.State = 8 Then
        Lbl_Status2.Caption = "Peer closed connection on port: " & HttpSvr.LocalPort
    ElseIf HttpSvr.State = 9 Then
        Lbl_Status2.Caption = "Error on port: " & HttpSvr.LocalPort
    End If
    
    Call AddIcon(Me, "Hyundai Corp. P2P Program 보기")
    
End Sub

Private Sub Form_MouseMove(Button As Integer, Shift As Integer, X As Single, Y As Single)
    On Error Resume Next
    Dim Result As Long, msg As Long
    If Me.ScaleMode = vbPixels Then
        msg = X
    Else
        msg = X / Screen.TwipsPerPixelX
    End If
    
    If msg = 514 Then
        Result = SetForegroundWindow(Me.hwnd)
        Me.Show
    End If
End Sub

Private Sub Form_Unload(Cancel As Integer)
    HttpSvr.Close
    Call DeleteIcon
    DoEvents
    End
End Sub

Private Sub HttpSvr_ConnectionRequest(ByVal requestID As Long)
    Dim i As Integer
    For i = 0 To 200
        If Hoe(i).State = sckClosed Then
            Hoe(i).Close
            Hoe(i).Accept (requestID)
            current.Caption = current.Caption + 1
            Exit Sub
        End If
    Next i
End Sub

Private Sub Hoe_DataArrival(Index As Integer, ByVal bytesTotal As Long)
    Dim strData As String
    Dim strGet As String
    Dim spc2 As Long
    Dim page As String
    Dim Data As String
    
    Hoe(Index).GetData strData
    If Mid(strData, 1, 3) = "GET" Then
        strGet = InStr(strData, "GET ")
        spc2 = InStr(strGet + 5, strData, " ")
        page = Trim(Mid(strData, strGet + 5, spc2 - (strGet + 4)))
        If Right(page, 1) = "/" Then page = Left(page, Len(page) - 1)
        
        IniPath = App.Path & "\" & "P2P.ini"
        
        If FileExists(IniPath) Then
            SharedPath = ReadValue(IniPath, "Folder", "SharedFolder")
        End If
        
        If page = "user" Then
        
            Hoe(Index).SendData "HTTP/1.0 200 OK" & vbCrLf & _
                            "Host: " & Hoe(Index).LocalIP & vbCrLf & _
                            "HostName: " & Hoe(Index).LocalHostName & vbCrLf & _
                            vbCrLf
        ElseIf page = "" Then
            page = "index.html"
            
            Data = SharedPath & "\" & page
            Hoe(Index).SendData "HTTP/1.0 200 OK" & vbCrLf & _
                            "Host: " & Hoe(Index).LocalIP & vbCrLf & _
                            "HostName: " & Hoe(Index).LocalHostName & vbCrLf & _
                            "Content-Length: " & FileLen(Data) & vbCrLf & _
                            "Content-Type: " & "*/*" & vbCrLf & _
                            vbCrLf & _
                            LoadFile(Data)
                            
        Else
        
            page = Replace(page, "%20", " ", 1, Len(page), vbTextCompare)
            Data = SharedPath & "\" & page
            
            Hoe(Index).SendData "HTTP/1.0 200 OK" & vbCrLf & _
                            "Content-Length: " & FileLen(Data) & vbCrLf & _
                            vbCrLf
                            
            Dim i As Integer
    
            Dim BlockSize As Long
            Dim LeftSize As Long
            Dim FileSize  As Long
            Dim FileCount As Integer
            Dim FileEnd As Boolean

            Dim BinData() As Byte
        
            Open Data For Binary Access Read As #1
            FileSize = LOF(1)
            BlockSize = 2048
            If BlockSize < FileSize Then
                BlockSize = FileSize
            Else
                LeftSize = FileSize Mod BlockSize
            End If
            ReDim BinData(1 To BlockSize)
            FileEnd = False
            Do
                FileCount = FileCount + 1
                If FileCount = CInt(FileSize / BlockSize) Then
                    If LeftSize > 0 Then
                        BlockSize = LeftSize
                        ReDim BinData(1 To BlockSize)
                    End If
                    FileEnd = True
                End If
                Get #1, , BinData
        
                Hoe(Index).SendData BinData
            Loop Until FileEnd
            Close #1
        End If
    End If

End Sub

Private Sub Hoe_SendComplete(Index As Integer)
    current.Caption = current.Caption - 1
    Hoe(Index).Close
End Sub

Private Sub lvFiles_DblClick()
    Dim listx As ListItem

    On Error Resume Next

    If doingDL Then

        Set listx = frmDownload.lvDL.ListItems.Add(, , , , 4)
        listx.Text = lvFiles.SelectedItem.Text
        listx.SubItems(1) = lvFiles.SelectedItem.ListSubItems(1).Text
        listx.SubItems(2) = "Queued"
        listx.Key = lvFiles.SelectedItem.Key
    
        frmDownload.Show , Me
    Else
        Set listx = frmDownload.lvDL.ListItems.Add(, , , , 1)
        listx.Text = lvFiles.SelectedItem.Text
        listx.SubItems(1) = lvFiles.SelectedItem.ListSubItems(1).Text
        listx.SubItems(2) = "Downloading..."
        listx.Key = lvFiles.SelectedItem.Key
        
        currIndex = listx.Index
        
        frmDownload.lblRFile = listx.Key
        frmDownload.lblLFile = listx.Text
        frmDownload.lblProgress = ""
        
        frmDownload.pb1.Max = lvFiles.SelectedItem.ListSubItems(1).Text
        
        frmDownload.Show , Me
        
        With frmDownload.dl
            .Url = lvFiles.SelectedItem.Key
            .GetFileInformation
            .SaveLocation = SharedPath & "\" & lvFiles.SelectedItem.Text
            .DownLoad
        End With
    End If
End Sub

Private Sub mnuComScan_Click()
    frmHostScan.Show
End Sub


Private Sub wscHttp_Close()

    On Error Resume Next

    Dim strHttpResponseHeader As String
    Dim listx As ListItem
    Dim vFiles As Variant
    Dim vTemp As Variant
    Dim i As Long

    lblMsg.Caption = "Done."

    If Not m_bResponseReceived Then
        strHttpResponseHeader = Left$(m_strHttpResponse, InStr(1, m_strHttpResponse, vbCrLf & vbCrLf) - 1)
        
        If Mid$(strHttpResponseHeader, 10, 4) = "404 " Then
            m_bResponseReceived = True
            tmrTimeout.Enabled = False
            StillExecuting = False
            Exit Sub
        End If
        
        m_strHttpResponse = Mid(m_strHttpResponse, InStr(1, m_strHttpResponse, vbCrLf & vbCrLf) + 4)
        
        Text2.Text = m_strHttpResponse
        Text2.Text = Text2.Text
        Text2.Text = Replace(Text2.Text, vbCrLf, "", 1, Len(Text2.Text), vbTextCompare)
        Text2.Text = Replace(Text2.Text, Chr(10), vbCrLf, 1, Len(Text2.Text), vbTextCompare)
        Text2.Text = Replace(Text2.Text, "<tr><td><a href=", vbCrLf, 1, Len(Text2.Text), vbTextCompare)
        Text2.Text = Replace(Text2.Text, Chr(34) & ">", "|", 1, Len(Text2.Text), vbTextCompare)
        Text2.Text = Replace(Text2.Text, Chr(34), "", 1, Len(Text2.Text), vbTextCompare)
        Text2.Text = Replace(Text2.Text, "</a></td><td>", "|", 1, Len(Text2.Text), vbTextCompare)
        Text2.Text = Replace(Text2.Text, "</td><td>", "|", 1, Len(Text2.Text), vbTextCompare)
        Text2.Text = Replace(Text2.Text, "</td></tr>", "|", 1, Len(Text2.Text), vbTextCompare)
        Text2.Text = Replace(Text2.Text, "<html><body><table>" & vbCrLf, "", 1, Len(Text2.Text), vbTextCompare)
        Text2.Text = Replace(Text2.Text, "</table></body></html>" & vbCrLf, "", 1, Len(Text2.Text), vbTextCompare)
        Text2.Text = Replace(Text2.Text, vbCrLf & vbCrLf, vbCrLf, 1, Len(Text2.Text), vbTextCompare)
        If Asc(Left(Text2.Text, 1)) = 13 Then Text2.Text = Right(Text2.Text, Len(Text2.Text) - 1)
        If Asc(Left(Text2.Text, 1)) = 10 Then Text2.Text = Right(Text2.Text, Len(Text2.Text) - 1)
        If Asc(Right(Text2.Text, 1)) = 13 Then Text2.Text = Left(Text2.Text, Len(Text2.Text) - 1)
        If Asc(Right(Text2.Text, 1)) = 10 Then Text2.Text = Left(Text2.Text, Len(Text2.Text) - 1)
        If Asc(Right(Text2.Text, 1)) = 13 Then Text2.Text = Left(Text2.Text, Len(Text2.Text) - 1)

        vFiles = Split(Text2.Text, vbCrLf)
                
        'Display results accordingly
            
        For i = 0 To UBound(vFiles)
            vTemp = Split(vFiles(i), "|")
              
            If Right(LCase(vTemp(1)), 3) = "mpg" Or Right(LCase(vTemp(1)), 3) = "avi" Or Right(LCase(vTemp(1)), 4) = "mpeg" Or Right(LCase(vTemp(1)), 3) = "mov" Or Right(LCase(vTemp(1)), 3) = "wma" Then
                               
                If Not Len(vTemp(1)) = 0 Then
                    Set listx = lvFiles.ListItems.Add(, , , , 5)
                    listx.Text = vTemp(1)
                    listx.SubItems(1) = vTemp(2)
                    listx.SubItems(2) = vTemp(3)
                    listx.Key = "http://" & wscHttp.RemoteHostIP & ":821/" & vTemp(0)
                End If
    
                lblMsg.Caption = "Browsing: " & lvFiles.ListItems.Count & " files"
           
            ElseIf Right(LCase(vTemp(1)), 3) = "wav" Or Right(LCase(vTemp(1)), 3) = "snd" Or Right(LCase(vTemp(1)), 3) = "mp3" Then
    
                If Not Len(vTemp(1)) = 0 Then
                                
                    Set listx = lvFiles.ListItems.Add(, , , , 4)
                    listx.Text = vTemp(1)
                    listx.SubItems(1) = vTemp(2)
                    listx.SubItems(2) = vTemp(3)
                    listx.Key = "http://" & wscHttp.RemoteHostIP & ":821/" & vTemp(0)

                End If
    
                lblMsg.Caption = "Browsing: " & lvFiles.ListItems.Count & " files"
    
            ElseIf Right(LCase(vTemp(1)), 3) = "jpg" Or Right(LCase(vTemp(1)), 3) = "bmp" Or Right(LCase(vTemp(1)), 3) = "gif" Then
    
                If Not Len(vTemp(1)) = 0 Then
                            
                    Set listx = lvFiles.ListItems.Add(, , , , 6)
                    listx.Text = vTemp(1)
                    listx.SubItems(1) = vTemp(2)
                    listx.SubItems(2) = vTemp(3)
                    listx.Key = "http://" & wscHttp.RemoteHostIP & ":821/" & vTemp(0)
        
                End If
    
                lblMsg.Caption = "Browsing: " & lvFiles.ListItems.Count & " files"
    
            ElseIf Right(LCase(vTemp(1)), 3) = "vbs" Then
    
                If Not Len(vTemp(1)) = 0 Then
                               
                    Set listx = lvFiles.ListItems.Add(, , , , 8)
                    listx.Text = vTemp(1)
                    listx.SubItems(1) = vTemp(2)
                    listx.SubItems(2) = vTemp(3)
                    listx.Key = "http://" & wscHttp.RemoteHostIP & ":821/" & vTemp(0)
        
                End If
    
                lblMsg.Caption = "Browsing: " & lvFiles.ListItems.Count & " files"
    
            Else
    
                If Not Len(vTemp(1)) = 0 Then
                                
                    Set listx = lvFiles.ListItems.Add(, , , , 7)
                    listx.Text = vTemp(1)
                    listx.SubItems(1) = vTemp(2)
                    listx.SubItems(2) = vTemp(3)
                    listx.Key = "http://" & wscHttp.RemoteHostIP & ":821/" & vTemp(0)
        
                    'If Len(Search) > 0 Then
                    '    If InStr(1, LCase(vTemp(1)), LCase(Search)) > 0 Then
                    '        listx.Bold = True
                    '        listx.ForeColor = &HFF&
                    '    End If
                    'End If
                                    
                End If
    
                lblMsg.Caption = "Browsing: " & lvFiles.ListItems.Count & " files"
    
            End If
                     
        Next i
                                                  
        If lvFiles.ListItems.Count = 0 Then
            Set listx = lvFiles.ListItems.Add
            listx.Text = "출력할 파일이 없습니다..."
            lblMsg.Caption = ""
        End If
     
        m_bResponseReceived = True
        
    End If
    
    tmrTimeout.Enabled = False
    StillExecuting = False
    
End Sub

Private Sub wscHttp_Connect()

    Dim strHttpRequest As String
    
    tmrTimeout.Enabled = False
    lblMsg.Caption = "Connected!"

    'MsgBox m_strFilePath

    strHttpRequest = "GET " & m_strFilePath & " HTTP/1.1" & vbCrLf
    strHttpRequest = strHttpRequest & "Host: " & m_strRemoteHost & vbCrLf
    strHttpRequest = strHttpRequest & "Connection: close" & vbCrLf
    strHttpRequest = strHttpRequest & "Accept: */*" & vbCrLf
    strHttpRequest = strHttpRequest & vbCrLf
    
    wscHttp.SendData strHttpRequest
    
End Sub

Private Sub wscHttp_DataArrival(ByVal bytesTotal As Long)
    On Error Resume Next
    Dim strData As String

    lblMsg.Caption = "Reading data..."
    
    wscHttp.GetData strData
    m_strHttpResponse = m_strHttpResponse & strData
End Sub

Private Sub wscHttp_Error(ByVal Number As Integer, Description As String, ByVal Scode As Long, ByVal Source As String, ByVal HelpFile As String, ByVal HelpContext As Long, CancelDisplay As Boolean)
tmrTimeout.Enabled = False
StillExecuting = False
End Sub


Private Sub mnuFolder_Click()
    mnuSysTray.Enabled = False
    frmMP3.Show
End Sub

Private Sub mnuMp3P_Click()
    mnuSysTray.Enabled = False
    frmMP3.Show
End Sub

Private Sub mnuSysTray_Click()
    frmPlay.Hide
    Me.Hide
End Sub
