#include "Math.h"

#include <rttr/registration.h>

using namespace PizzaBox;

//Suppress meaningless and unavoidable warning
#pragma warning( push )
#pragma warning( disable : 26444 )
RTTR_REGISTRATION{
	rttr::registration::class_<Math>("Math")
		.method("PI", &Math::PI)
		.method("ConvertToRadians", &Math::ConvertToRadians)
		.method("ConvertToDegrees", &Math::ConvertToDegrees)
		.method("Infinity", &Math::Infinity)
		.method("Pow", &Math::Pow)
		.method("Sqrt", &Math::Sqrt)
		.method("NearZero", static_cast<float(*)()>(&Math::NearZero))
		.method("NearZero", static_cast<bool(*)(float)>(&Math::NearZero))
		.method("Near", &Math::Near)
		.method("Sin", &Math::Sin)
		.method("Cos", &Math::Cos)
		.method("Tan", &Math::Tan)
		.method("Asin", &Math::Asin)
		.method("Acos", &Math::Acos)
		.method("Atan", &Math::Atan)
		.method("Atan2", &Math::Atan2);
}
#pragma warning( pop )