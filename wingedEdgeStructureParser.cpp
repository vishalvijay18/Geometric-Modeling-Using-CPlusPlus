#include "smfParser.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::map;
using std::stringstream;

map<vertex*, int> vertexIndexMap;
std::ofstream outfile;

void updateNum()
{
	outfile << "# " << numVertices << " " << numFaces << "\n";
}

//This function outputs list of vertices to output file
void findVertices()
{
	for( int i=1; i<=numVertices; i++ )
    	{

		vertexIndexMap.insert(std::make_pair(vertexMap[i], i));	

		outfile << "v " << vertexMap[i]->x << " " << vertexMap[i]->y << " " << vertexMap[i]->z << "\n";
    	}
}

//This function outputs all faces of a model
void findFaces()
{
	w_edge* faceEdgeArray[3];
	

	for( int i=1; i<=numFaces; i++)
	{
		string outputStr = "f ";
      		face* value = faceMap[i];
		
		findAllEdgesofFace(value, faceEdgeArray);
		
		//using 1st edge for 2 vertices
		w_edge* edge1 = faceEdgeArray[0];

		if(edge1->left == value)
		{
			outfile << outputStr << vertexIndexMap[edge1->start] << " " << vertexIndexMap[edge1->end] << " ";  
		}
		else if(edge1->right == value)
		{
			outfile << outputStr << vertexIndexMap[edge1->end] << " " << vertexIndexMap[edge1->start] << " ";
		}
		
		//using 2nd edge for 3rd vertex
		w_edge* edge2 = faceEdgeArray[1];

		if(edge1->start == edge2->start || edge1->end == edge2->start)
		{
			outfile << vertexIndexMap[edge2->end] << "\n";
		}
		else
		{	
			outfile << vertexIndexMap[edge2->start] << "\n";
		}
	}
}

void clearOldData()
{
	vertexIndexMap.clear();
}

void wingedEdgeParser()
{
  	outfile.open ("output.smf");
	
	clearOldData();
	
	updateNum();
	findVertices();
	findFaces();

	outfile.close();
}
