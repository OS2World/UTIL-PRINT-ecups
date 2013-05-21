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

/*:VRX         __VXREXX____APPENDS__
*/
__VXREXX____APPENDS__:
/*
#append ..\..\Shared\PrManUtl.VRS
*/
return
/*:VRX         CheckPrograms
*/
CheckPrograms: PROCEDURE EXPOSE globals.

    globals.!hascups = 0

    /* See if CUPSLPR.EXE (required by CUPS.PDR) is installed.
     */
    tcpipbin = SysSearchPath('PATH', 'inetd.exe')
    IF tcpipbin <> '' THEN
        tcpipbin = VRParseFileName( tcpipbin, 'DP')
    ELSE
        tcpipbin = globals.!bootdrv'\TCPIP\BIN'
    globals.!programs.!cupslpr = STREAM( tcpipbin'\cupslpr.exe', 'C', 'QUERY EXISTS')
    IF globals.!programs.!cupslpr == '' THEN
        globals.!programs.!cupslpr = SysSearchPath('PATH', 'cupslpr.exe')
    IF globals.!programs.!cupslpr == '' THEN
        globals.!programs.!cupslpr = STREAM( globals.!mydir'\cupslpr.exe', 'C', 'QUERY EXISTS')

    /* See if CUPS.PDR is installed.
     */
    IF globals.!programs.!cupslpr <> '' THEN DO
        cups_pdr = VRGetIni('PM_PORT_DRIVER', 'CUPS', 'System')
        IF cups_pdr == '' THEN DO
            cups_pdr = STREAM( globals.!bootdrv'\OS2\DLL\CUPS.PDR', 'C', 'QUERY EXISTS')
            IF cups_pdr <> '' THEN CALL VRSetIni('PM_PORT_DRIVER', 'CUPS', cups_pdr, 'System')
        END
        IF cups_pdr <> '' THEN
            globals.!hascups = 1
    END

    /* See if GZIP.EXE is installed (required for unpacking CUPS PPDs).
     */
    globals.!programs.!gzip = SysSearchPath('PATH', 'gzip.exe')
    IF globals.!programs.!gzip == '' THEN
        globals.!programs.!gzip = SysSearchPath('PATH', 'gunzip.exe')
    IF globals.!programs.!gzip == '' THEN
        globals.!programs.!gzip = STREAM( globals.!mydir'\gzip.exe', 'C', 'QUERY EXISTS')

    /* Try and find some other CUPS programs we use.
     */
    globals.!programs.!lpadmin   = STREAM( globals.!cupsdir'\sbin\lpadmin.exe',  'C', 'QUERY EXISTS')
    globals.!programs.!lpinfo    = STREAM( globals.!cupsdir'\sbin\lpinfo.exe',   'C', 'QUERY EXISTS')
    globals.!programs.!lpoptions = STREAM( globals.!cupsdir'\bin\lpoptions.exe', 'C', 'QUERY EXISTS')
    globals.!programs.!lpstat    = STREAM( globals.!cupsdir'\bin\lpstat.exe',    'C', 'QUERY EXISTS')

RETURN

/*:VRX         CN_DEVICES_ContextMenu
*/
CN_DEVICES_ContextMenu:  PROCEDURE EXPOSE globals.

    _record = VRInfo("Record")
    IF _record <> '' THEN DO
        CALL VRMethod 'CN_DEVICES', 'SetRecordAttr', _record, 'Selected', 1
        CALL VRMethod 'MN_DRIVERS', 'Popup'
    END
    ELSE
        CALL VRMethod 'MN_VIEW', 'Popup'

RETURN

/*:VRX         CN_DEVICES_KeyPress
*/
CN_DEVICES_KeyPress: 
    IF VRGet('CN_DEVICES', 'KeyString') == '{Backtab}' THEN
        CALL VRMethod 'TDL_MAIN', 'SetFocus'
return

/*:VRX         CN_PORTS_ContextMenu
*/
CN_PORTS_ContextMenu: PROCEDURE EXPOSE globals.

    _record = VRInfo("Record")
    IF _record <> '' THEN DO
        CALL VRMethod 'CN_PORTS', 'SetRecordAttr', _record, 'Selected', 1
        CALL VRMethod 'MN_PORT', 'Popup'
    END
    ELSE DO
        CALL VRMethod 'MN_VIEW', 'Popup'
    END

RETURN

/*:VRX         CN_PORTS_KeyPress
*/
CN_PORTS_KeyPress: 
    IF VRGet('CN_PORTS', 'KeyString') == '{Backtab}' THEN
        CALL VRMethod 'TDL_MAIN', 'SetFocus'
return

/*:VRX         CN_PRINTERPAKS_ContextMenu
*/
CN_PRINTERPAKS_ContextMenu: PROCEDURE EXPOSE globals.
    _record = VRInfo("Record")
    IF _record <> '' THEN DO
        CALL VRMethod 'CN_PRINTERPAKS', 'SetRecordAttr', _record, 'Selected', 1
        CALL VRMethod 'MN_PRINTERPAK', 'Popup'
    END
RETURN

/*:VRX         CN_PRINTERS_ContextMenu
*/
CN_PRINTERS_ContextMenu: PROCEDURE EXPOSE globals.

    _record = VRInfo("Record")
    IF _record <> '' THEN DO
        CALL VRMethod 'CN_PRINTERS', 'SetRecordAttr', _record, 'Selected', 1
        _flags = VRMethod('CN_PRINTERS', 'GetFieldData', _record, globals.!hcfPrinters.!flags )
        IF POS('D', _flags ) > 0 THEN
            CALL VRSet 'MI_PRINTER_DEFAULT', 'Checked', 1
        ELSE
            CALL VRSet 'MI_PRINTER_DEFAULT', 'Checked', 0
        IF POS('P', _flags ) > 0 THEN
            CALL VRSet 'MI_PRINTER_HOLD', 'Checked', 1
        ELSE
            CALL VRSet 'MI_PRINTER_HOLD', 'Checked', 0
        CALL VRMethod 'MN_PRINTER', 'Popup'
    END
    ELSE
        CALL VRMethod 'MN_VIEW', 'Popup'

RETURN

/*:VRX         CN_PRINTERS_DoubleClick
*/
CN_PRINTERS_DoubleClick:
    CALL PrinterOpen VRInfo("Record")
RETURN

/*:VRX         CN_PRINTERS_KeyPress
*/
CN_PRINTERS_KeyPress: 
    IF VRGet('CN_PRINTERS', 'KeyString') == '{Backtab}' THEN
        CALL VRMethod 'TDL_MAIN', 'SetFocus'
RETURN

/*:VRX         CupsDeletePrinter
*/
CupsDeletePrinter: PROCEDURE EXPOSE globals.
    PARSE ARG _host, _dest

    IF TRANSLATE( _host ) <> 'LOCALHOST' THEN
        del_cmd = globals.!programs.!lpadmin '-h' _host '-x' _dest
    ELSE
        del_cmd = globals.!programs.!lpadmin '-x' _dest
    CALL LINEOUT globals.!log1, 'Delete CUPS destination' _dest' with:'  del_cmd

/*
    nq = RXQUEUE('CREATE')
    oq = RXQUEUE('SET', nq )
    ADDRESS CMD '@' del_cmd '2>&1 | RXQUEUE' nq
    DO QUEUED()
        PARSE PULL _output
        CALL LINEOUT globals.!log2, _output
        IF LEFT( _output, 12 ) == 'Password for' & RIGHT( STRIP( _output ), 1 ) == '?' THEN DO
            CALL SysSleep 1
            PUSH '0d0a'x
        END
    END
    CALL RXQUEUE 'SET',    oq
    CALL RXQUEUE 'DELETE', nq
*/

    _od = DIRECTORY()
    CALL DIRECTORY VRParseFileName( globals.!programs.!lpadmin, 'DP')
    ADDRESS CMD '@' del_cmd '2>&1 >>' globals.!log2
    CALL DIRECTORY _od

    CALL LINEOUT globals.!log2, ''
    CALL LINEOUT globals.!log2
    CALL LINEOUT globals.!log1, ''

RETURN rc

/*:VRX         DDCB_DEVICE_3RDPARTY_Click
*/
DDCB_DEVICE_3RDPARTY_Click: PROCEDURE EXPOSE globals. shipped.
    new_path = TRANSLATE( STRIP( VRGet('DDCB_DEVICE_3RDPARTY', 'SelectedString')))
    IF new_path == '' THEN RETURN

    old_path = VRGet('DDCB_DEVICE_3RDPARTY', 'UserData')
    IF old_path == new_path THEN RETURN

    CALL SetDriverList new_path
RETURN

/*:VRX         DDCB_DEVICE_3RDPARTY_Verify
*/
DDCB_DEVICE_3RDPARTY_Verify: PROCEDURE EXPOSE globals. shipped.
    old_path = VRGet('DDCB_DEVICE_3RDPARTY', 'UserData')
    new_path = TRANSLATE( STRIP( VRGet('DDCB_DEVICE_3RDPARTY', 'Value')))
    IF new_path == '' THEN DO
        IF old_path <> '' THEN CALL VRSet 'DDCB_DEVICE_3RDPARTY', 'Value', old_path
        RETURN
    END

    new_path = VRExpandFileName( new_path )
    IF new_path == '' THEN DO
        IF old_path <> '' THEN CALL VRSet 'DDCB_DEVICE_3RDPARTY', 'Value', old_path
        RETURN
    END

    IF old_path == new_path THEN RETURN

    CALL SetDriverList new_path
RETURN

/*:VRX         DriverAdd
*/
DriverAdd: PROCEDURE EXPOSE globals.
    updated = 0
    CALL VRLoadSecondary 'SW_DEVICE_ADD', 'W'
    IF updated == 1 THEN
        CALL RefreshDevices
RETURN

/*:VRX         DriverDelete
*/
DriverDelete: PROCEDURE EXPOSE globals.
    PARSE ARG record
    IF record == '' THEN DO
        ok = VRMethod( "CN_DEVICES", "GetRecordList", "Selected", "recs." )
        IF ok == 0 | recs.0 < 1 THEN RETURN
        record = recs.1
    END
    CALL VRMethod 'CN_DEVICES', 'SetRecordAttr', record, 'Source', 0

    _name   = VRMethod('CN_DEVICES', 'GetFieldData', record, globals.!hcfDevices.!model )
    _driver = VRMethod('CN_DEVICES', 'GetFieldData', record, globals.!hcfDevices.!driver )
    IF _name == '' THEN RETURN
    IF _driver == '' THEN RETURN

    IF _driver == 'IBMNULL' THEN DO
        CALL VRMessage 'WN_MAIN', NLSGetMessage( 212 ), NLSGetMessage( 209 ), 'W'
        RETURN
    END

    device_name = _driver'.'_name
    device_in_use = 0
    printers.0 = 0
    CALL RPUEnumPrinters 'printers.'
    DO i = 1 TO printers.0
        CALL RPUPrinterQuery printers.i.!name, 'details.'
        CALL StringTokenize details.!drivers, ',', 'used_dev.'
        DO j = 1 TO used_dev.0
            IF used_dev.j == device_name THEN DO
                device_in_use = 1
                LEAVE
            END
        END
        IF device_in_use == 1 THEN DO
            _title = TRANSLATE( printers.i.!description, '  ', '0d0a'x)
            CALL VRMessage 'WN_MAIN', NLSGetMessage( 208, device_name, _title ), NLSGetMessage( 209 ), 'W'
            RETURN
        END
    END

    _btns.0 = 2
    _btns.1 = NLSGetMessage( 14 )
    _btns.2 = NLSGetMessage( 15 )
    _confirm = VRMessage('WN_MAIN', NLSGetMessage( 203, device_name ), NLSGetMessage( 200 ), 'Q', '_btns.', 2, 2 )
    IF _confirm == 1 THEN DO
        success = DeletePrintDriver( _driver, _name )
        IF success == 1 THEN CALL RefreshDevices
        /* TODO check if any other _driver definitions exist and if not, offer to delete _driver files & profile */
    END

RETURN

/*:VRX         Fini
*/
Fini:
    window = VRWindow()
    call VRSet window, "Visible", 0
    drop window
return 0

/*:VRX         GetPrinterPaks
*/
GetPrinterPaks: PROCEDURE EXPOSE globals.
    IF globals.!prdrv  == '' THEN RETURN 0

    IF globals.!is_ecs == 1 THEN _osname = 'eComStation'
    ELSE _osname = 'OS/2'

    CALL VRSet 'WN_MAIN', 'Pointer', 'WAIT'

    _count = 0
    DO WHILE LINES( globals.!prdrv ) > 0
        /* Read the driver definition from PRDRV.LST
         */
        _ppdef = STRIP( LINEIN( globals.!prdrv ))
        IF _ppdef == '' THEN ITERATE
        PARSE VAR _ppdef _ppdriver _ppdisk _ppdesc
        IF _ppdisk == '' THEN ITERATE
        _ppdesc = STRIP( _ppdesc )
        _pppath = globals.!repository'\PMDD_'_ppdisk

        /* Check to see if the driver is installed/active.
         */
        _flags = 'R'
        PARSE UPPER VAR _ppdriver _basename'.DRV' .
        PARSE VALUE VRGetIni('PM_DEVICE_DRIVERS', _basename, 'USER') WITH used_drv '00'x .
        IF ( used_drv <> '') & VRFileExists( used_drv ) THEN DO
            _ppicon = '$1:'used_drv
            _ppstatus = NLSGetMessage( 177, _osname ) NLSGetMessage( 180 )
            _flags = 'I'
        END
        ELSE IF VRFileExists( _pppath'\'_ppdriver ) THEN DO
            _ppicon = '$1:'_pppath'\'_ppdriver
            _ppstatus = NLSGetMessage( 177, _osname )
        END
        ELSE ITERATE

        _pprev = BldLevelVersion( _pppath'\'_ppdriver )

        /* Driver found, add the record.
         */
        _count = _count + 1
        _fielddata = globals.!hcfPaks.!icon';'_ppicon';' ||,
                     globals.!hcfPaks.!name';'_ppdriver';' ||,
                     globals.!hcfPaks.!status';'_ppstatus';' ||,
                     globals.!hcfPaks.!path';'_pppath';' ||,
                     globals.!hcfPaks.!version';'_pprev';' ||,
                     globals.!hcfPaks.!desc';'_ppdesc';' ||,
                     globals.!hcfPaks.!flags';'_flags';'
        ppaks._count = ';'_ppdriver';'_ppicon';;;'_fielddata
    END
    CALL STREAM globals.!prdrv, 'C', 'CLOSE'

    /* Now look for some known third-party drivers.
     */
    PARSE UPPER VALUE VRGetIni('PrinterManager', 'ThirdParty', 'USER') WITH known_3p '00'x .
    IF known_3p == '' THEN
        known_3p = 'ECUPS ECUPS-HP GUTENPRT PSPRINT'
    DO i = 1 TO WORDS( known_3p )
        _basename = WORD( known_3p, i )
        _ppini = VRGetIni('PM_INSTALL', _basename'_DIR', 'USER')
        IF _ppini == '' THEN ITERATE
        PARSE VAR _ppini _pppath '00'x .

        /* Check to see if the driver is installed/active.
         */
        _flags = ''
        _ppdriver = _basename'.DRV'
        PARSE VALUE VRGetIni('PM_DEVICE_DRIVERS', _basename, 'USER') WITH used_drv '00'x .
        IF ( used_drv <> '') & VRFileExists( used_drv ) THEN DO
            _ppicon = '$1:'used_drv
            _ppstatus = NLSGetMessage( 178 ) NLSGetMessage( 180 )
            _flags = 'I'
        END
        ELSE IF VRFileExists( _pppath'\'_ppdriver ) THEN DO
            _ppicon = '$1:'_pppath'\'_ppdriver
            _ppstatus = NLSGetMessage( 178 )
        END
        ELSE ITERATE

        _pprev = BldLevelVersion( _pppath'\'_ppdriver )

        /* Driver found, add the record.
         */
        _count = _count + 1
        _fielddata = globals.!hcfPaks.!icon';'_ppicon';' ||,
                     globals.!hcfPaks.!name';'_ppdriver';' ||,
                     globals.!hcfPaks.!status';'_ppstatus';' ||,
                     globals.!hcfPaks.!path';'_pppath';'  ||,
                     globals.!hcfPaks.!version';'_pprev';' ||,
                     globals.!hcfPaks.!flags';'_flags';'
        ppaks._count = ';'_ppdriver';'_ppicon';;;'_fielddata
    END

    ppaks.0 = _count
    CALL VRMethod 'CN_PRINTERPAKS', 'AddRecordList',, 'Last', 'ppaks.'

    CALL VRSet 'WN_MAIN', 'Pointer', '<default>'

RETURN _count

/*:VRX         GetShippedDrivers
*/
GetShippedDrivers: PROCEDURE EXPOSE globals. shipped.

    count = 0
    CALL LINEIN globals.!prdesc, 1, 0
    DO WHILE LINES( globals.!prdesc )
        _next = LINEIN( globals.!prdesc )
        PARSE VAR _next _desc':' _rest
        _ppak = SUBSTR( _rest, LASTPOS('(', _rest ))
        PARSE VAR _ppak '('_drvr')' .
        count = count + 1
        shipped.count = ';'_desc';#2;;;NAME;'_desc';DRIVER;'_drvr';'
    END
    CALL STREAM globals.!prdesc, 'C', 'CLOSE'
    shipped.0 = count
    CALL VRSortStem 'shipped.'

RETURN count

/*:VRX         Halt
*/
Halt:
    signal _VREHalt
return

/*:VRX         ImportPPD
*/
/* Import a new PPD file into a PostScript driver.  This is unfortunately rather
 * complicated, all the more so because we need to deal with a large number of
 * possible situations.
 */
ImportPPD: PROCEDURE EXPOSE globals.
    ARG driver, ppdfile
    IF driver == '' THEN driver = 'PSCRIPT'

    CALL LINEOUT globals.!log1, 'Going to import PPD file into driver' driver'.'

    status = LocateDriverFiles( driver )

    SELECT

        /* An installable copy of the driver was found.  No special action is
         * required; we can proceed with the import logic below.
         */
        WHEN status == 1 THEN NOP

        /* An installable (working) copy of the driver could not be located, but
         * we found an installed (in use) version whence we can grab the files.
         */
        WHEN status == 2 THEN DO
            /* We need somewhere for the installable copies to live.  We ask
             * the user to confirm this path, as they'll have to specify the
             * directory later on if they install the driver from PM.
             */
            _btn.1 = NLSGetMessage( 2 )
            _btn.2 = NLSGetMessage( 3 )
            _btn.3 = NLSGetMessage( 188 )
            _btn.0 = 3
            _prompt = NLSGetMessage( 232, driver ) ||,
                      '0a0a0a'x || NLSGetMessage( 187, driver )

            /* Try and determine a suitable default directory */
            PARSE VALUE VRGetIni('PM_INSTALL', driver'_DIR', 'USER') WITH drvr_dir '00'x .
            IF drvr_dir == '' THEN
                PARSE VALUE VRGetIni('InstPDR', 'PATH_TO_'driver, 'USER') WITH drvr_dir '00'x .
            IF drvr_dir == '' THEN
                drvr_dir = globals.!repository'\'driver

            /* Now prompt the user to confirm the path */
            DO UNTIL _select <> 3
                _select = VRPrompt( VRWindow(), _prompt, 'drvr_dir',,
                                    NLSGetMessage( 189 ), '_btn.', 1, 2 )
                IF _select == 3 THEN
                    drv_dir = VRXDirectoryDialog( VRWindow(), drvr_dir, NLSGetMessage( 189 ))
            END
            IF drvr_dir == '' THEN RETURN 1                                     /** RC=1 cancelled */
            drvr_dir = VRExpandFileName( drvr_dir )
            IF drvr_dir == '' THEN RETURN 3                                     /** RC=3 bad path */

            /* Create the new directory if necessary */
            IF \VRIsDir( drvr_dir ) THEN
                IF VRMkDir( drvr_dir ) == 0 THEN RETURN 5                       /** RC=5 failed to create directory */

            /* Now copy the driver files */
            IF CopyPrinterPak( driver_path, drvr_dir ) == 0 THEN RETURN 3       /** RC=4 failed to copy driver files */
            CALL VRSetIni 'PM_INSTALL', driver'_DIR', drvr_dir||'00'x, 'USER'

            /* Point to the copied driver files as our new working copy */
            driver_path = drvr_dir'\'driver'.DRV'
        END

        OTHERWISE DO                    /* Driver was not found */
            CALL VRMessage VRWindow(), NLSGetMessage( 231, driver ),,
                           NLSGetMessage( 13 ), 'E'
            RETURN 2                                                            /** RC=2 driver not found */
        END

    END

    /* OK, we should now have an acceptable 'installable' copy of the
     * PrinterPak files.  Next, we will:
     *  - Create a temporary working directory & copy the PrinterPak files there
     *  - Pre-process the PPD file to make it ready for import, and also copy it
     *    to a driver-specific 'saved PPDs' directory for future use
     *  - Use PIN to import the PPD into into our temporary working copy
     *  - Copy the updated driver back to our installable copy
     *  - If the driver is actually installed, copy the updated driver back over
     *    the installed version as well.
     *  - If this is a 'shipped' driver (i.e. one listed in PRDRV.LST) then add
     *    the newly-defined printer to PRDESC.LST.
     */

    /** TODO should move all below this into a new shared function **/

    CALL LINEOUT globals.!log1, 'Driver source:          ' driver_path

    workdir = SysTempFileName( globals.!tmpdir'\PPD_????')
    ok = VRMkDir( workdir )
    IF ok == 1 THEN ok = VrMkDir( workdir'\OUT')
    IF ok <> 1 THEN
        RETURN 5                        /** RC=5 failed to create directory */

    CALL LINEOUT globals.!log1, 'Temporary directory:    ' workdir

    SELECT
        WHEN driver == 'ECUPS'    THEN ppddir = globals.!repository'\PPD_E'
        WHEN driver == 'ECUPS-HP' THEN ppddir = globals.!repository'\PPD_EHP'
        WHEN driver == 'PSPRINT'  THEN ppddir = globals.!repository'\PPD_PS'
        WHEN driver == 'PSPRINT2' THEN ppddir = globals.!repository'\PPD_PS2'
        WHEN driver == 'PSCRIPT2' THEN ppddir = globals.!repository'\PPD2'
        WHEN driver == 'GUTENPRT' THEN ppddir = globals.!repository'\PPD_GP'
        OTHERWISE                      ppddir = globals.!repository'\PPD'
    END

    /* Make sure ppddir (for keeping PPD files) exists */
    CALL SysFileTree ppddir, 'dirs.', 'DO'
    IF dirs.0 == 0 THEN DO
        ok = VRMkDir( ppddir )
        IF ok <> 1 THEN
            RETURN 5                    /** RC=5 failed to create directory */
    END

    CALL LINEOUT globals.!log1, 'Directory for PPD files:' ppddir

    /***
     *** Now do the actual work.
     ***/


    /* Copy the needed driver files to our working directories.
     */
    drvr_dir = VRParseFileName( driver_path, 'DP')
    drv_out  = workdir'\OUT\'driver'.DRV'
    pin_exe  = workdir'\PIN.EXE'
    ppd_exe  = workdir'\PPDENC.EXE'
    ok = VRCopyFile( driver_path, drv_out )
    IF ok == 1 THEN ok = VRCopyFile( drvr_dir'\PIN.EXE', pin_exe )
    IF ok == 1 THEN ok = VRCopyFile( drvr_dir'\PPDENC.EXE', ppd_exe )
    IF ok == 0 THEN DO
        RETURN 4                        /*** RC=4  Failed to copy driver files ***/
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
        ADDRESS CMD '@'globals.!programs.!gzip '-c -d' ppdfile '| RXQUEUE' nq
        DO QUEUED()
            PARSE PULL line
            CALL LINEOUT decppd, line
        END
        CALL LINEOUT decppd
        ppdfile = decppd
    END

    IF VRFileExists( ppdfile ) == 0 THEN DO
        CALL LINEOUT globals.!log1, 'PPD file' ppdfile 'could not be found.'
        RETURN 6                        /** RC=6  PPD import failed **/
    END

    ppd_use = ppddir'\' || VRParseFileName( ppdfile, 'NE')

    /* Now we have to clean up and validate the PPD file so PIN can use it.
     * First, PPDENC converts the codepage if necessary, and copies the results
     * to our working directory.
     */
    CALL LINEOUT globals.!log1, 'Converting PPD with:' ppd_exe ppdfile ppd_use
    ADDRESS CMD '@'ppd_exe ppdfile ppd_use '2>NUL | RXQUEUE' nq
    DO QUEUED()
        PULL output
        CALL LINEOUT globals.!log2, output
    END
    CALL LINEOUT globals.!log2, ''
    CALL LINEOUT globals.!log2

    IF VRFileExists( ppd_use ) == 0 THEN DO
        CALL LINEOUT globals.!log1, 'Hmm,' ppd_use 'was not created.  Copying manually.'
        CALL VRCopyFile ppdfile, ppd_use
    END

    /* Next we strip out some problematic PPD statements which are often
     * encountered in (for example) CUPS-based PPD files.
     */
    CALL CleanPPD ppd_use, globals.!log1

    /* Preparation complete.  Now do the import.
     */
    count = 0
    ADDRESS CMD '@'pin_exe 'ppd' ppddir drv_out '2>NUL | RXQUEUE' nq
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
        RETURN 6                        /** RC=6  PPD import failed **/
    END

    IF driver_repo == 1 THEN DO
        /* If we're working out of the repository, we need to update the
         * driver table in PRDESC.LST to add the new driver(s).
         */

        CALL LINEOUT globals.!log1, 'Updating' globals.!prdesc 'with new entries from' drv_out

/* -- This causes a SYS3175 in the .DRV for some reason...
        ok = UpdatePrDesc( driver'.DRV', drv_out )
        IF ok <> 0 THEN
            CALL LINEOUT globals.!log1, 'Failed to update' globals.!prdesc '(are EAs on' drv_out ' valid?)'
*/

        count = 0

        /* First, copy all lines that don't refer to the driver just updated */
        CALL LINEIN globals.!prdesc, 1, 0
        DO WHILE LINES( globals.!prdesc )
            _next = LINEIN( globals.!prdesc )
            PARSE UPPER VAR _next . ':' _rest
            _tail = SUBSTR( _rest, LASTPOS('(', _rest ))
            PARSE VAR _tail '('_prdrv')' .
            IF _prdrv == driver'.DRV' THEN ITERATE
            count = count + 1
            defs.count = _next
        END
        CALL STREAM globals.!prdesc, 'C', 'CLOSE'

        /* Next, create a new list for the updated driver and merge that in */
        newlist = workdir'\'driver'.LST'
        CALL CreateDriverList drv_out, newlist
        DO WHILE LINES( newlist )
            _line = LINEIN( newlist )
            count = count + 1
            defs.count = _line
        END
        CALL STREAM newlist, 'C', 'CLOSE'
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
            RETURN 7                    /** RC=7  Error updating PRDESC.LST **/
        END
        CALL VRDeleteFile prdesc_tmp

    END

    /* Finally, copy the updated driver files.
     */
    target = VRParseFilePath( driver_path, 'DP')
    CALL LINEOUT globals.!log1, 'Copying files from' workdir'\OUT to' target
    CALL PRReplaceModule target'\'driver'.DRV', '', ''
    ok = VRCopyFile( workdir'\OUT\'driver'.DRV', target'\'driver'.DRV')
    IF ok == 1 THEN
        ok = VRCopyFile( workdir'\OUT\AUXPRINT.PAK', target'\AUXPRINT.PAK')
    IF ok == 1 THEN DO
        /* Copy the updated files to \OS2\DLL\<driver>, replacing any
         * existing copies.  (This prevents problems if the OS/2 driver
         * installation doesn't/fails to copy them, which can happen under
         * some circumstances.)
         */
        IF VRFileExists( globals.!os2dir'\DLL\'driver'.DRV') THEN DO
            CALL VRCopyFile workdir'\OUT\AUXPRINT.PAK',,
                            globals.!os2dir'\DLL\'driver'\AUXPRINT.PAK'
            CALL PRReplaceModule globals.!os2dir'\DLL\'driver'\'driver'.DRV', '', ''
            CALL VRCopyFile workdir'\OUT\'driver'.DRV', globals.!os2dir'\DLL\'driver'\'driver'.DRV'
        END
    END
    IF ok == 0 THEN DO
        CALL LINEOUT globals.!log1, VRError()
        RETURN 4                        /*** RC=4  Failed to copy driver files ***/
    END

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
    /* Hide the VX-REXX console window
     */
    CALL VRSet 'Console', 'WindowListTitle', ''

    CALL RxFuncAdd 'SysLoadFuncs',     'REXXUTIL', 'SysLoadFuncs'
    CALL SysLoadFuncs
    CALL RxFuncAdd 'RPULoadFuncs',     'RXPRTUTL', 'RPULoadFuncs'
    CALL RPULoadFuncs
    CALL RxFuncAdd 'PRLoadFuncs',      'PR1UTIL', 'PRLoadFuncs'
    CALL PRLoadFuncs
    CALL RxFuncAdd 'WPToolsLoadFuncs', 'WPTOOLS',  'WPToolsLoadFuncs'
    CALL WPToolsLoadFuncs
    CALL RxFuncAdd 'VRXLoadFuncs',     'VROBJEX',  'VRXLoadFuncs'
    CALL VRXLoadFuncs

    globals.!wizard = 0
    globals.!debug  = 0
    globals.!initstate = 0

    DO i = 1 TO InitArgs.0
        PARSE UPPER VAR InitArgs.i _arg
        SELECT
            WHEN _arg == '/CREATE' THEN globals.!wizard = 1
            WHEN LEFT( _arg, 2 ) == '/D' THEN globals.!debug = 1
            OTHERWISE NOP
        END
    END

    CALL LoadSettings
    CALL SetLanguage
    CALL CheckPrograms

    window = VRWindow()
    call VRMethod window, "CenterWindow"
    call VRMethod window, "Activate"
    IF globals.!wizard THEN DO
        CALL VRMethod 'Application', 'PostQueue', 0, 1, 'CALL PB_PRINTER_ADD_Click'
    END
    ELSE DO
        CALL VRSet window, "Visible", 1
        CALL VRMethod 'TDL_MAIN', 'SetFocus'
    END
    drop window

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

    globals.!version = '0.42'
    globals.!copyright = '2013'

    me = VRGet('Application', 'Program')
    IF me == '' THEN PARSE SOURCE . . me
    globals.!mydir = VRParseFilePath( me, 'DP')

    /* Get CUPS paths.
     */
    PARSE VALUE VRGetIni('eCups', 'CUPS', 'USER') WITH cupsdrv '00'x .
    IF cupsdrv == '' THEN
        cupsdrv = VRParseFilePath( me, 'D') || ':'
    ELSE
        cupsdrv = STRIP( cupsdrv, 'T', '\')
    globals.!cupsdir = cupsdrv'\cups'

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

    globals.!log1 = globals.!logdir'\printman.l1'
    globals.!log2 = globals.!logdir'\printman.l2'
    CALL InitMessageLog
    IF VRFileExists( globals.!log2 ) THEN
        CALL VRDeleteFile globals.!log2


    /* Get printer-related paths.
     */
    PARSE VALUE VRGetIni('PM_INSTALL', 'PDR_DIR', 'USER') WITH repos_dir '00'x .
    globals.!repository = repos_dir
    globals.!prdrv      = STREAM( globals.!os2dir'\install\prdrv.lst',  'C', 'QUERY EXISTS')
    globals.!prdesc     = STREAM( globals.!os2dir'\install\prdesc.lst', 'C', 'QUERY EXISTS')

    /* Set the language file name.
     */
    globals.!nlsname = 'printm'

    /* Determine the OS.
     */
    IF VRFileExists( globals.!os2dir'\INSTALL\SYSLEVEL.ECS') THEN
        globals.!is_ecs = 1
    ELSE
        globals.!is_ecs = 0

    globals.!create_mode = ''

RETURN

/*:VRX         MI_ABOUT_Click
*/
MI_ABOUT_Click:
    CALL VRLoadSecondary 'SW_ABOUT', 'W'
RETURN

/*:VRX         MI_DETAILVIEW_Click
*/
MI_DETAILVIEW_Click: PROCEDURE EXPOSE globals.

    _page = VRGet('TDL_MAIN', 'Selected')
    _pagewin = VRMethod('TDL_MAIN', 'GetPageWindow', _page )
    _cnr = VRGet( _pagewin, 'UserData')
    IF _cnr == '' THEN RETURN

    CALL VRSet _cnr, 'Source', 0
    _set = VRGet('MI_DETAILVIEW', 'Checked')
    IF _set == 1 THEN RETURN

    CALL VRSet _cnr, 'View', 'Detail'
    CALL VRSet _cnr, 'MiniIcons', 1

    CALL VRSet 'MI_ICONVIEW',   'Checked', 0
    CALL VRSet 'MI_DETAILVIEW', 'Checked', 1

RETURN

/*:VRX         MI_DRIVERS_INSTALL_Click
*/
MI_DRIVERS_INSTALL_Click:
    CALL DriverAdd
return

/*:VRX         MI_DRIVERS_REMOVE_Click
*/
MI_DRIVERS_REMOVE_Click:
    CALL DriverDelete
return

/*:VRX         MI_ICONVIEW_Click
*/
MI_ICONVIEW_Click: PROCEDURE EXPOSE globals.

    _page = VRGet('TDL_MAIN', 'Selected')
    _pagewin = VRMethod('TDL_MAIN', 'GetPageWindow', _page )
    _cnr = VRGet( _pagewin, 'UserData')
    IF _cnr == '' THEN RETURN

    CALL VRSet _cnr, 'Source', 0
    _set = VRGet('MI_ICONVIEW', 'Checked')
    IF _set == 1 THEN RETURN

    CALL VRSet _cnr, 'View', 'Icon'
    CALL VRSet _cnr, 'MiniIcons', 0

    CALL VRSet 'MI_ICONVIEW',   'Checked', 1
    CALL VRSet 'MI_DETAILVIEW', 'Checked', 0

RETURN

/*:VRX         MI_IMPORT_Click
*/
MI_IMPORT_Click: PROCEDURE EXPOSE globals. new_ppd

    /* Prompt the user for the PPD file to import.
     */
    new_ppd = VRFileDialog( VRWindow(), NLSGetMessage( 230 ), 'O', '*.PPD')
    IF new_ppd == '' THEN RETURN 0

    /* Let the user select the Postscript driver to update.
     */
    CALL VRLoadSecondary 'SW_IMPORT', 'W'

RETURN ok

/*:VRX         MI_MANAGE_Click
*/
MI_MANAGE_Click: PROCEDURE EXPOSE globals.
    CALL VRLoadSecondary 'SW_PRINTERPAKS', 'W'
RETURN

/*:VRX         MI_PORT_CREATE_Click
*/
MI_PORT_CREATE_Click:
    CALL PortAdd
return

/*:VRX         MI_PORT_DELETE_Click
*/
MI_PORT_DELETE_Click:
    CALL PortDelete
return

/*:VRX         MI_PORT_PROPS_Click
*/
MI_PORT_PROPS_Click:
    CALL PortProperties
RETURN

/*:VRX         MI_PPAK_COPY_Click
*/
MI_PPAK_COPY_Click:
    ok = VRMethod( "CN_PRINTERPAKS", "GetRecordList", "Selected", "recs." )
    IF ok == 0 | recs.0 < 1 THEN RETURN
    record = recs.1

    _path = VRMethod('CN_PRINTERPAKS', 'GetFieldData', record, globals.!hcfPaks.!path )
    CALL VRMethod 'Application', 'PutClipboard', _path

    CALL VRMethod 'CN_PRINTERPAKS', 'SetRecordAttr', record, 'Source', 0
RETURN

/*:VRX         MI_PPAK_MODELS_Click
*/
MI_PPAK_MODELS_Click: PROCEDURE EXPOSE globals.

    ok = VRMethod( "CN_PRINTERPAKS", "GetRecordList", "Selected", "recs." )
    IF ok == 0 | recs.0 < 1 THEN RETURN
    CALL VRMethod 'CN_PRINTERPAKS', 'SetRecordAttr', recs.1, 'Source', 0

    CALL VRLoadSecondary 'SW_MODELS', 'W'

RETURN

/*:VRX         MI_PRINTER_CREATE_Click
*/
MI_PRINTER_CREATE_Click:
    ok = VRMethod( "CN_PRINTERS", "GetRecordList", "Selected", "recs." )
    IF ok == 1 & recs.0 > 0 THEN
        CALL VRMethod 'CN_PRINTERS', 'SetRecordAttr', recs.1, 'Source', 0
    CALL PrinterWizard
RETURN

/*:VRX         MI_PRINTER_DEFAULT_Click
*/
MI_PRINTER_DEFAULT_Click:
    CALL PrinterSetDefault
RETURN

/*:VRX         MI_PRINTER_DELETE_Click
*/
MI_PRINTER_DELETE_Click:
    CALL PrinterDelete
RETURN

/*:VRX         MI_PRINTER_HOLD_Click
*/
MI_PRINTER_HOLD_Click:
    CALL PrinterHold
return

/*:VRX         MI_PRINTER_OPEN_Click
*/
MI_PRINTER_OPEN_Click:
    CALL PrinterOpen
RETURN

/*:VRX         MI_PRINTER_PROPS_Click
*/
MI_PRINTER_PROPS_Click:
    CALL PrinterProperties
RETURN

/*:VRX         MI_QUIT_Click
*/
MI_QUIT_Click:
    CALL Quit
return

/*:VRX         MI_REFRESH_Click
*/
MI_REFRESH_Click: PROCEDURE EXPOSE globals.

    _page = VRGet('TDL_MAIN', 'Selected')
    _pagewin = VRMethod('TDL_MAIN', 'GetPageWindow', _page )
    _cnr = VRGet( _pagewin, 'UserData')
    IF _cnr == '' THEN RETURN

    CALL VRSet _cnr, 'Source', 0

    SELECT
        WHEN _cnr == 'CN_PRINTERS' THEN CALL RefreshPrinters
        WHEN _cnr == 'CN_PORTS' THEN CALL RefreshPorts
        WHEN _cnr == 'CN_DEVICES' THEN CALL RefreshDevices
        OTHERWISE NOP
    END

RETURN

/*:VRX         New_CUPS
*/
New_CUPS: PROCEDURE EXPOSE globals.
    PARSE ARG ppd_file

    IF WPToolsQueryObject('<ECUPS_WIZARD>',,,'SetupString') == 1 THEN DO
        PARSE VAR SetupString . 'EXENAME='cupswiz';' .
        PARSE VAR SetupString . 'STARTUPDIR='_workdir';' .
        PARSE VAR SetupString . 'PARAMETERS='_parms';' .
        IF _workdir == '' THEN _workdir = VRParseFilePath( cupswiz, 'DP')
    END
    ELSE DO
        cupswiz = Stream( globals.!cupsdir'\cups\cupswiz.exe', 'C', 'QUERY EXISTS')
        _workdir = VRParseFilePath( cupswiz, 'DP')
        _parms = ''
    END
    IF cupswiz <> '' THEN DO
        od = DIRECTORY()
        CALL DIRECTORY _workdir
        IF ppd_file <> '' & VRFileExists( ppd_file ) THEN
            _parms = _parms ppd_file
        ADDRESS CMD 'start' cupswiz _parms
        CALL DIRECTORY od
        CALL Quit
    END

RETURN

/*:VRX         New_LAN
*/
New_LAN: PROCEDURE EXPOSE globals.

    ok = OpenPrinterTemplate('WPRPrinter')
    CALL Quit

RETURN ok

/*:VRX         New_Standard
*/
New_Standard: PROCEDURE EXPOSE globals.

    ok = OpenPrinterTemplate('WPPrinter')
    CALL Quit

RETURN ok

/*:VRX         OpenPrinterTemplate
*/
OpenPrinterTemplate: PROCEDURE EXPOSE globals.
    PARSE ARG _class

    ok = 0
    IF WPToolsFolderContent('<WP_TEMPS>', '_temps.', 'F') THEN
    DO i = 1 TO _temps.0
        IF WPToolsQueryObject( _temps.i, 'szClass') THEN DO
            IF szClass == _class THEN DO
                ok = WPToolsSetObjectData( _temps.i, 'OPEN=DEFAULT;')
                LEAVE
            END
        END
    END

    IF ok == 0 THEN DO
        IF SysCreateObject( _class, _class, globals.!tmpdir,,
                            'OBJECTID=<TEMP_PRT>;TAKEDEFAULTS=NO;TEMPLATE=YES', 'R') THEN
        DO
            ok = SysSetObjectData '<TEMP_PRT>','OPEN=DEFAULT;'
            CALL SysDestroyObject '<TEMP_PRT>'
        END
    END

RETURN ok

/*:VRX         PB_ABOUT_OK_Click
*/
PB_ABOUT_OK_Click:
    CALL SW_ABOUT_Close
RETURN

/*:VRX         PB_ADDPORT_CANCEL_Click
*/
PB_ADDPORT_CANCEL_Click:
    CALL SW_PORT_ADD_Close
RETURN

/*:VRX         PB_ADDPORT_OK_Click
*/
PB_ADDPORT_OK_Click: PROCEDURE EXPOSE globals. updated

    ok = VRMethod('CN_ADDPORT', 'GetRecordList', 'Selected', 'recs.')
    IF ok == 0 | recs.0 < 1 THEN RETURN
    record = recs.1

    _name = VRMethod('CN_ADDPORT', 'GetFieldData', record, 'NAME')
    btns.0 = 2
    btns.1 = NLSGetMessage( 14 )
    btns.2 = NLSGetMessage( 15 )
    ok = VRMessage('SW_PORT_ADD', NLSGetMessage( 205, _name ), NLSGetMessage( 200 ), 'Q', 'btns.', 1, 2 )
    IF ok == 1 THEN DO
        _portname = GetNextPortName( _name )
        ok = RPUPortInstall( _name, _portname )
        IF ok <> 1 THEN
            CALL VRMessage 'SW_PORT_ADD', NLSGetMessage( 167 ) RPUERROR, NLSGetMessage( 13 ), 'E'
        ELSE DO
            CALL RPUPortDialog _name, _portname
            CALL SW_PORT_ADD_Close
            updated = 1
        END
    END

RETURN ok

/*:VRX         PB_DEVICE_ADD_Click
*/
PB_DEVICE_ADD_Click:
    CALL DriverAdd
RETURN

/*:VRX         PB_DEVICE_CANCEL_Click
*/
PB_DEVICE_CANCEL_Click:
    CALL SW_DEVICE_ADD_Close
RETURN

/*:VRX         PB_DEVICE_INSTALL_Click
*/
PB_DEVICE_INSTALL_Click: PROCEDURE EXPOSE globals. updated
    updated = 1

    ok = VRMethod('CN_DEVICE_LIST', 'GetRecordList', 'Selected', 'recs.')
    IF ok == 0 THEN RETURN
    IF recs.0 == 0 THEN RETURN
    selected = recs.1

    dev_name = VRMethod('CN_DEVICE_LIST', 'GetFieldData', selected, 'NAME')
    dev_driver = VRMethod('CN_DEVICE_LIST', 'GetFieldData', selected, 'DRIVER')
    driver_short = TRANSLATE( VRParseFileName( dev_driver, 'N'))
    IF dev_name == '' | dev_driver == '' | driver_short == '' THEN RETURN

    /* Shipped drivers don't include the FQ path in dev_driver so we must
     * resolve it first.
     */
    IF VRGet('RB_DEVICE_SHIPPED', 'Set') == 1 THEN
        dev_driver = GetDriverSource( driver_short )
    IF dev_driver == '' THEN RETURN

    ok = InstallPrintDriver( driver_short, dev_driver, dev_name )

    IF ok <> 0 THEN DO
        CALL VRMessage VRWindow(), NLSGetMessage( 168, driver_short'.'dev_name ), NLSGetMessage( 13 ), 'E'
    END
    CALL SW_DEVICE_ADD_Close

RETURN

/*:VRX         PB_DEVICE_OTHER_Click
*/
PB_DEVICE_OTHER_Click: PROCEDURE EXPOSE globals. shipped.

    old_path = VRGet('DDCB_DEVICE_3RDPARTY', 'UserData')
    new_path = VRXDirectoryDialog( VRWindow(), old_path, NLSGetMessage( 189 ))
    IF new_path == '' THEN RETURN
    IF old_path == TRANSLATE( new_path ) THEN RETURN

    CALL VRSet 'DDCB_DEVICE_3RDPARTY', 'Value', new_path
    CALL SetDriverList new_path

RETURN

/*:VRX         PB_DEVICE_REFRESH_Click
*/
PB_DEVICE_REFRESH_Click: PROCEDURE EXPOSE globals. shipped.
    CALL SetDriverList
return

/*:VRX         PB_DEVICE_REMOVE_Click
*/
PB_DEVICE_REMOVE_Click:
    CALL DriverDelete
RETURN

/*:VRX         PB_DEVICE_REMOVE_KeyPress
*/
PB_DEVICE_REMOVE_KeyPress: 
    IF VRGet('PB_DEVICE_REMOVE', 'KeyString') == '{Tab}' THEN
        CALL VRMethod 'TDL_MAIN', 'SetFocus'
return

/*:VRX         PB_IMPORTCANCEL_Click
*/
PB_IMPORTCANCEL_Click:
    CALL SW_IMPORT_Close
return

/*:VRX         PB_IMPORTOK_Click
*/
PB_IMPORTOK_Click: PROCEDURE EXPOSE globals. new_ppd

    IF VRGet('RB_IMPORTCUPS', 'Set') == 1 THEN DO
        CALL New_CUPS new_ppd
        CALL SW_IMPORT_Close
        CALL VRSet 'WN_MAIN', 'Shutdown', 1
        RETURN
    END
    driver = VRGet('DDCB_IMPORTDRV', 'Value')
    ok = ImportPPD( driver, new_ppd )

    SELECT
        /* Check the return code from ImportPPD and display an appropriate message.
         */

        WHEN ok == 0 THEN DO            /* Success! */
            _message = NLSGetMessage( 238 )
            _mbtitle = NLSGetMessage( 16 )
            _mbicon  = 'I'
        END

        /* Note: Error codes 1 & 2 are ignored here.
         *       1 means the user cancelled, so no message is required.
         *       For 2, ImportPPD itself already displayed the error message.
         */

        WHEN ok == 3 THEN DO            /* Invalid path specified */
            _message = NLSGetMessage( 222 )
            _mbtitle = NLSGetMessage( 13 )
            _mbicon  = 'E'
        END
        WHEN ok == 4 THEN DO            /* Failed to copy driver files */
            _message = NLSGetMessage( 220 )
            _mbtitle = NLSGetMessage( 13 )
            _mbicon  = 'E'
        END
        WHEN ok == 5 THEN DO            /* Failed to create directory */
            _message = NLSGetMessage( 221 )
            _mbtitle = NLSGetMessage( 13 )
            _mbicon  = 'E'
        END
        WHEN ok == 6 THEN DO            /* PPD import failed */
            _message = NLSGetMessage( 223 )
            _mbtitle = NLSGetMessage( 13 )
            _mbicon  = 'E'
        END
        WHEN ok == 7 THEN DO            /* Error updating PRDESC.LST */
            _message = NLSGetMessage( 224, globals.!prdesc )
            _mbtitle = NLSGetMessage( 13 )
            _mbicon  = 'E'
        END

        OTHERWISE DO
            _message = ''
            _mbtitle = ''
            _mbicon  = 'N'
        END
    END

    IF message <> '' THEN
        CALL VRMessage VRWindow(), _message, _mbtitle, _mbicon

    CALL SW_IMPORT_Close
RETURN

/*:VRX         PB_MODELS_CLOSE_Click
*/
PB_MODELS_CLOSE_Click:
    CALL SW_MODELS_Close
return

/*:VRX         PB_OTHERPORT_Click
*/
PB_OTHERPORT_Click: PROCEDURE EXPOSE globals.
    _new_pdr = VRFileDialog('SW_PORT_ADD', NLSGetMessage( 165 ), 'O', '*.PDR')
    IF _new_pdr == '' THEN RETURN 0

    ok = InstallPortDriver( _new_pdr )
    IF ok == 0 THEN DO
        _name = VRParseFileName( _new_pdr, 'N')
        _rec = VRMethod('CN_ADDPORT', 'AddRecord',,, _name, _new_pdr )
        CALL VRMethod 'CN_ADDPORT', 'SetFieldData', _rec, 'NAME', _name, 'PATH', _new_pdr
    END
    ELSE
        CALL VRMessage 'SW_PORT_ADD', NLSGetMessage( 166 ) || '0a0a'x || VRError(), NLSGetMessage( 13 ), 'E'
RETURN ok

/*:VRX         PB_PORT_ADD_Click
*/
PB_PORT_ADD_Click:
    CALL PortAdd
RETURN

/*:VRX         PB_PORT_DELETE_Click
*/
PB_PORT_DELETE_Click:
    CALL PortDelete
return

/*:VRX         PB_PORT_EDIT_Click
*/
PB_PORT_EDIT_Click: 
    CALL PortProperties
return

/*:VRX         PB_PORT_EDIT_KeyPress
*/
PB_PORT_EDIT_KeyPress: 
    IF VRGet('PB_PORT_EDIT', 'KeyString') == '{Tab}' THEN
        CALL VRMethod 'TDL_MAIN', 'SetFocus'
return

/*:VRX         PB_PPCLOSE_Click
*/
PB_PPCLOSE_Click:
    CALL SW_PRINTERPAKS_Close
return

/*:VRX         PB_PPUPDATE_Click
*/
PB_PPUPDATE_Click: PROCEDURE EXPOSE globals.

    /* Let the user select the driver .DRV to install.
     */
    new_drv = VRFileDialog( VRWindow(), NLSGetMessage( 182 ), 'O', '*.DRV')
    IF new_drv == '' THEN RETURN 0

    drv_stem   = TRANSLATE( VRParseFileName( new_drv, 'N'))
    drv_name   = TRANSLATE( VRParseFileName( new_drv, 'NE'))
    drv_source = TRANSLATE( VRParseFileName( new_drv, 'DP'))

    /* See if this is the same as an existing driver (compared by filename).
     */
    existing = 0
    skip_repo = 0
    ok = VRMethod('CN_PRINTERPAKS', 'GetRecordList', 'All', 'recs.')
    DO i = 1 TO recs.0
        _ppname = VRMethod('CN_PRINTERPAKS', 'GetFieldData', recs.i, globals.!hcfPaks.!name )
        IF TRANSLATE( _ppname ) == TRANSLATE( new_name ) THEN DO
            existing = 1
            _pppath = VRMethod('CN_PRINTERPAKS', 'GetFieldData', recs.i, globals.!hcfPaks.!path )
            LEAVE
        END
    END

    PARSE UPPER VALUE VRGetIni('PrinterManager', 'ThirdParty', 'USER') WITH known_3p '00'x .
    IF known_3p == '' THEN
        known_3p = 'ECUPS ECUPS-HP GUTENPRT PSPRINT'

    IF existing THEN DO
        drv_target = TRANSLATE( _pppath )
        IF drv_source == drv_target ) THEN DO
            CALL VRMessage VRWindow(), NLSGetMessage( 183 ), NLSGetMessage( 13 ), 'E'
            RETURN 1
        END
        IF CheckWritablePath( drv_target ) <> 0 THEN DO
            existing = 0
            _msg.1 = NLSGetMessage( 185, drv_target )
            _msg.2 = ''
            _msg.3 = NLSGetMessage( 186, drv_name )
            _msg.0 = 3
            _btns.1 = NLSGetMessage( 2 )
            _btns.2 = NLSGetMessage( 3 )
            _btns.0 = 2
            _select = VRMessageStem( VRWindow(), '_msg.', NLSGetMessage( 184 ),
                                     'I', '_btns.', 1, 2 )
            IF _select == 2 THEN skip_repo = 1
        END
    END

    IF \skip_repo THEN DO
        IF \existing THEN DO
            /* If we already know about this driver, try and get its saved location. */
            drv_target = ''
            IF WORDPOS( drv_stem, known_3p ) > 0 THEN
                PARSE UPPER VALUE VRGetIni('PM_INSTALL', drv_stem'_DIR', 'USER') WITH drv_target '00'x .
            ELSE DO
                known_3p = known_3p drv_stem
                CALL VRSetIni 'PrinterManager', 'ThirdParty', known_3p || '00'x, 'USER'
            END
            IF drv_target == '' THEN DO
                drv_target = globals.!repository'\'drv_target
                _btns.1 = NLSGetMessage( 2 )
                _btns.2 = NLSGetMessage( 3 )
                _btns.3 = NLSGetMessage( 188 )
                _btns.0 = 3
                DO UNTIL _select <> 3
                    _select = VRPrompt( VRWindow(), NLSGetMessage( 187, drv_name ),,
                                        'drv_target', NLSGetMessage( 189 ), '_btns.', 1, 2 )
                    IF _select == 3 THEN
                        drv_target = VRXDirectoryDialog( VRWindow(), drv_target, NLSGetMessage( 189 ))
                END
                IF _select == 2 THEN
                    skip_repo = 1
                ELSE
                    CALL VRSetIni 'PM_INSTALL', drv_stem'_DIR', TRANSLATE( drv_target ) || '00'x, 'USER'
            END
        END

        IF \skip_repo THEN DO
            IF VRFileExists( drv_target ) THEN DO
                /* Clear any RHS attributes on existing files */
                CALL SysFileTree drv_target'\*', '_ignore.',,, '**---'
                CALL PRReplaceModule drv_target'\'drv_name, '', ''
            END
            ELSE
                /* Create the target directory if necessary */
                ok = VRMkDir( drv_target )

            ok = CopyPrinterPak( new_drv, drv_target )
            IF existing & ok == 1 THEN DO
                CALL UpdatePrDesc new_drv, drv_target'\'new_drv
            END
            IF ok == 0 THEN DO
                CALL VRMessage VRWindow(), NLSGetMessage( 220 ), NLSGetMessage( 13 ), 'E'
                RETURN 2
            END
            ELSE
                CALL VRMessage VRWindow(), NLSGetMessage( 193 ), NLSGetMessage( 16 ), 'I'
        END
    END

    PARSE VALUE VRGetIni('PM_DEVICE_DRIVERS', drv_stem, 'USER') WITH drv_used '00'x .
    IF ( drv_used <> '') & VRFileExists( drv_used ) THEN DO
        /* Driver is currently installed; ask if the user wants to update the
         * in-use copy as well.
         */
        _btns.1 = NLSGetMessage( 14 )
        _btns.2 = NLSGetMessage( 15 )
        _btns.0 = 2
        _select = VRMessage( VRWindow(), NLSGetMessage( 190 ), NLSGetMessage( 191 ), 'Q', '_btns.', 1, 2 )
        IF _select == 1 THEN DO
            CALL PRSetFileAttr drv_used , 'rsh'
            CALL PRReplaceModule drv_used, '', ''
            used_target = VRParseFileName( drv_used, 'DP')
            ok = CopyPrinterPak( new_drv, used_target )
            IF ok == 0 THEN DO
                CALL VRMessage VRWindow(), NLSGetMessage( 220 ), NLSGetMessage( 13 ), 'E'
                RETURN 2
            END
            ELSE
                CALL VRMessage VRWindow(), NLSGetMessage( 192 ), NLSGetMessage( 16 ), 'I'
        END
    END

RETURN 0

/*:VRX         PB_PRINTER_ADD_Click
*/
PB_PRINTER_ADD_Click:
    CALL PrinterWizard
RETURN

/*:VRX         PB_PRINTER_DEL_Click
*/
PB_PRINTER_DEL_Click:
    CALL PrinterDelete
return

/*:VRX         PB_PRINTER_EDIT_Click
*/
PB_PRINTER_EDIT_Click: 
    CALL PrinterProperties
RETURN

/*:VRX         PB_PRINTER_EDIT_KeyPress
*/
PB_PRINTER_EDIT_KeyPress: 
    IF VRGet('PB_PRINTER_EDIT', 'KeyString') == '{Tab}' THEN
        CALL VRMethod 'TDL_MAIN', 'SetFocus'
return

/*:VRX         PB_PRINTER_OPEN_Click
*/
PB_PRINTER_OPEN_Click:
    CALL PrinterOpen
RETURN

/*:VRX         PB_WIZARD_CANCEL_Click
*/
PB_WIZARD_CANCEL_Click:

    CALL SW_WIZARD_Close

return

/*:VRX         PB_WIZARD_NEXT_Click
*/
PB_WIZARD_NEXT_Click: PROCEDURE EXPOSE globals.
    SELECT
        WHEN VRGet('RB_CREATE_CUPS', 'Set') == 1 THEN DO
            globals.!create_mode = 'C'
            CALL New_CUPS
        END
        WHEN ( VRGet('RB_CREATE_LEGACY', 'Set') == 1 ) & ( VRGet('RB_CREATE_STANDARD', 'Set') == 1 ) THEN DO
            globals.!create_mode = 'S'
            CALL New_Standard
        END
        WHEN ( VRGet('RB_CREATE_LEGACY', 'Set') == 1 ) & ( VRGet('RB_CREATE_LAN', 'Set') == 1 ) THEN DO
            globals.!create_mode = 'L'
            CALL New_LAN
        END
        OTHERWISE DO
            globals.!create_mode = ''
            globals.!wizard = 0
        END
    END
    CALL SW_WIZARD_Fini

RETURN

/*:VRX         PortAdd
*/
PortAdd: PROCEDURE EXPOSE globals.

    updated = 0
    CALL VRLoadSecondary 'SW_PORT_ADD', 'W'
    IF updated == 1 THEN
        CALL RefreshPorts

RETURN

/*:VRX         PortDelete
*/
PortDelete: PROCEDURE EXPOSE globals.
    PARSE ARG record
    IF record == '' THEN DO
        ok = VRMethod( "CN_PORTS", "GetRecordList", "Selected", "recs." )
        IF ok == 0 | recs.0 < 1 THEN RETURN
        record = recs.1
    END
    CALL VRMethod 'CN_PORTS', 'SetRecordAttr', record, 'Source', 0

    _name = VRMethod('CN_PORTS', 'GetFieldData', record, globals.!hcfPorts.!name )

    printers.0 = 0
    CALL RPUEnumPrinters 'printers.'
    DO i = 1 TO printers.0
        CALL RPUPrinterQuery printers.i.!name, 'details.'
        IF _name == details.!port THEN DO
            _title = TRANSLATE( printers.i.!description, '  ', '0d0a'x)
            CALL VRMessage 'WN_MAIN', NLSGetMessage( 206, _name, _title ), NLSGetMessage( 207 ), 'W'
            RETURN
        END
    END

    IF _name == '' THEN RETURN
    _btns.0 = 2
    _btns.1 = NLSGetMessage( 14 )
    _btns.2 = NLSGetMessage( 15 )
    _confirm = VRMessage('WN_MAIN', NLSGetMessage( 202, _name ), NLSGetMessage( 200 ), 'Q', '_btns.', 2, 2 )
    IF _confirm == 1 THEN DO
        success = RPUPortDelete( _name )
        IF success == 1 THEN CALL RefreshPorts
    END

RETURN

/*:VRX         PortProperties
*/
PortProperties: PROCEDURE EXPOSE globals.
    PARSE ARG record
    IF record == '' THEN DO
        ok = VRMethod( "CN_PORTS", "GetRecordList", "Selected", "recs." )
        IF ok == 0 | recs.0 < 1 THEN RETURN
        record = recs.1
    END

    _name   = VRMethod('CN_PORTS', 'GetFieldData', record, globals.!hcfPorts.!name )
    _module = VRMethod('CN_PORTS', 'GetFieldData', record, globals.!hcfPorts.!driver )
    IF _module == '' THEN DO
        CALL VRMessage VRWindow(), NLSGetMessage( 211, _name ), NLSGetMessage( 210 ), 'N'
        RETURN
    END

    CALL RPUPortDialog _module, _name

    CALL VRMethod 'CN_PORTS', 'SetRecordAttr', record, 'Source', 0
RETURN

/*:VRX         PrinterDelete
*/
PrinterDelete: PROCEDURE EXPOSE globals.
    PARSE ARG record
    IF record == '' THEN DO
        ok = VRMethod( "CN_PRINTERS", "GetRecordList", "Selected", "recs." )
        IF ok == 0 | recs.0 < 1 THEN RETURN 0
        record = recs.1
    END
    CALL VRMethod 'CN_PRINTERS', 'SetRecordAttr', record, 'Source', 0

    success = 0
    _title = VRMethod('CN_PRINTERS', 'GetFieldData', record, globals.!hcfPrinters.!title )
    _queue = VRMethod('CN_PRINTERS', 'GetFieldData', record, globals.!hcfPrinters.!queue )
    _port  = VRMethod('CN_PRINTERS', 'GetFieldData', record, globals.!hcfPrinters.!port )

    _btns.0 = 2
    _btns.1 = NLSGetMessage( 14 )
    _btns.2 = NLSGetMessage( 15 )
    _confirm = VRMessage('WN_MAIN', NLSGetMessage( 201, _title ), NLSGetMessage( 200 ), 'Q', '_btns.', 2, 2 )
    IF _confirm == 1 THEN DO
        success = RPUPrinterDelete( _queue )
        CALL RPUPortInfo _port, 'port.'
        IF port.!driver == 'CUPS' THEN DO
            /* If this corresponds to a locally-defined CUPS queue, offer to
             * to delete that as well.
             */
            cups_prnt = RPUPortQuery('CUPS', _port )
            IF cups_prnt <> '' & ( LENGTH( cups_prnt ) == 130 ) THEN DO
                PARSE VAR cups_prnt 1 _cups_host 66 _cups_queue
                PARSE VAR _cups_host cups_host '00'x .
                PARSE VAR _cups_queue cups_queue '00'x .
                IF ( TRANSLATE( cups_host ) == 'LOCALHOST') | cups_host = '127.0.0.1' THEN DO
                    _confirm = VRMessage('WN_MAIN', NLSGetMessage( 204, _title, cups_queue, _port ),,
                                          NLSGetMessage( 200 ), 'Q', '_btns.', 2, 2 )
                    IF _confirm == 1 THEN DO
                        success = CupsDeletePrinter( cups_host, cups_queue )
                        IF success == 0 THEN DO
                            CALL RPUPortDelete _port
                            CALL RefreshPorts
                        END
                        ELSE
                            CALL VRMessage 'WN_MAIN', NLSGetMessage( 251, cups_queue, success ), NLSGetMessage( 18 ), 'E'
                    END
                END
            END
        END
    END

    CALL RefreshPrinters

RETURN success

/*:VRX         PrinterHold
*/
PrinterHold: PROCEDURE EXPOSE globals.
    PARSE ARG record
    IF record == '' THEN DO
        ok = VRMethod( "CN_PRINTERS", "GetRecordList", "Selected", "recs." )
        IF ok == 0 | recs.0 < 1 THEN RETURN
        record = recs.1
    END
    _queue = VRMethod('CN_PRINTERS', 'GetFieldData', record, globals.!hcfPrinters.!queue )
    _flags = VRMethod('CN_PRINTERS', 'GetFieldData', record, globals.!hcfPrinters.!flags )
    _p = POS('P', _flags )
    IF _p > 0 THEN DO
        ok = RPUQueueHold( _queue, 'N')
        IF ok == 1 THEN DO
            CALL VRSet 'MI_PRINTER_HOLD', 'Checked', 0
            _flags = DELSTR( _flags, _p, 1 )
            CALL VRMethod 'CN_PRINTERS', 'SetFieldData', record, globals.!hcfPrinters.!flags, _flags
        END
    END
    ELSE DO
        ok = RPUQueueHold( _queue, 'Y')
        IF ok == 1 THEN DO
            CALL VRSet 'MI_PRINTER_HOLD', 'Checked', 1
            _flags = _flags'P'
            CALL VRMethod 'CN_PRINTERS', 'SetFieldData', record, globals.!hcfPrinters.!flags, _flags
        END
    END

    CALL VRMethod 'CN_PRINTERS', 'SetRecordAttr', record, 'Source', 0
RETURN

/*:VRX         PrinterOpen
*/
PrinterOpen: PROCEDURE EXPOSE globals.
    PARSE ARG record
    IF record == '' THEN DO
        ok = VRMethod( "CN_PRINTERS", "GetRecordList", "Selected", "recs." )
        IF ok == 0 | recs.0 < 1 THEN RETURN
        record = recs.1
    END

    _queue = VRMethod('CN_PRINTERS', 'GetFieldData', record, globals.!hcfPrinters.!queue )
    IF _queue <> '' THEN
        CALL RPUOpenView _queue, 'O'

    CALL VRMethod 'CN_PRINTERS', 'SetRecordAttr', record, 'Source', 0
RETURN

/*:VRX         PrinterProperties
*/
PrinterProperties: PROCEDURE EXPOSE globals.
    PARSE ARG record
    IF record == '' THEN DO
        ok = VRMethod( "CN_PRINTERS", "GetRecordList", "Selected", "recs." )
        IF ok == 0 | recs.0 < 1 THEN RETURN
        record = recs.1
    END

    _queue = VRMethod('CN_PRINTERS', 'GetFieldData', record, globals.!hcfPrinters.!queue )
    IF _queue <> '' THEN
        CALL RPUOpenView _queue, 'S'

    CALL VRMethod 'CN_PRINTERS', 'SetRecordAttr', record, 'Source', 0
RETURN

/*:VRX         PrinterSetDefault
*/
PrinterSetDefault: PROCEDURE EXPOSE globals.
    PARSE ARG record
    IF record == '' THEN DO
        ok = VRMethod( "CN_PRINTERS", "GetRecordList", "Selected", "recs." )
        IF ok == 0 | recs.0 < 1 THEN RETURN 0
        record = recs.1
    END
    _queue = VRMethod('CN_PRINTERS', 'GetFieldData', record, globals.!hcfPrinters.!queue )
    ok = RPUQueueDefault( _queue )
/*
    IF ok == 1 THEN DO
        CALL VRSet 'MI_PRINTER_DEFAULT', 'Checked', 1
        _flags = VRMethod('CN_PRINTERS', 'GetFieldData', record, globals.!hcfPrinters.!flags )
        IF POS( _flags, 'D') == 0 THEN DO
            _flags = 'D'_flags
            CALL VRMethod 'CN_PRINTERS', 'SetFieldData', record, globals.!hcfPrinters.!flags, _flags
        END
    END
*/
    CALL VRMethod 'CN_PRINTERS', 'SetRecordAttr', record, 'Source', 0
RETURN ok

/*:VRX         PrinterWizard
*/
PrinterWizard: PROCEDURE EXPOSE globals.
    CALL VRLoadSecondary 'SW_WIZARD', 'N'
RETURN

/*:VRX         Quit
*/
Quit:
    window = VRWindow()
    call VRSet window, "Shutdown", 1
    drop window
return

/*:VRX         RB_ADVANCED_Click
*/
RB_ADVANCED_Click: PROCEDURE
    CALL VRSet 'RB_CREATE_STANDARD', 'Enabled', 0
    CALL VRSet 'RB_CREATE_LAN',      'Enabled', 0
RETURN

/*:VRX         RB_CREATE_CUPS_Click
*/
RB_CREATE_CUPS_Click: PROCEDURE
    CALL VRSet 'RB_CREATE_STANDARD', 'Enabled', 0
    CALL VRSet 'RB_CREATE_LAN',      'Enabled', 0
RETURN

/*:VRX         RB_CREATE_LEGACY_Click
*/
RB_CREATE_LEGACY_Click: PROCEDURE
    CALL VRSet 'RB_CREATE_STANDARD', 'Enabled', 1
    CALL VRSet 'RB_CREATE_LAN',      'Enabled', 1
RETURN

/*:VRX         RB_DEVICE_3RDPARTY_Click
*/
RB_DEVICE_3RDPARTY_Click: PROCEDURE EXPOSE globals. shipped.
    CALL VRSet 'DDCB_DEVICE_3RDPARTY', 'Enabled', 1
    CALL VRSet 'PB_DEVICE_OTHER',      'Enabled', 1
    CALL VRSet 'PB_DEVICE_REFRESH',    'Enabled', 1
    CALL SetDriverList
RETURN

/*:VRX         RB_DEVICE_SHIPPED_Click
*/
RB_DEVICE_SHIPPED_Click: PROCEDURE EXPOSE globals. shipped.
    CALL VRSet 'DDCB_DEVICE_3RDPARTY', 'Enabled', 0
    CALL VRSet 'PB_DEVICE_OTHER',      'Enabled', 0
    CALL VRSet 'PB_DEVICE_REFRESH',    'Enabled', 0
    CALL SetDriverList
RETURN


/*:VRX         RefreshDevices
*/
RefreshDevices: PROCEDURE EXPOSE globals.

    ok = RPUEnumDrivers('drivers.')
    IF rc <> 0 THEN DO
        CALL VRSet 'CN_DEVICES', 'Painting', 0
        CALL VRMethod 'CN_DEVICES', 'RemoveRecord', 'All'
        DO i = 1 TO drivers.0
            PARSE VAR drivers.i _pdrv'.'_model
            IF _pdrv == 'IBMNULL' & _model == '' THEN
                _model = NLSGetMessage( 106 )
            ELSE IF _model == '' THEN
                _model = '-'
            PARSE VALUE VRGetIni('PM_DEVICE_DRIVERS', _pdrv, 'USER') WITH _drvspec '00'x .
            driverinfo.i = ';'drivers.i';'_drvspec';;;'globals.!hcfDevices.!icon';'_drvspec';'globals.!hcfDevices.!model';'_model';'globals.!hcfDevices.!driver';'_pdrv
        END
        driverinfo.0 = drivers.0
        ok = VRMethod( "CN_DEVICES", "AddRecordList",,, "driverinfo.")
        DROP drivers.
        DROP driverinfo.
        CALL VRSet 'CN_DEVICES', 'Painting', 1
    END

RETURN

/*:VRX         RefreshPorts
*/
RefreshPorts: PROCEDURE EXPOSE globals.

    ok = RPUEnumPorts('ports.')
    IF rc <> 0 THEN DO
        CALL VRSet 'CN_PORTS', 'Painting', 0
        CALL VRMethod 'CN_PORTS', 'RemoveRecord', 'All'
        DO i = 1 TO ports.0
            PARSE VAR ports.i _name _driver _path
            SELECT
                WHEN _name == 'FILE'       THEN _icon = '$24,PMWP'
                WHEN _driver == 'USBPRT'   THEN _icon = '$7001,'_path
                WHEN _driver == 'SERIAL'   THEN _icon = '$7001,'_path
                WHEN _driver == 'PARALLEL' THEN _icon = '$7001,'_path
                WHEN _driver == 'SLPR'     THEN _icon = '$7001,'_path
                WHEN _driver == 'LPRPDRVR' THEN _icon = '$256,'_path
/*
                WHEN _driver == 'USBPRT'   THEN _icon = '$7'
                WHEN _driver == 'SMB'      THEN _icon = '$6'
                WHEN _driver == 'SLPR'     THEN _icon = '$6'
                WHEN _driver == 'LPR32'    THEN _icon = '$6'
                WHEN _driver == 'LPRPDRVR' THEN _icon = '$6'
                WHEN _driver == 'CUPS'     THEN _icon = '$10'
                OTHERWISE _icon = '$5'
*/
                OTHERWISE _icon = _path
            END

            portinfo.i = ';'_name';'_icon';;;'globals.!hcfPorts.!icon';'_icon';'globals.!hcfPorts.!name';'_name';'globals.!hcfPorts.!driver';'_driver';'
        END
        portinfo.0 = ports.0
        ok = VRMethod( "CN_PORTS", "AddRecordList",,, "portinfo.")
        DROP ports.
        DROP portinfo.
        CALL VRSet 'CN_PORTS', 'Painting', 1
    END

RETURN

/*:VRX         RefreshPrinters
*/
RefreshPrinters: PROCEDURE EXPOSE globals.
    ok = RPUEnumPrinters('printers.')
    IF ok <> 0 THEN DO
        CALL VRSet 'CN_PRINTERS', 'Painting', 0
        CALL VRMethod 'CN_PRINTERS', 'RemoveRecord', 'All'

        DO i = 1 TO printers.0
            _title = TRANSLATE( printers.i.!description, '  ', '0d0a'x)
            ok = RPUPrinterQuery( printers.i.!name, 'details.')
            IF ok == 0 THEN DO
                _driver = '?'
                _port   = '?'
            END
            ELSE DO
                PARSE VAR details.!drivers _driver ',' .
                _port = details.!port
            END
            IF printers.i.!host <> '' THEN
                _icon = '$65,PMWP'
            ELSE IF _driver <> '?' THEN DO
                PARSE VAR _driver _pdrv'.'_model
                PARSE VALUE VRGetIni('PM_DEVICE_DRIVERS', _pdrv, 'USER') WITH _icon '00'x .
            END
            ELSE _icon = '$2'

            printerinfo.i = ';'printers.i.!description';'_icon';;;' ||,
                                globals.!hcfPrinters.!title';'_title';'              ||,
                                globals.!hcfPrinters.!icon';'_icon';'                ||,
                                globals.!hcfPrinters.!name';'printers.i.!name';'     ||,
                                globals.!hcfPrinters.!queue';'printers.i.!queue';'   ||,
                                globals.!hcfPrinters.!host';'printers.i.!host';'     ||,
                                globals.!hcfPrinters.!port';'_port';'                ||,
                                globals.!hcfPrinters.!driver';'_driver';'            ||,
                                globals.!hcfPrinters.!flags';'printers.i.!flags';'   ||,
                                globals.!hcfPrinters.!handle';'printers.i.!handle';'
        END
        printerinfo.0 = printers.0
        ok = VRMethod( "CN_PRINTERS", "AddRecordList",,, "printerinfo.")
        DROP printers.
        DROP printerinfo.
        DROP details.

        CALL VRSet 'CN_PRINTERS', 'Painting', 1
    END

RETURN

/*:VRX         SetDriverList
*/
SetDriverList: PROCEDURE EXPOSE globals. shipped.
    ARG driver_path

    CALL VRSet VRWindow(), 'Pointer', 'WAIT'
    CALL VRMethod 'CN_DEVICE_LIST', 'RemoveRecord', 'All'

    IF VRGet('RB_DEVICE_SHIPPED', 'Set') == 1 THEN DO
        CALL VRMethod 'CN_DEVICE_LIST', 'AddRecordList',,, 'shipped.'
    END
    ELSE DO
        IF driver_path == '' THEN
            driver_path = TRANSLATE( STRIP( VRGet('DDCB_DEVICE_3RDPARTY', 'Value')))
        IF driver_path <> '' THEN DO
            drivers.0 = 0
            _total = 0
            CALL SysFileTree driver_path'\*.DRV', 'drivers.', 'FO'
            DO i = 1 TO drivers.0
                list.0 = 0
                IF SysGetEA( drivers.i, '.EXPAND', 'eaval') == 0 THEN DO
                    PARSE VAR eaval 3 ealen 5 models
                    offs = 1
                    datalen = C2D( REVERSE( ealen ))
                    DO WHILE offs <= datalen
                        start = SUBSTR( models, offs )
                        inc = POS('00'x, start )
                        IF inc > 1 THEN DO
                            current_name = STRIP( SUBSTR( start, 1, inc-1 ))
                            _total = _total + 1
                            list._total = ';'_desc';#2;;;NAME;'current_name';DRIVER;'|| TRANSLATE( drivers.i ) ||';'
                        END
                        offs = offs + inc
                    END
                END
            END

            list.0 = _total
            CALL VRSortStem 'list.'
            CALL VRMethod 'CN_DEVICE_LIST', 'AddRecordList',,, 'list.'
            CALL VRSet 'DDCB_DEVICE_3RDPARTY', 'UserData', driver_path
        END
    END

    CALL VRSet VRWindow(), 'Pointer', '<default>'

RETURN

/*:VRX         SetLanguage
*/
SetLanguage: PROCEDURE EXPOSE globals.
    PARSE ARG locale

    /*
     * This function locates the proper language files, and uses the message
     * file to sets all UI text.  If the language could not be determined, we
     * default to English.
     */
    execPath = VRGet('Application', 'Program')
    execDir  = VRParseFileName( execPath, 'DP')

    /*
     * First, figure out what language/message file to use.
     */
    IF locale <> '' THEN
        syslanguage = locale
    ELSE
        syslanguage = VALUE('LANG',,'OS2ENVIRONMENT')

    SELECT
        WHEN TRANSLATE( syslanguage ) == 'ZH_TW' THEN nlv = 'tw'
        WHEN TRANSLATE( syslanguage ) == 'ZH_CN' THEN nlv = 'cx'
        OTHERWISE                                PARSE VAR syslanguage nlv '_' .
    END
    nlvfile = globals.!nlsname || nlv
    IF ( STREAM( execDir'\'nlvfile'.msg', 'C', 'QUERY EXISTS') \= '') | ( SysSearchPath('DPATH', nlvfile'.msg') \= '') THEN DO
        globals.!messages = nlvfile'.msg'
        helpfile          = nlvfile'.hlp'
        CALL VRSet 'WN_MAIN', 'HelpFile', helpfile
    END
    ELSE DO
        globals.!messages = globals.!nlsname || 'en.msg'
        helpfile          = globals.!nlsname || 'en.hlp'
        CALL VRSet 'WN_MAIN', 'HelpFile', helpfile
    END

    /*
     * If the message file is missing or unreadable, display an error and then exit.
     */
    IF NLSGetMessage( 1 ) == '' THEN DO
        CALL VRMessage VRWindow(), 'Language file' TRANSLATE( globals.!messages ) 'could not be loaded.', 'Cannot Continue', 'E'
        RETURN 0
    END

    /*
     * Now set the captions for the UI controls on the main window, according
     * to the specified language.
     */
    CALL NLSSetText 'WN_MAIN', 'Caption', 1
    CALL VRMethod 'TDL_MAIN', 'SetTabText', 1, NLSGetMessage( 100 )
    CALL VRMethod 'TDL_MAIN', 'SetTabText', 2, NLSGetMessage( 101 )
    CALL VRMethod 'TDL_MAIN', 'SetTabText', 3, NLSGetMessage( 102 )

    CALL NLSSetText 'MN_PROGRAM', 'Caption', 20
    CALL NLSSetText 'MI_MANAGE',  'Caption', 21
    CALL NLSSetText 'MI_IMPORT',  'Caption', 22
    CALL NLSSetText 'MI_QUIT',    'Caption', 29

    CALL NLSSetText 'MN_PRINTER',         'Caption', 30
    CALL NLSSetText 'MI_PRINTER_OPEN',    'Caption', 31
    CALL NLSSetText 'MI_PRINTER_PROPS',   'Caption', 32
    CALL NLSSetText 'MI_PRINTER_HOLD',    'Caption', 33
    CALL NLSSetText 'MI_PRINTER_DEFAULT', 'Caption', 34
    CALL NLSSetText 'MI_PRINTER_CREATE',  'Caption', 35
    CALL NLSSetText 'MI_PRINTER_DELETE',  'Caption', 36

    CALL NLSSetText 'MN_PORT',        'Caption', 40
    CALL NLSSetText 'MI_PORT_CREATE', 'Caption', 41
    CALL NLSSetText 'MI_PORT_DELETE', 'Caption', 42
    CALL NLSSetText 'MI_PORT_PROPS',  'Caption', 43

    CALL NLSSetText 'MN_DRIVERS',         'Caption', 50
    CALL NLSSetText 'MI_DRIVERS_INSTALL', 'Caption', 51
    CALL NLSSetText 'MI_DRIVERS_REMOVE',  'Caption', 52

    CALL NLSSetText 'MN_VIEW',       'Caption', 60
    CALL NLSSetText 'MI_DETAILVIEW', 'Caption', 61
    CALL NLSSetText 'MI_ICONVIEW',   'Caption', 62
    CALL NLSSetText 'MI_REFRESH',    'Caption', 63

    CALL NLSSetText 'MN_PRINTERPAK',  'Caption', 70
    CALL NLSSetText 'MI_PPAK_MODELS', 'Caption', 71
    CALL NLSSetText 'MI_PPAK_COPY',   'Caption', 72

    CALL NLSSetText 'MN_HELP',         'Caption', 80
    CALL NLSSetText 'MI_HELP_GENERAL', 'Caption', 81
    CALL NLSSetText 'MI_ABOUT',        'Caption', 89

    CALL VRMethod 'WN_MAIN', 'InstallAccelerators'

RETURN 1

/*:VRX         SW_ABOUT_Close
*/
SW_ABOUT_Close:
    call SW_ABOUT_Fini
return

/*:VRX         SW_ABOUT_Create
*/
SW_ABOUT_Create:
    call SW_ABOUT_Init
return

/*:VRX         SW_ABOUT_Fini
*/
SW_ABOUT_Fini:
    window = VRInfo( "Window" )
    call VRDestroy window
    drop window
return
/*:VRX         SW_ABOUT_Init
*/
SW_ABOUT_Init: PROCEDURE EXPOSE globals.

    IF globals.!is_ecs == 1 THEN
        _platform = 'eComStation'
    ELSE
        _platform = 'PM'

    CALL NLSSetText 'SW_ABOUT',     'Caption', 130
    CALL NLSSetText 'DT_PRODUCT',   'Caption', 131, _platform
    CALL NLSSetText 'DT_VERSION',   'Caption', 132, globals.!version
    CALL NLSSetText 'DT_COPYRIGHT', 'Caption', 133, globals.!copyright
    CALL NLSSetText 'PB_ABOUT_OK',  'Caption', 2

    PARSE VERSION _rexxver
    PARSE VALUE VRVersion('VROBJ.DLL') WITH _vrver .
    PARSE VALUE RPUVersion() WITH _rpuver .
    PARSE VALUE VRXVersion() WITH _vrxver .
    _pr1ver = PRVersion()
    _wptver = WPToolsVersion()
    _bullet = NLSGetMessage( 135 )
    CALL VRSet 'MLE_LICENSES', 'Value', NLSGetMessage( 134, _rexxver ) || '0a'x ,
                                        _bullet 'IBM REXXUTIL' SysUtilVersion() || '0a'x ,
                                        _bullet 'VX-REXX VROBJ' _vrver || '0a'x ,
                                        _bullet 'VX-REXX Extras (VROBJEX)' _vrxver || '0a'x ,
                                        _bullet 'RXPRTUTL' _rpuver || '0a'x ,
                                        _bullet 'WPTOOLS' _wptver || '0a'x ,
                                        _bullet _pr1ver || '0a'x||' ----- '||'0a'x || ,
                                        NLSGetMessage( 137 ) || '0a'x' ----- '||'0a'x || ,
                                        NLSGetMessage( 138 )

    window = VRInfo( "Object" )
    if( \VRIsChildOf( window, "Notebook" ) ) then do
        call VRMethod window, "CenterWindow"
        call VRSet window, "Visible", 1
        call VRMethod window, "Activate"
    end
    drop window
return

/*:VRX         SW_DEVICE_ADD_Close
*/
SW_DEVICE_ADD_Close:
    call SW_DEVICE_ADD_Fini
return

/*:VRX         SW_DEVICE_ADD_Create
*/
SW_DEVICE_ADD_Create:
    call SW_DEVICE_ADD_Init
return

/*:VRX         SW_DEVICE_ADD_Fini
*/
SW_DEVICE_ADD_Fini:
    window = VRInfo( "Window" )
    call VRDestroy window
    drop window
return
/*:VRX         SW_DEVICE_ADD_Init
*/
SW_DEVICE_ADD_Init: PROCEDURE EXPOSE globals. shipped.

    fld_name   = VRMethod('CN_DEVICE_LIST', 'AddField', 'String', NLSGetMessage( 265 ), 'NAME')
    fld_driver = VRMethod('CN_DEVICE_LIST', 'AddField', 'String', NLSGetMessage( 266 ), 'DRIVER')

    IF globals.!is_ecs == 1 THEN
        _platform = 'eComStation'
    ELSE
        _platform = 'OS/2'
    CALL NLSSetText 'SW_DEVICE_ADD',      'Caption', 260
    CALL NLSSetText 'RB_DEVICE_SHIPPED',  'Caption', 261, _platform
    CALL NLSSetText 'RB_DEVICE_3RDPARTY', 'Caption', 262, _platform
    CALL NLSSetText 'PB_DEVICE_OTHER',    'Caption', 188
    CALL NLSSetText 'PB_DEVICE_REFRESH',  'Caption', 63
    CALL NLSSetText 'PB_DEVICE_INSTALL',  'Caption', 8
    CALL NLSSetText 'PB_DEVICE_CANCEL',   'Caption', 3

    window = VRInfo( "Object" )
    CALL VRSet window, 'Pointer', 'WAIT'

    extras_num = 0
    PARSE UPPER VALUE VRGetIni('PrinterManager', 'LastDriverPath', 'USER') WITH last_path '00'x .
    IF last_path <> '' THEN DO
        extras_num = extras_num + 1
        extras_path.extras_num = last_path
    END
    PARSE UPPER VALUE VRGetIni('PrinterManager', 'ThirdParty', 'USER') WITH known_3p '00'x .
    IF known_3p == '' THEN known_3p = 'ECUPS ECUPS-HP GUTENPRT PSPRINT'
    DO i = 1 TO WORDS( known_3p )
        _basename = WORD( known_3p, i )
        _ppdriver = _basename'.DRV'
        _ppini = VRGetIni('PM_INSTALL', _basename'_DIR', 'USER')
        IF _ppini == '' THEN ITERATE
        PARSE VAR _ppini _pppath '00'x .
        IF VRFileExists( _pppath'\'_ppdriver ) THEN DO
            extras_num = extras_num + 1
            extras_name.extras_num = _ppdriver
            extras_path.extras_num = _pppath
        END
    END
    extras_name.0 = extras_num
    extras_path.0 = extras_num
    CALL VRMethod 'DDCB_DEVICE_3RDPARTY', 'AddStringList', 'extras_path.'
    IF extras_num > 0 THEN CALL VRSet 'DDCB_DEVICE_3RDPARTY', 'Selected', 1

    if( \VRIsChildOf( window, "Notebook" ) ) then do
        call VRMethod window, "CenterWindow"
        call VRSet window, "Visible", 1
        call VRMethod window, "Activate"
    end

    IF GetShippedDrivers() > 0 THEN DO
/*        CALL VRMethod 'CN_DEVICE_LIST', 'AddRecordList',,, 'shipped.' */
    END

    CALL VRSet window, 'Pointer', '<default>'
    drop window

RETURN

/*:VRX         SW_DEVICES_Close
*/
SW_DEVICES_Close:
    call SW_DEVICES_Fini
return

/*:VRX         SW_DEVICES_Create
*/
SW_DEVICES_Create:
    call SW_DEVICES_Init
return

/*:VRX         SW_DEVICES_Fini
*/
SW_DEVICES_Fini:
    window = VRInfo( "Window" )
    call VRDestroy window
    drop window
return
/*:VRX         SW_DEVICES_Init
*/
SW_DEVICES_Init: PROCEDURE EXPOSE globals.

    globals.!hcfDevices.!icon   = VRMethod( "CN_DEVICES", "AddField", "Icon",   "" )
    globals.!hcfDevices.!model  = VRMethod( "CN_DEVICES", "AddField", "String", NLSGetMessage( 111 ))
    globals.!hcfDevices.!driver = VRMethod( "CN_DEVICES", "AddField", "String", NLSGetMessage( 116 ))

    CALL VRSet "CN_DEVICES", "DetailSort", globals.!hcfDevices.!model
    ok = VRMethod( "CN_DEVICES", "SetFieldAttr", globals.!hcfDevices.!icon, "HorizSeparator", "0" )

    CALL NLSSetText 'CN_DEVICES',       'Caption', 104
    CALL NLSSetText 'PB_DEVICE_ADD',    'Caption', 8
    CALL NLSSetText 'PB_DEVICE_REMOVE', 'Caption', 9

    CALL RefreshDevices

    window = VRInfo( "Object" )
    if( \VRIsChildOf( window, "TabbedDialog" ) ) then do
        call VRMethod window, "CenterWindow"
        call VRSet window, "Visible", 1
        call VRMethod window, "Activate"
    end

    CALL VRSet window, 'UserData', 'CN_DEVICES'
    drop window
/*
    globals.!initstate = globals.!initstate + 1
    IF globals.!initstate == 3 THEN CALL VRSet 'TM_REFRESH', 'Enabled', 1
*/
RETURN

/*:VRX         SW_IMPORT_Close
*/
SW_IMPORT_Close:
    call SW_IMPORT_Fini
return

/*:VRX         SW_IMPORT_Create
*/
SW_IMPORT_Create:
    call SW_IMPORT_Init
return

/*:VRX         SW_IMPORT_Fini
*/
SW_IMPORT_Fini:
    window = VRInfo( "Window" )
    call VRDestroy window
    drop window
return
/*:VRX         SW_IMPORT_Init
*/
SW_IMPORT_Init: PROCEDURE EXPOSE globals. new_ppd

    CALL NLSSetText 'SW_IMPORT',       'Caption', 236
    CALL NLSSetText 'DT_IMPORT',       'Caption', 233
    CALL NLSSetText 'RB_IMPORTCUPS',   'Caption', 234
    CALL NLSSetText 'RB_IMPORTLEGACY', 'Caption', 235
    CALL NLSSetText 'PB_IMPORTOK',     'Caption', 2
    CALL NLSSetText 'PB_IMPORTCANCEL', 'Caption', 3

    _count = 0
    IF ( DriverIsInstalled('PSCRIPT') <> '') | ( GetDriverSource('PSCRIPT') <> '') THEN DO
        _count = _count + 1
        psdrv._count = 'PSCRIPT'
    END
    IF ( DriverIsInstalled('PSCRIPT2') <> '') | ( GetDriverSource('PSCRIPT2') <> '') THEN DO
        _count = _count + 1
        psdrv._count = 'PSCRIPT2'
    END
    IF ( DriverIsInstalled('PSPRINT') <> '') | ( GetDriverSource('PSPRINT') <> '') THEN DO
        _count = _count + 1
        psdrv._count = 'PSPRINT'
    END
    IF ( DriverIsInstalled('PSPRINT2') <> '') | ( GetDriverSource('PSPRINT2') <> '') THEN DO
        _count = _count + 1
        psdrv._count = 'PSPRINT2'
    END
    IF ( DriverIsInstalled('ECUPS') <> '') | ( GetDriverSource('ECUPS') <> '') THEN DO
        _count = _count + 1
        psdrv._count = 'ECUPS'
    END
    IF ( DriverIsInstalled('ECUPS-HP') <> '') | ( GetDriverSource('ECUPS-HP') <> '') THEN DO
        _count = _count + 1
        psdrv._count = 'ECUPS-HP'
    END
    IF DriverIsInstalled('GUTENPRT') <> '' THEN DO
        _count = _count + 1
        psdrv._count = 'GUTENPRT'
    END

    IF _count < 1 THEN DO
        IF globals.!hascups == 0 THEN DO
            CALL VRMessage VRWindow(), NLSGetMessage( 237 ), NLSGetMessage( 13 ), 'E'
            CALL SW_IMPORT_Close
        END
        CALL VRSet 'RB_IMPORTCUPS',   'Set',     1
        CALL VRSet 'RB_IMPORTLEGACY', 'Enabled', 0
        CALL VRSet 'DDCB_IMPORTDRV',  'Enabled', 0
    END
    ELSE IF \globals.!hascups THEN DO
        CALL VRSet 'RB_IMPORTCUPS',   'Enabled', 0
    END

    psdrv.0 = _count
    CALL VRMethod 'DDCB_IMPORTDRV', 'AddStringList', 'psdrv.'
    CALL VRSet 'DDCB_IMPORTDRV', 'Selected', 1

    window = VRInfo( "Object" )
    if( \VRIsChildOf( window, "Notebook" ) ) then do
        call VRMethod window, "CenterWindow"
        call VRSet window, "Visible", 1
        call VRMethod window, "Activate"
    end
    drop window
RETURN

/*:VRX         SW_MODELS_Close
*/
SW_MODELS_Close:
    call SW_MODELS_Fini
return

/*:VRX         SW_MODELS_Create
*/
SW_MODELS_Create:
    call SW_MODELS_Init
return

/*:VRX         SW_MODELS_Fini
*/
SW_MODELS_Fini:
    window = VRInfo( "Window" )
    call VRDestroy window
    drop window
return
/*:VRX         SW_MODELS_Init
*/
SW_MODELS_Init: PROCEDURE EXPOSE globals.

    ok = VRMethod( "CN_PRINTERPAKS", "GetRecordList", "Selected", "recs." )
    IF ok == 0 | recs.0 < 1 THEN RETURN
    record = recs.1

    _name = VRMethod('CN_PRINTERPAKS', 'GetFieldData', record, globals.!hcfPaks.!name )
    _path = VRMethod('CN_PRINTERPAKS', 'GetFieldData', record, globals.!hcfPaks.!path )
    _ver  = VRMethod('CN_PRINTERPAKS', 'GetFieldData', record, globals.!hcfPaks.!version )
    IF _ver == '-' THEN _ver = '??'
    driver = _path'\'_name

    CALL NLSSetText 'SW_MODELS', 'Caption', 194
    CALL NLSSetText 'PB_MODELS_CLOSE', 'Caption', 12

    CALL VRSet 'WN_MAIN', 'Pointer', 'WAIT'
    ok = RPUEnumModels( driver, 'models.')
    IF ok == 1 THEN
        CALL VRMethod 'LB_MODELS', 'AddStringList', 'models.'
    CALL VRSet 'WN_MAIN', 'Pointer', '<default>'

    CALL NLSSetText 'DT_MODELS', 'Caption', 195, _name, models.0

    window = VRInfo( "Object" )
    if( \VRIsChildOf( window, "Notebook" ) ) then do
        call VRMethod window, "CenterWindow"
        call VRSet window, "Visible", 1
        call VRMethod window, "Activate"
    end
    drop window

RETURN

/*:VRX         SW_PORT_ADD_Close
*/
SW_PORT_ADD_Close:
    call SW_PORT_ADD_Fini
return

/*:VRX         SW_PORT_ADD_Create
*/
SW_PORT_ADD_Create:
    call SW_PORT_ADD_Init
return

/*:VRX         SW_PORT_ADD_Fini
*/
SW_PORT_ADD_Fini:
    window = VRInfo( "Window" )
    call VRDestroy window
    drop window
return
/*:VRX         SW_PORT_ADD_Init
*/
SW_PORT_ADD_Init: PROCEDURE EXPOSE globals. updated

    fld_name = VRMethod('CN_ADDPORT', 'AddField', 'String',, 'NAME')
    fld_path = VRMethod('CN_ADDPORT', 'AddField', 'String',, 'PATH')

    CALL NLSSetText 'SW_PORT_ADD',  'Caption', 160
    CALL NLSSetText 'DT_ADDPORT',   'Caption', 161
    CALL NLSSetText 'CN_ADDPORT',   'Caption', 162
    CALL NLSSetText 'DT_OTHERPORT', 'Caption', 163
    CALL NLSSetText 'PB_OTHERPORT', 'Caption', 164

    CALL NLSSetText 'PB_ADDPORT_OK',     'Caption', 2
    CALL NLSSetText 'PB_ADDPORT_CANCEL', 'Caption', 3

    IF GetPortDrivers() > 0 THEN DO
        objects.0 = portdrivers.0
        DO i = 1 TO portdrivers.0
            PARSE VAR portdrivers.i _name _path
            SELECT
                WHEN _name == 'FILE'     THEN _icon = '$24,PMWP'
                WHEN _name == 'USBPRT'   THEN _icon = '$7001,'_path
                WHEN _name == 'SERIAL'   THEN _icon = '$7001,'_path
                WHEN _name == 'PARALLEL' THEN _icon = '$7001,'_path
                WHEN _name == 'SLPR'     THEN _icon = '$7001,'_path
                WHEN _name == 'LPRPDRVR' THEN _icon = '$256,'_path
                OTHERWISE _icon = _path
            END
            objects.i = ';'_name';'_icon';;;'fld_name';'_name';'fld_path';'_path
        END
        CALL VRMethod 'CN_ADDPORT', 'AddRecordList',,, 'objects.'
    END

    window = VRInfo( "Object" )
    if( \VRIsChildOf( window, "Notebook" ) ) then do
        call VRMethod window, "CenterWindow"
        call VRSet window, "Visible", 1
        call VRMethod window, "Activate"
    end
    drop window
RETURN

/*:VRX         SW_PORTS_Close
*/
SW_PORTS_Close:
    call SW_PORTS_Fini
return

/*:VRX         SW_PORTS_Create
*/
SW_PORTS_Create:
    call SW_PORTS_Init
return

/*:VRX         SW_PORTS_Fini
*/
SW_PORTS_Fini:
    window = VRInfo( "Window" )
    call VRDestroy window
    drop window
return
/*:VRX         SW_PORTS_Init
*/
SW_PORTS_Init: PROCEDURE EXPOSE globals.

    globals.!hcfPorts.!icon   = VRMethod( "CN_PORTS", "AddField", "Icon",   "" )
    globals.!hcfPorts.!name   = VRMethod( "CN_PORTS", "AddField", "String", NLSGetMessage( 111 ))
    globals.!hcfPorts.!driver = VRMethod( "CN_PORTS", "AddField", "String", NLSGetMessage( 119 ))

    CALL VRSet "CN_PORTS", "DetailSort", globals.!hcfPorts.!name
    ok = VRMethod( "CN_PORTS", "SetFieldAttr", globals.!hcfPorts.!icon, "HorizSeparator", "0" )

    CALL NLSSetText 'CN_PORTS',       'Caption', 105
    CALL NLSSetText 'PB_PORT_ADD',    'Caption', 8
    CALL NLSSetText 'PB_PORT_DELETE', 'Caption', 9
    CALL NLSSetText 'PB_PORT_EDIT',   'Caption', 10

    CALL RefreshPorts

    window = VRInfo( "Object" )
    if( \VRIsChildOf( window, "TabbedDialog" ) ) then do
        call VRMethod window, "CenterWindow"
        call VRSet window, "Visible", 1
        call VRMethod window, "Activate"
    end

    CALL VRSet window, 'UserData', 'CN_PORTS'
    drop window
/*
    globals.!initstate = globals.!initstate + 1
    IF globals.!initstate == 3 THEN CALL VRSet 'TM_REFRESH', 'Enabled', 1
*/
RETURN

/*:VRX         SW_PRINTERPAKS_Close
*/
SW_PRINTERPAKS_Close:
    call SW_PRINTERPAKS_Fini
return

/*:VRX         SW_PRINTERPAKS_Create
*/
SW_PRINTERPAKS_Create:
    call SW_PRINTERPAKS_Init
return

/*:VRX         SW_PRINTERPAKS_Fini
*/
SW_PRINTERPAKS_Fini:
    window = VRInfo( "Window" )
    call VRDestroy window
    drop window
return
/*:VRX         SW_PRINTERPAKS_Init
*/
SW_PRINTERPAKS_Init: PROCEDURE EXPOSE globals.

    globals.!hcfPaks.!icon   = VRMethod( "CN_PRINTERPAKS", "AddField", "Icon",   "" )
    globals.!hcfPaks.!name   = VRMethod( "CN_PRINTERPAKS", "AddField", "String", NLSGetMessage( 173 ))
    globals.!hcfPaks.!version= VRMethod( "CN_PRINTERPAKS", "AddField", "String", NLSGetMessage( 181 ))
    globals.!hcfPaks.!status = VRMethod( "CN_PRINTERPAKS", "AddField", "String", NLSGetMessage( 176 ))
    globals.!hcfPaks.!path   = VRMethod( "CN_PRINTERPAKS", "AddField", "String", NLSGetMessage( 175 ))
    globals.!hcfPaks.!desc   = VRMethod( "CN_PRINTERPAKS", "AddField", "String", NLSGetMessage( 174 ))
    globals.!hcfPaks.!flags  = VRMethod( "CN_PRINTERPAKS", "AddField", "String", 'FLAGS')

    CALL VRSet "CN_PRINTERPAKS", "DetailSort", globals.!hcfPaks.!name
    ok = VRMethod( "CN_PRINTERPAKS", "SetFieldAttr", globals.!hcfPaks.!icon,  "HorizSeparator", "0" )
    ok = VRMethod( "CN_PRINTERPAKS", "SetFieldAttr", globals.!hcfPaks.!flags, "Visible", "0" )

    CALL NLSSetText 'SW_PRINTERPAKS', 'Caption', 170
    CALL NLSSetText 'DT_PRINTERPAKS', 'Caption', 171
    CALL NLSSetText 'PB_PPUPDATE',    'Caption', 179
    CALL NLSSetText 'PB_PPCLOSE',     'Caption', 12

    CALL GetPrinterPaks

    window = VRInfo( "Object" )
    if( \VRIsChildOf( window, "TabbedDialog" ) ) then do
        call VRMethod window, "CenterWindow"
        call VRSet window, "Visible", 1
        call VRMethod window, "Activate"
    end
    drop window
return

/*:VRX         SW_PRINTERS_Close
*/
SW_PRINTERS_Close:
    call SW_PRINTERS_Fini
return

/*:VRX         SW_PRINTERS_Create
*/
SW_PRINTERS_Create:
    call SW_PRINTERS_Init
return

/*:VRX         SW_PRINTERS_Fini
*/
SW_PRINTERS_Fini:
    window = VRInfo( "Window" )
    call VRDestroy window
    drop window
return
/*:VRX         SW_PRINTERS_Init
*/
SW_PRINTERS_Init: PROCEDURE EXPOSE globals.

    CALL NLSSetText 'CN_PRINTERS',     'Caption', 103
    CALL NLSSetText 'PB_PRINTER_ADD',  'Caption', 8
    CALL NLSSetText 'PB_PRINTER_DEL',  'Caption', 9
    CALL NLSSetText 'PB_PRINTER_OPEN', 'Caption', 7
    CALL NLSSetText 'PB_PRINTER_EDIT', 'Caption', 10

    globals.!hcfPrinters.!icon   = VRMethod( "CN_PRINTERS", "AddField", "Icon", "" )
    globals.!hcfPrinters.!title  = VRMethod( "CN_PRINTERS", "AddField", "String", NLSGetMessage( 110 ))
    globals.!hcfPrinters.!name   = VRMethod( "CN_PRINTERS", "AddField", "String", NLSGetMessage( 112 ))
    globals.!hcfPrinters.!queue  = VRMethod( "CN_PRINTERS", "AddField", "String", NLSGetMessage( 113 ))
    globals.!hcfPrinters.!host   = VRMethod( "CN_PRINTERS", "AddField", "String", NLSGetMessage( 115 ))
    globals.!hcfPrinters.!driver = VRMethod( "CN_PRINTERS", "AddField", "String", NLSGetMessage( 116 ))
    globals.!hcfPrinters.!port   = VRMethod( "CN_PRINTERS", "AddField", "String", NLSGetMessage( 117 ))
    globals.!hcfPrinters.!flags  = VRMethod( "CN_PRINTERS", "AddField", "String", NLSGetMessage( 118 ))
    globals.!hcfPrinters.!handle = VRMethod( "CN_PRINTERS", "AddField", "String", "HANDLE" )

    CALL VRSet 'CN_PRINTERS', 'DetailSort', globals.!hcfPrinters.!title
    CALL VRSet 'CN_PRINTERS', 'LastSplitField', globals.!hcfPrinters.!host
    CALL VRSet 'CN_PRINTERS', 'SplitBarLeft', 3000

    ok = VRMethod( "CN_PRINTERS", "SetFieldAttr", globals.!hcfPrinters.!icon, "HorizSeparator", "0" )
    ok = VRMethod( "CN_PRINTERS", "SetFieldAttr", globals.!hcfPrinters.!handle, "Visible", "0" )
/*    ok = VRMethod( "CN_PRINTERS", "SetFieldAttr", globals.!hcfPrinters.!flags,  "Visible", "0" ) */

    CALL RefreshPrinters

    window = VRInfo( "Object" )
    if( \VRIsChildOf( window, "TabbedDialog" ) ) then do
        call VRMethod window, "CenterWindow"
        call VRSet window, "Visible", 1
        call VRMethod window, "Activate"
    end

    CALL VRSet window, 'UserData', 'CN_PRINTERS'
    drop window
/*
    globals.!initstate = globals.!initstate + 1
    IF globals.!initstate == 3 THEN CALL VRSet 'TM_REFRESH', 'Enabled', 1
*/

RETURN

/*:VRX         SW_PRINTERS_Resize
*/
SW_PRINTERS_Resize:

return

/*:VRX         SW_WIZARD_Close
*/
SW_WIZARD_Close:
    call SW_WIZARD_Fini
return

/*:VRX         SW_WIZARD_Create
*/
SW_WIZARD_Create:
    call SW_WIZARD_Init
return

/*:VRX         SW_WIZARD_Fini
*/
SW_WIZARD_Fini:
    window = VRInfo( "Window" )
    call VRDestroy window
    drop window

    IF globals.!wizard THEN CALL Quit
    ELSE DO
        call VRSet 'WN_MAIN', "Visible", 1
        IF globals.!create_mode == '' THEN DO
            call VRMethod 'WN_MAIN', "Activate"
            CALL VRMethod 'TDL_MAIN', 'SetFocus'
        END
    END
RETURN
/*:VRX         SW_WIZARD_Init
*/
SW_WIZARD_Init:

    CALL NLSSetText 'SW_WIZARD',          'Caption', 140
    IF globals.!wizard THEN
        CALL NLSSetText 'DT_WIZARD',      'Caption', 142
    ELSE
        CALL NLSSetText 'DT_WIZARD',      'Caption', 141
    CALL NLSSetText 'RB_CREATE_CUPS',     'Caption', 143
    CALL NLSSetText 'RB_CREATE_LEGACY',   'Caption', 144
    CALL NLSSetText 'RB_CREATE_STANDARD', 'Caption', 145
    CALL NLSSetText 'RB_CREATE_LAN',      'Caption', 146
    CALL NLSSetText 'RB_ADVANCED',        'Caption', 147
    CALL NLSSetText 'PB_WIZARD_NEXT',     'Caption', 11
    CALL NLSSetText 'PB_WIZARD_CANCEL',   'Caption', 3
    CALL NLSSetText 'PB_WIZARD_HELP',     'Caption', 4

    IF globals.!wizard THEN CALL VRSet 'RB_ADVANCED', 'Visible', 1
    window = VRInfo( "Object" )
    if( \VRIsChildOf( window, "Notebook" ) ) then do
        call VRMethod window, "CenterWindow"
        call VRSet window, "Visible", 1
        call VRMethod window, "Activate"
    end
    drop window

    globals.!create_mode = ''

RETURN

/*:VRX         TDL_MAIN_PageSelected
*/
TDL_MAIN_PageSelected: PROCEDURE EXPOSE globals.
    _pageidx = VRInfo("Page")
    _pagewin = VRMethod( "TDL_MAIN", "GetPageWindow", _pageidx )
    _cnr = VRGet( _pagewin, "UserData")
    IF _cnr == '' THEN RETURN

    _view = VRGet( _cnr, "View")
    IF _view == 'Detail' THEN DO
        CALL VRSet 'MI_ICONVIEW',   'Checked', 0
        CALL VRSet 'MI_DETAILVIEW', 'Checked', 1
    END
    ELSE DO
        CALL VRSet 'MI_ICONVIEW',   'Checked', 1
        CALL VRSet 'MI_DETAILVIEW', 'Checked', 0
    END

    SELECT
        WHEN _cnr == 'CN_PRINTERS' THEN DO
        END
        WHEN _cnr == 'CN_PORTS' THEN DO
        END
        WHEN _cnr == 'CN_DEVICES' THEN DO
        END
        OTHERWISE DO
        END
    END

RETURN

/*:VRX         WN_MAIN_Close
*/
WN_MAIN_Close:
    call Quit
return

/*:VRX         xxUpdatePrDesc
*/
xxUpdatePrDesc: PROCEDURE EXPOSE globals.
    ARG driver, fqn

    IF globals.!prdesc == '' THEN RETURN 1

    ok = RPUEnumModels( fqn, 'newdevs.')
    IF ok == 0 THEN RETURN 2

    _count = 0
    CALL LINEIN globals.!prdesc, 1, 0
    DO WHILE LINES( globals.!prdesc )
        _next = LINEIN( globals.!prdesc )
        PARSE UPPER VAR _next WITH . ':' . '('_prdrv')' .
        IF _prdrv == driver THEN ITERATE
        _count = _count + 1
        prdefs.count = _next
    END
    CALL STREAM prdesc, 'C', 'CLOSE'

    DO i = 1 TO devs.0
        _count = _count + 1
        prdefs._count = newdevs.i':' newdevs.i '('driver')'
    END
    prdefs.0 = count

    CALL VRSortStem 'prdefs.'

    _prdir = VRParseFileName( globals.!prdesc, 'DP')
    CALL VRCopyFile globals.!prdesc, _prdir'\PRDESC.BAK'
    CALL VRDeleteFile globals.!prdesc
    DO i = 1 TO prdefs.0
        CALL LINEOUT globals.!prdesc, prdefs.i
    END
    CALL LINEOUT globals.!prdesc

RETURN 0

