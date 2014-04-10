rem Indent all KQ files as we like them. Requires Gnu Indent
rem See http://gnuwin32.sourceforge.net/
rem Put indent (or indent.exe) somewhere in your search path

set INDENT_PROFILE=other\indent.pro
indent src\*.c include\*.h
