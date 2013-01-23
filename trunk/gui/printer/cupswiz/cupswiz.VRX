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
/*:VRX         CHK_CREATEPM_Click
*/
CHK_CREATEPM_Click: PROCEDURE
    set = VRGet( "CHK_CREATEPM", "Set" )
    CALL VRSet 'DT_PRESDRV',   'Enabled', set
    CALL VRSet 'DDCB_PRESDRV', 'Enabled', set
RETURN

/*:VRX         ConfirmAndCreate
*/
ConfirmAndCreate: PROCEDURE EXPOSE globals.

    globals.!create = 0
    CALL VRLoadSecondary 'SW_CREATE', 'W'
    IF globals.!create <> 1 THEN RETURN

    CALL SetPage4
    CALL VRSet 'WN_MAIN', 'Pointer', 'WAIT'
    CALL NLSSetText 'DT_INFO', 'Caption', 50                                /* 50: Creating printer... */
    ok = CreatePrinter( globals.!os2printer )

    CALL VRSet 'DT_INFO', 'Caption', ''
    CALL VRSet 'WN_MAIN', 'Pointer', '<default>'

    IF ok <> 0 THEN DO
        SELECT
            WHEN ok == 1 THEN reason = NLSGetMessage( 70 )                  /* 70: Error importing PPD file. */
            WHEN ok == 2 THEN reason = NLSGetMessage( 71, globals.!log1 )   /* 71: Error creating CUPS printer.  See %1 for more information. */
            WHEN ok == 3 THEN reason = NLSGetMessage( 72, globals.!log1 )   /* 72: Error creating printer object.  See %1 for more information. */
            OTHERWISE reason = NLSGetMessage( 73)                           /* 73: Unknown error. */
        END
        CALL VRMessage VRWindow(), NLSGetMessage( 60 ) ||,                  /* 60: The following error occurred when trying to create the printer: */
                       '0d0a0d0a'x || reason, NLSGetMessage( 56 ), 'E'      /* 56: Error Creating Printer */
        CALL NLSSetText 'DT_INFO', 'Caption', 52                            /* 52: The printer could not be created. */
        CALL NLSSetText 'PB_NEXT', 'Caption', 54                            /* 54: Return */
    END
    ELSE DO
        CALL NLSSetText 'DT_INFO', 'Caption', NLSGetMessage( 51 )           /* 51: The printer has been created. */
        CALL NLSSetText 'PB_NEXT', 'Caption', NLSGetMessage( 53 )           /* 53: Create another */
    END
    CALL NLSSetText 'PB_CANCEL', 'Caption', NLSGetMessage( 55 )             /* 55: Close */

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
    ELSE IF globals.!mode == 2 THEN
        cups_cmd = cups_cmd '-v "'globals.!prt_port'" -P "'globals.!prt_ppd'"'
    ELSE DO
        PARSE VAR globals.!prt_ppd (globals.!cupsdir)'\share\cups\model\' model
        IF model == '' THEN model = globals.!prt_ppd
        cups_cmd = cups_cmd '-v "'globals.!prt_port'" -m "'model'"'
    END

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

    /* Determine the printer driver source directories/files.  Yes, this is
     * repeated from ImportPPD, but we have to do it again here because
     * (a) we might not have gone through ImportPPD to get to this point, and
     * (b) even if we did, the repository information might have changed.
     */
    driver_path = GetDriverSource( prnt_drv )
    IF driver_path == '' THEN
        /* Driver was not found in the repository.  Check for a previously-
         * installed copy under \OS2\DLL.
         */
        driver_path = STREAM( globals.!os2dir'\DLL\'prnt_drv'\'prnt_drv'.DRV', 'C', 'QUERY EXISTS')

    IF driver_path == '' THEN DO
        /* TODO allow the user to browse for the driver on their system */
        CALL VRMessage VRWindow(),,
                       NLSGetMessage( 61, prnt_drv, globals.!os2dir'\DLL\'prnt_drv ),,    /* 61: The printer driver %1 could not be located.  If you have the driver files, please copy them to the directory %2 and then try again. */
                       NLSGetMessage( 57 ), 'E'                                           /* 57: Driver Not Found */
        RETURN 1
    END

    /* Create a new CUPS port. */
    /* TODO currently this assumes that CUPS.PDR is already installed. */
    port_ok = AddPort_CUPS( port_name, host, printer )
    IF port_ok > 1 THEN
        RETURN 1

    /* New logic to create the printer ourselves (instead of using RINSTPRN).
     * There are two necessary steps: make sure the driver is installed, and
     * then create the printer object (which causes the underlying device and
     * queue to be created automatically as well).
     */

    result = InstallPrintDriver( prnt_drv, driver_path, printer_model )
    CALL LINEOUT globals.!log1, 'InstallPrintDriver(' prnt_drv',' driver_path',' printer_model ') RC =' result

    IF result == 0 THEN
        result = CreatePrinterObject( prnt_drv, printer_model,,
                                      port_name, queue_name, printer_title )
    CALL LINEOUT globals.!log1, 'CreatePrinterObject(' prnt_drv',' printer_model,
                                ',' port_name',' queue_name',' printer_title,
                                ') RC =' result

    IF result <> 0 THEN
        CALL DeletePort port_name
    ELSE IF port_ok == 1 THEN DO
        CALL VRMessage VRWindow(),,
                       NLSGetMessage( 62 ),,        /* 62: The installed version of CUPS.PDR appears to be out of date.  As a result, the desktop printer will not be usable until the desktop is restarted. */
                       NLSGetMessage( 58 ), 'W'     /* 58: Port Driver Problem */ 
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
    IF create_os2 == 1 & globals.!os2driver <> '' THEN DO

        /* Make sure the presentation driver supports the printer */
        IF globals.!mode == 2 THEN DO
            /* Always (re)import when a PPD is provided by the user
             */
            ok = ImportPPD( globals.!os2driver, globals.!prt_ppd )
            IF ok <> 0 THEN DO
                CALL LINEOUT globals.!log1, 'PPD import failed:' ok
                CALL VRMessage VRWindow(), NLSGetMessage( 69 ),,        /* 69: The printer parameters could not ... PPD file. */
                               NLSGetMessage( 70 ), 'W'                 /* 70: Error importing PPD file */
                CALL PromptForPMName
                /* RETURN 1                RC=1  PPD import failed **/
            END
        END
        ELSE IF PrinterExistsInDRV( globals.!os2driver, globals.!prt_nick ) == 0 THEN DO
            cups_ppd = globals.!prt_ppd
            IF cups_ppd == '' THEN
                cups_ppd = STREAM( globals.!cupsdir'\etc\cups\ppd'globals.!prt_name'.ppd', 'C', 'QUERY EXISTS')
            IF cups_ppd == '' THEN
                CALL PromptForPMName
            ELSE DO
                ok = ImportPPD(globals.!os2driver, cups_ppd )
                IF ok <> 0 THEN DO
                    CALL LINEOUT globals.!log1, 'PPD import failed:' ok
                    CALL VRMessage VRWindow(), NLSGetMessage( 69 ),  NLSGetMessage( 70 ), 'W'
                    CALL PromptForPMName
                    /* RETURN 1                RC=1  PPD import failed **/
                END
            END
        END

        ok = CreateOS2Printer( globals.!os2driver )
        IF ok <> 0 THEN
            RETURN 3                    /** RC=3  Error running rinstprn **/
    END

RETURN 0

/*:VRX         DDCB_PROTOCOL_Change
*/
DDCB_PROTOCOL_Change: PROCEDURE EXPOSE globals.

    idx = VRGet('DDCB_PROTOCOL', 'Selected')
    which = VRMethod('DDCB_PROTOCOL', 'GetItemData', idx )
    SELECT
        WHEN which == 1 THEN DO     /* IPP */
            show_queue = 1
            show_user  = 1
            show_pass  = 1
            server_caption = 82          /* 82: Printer or server address: */
            queue_caption  = 84          /* 84: Printer queue name: */
            userid_caption = 86          /* 86: User ID: */
        END
        WHEN which == 2 THEN DO     /* SOCKET */
            show_queue = 0
            show_user  = 0
            show_pass  = 0
            server_caption = 82          /* 82: Printer or server address: */
            queue_caption  = 84          /* 84: Printer queue name: */
            userid_caption = 86          /* 86: User ID: */
        END
        WHEN which == 3 THEN DO     /* LPD */
            show_queue = 1
            show_user  = 1
            show_pass  = 0
            server_caption = 82          /* 82: Printer or server address: */
            queue_caption  = 84          /* 84: Printer queue name: */
            userid_caption = 87          /* 87: User ID (if required): */
        END
        WHEN which == 4 THEN DO     /* SMB */
            show_queue = 1
            show_user  = 1
            show_pass  = 1
            server_caption = 83          /* 83: Print server name: */
            queue_caption  = 85          /* 85: Shared printer name: */
            userid_caption = 86          /* 86: User ID: */
        END
        OTHERWISE DO                /* CUPS */
            show_queue = 1
            show_user  = 0
            show_pass  = 0
            server_caption = 88          /* 88: CUPS server name: */
            queue_caption  = 89          /* 89: CUPS printer name: */
            userid_caption = 86          /* 86: User ID: */
        END
    END

    r = NLSSetText('DT_SERVER', 'Caption', server_caption )
    r = NLSSetText('DT_QUEUE',  'Caption', queue_caption )
    r = NLSSetText('DT_USERID', 'Caption', userid_caption )

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

    CALL NLSSetText 'DT_INFO', 'Caption', 30        /* 30: Looking for connected printers.  Please wait... */

    lpinfo_cmd = globals.!cupsdir'\sbin\lpinfo.exe -v |rxqueue'
    snmp_cmd   = globals.!cupsdir'\lib\cups\backend\snmp.exe |rxqueue'

    _od = DIRECTORY()
    CALL DIRECTORY globals.!cupsdir
    listqueue = RXQUEUE('CREATE')
    defqueue  = RXQUEUE('SET', listqueue )

    /* Use the SNMP backend to find network printers.  We do this in addition
     * to lpinfo because SNMP may provide the actual printer name.  We'll
     * merge this with the output of lpinfo in the next step.
     */
    ADDRESS CMD '@' snmp_cmd listqueue
    DO QUEUED()
        PARSE PULL line
        PARSE VAR line _type _uri '"'_identifier'"' .
        _uri = STRIP( _uri )
        _type = STRIP( _type )
        IF _uri == '' THEN ITERATE
        IF _type <> 'network' THEN ITERATE
        netnames._uri = _identifier
    END

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
            IF _queue <> '' THEN _queue = '(queue '_queue')'
            IF SYMBOL('netnames._name') == 'VAR' THEN
                devices.i = NLSGetMessage( 32, _protocol, netnames._name, _host _queue )    /* 32: Detected network printer (%1) "%2" at %3 */
            ELSE
                devices.i = NLSGetMessage( 33, _protocol, _host _queue )                    /* 33: Detected network printer (%1) at %2 */
        END
        ELSE IF _type <> 'direct' THEN ITERATE
        ELSE DO
            IF _name == 'hp' THEN ITERATE
            i = i + 1
            devices.i = NLSGetMessage( 35, _name )                                          /* 35: Local printer: %1 */
        END
        ports.i = _name
    END
    devices.0 = i

    CALL RXQUEUE 'SET',    defqueue
    CALL RXQUEUE 'DELETE', listqueue
    CALL DIRECTORY _od

    CALL VRSet    'LB_SELECT', 'Painting', 0
    CALL VRMethod 'LB_SELECT', 'AddStringList', 'devices.',, 'ports.'
    CALL VRMethod 'LB_SELECT', 'AddString', NLSGetMessage( 34 ),, ''                        /* 34: Network printer (manual configuration) */
    CALL VRSet    'LB_SELECT', 'Selected', 1
    CALL VRSet    'LB_SELECT', 'Painting', 1

    CALL VRSet 'DT_INFO', 'Caption', NLSGetMessage( 31 )                                    /* 31: Select the connection for this printer. */

RETURN

/*:VRX         GetCupsPrinters
*/
GetCupsPrinters: PROCEDURE EXPOSE globals. manufacturers.

    CALL NLSSetText 'DT_INFO', 'Caption', 20                /* 20: Getting list of supported printers.  Please wait... */

    lpinfo_cmd = globals.!cupsdir'\sbin\lpinfo.exe --exclude-schemes drv -m 2>&1 |rxqueue'

    _od = DIRECTORY()
    CALL DIRECTORY globals.!cupsdir
    listqueue = RXQUEUE('CREATE')
    defqueue  = RXQUEUE('SET', listqueue )

    ADDRESS CMD '@' lpinfo_cmd listqueue
    i = 0
    makers = ''
    DO QUEUED()
        PARSE PULL line
        IF LEFT( line, 6 ) == 'drv://' THEN ITERATE
        IF WORD( line, 1 ) == 'raw' THEN ITERATE
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
            PARSE VAR _name _brand _model
            IF WORDPOS( _brand, makers ) == 0 THEN makers = makers || _brand' '
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

/* Now build the 'manufacturers' stem.
 * This stem takes the following format:
 *    manufacturers.0                         number of manufacturers
 *    manufacturers.i.!name                   manufacturer name (e.g. 'Apple')
 *    manufacturers.i.!printers.0             number of printer models
 *    manufacturers.i.!printers.j.!model      printer model name, short form (e.g. 'LaserWriter 330')
 *    manufacturers.i.!printers.j.!type       driver version/type (e.g. 'Gutenprint v5.2.7')
 *    manufacturers.i.!printers.j.!driver     the internal CUPS printer driver name (PPD or URI)
 *                                            - by preference this is the "simplified" version if available
 *    manufacturers.i.!printers.j.!remark     any distinguishing remarks (e.g. 'Simplified')
 *    manufacturers.i.!printers.j.!driver2    alternate driver in case there is more than one (PPD or URI)
 *                                            - if defined this is usually the non-"simplified" driver
 *    manufacturers.i.!printers.j.!remark2    any distinguishing remarks for the alternate driver
 */
    manufacturers.0 = WORDS( makers )
    DO i = 1 TO manufacturers.0
        manufacturers.i.!name = WORD( makers, i )
        manufacturers.i.!printers.0 = 0
    END

    DO i = 1 TO models.0
        PARSE VAR models.i _brand _model ' - CUPS+' _type
        IF _type == '' THEN DO
            PARSE VAR models.i _brand _model ',' _type
            IF WORD( _type, 1 ) == 'hpcups' THEN
                _type = 'HPLIP' /* SUBWORD( _type, 2 ) */
            ELSE IF ( _type == '') & ( LEFT( ppds.i, 3 ) == 'hp/') THEN
                _type = 'HPLIP'
        END
        man = WORDPOS( _brand, makers )
        IF man == 0 THEN ITERATE

        PARSE VAR _type _version 'Simplified' .
        _version = STRIP( _version )
        _flag    = RIGHT( _type, 10 )

        count = manufacturers.man.!printers.0
        IF count > 0 THEN DO
            /* If there are two or more printers defined with the same model
             * name and driver version, keep the first and last as the regular
             * and alternate drivers; if one if them is 'Simplified', make that
             * the regular driver by preference.
             */
            IF ( TRANSLATE( manufacturers.man.!printers.count.!model ) == TRANSLATE( _model )) THEN DO
                IF ( manufacturers.man.!printers.count.!type == _version ) THEN DO
                    IF ( WORD( _version, 1 ) == 'Gutenprint') & ( _flag == 'Simplified') THEN DO
                        manufacturers.man.!printers.count.!driver2 = manufacturers.man.!printers.count.!driver
                        manufacturers.man.!printers.count.!remark2 = manufacturers.man.!printers.count.!remark
                        manufacturers.man.!printers.count.!driver  = ppds.i
                        manufacturers.man.!printers.count.!remark  = 'Simplified'
                    END
                    ELSE DO
                        manufacturers.man.!printers.count.!driver2 = ppds.i
                        IF _flag == 'Simplified' THEN
                            manufacturers.man.!printers.count.!remark2 = 'Simplified'
                        ELSE
                            manufacturers.man.!printers.count.!remark2 = ''
                    END
                    ITERATE
                END
            END
        END
        count = count + 1
        manufacturers.man.!printers.count.!model   = _model
        manufacturers.man.!printers.count.!type    = _version
        manufacturers.man.!printers.count.!driver  = ppds.i
        IF _flag == 'Simplified' THEN
            manufacturers.man.!printers.count.!remark = 'Simplified'
        ELSE
            manufacturers.man.!printers.count.!remark = ''
        manufacturers.man.!printers.count.!driver2 = ''
        manufacturers.man.!printers.count.!remark2 = ''
        manufacturers.man.!printers.0 = count
    END

    /* Add one more category for special items ...
     */
    count = manufacturers.0 + 1
    manufacturers.count.!name = NLSGetMessage( 22 )                     /* 22: -- Custom -- */
    manufacturers.count.!printers.0 = 1
    manufacturers.count.!printers.1.!model   = NLSGetMessage( 23 )      /* 23: -- Other printer (requires PPD) -- */
    manufacturers.count.!printers.1.!driver  = ''
    manufacturers.count.!printers.1.!type    = ''
    manufacturers.count.!printers.1.!remark  = manufacturers.count.!printers.1.!model
    manufacturers.count.!printers.1.!driver2 = ''
    manufacturers.count.!printers.1.!remark2 = ''
    manufacturers.0 = count

    CALL NLSSetText 'DT_INFO', 'Caption', 21                            /* 21: Select the printer manufacturer and model from the list below. */

RETURN

/*:VRX         Halt
*/
Halt:
    signal _VREHalt
return

/*:VRX         ImportPPD
*/
ImportPPD: PROCEDURE EXPOSE globals.
    /* Import a new PPD file into a PostScript driver.
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
        pbtn.1 = NLSGetMessage( 2 )                     /* 2: OK */
        pbtn.2 = NLSGetMessage( 3 )                     /* 3: Cancel */
        pbtn.0 = 2
        ptext = NLSGetMessage( 63, driver ) ||,         /* 63: The install files for the %1 driver could not be located.  These files will be recreated from the driver files which are already installed on your system. */
                '0d0a0d0a'x || NLSGetMessage( 64 )      /* 64: Please enter the directory where the install files will be placed. */

        PARSE VALUE VRGetIni('PM_INSTALL', driver'_DIR', 'USER') WITH drvr_dir '00'x .
        IF drvr_dir == '' THEN
            PARSE VALUE VRGetIni('InstPDR', 'PATH_TO_'driver, 'USER') WITH drvr_dir '00'x .
        ok = VRPrompt('WN_MAIN', ptext, 'drvr_dir', NLSGetMessage( 59 ), 'pbtn.', 1, 2 )        /* 59: Enter Directory */
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
    ADDRESS CMD '@'ppd_exe ppdfile ppd_use '2>NUL 1| RXQUEUE' nq
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
    ADDRESS CMD '@'pin_exe 'ppd' ppddir drv_out '2>NUL 1| RXQUEUE' nq
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

    /*IF mustcopy == 0 THEN DO*/
    IF pmdx <> '' THEN DO
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

    /* Hide the VX-REXX console window
     */
    CALL VRSet 'Console', 'WindowListTitle', ''

    CALL RxFuncAdd 'SysLoadFuncs', 'REXXUTIL', 'SysLoadFuncs'
    CALL SysLoadFuncs

    CALL LoadSettings
    CALL SetLanguage

    IF InitArgs.0 > 0 THEN globals.!cupsdir = InitArgs.1

    IF \VRIsDir( globals.!cupsdir ) THEN DO
        IF TRANSLATE( globals.!cupsdir ) <> '/R' THEN
            CALL VRMessage VRWindow(), 
                           NLSGetMessage( 65, 'CUPSWIZ F:\CUPS') ||,    /* 65: CUPS could not be located ... on the command line to this program (e.g "%1"). */
                           '0d0a0d0a'x || NLSGetMessage( 66 ),,         /* 66: Only remote CUPS queues ... start this program with the /R parameter.) */
                           NLSGetMessage( 28 ), 'E'                     /* 28: CUPS Path Not Found or Not Valid */
        globals.!cupsdir = ''
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

    manufacturers.0 = 0
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

/*:VRX         LB_BRAND_Click
*/
LB_BRAND_Click: PROCEDURE EXPOSE globals. manufacturers.

    CALL VRSet 'WN_MAIN', 'StatusText', ''

    index = VRGet('LB_BRAND', 'Selected')
    IF index == 0 THEN RETURN

    item = VRMethod('LB_BRAND', 'GetItemData', index )
    IF item == 0 THEN RETURN

    DO i = 1 TO manufacturers.item.!printers.0
        models.i = manufacturers.item.!printers.i.!model
        /* ppds.i   = manufacturers.item.!printers.i.!driver */
        IF manufacturers.item.!printers.i.!driver == '' THEN
            ppds.i = ''
        ELSE
            ppds.i   = i

/* DEBUG
say 'Model:    ' manufacturers.item.!printers.i.!model
say 'Driver:   ' manufacturers.item.!printers.i.!driver
say '          ' manufacturers.item.!printers.i.!longname
if manufacturers.item.!printers.i.!driver2 <> '' then
    say 'Alternate:' manufacturers.item.!printers.i.!driver2
if manufacturers.item.!printers.i.!longname2 <> '' then
    say '          ' manufacturers.item.!printers.i.!longname2
say 'Type:     ' manufacturers.item.!printers.i.!type
say '---'
 */

    END
    models.0 = manufacturers.item.!printers.0
    ppds.0 = manufacturers.item.!printers.0

    CALL VRSet    'LB_SELECT', 'Painting', 0
    CALL VRMethod 'LB_SELECT', 'Clear'
    CALL VRMethod 'LB_SELECT', 'AddStringList', 'models.',, 'ppds.'
    CALL VRSet    'LB_SELECT', 'Painting', 1
/*    CALL VRSet 'LB_SELECT', 'Selected', 1 */

RETURN

/*:VRX         LB_SELECT_Click
*/
LB_SELECT_Click: PROCEDURE EXPOSE globals. manufacturers.
    SELECT
        WHEN globals.!page == 1 THEN DO
            CALL VRSet 'WN_MAIN', 'StatusText', ''

            brand = VRGet('LB_BRAND', 'Selected')
            IF brand == 0 THEN RETURN
            selected = VRGet('LB_SELECT', 'Selected')
            IF selected == 0 THEN RETURN
            which = VRMethod('LB_SELECT', 'GetItemData', selected )
            IF which <> '' THEN DO
                man = VRMethod('LB_BRAND', 'GetItemData', brand )
                IF man == '' THEN RETURN
                make  = manufacturers.man.!name
                model = manufacturers.man.!printers.which.!model
                driver = manufacturers.man.!printers.which.!type
                IF WORD( driver, 1 ) == 'hpcups' THEN driver = 'HPLIP'
                remark = manufacturers.man.!printers.which.!remark
                IF driver == '' THEN
                    type = ''
                ELSE IF remark == '' THEN
                    type = ' ('driver')'
                ELSE
                    type = ' ('driver' - 'remark')'
                CALL VRSet 'WN_MAIN', 'StatusText', make model type
            END
        END

        OTHERWISE NOP
    END

    CALL VRSet 'PB_NEXT', 'Enabled', 1
RETURN

/*:VRX         LoadSettings
*/
LoadSettings: PROCEDURE EXPOSE globals.

    me = VRGet('Application', 'Program')
    IF me == '' THEN PARSE SOURCE . . me
    globals.!mydir = VRParseFilePath( me, 'DP')

    /* Get CUPS paths.
     */
    PARSE VALUE VRGetIni('eCups', 'CUPS', 'USER') WITH cupsdrv '00'x
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

    globals.!log1 = globals.!logdir'\cupswiz.l1'
    globals.!log2 = globals.!logdir'\cupswiz.l2'

    /* Get printer-related paths.
     */
    PARSE VALUE VRGetIni('PM_INSTALL', 'PDR_DIR', 'USER') WITH repos_dir '00'x .
    globals.!repository = repos_dir
    globals.!prdrv      = STREAM( globals.!os2dir'\install\prdrv.lst',  'C', 'QUERY EXISTS')
    globals.!prdesc     = STREAM( globals.!os2dir'\install\prdesc.lst', 'C', 'QUERY EXISTS')

    /* Set the language file name.
     */
    globals.!nlsfile = 'cupswz'

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

/*:VRX         PB_ABOUT_Click
*/
PB_ABOUT_Click:
    CALL SW_ABOUT_Close
RETURN

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
    globals.!os2driver  = VRGet('DDCB_PRESDRV', 'SelectedString')
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

    idx = VRGet('DDCB_PROTOCOL', 'Selected')
    which = VRMethod('DDCB_PROTOCOL', 'GetItemData', idx )
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
        CALL VRMessage VRWindow(), NLSGetMessage( 74 ),,        /* 74: Missing required value(s). */
                                   NLSGetMessage( 75 ), 'E'     /* 75: Missing Value(s) */
        RETURN
    END

    port = uri
    CALL SW_NETWORK_Fini

RETURN

/*:VRX         PB_NEXT_Click
*/
PB_NEXT_Click: PROCEDURE EXPOSE globals. manufacturers.

    SELECT
        WHEN globals.!page == 1 THEN DO
            brand = VRGet('LB_BRAND', 'Selected')
            IF brand == 0 THEN RETURN
            selected = VRGet('LB_SELECT', 'Selected')
            IF selected == 0 THEN RETURN

            CALL VRSet 'WN_MAIN', 'StatusText', ''

            which = VRMethod('LB_SELECT', 'GetItemData', selected )
            IF which == '' THEN DO
                globals.!mode = 2   /* Mode 2: user-selected PPD file */
                ppd = VRFileDialog( VRWindow(), NLSGetMessage( 24 ), 'O', '*.ppd')      /* 24: Select PPD */
                IF ppd == '' THEN RETURN
                globals.!prt_ppd  = ppd
                globals.!prt_dev  = ''
                globals.!prt_nick = GetNameFromPPD( ppd )
                IF globals.!prt_nick == '' THEN DO
                    CALL VRMessage VRWindow(), NLSGetMessage( 76, ppd ),,       /* 76: Could not read printer name from %1. */
                                               NLSGetMessage( 77 ), 'E'         /* 77: Invalid PPD */
                    RETURN
                END
            END
            ELSE DO
                man = VRMethod('LB_BRAND', 'GetItemData', brand )
                IF man == '' THEN DO
                    /* TODO display an error?  But this shouldn't be possible... */
                    RETURN
                END
                ppd = manufacturers.man.!printers.which.!driver
                globals.!mode = 1   /* Mode 1: CUPS-included model */
                IF POS('exe://', ppd ) > 0 THEN DO
                    globals.!prt_ppd  = ''
                    globals.!prt_dev  = ppd
                    sel_brand = VRGet('LB_BRAND', 'SelectedString')
                    sel_name = sel_brand VRMethod('LB_SELECT', 'GetString', selected )
                    PARSE VAR sel_name _nick ' - CUPS' .
                    IF _nick == '' THEN
                        globals.!prt_nick = STRIP( sel_name )
                    ELSE
                        globals.!prt_nick = STRIP( _nick )
                END
                ELSE DO
                    globals.!prt_ppd  = TRANSLATE( globals.!cupsdir'/share/cups/model/'ppd, '\', '/')
                    globals.!prt_dev  = ''
                    globals.!prt_nick = GetNameFromPPD( globals.!prt_ppd )
                END
            END
            globals.!remotecups = ''

            CALL InitMessageLog
            IF globals.!mode == 2 THEN
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
                    _errmsg = NLSGetMessage( 42 )                               /* 42: You must enter a description. */
                ELSE
                    _errmsg = NLSGetMessage( 43 )                               /* 43: You must enter a name, a location, and a description. */
                CALL VRMessage VRWindow(), _errmsg, NLSGetMessage( 75 ), 'E'    /* 75: Missing Value(s) */
                RETURN
            END
            IF ( globals.!remotecups == '' &,
                ( POS( LEFT( globals.!prt_name, 1 ),,
                      'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz') == 0 ) |,
                ( VERIFY( globals.!prt_name, '/# ' || '09'x, 'MATCH') <> 0 )) THEN
            DO
                CALL VRMessage VRWindow(), NLSGetMessage( 44 ),,                /* 44: The printer name must start with ... or tab characters. */
                                           NLSGetMessage( 45 ), 'E'             /* 45: Invalid Name */
                RETURN
            END

            /* TODO support other drivers
             */
            IF WORD( globals.!prt_nick, 1 ) == 'Apollo' |,
               WORD( globals.!prt_nick, 1 ) == 'HP' |,
               WORD( globals.!prt_nick, 1 ) == 'Hewlett-Packard' THEN
                globals.!os2driver = 'ECUPS-HP'
            ELSE
                globals.!os2driver = 'ECUPS'

            CALL ConfirmAndCreate
        END

        WHEN globals.!page == 4 THEN DO
            CALL NLSSetText 'PB_NEXT', 'Caption', 5                 /* 5: Next > */
            CALL VRSet 'EF_NAME',     'Value', ''
            CALL VRSet 'EF_LOCATION', 'Value', ''
            CALL VRSet 'EF_DESC',     'Value', ''
            CALL SetPage1
        END

        OTHERWISE NOP
    END

RETURN

/*:VRX         PICT_ICON_Click
*/
PICT_ICON_Click: PROCEDURE EXPOSE globals.
    CALL VRLoadSecondary 'SW_ABOUT', 'W'
RETURN

/*:VRX         PromptForPMName
*/
PromptForPMName: PROCEDURE EXPOSE globals. models. best
    best = MatchPrinterModel( globals.!os2driver, globals.!prt_nick )

    IF models.0 == 0 THEN DO
        /* No similar models were found in the list of supported printers.
         * Just use the generic PS driver.
         */
        CALL VRMessage VRWindow(), NLSGetMessage( 67, globals.!prt_nick, globals.!os2driver ),,     /* 67: The printer "%1" does not ... application support. */
                                   NLSGetMessage( 68 ), 'W'                                         /* 68: Printer Name Not Found */
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
    nlvfile = globals.!nlsfile || nlv
    IF ( STREAM( execDir'\'nlvfile'.msg', 'C', 'QUERY EXISTS') \= '') | ( SysSearchPath('DPATH', nlvfile'.msg') \= '') THEN DO
        globals.!messages = nlvfile'.msg'
        helpfile          = nlvfile'.hlp'
        CALL VRSet 'WN_MAIN', 'HelpFile', helpfile
    END
    ELSE DO
        globals.!messages = globals.!nlsfile || 'en.msg'
        helpfile          = globals.!nlsfile || 'en.hlp'
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
    CALL NLSSetText 'WN_MAIN',    'Caption', 1         /* 1: Create CUPS Printer */
    CALL NLSSetText 'PB_NEXT',    'Caption', 5         /* 5: Next > */
    CALL NLSSetText 'PB_CANCEL',  'Caption', 3         /* 3: Cancel */
    CALL NLSSetText 'PB_REFRESH', 'Caption', 6         /* 6: Refresh */

RETURN 1

/*:VRX         SetPage1
*/
SetPage1: PROCEDURE EXPOSE globals. manufacturers.

    CALL VRMethod 'LB_BRAND', 'Clear'
    CALL VRSet 'PB_NEXT',  'Enabled', 0
    CALL VRSet 'LB_BRAND', 'Visible', 1

    /* Resize LB_SELECT if necessary to make room for LB_BRAND */
    lb_x = VRGet('LB_BRAND', 'Left') + VRGet('LB_BRAND', 'Width') + 60
    lb_w = VRGet('GB_INFO', 'Width') - lb_x + VRGet('GB_INFO', 'Left')
    CALL VRSet 'LB_SELECT', 'Left', lb_x
    CALL VRSet 'LB_SELECT', 'Width', lb_w

    CALL VRSet 'LB_SELECT', 'Visible', 1
    CALL VRSet 'GB_INFO',   'Visible', 0

    globals.!page       = 1
    globals.!prt_ppd    = ''
    globals.!prt_dev    = ''
    globals.!prt_nick   = ''
    globals.!prt_port   = ''
    globals.!remotecups = ''
    globals.!prt_name   = ''
    globals.!prt_loc    = ''
    globals.!prt_info   = ''
    globals.!os2driver  = ''

    CALL VRSet 'WN_MAIN', 'Pointer', 'WAIT'

    IF manufacturers.0 == 0 THEN DO

        /* Populate the manufacturer list */
        IF globals.!cupsdir == '' THEN DO
            manufacturers.1.!name = NLSGetMessage( 22 )                     /* 22: -- Custom -- */
            manufacturers.1.!printers.0 = 1
            manufacturers.1.!printers.1.!model   = NLSGetMessage( 23 )      /* 23: -- Other printer (requires PPD) -- */
            manufacturers.1.!printers.1.!driver  = ''
            manufacturers.1.!printers.1.!type    = ''
            manufacturers.1.!printers.1.!remark  = NLSGetMessage( 23 )      /* 23: -- Other printer (requires PPD) -- */
            manufacturers.1.!printers.1.!driver2 = ''
            manufacturers.1.!printers.1.!remark2 = ''
            manufacturers.0 = 1
        END
        ELSE
            CALL GetCupsPrinters

    END

    CALL VRSet 'LB_BRAND',  'Painting', 0
    DO i = 1 TO manufacturers.0
        addman.i = manufacturers.i.!name
        addidx.i = i
    END
    addman.0 = manufacturers.0
    addidx.0 = manufacturers.0
    CALL VRMethod 'LB_BRAND', 'AddStringList', 'addman.',, 'addidx.'
    CALL VRSet 'LB_BRAND',  'Painting', 1

    CALL VRMethod 'LB_BRAND', 'SetFocus'
    CALL VRSet 'LB_BRAND', 'Selected', 1
    CALL VRSet 'WN_MAIN', 'Pointer', '<default>'

RETURN

/*:VRX         SetPage2
*/
SetPage2: PROCEDURE EXPOSE globals.

    globals.!page = 2
    CALL VRSet 'LB_BRAND', 'Visible', 0
    CALL VRSet 'PB_NEXT',  'Enabled', 0
    CALL NLSSetText 'WN_MAIN', 'StatusText', 29, globals.!prt_nick          /* 29: Selected printer:  %1 */

    /* Resize LB_SELECT to the full width of GB_INFO */
    lb_x = VRGet('GB_INFO', 'Left')
    lb_w = VRGet('GB_INFO', 'Width')
    CALL VRSet 'LB_SELECT', 'Left', lb_x
    CALL VRSet 'LB_SELECT', 'Width', lb_w

    CALL VRMethod 'LB_SELECT', 'Clear'

    CALL VRSet 'WN_MAIN', 'Pointer', 'WAIT'
    IF globals.!cupsdir == '' THEN DO
        CALL VRMethod 'LB_SELECT', 'AddString', NLSGetMessage( 34 ),, ''    /* 34: Network printer (manual configuration) */
        CALL VRSet    'LB_SELECT', 'Selected', 1
    END
    ELSE
        CALL GetCupsPorts
    CALL VRMethod 'LB_SELECT', 'SetFocus'
    CALL VRSet 'WN_MAIN', 'Pointer', '<default>'

RETURN

/*:VRX         SetPage3
*/
SetPage3: PROCEDURE EXPOSE globals.

    globals.!page = 3
    CALL VRSet 'EF_DESC', 'Value', globals.!prt_nick
    CALL NLSSetText DT_NAME,     'Caption', 103             /* 103: Name: */
    CALL NLSSetText DT_LOCATION, 'Caption', 105             /* 105: Location: */
    CALL NLSSetText DT_DESC,     'Caption', 106             /* 106: Description: */

    CALL VRMethod 'LB_SELECT', 'Clear'

    CALL VRSet 'LB_SELECT',   'Visible', 0
    CALL VRSet 'GB_INFO',     'Visible', 1
    IF globals.!remotecups == '' THEN DO
        CALL NLSSetText 'DT_INFO', 'Caption', 40        /* 40: Choose the printer name, and enter its location and a short description. */
        CALL VRSet 'DT_NAME',     'Visible', 1
        CALL VRSet 'EF_NAME',     'Visible', 1
        CALL VRSet 'DT_LOCATION', 'Visible', 1
        CALL VRSet 'EF_LOCATION', 'Visible', 1
    END
    ELSE
        CALL NLSSetText 'DT_INFO', 'Caption', 41        /* 41: Enter a short description of this printer.  This will be used for the printer object that appears on your desktop. */
    CALL VRSet 'DT_DESC',     'Visible', 1
    CALL VRSet 'EF_DESC',     'Visible', 1

    CALL VRMethod 'EF_NAME', 'SetFocus'

RETURN

/*:VRX         SetPage4
*/
SetPage4:

    globals.!page = 4
    CALL VRSet 'LB_SELECT', 'Visible',    0
    CALL VRSet 'GB_INFO',   'Visible',    0
    CALL VRSet 'WN_MAIN',   'StatusText', ''

RETURN

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
SW_ABOUT_Init:

    CALL NLSSetText 'SW_ABOUT',  'Caption', 10                                          /* 10: Product Information */
    CALL NLSSetText 'DT_ABOUT1', 'Caption', 11                                          /* 11: eCups Printer Install Utility */
    CALL NLSSetText 'DT_ABOUT2', 'Caption', 12, '0.98'                                  /* 12: Version %1 */
    CALL NLSSetText 'DT_ABOUT3', 'Caption', 13, '2010,2012'                             /* 13: (C) %1 Alex Taylor */
    CALL NLSSetText 'PB_ABOUT',  'Caption', 2                                           /* 10: Product Information */

    CALL NLSSetText 'DT_BOOTDRIVE',  'Caption', 14, TRANSLATE( globals.!bootdrv )       /* 14: System boot volume: %1 */ 
    CALL NLSSetText 'DT_CUPSPATH',   'Caption', 15, TRANSLATE( globals.!cupsdir )       /* 15: Local CUPS directory: %1 */ 
    CALL NLSSetText 'DT_REPOSITORY', 'Caption', 16, TRANSLATE( globals.!repository )    /* 16: Local driver repository: %1 */

    window = VRInfo( "Object" )
    if( \VRIsChildOf( window, "Notebook" ) ) then do
        call VRMethod window, "CenterWindow"
        call VRSet window, "Visible", 1
        call VRMethod window, "Activate"
    end
    drop window
return

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

    CALL NLSSetText 'SW_CREATE',  'Caption', 115                 /* 115: Confirm Create Printer */
    CALL NLSSetText 'DT_CRMODEL', 'Caption', 102                 /* 102: Model: */
    CALL NLSSetText 'DT_CRLOC',   'Caption', 105                 /* 105: Location: */
    CALL NLSSetText 'DT_CRDESC',  'Caption', 106                 /* 106: Description: */

    CALL NLSSetText 'CHK_CREATEPM', 'Caption',  109              /* 109: Create printer object */
    CALL NLSSetText 'DT_PRESDRV',   'Caption',  110              /* 110: Presentation driver: */
    CALL NLSSetText 'PB_CREATEOK',  'Caption',  111              /* 111: Create */
    CALL NLSSetText 'PB_CREATECANCEL', 'Caption',  3             /* 3: Cancel */
    CALL NLSSetText 'PB_CREATEHELP',   'Caption',  4             /* 4: ~Help */

    CALL VRSet 'EF_CRMODEL', 'Value', globals.!prt_nick

    IF globals.!remotecups <> '' THEN DO
        /* An existing CUPS printer was indicated.  This means we're not
         * creating a CUPS printer, but only a PM printer object (and port)
         * that points to it.
         */
        CALL NLSSetText 'DT_CREATE', 'Caption', 101             /* 101: Ready to create printer object with the following parameters. */

        PARSE VAR globals.!remotecups cups_host cups_printer .
        CALL NLSSetText 'DT_CRNAME', 'Caption', 107             /* 107: CUPS server: */
        CALL NLSSetText 'DT_CRURI',  'Caption', 108             /* 108: CUPS queue:  */
        CALL VRSet 'DT_CRLOC',     'Visible', 0
        CALL VRSet 'EF_CRNAME',    'Value',   cups_host
        CALL VRSet 'EF_CRURI',     'Value',   cups_printer
        CALL VRSet 'EF_CRLOC',     'Visible', 0
        CALL VRSet 'CHK_CREATEPM', 'Visible', 0
    END
    ELSE DO
        CALL NLSSetText 'DT_CREATE', 'Caption', 100             /* 100: Ready to create CUPS printer with the following parameters. */
        CALL NLSSetText 'DT_CRNAME', 'Caption', 103             /* 103: Name: */
        CALL NLSSetText 'DT_CRURI',  'Caption', 104             /* 104: URI: */
        CALL VRSet 'EF_CRNAME', 'Value', globals.!prt_name
        CALL VRSet 'EF_CRURI',  'Value', globals.!prt_port
        CALL VRSet 'EF_CRLOC',  'Value', globals.!prt_loc
    END
    CALL VRSet 'EF_CRDESC', 'Value', globals.!prt_info

    IF QueryAvailableDrivers() > 0 THEN DO
        def_idx = 1
        CALL VRMethod 'DDCB_PRESDRV', 'AddStringList', 'drv_list.'
        DO i = 1 TO drv_list.0
            IF drv_list.i == globals.!os2driver THEN DO
                def_idx = i
                LEAVE
            END
        END
        CALL VRSet 'DDCB_PRESDRV', 'Selected', def_idx
    END
    ELSE DO                                             /* 112: No eCups-compatible OS/2 presentation drivers are installed. */
                                                        /* 113: Please install the ECUPS or ECUPS-HP printer driver before continuing. */
        CALL VRMessage VRWindow(), 
                       NLSGetMessage( 112 ) || '0d0a0d0a'x || NLSGetMessage( 113 ),,  
                       NLSGetMessage( 114 ), 'E'                                        /* 114: Missing PM Driver */
        CALL VRSet 'PB_CREATEOK', 'Enabled', 0
    /* TODO give an error if no drivers were found */
    END

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

    CALL NLSSetText 'SW_MODEL', 'Caption', 120                      /* 120: Select Printer Model */

    /* We should have a list of suggested printer models whose names at least
     * partially match the requested model, sorted in order with the closest
     * match at the top.
     */
    ok = VRMethod( "LB_OS2MODELS", "AddStringList", "models.",  )
    IF best > 0 THEN
        CALL VRSet 'LB_OS2MODELS', 'Selected', best

    CALL NLSSetText 'DT_MODEL1', 'Caption',, 
                    121, globals.!prt_nick, globals.!os2driver      /* 121: The printer "%1" could not ... to applications. */
    CALL NLSSetText 'DT_MODEL2', 'Caption', 122                     /* 122: You can select one of ... use a generic driver. */

    CALL NLSSetText 'PB_MODELOK',     'Caption', 2                  /* 2: OK */
    CALL NLSSetText 'PB_MODELCANCEL', 'Caption', 3                  /* 3: Cancel */

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

    CALL NLSSetText 'SW_NETWORK',   'Caption',  80    /* 80: Network Printer */
    CALL NLSSetText 'DT_PROTOCOL',  'Caption',  81    /* 81: Network protocol: */
    CALL NLSSetText 'DT_PASSWORD',  'Caption',  90    /* 90: Password: */
    CALL NLSSetText 'PB_NETOK',     'Caption',  2     /* 2: OK */
    CALL NLSSetText 'PB_NETCANCEL', 'Caption',  3     /* 3: Cancel */
    CALL NLSSetText 'PB_NETHELP',   'Caption',  4     /* 4: ~Help */

    IF globals.!cupsdir == '' THEN DO
        protos.0 = 1
        protos.1 = NLSGetMessage( 91 )              /* 91: Existing CUPS printer (remote server) */
        pnums.0 = 1
        pnums.1 = 5
    END
    ELSE DO
        protos.0 = 5
        protos.1 = NLSGetMessage( 92 )              /* 92: Internet Printing Protocol (IPP) */
        protos.2 = NLSGetMessage( 93 )              /* 93: AppSocket/JetDirect */
        protos.3 = NLSGetMessage( 94 )              /* 94: Line Printer Remote daemon (LPD) */
        protos.4 = NLSGetMessage( 95 )              /* 95: Windows/SMB network */
        protos.5 = NLSGetMessage( 96 )              /* 96: Existing CUPS printer */
        pnums.0 = 5
        pnums.1 = 1
        pnums.2 = 2
        pnums.3 = 3
        pnums.4 = 4
        pnums.5 = 5
    END

    CALL VRMethod 'DDCB_PROTOCOL', 'AddStringList', 'protos.',, 'pnums.'
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

