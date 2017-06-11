//modified by: Abdulelah Aldeshash
//date: June/11/2017
//purpose: hw1
//
//cs3350 Spring 2017 Lab-1
//author: Gordon Griesel
//date: 2014 to present
//This program demonstrates the use of OpenGL and XWindows
//
//Assignment is to modify this program.
//You will follow along with your instructor.
//
//Elements to be learned in this lab...
//
//. general animation framework
//. animation loop
//. object definition and movement
//. collision detection
//. mouse/keyboard interaction
//. object constructor
//. coding style
//. defined constants
//. use of static variables
//. dynamic memory allocation
//. simple opengl components
//. git
//
//elements we will add to program...
//. Game constructor
//. multiple particles
//. gravity
//. collision detection
//. more objects
//

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "str.h"
#include "fonts.h"

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define MAX_PARTICLES 10000
#define GRAVITY 0.1

//X Windows variables
Display *dpy;
Window win;
GLXContext glc;


//Function prototypes
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_mouse(XEvent *e, Game *game);
int check_keys(XEvent *e, Game *game);
void movement(Game *game);
void render(Game *game);


int main(void)
{
	int done=0;
	srand(time(NULL));
	initXWindows();
	init_opengl();
	//declare game object
	Game game;	
	game.n=0;

	//declar box shape
	int x1 =220;
	int x2 =750;
	for (int i=0; i<5; i++) {
		game.box1[i].width = 100;
		game.box1[i].height = 13;
		game.box1[i].center.x += x1 + i*60;
		game.box1[i].center.y += (x2-(i*50) - (5*60));
	}
	//start animation
	while (!done) {
		while (XPending(dpy)) {
			XEvent e;
			XNextEvent(dpy, &e);
			check_mouse(&e, &game);
			done = check_keys(&e, &game);
		}
		movement(&game);
		render(&game);
		glXSwapBuffers(dpy, win);
	}
	cleanupXWindows();
	cleanup_fonts();
	return 0;
}

void set_title(void)
{
	//Set the window title bar.
	XMapWindow(dpy, win);
	XStoreName(dpy, win, "335 Lab1   LMB for particle");
}

void cleanupXWindows(void)
{
	//do not change
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
}

void initXWindows(void)
{
	//do not change
	GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	int w=WINDOW_WIDTH, h=WINDOW_HEIGHT;
	dpy = XOpenDisplay(NULL);
	if (dpy == NULL) {
		std::cout << "\n\tcannot connect to X server\n" << std::endl;
		exit(EXIT_FAILURE);
	}
	Window root = DefaultRootWindow(dpy);
	XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
	if (vi == NULL) {
		std::cout << "\n\tno appropriate visual found\n" << std::endl;
		exit(EXIT_FAILURE);
	} 
	Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
	XSetWindowAttributes swa;
	swa.colormap = cmap;
	swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
		ButtonPress | ButtonReleaseMask | PointerMotionMask |
		StructureNotifyMask | SubstructureNotifyMask;
	win = XCreateWindow(dpy, root, 0, 0, w, h, 0, vi->depth,
		InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
	set_title();
	glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	glXMakeCurrent(dpy, win, glc);
}

void init_opengl(void)
{
	//OpenGL initialization
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	//Initialize matrices
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	//Set 2D mode (no perspective)
	glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
	//Set the screen background color
	glClearColor(0.1, 0.1, 0.1, 1.0);

	//initialize fonts
	glEnable(GL_TEXTURE_2D);
	initialize_fonts();
}


extern void makeParticle(Game *game, int x, int y);

void check_mouse(XEvent *e, Game *game)
{
	static int savex = 0;
	static int savey = 0;
	static int n = 0;

	if (e->type == ButtonRelease) {
		return;
	}
	if (e->type == ButtonPress) {
		if (e->xbutton.button==1) {
			//Left button was pressed
			int y = WINDOW_HEIGHT - e->xbutton.y;
			makeParticle(game, e->xbutton.x, y);
			makeParticle(game, e->xbutton.x, y);
			makeParticle(game, e->xbutton.x, y);
			makeParticle(game, e->xbutton.x, y);
			makeParticle(game, e->xbutton.x, y);
			return;
		}
		if (e->xbutton.button==3) {
			//Right button was pressed
			return;
		}
	}
	//Did the mouse move?
	if (savex != e->xbutton.x || savey != e->xbutton.y) {
		savex = e->xbutton.x;
		savey = e->xbutton.y;
		if (++n < 10)
			return;
		int y = WINDOW_HEIGHT - e->xbutton.y;
		for ( int i =0; i<10; i++)
		    makeParticle(game, e->xbutton.x, y);
	
	}
}

int check_keys(XEvent *e, Game *game)
{
	//Was there input from the keyboard?
	if (e->type == KeyPress) {
		int key = XLookupKeysym(&e->xkey, 0);
		if (key == XK_Escape) {
			return 1;
		}
		//You may check other keys here.
	}
	return 0;
}

void movement(Game *game)
{
	Particle *p;

	if (game->n <= 0)
		return;
	
	for (int i=0; i<game->n; i++) {
		p = &game->particle[i];
		p->velocity.y -= GRAVITY;
		// Speed pf particles
		p->s.center.x += p->velocity.x;
		p->s.center.y += p->velocity.y;
		
		Shape *s1;	
		//check for collision with shapes...
		for (int i=0; i<5; i++) {
		s1= &game->box1[i];
		if (
			p->s.center.y < s1->center.y + s1->height &&
			p->s.center.y > s1->center.y - s1->height &&
			p->s.center.x >= s1->center.x - s1->width &&
			p->s.center.x <= s1->center.x + s1->width) {

		    p->s.center.y = s1->center.y + s1->height;
		    p->velocity.y = -p->velocity.y *0.5;
		    // force pushing particles to the right
		    p->velocity.x = 1;
			std::cout<<"hit the box!!!!!!!!!!!!!"<<std::endl;
		}
		}
		/*
		s2= &game->box2;
		if (    p->s.center.y < s2->center.y + s2->height &&
			p->s.center.y > s2->center.y - s2->height &&
			p->s.center.x >= s2->center.x - s2->width &&
			p->s.center.x <= s2->center.x + s2->width) {
		    
		    p->s.center.y = s2->center.y + s2->height;
		    p->velocity.y = -p->velocity.y *0.5;
		    p->velocity.x = 1;
		 }

		s3 = &game->box3;
		if (    p->s.center.y < s3->center.y + s3->height &&
			p->s.center.y > s3->center.y - s3->height &&
			p->s.center.x >= s3->center.x - s3->width &&
			p->s.center.x <= s3->center.x + s3->width) {
		    
		    p->s.center.y = s3->center.y + s3->height;
		    p->velocity.y = -p->velocity.y *0.5;
		    p->velocity.x = 1;
		}
		s4= &game->box4;
		if (    p->s.center.y < s4->center.y + s4->height &&
				p->s.center.y > s4->center.y - s4->height &&
				p->s.center.x >= s4->center.x - s4->width &&
				p->s.center.x <= s4->center.x + s4->width) {

            
			p->s.center.y = s4->center.y + s4->height;
    
			p->velocity.y = -p->velocity.y *0.5;

			p->velocity.x = 1;

		}

		s5 = &game->box5;
		if (    p->s.center.y < s5->center.y + s5->height &&
				p->s.center.y > s5->center.y - s5->height &&
				p->s.center.x >= s5->center.x - s5->width &&
				p->s.center.x <= s5->center.x + s5->width) {

			p->s.center.y = s5->center.y + s5->height;
			p->velocity.y = -p->velocity.y *0.5;
			p->velocity.x = 1;
		}*/
		//check for off-screen
		if (p->s.center.y < 0.0 || p->s.center.y > WINDOW_HEIGHT) {
	    	    //std::cout << "off screen" << std::endl;
	    	    game->particle[i] = game->particle[game->n-1];
	    	    game->n --;
		}	
	}
}

void render(Game *game)
{
	Rect r;
	float w, h;
	glClear(GL_COLOR_BUFFER_BIT);
	
	//Draw shapes...
	Shape *s1;
	for (int i=0; i<5; i++) {
		glColor3ub(90,140,90);
		s1 = &game->box1[i];
		glPushMatrix();
		glTranslatef(s1->center.x, s1->center.y, s1->center.z);
		w = s1->width;
		h = s1->height;
		glBegin(GL_QUADS);
			glVertex2i(-w,-h);
			glVertex2i(-w, h);
			glVertex2i( w, h);
			glVertex2i( w,-h);
		glEnd();
		glPopMatrix();
	}

	//draw particles
	glPushMatrix();
	glColor3ub(150,160,220);
	for ( int i=0; i<game->n; i++) {
		Vec *c = &game->particle[i].s.center;
		w = 2;
		h = 2;
		glBegin(GL_QUADS);
			glVertex2i(c->x-w, c->y-h);
			glVertex2i(c->x-w, c->y+h);
			glVertex2i(c->x+w, c->y+h);
			glVertex2i(c->x+w, c->y-h);
		glEnd();
		glPopMatrix();
	}
	
	//draw text
		//game.box1[i].center.x += x1 + i*60;
		//game.box1[i].center.y += (x2-(i*50) - (5*60));

	unsigned int c = 0x00ffff44;
	
	r.bot = WINDOW_HEIGHT-160;
	r.left = 160;
	r.center = 0;
	ggprint16(&r, 16, c, "Requirement");
	
	r.bot = WINDOW_HEIGHT-210;
	r.left = 220;
	r.center = 0;
	ggprint16(&r, 16, c, "  Design");
	
	r.bot = WINDOW_HEIGHT-260;
	r.left = 280;
	r.center = 0;
	ggprint16(&r, 16, c, "Implementation");
	
	r.bot = WINDOW_HEIGHT-310;
	r.left = 340;
	r.center = 0;
	ggprint16(&r, 16, c, "   Test");
	
	r.bot = WINDOW_HEIGHT-360;
	r.left = 400;
	r.center = 0;
	ggprint16(&r, 16, c, " Release");

		
}
