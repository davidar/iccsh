**iCCsh** is an interactive C Compiler shell, based upon [TinyCC (TCC)](http://bellard.org/tcc/).

It allows C code to be quickly and
easily evaluated, without the need for manually constructing and compiling an
entire C source file. An example session is shown below:

```c
>>> int i
:declare i int i
>>> for(i = 0; i < 20; i++) \
...     printf("%d ", i); \
... printf("\n");
:eval for(i = 0; i < 20; i++) ...
0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 
>>> :f hello
... void hello(const char *name) {
...     printf("Hello %s!\n", name);
... }
... 
>>> hello("world")
:eval hello("world")
Hello world!
>>> :i math
>>> %f cos(M_PI/4)
:printf %f cos(M_PI/4)
0.707107
```

iCCsh can also interface with native libraries, such as Xlib (X11):

```c
>>> :sandbox off
>>> :i X11/Xlib
>>> :l X11
>>> :d display Display *display
>>> display = XOpenDisplay("")
>>> :d screen Screen *screen
>>> screen = XScreenOfDisplay(display, 0)
>>> :p width=%d\nheight=%d\ndepth=%d \
...     screen->width, screen->height, screen->root_depth
width=1280
height=800
depth=24
>>> XCloseDisplay(display)
```

Run `make && ./iccsh` to compile and execute. Type 'help' within the shell for
usage information. Note that `texi2html` must be installed for TCC to compile.

See also [CINT](https://root.cern.ch/drupal/content/cint) C/C++ interpreter
