Outcast Lives Forever (OCLF)
============================
Following most recent Outcast 1.1 (re)release and many questions related to
file formats and OCLF source code I felt it is the right time to make it open
source and hope that other Outcast fans out there will make good use of it.

Most of the code and work has been done back in 2002-2003 during the same time
I worked on 64k intros thus some parts of this code, mostly renderer and math
library, are coming from there with some minor modifications. Compared to
actual intro code this is not as convoluted so I had very little clean up to do
in order to release it. It is pretty much the same. Some unused files were
removed as well as commented lines with temporary code. But there is some
inconsistency in coding style since some parts were written at different times
for different things while being influenced by something else.

Original project was setup for Visual Studio 6 which I could no longer find
anywhere so all those old solution files are not included. Instead there is new
solution for Visual Studio 2010 Express which is more easily available at the
moment. All warnings are disabled though since there are tons of them.

What do we have in here:

    data/  
      luxsys/        - Data files for the viewer UI (artwork, shader binaries).  
        shdsrc/      - Pixel and vertex shader assembly source files.  
    DCL/             - Reverse engineered PKWARE's decompression routines.  
    OC_data/         - Classes that map to original Outcast file formats.  
    OC_pack/         - Pack file creator (no compression).  
    OC_unpack/       - Deprecated pak file unpacker. Used it prior to DCL.  
    OC_viewer/       - Main viewer app and core subsystems.  
    OC3D/            - "Client / User" code of the viewer so to speak.  
    vc2010-build/    - VC2010 Express solution and projects.  
    XFont/           - Deprecated font file converter. Used just for one file.  
    xmath/           - Trivial math classes and helpers (vector, matrix, etc).  
    oc_readme.txt    - Original readme file and work log.  

How to build and run it:

* Install Visual Studio 2010 Express. It is free.
* Install DirectX SDK. I have tested with June 2010 release. Make sure that
  **DXSDK_DIR** environment variable is set up correctly.  
  *e.g. DXSDK_DIR=C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\*
* Install NASM and make sure it is added to your **path** environment variable.  
  *e.g. path=%path%;C:\Program Files (x86)\NASM*
* (Optional) Install Windows or Platform SDK.
* Open *vc2010-build\oclf\oclf.sln* and build the solution (Debug or Release).
  This will generate executables and luxsys.pak files into
  *vc2010-build\oclf\bin* directory.
* Copy OC_viewer.exe and luxsys.pak into your Outcast game directory
  where all **pak** or **opk** (Outcast 1.1) files are placed.  
  *e.g. C:\Program Files (x86)\Steam\steamapps\common\Outcast 1.1\Legacy\*
* Run OC_viewer.exe
* Have fun!

Original executables could be found here http://and.intercon.ru/releases/fanstuff/oclf

On a final note I would like to give extra credit to Outcast modding
community that did some file format investigation efforts back in days and
published those on forums that no longer exist. As well as a guy whose name I
don't remember that investigated animation file formats and how mesh skinning
was done.

Cheers!  
*released December 2014*
