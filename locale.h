#pragma once

#include "obs-module.h"

#define LOCALE(str) QString::fromUtf8(obs_module_text(str))
