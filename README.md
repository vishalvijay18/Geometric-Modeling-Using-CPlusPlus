# Geometric Modeling using OpenGL
This repository will contain some basic geometric modeling tasks using OpenGL and GLUI.

You will need to install OpenGL and GLUI libraries for running codes in this repository.

##Steps for setup on Ubuntu (15.10)

- Use following command for installing freeglut package
```
sudo apt-get install freeglut3 freeglut3-dev`
sudo apt-get install binutils-gold
```
- Download packages libglui2c2 and libglui-dev from ubuntu package archive. Links below for 64bit architecture:
[libglui2c2](http://packages.ubuntu.com/precise/amd64/libglui2c2)
[libglui-dev](http://packages.ubuntu.com/precise/libglui-dev)
- Install them:
```
sudo dpkg -i libglui2c2_2.36-4ubuntu1_amd64.deb
sudo dpkg -i libglui-dev_2.36-4ubuntu1_amd64.deb
```
- Try testing a simple code to test if it works. Sample command:
`g++ sample.cpp -L/usr/lib -lglui -lglut -lGLU -lGL -lm`

Interesting read to understand basics of OpenGL code:
[OpenGL basics](https://www3.ntu.edu.sg/home/ehchua/programming/opengl/CG_Introduction.html)

Let me know if you face any issues.
