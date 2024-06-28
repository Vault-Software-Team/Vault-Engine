#pragma once

#include <algorithm>
#include <vector>

template <typename t>
void vector_move(std::vector<t> &v, size_t old_index, size_t new_index) {
    if (old_index > new_index)
        std::rotate(v.rend() - old_index - 1, v.rend() - old_index, v.rend() - new_index);
    else
        std::rotate(v.begin() + old_index, v.begin() + old_index + 1, v.begin() + new_index + 1);
}