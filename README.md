# sphp
> Command Line App that changes PATH variable to point to a different PHP version
> for Windows 64-bit Computers

## Basic Usage
You will need to add the paths to the different PHP versions in `sphp.cpp`

sphp [VERSION] </br>
Version: </br>
&nbsp;&nbsp;&nbsp;56&nbsp;&nbsp;&nbsp;Change to PHP Version 5.6 </br>
&nbsp;&nbsp;&nbsp;70&nbsp;&nbsp;&nbsp;Change to PHP Version 7.0 </br>
&nbsp;&nbsp;&nbsp;71&nbsp;&nbsp;&nbsp;Change to PHP Version 7.1 </br>
&nbsp;&nbsp;&nbsp;72&nbsp;&nbsp;&nbsp;Change to PHP Version 7.2 </br>
&nbsp;&nbsp;&nbsp;73&nbsp;&nbsp;&nbsp;Change to PHP Version 7.3 </br>

## How to Run
```bash
#Build Executable
make

#Run and open new command prompt to view your changes 
sphp [VERSION]
```