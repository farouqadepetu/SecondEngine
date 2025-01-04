#include "SECamera.h"

void LookAt(SECamera* cam, vec3 position, vec3 target, vec3 up)
{
	cam->position = position;
	cam->forward = Normalize(target - position);
	cam->right = Normalize(CrossProduct(up, cam->forward));
	cam->up = CrossProduct(cam->forward, cam->right);
}

void UpdateViewMatrix(SECamera* cam)
{
	cam->viewMat.SetRow(0, vec4(cam->right.GetX(), cam->up.GetX(), cam->forward.GetX(), 0.0f));
	cam->viewMat.SetRow(1, vec4(cam->right.GetY(), cam->up.GetY(), cam->forward.GetY(), 0.0f));
	cam->viewMat.SetRow(2, vec4(cam->right.GetZ(), cam->up.GetZ(), cam->forward.GetZ(), 0.0f));

	cam->viewMat.SetRow(3, 
		vec4(-DotProduct(cam->position, cam->right), -DotProduct(cam->position, cam->up), -DotProduct(cam->position, cam->forward), 1.0f));
}

//FOR VULKAN, NEED TO NEGATE THE Y VALUE (1, 1) SO THE OBJECT WON'T BE RENDERERD UPSIDE DOWN.
void UpdatePerspectiveProjectionMatrix(SECamera* cam)
{
	float inverseAR = 1.0f / cam->aspectRatio;
	float d = 1.0f / tan(cam->vFov / 2.0f);
	float fMinusN = cam->farP - cam->nearP;
	cam->perspectiveProjMat.SetRow(0, inverseAR * d, 0.0f, 0.0f, 0.0f);
	cam->perspectiveProjMat.SetRow(1, 0.0f, d, 0.0f, 0.0f);
	cam->perspectiveProjMat.SetRow(2, 0.0f, 0.0f, cam->farP / fMinusN, 1.0f);
	cam->perspectiveProjMat.SetRow(3, 0.0f, 0.0f, (-cam->nearP * cam->farP) / fMinusN, 0.0f);
}

void RotateCamera(SECamera* cam, quat rot)
{
	cam->right = Rotate(rot, cam->right);
	cam->up = Rotate(rot, cam->up);
	cam->forward = Rotate(rot, cam->forward);
}

void RotateCameraP(SECamera* cam, quat rot)
{
	cam->position = Rotate(rot, cam->position);
}

void MoveLeft(SECamera* cam, float d)
{
	cam->position -= d * cam->right;
}

void MoveRight(SECamera* cam, float d)
{
	cam->position += d * cam->right;
}

void MoveForward(SECamera* cam, float d)
{
	cam->position += d * cam->forward;
}

void MoveBackward(SECamera* cam, float d)
{
	cam->position -= d * cam->forward;
}

void MoveUp(SECamera* cam, float d)
{
	cam->position += d * cam->up;
}

void MoveDown(SECamera* cam, float d)
{
	cam->position -= d * cam->up;
}
