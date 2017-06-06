#include "str.h"
#include <stdlib.h>
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

