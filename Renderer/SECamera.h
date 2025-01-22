#pragma once

#include "..\Math\SEMath_Header.h"

struct Camera
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
void LookAt(Camera* cam, vec3 position, vec3 target, vec3 up);

//Updates the view matrix of the specified camera.
void UpdateViewMatrix(Camera* cam);

//Updates the perspective projection matrix of the specified camera.
void UpdatePerspectiveProjectionMatrix(Camera* cam);

//Rotates the camera using a rotation quaternion.
//Updates the cameras axes.
void RotateCamera(Camera* cam, quat rot);

//Rotates the camera using a rotation quaternion.
//Updates the cameras position.
void RotateCameraP(Camera* cam, quat rot);

//Moves the camera along its negative x-axis.
void MoveLeft(Camera* cam, float d);

//Moves the camera along its positive x-axis.
void MoveRight(Camera* cam, float d);

//Moves the camera along its positive z-axis.
void MoveForward(Camera* cam, float d);

//Moves the camera along its negative z-axis.
void MoveBackward(Camera* cam, float d);

//Moves the camera along its positive y-axis.
void MoveUp(Camera* cam, float d);

//Moves the camera along its negative y-axis.
void MoveDown(Camera* cam, float d);
