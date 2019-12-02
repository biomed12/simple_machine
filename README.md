# Simple machine state machine design framework.

A simple state machine design framework for embedded systems.

You can create your own makefile or simply run the shell command to see the application result:

* $gcc.exe main.c exm/* sm/* -o main
* $./main

* I assumed, gcc toolchain was added path in your machine envs.

You can find the UML StateCharts representation of the implemented machine at the link:

https://i.hizliresim.com/kMPrjJ.png

The machine is cited from the Miro Samek's book, Practical UML StateChart in C and C++, page 88.

Notice: If you use this tiny framework in your projects, please make sure your machine runs 
as you expected in the manner of handling all the types of events. Otherwise, please feedback!
