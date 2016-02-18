all:
	g++ MainGUI.cpp smfParser.cpp wingedEdgeStructureParser.cpp -o smf_view -L/usr/lib -lglui -lglut -lGLU -lGL -lm
