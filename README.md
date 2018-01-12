
# Smart Camera Car

>2017 Fall NTUEE ESlab final project  
>group members:  
>b03901004  
>b03901162 

## Menu
* [Webpage](https://ntuee-eslab.github.io/2017Fall-SmartCameraCar/webpage/index.html)
* [Introduction](https://github.com/NTUEE-ESLab/2017Fall-SmartCameraCar#introduction)
* [Motivation](https://github.com/NTUEE-ESLab/2017Fall-SmartCameraCar#motivation)
* [Requirements](https://github.com/NTUEE-ESLab/2017Fall-SmartCameraCar#requirements)
* [Before use the code](https://github.com/NTUEE-ESLab/2017Fall-SmartCameraCar#before-use-the-code)

## Introduction
This is a small camera car that can follow your motion and shot video for you.

## Motivation
If you want to make a third-person video of yourself, but there are no helpers, and you don't have a good device, like a drone, to do that for you, how will you get it done?  
We want to build a device to help we shot high quality, smooth videos.
It should carry your own device, like cellphones or cameras, to ensure the video quality.  
It should to be cheap.  
Then we built this Smart Camera Car, everybody can use it.  

## Requirements

### Hardwares
- Raspberry pi 3 (Model B), with micro sd card larger than 16GB
- Raspberry pi camera module with cable longer than 30cm
- Arduino uno
- L298N
- 2 motors and wheels
- 2 universal wheels
- Tower Pro SG-90 servo motor
- 2 output Powerbank (we use asus zenpower pro)
- 8 AA battery and a battery box
- car body (how to design it is up to you)

### Softwares
- C++
- python3
- OpenCV
- darknet-nnpack

## Before use the code

This Project contain 2 main devices, Raspberry pi and Arduino.  There are some preparation you should do before use it.

### For Raspberry pi:  

### Install OpenCV

This is used for target detection and tracking.

You should install OpenCV on the Raspberry pi  

Intsall cmake and some other tools

>sudo apt-get install build-essential cmake pkg-config  
>sudo apt-get install libjpeg-dev libtiff5-dev libjasper-dev libpng12-dev  
>sudo apt-get install libavcodec-dev libavformat-dev libswscale-dev libv4l-dev  
>sudo apt-get install libxvidcore-dev libx264-dev  
>sudo apt-get install libgtk2.0-dev  
>sudo apt-get install libatlas-base-dev gfortran

Then clone OpenCV and contrib repository.  cd to a directory, and 

>git clone https://github.com/opencv/opencv.git  
>git clone https://github.com/opencv/opencv_contrib.git

Then make a build directory and get into it, for example:

>cd opencv  
>mkdir build  
>cd build

Then use cmake to prepare the files

>cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local  -D INSTALL_PYTHON_EXAMPLES=ON -D OPENCV_EXTRA_MODULES_PATH=&lt;opencv_contrib folder&gt;/modules &lt;opencv folder&gt;

Then compile it, use `-j4` to use 4 core of Raspberry pi.  It may takes more than 2 hours, depends on the speed of micro sd card.

>make -j4

After compile, install it

>sudo make install  
>sudo ldconfig

Then install is finish.  To save the space, you can remove the source code folders.

>sudo rm -rf opencv opencv_contrib

### Install Darknet-NNPACK

This is used for detect target recognition.

You need to install PeachPy and confu

>sudo pip install --upgrade git+https://github.com/Maratyszcza/PeachPy  
>sudo pip install --upgrade git+https://github.com/Maratyszcza/confu  

Also you need to install Ninja

>git clone https://github.com/ninja-build/ninja.git  
>cd ninja  
>git checkout release  
>./configure.py --bootstrap  
>export NINJA_PATH=$PWD  

Install clang

>sudo apt-get install clang  

Install NNPACK-darknet

>git clone https://github.com/thomaspark-pkj/NNPACK-darknet.git  
>cd NNPACK-darknet  
>confu setup  
>python ./configure.py --backend auto  
>$NINJA_PATH/ninja  
>sudo cp -a lib/* /usr/lib/  
>sudo cp include/nnpack.h /usr/include/  
>sudo cp deps/pthreadpool/include/pthreadpool.h /usr/include/  

Build darknet-nnpack

>git clone https://github.com/thomaspark-pkj/darknet-nnpack.git  
>cd darknet-nnpack  

Then edit the Makefile to build darknet-nnpack for opencv

>vim Makefile  

Edit the 3rd line of the makefile

>OPENCV=1  

Then make it

>make  

### Install flask

This is used for sending video stream to webpage.

>sudo pip install Flask  

### Enable camera

Insert the camra module onto Raspberry pi.  

Enable the camera

>sudo raspi-config

Then reboot the pi.

After rebooting, you can use `vcgencmd get_camera` to check if the camera is available.  You should see the text below:

>supported=1 detected=1

For OpenCV to use the camera, we should enable v4l2

>sudo modprobe bcm2835-v4l2

Then you can see the camera device at `/dev/video0`, and you can use `VideoCapture video(0);` to get the camera frames.

### Enable I2C

Enable the I2C

>sudo raspi-config

Then reboot the pi.

Install I2C tool

>sudo apt-get install i2c-tools

### For Arduino:  
Use Arduino IDE to burn the code in *** into Arduino
[](add file location)

### Build the car
Please refer to the webpage.
[](add hyper link)

## Usage
After setup the whole car, turn on Arduino, Raspberry pi and the whole power.  
On Raspberry pi, you should run 2 programs.  
The `myapp_opencv` setup a server for webpage monitering.  
The `Tracker_and_I2C` is the main tracking program.  

> cd RPi_programs  
> python3 myapp_opencv.py &  
> ./Tracker_and_I2C  

After run the programs, stay in front of the camera for a while, and the car will automatically start tracking you.  
You can use devices connected to the same LAN as RPi, and use web browser to `<RPi IP>:5000`, and you can moniter the car.

If the tracking fail, press `esc` key to terminate the program.
If you kill the program directly, the car may still runing.  Then run the `hardstop` program to stop it.
If you see `Address already in use` error while use `myapp_opencv.py`, then follow the steps below:
