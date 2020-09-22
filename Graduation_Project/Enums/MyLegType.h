#pragma once

#include "UObject/Class.h"

#include "MyLegType.generated.h"

UENUM()
enum class EMyLegType : uint8
{
	RIGHT_LEG,
	LEFT_LEG,
	MAX
};