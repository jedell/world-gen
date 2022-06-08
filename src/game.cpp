#include "game.hpp"
#include "./render/draw.hpp"
#include "./render/mesh_gen.hpp"
#include "map"

#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

#define GLSL_VERSION 330

// Utilities
Manager manager;
Camera3D camera;
OrbitControls *controls;
Color *color_map;
int texture_width = 0;
int texture_height = 0;

// Objects
Light light;
Light light2;
Model world;
Model skybox;
Grid *grid;

// Object Storage
std::vector<Model> tiles;
std::vector<Tile> tile_model_map;

// Shaders
Shader atmosphere_shader;
Shader star_shader;

// Collision Ray
Ray mouse_ray = {0};
RayCollision collision = {0};

Model selection_white;
bool selected = false;
int selected_idx = 0;

Vector2 last_mp;

// Shader Locs
int frameLoc;
int frameLoc2;

Game::Game() {}
Game::~Game() {}

void Game::init()
{
    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();

    // size 9
    grid = size_n_grid(9);

    InitWindow(screenWidth, screenHeight, "globe map generator");

    atmosphere_shader = LoadShader("src/resources/shaders/base.vs", "src/resources/shaders/atmosphere.fs");
    star_shader = LoadShader("src/resources/shaders/base.vs", "src/resources/shaders/star.fs");

    int resolutionLoc = GetShaderLocation(atmosphere_shader, "resolution");
    SetShaderValue(atmosphere_shader, resolutionLoc, (float[2]){(float)screenWidth, (float)screenHeight}, SHADER_UNIFORM_VEC2);
    resolutionLoc = GetShaderLocation(star_shader, "resolution");
    SetShaderValue(star_shader, resolutionLoc, (float[2]){(float)screenWidth, (float)screenHeight}, SHADER_UNIFORM_VEC2);

    // Get some atmosphere_shader loactions
    atmosphere_shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(atmosphere_shader, "viewPos");
    star_shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(star_shader, "viewPos");

    // Define the camera to look into our 3d world
    camera = {0};
    camera.position = (Vector3){10.0f, 10.0f, 10.0f}; // Camera position
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};      // Camera looking at point
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                              // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;           // Camera mode type

    light2 = CreateLight(LIGHT_POINT, (Vector3){-500, 0, -100}, Vector3Zero(), WHITE, atmosphere_shader);

    Image generated_texture = LoadImage("textures/color_map.png");
    color_map = LoadImageColors(generated_texture);
    texture_width = generated_texture.width;
    texture_height = generated_texture.height;
    Texture2D texture = LoadTextureFromImage(generated_texture);
    UnloadImage(generated_texture);

    int textureLoc = GetShaderLocation(atmosphere_shader, "texture0");
    SetShaderValue(atmosphere_shader, textureLoc, &texture, SHADER_UNIFORM_SAMPLER2D);

    world = LoadModelFromMesh(GenMeshWorld(grid));
    world.materials[0].shader = atmosphere_shader; // atmos

    world.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    world.materials[0].maps[MATERIAL_MAP_DIFFUSE].value = 0.2;
    world.materials[0].maps[MATERIAL_MAP_SPECULAR].value = 0.1;
    world.materials[0].maps[MATERIAL_MAP_METALNESS].value = 0.01;

    skybox = LoadModelFromMesh(GenMeshCube(20, 20, 20));
    skybox.materials[0].shader = star_shader;

    for (Tile t : grid->tiles)
    {
        Model m = LoadModelFromMesh(GenMeshTile(&t));
        m.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
        m.materials[0].shader = atmosphere_shader;
        tile_model_map.push_back(t);
        tiles.push_back(m);
    }

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    controls = new OrbitControls(camera);

    frameLoc = GetShaderLocation(star_shader, "frame");
    frameLoc2 = GetShaderLocation(atmosphere_shader, "frame");
}

void Game::handleEvents()
{
    Vector2 mp = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        last_mp = GetMousePosition();
        if (mp.x == last_mp.x && mp.y == last_mp.y)
        {
            mouse_ray = GetMouseRay(mp, camera);
            float dist = INT_MAX;
            Model curr_hit;

            int i = 0;
            for (Model m : tiles)
            {
                collision = GetRayCollisionMesh(mouse_ray, m.meshes[0], m.transform);
                if (collision.hit)
                {
                    curr_hit = m;
                    if (collision.distance < dist)
                    {
                        selected_idx = i;
                        selected = true;
                        dist = collision.distance;
                    }
                }
                i++;
            }
        }
    }
}

void Game::update()
{
    controls->update();

    UpdateLightValues(atmosphere_shader, light2);

    // Update the atmosphere_shader with the camera view vector (points towards { 0.0f, 0.0f, 0.0f })
    float cameraPos[3] = {camera.position.x, camera.position.y, camera.position.z};
    SetShaderValue(atmosphere_shader, atmosphere_shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
    SetShaderValue(star_shader, star_shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

    float cur_frame = GetTime();
    SetShaderValue(star_shader, frameLoc, &cur_frame, SHADER_UNIFORM_FLOAT);
    SetShaderValue(atmosphere_shader, frameLoc2, &cur_frame, SHADER_UNIFORM_FLOAT);

    world.transform = MatrixMultiply(world.transform, MatrixRotateXYZ((Vector3){0.0, 0.0001, 0.0}));

    for (int i = 0; i < tiles.size(); i++)
    {
        tiles[i].transform = MatrixMultiply(tiles[i].transform, MatrixRotateXYZ((Vector3){0.0, 0.0001, 0.0}));
    }
}

void Game::render()
{
    BeginDrawing();

    ClearBackground(BLACK);

    BeginMode3D(camera);

    // DrawSphereEx(light2.position, 1, 8, 8, YELLOW);

    // DrawModel(atmos, Vector3Zero(), 1, WHITE);

    rlDisableBackfaceCulling();
    rlDisableDepthMask();
    DrawModel(skybox, (Vector3){0, 0, 0}, 1.0f, WHITE);
    rlEnableBackfaceCulling();
    rlEnableDepthMask();

    DrawModel(world, Vector3Zero(), 1, WHITE);

    // for (const Edge e : grid->edges)
    // {
    //     draw_edge(&e);
    // }

    if (selected)
    {
        Tile t = tile_model_map[selected_idx];
        Color c = get_color(t.v, color_map, texture_width, texture_height);
        c.r = (c.r + 255) / 2;
        c.g = (c.g + 255) / 2;
        c.b = (c.b + 255) / 2;

        selection_white = LoadModelFromMesh(Mesh(tiles[selected_idx].meshes[0]));
        selection_white.transform = MatrixMultiply(tiles[selected_idx].transform, MatrixScale(1, 1, 1));

        // Vector3 o = Vector3Add(t.v, Vector3Scale(Vector3Negate(t.v), 0.99));

        DrawModel(selection_white, Vector3Zero(), 1, c);
        // DrawModel(m, Vector3Zero(), 1, WHITE); // Vector3Add(o, Vector3Zero())
        std::cout << (float)c.r << " " << (float)c.g << " " << (float)c.b << " " << (float)c.a << std::endl;
    }

    EndShaderMode();

    EndMode3D();

    EndDrawing();
}