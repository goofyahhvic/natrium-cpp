#if !defined(NA_PCH_BASE_HPP)
#define NA_PCH_BASE_HPP

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include <iostream>
#include <fstream>
#include <utility>
#include <algorithm>
#include <functional>
#include <exception>
#include <stdexcept>
#include <filesystem>
#include <chrono>
#include <thread>
#include <limits>

#include <initializer_list>
#include <string>
#include <array>
#include <vector>
#include <deque>
#include <list>
#include <tuple>
#include <memory>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>

#include <fmt/format.h>
#include <fmt/chrono.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Natrium-Renderer/Vulkan.hpp"

#include "Natrium-Core/Template/ArrayList.hpp"
#include "Natrium-Core/Template/ArrayVector.hpp"
#include "Natrium-Core/Template/DoubleList.hpp"

#endif // NA_PCH_BASE_HPP