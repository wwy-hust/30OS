30OS
=========
*After learning "Making an OS in 30 days", I made this OS, just for study & fun. This Os is writen by assembly and C.*

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

###How To Build & Run###
I made this OS on windows platform. You should install Cygwin ( install rm, cp, make & cat ) to build the OS. Open your Cygwin terminal and do next steps.

    [user@Localhost]$ cd MyOS
    [user@Localhost]$ make IMG       # to generate OS.img
    use OS.img as floppy disk image to boot OS with virtual machine. (I used VMWare)
