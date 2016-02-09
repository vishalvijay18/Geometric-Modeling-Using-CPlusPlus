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

typedef struct Vertex vertex;
typedef struct Face face;
typedef struct W_edge w_edge;

//Global variables
extern int numVertices, numFaces;
extern std::map<int, vertex*> vertexMap;
extern std::map<std::string, w_edge*> wingedEdgeMap;
extern std::map<std::string, face> faceMap;

//main parser function
int initialize();

#endif
