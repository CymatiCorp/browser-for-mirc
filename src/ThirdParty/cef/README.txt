Chromium Embedded Framework (CEF) Binary Distribution
-------------------------------------------------------------------------------

CEF Revision:        42
Chromium Revision:   26432
Date:                September 17, 2009

This distribution contains all files necessary to build an application using
CEF.  Please read the included LICENSE.txt file for licensing terms and
restrictions.


CONTENTS
--------

build       Contains the icudt38.dll and libcef.dll DLLs required to run
            CEF-based applications.  Also acts as the build target for the
            included cefclient sample application.

cefclient   Contains the cefclient sample application configured to build
            using the files in this distribution.

include     Contains all required CEF and NPAPI-related header files.  Read
            the include/npapi/NPAPI-README.txt file for more information about
            the NPAPI-related header files.

lib         Contains the libcef.lib and libcef_dll_wrapper.lib library files
            that all CEF-based C++ applications must link against.


USAGE
-----

Open the cefclient/cefclient.sln solution in Visual Studio 2005 and build.
Please visit the CEF Website for additional usage information.

http://code.google.com/p/chromiumembedded
