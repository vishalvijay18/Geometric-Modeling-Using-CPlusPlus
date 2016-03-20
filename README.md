# Geometric Modeling using C++ and OpenGL
This repository will contain some basic geometric modeling tasks using C++, OpenGL and GLUI.

You will need to install OpenGL and GLUI libraries for running codes in this repository.

###Steps for setup on Ubuntu (15.10)
---
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
```
g++ sample.cpp -L/usr/lib -lglui -lglut -lGLU -lGL -lm`
```

Interesting read to understand basics of OpenGL code:
[OpenGL basics](https://www3.ntu.edu.sg/home/ehchua/programming/opengl/CG_Introduction.html)

Let me know if you face any issues.

###Basics
---
1. What is this project all about?
  * The code present in this repository is basically implementation of major geometric modelling paradigms in computer graphics. It provides smf reader, winged-edge data structure parser and decimation of 3D model (coming soon) using C++, OpenGL and GLUI. 

2. What are .smf files?
  * In the SMF (Simple Mesh Format) format, a mesh is given by a vertex list followed by a face  list. Each  line  in  the vertex list starts with the character ‘v’, followed by the x, y, and z vertex coordinates. Each line in the face list starts with the character ‘f’, and followed by three integers indexing into the vertex list. The vertex indexes start with 1 and are given in counterclockwise order, viewed from the  tip  of the  triangle’s  outward  pointing  normal.  Any  comments  start with the character ‘#’. The very first line of the SMF file is of the form “# n m”, where n is the number of vertices and m the number of faces in the mesh.

3. What are winged-edge data structures?
  * It is a representation of an orientable mesh which facilitates easy edge processing and mesh manipulation. Refer [this](http://what-when-how.com/advanced-methods-in-computer-graphics/mesh-processing-advanced-methods-in-computer-graphics-part-2/) for an easy to understand details of this data structure.



###References
---
- CMPT 764 course SFU [(course web page)](http://www2.cs.sfu.ca/~haoz/teaching/cmpt464/)
