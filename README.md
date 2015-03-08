30OS
=========
*After learning "Making an OS in 30 days", I made this OS for study & fun. This Os is writen by assembly and C.*

*document added, you could use Doxygen to generate document.*

###Supported Function:###

1. task shedule
2. windows management (double buffer)
3. mem management
4. console supported
5. simple API supported
6. GB2312 charset supported
7. timer supported

###Unsupported Function:###

1. file system
2. auido
3. ....

###ScreenShot###
![image](https://github.com/wwy-hust/30OS/blob/master/screenshot.png)

###How To Build###
I made this OS on windows platform. You should install Cygwin ( install rm, cp, make & cat ) to build the OS. 

Open your Cygwin terminal and do next steps.

    [user@Localhost]$ chmod +x ./z_tools/*
    [user@Localhost]$ cd src
    [user@Localhost]$ make IMG       # to generate OS.img, If failed first time, make again
    
###How To Run###
Use OS.img as floopy img to boot with Virtual Machines software.

In OS you could operate as a windows. Use "Win + F1" to open a new console.