#include<GL/glui.h>
#include<GL/glut.h>
#include "smfParser.h"
#include "wingedEdgeStructureParser.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::map;

#define WIDTH 800
#define HEIGHT 600
#define FLAT_SHADED 1
#define SMOOTH_SHADED 2
#define EDGES_SHADED 3
#define WIREFRAME 4
#define ROTATION_ID 10
#define OPEN_ID 11
#define SAVE_ID 12
#define QUIT_ID 13
#define RESET_ID 14

float obj_pos[] = {0.0, 0.0, 0.0};
float sphere_rotate [16] = {1, 0, 0, 0,	0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

//live variables
int window;
int color=1;
GLfloat x=200, y=200;
int display_type=FLAT_SHADED;
int segments=10;
float scale = 1.0;
float rotationX = 0.0, rotationY = 0.0;
int   lastx, lasty;
char file[1024];

GLUI_Listbox *listbox;

void setupViewport(int w, int h) {

  	/* This sets up the viewport so that the x- and y-scales are proportional */
  	/* to the sides of the window.  If you draw a square it will be displayed */
  	/* as a square, regardless of how the window is resized.  */

  	//glViewport(0, 0, w, h); 
	GLUI_Master.auto_set_viewport();  	
	glMatrixMode(GL_PROJECTION); // sets the matrix mode to projection -- to modify the 'view'
  	glLoadIdentity(); // intialize matrix to identity matrix
	glFrustum( -((float)w/(float)h)*0.08, ((float)w/(float)h)*0.08, -0.08, 0.08, .1, 15.0 );
}

void init() {

	glClearColor(0.7, 0.7, 1.0, 0.0); //background color
  	setupViewport(WIDTH, HEIGHT);
	file[0]='\0';
}

//handle controls
void controlBlock( int control )
{
	if (control == ROTATION_ID) 
	{
		glutPostRedisplay();
	}
	
	if (control == OPEN_ID)
	{
		string output;
		output = exec("zenity --file-selection --file-filter='SMF files (smf) | *.smf' --title=\"Select a SMF file\" 2>/dev/null");
		obj_pos[0]=0.0; obj_pos[1]=0.0; obj_pos[2]=-2.0;
		initialize(output);
		glutPostRedisplay();
	}
	
	//will be saved in output.smf
	if (control == SAVE_ID)
	{
		wingedEdgeParser();
		exec("zenity --info --text \"Successfully saved in output.smf\" 2>/dev/null");
	}

	if (control == RESET_ID) 
	{	
		scale = 1.0;
		obj_pos[0]=0.0;	obj_pos[1]=0.0; obj_pos[2]=-2.0;
		sphere_rotate[0] = 1; sphere_rotate[1] = 0; sphere_rotate[2] = 0; sphere_rotate[3] = 0; sphere_rotate[4] = 0; 
		sphere_rotate[5] = 1; sphere_rotate[6] = 0; sphere_rotate[7] = 0; sphere_rotate[8] = 0;	sphere_rotate[9] = 0;
		sphere_rotate[10] = 1; sphere_rotate[11] = 0; sphere_rotate[12] = 0; sphere_rotate[13] = 0; sphere_rotate[14] = 0;
		sphere_rotate[15] = 1;
		
		glutPostRedisplay();
	}
}

void handleMotion(int x, int y )
{
  rotationX += (float) (y - lasty);
  rotationY += (float) (x - lastx);

  lastx = x;
  lasty = y;

  glutPostRedisplay(); 
}

void handleDisplay()
{  	
  	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  	glMatrixMode( GL_MODELVIEW );
  	glLoadIdentity();
	glTranslatef(obj_pos[0], obj_pos[1], obj_pos[2]);
	glScalef( scale, scale, scale );
	glMultMatrixf(sphere_rotate);
  	glRotatef( rotationY, 0.0, 1.0, 0.0 );
  	glRotatef( rotationX, 1.0, 0.0, 0.0 );
	
	glColor3f(1.0, 1.0, 0.0);
	if(display_type==FLAT_SHADED || display_type==EDGES_SHADED)
	{		
		w_edge* faceEdgeArray[3];
		glShadeModel(GL_FLAT);
		glEnable(GL_NORMALIZE);
		glBegin(GL_TRIANGLES);
		for( int i=1; i<=numFaces; i++)
		{	
      			face* value = faceMap[i];
			findAllEdgesofFace(value, faceEdgeArray);
		
			//using 1st edge for 2 vertices
			w_edge* edge1 = faceEdgeArray[0];
			
			map<string, glvector*>::iterator it3 = faceNormalMap.find(faceNormalHash(i, edge1->start));
	
			if(it3 != faceNormalMap.end())
			{
				glvector* faceNormal = it3->second;
				glNormal3f(faceNormal->x, faceNormal->y, faceNormal->z);
			}
			else
			{	
				cout << "error in FLAT Shading display" << endl; 
			}

			if(edge1->left == value)
			{
				glVertex3f(edge1->start->x, edge1->start->y, edge1->start->z);
				glVertex3f(edge1->end->x, edge1->end->y, edge1->end->z);
			}
			else if(edge1->right == value)
			{
				glVertex3f(edge1->end->x, edge1->end->y, edge1->end->z);
				glVertex3f(edge1->start->x, edge1->start->y, edge1->start->z);
			}
		
			//using 2nd edge for 3rd vertex
			w_edge* edge2 = faceEdgeArray[1];

			if(edge1->start == edge2->start || edge1->end == edge2->start)
			{
				glVertex3f(edge2->end->x, edge2->end->y, edge2->end->z);
			}
			else
			{	
				glVertex3f(edge2->start->x, edge2->start->y, edge2->start->z);
			}
		}
		glEnd();
		
  	}
	
	if(display_type==SMOOTH_SHADED)
	{
		w_edge* faceEdgeArray[3];
		
		glShadeModel(GL_SMOOTH);

		glBegin(GL_TRIANGLES);
		for( int i=1; i<=numFaces; i++)
		{
      			face* value = faceMap[i];
			glvector *normal1, *normal2, *normal3;

			findAllEdgesofFace(value, faceEdgeArray);
		
			//using 1st edge for 2 vertices
			w_edge* edge1 = faceEdgeArray[0];
			
			normal1 = findVertexNormal(edge1->start);		
			normal2 = findVertexNormal(edge1->end);

			if(edge1->left == value)
			{
				if(normal1 != NULL)
					glNormal3f(normal1->x, normal1->y, normal1->z);

				glVertex3f(edge1->start->x, edge1->start->y, edge1->start->z);

				if(normal2 != NULL)
					glNormal3f(normal2->x, normal2->y, normal2->z);
				glVertex3f(edge1->end->x, edge1->end->y, edge1->end->z); 
			}
			else if(edge1->right == value)
			{
				if(normal2 != NULL)
					glNormal3f(normal2->x, normal2->y, normal2->z);

				glVertex3f(edge1->end->x, edge1->end->y, edge1->end->z);

				if(normal1 != NULL)
					glNormal3f(normal1->x, normal1->y, normal1->z);

				glVertex3f(edge1->start->x, edge1->start->y, edge1->start->z);
			}
		
			//using 2nd edge for 3rd vertex
			w_edge* edge2 = faceEdgeArray[1];

			if(edge1->start == edge2->start || edge1->end == edge2->start)
			{	
				normal3 = findVertexNormal(edge2->end);
				
				if(normal3 != NULL)
					glNormal3f(normal3->x, normal3->y, normal3->z);

				glVertex3f(edge2->end->x, edge2->end->y, edge2->end->z);
			}
			else
			{	
				normal3 = findVertexNormal(edge2->start);

				if(normal3 != NULL)
					glNormal3f(normal3->x, normal3->y, normal3->z);

				glVertex3f(edge2->start->x, edge2->start->y, edge2->start->z);
			}
		}
		glEnd();
  	}
	
	if(display_type==EDGES_SHADED)
	{
		glPolygonOffset(1.0, 1.0);
		glBegin(GL_LINES);
		for( map<string, w_edge*>::const_iterator it = wingedEdgeMap.begin(); it != wingedEdgeMap.end(); ++it )
    		{
      			string key = it->first;
      			w_edge* value = it->second;
			
			glVertex3f(value->start->x, value->start->y, value->start->z);
			glVertex3f(value->end->x, value->end->y, value->end->z);
    		}
    		glEnd();
	}
	
	if(display_type==WIREFRAME)
	{
		glBegin(GL_LINES);
		for( map<string, w_edge*>::const_iterator it = wingedEdgeMap.begin(); it != wingedEdgeMap.end(); ++it )
    		{
      			string key = it->first;
      			w_edge* value = it->second;
			
			glVertex3f(value->start->x, value->start->y, value->start->z);
			glVertex3f(value->end->x, value->end->y, value->end->z);
    		}
    		glEnd();
	}

 	glutSwapBuffers();

}

void handleReshape(int w, int h) {
        setupViewport(w, h);
  	glutSetWindow(window);
	glutPostRedisplay();  
}

void handleIdle(void) {

	if (glutGetWindow() != window)
    		glutSetWindow(window);

	glutPostRedisplay();
}



int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); //double buffering, RGB mode, need depth buffer
	glutInitWindowSize(WIDTH,HEIGHT); //width, height of the window
	glutInitWindowPosition(0, 0);	// location on the screen the window appears
	
	window = glutCreateWindow("Assignment 1 GUI");	// creates the window
	
	init(); //things to do once, mostly in beginning	
	
	glutDisplayFunc(handleDisplay);		// tells glut which function to call to render a screen.
	glutMotionFunc(handleMotion);		// handle when motion (this generally means when mouse is moved with a button pressed)
	
	//glutIdleFunc(handleIdle);		// handle when no events are occuring	
	//glutReshapeFunc(handleReshape); 	// handle when reshape
        //glutMouseFunc(handleMouse);		// handle when mouse moved without any button pressed
  	//glutKeyboardFunc(handleKeyboard);	// handle when keyboard key pressed
	
	//GLUI_Master.set_glutKeyboardFunc( myGlutKeyboard );
 	//GLUI_Master.set_glutSpecialFunc( myGlutSpecial );
	//GLUI_Master.set_glutMotionFunc( handleMotion );
 	GLUI_Master.set_glutReshapeFunc( handleReshape );

  	//Setting opengl lights

  	GLfloat light0_ambient[] =  {0.1f, 0.1f, 0.3f, 1.0f};
  	GLfloat light0_diffuse[] =  {.6f, .6f, 1.0f, 1.0f};
  	GLfloat light0_position[] = {1.0f, 1.0f, 1.0f, 0.0f};

  	glEnable(GL_LIGHTING);
  	glEnable(GL_LIGHT0);
  	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
  	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
  	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	
	glEnable(GL_DEPTH_TEST);		// enabling depth
	
	GLUI *glui = GLUI_Master.create_glui_subwindow(window,
                     GLUI_SUBWINDOW_RIGHT);
  	new GLUI_StaticText( glui, "Assignment 1 GLUI" );
  	new GLUI_Separator(glui);
	
	glui->set_main_gfx_window( window );
	GLUI_Panel *obj_panel = glui->add_panel ("Display option panel");
  	
	listbox = new GLUI_Listbox(obj_panel, "Mesh display option", &display_type, 1, controlBlock);
  	listbox->add_item(FLAT_SHADED, "Flat Shaded");
	listbox->add_item(SMOOTH_SHADED, "Smooth Shaded");
	listbox->add_item(WIREFRAME, "Wireframe");
  	listbox->add_item(EDGES_SHADED, "Shaded with mesh edges");

	//scale code
	GLUI_Spinner *scale_spinner = new GLUI_Spinner( obj_panel, "Scale:", &scale);
  	scale_spinner->set_float_limits( .2f, 5.0 );
  	scale_spinner->set_alignment( GLUI_ALIGN_RIGHT );
	
	//rotation code
	GLUI_Panel * rotation_panel = glui->add_panel_to_panel(obj_panel, "", GLUI_PANEL_NONE);
	
	GLUI_Rotation *sph_rot =
		glui->add_rotation_to_panel(rotation_panel,"Rotate", sphere_rotate,
		ROTATION_ID, controlBlock); 
	sph_rot->set_spin(1.0);

	//translation code
	GLUI_Panel * translate_panel = glui->add_panel_to_panel(obj_panel, "",
		GLUI_PANEL_NONE);	

	GLUI_Translation * move_z =
		glui->add_translation_to_panel(translate_panel,"Object Z",
		GLUI_TRANSLATION_Z, &obj_pos[2]); 
	move_z->scale_factor = 0.1f;

	glui->add_column_to_panel(translate_panel, true);
	
	GLUI_Translation * move_around =
		glui->add_translation_to_panel(translate_panel,"Object XY",
		GLUI_TRANSLATION_XY, obj_pos);
	move_around->scale_factor = 0.1f;

	//other details
	glui->add_separator();
	glui->add_button("Open", OPEN_ID, controlBlock);
	glui->add_button("Save", SAVE_ID, controlBlock);
	glui->add_button("Reset", RESET_ID, controlBlock);
  	glui->add_button("Quit", QUIT_ID, (GLUI_Update_CB)exit);

	/* register idle callback with GLUI, NOT with GLUT */
  	GLUI_Master.set_glutIdleFunc( handleIdle );	// handle when no events are occuring 

	glutMainLoop();				// once this is called, glut takes over --
						// returns only when the window is closed	
	
	return EXIT_SUCCESS;
}
