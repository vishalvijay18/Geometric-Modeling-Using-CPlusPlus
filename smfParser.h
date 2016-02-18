#ifndef SMFPARSER_H
#define SMFPARSER_H

#include <stdio.h>
#include <iostream>
#include <map>
#include <fstream>
#include <sstream>

//Structure defining a winged-edge
struct W_edge
{
	struct Vertex *start, *end;
	struct Face *left, *right;
	struct W_edge *left_prev, *left_next;
	struct W_edge *right_prev, *right_next;
};

//Structure defining a vertex in wionged-egde format
struct Vertex
{
	float x, y, z;
	struct W_edge *edge;
};

//Structure defining a face in winged-edge format
struct Face
{
	struct W_edge * edge;
};

//Structure defining a vector (to be used for normals)
struct GLVector
{
	float x, y, z;
};

typedef struct Vertex vertex;
typedef struct Face face;
typedef struct W_edge w_edge;
typedef struct GLVector glvector;

//Global variables
extern int numVertices, numFaces;
extern std::map<int, vertex*> vertexMap;
extern std::map<std::string, w_edge*> wingedEdgeMap;
extern std::map<int, face*> faceMap;
extern std::map<std::string, glvector*> faceNormalMap;
extern std::map<vertex*, glvector*> vertexNormalMap;

//main parser function
int initialize(std::string);

//execute command
std::string exec(const char*);

//find all edges of a face
void findAllEdgesofFace(face*, w_edge**);

//function to find all edges incident to a vertex
void findAllEdgesofVertex(int);

//function to calculate faceNormalHash
std::string faceNormalHash(int, vertex*);

//function to find vertexNormal if exists
glvector* findVertexNormal(vertex*);

#endif
