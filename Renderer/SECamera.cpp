#include "SECamera.h"
#include <cmath>

void LookAt(Camera* cam, vec3 position, vec3 target, vec3 up)
{
	cam->position = position;
	cam->forward = Normalize(target - position);
	cam->right = Normalize(CrossProduct(up, cam->forward));
	cam->up = CrossProduct(cam->forward, cam->right);
}

void UpdateViewMatrix(Camera* cam)
{
	cam->viewMat.SetRow(0, vec4(cam->right.GetX(), cam->up.GetX(), cam->forward.GetX(), 0.0f));
	cam->viewMat.SetRow(1, vec4(cam->right.GetY(), cam->up.GetY(), cam->forward.GetY(), 0.0f));
	cam->viewMat.SetRow(2, vec4(cam->right.GetZ(), cam->up.GetZ(), cam->forward.GetZ(), 0.0f));

	cam->viewMat.SetRow(3, 
		vec4(-DotProduct(cam->position, cam->right), -DotProduct(cam->position, cam->up), -DotProduct(cam->position, cam->forward), 1.0f));
}

//FOR VULKAN, NEED TO NEGATE THE Y VALUE (1, 1) SO THE OBJECT WON'T BE RENDERERD UPSIDE DOWN.
void UpdatePerspectiveProjectionMatrix(Camera* cam)
{
	float inverseAR = 1.0f / cam->aspectRatio;
	float d = 1.0f / tan(cam->vFov / 2.0f);
	float fMinusN = cam->farP - cam->nearP;
	cam->perspectiveProjMat.SetRow(0, inverseAR * d, 0.0f, 0.0f, 0.0f);
	cam->perspectiveProjMat.SetRow(1, 0.0f, d, 0.0f, 0.0f);
	cam->perspectiveProjMat.SetRow(2, 0.0f, 0.0f, cam->farP / fMinusN, 1.0f);
	cam->perspectiveProjMat.SetRow(3, 0.0f, 0.0f, (-cam->nearP * cam->farP) / fMinusN, 0.0f);
}

void UpdateOrthographicProjectionMatrix(Camera* cam)
{
	float fMinusN = cam->farP - cam->nearP;
	float nMinusF = cam->nearP - cam->farP;
	cam->orthographicProjMat.SetRow(0, 2.0f / cam->width, 0.0f, 0.0f, 0.0f);
	cam->orthographicProjMat.SetRow(1, 0.0f, 2.0f / cam->height, 0.0f, 0.0f);
	cam->orthographicProjMat.SetRow(2, 0.0f, 0.0f, 1.0f / fMinusN, 0.0f);
	cam->orthographicProjMat.SetRow(3, 0.0f, 0.0f, cam->nearP / nMinusF, 1.0f);
}

void RotateCamera(Camera* cam, quat rot)
{
	cam->right = Rotate(rot, cam->right);
	cam->up = Rotate(rot, cam->up);
	cam->forward = Rotate(rot, cam->forward);
}

void RotateCameraP(Camera* cam, quat rot)
{
	cam->position = Rotate(rot, cam->position);
}

void MoveLeft(Camera* cam, float d)
{
	cam->position -= d * cam->right;
}

void MoveRight(Camera* cam, float d)
{
	cam->position += d * cam->right;
}

void MoveForward(Camera* cam, float d)
{
	cam->position += d * cam->forward;
}

void MoveBackward(Camera* cam, float d)
{
	cam->position -= d * cam->forward;
}

void MoveUp(Camera* cam, float d)
{
	cam->position += d * cam->up;
}

void MoveDown(Camera* cam, float d)
{
	cam->position -= d * cam->up;
}
