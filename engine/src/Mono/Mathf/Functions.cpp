#include <Engine/Mono/Audio/Functions.hpp>
#include <Engine/Audio.hpp>
#include <Engine/Mono/HelperFunctions.hpp>
#include <algorithm>

namespace Engine {
    namespace CSharpInternalFunctions::Mathf {
        float Deg2Rad(float value) {
            return glm::radians(value);
        }
        float Rad2Deg(float value) {
            return glm::degrees(value);
        }
        float Abs(float value) {
            return glm::abs(value);
        }
        float Acos(float value) {
            return glm::acos(value);
        }
        float Asin(float value) {
            return glm::asin(value);
        }
        float Atan(float value) {
            return glm::atan(value);
        }
        float Atan2(float x, float y) {
            return std::atan2(x, y);
        }
        float Ceil(float value) {
            return glm::ceil(value);
        }
        float Clamp(float value, float min, float max) {
            return std::clamp(value, min, max);
        }
        float Cos(float value) {
            return glm::cos(value);
        }
        float Sin(float value) {
            return glm::sin(value);
        }
        float Sqrt(float value) {
            return glm::sqrt(value);
        }
        float Tan(float value) {
            return glm::tan(value);
        }
        float Round(float value) {
            return glm::round(value);
        }
        float Pow(float x, float y) {
            return std::pow(x, y);
        }
        float Log(float value) {
            return std::log(value);
        }
        float Log10(float value) {
            return std::log10(value);
        }
        float Max(float x, float y) {
            return std::max(x, y);
        }
        float Min(float x, float y) {
            return std::min(x, y);
        }
        float Exp(float x) {
            return std::exp(x);
        }
        float Lerp(float a, float b, float t) {
            return a + t * (b - a);
        }
        static bool seed_set = false;
        int RandomRange(int min, int max) {
            if (!seed_set) {
                srand((uint32_t)time(NULL));
                seed_set = true;
            }
            int range = (min - max) + 1;
            int rnd = min + rand() % range;

            return rnd;
        }

        float FloatRandomRange(float min, float max) {
            if (!seed_set) {
                srand((uint32_t)time(NULL));
                seed_set = true;
            }
            float val = ((float)rand() / RAND_MAX) * (max - min) + min;
            return val;
        }
    } // namespace CSharpInternalFunctions::Mathf
} // namespace Engine