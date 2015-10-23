VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form frmDownload 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Download"
   ClientHeight    =   4845
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   7350
   Icon            =   "frmdownload.frx":0000
   LinkTopic       =   "Form5"
   MaxButton       =   0   'False
   ScaleHeight     =   4845
   ScaleWidth      =   7350
   Begin hdcp2p.DownLoad dl 
      Left            =   6600
      Top             =   120
      _ExtentX        =   847
      _ExtentY        =   847
   End
   Begin VB.CommandButton Command1 
      Caption         =   "&Cancel"
      Height          =   255
      Left            =   120
      TabIndex        =   10
      Top             =   1320
      Visible         =   0   'False
      Width           =   855
   End
   Begin MSComctlLib.ProgressBar pb1 
      Height          =   255
      Left            =   120
      TabIndex        =   2
      Top             =   1320
      Width           =   6735
      _ExtentX        =   11880
      _ExtentY        =   450
      _Version        =   393216
      Appearance      =   1
      Scrolling       =   1
   End
   Begin MSComctlLib.StatusBar sb1 
      Align           =   2  'Align Bottom
      Height          =   255
      Left            =   0
      TabIndex        =   1
      Top             =   4590
      Width           =   7350
      _ExtentX        =   12965
      _ExtentY        =   450
      Style           =   1
      _Version        =   393216
      BeginProperty Panels {8E3867A5-8586-11D1-B16A-00C0F0283628} 
         NumPanels       =   1
         BeginProperty Panel1 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
         EndProperty
      EndProperty
   End
   Begin MSComctlLib.ImageList imgDL 
      Left            =   1320
      Top             =   3600
      _ExtentX        =   1005
      _ExtentY        =   1005
      BackColor       =   -2147483643
      ImageWidth      =   16
      ImageHeight     =   16
      MaskColor       =   12632256
      _Version        =   393216
      BeginProperty Images {2C247F25-8591-11D1-B16A-00C0F0283628} 
         NumListImages   =   4
         BeginProperty ListImage1 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmdownload.frx":2372
            Key             =   ""
         EndProperty
         BeginProperty ListImage2 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmdownload.frx":4B24
            Key             =   ""
         EndProperty
         BeginProperty ListImage3 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmdownload.frx":4F76
            Key             =   ""
         EndProperty
         BeginProperty ListImage4 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmdownload.frx":53C8
            Key             =   ""
         EndProperty
      EndProperty
   End
   Begin MSComctlLib.ListView lvDL 
      Height          =   2895
      Left            =   0
      TabIndex        =   0
      Top             =   1680
      Width           =   7335
      _ExtentX        =   12938
      _ExtentY        =   5106
      View            =   3
      LabelEdit       =   1
      LabelWrap       =   -1  'True
      HideSelection   =   -1  'True
      _Version        =   393217
      SmallIcons      =   "imgDL"
      ForeColor       =   -2147483640
      BackColor       =   -2147483643
      BorderStyle     =   1
      Appearance      =   1
      NumItems        =   3
      BeginProperty ColumnHeader(1) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         Text            =   "File:"
         Object.Width           =   7832
      EndProperty
      BeginProperty ColumnHeader(2) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         SubItemIndex    =   1
         Text            =   "Size (bytes):"
         Object.Width           =   2540
      EndProperty
      BeginProperty ColumnHeader(3) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         SubItemIndex    =   2
         Text            =   "Progress:"
         Object.Width           =   3529
      EndProperty
   End
   Begin VB.Label Label5 
      Alignment       =   1  'Right Justify
      BackStyle       =   0  'Transparent
      Caption         =   "0"
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
      Height          =   255
      Left            =   0
      TabIndex        =   13
      Top             =   0
      Width           =   375
   End
   Begin VB.Label lblRate 
      AutoSize        =   -1  'True
      BackStyle       =   0  'Transparent
      Caption         =   "0kb"
      Height          =   195
      Left            =   1200
      TabIndex        =   12
      Top             =   960
      Width           =   270
   End
   Begin VB.Label Label3 
      AutoSize        =   -1  'True
      BackStyle       =   0  'Transparent
      Caption         =   "Speed:"
      Height          =   180
      Left            =   120
      TabIndex        =   11
      Top             =   960
      Width           =   600
   End
   Begin VB.Line Line2 
      BorderColor     =   &H00FFFFFF&
      X1              =   0
      X2              =   7920
      Y1              =   10
      Y2              =   10
   End
   Begin VB.Line Line1 
      BorderColor     =   &H00808080&
      X1              =   0
      X2              =   7920
      Y1              =   0
      Y2              =   0
   End
   Begin VB.Label lblProgress 
      BackStyle       =   0  'Transparent
      Caption         =   "/"
      Height          =   255
      Left            =   1200
      TabIndex        =   9
      Top             =   720
      Width           =   2775
   End
   Begin VB.Label Label4 
      AutoSize        =   -1  'True
      BackStyle       =   0  'Transparent
      Caption         =   "Progress :"
      Height          =   180
      Left            =   120
      TabIndex        =   8
      Top             =   720
      Width           =   885
   End
   Begin VB.Label lblPercent 
      Alignment       =   1  'Right Justify
      BackStyle       =   0  'Transparent
      Caption         =   "0"
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
      Height          =   255
      Left            =   6840
      TabIndex        =   7
      Top             =   1320
      Width           =   375
   End
   Begin VB.Label lblLFile 
      BackStyle       =   0  'Transparent
      Caption         =   "--"
      Height          =   255
      Left            =   1200
      TabIndex        =   6
      Top             =   360
      Width           =   6735
   End
   Begin VB.Label Label2 
      AutoSize        =   -1  'True
      BackStyle       =   0  'Transparent
      Caption         =   "Save File:"
      Height          =   180
      Left            =   120
      TabIndex        =   5
      Top             =   360
      Width           =   840
   End
   Begin VB.Label lblRFile 
      BackStyle       =   0  'Transparent
      Caption         =   "--"
      Height          =   255
      Left            =   1200
      TabIndex        =   4
      Top             =   120
      Width           =   6735
   End
   Begin VB.Label Label1 
      AutoSize        =   -1  'True
      BackStyle       =   0  'Transparent
      Caption         =   "File Location:"
      Height          =   180
      Left            =   120
      TabIndex        =   3
      Top             =   120
      Width           =   1140
   End
End
Attribute VB_Name = "frmDownload"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim strFilePath As String

Private Sub Command1_Click()

On Error Resume Next

dl.Cancel

lblRFile = "N/A"
lblLFile = "N/A"
lblPercent = "N/A"
lblProgress = "N/A"
lblRate = "N/A"
pb1.Value = 0
doingDL = False

Kill strFilePath & "\" & lvDL.SelectedItem.Text

lvDL.ListItems(currIndex).ListSubItems(2).Text = "Download Quit."
lvDL.ListItems(currIndex).SmallIcon = 3

currIndex = currIndex + 1

doingDL = False
End Sub

Private Sub dl_DLComplete()

'On Error Resume Next

lvDL.ListItems(lvDL.SelectedItem.Index).ListSubItems(2).Text = "Download Complete!"
lvDL.ListItems(lvDL.SelectedItem.Index).SmallIcon = 2

currIndex = lvDL.SelectedItem.Index + 1

If currIndex > lvDL.ListItems.Count Then
    lblRFile = "N/A"
    lblLFile = "N/A"
    lblPercent = "N/A"
    lblProgress = "N/A"
    lblRate = "N/A"
    pb1.Value = 0
    doingDL = False
    Exit Sub
Else
    lvDL.ListItems(currIndex).Selected = True
    lvDL.SelectedItem.SmallIcon = 1
    lblLFile.Caption = strFilePath & "\" & lvDL.SelectedItem.Text
    lblRFile.Caption = lvDL.SelectedItem.Key
    
    pb1.Max = lvDL.SelectedItem.ListSubItems(1).Text
    
    dl.Url = lvDL.SelectedItem.Key
    dl.GetFileInformation
    dl.SaveLocation = strFilePath & "\" & lvDL.SelectedItem.Text
    dl.DownLoad
End If

End Sub

Private Sub dl_DLError(lpErrorDescription As String)

On Error Resume Next

lvDL.ListItems(currIndex).SmallIcon = 3
lvDL.ListItems(currIndex).ListSubItems(2).Text = lpErrorDescription

currIndex = currIndex + 1

If currIndex > lvDL.ListItems.Count Then
    lblRFile = "N/A"
    lblLFile = "N/A"
    lblPercent = "N/A"
    lblProgress = "N/A"
    lblRate = "N/A"
    pb1.Value = 0
    doingDL = False
    Exit Sub
Else
    lvDL.ListItems(currIndex).Selected = True
    lvDL.SelectedItem.SmallIcon = 1
    lblLFile.Caption = strFilePath & "\" & lvDL.SelectedItem.Text
    lblRFile.Caption = lvDL.SelectedItem.Key
    dl.Url = lvDL.SelectedItem.Key
    dl.GetFileInformation
    dl.SaveLocation = strFilePath & "\" & lvDL.SelectedItem.Text
    dl.DownLoad
End If

End Sub

Private Sub dl_Percent(lPercent As Long)
lblPercent = lPercent & "%"
End Sub

Private Sub dl_Rate(lpRate As String)
lblRate.Caption = lpRate
End Sub

Private Sub dl_RecievedBytes(lnumBYTES As Long)
On Error Resume Next
lblProgress.Caption = lnumBYTES & " / " & lvDL.ListItems(currIndex).ListSubItems(1).Text
pb1.Value = lnumBYTES
End Sub

Private Sub dl_StatusChange(lpStatus As String)
On Error Resume Next
lvDL.ListItems(currIndex).ListSubItems(2).Text = lpStatus
End Sub

Private Sub Form_Load()
Dim IniPath As String
IniPath = App.Path & "\" & "P2P.ini"
strFilePath = ReadValue(IniPath, "Folder", "SharedFolder")
doingDL = True
End Sub

Private Sub Form_Paint()
On Error Resume Next
lvDL.Width = Me.ScaleWidth
lvDL.Height = Me.ScaleHeight - 2000
End Sub

Private Sub Form_Resize()
Form_Paint
End Sub

Private Sub lvDL_DblClick()

currIndex = lvDL.SelectedItem.Index

lvDL.SelectedItem.SmallIcon = 1
lblLFile.Caption = strFilePath & "\" & lvDL.SelectedItem.Text
lblRFile.Caption = lvDL.SelectedItem.Key

dl.Url = lvDL.SelectedItem.Key
dl.GetFileInformation
dl.SaveLocation = strFilePath & "\" & lvDL.SelectedItem.Text
dl.DownLoad

End Sub

Private Sub mnuCancel_Click()
Command1_Click
End Sub

Private Sub mnuDelQueue_Click()
If lvDL.SelectedItem.SmallIcon = 1 Then
    If MsgBox("Download Stop." & vbCrLf & "Continue?", vbQuestion + vbYesNo) = vbYes Then
        Command1_Click
        lvDL.ListItems.Remove (lvDL.SelectedItem.Index)
    End If
Else
    lvDL.ListItems.Remove (lvDL.SelectedItem.Index)
End If
End Sub


Private Sub mnuTray_Click()
Me.Hide
Call AddIcon(Me, "Downloading...")
End Sub
