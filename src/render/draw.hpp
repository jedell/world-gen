#ifndef draw_hpp
#define draw_hpp

#include "../game.hpp"

void draw_triangle_fan(Vector3 center, const Corner *const *points, int pointCount, Color color)
{
    if (pointCount >= 3)
    {
        rlCheckRenderBatchLimit((pointCount - 2) * 4);

        rlBegin(RL_TRIANGLES);

        rlColor4ub(color.r, color.g, color.b, color.a);

        for (int i = 0; i < pointCount; i++)
        {
            if (i < pointCount - 1)
            {
                rlVertex3f(center.x, center.y, center.z);
                rlVertex3f(points[i]->v.x, points[i]->v.y, points[i]->v.z);
                rlVertex3f(points[i + 1]->v.x, points[i + 1]->v.y, points[i + 1]->v.z);
            }
            else
            {
                rlVertex3f(center.x, center.y, center.z);
                rlVertex3f(points[i]->v.x, points[i]->v.y, points[i]->v.z);
                rlVertex3f(points[0]->v.x, points[0]->v.y, points[0]->v.z);
            }
        }

        rlEnd();
    }
}

void draw_tile(const Tile *t, Color color)
{
    auto points = &t->corners[0];
    draw_triangle_fan(t->v, points, t->corners.size(), color);
}

void draw_edge(const Edge *e)
{
    DrawLine3D(e->corners[0]->v, e->corners[1]->v, WHITE);
}

#endif