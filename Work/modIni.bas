Attribute VB_Name = "modIni"
Public Declare Function GetPrivateProfileString Lib "kernel32" Alias "GetPrivateProfileStringA" (ByVal lpApplicationName As String, ByVal lpKeyName As Any, ByVal lpDefault As String, ByVal lpReturnedString As String, ByVal nSize As Long, ByVal lpFileName As String) As Long
Public Declare Function WritePrivateProfileString Lib "kernel32" Alias "WritePrivateProfileStringA" (ByVal lpApplicationName As String, ByVal lpKeyName As Any, ByVal lpString As Any, ByVal lpFileName As String) As Long
Public Declare Function WritePrivateProfileSection Lib "kernel32" Alias "WritePrivateProfileSectionA" (ByVal lpApplicationName As String, ByVal lpString As String, ByVal lpFileName As String) As Long


Public Function ReadValue(IniFile As String, Section As String, Key As String, Optional Default As String)
    Dim sReturn As String
    sReturn = String(255, Chr(0))
    ReadValue = Left(sReturn, GetPrivateProfileString(Section, Key, Default, sReturn, Len(sReturn), IniFile))
    ReadValue = Replace(ReadValue, Chr(0), "", 1, Len(ReadValue), vbTextCompare)
End Function

Public Sub SaveValue(IniFile As String, Section As String, Key As String, Value As String)
    WritePrivateProfileString Section, Key, Value, IniFile
End Sub

Public Sub DelSection(IniFile As String, Section As String, Key As String)
    WritePrivateProfileSection Section, Key, IniFile
End Sub
