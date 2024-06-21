#pragma once

#include "glm/ext.hpp"
#include "../vendor/glm/gtx/matrix_decompose.hpp"

bool DecomposeTransform(const glm::mat4 &transform, glm::vec3 &translation, glm::vec3 &rotation, glm::vec3 &scale);