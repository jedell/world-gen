#ifndef mesh_gen_hpp
#define mesh_gen_hpp

#include "../game.hpp"

Mesh GenMeshWorld(Grid *g)
{
    Mesh mesh = {0};
    mesh.triangleCount = ((g->tiles.size() - 12) * 6) + (12 * 5);
    mesh.vertexCount = mesh.triangleCount * 3;
    mesh.vertices = (float *)MemAlloc(mesh.vertexCount * 3 * sizeof(float));  // 3 vertices, 3 coordinates each (x, y, z)
    mesh.texcoords = (float *)MemAlloc(mesh.vertexCount * 2 * sizeof(float)); // 3 vertices, 2 coordinates each (x, y)
    mesh.normals = (float *)MemAlloc(mesh.vertexCount * 3 * sizeof(float));

    std::vector<Vector3> vertices;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords;

    for (int k = 0; k < g->tiles.size(); k++)
    {
        Tile *t = &g->tiles[k];
        for (int i = 0; i < t->corners.size(); i++)
        {
            if (i < t->corners.size() - 1)
            {
                vertices.push_back(t->v);
                vertices.push_back(t->corners[i]->v);
                vertices.push_back(t->corners[i + 1]->v);
            }
            else
            {
                vertices.push_back(t->v);
                vertices.push_back(t->corners[i]->v);
                vertices.push_back(t->corners[0]->v);
            }
        }

        // Normals definition
        for (int n = 0; n < t->corners.size() * 3; n++)
            normals.push_back(Vector3Normalize(t->v));

        // TexCoords definition
        for (int n = 0; n < t->corners.size() * 3; n++)
        {
            Vector3 d = Vector3Negate(Vector3Normalize(t->v));
            float u = 0.5 + atan2(d.x, d.z) / (2 * M_PI);
            float v = 0.5 + asin(d.y) / M_PI;
            texcoords.push_back((Vector2){u, v}); //(Vector2){0.0f, 0.0f};
        }
    }

    // Mesh vertices position array
    for (int i = 0; i < mesh.vertexCount; i++)
    {
        mesh.vertices[3 * i] = vertices[i].x;
        mesh.vertices[3 * i + 1] = vertices[i].y;
        mesh.vertices[3 * i + 2] = vertices[i].z;
    }

    // Mesh texcoords array
    for (int i = 0; i < mesh.vertexCount; i++)
    {
        mesh.texcoords[2 * i] = texcoords[i].x;
        mesh.texcoords[2 * i + 1] = texcoords[i].y;
    }

    // Mesh normals array
    for (int i = 0; i < mesh.vertexCount; i++)
    {
        mesh.normals[3 * i] = normals[i].x;
        mesh.normals[3 * i + 1] = normals[i].y;
        mesh.normals[3 * i + 2] = normals[i].z;
    }

    // RL_FREE(&vertices);
    // RL_FREE(&normals);
    // RL_FREE(&texcoords);

    UploadMesh(&mesh, false);

    return mesh;
}

Mesh GenMeshTile(Tile *t)
{
    Mesh mesh = {0};
    mesh.triangleCount = t->edges.size();
    mesh.vertexCount = mesh.triangleCount * 3;
    mesh.vertices = (float *)MemAlloc(mesh.vertexCount * 3 * sizeof(float));  // 3 vertices, 3 coordinates each (x, y, z)
    mesh.texcoords = (float *)MemAlloc(mesh.vertexCount * 2 * sizeof(float)); // 3 vertices, 2 coordinates each (x, y)
    mesh.normals = (float *)MemAlloc(mesh.vertexCount * 3 * sizeof(float));

    Vector3 *vertices = (Vector3 *)RL_MALLOC(mesh.vertexCount * sizeof(Vector3));
    for (int i = 0; i < t->corners.size(); i++)
    {
        if (i < t->corners.size() - 1)
        {
            vertices[i * 3 + 0] = t->v;
            vertices[i * 3 + 1] = t->corners[i]->v;
            vertices[i * 3 + 2] = t->corners[i + 1]->v;
        }
        else
        {
            vertices[i * 3 + 0] = t->v;
            vertices[i * 3 + 1] = t->corners[i]->v;
            vertices[i * 3 + 2] = t->corners[0]->v;
        }
    }

    // Normals definition
    Vector3 *normals = (Vector3 *)RL_MALLOC(mesh.vertexCount * sizeof(Vector3));
    for (int n = 0; n < mesh.vertexCount; n++)
        normals[n] = Vector3Normalize(t->v);

    // TexCoords definition
    Vector2 *texcoords = (Vector2 *)RL_MALLOC(mesh.vertexCount * sizeof(Vector2));
    for (int n = 0; n < mesh.vertexCount; n++)
    {
        Vector3 d = Vector3Negate(Vector3Normalize(t->v));
        float u = 0.5 + atan2(d.x, d.z) / (2 * M_PI);
        float v = 0.5 + asin(d.y) / M_PI;
        texcoords[n] = (Vector2){u, v}; //(Vector2){0.0f, 0.0f};
    }

    // Mesh vertices position array
    for (int i = 0; i < mesh.vertexCount; i++)
    {
        mesh.vertices[3 * i] = vertices[i].x;
        mesh.vertices[3 * i + 1] = vertices[i].y;
        mesh.vertices[3 * i + 2] = vertices[i].z;
    }

    // Mesh texcoords array
    for (int i = 0; i < mesh.vertexCount; i++)
    {
        mesh.texcoords[2 * i] = texcoords[i].x;
        mesh.texcoords[2 * i + 1] = texcoords[i].y;
    }

    // Mesh normals array
    for (int i = 0; i < mesh.vertexCount; i++)
    {
        mesh.normals[3 * i] = normals[i].x;
        mesh.normals[3 * i + 1] = normals[i].y;
        mesh.normals[3 * i + 2] = normals[i].z;
    }

    RL_FREE(vertices);
    RL_FREE(normals);
    RL_FREE(texcoords);

    UploadMesh(&mesh, false);

    return mesh;
}

#endif