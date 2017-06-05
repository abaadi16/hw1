//modified by: Abdulelah Aldeshash
//date: May/30/2017
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
//#include <fonts.h>


#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define MAX_PARTICLES 10000
#define GRAVITY 0.1

//X Windows variables
Display *dpy;
Window win;
GLXContext glc;

//Structures

struct Vec {
	float x, y, z;
};

struct Shape {
	float width, height;
	float radius;
	Vec center;
};

struct Particle {
	Shape s;
	Vec velocity;
};

struct Game {
	Shape box1, box2, box3, box4, box5;
	//Arrays of particles

	Particle particle[MAX_PARTICLES];
	int n;
	Game() {
		n = 0;
	}
};

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

	//declar box1 shape
	game.box1.width = 100;
        game.box1.height = 13;
        game.box1.center.x = 120;
        game.box1.center.y = 700 - 5*60;

        //declare box 2 shape
        game.box2.width = 100;
        game.box2.height = 13;
        game.box2.center.x = 200;
        game.box2.center.y = 650 - 5*60;

	//declare box 3 shape
	game.box3.width = 100;
	game.box3.height = 13;
	game.box3.center.x = 280;
	game.box3.center.y = 600 - 5*60;

        //declare box 4 shape
	game.box4.width = 100;
	game.box4.height = 13;
	game.box4.center.x = 360;
	game.box4.center.y = 550 - 5*60;

	//declare box 5 shape
	game.box5.width = 100;
	game.box5.height = 13;
	game.box5.center.x = 440;
	game.box5.center.y = 500 - 5*60;

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
	//cleanup_fonts();
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
	//initialize_fonts();
}
#define rnd() (float)rand() / (float)RAND_MAX
void makeParticle(Game *game, int x, int y)
{
	if (game->n >= MAX_PARTICLES)
		return;
	//std::cout << "makeParticle() " << x << " " << y << std::endl;
	//position of particle
	Particle *p = &game->particle[game->n];
	p->s.center.x = x;
	p->s.center.y = y;
	p->velocity.y = rnd() *2.0 - 1.0;
	p->velocity.x = rnd() *2.0 - 1.0;
	game->n++;
}

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
	


		//check for collision with shapes...

		Shape *s1, *s2, *s3, *s4, *s5;

		s1= &game->box1;
		if (	p->s.center.y < s1->center.y + s1->height &&
			p->s.center.y > s1->center.y - s1->height &&
			p->s.center.x >= s1->center.x - s1->width &&
			p->s.center.x <= s1->center.x + s1->width) {

		    p->s.center.y = s1->center.y + s1->height;
		    p->velocity.y = -p->velocity.y *0.5;
		    // force pushing particles to the right
		    p->velocity.x = 1;
		}
		
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
                }

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
	float w, h;
	glClear(GL_COLOR_BUFFER_BIT);
	//Draw shapes...

	//draw box1
	Shape *s1;
	glColor3ub(90,140,90);
	s1 = &game->box1;
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

	//draw box2
	Shape *s2;
        glColor3ub(90,140,90);
        s2 = &game->box2;
        glPushMatrix();
        glTranslatef(s2->center.x, s2->center.y, s2->center.z);
        w = s2->width;
        h = s2->height;
        glBegin(GL_QUADS);
                glVertex2i(-w,-h);
                glVertex2i(-w, h);
                glVertex2i( w, h);
                glVertex2i( w,-h);
        glEnd();
        glPopMatrix();

        //draw box3
        Shape *s3;
        glColor3ub(90,140,90);
        s3 = &game->box3;
        glPushMatrix();
        glTranslatef(s3->center.x, s3->center.y, s3->center.z);
        w = s3->width;
        h = s3->height;
        glBegin(GL_QUADS);
                glVertex2i(-w,-h);
                glVertex2i(-w, h);
                glVertex2i( w, h);
                glVertex2i( w,-h);
        glEnd();
        glPopMatrix();

        //draw box4
        Shape *s4;
        glColor3ub(90,140,90);
        s4 = &game->box4;
        glPushMatrix();
        glTranslatef(s4->center.x, s4->center.y, s4->center.z);
        w = s4->width;
        h = s4->height;
        glBegin(GL_QUADS);
                glVertex2i(-w,-h);
                glVertex2i(-w, h);
                glVertex2i( w, h);
                glVertex2i( w,-h);
        glEnd();
        glPopMatrix();

        //draw box5
        Shape *s5;
        glColor3ub(90,140,90);
        s5 = &game->box5;
        glPushMatrix();
        glTranslatef(s5->center.x, s5->center.y, s5->center.z);
        w = s5->width;
        h = s5->height;
        glBegin(GL_QUADS);
                glVertex2i(-w,-h);
                glVertex2i(-w, h);
                glVertex2i( w, h);
                glVertex2i( w,-h);
        glEnd();
        glPopMatrix();

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
}

