PM Printer Manager
Version 0.45

  Printer Manager is a new graphical printer management tool designed to 
  replace several older, separate programs by integrating their functions
  in one, centralize utility:
    - INSTPDR.EXE (done)
    - ECSPRINT.EXE (done)
    - CUPSWIZ.EXE (not yet implemented)


INSTALLATION

  Requirements:
   - VROBJ.DLL (widely available, included with eCS)
   - WPTOOLS.DLL (widely available, included with eCS)
   - PR1UTIL.DLL (widely available, included with eCS 2.x)
   - VROBJEX.DLL (included here, to be included in eCS 2.2)
   - RXPRTUTL.DLL (included here)

  This program also uses a number of CUPS tools if CUPS is installed.  It
  may work on systems without CUPS (or it may not); this has not been tested.

  Place the contents of the ZIP file into any directory, and create a
  program object for PRINTMAN.EXE.  Make sure the first three DLLs listed
  above are installed on your system (via the LIBPATH).


KNOWN PROBLEMS/LIMITATIONS

  At the moment, choosing to create a printer causes Printer Manager to just
  launch the appropriate external printer wizard and then exit.  Eventually
  this should all be handled by Printer Manager itself.

  If the CUPS server (local or remote) requires password authentication for
  the use of commands like lpadmin, lpinfo, etc. then these programs will 
  block waiting for a password during various operations.  (There doesn't seem
  to be any way to fix this other than replacing the programs entirely... I'm
  still looking at possible solutions.)

  There is no help file.


LICENSE

  Source code is available via NetLabs SVN at:  http://svn.netlabs.org/ecups

  PM Printer Manager
  (C) 2013 Alexander Taylor
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
  Permission is granted for the inclusion of the PM Printer Manager binaries
  in the eComStation operating system.  In this case, the full source code 
  need not be included in the OS distribution so long it remains publically
  available through affiliated channels and/or in a location which is clearly
  indicated by the accompanying documentation.

-- 
Alex Taylor
August 2013
