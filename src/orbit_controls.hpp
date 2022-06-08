#pragma once

#include <math.h>
#include <raylib.h>

#define CAMERA_THIRD_PERSON_MIN_DISTANCE_CLAMP 1.5f
#define CAMERA_THIRD_PERSON_MAX_DISTANCE_CLAMP 10.0f
#define CAMERA_MOUSE_SCROLL_SENSITIVITY 0.1f
#define CAMERA_FREE_MIN_CLAMP 85.0f
#define CAMERA_FREE_MAX_CLAMP -85.0f
#define CAMERA_FREE_MOUSE_SENSITIVITY 0.1f

// Adopted from rcamera.h module to fit my own needs;

class OrbitControls
{
    Camera3D *camera;
    float targetDistance;          // Camera distance from position to target
    float playerEyesPosition;      // Player eyes position from ground (in meters)
    Vector2 angle;                 // Camera angle in plane XZ
    Vector2 previousMousePosition; // Previous mouse position

public:
    OrbitControls(Camera3D &c)
    {
        camera = &c;
        Vector3 v1 = camera->position;
        Vector3 v2 = camera->target;

        float dx = v2.x - v1.x;
        float dy = v2.y - v1.y;
        float dz = v2.z - v1.z;

        targetDistance = sqrtf(dx * dx + dy * dy + dz * dz); // Distance to target

        // Camera angle calculation
        angle.x = atan2f(dx, dz);                       // Camera angle in plane XZ (0 aligned with Z, move positive CCW)
        angle.y = atan2f(dy, sqrtf(dx * dx + dz * dz)); // Camera angle in plane XY (0 aligned with X, move positive CW)

        playerEyesPosition = camera->position.y; // Init player eyes position to camera Y position

        previousMousePosition = GetMousePosition(); // Init mouse position
    }
    ~OrbitControls() {}

    void update()
    {
        Vector2 mousePositionDelta = {0.0f, 0.0f};
        Vector2 mousePosition = GetMousePosition();
        float mouseWheelMove = GetMouseWheelMove();

        mousePositionDelta.x = mousePosition.x - previousMousePosition.x;
        mousePositionDelta.y = mousePosition.y - previousMousePosition.y;

        previousMousePosition = mousePosition;

        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            // Camera rotation
            angle.x += mousePositionDelta.x * -CAMERA_FREE_MOUSE_SENSITIVITY * (targetDistance / 100);
            angle.y += mousePositionDelta.y * -CAMERA_FREE_MOUSE_SENSITIVITY * (targetDistance / 100);

            // Angle clamp
            if (angle.y > CAMERA_FREE_MIN_CLAMP * DEG2RAD)
                angle.y = CAMERA_FREE_MIN_CLAMP * DEG2RAD;
            else if (angle.y < CAMERA_FREE_MAX_CLAMP * DEG2RAD)
                angle.y = CAMERA_FREE_MAX_CLAMP * DEG2RAD;
        }

        targetDistance -= (mouseWheelMove * CAMERA_MOUSE_SCROLL_SENSITIVITY); // Camera zoom

        // Camera distance clamp
        if (targetDistance < CAMERA_THIRD_PERSON_MIN_DISTANCE_CLAMP)
            targetDistance = CAMERA_THIRD_PERSON_MIN_DISTANCE_CLAMP;
        else if (targetDistance > CAMERA_THIRD_PERSON_MAX_DISTANCE_CLAMP)
            targetDistance = CAMERA_THIRD_PERSON_MAX_DISTANCE_CLAMP;

        // Update camera position with changes
        camera->position.x = -sinf(angle.x) * targetDistance * cosf(angle.y) + camera->target.x;
        camera->position.y = -sinf(angle.y) * targetDistance + camera->target.y;
        camera->position.z = -cosf(angle.x) * targetDistance * cosf(angle.y) + camera->target.z;
    }
};
