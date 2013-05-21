:userdoc.

.* ----------------------------------------------------------------------------
:h1 res=100 name=intro
    x=left y=bottom width=100% height=100%.General Information
:p.The eCups Printer Wizard is a simple graphical front-end for creating CUPS-based
printers.
:p.This program allows you to select your printer make and model from the list of
those supported by the CUPS server, or to import a CUPS-compatible PPD file to
identify your printer directly.  Then you can set up the printer connection
(whether via USB or one of a number of types of network connection), give it a
name and description, and your printer will be created with all the necessary
parameters.
:p.The eCups Printer Wizard operates in one of two modes.  By default, it looks
for a working installation of CUPS on the local system.  If found, it queries
CUPS for a list of supported printers (which depends on the installed options)
and allows you to select one or provide a PPD.
:p.If a local CUPS install cannot be found, the program will revert to
&osq.remote&csq. operation mode.  In this mode, you are required to have a
working installation of CUPS accessible on your local network, and any printer
objects you create will redirect all jobs to a particular print queue (which
must exist) on that server.  In this mode, you are required to identify the
model of printer by providing a PPD file directly.
:p.You can force the program to run in &osq.remote&csq. mode by starting it
with the parameter &osq./R&csq..
:p.Note that &osq.remote&csq. mode has certain limitations, in particular&colon.
:ul.
:li.Applications which require the existence of a local CUPS print queue will
not be able to print in this mode.  (This particularly applies to QT-based
programs.)
:li.The remote CUPS server must not have password authentication enabled.
:eul.


.* ............................................................................
:h2 res=110 name=howitworks
    x=left y=bottom width=100% height=100%.Technical Information
:p.This section provides a brief explanation of how the eCups printing system
works.
:p.CUPS (the Common Unix Printing System) is essentially a self-contained
print subsystem, which includes its own print spooler plus a collection of
plugin-like printer drivers.  The CUPS :hp1.server:ehp1. runs on the system
as a background process and manages all CUPS-based printers and jobs.
:p.In principle, CUPS itself operates independently of the standard desktop
printer system.  Applications which are &osq.CUPS-aware&csq.
can print directly to CUPS, bypassing any desktop printer objects entirely.
:p.However, most applications are not specifically written to use CUPS, which
means they must still submit print jobs through a standard desktop printer
object and queue.  With eCups, these jobs are then forwarded on to the CUPS
server, which controls the actual print processing.
:nt.&osq.eCups&csq.is the term used to describe the entire framework by which
the OS/2 printing system is linked together with the cross-platform CUPS
components, in order to provide seamless CUPS-based printing for all
applications.:ent.
:p.:hp2.Legacy Printing Logic:ehp2.
:p.With standard (non-CUPS or &osq.legacy&csq.) printing, the printer
configuration has three major parts&colon.
:ul.
:li.The desktop printer object, which corresponds to a &osq.print queue&csq.
controlled by the Presentation Manager spooler.
:li.The presentation driver, which converts print data from an application
into a format suitable for the printer.
:li.The port driver, which is responsible for transmitting the print
job data to the printer, across the appropriate (physical or network) connection.
:eul.
:p.
:p.:hp2.eCups Printing Logic:ehp2.
:p.An eCups printer configuration includes these components&colon.
:ul.
:li.The desktop printer object, which corresponds to a &osq.print queue&csq.
controlled by the Presentation Manager spooler.
:li.The presentation driver, which converts print data from an application
into a PostScript file which may (optionally) contain printer-specific job
properties.
:li.The eCups port driver, which is responsible for transmitting the
PostScript job file to the CUPS server.
:li.The CUPS server itself, which manages a separate CUPS printer definition
and queue for the printer in question.  The CUPS server takes the incoming
PostScript file, validates it, and applies any additional job properties
that are defined by the CUPS printer definition.
:li.The CUPS printer driver, which converts the validated PostScript data
into printer-specific format.  CUPS printer drivers are typically distributed
as omnibus driver packages from various sources (both commercial and
community-based); popular packages include Gutenprint, SpliX and HP-LIP.
:li.The CUPS &osq.backend&csq. (analogous to a port driver) which is
responsible for transmitting the converted print job data to the printer,
across the appropriate (physical or network) connection.
:eul.
:p.The eCups Printer Wizard is designed to automate the configuration of all
of these components according to your selections.
:p.As you can see, eCups printing is more complex than standard (legacy)
printing.  A common cause for confusion is the fact that every eCups
printer is essentially defined twice&colon. once as a desktop printer
object, and once as a CUPS printer within the CUPS server.
:p.This dual configuration makes the setting of print job properties somewhat
complicated.  Some points to be aware of&colon.
:ul.
:li.A normal (non-CUPS-aware) application is only capable of setting
job properties that are known to the desktop printer object.
:li.A CUPS-aware application, conversely, will only use the CUPS printer
definition to determine the job properties.
:li.Every CUPS printer has default job properties associated with it, as
defined within CUPS (using the browser-based CUPS administration GUI).
:li.However, you can also configure a printer's default job properties in
the desktop printer object.  Job properties defined in this way will only
apply to non-CUPS-aware applications.
:eul.
:p.Any job property which is not defined through the desktop printer object
will be taken from the CUPS printer definition.  This provides one notable
advantage&colon. it is thus possible to use a generic PostScript driver for
the desktop printer object, and still take advantage of printer-specific
features so long as the CUPS printer definition is aware of them.  (The
trade-off is that non-CUPS-aware applications would then be unable to set
printer-specific properties for a particular job at print time.)
:p.For more detailed information regarding these and related topics, refer to
the eCups FAQ at&colon. http&colon.//svn.netlabs.org/ecups/wiki/CupsFaq

.* ----------------------------------------------------------------------------
:h1 res=200 name=model
    x=left y=bottom width=100% height=100%.Select Printer
:p.The first step in creating a CUPS printer object is to select the
manufacturer and model of your printer.
:p.If the precise model of your printer is not listed, you may be able to
select a similar model.
:p.For information about what printers are supported, and what driver(s) may
be compatible, you can refer to the OpenPrinting database at&colon.
http&colon.//www.openprinting.org/printers

:p.:hp2.Importing a PPD:ehp2.
:p.If your printer natively supports PostScript, you can choose to import a
manufacturer-supplied PPD file using the :hp2.--Custom--:ehp2.
option.
:nt.You can also choose to import a PPD file as a shortcut to selecting your
printer's make and model from the list.  However, if your printer is not a
native PostScript printer, it :hp1.must:ehp1. be supported by one of the CUPS
driver packages that you have installed (and the PPD file you provide must be
have been specially written for use with CUPS).:ent.

:p.:hp2.Before Continuing:ehp2.
:p.Make sure that your printer is connected (either physically or through a
working network connection) and powered on before you select the :hp2.Next:ehp2.
button.


.* ----------------------------------------------------------------------------
:h1 res=300 name=connection
    x=left y=bottom width=100% height=100%.Select Connection
:p.When you come to this page, the eCups Printer Wizard will scan your system
and network for connected printers.  (On slow systems or networks this may
take several moments.)
:p.Depending on your network configuration, not all network printers may be
detected automatically.  If your network printer is not detected, select
:hp2.Other printer (manual configuration):ehp2. and configure it manually.  (See
:link reftype=hd refid=network.Configuring a network printer:elink. for details.)

:p.If you are installing a USB printer, it should appear in the list of
detected printers.  If it does not appear, exit the eCups Printer Wizard,
verify that the printer is connected and powered on, and try again.


.* ............................................................................
:h2 res=310 name=network
    x=left y=bottom width=100% height=100%.Configuring a network printer
:p.The settings on this dialog specify how your network printer is connected.
:p.First, you must select the type of network connection using the
:hp2.Network protocol:ehp2. control.  The remaining options will vary
depending on the network type.
:dl break=all.
:dt.:hp2.Internet Printing Protocol (IPP):ehp2.
:dd.IPP is a modern TCP/IP protocol for network printers which supports a
number of advanced features like encryption, authentication, and job control.  CUPS
itself uses IPP when sharing its printers out over the network, so you would
normally select this option when connecting to a remote CUPS server.
:dl break=fit tsize=30.
:dt.Printer or server address
:dd.Enter the IP address or hostname of the remote server.
:dt.Printer queue name
:dd.Enter the name of the printer device on the server.  The required format
may vary depending on the particular print server; for a remote CUPS server,
the name normally takes the form&colon. :xmp.printers/:hp1.name:ehp1.
:exmp.
For other IPP printer servers, consult the vendor's documentation to determine
the proper queue name to use.
:dt.User ID &amp. Password
:dd.If the server requires a user ID and/or password, enter them here; otherwise,
these fields may be left blank.
:edl.
:dt.:hp2.AppSocket/JetDirect:ehp2.
:dd.This is a simple TCP/IP protocol, sometimes called &osq.raw&csq. IP printing.  It
is supported by many network printers and print servers.
:dl break=fit tsize=30.
:dt.Printer or server address
:dd.Enter the IP address or hostname of the network print server.  For printers
with built-in JetDirect support, this will be the IP address of the printer
itself.
:edl.
:dt.:hp2.Line Printer Remote daemon (LPD):ehp2.
:dd.LPD (or LPR) is the oldest and most common network printing protocol in use;
virtually all TCP/IP network printers and print servers should support it.
:dl break=fit tsize=30.
:dt.Printer or server address
:dd.Enter the IP address or hostname of the network print server.  For
printers with built-in LPD servers, this will be the IP address of the printer
itself.
:dt.Printer queue name
:dd.Enter the name of the printer device on the server.  If no explicit device
name is required (which may be the case when the LPD server is built into
the printer itself), specify :hp2.*:ehp2. or leave this field blank.
:dt.User ID (if required)
:dd.If the server requires a user ID, enter it here; otherwise, leave this
field blank.
:edl.
:dt.:hp2.Windows/SMB network:ehp2.
:dd.The SMB protocol is used by Windows Networking, as well as Samba servers and
various dedicated print servers.
:dl break=fit tsize=30.
:dt.Print server name
:dd.Enter the network (LAN) name of the printer server.  (Do not include a
&osq.\\&csq. prefix.)
:dt.Shared printer name
:dd.Enter the network share name of the remote printer.
:dt.User ID &amp. Password
:dd.If a user ID and password are required to access this shared printer,
enter them here; otherwise, these fields may be left blank.
:edl.
:dt.:hp2.Existing CUPS printer:ehp2.
:dd.This option assumes that the printer definition has already been created on
the (local or remote) CUPS server.  Choosing it will cause the eCups Printer
Wizard to bypass the creation of the CUPS printer queue, and simply create a
Presentation Manager print device and port to send jobs directly to the CUPS
server.
:dl break=fit tsize=30.
:dt.CUPS server name
:dd.Enter the IP address or hostname of the CUPS server where this printer
is already defined.  For the local system, use &osq.localhost&csq. or the
IP address 127.0.0.1
:dt.CUPS printer name
:dd.Enter the name of the CUPS printer queue as defined on the server.
:edl.
:p.You may use this option as an alternative to IPP for printing to a remote
CUPS server.  However, in this case there will be no local CUPS queue for the
printer in question, which may prevent CUPS-aware applications from accessing
that printer.
:p.This option requires that the CUPS server not be configured to require
authentication for printing.
:nt.If CUPS is not locally installed, then this will be the only option
available.:ent.
:edl.
:p.All network protocols except for :hp2.Existing CUPS printer:ehp2. require
CUPS to be installed on the local system.
:p.Choose whichever protocol your printer (or its print server) best supports.  Consult
your printer or server documentation, or ask your network administrator.  If unsure,
try using LPD or AppSocket first.
:nt.If you want to access a printer which is already defined on a remote CUPS
server, you have the option of selecting either :hp2.Internet Printing Protocol
(IPP):ehp2. or :hp2.Existing CUPS printer:ehp2..  However, of these two methods,
only the former (IPP) will result in a local CUPS print queue being created, which
may be required by certain applications (in particular those based on the QT4
libraries).  The former method must also be used if the CUPS server requires a
user ID and password for printing.:ent.

.* ----------------------------------------------------------------------------
:h1 res=400 name=identify
    x=left y=bottom width=100% height=100%.Identify Printer
:p.On this page, you must enter some information which allows both CUPS and
the desktop printing system (as well as you) to identify the new printer.
:dl break=fit tsize=15.
:dt.Name
:dd.This is a short name which CUPS will use internally to identify the
printer.  It must start with a letter of the alphabet, and may not include
spaces, tabs, or the characters :hp2./:ehp2. or :hp2.#:ehp2..
:dt.Location
:dd.This is a brief, free-form description of where the printer is located
(such as "Office", "Home", or "North-east corner by the photocopier".)
:dt.Description
:dd.This is a human-readable name for the printer.  What you enter here
will be used for the title of the printer object that gets created on your
desktop.
:edl.

.* ----------------------------------------------------------------------------
:h1 res=500 name=confirm
    x=left y=bottom width=100% height=100%.Confirm Create Printer
:p.This dialog appears when the eCups Printer Wizard is ready to create
the new printer.  You can review the selected settings before confirming
the printer creation.
:dl break=fit tsize=24.
:dt.Model, Name, etc.
:dd.These fields are purely informational, and correspond to the selections you
entered on the previous pages.
:dt.Create printer object
:dd.This checkbox determines whether a desktop printer object will be created.
If deselected, only a CUPS printer definition will be created.  Normally you
should leave this selected, as most applications require a desktop printer
object in order for printing to function.
:dt.Presentation driver
:dd.If you have multiple eCups-compatible presentation (desktop) printer drivers
installed, you may select the one which the desktop printer object will be
configured to use.  It is recommended, however, that you accept the default
selection.  This has no effect on the internal CUPS printer configuration.
:edl.
:euserdoc.

