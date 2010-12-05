; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{9C1C8AB3-FD1C-46E2-8387-835E9C7CAD8A}
AppName=SI Simple
AppVersion=0.4
;AppVerName=SI Simple 0.4
AppPublisher=undy
AppPublisherURL=http://sisimple.sourceforge.net
AppSupportURL=http://sisimple.sourceforge.net
AppUpdatesURL=http://sisimple.sourceforge.net
DefaultDirName={pf}\SI Simple
DefaultGroupName=SI Simple
LicenseFile=C:\Documents and Settings\Andy\Desktop\license.txt
OutputDir=D:\Andy Documents\Projects\sisimple\WindowsInstaller
OutputBaseFilename=sisimple.0.4setup
SetupIconFile=D:\Andy Documents\Projects\sisimple\sisimple.ico
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "C:\Documents and Settings\Andy\Desktop\sisimple.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Documents and Settings\Andy\Desktop\libgcc_s_dw2-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Documents and Settings\Andy\Desktop\mingwm10.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Documents and Settings\Andy\Desktop\QtCore4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Documents and Settings\Andy\Desktop\QtGui4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Documents and Settings\Andy\Desktop\QtSvg4.dll"; DestDir: "{app}"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\SI Simple"; Filename: "{app}\sisimple.exe"
Name: "{commondesktop}\SI Simple"; Filename: "{app}\sisimple.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\sisimple.exe"; Description: "{cm:LaunchProgram,SI Simple}"; Flags: nowait postinstall skipifsilent

