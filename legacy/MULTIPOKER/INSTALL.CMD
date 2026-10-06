/* Compulsive Gambler's Toolkit Installation v1.00 */
call RxFuncAdd 'SysCls','RexxUtil','SysCls'

call SysCls	/* Clear the screen */

say "Compulsive Gambler's Toolkit v1.00 Installation"
say "Copyright (C) 1999 IglooSoft"
say ""
say "This will install The Compulsive Gambler's Toolkit onto your system."
say ""
say "Enter the target drive and directory"
say "(Default=C:\Gambler)"
pull Target
if Target="" then Target="C:\Gambler"

Current=DIRECTORY()

/* Check to see if directory already exists */
if \(Target=DIRECTORY(Target)) then
do

    call RxFuncAdd 'SysMkDir','RexxUtil','SysMkDir'

    RetCode=SysMkDir(Target)
    if(RetCode=1) then
    do
        say "Cannot create directory "Target
        exit
    end
end

call DIRECTORY(Current)

/* Now copy files over */
"copy *.exe" Target
"copy *.dll" Target
"copy *.hlp" Target
"copy *.doc" Target
"copy *.cmd" Target

/* Copying complete.  Ask for installation of desktop object */
say "Would you like to install a program object (Y/N)?"
pull Response
if(Response='y' | Response='Y') then
do
    /* Install folder object */
    call RxFuncAdd 'SysCreateObject','RexxUtil','SysCreateObject'

    Crap=SysCreateObject("WPFolder", "Compulsive Gambler's Toolkit", "<WP_DESKTOP>",,
        "OBJECTID=<MYFOLDER>")
    Crap=SysCreateObject("WPProgram","Multi Poker", "<MYFOLDER>",,
          "EXENAME="Target"\MPoker.exe", "replace")
    Crap=SysCreateObject("WPProgram","Caribbean Stud", "<MYFOLDER>",,
        "EXENAME="Target"\CaribStd.exe", "replace")
end
e Gambler.doc
say "Kiss that wallet goodbye!"
exit
