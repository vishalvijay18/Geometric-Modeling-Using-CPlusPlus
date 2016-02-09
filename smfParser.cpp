#include "smfParser.h"

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
map<string, face> faceMap;

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
		edge1->right_next = edge2;
		edge1->right_prev = edge3;
	}
	else
	{
		edge1->left_next = edge2;
		edge1->left_prev = edge3;
	}

	if(edge2Dir == 0)
	{
		edge2->right_next = edge3;
		edge2->right_prev = edge1;
	}
	else
	{
		edge2->left_next = edge3;
		edge2->left_prev = edge1;
	}

	if(edge3Dir == 0)
	{
		edge3->right_next = edge1;
		edge3->right_prev = edge2;
	}
	else
	{
		edge3->left_next = edge1;
		edge3->left_prev = edge2;
	}
}


//function to search for edges between vertices and add edges if doesn't exist
void searchAndAdd(int vertex1, int vertex2, int vertex3)
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

//function to find all edges incident to a vertex
void findAllEdgesofVertex(int vIndex)
{
	vertex *v = vertexMap[vIndex];
	w_edge *e0 = v->edge;
	w_edge *e1 = e0;
	
	//cout << "check1: " << v->x << " " << v->y << " " << v->z << endl; 
	//cout << "check2: " << e0->start->x << " " << e0->end->x << endl;

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

int initialize()
{
	int count=1;
	string inputType;
	
	//input smf file
	std::ifstream infile("sphere.smf");

	//reading number of vertices and number of faces
	infile >> inputType;
	infile >> numVertices >> numFaces;
	
	//cout << "So, this were the input: " << numVertices << " and " << numFaces << endl;
	//infile >> inputType;

	//!inputType.compare("v")

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
	
	//cout << "Well, this were the vertices: " << vertexMap[2]->x << " and " << vertexMap[3]->x << " with count: " << count << endl;

	//reinitializing count to '0'
	count=1;
	
	while(count <= numFaces)
	{
		//vertices indexes
        	int v1, v2, v3;	
	
		infile >> inputType;
                infile >> v1 >> v2 >> v3;	
		
		//cout << "Entered with v's " << inputType << " " << v1 <<  " " << v2 << " " << v3 << endl;
		searchAndAdd(v1, v2, v3);
		
		//cout << "Mission successful: " << count << endl; 
		count++;	
	}
	
	//findAllEdgesofVertex(20);
	infile.close();
	return 0;
}
