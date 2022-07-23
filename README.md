# Icemaze Route Finder

This application tries to find the shortest route of the icemaze.

I made this program because I couldn't [solve it](https://twitter.com/bijutsuyarou/status/1549415589608714240).

## How to
```
$ make
cc -Wextra -O3 --std=c99 --pedantic   -c -o main.o main.c
cc -Wextra -O3 --std=c99 --pedantic   -c -o list.o list.c
cc  main.o list.o -o finder
$ ./finder < example.0.txt
Route is Found.

|   Backtrace   |
|~~~~~~~~~~~~~~~|
|X |Y |Type     |
|--|--|---------|
|11|14|Goal     |
|11| 4|Cell     |
| 5| 4|Cell     |
| 5|10|Cell     |
| 3|10|Cell     |
| 3| 8|Cell     |
| 7| 8|Cell     |
| 7|10|Cell     |
|15|10|Cell     |
|15| 6|Cell     |
|14| 6|Cell     |
|14|11|Cell     |
| 4|11|Cell     |
| 4| 1|Start    |
$ ./finder < example.1.txt
Route is not found.
$ 
```

## Additional note
GCC 11.2.0 cause maybe-uninitialized on some line.

If you are very sensitive to warnings, you can use clang. (tested on 14.0.0)

```
$ CC=gcc make
gcc -Wextra -O3 --std=c99 --pedantic   -c -o main.o main.c
main.c: In function ‘walk’:
main.c:76:31: warning: ‘y’ may be used uninitialized in this function [-Wmaybe-uninitialized]
   76 |                 p.x += x, p.y += y;
      |                               ^~
main.c:76:21: warning: ‘x’ may be used uninitialized in this function [-Wmaybe-uninitialized]
   76 |                 p.x += x, p.y += y;
      |                     ^~
gcc -Wextra -O3 --std=c99 --pedantic   -c -o list.o list.c
gcc  main.o list.o -o finder
$ make clean
$ CC=clang make
clang -Wextra -O3 --std=c99 --pedantic   -c -o main.o main.c
clang -Wextra -O3 --std=c99 --pedantic   -c -o list.o list.c
clang  main.o list.o -o finder
$ 
```
