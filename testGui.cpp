#include<stdlib.h>
#include<stdio.h>
#include<iostream>
#include<GL/glui.h>
#include<GL/glut.h>

#define WIDTH 500
#define HEIGHT 500
#define TRUE 1
#define FALSE 0
#define X_SCALE 500.0
#define Y_SCALE 500.0
#define FLAT_SHADED 1
#define SMOOTH_SHADED 2
#define EDGES_SHADED 3
#define WIREFRAME 4
#define ROTATION_ID 10

float obj_pos[] = {0.0, 0.0, -3.0};
float sphere_rotate [16] = {1, 0, 0, 0,	0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

int window;
int color=1;
GLfloat x=200, y=200;
int side=50;
int display_type=1;
int segments=10;
float rotationX = 0.0, rotationY = 0.0;
int   last_x, last_y;

GLUI_Listbox *listbox;

void setupViewport(int w, int h) {

  	/* This sets up the viewport so that the x- and y-scales are proportional */
  	/* to the sides of the window.  If you draw a square it will be displayed */
  	/* as a square, regardless of how the window is resized.  */

  	glViewport(0, 0, w, h); 
  	glMatrixMode(GL_PROJECTION); // sets the matrix mode to projection -- to modify the 'view'
  	glLoadIdentity(); // intialize matrix to identity matrix
  	//gluOrtho2D(0.0, X_SCALE*w/WIDTH, 0.0, h*Y_SCALE/HEIGHT ); // sets screen co-ordinates: left, right, bottom, top
	glFrustum( -((float)w/(float)h)*0.08, ((float)w/(float)h)*0.08, -0.08, 0.08, .1, 15.0 );
}

void init() {

	glClearColor(0.7, 0.7, 1.0, 0.0); //background color
  	setupViewport(WIDTH, HEIGHT);

}

void controlBlock( int control )
{
	//TODO:handle controls
	if (control == ROTATION_ID) 
	{
		glutPostRedisplay();
	}
}

void handleMotion(int x, int y )
{
  rotationX += (float) (y - last_y);
  rotationY += (float) (x - last_x);

  last_x = x;
  last_y = y;

  glutPostRedisplay(); 
}

void handleDisplay()
{  	
  	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  	glMatrixMode( GL_MODELVIEW );
  	glLoadIdentity();
	glTranslatef(obj_pos[0], obj_pos[1], obj_pos[2]);
	glMultMatrixf(sphere_rotate);
  	glRotatef( rotationY, 0.0, 1.0, 0.0 );
  	glRotatef( rotationX, 1.0, 0.0, 0.0 );
	
	glColor3f(1.0, 1.0, 0.0);
	if(display_type==FLAT_SHADED)
	{
		glShadeModel(GL_FLAT);
		glutSolidTorus( .2,.5,16,segments );
  	}
	else if(display_type==SMOOTH_SHADED)
	{
		glShadeModel(GL_SMOOTH);
		glutSolidTorus( .2,.5,16,segments );
  	}
	else if(display_type==EDGES_SHADED)
	{
		glutWireTorus( .2,.5,16,segments );
	}
	else if(display_type==WIREFRAME)
	{
		glutWireTorus( .2,.5,16,segments );
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
	
	window = glutCreateWindow("Test GUI");	// creates the window
	init(); //things to do once, mostly in beginning	
	
	glutDisplayFunc(handleDisplay);		// tells glut which function to call to render a screen.
	//glutIdleFunc(handleIdle);		// handle when no events are occuring	
	glutReshapeFunc(handleReshape); 	// handle when reshape
	glutMotionFunc(handleMotion);		// handle when motion (this generally means when mouse is moved with a button pressed)
        //glutMouseFunc(handleMouse);		// handle when mouse moved without any button pressed
  	//glutKeyboardFunc(handleKeyboard);	// handle when keyboard key pressed
	
  	/****************************************/
  	/*       Set up OpenGL lights           */
  	/****************************************/

  	GLfloat light0_ambient[] =  {0.1f, 0.1f, 0.3f, 1.0f};
  	GLfloat light0_diffuse[] =  {.6f, .6f, 1.0f, 1.0f};
  	GLfloat light0_position[] = {1.0f, 1.0f, 1.0f, 0.0f};

  	glEnable(GL_LIGHTING);
  	glEnable(GL_LIGHT0);
  	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
  	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
  	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	
	glEnable(GL_DEPTH_TEST);		// enabling depth
	
	//GLUI code begins
	GLUI *glui = GLUI_Master.create_glui( "TEST_GLUI", 0, WIDTH+50, 100);
  	new GLUI_StaticText( glui, "Test GLUI" );
  	new GLUI_Separator(glui);
	
	glui->set_main_gfx_window( window );
	GLUI_Panel *obj_panel = glui->add_panel ("Display option panel");
  	
	listbox = new GLUI_Listbox(obj_panel, "Mesh display option", &display_type, 1, controlBlock);
  	listbox->add_item(FLAT_SHADED, "Flat Shaded");
	listbox->add_item(SMOOTH_SHADED, "Smooth Shaded");
	listbox->add_item(WIREFRAME, "Wireframe");
  	listbox->add_item(EDGES_SHADED, "Shaded with mesh edges");
	
	//rotation code
	GLUI_Panel * rotation_panel = glui->add_panel_to_panel(obj_panel, "", GLUI_PANEL_NONE);
	
	GLUI_Rotation *sph_rot =
		glui->add_rotation_to_panel(rotation_panel,"Rotate", sphere_rotate,
		ROTATION_ID, controlBlock); 
	sph_rot->set_spin(1.0);

	//translation code
	GLUI_Panel * translate_panel = glui->add_panel_to_panel(obj_panel, "",
		GLUI_PANEL_NONE);	

	GLUI_Translation * zoom =
		glui->add_translation_to_panel(translate_panel,"Object Z",
		GLUI_TRANSLATION_Z, &obj_pos[2]); zoom->set_speed(0.01);

	glui->add_column_to_panel(translate_panel, true);
	
	GLUI_Translation * move_around =
		glui->add_translation_to_panel(translate_panel,"Object XY",
		GLUI_TRANSLATION_XY, obj_pos);
	move_around->set_speed(0.01);

	//other details
	glui->add_separator();
  	//GLUI_Panel *obj_panel = glui->add_panel ("Test Panel");
	glui->add_button("Open", 1);
	glui->add_button("Save", 2);
  	glui->add_button("Quit", 3, (GLUI_Update_CB)exit);

	/* register idle callback with GLUI, NOT with GLUT */
  	GLUI_Master.set_glutIdleFunc( handleIdle );	// handle when no events are occuring 

	glutMainLoop();				// once this is called, glut takes over --
						// returns only when the window is closed	
	
	return EXIT_SUCCESS;
}
