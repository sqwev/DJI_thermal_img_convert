# **DJI Thermal SDK (TSDK)**

Please open this file by markdown viewer.

## **What is the DJI Thermal SDK?**
The DJI Thermal SDK enables you to process R-JPEG (Radiometric JPEG) images which were captured by DJI infrared camera products.
Using the DJI Thermal SDK, create a customized application program to perform post processing for R-JPEG images (Including professional infrared image analysis and temperature measurement).

## **Get Started Immediately**

### **Prerequisites**

To use DJI Thermal SDK, the following environments are required:

- A PC or a laptop.
- Visual Studio 2015 for Windows 10, version 1804.
- gcc/g++ 5.4.0 for Ubuntu 16.04 LTS.

To build sample codes for DJI Thermal SDK, the following environments are required:

- Cmake 3.13.3

### **Supported Infrared Cameras**

- Zenmuse H20T
- Zenmuse H20N
- Zenmuse XT S
- M2EA (Mavic 2 Enterprise Advanced)
- M30T

### **Run Official Sample Program**

Change directory to **./utility/bin/\<windows or linux\>/\<release_x86 or release_x64\>**

Call sample executables with **--help** to show help information.
```
./dji_irp.exe --help
./dji_irp_omp.exe --help
./dji_ircm.exe --help
```

***NOTICE:***

For Linux version, there is no extension name of **.exe** and enter **export LD_LIBRARY_PATH=\<so library path\>** before executing. You may also need to run the **chmod +x** command for executables.

### **Command Example**

Input R-JPEG and output original RAW16 image.
```
./dji_irp.exe -s ../../../../dataset/H20T/DJI_0001_R.JPG -a extract -o extract.raw
```

Input R-JPEG and output global temperature value image which pixel type is INT16 or FLOAT32.
```
./dji_irp.exe -s ../../../../dataset/H20T/DJI_0001_R.JPG -a measure -o measure.raw
./dji_irp.exe -s ../../../../dataset/H20T/DJI_0001_R.JPG -a measure -o measure_float.raw --measurefmt float32
```

Input R-JPEG and output pseudo color image which pixel type is RGB888.
```
./dji_irp.exe -s ../../../../dataset/H20T/DJI_0001_R.JPG -a process -o process.raw -p iron_red
```

Input R-JPEG and output streching image which pixel type is FLOAT32.
```
./dji_irp.exe -s ../../../../dataset/H20T/DJI_0001_R.JPG -a process -o process_strech.raw --strech on
```

Input streching image and output pseudo color image. And generate color mapping LUT image.
```
./dji_ircm.exe -r ../../../../dataset/H20T/DJI_0001_R.JPG -s ../../../../dataset/orthomosaic/ir.raw -o ir_cm.raw --width 2000 --height 2000 -p fulgurite -l lut
```

Input R-JPEG files in specific directory and output all results.
```
./dji_irp_omp.exe -s ../../../../dataset/H20T/ -a extract -o extract_p
./dji_irp_omp.exe -s ../../../../dataset/H20T/ -a measure -o measure_p
./dji_irp_omp.exe -s ../../../../dataset/H20T/ -a process -o process_p -p iron_red
```

## **SDK API Reference**

[API Docuement ./doc/index.html](./doc/index.html) is generated from [dirp_api.h](./tsdk-core/api/dirp_api.h) with [Doxygen](https://www.doxygen.nl/index.html).

And **dirp_api.h** is the main API interface definition header file of TSDK.

You can understand the description and usage of each function through the annotation information in front of the data structure or function.

## **Develop Custom Program**

### **Based On Sample Code**

Change directory to **./sample**

Run **build.bat** or **build.sh** to call cmake to compile, build and install custom programs with MSVC or GCC.

Cmake will build sample projects under directory of **./sample/build/\<Release_x86 or Release_x64\>**

Cmake will install sample executables and and some other dependencies to directory of **./utility/bin/\<windows or linux\>/\<release_x86 or release_x64\>**

Then you can change to the installation directory to run custom execultables compiled by cmake.

### **Based On TSDK library**

#### **Copy TSDK Libraries**

Enter directory of **./tsdk-core/lib/\<windows or linux\>/\<release_x86 or release_x64\>** and copy all files to your custom application's dependence directory.
Such as **\<custom application path\>/3rdparty/tsdk/lib**

Enter directory of **./tsdk-core/api** and copy all files to your custom application's dependence directory.
Such as **\<custom application path\>/3rdparty/tsdk/include**

#### **Configure and Build Project**

You can develop your own application following these steps:
- Project configuration:
  - Add **\<custom application path\>/3rdparty/tsdk/include** to your include path.
  - Link **\<custom application path\>/3rdparty/tsdk/lib/\<libdirp.so or libdirp.dll\>** to your project.
- Source Code Development:
  - Include the **\<custom application path\>/3rdparty/tsdk/include/dirp_api.h**
in your source code.
  - Call TSDK APIs to perform infrared image processing.
- Build project with MSVC or GCC.

## **Support**
You can get help from DJI forum.

- Post questions on Developer Forum
  - [DJI Thermal SDK Developer Forum (CN)](https://bbs.dji.com/forum.php?mod=viewthread&tid=290236)
  - [DJI Thermal SDK Developer Forum (EN)](https://forum.dji.com/forum.php?mod=viewthread&tid=230321)
- Send E-mail to [dev@dji.com](dev@dji.com) describing your problem and a clear description of your setup
