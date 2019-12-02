# Simple machine state machine design framework.

A simple state machine design framework for embedded systems.

You can create your own makefile or simply run the shell command to see the application result:

$gcc.exe main.c exm/* sm/* -o main  
$./main  

-topmost initial -s-entry--s2-entry--s2-init--s21-entry--s211-entry-  
-s21-G--s211-exit--s21-exit--s2-exit--s1-entry- -s1-init- -s11-entry-  
-s1-I:  
-s1-A: -s11-exit- -s1-exit- -s1-entry- -s1-init- -s11-entry-  
-s1-D:-s11-exit- -s1-exit- -s-init--s1-entry- -s11-entry-  
-s11-D:-s11-exit- -s1-init- -s11-entry-  
-s1-C:-s11-exit- -s1-exit- -s2-entry--s2-init--s21-entry--s211-entry-  
-s-E:-s211-exit--s21-exit--s2-exit--s1-entry- -s11-entry-  
-s-E:-s11-exit- -s1-exit- -s1-entry- -s11-entry-  
-s11-G:-s11-exit- -s1-exit- -s2-entry--s21-entry--s211-entry-  
-s2-I-  
-s-I-  


* I assumed, gcc toolchain was added to PATH.

You can find the UML StateCharts representation of the implemented machine at the link:

https://i.hizliresim.com/kMPrjJ.png

The machine is cited from the Miro Samek's book, Practical UML StateChart in C and C++, page 88.

Notice: If you use this tiny framework in your projects, please make sure your machine runs 
as you expected in the manner of handling all the types of events. Otherwise, please feedback!
