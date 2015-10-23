VERSION 5.00
Object = "{248DD890-BB45-11CF-9ABC-0080C7E7B78D}#1.0#0"; "MSWINSCK.OCX"
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form frmHostScan 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Sharing Search"
   ClientHeight    =   3495
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   4935
   Icon            =   "frmHostScan.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3495
   ScaleWidth      =   4935
   StartUpPosition =   3  'Windows Default
   Begin MSComctlLib.ImageList imgIcons 
      Left            =   360
      Top             =   1680
      _ExtentX        =   1005
      _ExtentY        =   1005
      BackColor       =   -2147483643
      ImageWidth      =   16
      ImageHeight     =   16
      MaskColor       =   12632256
      _Version        =   393216
      BeginProperty Images {2C247F25-8591-11D1-B16A-00C0F0283628} 
         NumListImages   =   1
         BeginProperty ListImage1 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmHostScan.frx":27A2
            Key             =   ""
         EndProperty
      EndProperty
   End
   Begin VB.Frame Frame2 
      Height          =   1335
      Left            =   0
      TabIndex        =   0
      Top             =   0
      Width           =   4935
      Begin VB.Timer Timer1 
         Enabled         =   0   'False
         Interval        =   1000
         Left            =   4200
         Top             =   0
      End
      Begin VB.CommandButton cmdStartScan 
         Caption         =   "Start"
         Enabled         =   0   'False
         Height          =   375
         Left            =   2760
         TabIndex        =   9
         Top             =   880
         Width           =   1935
      End
      Begin VB.TextBox EndGroup4 
         Alignment       =   2  'Center
         Enabled         =   0   'False
         Height          =   285
         Left            =   2160
         TabIndex        =   8
         Text            =   "255"
         Top             =   600
         Width           =   375
      End
      Begin VB.TextBox EndGroup3 
         Alignment       =   2  'Center
         Enabled         =   0   'False
         Height          =   285
         Left            =   1680
         TabIndex        =   7
         Text            =   "255"
         Top             =   600
         Width           =   375
      End
      Begin VB.TextBox EndGroup2 
         Alignment       =   2  'Center
         Enabled         =   0   'False
         Height          =   285
         Left            =   1200
         TabIndex        =   6
         Text            =   "0"
         Top             =   600
         Width           =   375
      End
      Begin VB.TextBox EndGroup1 
         Alignment       =   2  'Center
         Enabled         =   0   'False
         Height          =   285
         Left            =   720
         TabIndex        =   5
         Text            =   "127"
         Top             =   600
         Width           =   375
      End
      Begin VB.TextBox StartGroup4 
         Alignment       =   2  'Center
         Enabled         =   0   'False
         Height          =   285
         Left            =   2160
         TabIndex        =   4
         Text            =   "0"
         Top             =   240
         Width           =   375
      End
      Begin VB.TextBox StartGroup3 
         Alignment       =   2  'Center
         Enabled         =   0   'False
         Height          =   285
         Left            =   1680
         TabIndex        =   3
         Text            =   "0"
         Top             =   240
         Width           =   375
      End
      Begin VB.TextBox StartGroup2 
         Alignment       =   2  'Center
         Enabled         =   0   'False
         Height          =   285
         Left            =   1200
         TabIndex        =   2
         Text            =   "0"
         Top             =   240
         Width           =   375
      End
      Begin VB.TextBox StartGroup1 
         Alignment       =   2  'Center
         Enabled         =   0   'False
         Height          =   285
         Left            =   720
         TabIndex        =   1
         Text            =   "127"
         Top             =   240
         Width           =   375
      End
      Begin MSComctlLib.Slider Slider1 
         Height          =   255
         Left            =   3360
         TabIndex        =   10
         Top             =   600
         Width           =   1425
         _ExtentX        =   2514
         _ExtentY        =   450
         _Version        =   393216
         Min             =   -1000
         Max             =   -1
         SelStart        =   -1
         TickStyle       =   3
         Value           =   -1
      End
      Begin MSWinsockLib.Winsock Winsock1 
         Index           =   0
         Left            =   3720
         Top             =   0
         _ExtentX        =   741
         _ExtentY        =   741
         _Version        =   393216
      End
      Begin VB.Label Label5 
         AutoSize        =   -1  'True
         BackStyle       =   0  'Transparent
         Caption         =   "Speed:"
         Height          =   180
         Left            =   2760
         TabIndex        =   16
         Top             =   600
         Width           =   600
      End
      Begin VB.Label lblFound 
         BackStyle       =   0  'Transparent
         Caption         =   "0"
         Height          =   255
         Left            =   3720
         TabIndex        =   15
         Top             =   360
         Width           =   975
      End
      Begin VB.Label Label3 
         AutoSize        =   -1  'True
         BackStyle       =   0  'Transparent
         Caption         =   "Result:"
         Height          =   180
         Left            =   2760
         TabIndex        =   14
         Top             =   360
         Width           =   585
      End
      Begin VB.Label Label2 
         AutoSize        =   -1  'True
         BackStyle       =   0  'Transparent
         Caption         =   "To :"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H00404040&
         Height          =   195
         Left            =   240
         TabIndex        =   13
         Top             =   600
         Width           =   345
      End
      Begin VB.Label Label1 
         AutoSize        =   -1  'True
         BackStyle       =   0  'Transparent
         Caption         =   "From :"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H00404040&
         Height          =   195
         Left            =   120
         TabIndex        =   12
         Top             =   240
         Width           =   525
      End
      Begin VB.Label Label4 
         Alignment       =   2  'Center
         BackColor       =   &H00FFFFFF&
         BorderStyle     =   1  'Fixed Single
         Caption         =   "Progress Status"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H00004000&
         Height          =   270
         Left            =   720
         TabIndex        =   11
         Top             =   960
         Width           =   1815
      End
   End
   Begin MSComctlLib.ListView lvUsers 
      Height          =   2175
      Left            =   0
      TabIndex        =   17
      Top             =   1320
      Width           =   4935
      _ExtentX        =   8705
      _ExtentY        =   3836
      View            =   3
      LabelEdit       =   1
      LabelWrap       =   -1  'True
      HideSelection   =   -1  'True
      OLEDragMode     =   1
      FullRowSelect   =   -1  'True
      GridLines       =   -1  'True
      _Version        =   393217
      SmallIcons      =   "imgIcons"
      ForeColor       =   -2147483640
      BackColor       =   -2147483643
      BorderStyle     =   1
      Appearance      =   1
      OLEDragMode     =   1
      NumItems        =   2
      BeginProperty ColumnHeader(1) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         Text            =   "IP : "
         Object.Width           =   6068
      EndProperty
      BeginProperty ColumnHeader(2) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         SubItemIndex    =   1
         Text            =   "컴퓨터이름 : "
         Object.Width           =   3352
      EndProperty
   End
End
Attribute VB_Name = "frmHostScan"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Dim IniPath As String
Dim TimesAround As Integer
Dim CurrIP As String

Private Sub cmdStartScan_Click()
    'This is par
    Select Case cmdStartScan.Caption
        Case "Start"
            Timer1.Enabled = True
            cmdStartScan.Caption = "Stop"
        Case "Stop"
            Timer1.Enabled = False
            cmdStartScan.Caption = "Start"
    End Select
End Sub

Private Sub Form_Activate()

    Dim StartGroup As String
    Dim EndGroup As String
    Dim vTemp As Variant
    
    IniPath = App.Path & "\" & "P2P.ini"
    StartGroup = ReadValue(IniPath, "Host", "StartIP")
    EndGroup = ReadValue(IniPath, "Host", "EndIP")
    
    If StartGroup = "" Or EndGroup = "" Then
        Call SaveValue(IniPath, "Host", "StartIP", "127.0.0.1")
        Call SaveValue(IniPath, "Host", "EndIP", "127.0.0.1")
        StartGroup = ReadValue(IniPath, "Host", "StartIP")
        EndGroup = ReadValue(IniPath, "Host", "EndIP")
    End If
    
    vTemp = Split(StartGroup, ".")
    
    StartGroup1.Text = vTemp(0)
    StartGroup2.Text = vTemp(1)
    StartGroup3.Text = vTemp(2)
    StartGroup4.Text = vTemp(3)
    
    vTemp = Split(EndGroup, ".")
    
    EndGroup1.Text = vTemp(0)
    EndGroup2.Text = vTemp(1)
    EndGroup3.Text = vTemp(2)
    EndGroup4.Text = vTemp(3)
    
    lvUsers.ListItems.Clear
    lblFound.Caption = 0
    
    Timer1.Enabled = True
    
End Sub


Private Sub Form_Load()
    Me.Top = frmMain.Top + ((frmMain.Height - Me.Height) / 2)
    Me.Left = frmMain.Left + ((frmMain.Width - Me.Width) / 2)
End Sub

Private Sub Timer1_Timer()
    On Error Resume Next

    Timer1.Interval = -Slider1.Value
    TimesAround = TimesAround + 1

    Load Winsock1(TimesAround) 'load new winsock
    If TimesAround > 50 Then Unload Winsock1(TimesAround - 50) 'unload winsock control (time out)

    If Val(StartGroup4) < Val(EndGroup4) Then
        StartGroup4.Text = StartGroup4.Text + 1 'increase current ip address by one
    ElseIf Val(StartGroup4) = Val(EndGroup4) Then
        StartGroup3 = StartGroup3 + 1 'increase 3rd group in ip address by one
        StartGroup4 = 0 'reset last group in ip address
    End If

    If Val(StartGroup3) > Val(EndGroup3) Then 'check if scan is complete
        'MsgBox "Scan Complete"
        Timer1.Enabled = False
        cmdStartScan.Caption = "Start"
        Me.Hide
    End If
    CurrIP = StartGroup1 & "." & StartGroup2 & "." & StartGroup3 & "." & StartGroup4
    Winsock1(TimesAround).Connect CurrIP, 821 'connect to user
    Label4.Caption = StartGroup1 & "." & StartGroup2 & "." & StartGroup3 & "." & StartGroup4 'display current ip address
End Sub

Private Sub Winsock1_Connect(Index As Integer)
    Winsock1(Index).SendData "GET " & "/user" & " HTTP/1.1" & vbCrLf & _
                             "Host: " & Winsock1(Index).LocalIP & vbCrLf & _
                             "Connection: close" & vbCrLf & _
                             "Accept: */*" & vbCrLf & _
                             vbCrLf
End Sub

Private Sub Winsock1_DataArrival(Index As Integer, ByVal bytesTotal As Long)
    Dim Data As String
    Dim vTemp As Variant
    Dim listx As ListItem
    
    Winsock1(Index).GetData Data, vbString

    Data = Left$(Data, InStr(1, Data, vbCrLf & vbCrLf) - 1)
        
    If Mid$(Data, 10, 6) = "200 OK" Then
        lblFound.Caption = lblFound.Caption + 1
        Data = Replace(Data, vbCrLf & vbCrLf, "", 1, Len(Data), vbTextCompare)
        Data = Replace(Data, vbCrLf, "|", 1, Len(Data), vbTextCompare)
        Data = Replace(Data, "Host:", "", 1, Len(Data), vbTextCompare)
        Data = Replace(Data, "HostName:", "", 1, Len(Data), vbTextCompare)
        vTemp = Split(Data, "|")
        'Call SaveValue(IniPath, "USER", lblFound.Caption, CStr(vTemp(1)))
        Set listx = lvUsers.ListItems.Add(, , , , 1)
        listx.Text = CStr(vTemp(1))
        listx.SubItems(1) = CStr(vTemp(2))
    End If
    Winsock1(Index).Close

End Sub
