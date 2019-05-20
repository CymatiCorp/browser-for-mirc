# CEF chromium framework for mIRC

Acquired from project home at sourceforge:
https://sourceforge.net/projects/mchef/

Built with Visual C++ 2008 Express
on a Microsoft Windows 10 installation

VC 2008 Express does not contain ATL/MFC libraries , a work-around <br>
was to use ATL/MFC folders. 

(Added include folders)
```
Tools -> Options -> Project and Solutions -> VC++ Directories

Platform: Win32

Show directories for:  Include Files

C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.20.27508\atlmfc\include\
C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.20.27508\atlmfc\lib\x86\
C:\Program Files (x86)\Windows Kits\10\Include\10.0.17763.0\um


```

ATL/MFC Include files were acquired from the VC++ community 2019 build. ( Calls made to afxres.h ) <br>
the afxres.h then calls winres.h  which is acquired from \Windows Kits\ SDK

```
mChef Properties -> Configuration Properties -> General -> Use of MFC->  (Use Standard Window Libraries)
```

```
Changed Build Events for ver.exe
to be "Excluded from Build: Yes"
```


Visual Studio 2008 Express: <br>
http://download.microsoft.com/download/8/B/5/8B5804AD-4990-40D0-A6AA-CE894CBBB3DC/VS2008ExpressENUX1397868.iso <br><br>
Window Kits can be acquired here <br>
https://developer.microsoft.com/en-us/windows/downloads/windows-10-sdk <br><br>
Visual Studio Community 2019 <br>
https://visualstudio.microsoft.com/vs/ <br><br>

Note: 
Installing via  Visual Studio Community 2017 and 2019 
failed with various issues relating to paths.  
` "DirectoryName illegal characters in path." `
Which was due to quotation marks being read in the path.
Then we have a mismatch of library folders.  
Other issues were raised. Eventually it was decided to use it's 
native compiler VC2008 
 
