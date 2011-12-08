/**************************************************************/
/* EA2.CMD - a procedure to add and view ea for drivers       */
/*                                                            */
/*                                                            */
/*                                                            */
/**************************************************************/

/**************************************************************/
/* 4/15/95    Rakesh Bhakta                                   */
/**************************************************************/

parse arg Option_line

call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
call SysLoadFuncs

parse value Option_line with opt rest
parse value reverse(Option_line) with filename rest
filename = reverse(filename)
parse value Option_line with (opt) easpec (filename)

if filename <> '' then do
  rc = stream(filename,'c','query exists')
  if rc  =  "" then do
   say  "Error: " filename  "not found"
   return 1
  end
end

select
  when opt = '-e'  then rc = AddEA(filename, easpec)
  when opt = '-d'  then rc = RemoveEA(filename, easpec)
  when opt = '-v'  then rc = DisplayEA(filename, easpec,'false')
  when opt = '-vn' then rc = DisplayEA(filename, easpec,'true' )
  when opt = '-?'  then rc = DisplayHelp()
  when opt = ''    then rc = DisplayHelp()
  otherwise  rc = DisplayPrinterEA(filename)
end
return rc


/***********************************************/
/* AddEA - add the eas specifed by easpc to the*/
/*  file filename (easpec = 'eaname=eavalue')  */
/***********************************************/
AddEA:Procedure
parse arg filename, easpec
  parse value easpec with easpec '=' eavalue
  if pos("'",strip(easpec)) == 1 then
    parse value easpec with "'" easpec "'"
  if pos("'",strip(eavalue)) == 1 then
    parse value eavalue with "'" eavalue "'"
  easpec = strip(easpec)
  eavalue= strip(eavalue)
  rc = SysPutEa(filename,easpec, EAFormat(eavalue) );
return rc


/***********************************************/
/* RemoveEA Remove the eas specified by easpc  */
/***********************************************/
RemoveEA:Procedure
parse arg filename, easpec
  if pos("'",strip(easpec)) == 1 then
    parse value easpec with "'" easpec "'"
  easpec=strip(easpec)
  rc = SysPutEa(filename,easpec,'');
return rc


/***********************************************/
/* DisplayEA - display eas specifed by easpc    */
/***********************************************/
DisplayEA:Procedure
parse arg filename, easpec, fnull
  if pos("'",strip(easpec)) == 1 then
    parse value easpec with "'" easpec "'"
  easpec=strip(easpec)
  rc = SysGetEa(filename,easpec,'eavalue');
  rc = Display(easpec, eavalue, fnull)
return rc


/***********************************************/
/* DisplayPrinterEA - read and display the ea  */
/*  associated with print driver (*.drv)       */
/***********************************************/
DisplayPrinterEA: Procedure
parse arg filename
 ealist="REQUIREDCLASSFILES REQUIREDDRIVERFILES REQUIREDVENDORFILES",
        "OPTIONALCLASSFILES OPTIONALDRIVERFILES OPTIONALVENDORFILES",
        "CLASSNAME VENDORNAME .VERSION .ICON .CHECKSUM"
 do forever
   parse value ealist with easpec ealist
   if easpec = '' then leave
   easpec = strip(easpec)
   rc = SysGetEA(filename,easpec,'value')
   if rc = 0 then rc = Display(easpec,value,'false')
 end
 rc = SysGetEA(filename, '.EXPAND', 'value')      /* show only the first dev */
 if rc <> 0 then return rc
 dlimit = X2C('00')
 parse value value with value (dlimit) rest
 rc=Display('.EXPAND', value,'false')                  /* only show 1 device */
return rc



/***********************************************/
/* Display - display the easpec and value      */
/*  according to type of value                 */
/***********************************************/
Display:Procedure
parse arg easpec, value, fnull
  type = substr(value,1,2)
  select
    when type = 'FEFF'x then                        /* binary data to follow */
     outstr = '<binary>'
    when type = 'FDFF'x then                                   /* ascii data */
     outstr = substr(value,5)
    when type = 'FBFF'x then                                  /* bitmap data */
     outstr = '<bitmap>'
    when type = 'FAFF'x then                                /* metafile data */
     outstr = '<metafile>'
    when type = 'F9FF'x then                                    /* icon data */
     outstr = '<icon>'
    when type = 'FDEE'x then                                   /* ascii data */
     outstr = substr(value,5)
    when type = 'DFFF'x then                         /* mult-value muli-type */
     outstr =  substr(value,11)
    otherwise outstr = value                                      /* default */
  end
  if outstr = '' then return 0
  if fnull = 'false' then
    say '   ' easpec || '=' || outstr
  else do
    say '   ' easpec || '='
    dlimit = X2C('00')
    do forever
      parse value outstr with value1 (dlimit) outstr
      if value1 = '' then leave
      say '           ' value1
    end
  end
return 0


/***********************************************/
/* EAFormat  - return standard ea header for   */
/*   format <type><length>(ea_value)           */
/*   EAT_ASCII(0xFFFD) -length preceeded ASCII */
/***********************************************/
EAFormat: PROCEDURE
 parse arg eavalue
 len =  D2X(length(eavalue),4)
 header  = X2C('FDFF') || X2C(substr(len,3,2)) || X2C(substr(len,1,2))
return header || eavalue

DisplayHelp:Procedure
  say'usage: ea2 [-e name=value | -v name | ] file'
  say'        -e = Edit new/existing EA value.'
  say'        -v = view existing EA value.'
  say'        -d = removed EA.'
  say'        -vn= view existing EA null terminated list'
  say'        -? = Show this help information.'
  say' Otherwise = Display all EAs for print driver.'
  say' Examples:'
  say'   ea2 file.dat'
  say'   ea2 -e language=french   file.dat'
  say'   ea2 -e language=french german  file.dat'
  say'   ea2 -e my pie=apple and cherry   file.dat'
  say'   ea2 -e ''my pie''=''apple and cherry'' file.dat'
  say'   ea2 -v language file.dat'
  say'   ea2 -d language file.dat'
return 0
