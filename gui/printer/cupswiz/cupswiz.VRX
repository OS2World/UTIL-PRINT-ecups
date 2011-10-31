/*:VRX         Main
*/
/*  Main
*/
Main:
/*  Process the arguments.
    Get the parent window.
*/
    parse source . calledAs .
    parent = ""
    argCount = arg()
    argOff = 0
    if( calledAs \= "COMMAND" )then do
        if argCount >= 1 then do
            parent = arg(1)
            argCount = argCount - 1
            argOff = 1
        end
    end; else do
        call VROptions 'ImplicitNames'
        call VROptions 'NoEchoQuit'
    end
    InitArgs.0 = argCount
    if( argCount > 0 )then do i = 1 to argCount
        InitArgs.i = arg( i + argOff )
    end
    drop calledAs argCount argOff

/*  Load the windows
*/
    call VRInit
    parse source . . spec
    _VREPrimaryWindowPath = ,
        VRParseFileName( spec, "dpn" ) || ".VRW"
    _VREPrimaryWindow = ,
        VRLoad( parent, _VREPrimaryWindowPath )
    drop parent spec
    if( _VREPrimaryWindow == "" )then do
        call VRMessage "", "Cannot load window:" VRError(), ,
            "Error!"
        _VREReturnValue = 32000
        signal _VRELeaveMain
    end

/*  Process events
*/
    call Init
    signal on halt
    do while( \ VRGet( _VREPrimaryWindow, "Shutdown" ) )
        _VREEvent = VREvent()
        interpret _VREEvent
    end
_VREHalt:
    _VREReturnValue = Fini()
    call VRDestroy _VREPrimaryWindow
_VRELeaveMain:
    call VRFini
exit _VREReturnValue

VRLoadSecondary:
    __vrlsWait = abbrev( 'WAIT', translate(arg(2)), 1 )
    if __vrlsWait then do
        call VRFlush
    end
    __vrlsHWnd = VRLoad( VRWindow(), VRWindowPath(), arg(1) )
    if __vrlsHWnd = '' then signal __vrlsDone
    if __vrlsWait \= 1 then signal __vrlsDone
    call VRSet __vrlsHWnd, 'WindowMode', 'Modal'
    __vrlsTmp = __vrlsWindows.0
    if( DataType(__vrlsTmp) \= 'NUM' ) then do
        __vrlsTmp = 1
    end
    else do
        __vrlsTmp = __vrlsTmp + 1
    end
    __vrlsWindows.__vrlsTmp = VRWindow( __vrlsHWnd )
    __vrlsWindows.0 = __vrlsTmp
    do while( VRIsValidObject( VRWindow() ) = 1 )
        __vrlsEvent = VREvent()
        interpret __vrlsEvent
    end
    __vrlsTmp = __vrlsWindows.0
    __vrlsWindows.0 = __vrlsTmp - 1
    call VRWindow __vrlsWindows.__vrlsTmp
    __vrlsHWnd = ''
__vrlsDone:
return __vrlsHWnd

/*:VRX         __NoValue
*/
__NoValue:
    SAY FORMAT( sigl, 6 ) '+++' SOURCELINE( sigl )
    SAY FORMAT( sigl, 6 ) '+++ Uninitialized variable'
EXIT sigl

/*:VRX         __VXREXX____APPENDS__
*/
__VXREXX____APPENDS__:
/*
#append ..\..\Shared\PrintUtl.VRS
*/
return
/*:VRX         ConfirmAndCreate
*/
ConfirmAndCreate: PROCEDURE EXPOSE globals.

    globals.!create = 0
    CALL VRLoadSecondary 'SW_CREATE', 'W'
    IF globals.!create <> 1 THEN RETURN

    CALL SetPage4
    CALL VRSet 'WN_MAIN', 'Pointer', 'WAIT'
    CALL VRSet 'DT_INFO', 'Caption', 'Creating printer...'
    ok = CreatePrinter( globals.!os2printer )
    CALL VRSet 'DT_INFO', 'Caption', ''
    CALL VRSet 'WN_MAIN', 'Pointer', '<default>'

    IF ok <> 0 THEN DO
        SELECT
            WHEN ok == 1 THEN reason = 'Error importing PPD file.'
            WHEN ok == 2 THEN reason = 'Error creating CUPS printer.  See' globals.!log1 'for more information.'
            WHEN ok == 3 THEN reason = 'Error creating printer object.  See' globals.!log1 'for more information.'
            OTHERWISE reason = 'Unknown error.'
        END
        CALL VRMessage VRWindow(), 'The following error occurred when trying to create the printer:' ||,
                       '0d0a0d0a'x || reason, 'Error Creating Printer', 'E'
        CALL VRSet 'DT_INFO', 'Caption', 'The printer could not be created.'
        CALL VRSet 'PB_NEXT',   'Caption', 'Return'
    END
    ELSE DO
        CALL VRSet 'DT_INFO',   'Caption', 'The printer has been created.'
        CALL VRSet 'PB_NEXT',   'Caption', 'Create another'
    END
    CALL VRSet 'PB_CANCEL', 'Caption', 'Close'

    CALL LINEOUT globals.!log1, ''
    CALL LINEOUT globals.!log1

RETURN

/*:VRX         CreateCupsPrinter
*/
CreateCupsPrinter: PROCEDURE EXPOSE globals.

    od = DIRECTORY()
    CALL DIRECTORY globals.!cupsdir'\sbin'
    cups_cmd = 'lpadmin -p' globals.!prt_name '-E'
    IF globals.!prt_info <> '' THEN
        cups_cmd = cups_cmd '-D "'globals.!prt_info'"'
    IF globals.!prt_loc <> '' THEN
        cups_cmd = cups_cmd '-L "'globals.!prt_loc'"'
    IF globals.!prt_dev <> '' THEN
        cups_cmd = cups_cmd '-v "'globals.!prt_port'" -m' globals.!prt_dev
    ELSE
        cups_cmd = cups_cmd '-v "'globals.!prt_port'" -m "'globals.!prt_ppd'"'


    CALL LINEOUT globals.!log1, 'Creating CUPS printer using:'
    CALL LINEOUT globals.!log1, '   ' cups_cmd
    IF VRFileExists( globals.!log2 ) THEN DO
        CALL LINEOUT globals.!log2, ''
        CALL LINEOUT globals.!log2
    END
    ADDRESS CMD '@' cups_cmd '1>NUL 2>>' globals.!log2
    CALL LINEOUT globals.!log1, 'Return code: 0x' || D2X( rc, 4 )
    IF rc <> 0 THEN
        CALL LINEOUT globals.!log1, 'See' globals.!log2 'for details.'
    CALL DIRECTORY od
    CALL LINEOUT globals.!log1, ''

RETURN rc

/*:VRX         CreateOS2Printer
*/
/* Creates an OS/2 printer port, queue, and desktop object which point to
 * the just-created CUPS printer.  Uses the RINSTPRN utility from IBM.
 */
CreateOS2Printer: PROCEDURE EXPOSE globals.
    ARG prnt_drv

    IF globals.!remotecups == '' THEN DO
        host    = LoopbackName()
        printer = globals.!prt_name
    END
    ELSE
        PARSE VAR globals.!remotecups host printer .

    printer_title = globals.!prt_info
    printer_model = globals.!prt_nick
    queue_name    = GetQueueName( printer_title )
    port_name     = GetNextPortName('CUPS')
    use_drv       = globals.!prdrv

    /* Determine the source directories/files to pass to RINSTPRN.EXE.  (Some
     * of this logic is repeated from ImportPPD, but we have to do it again
     * here because (a) we might not have gone through ImportPPD to get to this
     * point, and (b) even if we did, the repository information might have
     * changed.)
     */
    copied_driver = 0
    driver_path = GetDriverSource( prnt_drv )

    IF ( pmdx <> '') & ( globals.!prdrv <> '') & ( globals.!prdesc <> '') THEN DO
        /* Printer driver is in the local repository, so we can just point
         * to that, and use the system PRDESC.LST file.
         */
        use_desc = globals.!prdesc
        use_src  = globals.!repository
    END
    ELSE IF driver_path <> '' THEN DO
        /* Driver is not in the repository, but was found in another designated
         * location.  Point to that location, and create a temporary PRDESC.LST
         * file for it.
         */
        use_src  = VRParseFilePath( driver_path, 'DP')
        use_desc = use_src'\PRDESC.LST'
        CALL CreateDriverList driver_path'\'prnt_drv'.DRV', use_desc
    END
    ELSE DO
        /* Driver is not in the repository.  Try to grab the installed files
         * from under \OS2\DLL, copy them into a temporary working directory,
         * and create a temporary PRDESC.LST file.
         */
        driver_path = globals.!os2dir'\DLL\'prnt_drv'\'prnt_drv'.DRV'

        use_src  = SysTempFileName( globals.!tmpdir'\PDR_????')
        use_desc = use_src'\PRDESC.LST'

        IF VRMkDir( use_src ) == 0 THEN DO
            CALL LINEOUT globals.!log1, 'Failed to create working directory' use_src':' VRError()
            RETURN 1
        END
        IF CopyDriverToSource( driver_path, use_src ) == 0 THEN DO
            CALL LINEOUT globals.!log1, 'Failed to copy driver' driver_path 'to working directory' use_src'.'
            RETURN 1
        END
        CALL CreateDriverList driver_path, use_desc
        copied_driver = 1
    END

    /* Create a new CUPS port. */
    /* TODO currently this assumes that CUPS.PDR is already installed. */
    port_ok = AddPort_CUPS( port_name, host, printer )
    IF port_ok > 1 THEN
        RETURN 1

    rsp_file = globals.!tmpdir'\printer.rsp'
    CALL RSPCreatePrinter rsp_file, prnt_drv, printer_model,,
                          port_name, queue_name, printer_title
    result = ExecRINSTPRN( use_desc, use_drv, use_src, rsp_file )

    IF result <> 0 THEN
        CALL DeletePort port_name
    ELSE IF port_ok == 1 THEN DO
        CALL VRMessage VRWindow(),
                       'The installed version of CUPS.PDR appears to be out of ',
                       'date.  As a result, the desktop printer will not be ',
                       'useable until the desktop is restarted.',,
                       'Port Driver Problem', 'W'
    END

    IF copied_driver == 1 THEN DO
        CALL VRDeleteFile use_src'\*'
        CALL VRRmDir use_src
    END

RETURN result

/*:VRX         CreatePrinter
*/
CreatePrinter: PROCEDURE EXPOSE globals.
    ARG create_os2

    /* Create the CUPS printer */
    IF globals.!remotecups == '' THEN DO
        ok = CreateCupsPrinter()
        IF ok <> 0 THEN
            RETURN 2                    /** RC=2  Error running lpadmin **/
    END

    /* Now create the OS/2 printer object */
    IF create_os2 == 1 THEN DO

        /* Make sure ECUPS.DRV has the printer */
        IF globals.!mode == 2 THEN DO
            /* Always (re)import when a PPD is provided by the user
             */
            ok = ImportPPD('ECUPS', globals.!prt_ppd )
            IF ok <> 0 THEN DO
                CALL LINEOUT globals.!log1, 'PPD import failed:' ok
                RETURN 1                /** RC=1  PPD import failed **/
            END
        END
        ELSE IF PrinterExistsInDRV('ECUPS', globals.!prt_nick ) == 0 THEN DO
            IF globals.!prt_ppd == '' THEN
                CALL PromptForPMName
            ELSE DO
                ok = ImportPPD('ECUPS', globals.!prt_ppd )
                IF ok <> 0 THEN DO
                    CALL LINEOUT globals.!log1, 'PPD import failed:' ok
                    RETURN 1                /** RC=1  PPD import failed **/
                END
            END
        END

        ok = CreateOS2Printer('ECUPS')
        IF ok <> 0 THEN
            RETURN 3                    /** RC=3  Error running rinstprn **/
    END

RETURN 0

/*:VRX         DDCB_PROTOCOL_Change
*/
DDCB_PROTOCOL_Change: PROCEDURE

    which = VRGet('DDCB_PROTOCOL', 'Selected')
    SELECT
        WHEN which == 1 THEN DO     /* IPP */
            show_queue = 1
            show_user  = 1
            show_pass  = 1
        END
        WHEN which == 2 THEN DO     /* SOCKET */
            show_queue = 0
            show_user  = 0
            show_pass  = 0
        END
        WHEN which == 3 THEN DO     /* LPD */
            show_queue = 1
            show_user  = 1
            show_pass  = 0
        END
        WHEN which == 4 THEN DO     /* SMB */
            show_queue = 1
            show_user  = 1
            show_pass  = 1
        END
        OTHERWISE DO                /* CUPS */
            show_queue = 1
            show_user  = 0
            show_pass  = 0
        END
    END

    CALL VRSet 'DT_QUEUE',    'Visible', show_queue
    CALL VRSet 'EF_QUEUE',    'Visible', show_queue
    CALL VRSet 'DT_USERID',   'Visible', show_user
    CALL VRSet 'EF_USERID',   'Visible', show_user
    CALL VRSet 'DT_PASSWORD', 'Visible', show_pass
    CALL VRSet 'EF_PASSWORD', 'Visible', show_pass

RETURN

/*:VRX         ExecRINSTPRN
*/
ExecRINSTPRN: PROCEDURE EXPOSE globals.
    PARSE ARG dsc, drv, src, rsp

    od = DIRECTORY()
    CALL DIRECTORY src
    rinstprn_cmd = 'rinstprn /DSC:'dsc '/DRV:'drv' /S:'src                    ,
                   '/T:'globals.!bootdrv '/L1:'globals.!logdir'\rinstprn.log' ,
                   '/R:'rsp

    CALL LINEOUT globals.!log1, 'Creating OS/2 printer using:'
    CALL LINEOUT globals.!log1, '   ' rinstprn_cmd
    ADDRESS CMD '@' rinstprn_cmd '1>NUL 2>NUL'
    CALL LINEOUT globals.!log1, 'Return code: 0x' || D2X( rc, 4 )
    IF rc <> 0 THEN
        CALL LINEOUT globals.!log1, 'See' globals.!logdir'\rinstprn.log for details.'
    CALL DIRECTORY od
    CALL LINEOUT globals.!log1, ''

RETURN rc

/*:VRX         Fini
*/
Fini:
    window = VRWindow()
    call VRSet window, "Visible", 0
    drop window
return 0

/*:VRX         GetCupsPorts
*/
GetCupsPorts: PROCEDURE EXPOSE globals.

    CALL VRSet 'DT_INFO', 'Caption', 'Looking for connected printers.  Please wait...'
    CALL VRMethod 'LB_SELECT', 'Clear'

    lpinfo_cmd = globals.!cupsdir'\sbin\lpinfo.exe -v |rxqueue'

    _od = DIRECTORY()
    CALL DIRECTORY globals.!cupsdir
    listqueue = RXQUEUE('CREATE')
    defqueue  = RXQUEUE('SET', listqueue )

    ADDRESS CMD '@' lpinfo_cmd listqueue
    i = 0
    DO QUEUED()
        PARSE PULL line
        PARSE VAR line _type _name
        IF _name == '' THEN ITERATE
        IF _type == 'network' & POS('://', _name ) > 0 THEN DO
            PARSE VAR _name _protocol '://' _host '/' _queue
            IF _protocol == 'socket' THEN _protocol = 'AppSocket'
            ELSE _protocol = TRANSLATE( _protocol )
            i = i + 1
            IF _queue == '' THEN _desc = '('_protocol')'
            ELSE _desc = _queue '('_protocol')'
            devices.i = 'Detected network printer' _desc 'at' _host 
        END
        ELSE IF _type <> 'direct' THEN ITERATE
        ELSE DO
            i = i + 1
            devices.i = 'Local printer:' _name
        END
        ports.i = _name
    END
    devices.0 = i

    CALL RXQUEUE 'SET',    defqueue
    CALL RXQUEUE 'DELETE', listqueue
    CALL DIRECTORY _od

    CALL VRSet    'LB_SELECT', 'Painting', 0
    CALL VRMethod 'LB_SELECT', 'AddStringList', 'devices.',, 'ports.'
    CALL VRMethod 'LB_SELECT', 'AddString', 'Network printer (manual configuration)',, ''
    CALL VRSet    'LB_SELECT', 'Selected', 1
    CALL VRSet    'LB_SELECT', 'Painting', 1

    CALL VRSet 'DT_INFO', 'Caption', 'Select the connection for this printer.'

RETURN

/*:VRX         GetCupsPrinters
*/
GetCupsPrinters: PROCEDURE EXPOSE globals.

    CALL VRSet 'DT_INFO', 'Caption', 'Getting list of supported printers.  Please wait...'

    lpinfo_cmd = globals.!cupsdir'\sbin\lpinfo.exe --exclude-schemes drv -m |rxqueue'

    _od = DIRECTORY()
    CALL DIRECTORY globals.!cupsdir
    listqueue = RXQUEUE('CREATE')
    defqueue  = RXQUEUE('SET', listqueue )

    ADDRESS CMD '@' lpinfo_cmd listqueue
    i = 0
    DO QUEUED()
        PARSE PULL line
        IF LEFT( line, 6 ) == 'drv://' THEN ITERATE
        PARSE VAR line _ppd '.ppd.gz ' _name
        _ppd = _ppd'.ppd.gz'
        IF _name == '' THEN DO
            PARSE VAR line _ppd '.ppd ' _name
            _ppd = _ppd'.ppd'
        END
        IF _name == '' THEN DO
            PARSE VAR line _ppd _name
        END
        IF _name <> '' THEN DO
            i = i + 1
            ppds.i   = _ppd
            models.i = _name
        END
    END
    ppds.0   = i
    models.0 = i

    CALL RXQUEUE 'SET',    defqueue
    CALL RXQUEUE 'DELETE', listqueue
    CALL DIRECTORY _od

    CALL VRSet    'LB_SELECT', 'Painting', 0
    CALL VRMethod 'LB_SELECT', 'AddString', '-- Other printer (requires PPD) --',, ''
    CALL VRMethod 'LB_SELECT', 'AddStringList', 'models.',, 'ppds.'
    CALL VRSet    'LB_SELECT', 'Selected', 1
    CALL VRSet    'LB_SELECT', 'Painting', 1

    CALL VRSet 'DT_INFO', 'Caption', 'Select your printer model.'

RETURN

/*:VRX         Halt
*/
Halt:
    signal _VREHalt
return

/*:VRX         ImportPPD
*/
ImportPPD: PROCEDURE EXPOSE globals.
    /* Import a new PPD file into a PostScript PrinterPak driver.
     */
    ARG driver, ppdfile
    IF driver == '' THEN driver = 'PSCRIPT'

    CALL LINEOUT globals.!log1, 'Going to import PPD file into driver' driver'.'

    /***
     *** First, get our working directories and locate the driver source files.
     ***/

    mustcopy = 0
    driver_path = GetDriverSource( driver )
    IF driver_path == '' THEN DO
        /* No source found.  We'll have to try copying the actual installed
         * driver files from under \OS2\DLL.
         */
        CALL LINEOUT globals.!log1, 'Driver source not found.'
        driver_path = STREAM( globals.!os2dir'\DLL\'driver'\'driver'.DRV', 'C', 'QUERY EXISTS')
        mustcopy = 1
    END
    IF driver_path <> '' THEN DO
        srcdir = VRParseFilePath( driver_path, 'DP')
        pin    = STREAM( srcdir'\PIN.EXE',    'C', 'QUERY EXISTS')
        ppdenc = STREAM( srcdir'\PPDENC.EXE', 'C', 'QUERY EXISTS')
        /* TODO should we check for all the REQUIREDDRIVER FILES as well? */
        DROP srcdir
    END

    /* Driver (or one of its required files) was not found.
     */
    IF ( driver_path == '') | ( pin == '') | ( ppdenc == '') | ,
       ( VerifyDriverEAs( driver_path ) == 0 ) THEN
    DO
        CALL LINEOUT globals.!log1, ' - Missing required driver files.'
        /* TODO should prompt for installable driver package? */
        RETURN 1                        /** RC=1  Missing required driver files **/
    END

    IF mustcopy <> 0 THEN DO
        /* Looks like the driver wasn't shipped with the OS; we have a source
         * (either the active files from \OS2\DLL, or a package provided by
         * the user), but we need somewhere for them to live.  The user needs
         * to tell us where, because they're going to have to specify the
         * directory later on if they install the driver from PM.
         */
        pbtn.1 = 'OK'
        pbtn.2 = 'Cancel'
        pbtn.0 = 2
        ptext = 'The install files for the' driver 'driver could not be '  ||,
                'located.  These files will be recreated from the driver ' ||,
                'files which are already installed on your system.'        ||,
                '0d0a0d0a'x || 'Please enter the directory where the '     ||,
                'install files will be placed.'
        PARSE VALUE VRGetIni('PM_INSTALL', driver'_DIR', 'USER') WITH drvr_dir '00'x .
        IF drvr_dir == '' THEN
            PARSE VALUE VRGetIni('InstPDR', 'PATH_TO_'driver, 'USER') WITH drvr_dir '00'x .
        ok = VRPrompt('WN_MAIN', ptext, 'drvr_dir',,
                      'Enter Directory', 'pbtn.', 1, 2 )
        IF ok <> 1 THEN RETURN 9        /** RC=9  User cancelled **/
        drvr_dir = VRExpandFileName( drvr_dir )
        DO WHILE drvr_dir == ''
            ok = VRPrompt('WN_MAIN', ptext, 'drvr_dir',,
                          'Enter Directory', 'pbtn.', 1, 2 )
            drvr_dir = VRExpandFileName( drvr_dir )
        END

        /* Now create the new directory, if necessary */
        IF VRMkDir( drvr_dir ) == 0 THEN
            RETURN 3                    /** RC=3  Failed to copy driver files **/
        IF CopyDriverToSource( driver_path, drvr_dir ) == 0 THEN
            RETURN 3                    /** RC=3  Failed to copy driver files **/

        /* Make sure we save the location (we'll need this information later) */
        CALL VRSetIni 'PM_INSTALL', driver'_DIR', drvr_dir||'00'x, 'USER'

        /* Now point to the new copy as the driver we will work on */
        driver_path = drvr_dir'\'driver'.DRV'
    END

    workdir = SysTempFileName( globals.!tmpdir'\PPD_????')
    ok = VrMkDir( workdir )
    IF ok == 1 THEN ok = VrMkDir( workdir'\OUT')
    IF ok <> 1 THEN
        RETURN 2                        /** RC=2  Failed to create temporary directory **/

    SELECT
        WHEN driver == 'ECUPS'    THEN ppddir = globals.!repository'\PPD_E'
        WHEN driver == 'ECUPS-HP' THEN ppddir = globals.!repository'\PPD_EHP'
        WHEN driver == 'PSPRINT'  THEN ppddir = globals.!repository'\PPD_PS'
        WHEN driver == 'PSPRINT2' THEN ppddir = globals.!repository'\PPD_PS2'
        WHEN driver == 'PSCRIPT2' THEN ppddir = globals.!repository'\PPD2'
        OTHERWISE                      ppddir = globals.!repository'\PPD'
    END

    /* Make sure ppddir (for keeping PPD files) exists */
    CALL SysFileTree ppddir, 'dirs.', 'DO'
    IF dirs.0 == 0 THEN DO
        ok = VrMkDir( ppddir )
        IF ok <> 1 THEN
            RETURN 4                    /** RC=4  Failed to create PPD directory **/
    END

    /***
     *** Now do the actual work.
     ***/

    /* Back up the modified files (AUXPRINT.PAK and <driver>.DRV) if we're
     * working out of the repository.
    IF mustcopy == 0 THEN DO
        repfiles.0 = 2
        repfiles.1 = driver'.DRV'
        repfiles.2 = 'AUXPRINT.PAK'
        CALL BackupDrivers driver_path
    END
     */

    /* Copy the needed driver files to our working directories.
     */
    drv_out = workdir'\OUT\'driver'.DRV'
    pin_exe = workdir'\PIN.EXE'
    ppd_exe = workdir'\PPDENC.EXE'
    ok = VRCopyFile( driver_path, drv_out )
    IF ok == 1 THEN ok = VRCopyFile( pin, pin_exe )
    IF ok == 1 THEN ok = VRCopyFile( ppdenc, ppd_exe )
    IF ok == 0 THEN DO
        RETURN 3                        /*** RC=3  Failed to copy driver files ***/
    END

    /* Set up the output redirection.
     */
    nq = RXQUEUE('CREATE')
    oq = RXQUEUE('SET', nq )

    /* If the PPD file is compressed, uncompress it.
     */
    IF VRParseFilePath( ppdfile, 'E') == 'GZ' THEN DO
        decppd = workdir'\' || VRParseFilePath( ppdfile, 'N')
        CALL LINEOUT globals.!log1, 'Decompressing' ppdfile 'to' decppd
        ADDRESS CMD '@gzip -c -d' ppdfile '| RXQUEUE' nq
        DO QUEUED()
            PARSE PULL line
            CALL LINEOUT decppd, line
        END
        CALL LINEOUT decppd
        ppdfile = decppd
    END

    IF VRFileExists( ppdfile ) == 0 THEN DO
        CALL LINEOUT globals.!log1, 'PPD file' ppdfile 'could not be found.'
        RETURN 5                        /** RC=5  PPD import failed **/
    END

    ppd_use = ppddir'\' || VRParseFileName( ppdfile, 'NE')

    /* Now we have to clean up and validate the PPD file so PIN can use it.
     * First, PPDENC converts the codepage if necessary, and copies the results
     * to our working directory.
     */
    ADDRESS CMD '@'ppd_exe ppdfile ppd_use '2>&1 | RXQUEUE' nq
    DO QUEUED()
        PULL output
        CALL LINEOUT globals.!log2, output
    END
    CALL LINEOUT globals.!log2, ''
    CALL LINEOUT globals.!log2

    /* Next we strip out some problematic lines used which are often encountered
     * in (e.g.) CUPS-based PPD files.
     */
    CALL CleanPPD ppd_use, globals.!log1

    /* Preparation complete.  Now do the import.
     */
    count = 0
    ADDRESS CMD '@'pin_exe 'ppd' ppddir drv_out '2>&1 | RXQUEUE' nq
    DO QUEUED()
        PARSE PULL output
        CALL LINEOUT globals.!log2, output
        PARSE VAR output . 'OK (' nickname
        IF nickname <> '' THEN DO
            count = count + 1
            newprinters.count = STRIP( nickname, 'T', ')')
        END
    END
    newprinters.0 = count
    CALL LINEOUT globals.!log2, ''
    CALL LINEOUT globals.!log2

    /* End the output redirection.
     */
    CALL RXQUEUE 'SET',    oq
    CALL RXQUEUE 'DELETE', nq


    /***
     *** Post-import processing.
     ***/

    IF newprinters.0 == 0 THEN DO
        RETURN 5                        /** RC=5  PPD import failed **/
    END

    IF mustcopy == 0 THEN DO
        /* If we're working out of the repository, we need to update the
         * driver table in PRDESC.LST to add the new driver(s).
         */
        CALL LINEOUT globals.!log1, 'Updating' globals.!prdesc 'with' newprinters.0 'new entries ...'
        count = 0
        match_drv = '('driver'.DRV)'
        match_len = LENGTH( match_drv )

        /* First, copy all lines that don't refer to the driver just updated */
        DO WHILE LINES( globals.!prdesc )
            _line = LINEIN( globals.!prdesc )
            IF TRANSLATE( RIGHT( _line, LENGTH( match_len ))) == match_drv THEN ITERATE
            count = count + 1
            defs.count = _line
        END
        CALL STREAM globals.!prdesc, 'C', 'CLOSE'

        /* Next, create a new list for the updated driver and merge that in */
        newlist = workdir'\'driver'.LST'
        CALL CreateDriverList driver, newlist
        DO WHILE LINES( newlist )
            _line = LINEIN( newlist )
            count = count + 1
            defs.count = _line
        END
        defs.0 = count

        /* Now sort the list and recreate PRDESC.LST */
        CALL SysStemSort 'defs.',, 'I'
        prdesc_tmp = workdir'\PRDESC.LST'
        IF STREAM( prdesc_tmp, 'C', 'QUERY EXISTS') <> '' THEN
            CALL VRDeleteFile prdesc_tmp
        DO i = 1 TO defs.0
            CALL LINEOUT prdesc_tmp, defs.i
        END
        CALL LINEOUT prdesc_tmp
        ok = VRCopyFile( prdesc_tmp, globals.!prdesc )
        IF ok == 0 THEN DO
            RETURN 6                    /** RC=6  Error updating PRDESC.LST **/
        END
        CALL VRDeleteFile prdesc_tmp

    END

    /* Finally, copy the updated driver files.
     */
    target = VRParseFilePath( driver_path, 'DP')
    ok = VRCopyFile( workdir'\OUT\*', target )
    IF ok == 1 THEN DO
        /* Copy the updated files to \OS2\DLL\<driver>, replacing any
         * existing copies.  (This prevents problems if the OS/2 driver
         * installation doesn't/fails to copy them, which can happen under
         * some circumstances.)
         */
        IF VRFileExists( globals.!os2dir'\DLL\'driver ) THEN DO
            CALL VRCopyFile workdir'\OUT\AUXPRINT.PAK',,
                            globals.!os2dir'\DLL\'driver'\AUXPRINT.PAK'
            CALL VRCopyFile workdir'\OUT\'driver'.DRV',,
                            globals.!os2dir'\DLL\'driver'\'driver'.DRV'
        END
    END
    IF ok == 0 THEN
        RETURN 3                        /*** RC=3  Failed to copy driver files ***/

    CALL LINEOUT globals.!log1, newprinters.0 'printers imported successfully.'
    DO i = 1 TO newprinters.0
        CALL LINEOUT globals.!log1, ' ->' newprinters.i
    END
    CALL LINEOUT globals.!log1, ''
    CALL LINEOUT globals.!log1

    /* Clean up our work directories.
     */
    CALL VRDeleteFile workdir'\OUT\*'
    CALL VRDeleteFile workdir'\*'
    CALL VRRmDir( workdir'\OUT')
    CALL VRRmDir( workdir )

RETURN 0

/*:VRX         Init
*/
Init:
    SIGNAL ON NOVALUE NAME __NoValue

    CALL RxFuncAdd 'SysLoadFuncs', 'REXXUTIL', 'SysLoadFuncs'
    CALL SysLoadFuncs

    CALL LoadSettings

    IF InitArgs.0 > 0 THEN globals.!cupsdir = InitArgs.1

    IF \VRIsDir( globals.!cupsdir ) THEN DO
        CALL VRMessage VRWindow(), 'Please specify the path where CUPS is installed as follows:' ||,
                                   '0d0a0d0a'x || 'CUPSWIZ <cups-path>',,
                                   'Unknown or Invalid CUPS Path', 'E'
        EXIT
    END

    /* Delete the error log file each time the program starts.
     */
    IF VRFileExists( globals.!log2 ) THEN
        CALL VRDeleteFile globals.!log2

    window = VRWindow()
    call VRMethod window, "CenterWindow"
    call VRSet window, "Visible", 1
    call VRMethod window, "Activate"
    drop window

    CALL SetPage1

RETURN

/*:VRX         InitMessageLog
*/
/* Initialize the message logfile.  Unlike the error log, which is cleared and
 * recreated every time the program starts, the message log is appended to
 * until it reaches 100kB in size.  If the message log is larger than 100kB when
 * this function is called, it is renamed (with '~' appended to the  name) and
 * a new message log is started.
 */
InitMessageLog: PROCEDURE EXPOSE globals.

    logsize = STREAM( globals.!log1, 'C', 'QUERY SIZE')
    IF ( logsize <> '') & ( logsize > 102400 ) THEN DO
        CALL VRCopyFile globals.!log1, globals.!log1 || '~'
        CALL VRDeleteFile globals.!log1
    END

    datestr = DATE('L') TIME('N')
    CALL LINEOUT globals.!log1, '--[' datestr ']' ||,
                 COPIES('-', 73 - LENGTH( datestr ))
RETURN

/*:VRX         LoadSettings
*/
LoadSettings: PROCEDURE EXPOSE globals.

    me = VRGet('Application', 'Program')
    IF me == '' THEN PARSE SOURCE . . me
    globals.!mydir = VRParseFilePath( me, 'DP')

    /* Get CUPS paths.
     */
    globals.!cupsdir = ''
    cupsdrv = VRParseFilePath( me, 'D')
    cupsdir = cupsdrv':\cups'

    globals.!cupsdir = cupsdir

    /* Get system paths.
     */
    globals.!bootdrv = SysBootDrive()
    IF globals.!bootdrv == '' THEN
        globals.!bootdrv = FILESPEC('DRIVE', VALUE('OS2_SHELL',,'OS2ENVIRONMENT'))
    globals.!os2dir = globals.!bootdrv'\OS2'
    globals.!tmpdir = VALUE('TMP',,'OS2ENVIRONMENT')
    IF globals.!tmpdir == '' THEN
        globals.!tmpdir = VALUE('TEMP',,'OS2ENVIRONMENT')
    IF globals.!tmpdir == '' THEN
        globals.!tmpdir = globals.!mydir
    globals.!logdir = VALUE('LOGFILES',,'OS2ENVIRONMENT')
    IF globals.!logdir == '' THEN
        globals.!logdir = globals.!mydir

    globals.!log1 = globals.!logdir'\cupswiz.l1'
    globals.!log2 = globals.!logdir'\cupswiz.l2'

    /* Get printer-related paths.
     */
    PARSE VALUE VRGetIni('PM_INSTALL', 'PDR_DIR', 'USER') WITH repos_dir '00'x .
    globals.!repository = repos_dir
    globals.!prdrv      = STREAM( globals.!os2dir'\install\prdrv.lst',  'C', 'QUERY EXISTS')
    globals.!prdesc     = STREAM( globals.!os2dir'\install\prdesc.lst', 'C', 'QUERY EXISTS')

RETURN 0

/*:VRX         LoopbackName
*/
/* Check to see if 'localhost' is defined in the HOSTS file.  If not, we'll
 * have to use '127.0.0.1' instead.
 */
LoopbackName: PROCEDURE

    lo_name = '127.0.0.1'
    etcdir = VALUE('ETC',,'OS2ENVIRONMENT')
    IF etcdir <> '' THEN DO
        hosts = STREAM( etcdir'\HOSTS', 'C', 'QUERY EXISTS')
        IF hosts <> '' THEN DO
            CALL LINEIN hosts, 1, 0
            DO WHILE LINES( hosts ) > 0
                _hostdef = TRANSLATE( LINEIN( hosts ))
                _hostdef = TRANSLATE( _hostdef, ' ', '09'x )
                IF WORDPOS('LOCALHOST', _hostdef ) == 2 THEN DO
                    lo_name = 'localhost'
                    LEAVE
                END
            END
        END
    END

RETURN lo_name

/*:VRX         PB_CANCEL_Click
*/
PB_CANCEL_Click:
    CALL Quit
return

/*:VRX         PB_CREATECANCEL_Click
*/
PB_CREATECANCEL_Click:
    CALL SW_CREATE_Close
RETURN

/*:VRX         PB_CREATEOK_Click
*/
PB_CREATEOK_Click:

    create_os2 = VRGet('CHK_CREATEPM', 'Set')
    globals.!os2printer = create_os2
    globals.!create = 1

    CALL SW_CREATE_Close

RETURN

/*:VRX         PB_MODELCANCEL_Click
*/
PB_MODELCANCEL_Click:
    CALL SW_MODEL_Close
    globals.!prt_nick = 'Generic PostScript Printer'
RETURN

/*:VRX         PB_MODELOK_Click
*/
PB_MODELOK_Click:
    globals.!prt_nick = VRGet( "LB_OS2MODELS", "SelectedString" )
    CALL SW_MODEL_Close
RETURN

/*:VRX         PB_NETCANCEL_Click
*/
PB_NETCANCEL_Click:
    CALL SW_NETWORK_Fini
RETURN

/*:VRX         PB_NETOK_Click
*/
PB_NETOK_Click: PROCEDURE EXPOSE globals. port
    which  = VRGet('DDCB_PROTOCOL', 'Selected')
    server = STRIP( VRGet("EF_SERVER",   "Value"))
    pqueue = STRIP( VRGet("EF_QUEUE",    "Value"))
    userid = STRIP( VRGet("EF_USERID",   "Value"))
    passwd = STRIP( VRGet("EF_PASSWORD", "Value"))

    invalid = 0

    SELECT
        WHEN which == 1 THEN DO     /* IPP */
            IF server == '' | pqueue == '' THEN invalid = 1
            uri = 'ipp://'
            IF userid <> '' THEN DO
                uri = uri || userid
                IF passwd <> '' THEN uri = uri':'passwd
                uri = uri'@'
            END
            uri = uri || server'/'pqueue
        END
        WHEN which == 2 THEN DO     /* SOCKET */
            IF server == '' THEN invalid = 1
            uri = 'socket://'server
        END
        WHEN which == 3 THEN DO     /* LPD */
            IF server == '' THEN invalid = 1
            IF pqueue  == '' THEN pqueue = '*'
            IF userid <> '' THEN
                uri = 'ipp://'userid'@'server'/'pqueue
            ELSE
                uri = 'lpd://'server'/'pqueue
        END
        WHEN which == 4 THEN DO     /* SMB */
            IF server == '' | pqueue == '' THEN invalid = 1
            uri = 'smb://'
            IF userid <> '' THEN DO
                uri = uri || userid
                IF passwd <> '' THEN uri = uri':'passwd
                uri = uri'@'
            END
            uri = uri || server'/'pqueue
        END
        OTHERWISE DO
            IF server == '' | pqueue == '' THEN invalid = 1
            ELSE DO
                globals.!remotecups = server pqueue
                uri = ''
            END
        END
    END

    IF invalid == 1 THEN DO
        CALL VRMessage VRWindow(), 'Missing required value(s).', 'Missing Value(s)', 'E'
        RETURN
    END

    port = uri
    CALL SW_NETWORK_Fini

RETURN

/*:VRX         PB_NEXT_Click
*/
PB_NEXT_Click: PROCEDURE EXPOSE globals.

    SELECT
        WHEN globals.!page == 1 THEN DO
            selected = VRGet('LB_SELECT', 'Selected')
            IF selected == 0 THEN RETURN

            ppd = VRMethod('LB_SELECT', 'GetItemData', selected )
            IF ppd == '' THEN DO
                globals.!mode = 2   /* Mode 2: user-selected PPD file */
                ppd = VRFileDialog( VRWindow(), 'Select PPD', 'O', '*.ppd')
                IF ppd == '' THEN RETURN
                globals.!prt_ppd  = ppd
                globals.!prt_dev  = ''
                globals.!prt_nick = GetNameFromPPD( ppd )
            END
            ELSE DO
                globals.!mode = 1   /* Mode 1: CUPS-included model */
                IF POS('exe://', ppd ) > 0 THEN DO
                    globals.!prt_ppd  = ''
                    globals.!prt_dev  = ppd
                    sel_name = VRMethod('LB_SELECT', 'GetString', selected )
                    PARSE VAR sel_name _nick ' - CUPS' .
                    IF _nick == '' THEN
                        globals.!prt_nick = STRIP( sel_name )
                    ELSE
                        globals.!prt_nick = STRIP( _nick )
                END
                ELSE DO
                    globals.!prt_ppd  = TRANSLATE( globals.!cupsdir'/share/cups/model/'ppd, '\', '/')
                    globals.!prt_dev  = ''
                    globals.!prt_nick = GetNameFromPPD( ppd )
                END
            END
            globals.!remotecups = ''

            CALL InitMessageLog
            IF globals.!mode ==2 THEN
                CALL LINEOUT globals.!log1, 'Starting printer install with user-provided PPD:'
            ELSE
                CALL LINEOUT globals.!log1, 'Starting printer install for built-in model:'
            IF globals.!prt_dev <> '' THEN
                CALL LINEOUT globals.!log1, ' - Device name:' globals.!prt_dev
            ELSE
                CALL LINEOUT globals.!log1, ' - PPD file:   ' globals.!prt_ppd
            CALL LINEOUT globals.!log1, ' - Model name: ' globals.!prt_nick
            CALL LINEOUT globals.!log1, ''

            CALL SetPage2
        END

        WHEN globals.!page == 2 THEN DO
            selected = VRGet('LB_SELECT', 'Selected')
            IF selected == 0 THEN RETURN
            port = VRMethod('LB_SELECT', 'GetItemData', selected )

            IF port == '' THEN DO
                /* Network printer selected; prompt for the connection details
                 */
                CALL VRLoadSecondary 'SW_NETWORK', 'W'
            END

            IF port == '' & globals.!remotecups == '' THEN RETURN
            globals.!prt_port = port

            CALL SetPage3
        END

        WHEN globals.!page == 3 THEN DO
            globals.!prt_name = STRIP( VRGet('EF_NAME',     'Value'))
            globals.!prt_loc  = STRIP( VRGet('EF_LOCATION', 'Value'))
            globals.!prt_info = STRIP( VRGet('EF_DESC',     'Value'))
            IF ((( globals.!prt_name == '') |,
                 ( globals.!prt_loc  == '')) & ( globals.!remotecups == '')) |,
                ( globals.!prt_info == '') THEN
            DO
                IF ( globals.!remotecups <> '') THEN
                    _errmsg = 'You must enter a description.'
                ELSE
                    _errmsg = 'You must enter a name, a location, and a description.'
                CALL VRMessage VRWindow(), _errmsg, 'Missing Value(s)', 'E'
                RETURN
            END
            IF ( globals.!remotecups == '' &,
                ( POS( LEFT( globals.!prt_name, 1 ),,
                      'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz') == 0 ) |,
                ( VERIFY( globals.!prt_name, '/# ' || '09'x, 'MATCH') <> 0 )) THEN
            DO
                CALL VRMessage VRWindow(),  'The printer name must start '     ||,
                               'with a letter, and may not include "/", "#", ' ||,
                               'space, or tab characters.', 'Invalid Name', 'E'
                RETURN
            END

            /* TODO support other drivers
             */
            globals.!os2driver = 'ECUPS'

            CALL ConfirmAndCreate
        END

        WHEN globals.!page == 4 THEN DO
            CALL VRSet 'PB_NEXT',     'Caption', 'Next >'
            CALL VRSet 'EF_NAME',     'Value', ''
            CALL VRSet 'EF_LOCATION', 'Value', ''
            CALL VRSet 'EF_DESC',     'Value', ''
            CALL SetPage1
        END

        OTHERWISE NOP
    END

RETURN

/*:VRX         PromptForPMName
*/
PromptForPMName: PROCEDURE EXPOSE globals. models. best
    best = MatchPrinterModel( globals.!os2driver, globals.!prt_nick )

    IF models.0 == 0 THEN DO
        /* No similar models were found in the list of supported printers.
         * Just use the generic PS driver.
         */
        CALL VRMessage VRWindow(),,
                       'The printer "'globals.!prt_nick'" does not appear to' ,
                       'be supported by the' globals.!os2driver 'driver, and' ,
                       'no similar models were found.  The generic PostScript',
                       'driver will be used for application support.',,
                       'Printer Name Not Found', 'E'
        globals.!prt_nick = 'Generic PostScript Printer'
        RETURN
    END

    CALL VRLoadSecondary 'SW_MODEL', 'W'

RETURN

/*:VRX         Quit
*/
Quit:
    window = VRWindow()
    call VRSet window, "Shutdown", 1
    drop window
return

/*:VRX         SetPage1
*/
SetPage1: PROCEDURE EXPOSE globals.

    CALL VRSet 'LB_SELECT',   'Visible', 1
    CALL VRSet 'GB_INFO',     'Visible', 0

    globals.!page = 1
    CALL VRSet 'WN_MAIN', 'Pointer', 'WAIT'
    CALL GetCupsPrinters
    CALL VRMethod 'LB_SELECT', 'SetFocus'
    CALL VRSet 'WN_MAIN', 'Pointer', '<default>'
RETURN

/*:VRX         SetPage2
*/
SetPage2: PROCEDURE EXPOSE globals.

    globals.!page = 2
    CALL VRSet 'WN_MAIN', 'Pointer', 'WAIT'
    CALL GetCupsPorts
    CALL VRMethod 'LB_SELECT', 'SetFocus'
    CALL VRSet 'WN_MAIN', 'Pointer', '<default>'

RETURN

/*:VRX         SetPage3
*/
SetPage3: PROCEDURE EXPOSE globals.

    globals.!page = 3
    CALL VRSet 'EF_DESC', 'Value', globals.!prt_nick

    CALL VRMethod 'LB_SELECT', 'Clear'

    CALL VRSet 'LB_SELECT',   'Visible', 0
    CALL VRSet 'GB_INFO',     'Visible', 1
    IF globals.!remotecups == '' THEN DO
        CALL VRSet 'DT_INFO', 'Caption', 'Choose the printer name, and enter its location and a short description.'
        CALL VRSet 'DT_NAME',     'Visible', 1
        CALL VRSet 'EF_NAME',     'Visible', 1
        CALL VRSet 'DT_LOCATION', 'Visible', 1
        CALL VRSet 'EF_LOCATION', 'Visible', 1
    END
    ELSE
        CALL VRSet 'DT_INFO', 'Caption', 'Enter a short description of this printer. ',
                                         'This will be used for the printer object that appears on your desktop.'
    CALL VRSet 'DT_DESC',     'Visible', 1
    CALL VRSet 'EF_DESC',     'Visible', 1

    CALL VRMethod 'EF_NAME', 'SetFocus'

RETURN

/*:VRX         SetPage4
*/
SetPage4:

    globals.!page = 4
    CALL VRSet 'LB_SELECT',   'Visible', 0
    CALL VRSet 'GB_INFO',     'Visible', 0

RETURN

/*:VRX         SW_CREATE_Close
*/
SW_CREATE_Close:
    call SW_CREATE_Fini
return

/*:VRX         SW_CREATE_Create
*/
SW_CREATE_Create:
    call SW_CREATE_Init
return

/*:VRX         SW_CREATE_Fini
*/
SW_CREATE_Fini:
    window = VRInfo( "Window" )
    call VRDestroy window
    drop window
return
/*:VRX         SW_CREATE_Init
*/
SW_CREATE_Init:

    CALL VRSet 'EF_CRMODEL', 'Value', globals.!prt_nick

    IF globals.!remotecups <> '' THEN DO
        /* An existing CUPS printer was indicated.  This means we're not
         * creating a CUPS printer, but only a PM printer object (and port)
         * that points to it.
         */
        CALL VRSet 'DT_CREATE', 'Caption', 'Ready to create printer object with the following parameters.'

        PARSE VAR globals.!remotecups cups_host cups_printer .
        CALL VRSet 'DT_CRNAME',    'Caption', 'CUPS server:'
        CALL VRSet 'EF_CRNAME',    'Value',   cups_host
        CALL VRSet 'DT_CRURI',     'Caption', 'CUPS queue:'
        CALL VRSet 'EF_CRURI',     'Value',   cups_printer
        CALL VRSet 'DT_CRLOC',     'Visible', 0
        CALL VRSet 'EF_CRLOC',     'Visible', 0
        CALL VRSet 'CHK_CREATEPM', 'Visible', 0
    END
    ELSE DO
        CALL VRSet 'EF_CRNAME', 'Value', globals.!prt_name
        CALL VRSet 'EF_CRURI',  'Value', globals.!prt_port
        CALL VRSet 'EF_CRLOC',  'Value', globals.!prt_loc
    END
    CALL VRSet 'EF_CRDESC', 'Value', globals.!prt_info

    window = VRInfo( "Object" )
    if( \VRIsChildOf( window, "Notebook" ) ) then do
        call VRMethod window, "CenterWindow"
        call VRSet window, "Visible", 1
        call VRMethod window, "Activate"
    end
    drop window

RETURN

/*:VRX         SW_MODEL_Close
*/
SW_MODEL_Close:
    call SW_MODEL_Fini
return

/*:VRX         SW_MODEL_Create
*/
SW_MODEL_Create:
    call SW_MODEL_Init
return

/*:VRX         SW_MODEL_Fini
*/
SW_MODEL_Fini:

    CALL VRSet 'WN_MAIN', 'Pointer', 'WAIT'

    window = VRInfo( "Window" )
    call VRDestroy window
    drop window
return
/*:VRX         SW_MODEL_Init
*/
SW_MODEL_Init:

    /* We should have a list of suggested printer models whose names at least
     * partially match the requested model, sorted in order with the closest
     * match at the top.
     */
    ok = VRMethod( "LB_OS2MODELS", "AddStringList", "models.",  )
    IF best > 0 THEN
        CALL VRSet 'LP_OS2MODELS', 'Selected', best

    CALL VRSet 'DT_MODEL1', 'Caption',,
               'The printer "'globals.!prt_nick'" could not be found in the'  ,
               globals.!os2driver'.DRV driver under that name.  Please choose',
               'the model of printer which will be reported to applications.'

    CALL VRSet 'WN_MAIN', 'Pointer', '<default>'

    window = VRInfo( "Object" )
    if( \VRIsChildOf( window, "Notebook" ) ) then do
        call VRMethod window, "CenterWindow"
        call VRSet window, "Visible", 1
        call VRMethod window, "Activate"
    end
    drop window

RETURN

/*:VRX         SW_NETWORK_Close
*/
SW_NETWORK_Close:
    call SW_NETWORK_Fini
return

/*:VRX         SW_NETWORK_Create
*/
SW_NETWORK_Create:
    call SW_NETWORK_Init
return

/*:VRX         SW_NETWORK_Fini
*/
SW_NETWORK_Fini:
    window = VRInfo( "Window" )
    call VRDestroy window
    drop window
return
/*:VRX         SW_NETWORK_Init
*/
SW_NETWORK_Init:

    CALL VRSet 'DDCB_PROTOCOL', 'Selected', 1

    window = VRInfo( "Object" )
    if( \VRIsChildOf( window, "Notebook" ) ) then do
        call VRMethod window, "CenterWindow"
        call VRSet window, "Visible", 1
        call VRMethod window, "Activate"
    end
    drop window
return

/*:VRX         WN_MAIN_Close
*/
WN_MAIN_Close:
    call Quit
return

