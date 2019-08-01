#pragma once

#include "Array.h"
#include "Base.h"
#include "Data.h"
#include "Geometry.h"
#include "Input.h"
#include "Ref.h"
#include "RefPtr.h"
#include "RefVector.h"
#include "Scheduler.h"
#include "TextFormat.h"
#include "Touch.h"
#include "TweenFunction.h"
#include "Types.h"
#include "Value.h"
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"

#include <AzCore/Math/Vector2.h>
#include <AzCore/Math/Vector3.h>
#include <AzCore/Math/Vector4.h>
#include <AzCore/Math/Matrix3x3.h>
#include <AzCore/Math/Transform.h>
#include <AzCore/Math/Quaternion.h>

#ifndef N_PI
#define N_PI (3.1415926535897932384626433832795028841971693993751f)
#endif

#if _WIN32

#include <BaseTsd.h>
#ifndef __SSIZE_T
#define __SSIZE_T
typedef SSIZE_T ssize_t;
#endif // __SSIZE_T

#endif