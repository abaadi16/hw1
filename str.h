#ifndef STR_H_
#define STR_H_

#define MAX_PARTICLES 10000

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
#endif
