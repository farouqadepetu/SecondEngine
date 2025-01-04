#pragma once

#include "math_header.h"

struct SECamera
{
	vec3 position;
	vec3 forward;
	vec3 right;
	vec3 up;

	mat4 viewMat;

	float aspectRatio;
	float vFov;
	float nearP;
	float farP;

	mat4 perspectiveProjMat;
};

//Defines the camera space.
void LookAt(SECamera* cam, vec3 position, vec3 target, vec3 up);

//Updates the view matrix of the specified camera.
void UpdateViewMatrix(SECamera* cam);

//Updates the perspective projection matrix of the specified camera.
void UpdatePerspectiveProjectionMatrix(SECamera* cam);

//Rotates the camera using a rotation quaternion.
//Updates the cameras axes.
void RotateCamera(SECamera* cam, quat rot);

//Rotates the camera using a rotation quaternion.
//Updates the cameras position.
void RotateCameraP(SECamera* cam, quat rot);

//Moves the camera along its negative x-axis.
void MoveLeft(SECamera* cam, float d);

//Moves the camera along its positive x-axis.
void MoveRight(SECamera* cam, float d);

//Moves the camera along its positive z-axis.
void MoveForward(SECamera* cam, float d);

//Moves the camera along its negative z-axis.
void MoveBackward(SECamera* cam, float d);

//Moves the camera along its positive y-axis.
void MoveUp(SECamera* cam, float d);

//Moves the camera along its negative y-axis.
void MoveDown(SECamera* cam, float d);
