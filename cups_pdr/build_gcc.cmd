del cups.o
del cups.pdr
cmd /c makedesc -R"CUPS port driver" -N"netlabs.org" -V"#define=DRIVER_VERSION,cups.h" cups.def
gcc -o cups.o cups.c -c
RC -r cups.RC cups.RES
gcc -Zdll -Zbin-files -Zomf -o cups.pdr cups.o cups.def cups.res
ea2 -e DEFAULT_PORT=CUPS cups.pdr
