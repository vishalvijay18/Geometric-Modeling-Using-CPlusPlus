#include "smfParser.h"
#include <cstdio>
#include <memory>
#include <boost/shared_ptr.hpp>
#include <math.h>

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::map;
using std::stringstream;

//Global variables
int numVertices, numFaces;
map<int, vertex*> vertexMap;
map<string, w_edge*> wingedEdgeMap;
map<int, face*> faceMap;
map<string, glvector*> faceNormalMap;
map<vertex*, glvector*> vertexNormalMap;

string exec(const char* cmd) {
    boost::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) return "ERROR";
    char buffer[128];
    string result = "";
    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }
    return result;
}

//function to create new vertex 
vertex *newVertex(float x, float y, float z)
{
	vertex *temp = new vertex;
	temp->x = x;
	temp->y = y;
	temp->z = z;
	temp->edge = NULL;
	return temp;
}

//function to create new wEdge
w_edge *newWEdge(vertex *start, vertex *end)
{
	w_edge* edge = new w_edge;
	edge->start = start;
	edge->end = end;
	edge->left = NULL;
	edge->right = NULL;
	edge->left_prev = NULL;
	edge->left_next = NULL;
	edge->right_prev = NULL;
	edge->right_next = NULL;
	
	return edge;
}

//function to create new wEdge with face structure passed as argument
w_edge *newWEdge(vertex *start, vertex *end, face* left)
{
        w_edge* edge = newWEdge(start, end);
	edge->left = left;

        return edge;
}

//function to create new face object
face *newFace(w_edge *edge)
{
	face* temp = new face;
	temp->edge = edge;

	return temp;	
}

//function to create new glvector
glvector *newGLVector(float x, float y, float z)
{
	glvector *temp = new glvector;
	temp->x = x;
	temp->y = y;
	temp->z = z;
	return temp;
}

//function for hashing used in wEdgeMap
string edgeHash(int v1, int v2)
{
	string hashedString;
	stringstream streamV1, streamV2;

	streamV1 << v1;
	streamV2 << v2;

	if(v1 < v2)
		hashedString = streamV1.str() + "||" + streamV2.str();
	else
		hashedString = streamV2.str() + "||" + streamV1.str();

	//cout << hashedString << endl;

	return hashedString;
}

//function for hashing used in faceNormalMap
string faceNormalHash(int faceid, vertex* v1)
{
	string hashedString;
	stringstream streamV0, streamV1, streamV2, streamV3;

	streamV0 << faceid;
	streamV1 << v1->x;
	streamV2 << v1->y;
	streamV3 << v1->z;

	hashedString = streamV0.str() + "||" + streamV1.str() + "||" + streamV2.str() + "||" + streamV3.str();

	return hashedString;
}

glvector* findVertexNormal(vertex* v)
{
	map<vertex*, glvector*>::iterator it = vertexNormalMap.find(v);
	
	if(it != vertexNormalMap.end())
	{
		return it->second;
	}
	else
	{
		cout << "error finding vertex normal\n";
	}
	
	return NULL;
}

//function to find if an edge has already been discovered between v1 and v2
w_edge *edgeExists(int v1, int v2)
{
	w_edge* edge = NULL;
	
	map<string, w_edge*>::iterator it = wingedEdgeMap.find(edgeHash(v1, v2));
	
	if(it != wingedEdgeMap.end())
	{
		edge = it->second;
	}
	
	return edge;
}

//function to update w_edge fields based on direction
void updateHelperEdges(w_edge *edge1, w_edge *edge2, w_edge *edge3, int edge1Dir, int edge2Dir, int edge3Dir)
{
	if(edge1Dir == 0)
	{
		edge1->left_prev = edge2;
		edge1->left_next = edge3;
	}
	else
	{
		edge1->right_prev = edge2;
		edge1->right_next = edge3;
	}

	if(edge2Dir == 0)
	{
		edge2->left_prev = edge3;
		edge2->left_next = edge1;
	}
	else
	{
		edge2->right_prev = edge3;
		edge2->right_next = edge1;
	}

	if(edge3Dir == 0)
	{
		edge3->left_prev = edge1;
		edge3->left_next = edge2;
	}
	else
	{
		edge3->right_prev = edge1;
		edge3->right_next = edge2;
	}
}


//function to search for edges between vertices and add edges if doesn't exist
void searchAndAdd(int vertex1, int vertex2, int vertex3, int count)
{
	w_edge *edge1, *edge2, *edge3;
	face *face1;
	
	//These variables are for storing edge directions. 0 represents forward, 1 backward
	int edge1Dir, edge2Dir, edge3Dir;

	edge1 = edgeExists(vertex1, vertex2);

	if(edge1 == NULL)
	{
		edge1 = newWEdge(vertexMap[vertex1], vertexMap[vertex2]);
		wingedEdgeMap.insert(std::make_pair(edgeHash(vertex1, vertex2),edge1));	
		face1 = newFace(edge1);
		edge1->left = face1;	
		faceMap[count] = face1;
		edge1Dir = 0;
		
		if(vertexMap[vertex1]->edge == NULL)
			vertexMap[vertex1]->edge = edge1;
		if(vertexMap[vertex2]->edge == NULL)
			vertexMap[vertex2]->edge = edge1;
	}
	else
	{	
		face1 = newFace(edge1);
		edge1->right = face1;
		faceMap[count] = face1;	
		edge1Dir = 1;
	}

	edge2 = edgeExists(vertex2, vertex3);

	if(edge2 == NULL)
	{
		edge2 = newWEdge(vertexMap[vertex2], vertexMap[vertex3], face1);
		wingedEdgeMap.insert(std::make_pair(edgeHash(vertex2, vertex3),edge2));	
		edge2Dir = 0;

		if(vertexMap[vertex3]->edge == NULL)
			vertexMap[vertex3]->edge = edge2;
	}
	else
	{
		edge2->right = face1;
		edge2Dir = 1;
	}
	
	edge3 = edgeExists(vertex3, vertex1);

	if(edge3 == NULL)
	{
		edge3 = newWEdge(vertexMap[vertex3], vertexMap[vertex1], face1);
		wingedEdgeMap.insert(std::make_pair(edgeHash(vertex3, vertex1),edge3));
		edge3Dir = 0;
	}
	else
	{
		edge3->right = face1;
		edge3Dir = 1;
	}
	
	updateHelperEdges(edge1, edge2, edge3, edge1Dir, edge2Dir, edge3Dir);
}

glvector *faceNormalFromVertexUnnormalized(face* f, vertex* v1, vertex* v2, vertex* v3)
{
	float x = ((v2->y - v1->y)*(v3->z - v1->z)) - ((v2->z - v1->z)*(v3->y - v1->y));
    	float y = ((v3->x - v1->x)*(v2->z - v1->z)) - ((v3->z - v1->z)*(v2->x - v1->x));
    	float z = ((v2->x - v1->x)*(v3->y - v1->y)) - ((v2->y - v1->y)*(v3->x - v1->x));

	return newGLVector(x, y, z);
}

//updating the vertexNormal in vertexNormalMap
void updateVertexNormal(int faceid, vertex* v1, vertex* v2, vertex* v3)
{
	glvector* vertexNormal;
	glvector* faceNormal;
	map<vertex*, glvector*>::iterator it = vertexNormalMap.find(v1);
	
	if(it != vertexNormalMap.end())
	{
		vertexNormal = it->second;
	}
	else
	{	
		vertexNormal = newGLVector(0.0, 0.0, 0.0);
		vertexNormalMap.insert(std::make_pair(v1, vertexNormal));
	}

	map<string, glvector*>::iterator it1 = faceNormalMap.find(faceNormalHash(faceid, v1));
	
	if(it1 != faceNormalMap.end())
	{
		faceNormal = it1->second;		
		float normalLength = sqrtf((faceNormal->x * faceNormal->x) + (faceNormal->y * faceNormal->y) + (faceNormal->z * faceNormal->z));

		float vector1X = v2->x - v1->x;
		float vector1Y = v2->y - v1->y;
		float vector1Z = v2->z - v1->z;

		float vector2X = v3->x - v1->x;
		float vector2Y = v3->y - v1->y;
		float vector2Z = v3->z - v1->z;

		float vector1Length = sqrtf((vector1X * vector1X) + (vector1Y * vector1Y) + (vector1Z * vector1Z));
		float vector2Length = sqrtf((vector2X * vector2X) + (vector2Y * vector2Y) + (vector2Z * vector2Z));

		float sin_alpha = normalLength/(vector1Length*vector2Length);

		float normalX = faceNormal->x/normalLength;
		float normalY = faceNormal->y/normalLength;
		float normalZ = faceNormal->z/normalLength;

		normalX = normalX * asin(sin_alpha);
		normalY = normalY * asin(sin_alpha);
		normalZ = normalZ * asin(sin_alpha);

		//updating the entry in vertexNormal
		vertexNormal->x = vertexNormal->x + normalX;
		vertexNormal->y = vertexNormal->y + normalY;
		vertexNormal->z = vertexNormal->z + normalZ;
	}
	else
	{	
		cout << "error in updateVertexNormal()";
	}
}

//function to find all edges incident to a vertex
void findAllEdgesofVertex(int vIndex)
{
	vertex *v = vertexMap[vIndex];
	w_edge *e0 = v->edge;
	w_edge *e1 = e0;

	do
	{
		if(e1->end == v)
		{
			e1 = e1->right_next;
		}		
		else
			e1 = e1->left_next;
		
		cout << e1->start->x << " " << e1->end->x << endl;
	} while(e1 != e0);
}

void findAllEdgesofFace(face* f, w_edge* eArray[3])
{
	w_edge *e0 = f->edge;
	w_edge *e1 = e0;
	int total = 0;

	do
	{
		eArray[total] = e1;

		if(e1->right == f)
			e1 = e1->right_prev;
		else
			e1 = e1->left_prev;
		
		total++;
		
	} while(e1 != e0);
}

void clearData()
{	
	numVertices = 0;
	numFaces = 0;

	std::map<int, vertex*>::iterator itr1;

	for(itr1 = vertexMap.begin(); itr1 != vertexMap.end(); itr1++)
	{
    		// found it - delete it
    		delete itr1->second;
    		vertexMap.erase(itr1);
	}

	std::map<string, w_edge*>::iterator itr2;

	for(itr2 = wingedEdgeMap.begin(); itr2 != wingedEdgeMap.end(); itr2++)
	{
    		// found it - delete it
    		delete itr2->second;
    		wingedEdgeMap.erase(itr2);
	}

	std::map<int, face*>::iterator itr3;

	for(itr3 = faceMap.begin(); itr3 != faceMap.end(); itr3++)
	{
    		// found it - delete it
    		delete itr3->second;
    		faceMap.erase(itr3);
	}

	std::map<string, glvector*>::iterator itr4;

	for(itr4 = faceNormalMap.begin(); itr4 != faceNormalMap.end(); itr4++)
	{
    		// found it - delete it
    		delete itr4->second;
    		faceNormalMap.erase(itr4);
	}

	std::map<vertex*, glvector*>::iterator itr5;

	for(itr5 = vertexNormalMap.begin(); itr5 != vertexNormalMap.end(); itr5++)
	{
    		// found it - delete it
    		delete itr5->second;
    		vertexNormalMap.erase(itr5);
	}

}

int initialize(string fileName)
{
	int count=1;
	string inputType;

	clearData();
	
	//need to remove 'newline' which is passed in output from linux command
	fileName.erase(std::remove(fileName.begin(), fileName.end(), '\n'), fileName.end());

	//input smf file
	std::ifstream infile(fileName.c_str());

	//reading number of vertices and number of faces
	infile >> inputType;
	infile >> numVertices >> numFaces;

	while(count <= numVertices)
	{
		float x, y, z;
		
		infile >> inputType;		
		infile >> x >> y >> z;
		
		//cout << " " << x << " " << y << " " << z << endl;
		vertex *v = newVertex(x, y, z);
		vertexMap[count] = v;

		count++;		
	}
	
	//reinitializing count to '1'
	count=1;
	
	while(count <= numFaces)
	{
		//vertices indexes
        	int v1, v2, v3;	
	
		infile >> inputType;
                infile >> v1 >> v2 >> v3;	
		
		searchAndAdd(v1, v2, v3, count);
		
		/*
		*create face normals
		*per face we will compute three normals - one through each vertex - and store them unnormalized)
		*vertices are taken in ANTI-CLOCKWISE direction
		*/
		
		glvector *vector1, *vector2, *vector3;

		vector1 = faceNormalFromVertexUnnormalized(faceMap[count], vertexMap[v1], vertexMap[v2], vertexMap[v3]);
		vector2 = faceNormalFromVertexUnnormalized(faceMap[count], vertexMap[v2], vertexMap[v3], vertexMap[v1]);
		vector3 = faceNormalFromVertexUnnormalized(faceMap[count], vertexMap[v3], vertexMap[v1], vertexMap[v2]);
		
		faceNormalMap.insert(std::make_pair(faceNormalHash(count, vertexMap[v1]), vector1));
		faceNormalMap.insert(std::make_pair(faceNormalHash(count, vertexMap[v2]), vector2));
		faceNormalMap.insert(std::make_pair(faceNormalHash(count, vertexMap[v3]), vector3));

		count++;	
	}

	count=1;
	while(count <= numFaces)
	{
		w_edge* faceEdgeArray[3];
		findAllEdgesofFace(faceMap[count], faceEdgeArray);
	
		w_edge* edge1 = faceEdgeArray[0];
	
		w_edge* edge2 = faceEdgeArray[1];

		if(edge1->start == edge2->start || edge1->end == edge2->start)
		{

			updateVertexNormal(count, edge1->start, edge1->end, edge2->end);
			updateVertexNormal(count, edge1->end, edge2->end, edge1->start);
			updateVertexNormal(count, edge2->end, edge1->start, edge1->end);
		}
		else
		{	
			updateVertexNormal(count, edge1->start, edge1->end, edge2->start);
			updateVertexNormal(count, edge1->end, edge2->start, edge1->start);
			updateVertexNormal(count, edge2->start, edge1->start, edge1->end);
		}

		count++;
	}
	
	infile.close();
	return 0;
}
