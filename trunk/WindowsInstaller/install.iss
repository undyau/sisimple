; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{E60CAA80-80BE-4FE4-BA63-F5321FF6A18C}
AppName=SI Simple
AppVersion=0.8.1
;AppVerName=SI Simple 0.8.1
AppPublisher=undy
AppPublisherURL=http://sisimple.sourceforge.net
AppSupportURL=http://sisimple.sourceforge.net
AppUpdatesURL=http://sisimple.sourceforge.net
DefaultDirName={pf}\SI Simple
DefaultGroupName=SI Simple
LicenseFile=D:\Andy Documents\Projects\sisimple\gpl.txt
OutputDir=D:\Andy Documents\Projects\sisimple\WindowsInstaller
OutputBaseFilename=SISimpleSetup
SetupIconFile=D:\Andy Documents\Projects\sisimple\sisimple.ico
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
; NOTE: Don't use "Flags: ignoreversion" on any shared system files
Source: sisimple.exe; DestDir: {app}; Flags: ignoreversion; 
Source: libgcc_s_dw2-1.dll; DestDir: {app}; Flags: ignoreversion; 
Source: mingwm10.dll; DestDir: {app}; Flags: ignoreversion; 
Source: QtCore4.dll; DestDir: {app}; Flags: ignoreversion; 
Source: QtGui4.dll; DestDir: {app}; Flags: ignoreversion; 
Source: QtXml4.dll; DestDir: {app}; Flags: ignoreversion; 
Source: QtSvg4.dll; DestDir: {app}; Flags: ignoreversion; 
Source: QtNetwork4.dll; DestDir: {app}; Flags: ignoreversion; 
Source: qsvgicon4.dll; DestDir: {app}\iconengines; Flags: ignoreversion; 


[Icons]
Name: "{group}\SI Simple"; Filename: "{app}\sisimple.exe"
Name: "{commondesktop}\SI Simple"; Filename: "{app}\sisimple.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\sisimple.exe"; Description: "{cm:LaunchProgram,SI Simple}"; Flags: nowait postinstall skipifsilent