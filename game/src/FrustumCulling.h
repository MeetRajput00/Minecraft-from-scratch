#ifndef FRUSTUM_CULLING
#define FRUSTUM_CUllING
#include <Windows.h>
#include "Transform.h"
boolean IsInFrustumSphere(const Sphere *sphere);
boolean IsInFrustumRectangle(const Rectangle3D *rect);
void ExtractFrustum();
#endif