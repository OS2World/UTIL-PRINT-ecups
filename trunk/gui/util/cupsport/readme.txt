CUPSPORT                                                                  

  Simple program to create a new CUPS printer port pointing to the specified
  CUPSD host and queue.                                                     
                                                                          

BUILDING

  No makefile at the moment, sorry.  Use BUILDICC.CMD to compile using IBM
  VACPP 3.08; 3.65 will probably work as well.  

  Not tested with GCC or OW, but they should be fine for building with as
  long as you know how.


USAGE
                                                                          
  Syntax:  cupsport <portname> <hostname> <queuename>                       

  For this to work properly, the latest CUPS.PDR (BLDLEVEL 1.03 or higher)
  needs to be used.  This version implements the SplPdSet and SplPdQuery
  functions which seem to be required for dynamic port creation to work 
  from an application.

  Without these updates, the created WPS printer object may not be usable 
  for printing until after the WPS is restarted, as the CUPS port fails to
  get initialized properly.  (You can tell when this happens if the printer 
  object shows an error message like "port is not accepting data", or 
  "paper tray not available" when you open it.)  
 
