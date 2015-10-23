VERSION 5.00
Begin VB.Form frmSetIP 
   BorderStyle     =   1  '´ÜÀÏ °íÁ¤
   Caption         =   "IP Range Setting"
   ClientHeight    =   975
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   3885
   Icon            =   "frmSetIP.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   975
   ScaleWidth      =   3885
   StartUpPosition =   3  'Windows ±âº»°ª
   Begin VB.Frame Frame2 
      Height          =   975
      Left            =   0
      TabIndex        =   0
      Top             =   0
      Width           =   3885
      Begin VB.CommandButton cmdStartScan 
         Caption         =   "Save&&Quit"
         Height          =   615
         Left            =   2640
         TabIndex        =   9
         Top             =   240
         Width           =   1095
      End
      Begin VB.TextBox EndGroup4 
         Alignment       =   2  '°¡¿îµ¥ ¸ÂÃã
         Height          =   285
         Left            =   2160
         TabIndex        =   8
         Text            =   "255"
         Top             =   600
         Width           =   375
      End
      Begin VB.TextBox EndGroup3 
         Alignment       =   2  '°¡¿îµ¥ ¸ÂÃã
         Height          =   285
         Left            =   1680
         TabIndex        =   7
         Text            =   "255"
         Top             =   600
         Width           =   375
      End
      Begin VB.TextBox EndGroup2 
         Alignment       =   2  '°¡¿îµ¥ ¸ÂÃã
         Height          =   285
         Left            =   1200
         TabIndex        =   6
         Text            =   "0"
         Top             =   600
         Width           =   375
      End
      Begin VB.TextBox EndGroup1 
         Alignment       =   2  '°¡¿îµ¥ ¸ÂÃã
         Height          =   285
         Left            =   720
         TabIndex        =   5
         Text            =   "127"
         Top             =   600
         Width           =   375
      End
      Begin VB.TextBox StartGroup4 
         Alignment       =   2  '°¡¿îµ¥ ¸ÂÃã
         Height          =   285
         Left            =   2160
         TabIndex        =   4
         Text            =   "0"
         Top             =   240
         Width           =   375
      End
      Begin VB.TextBox StartGroup3 
         Alignment       =   2  '°¡¿îµ¥ ¸ÂÃã
         Height          =   285
         Left            =   1680
         TabIndex        =   3
         Text            =   "0"
         Top             =   240
         Width           =   375
      End
      Begin VB.TextBox StartGroup2 
         Alignment       =   2  '°¡¿îµ¥ ¸ÂÃã
         Height          =   285
         Left            =   1200
         TabIndex        =   2
         Text            =   "0"
         Top             =   240
         Width           =   375
      End
      Begin VB.TextBox StartGroup1 
         Alignment       =   2  '°¡¿îµ¥ ¸ÂÃã
         Height          =   285
         Left            =   720
         TabIndex        =   1
         Text            =   "127"
         Top             =   240
         Width           =   375
      End
      Begin VB.Label Label2 
         AutoSize        =   -1  'True
         BackStyle       =   0  'Åõ¸í
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
         TabIndex        =   11
         Top             =   600
         Width           =   345
      End
      Begin VB.Label Label1 
         AutoSize        =   -1  'True
         BackStyle       =   0  'Åõ¸í
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
         TabIndex        =   10
         Top             =   240
         Width           =   525
      End
   End
End
Attribute VB_Name = "frmSetIP"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Dim IniPath As String
Dim TimesAround As Integer
Dim CurrIP As String

Private Sub cmdStartScan_Click()

    Call SaveValue(IniPath, "Host", "StartIP", StartGroup1.Text & "." & StartGroup2.Text & "." & StartGroup3.Text & "." & StartGroup4.Text)
    Call SaveValue(IniPath, "Host", "EndIP", EndGroup1.Text & "." & EndGroup2.Text & "." & EndGroup3.Text & "." & EndGroup4.Text)
    Me.Hide
        
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
    
End Sub


Private Sub Form_Load()
    Me.Top = frmMain.Top + ((frmMain.Height - Me.Height) / 2)
    Me.Left = frmMain.Left + ((frmMain.Width - Me.Width) / 2)
End Sub

Private Sub StartGroup1_KeyPress(KeyAscii As Integer)
    If KeyAscii = 46 Then
        StartGroup2.SetFocus
    End If
End Sub
Private Sub StartGroup2_KeyPress(KeyAscii As Integer)
    If KeyAscii = 46 Then
        StartGroup3.SetFocus
    End If
End Sub
Private Sub StartGroup3_KeyPress(KeyAscii As Integer)
    If KeyAscii = 46 Then
        StartGroup4.SetFocus
    End If
End Sub
Private Sub EndGroup1_KeyPress(KeyAscii As Integer)
    If KeyAscii = 46 Then
        EndGroup2.SetFocus
    End If
End Sub
Private Sub EndGroup2_KeyPress(KeyAscii As Integer)
    If KeyAscii = 46 Then
        EndGroup3.SetFocus
    End If
End Sub
Private Sub EndGroup3_KeyPress(KeyAscii As Integer)
    If KeyAscii = 46 Then
        EndGroup4.SetFocus
    End If
End Sub
Private Sub StartGroup1_GotFocus()
    StartGroup1.SelLength = Len(StartGroup1.Text)
End Sub
Private Sub StartGroup2_GotFocus()
    StartGroup1.Text = Replace(StartGroup1.Text, ".", "")
    StartGroup2.SelLength = Len(StartGroup2.Text)
End Sub
Private Sub StartGroup3_GotFocus()
    StartGroup2.Text = Replace(StartGroup2.Text, ".", "")
    StartGroup3.SelLength = Len(StartGroup3.Text)
End Sub
Private Sub StartGroup4_GotFocus()
    StartGroup3.Text = Replace(StartGroup3.Text, ".", "")
    StartGroup4.SelLength = Len(StartGroup4.Text)
End Sub
Private Sub EndGroup1_GotFocus()
    EndGroup1.SelLength = Len(EndGroup1.Text)
End Sub
Private Sub EndGroup2_GotFocus()
    EndGroup1.Text = Replace(EndGroup1.Text, ".", "")
    EndGroup2.SelLength = Len(EndGroup2.Text)
End Sub
Private Sub EndGroup3_GotFocus()
    EndGroup2.Text = Replace(EndGroup2.Text, ".", "")
    EndGroup3.SelLength = Len(EndGroup3.Text)
End Sub
Private Sub EndGroup4_GotFocus()
    EndGroup3.Text = Replace(EndGroup3.Text, ".", "")
    EndGroup4.SelLength = Len(EndGroup4.Text)
End Sub

