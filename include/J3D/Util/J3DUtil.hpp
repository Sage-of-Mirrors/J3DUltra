#pragma once

#include <vector>
#include <string>
#include <type_traits>
#include <filesystem>
#include <cstddef>
#include <algorithm>

namespace bStream { class CStream; }

namespace J3DUtility {
    // Returns the index of the given element in the given vector, or -1 if the element is not in that vector.
    template<typename T>
    ptrdiff_t VectorIndexOf(const std::vector<T>& vec, const T& elem)
    {
        ptrdiff_t result = -1;

        auto it = std::find(vec.begin(), vec.end(), elem);
        if (it != vec.end())
            result = it - vec.begin();

        return result;
    }

    // Returns whether the given element is contained in the given vector.
    template<typename T>
    bool VectorContains(const std::vector<T>& vec, const T& elem)
    {
        return VectorIndexOf(vec, elem) != -1;
    }

    // Returns whether the given element is contained in the given vector, with the index parameter set to the element's position.
    template<typename T>
    bool VectorContains(const std::vector<T>& vec, const T& elem, ptrdiff_t& index)
    {
        index = VectorIndexOf(vec, elem);
        return index != -1;
    }

    template<typename E>
    auto EnumToIntegral(E e) -> typename std::underlying_type<E>::type
    {
        return static_cast<typename std::underlying_type<E>::type>(e);
    }

    std::string LoadTextFile(std::filesystem::path filePath);

    void PadStreamWithString(bStream::CStream* stream, uint32_t padValue, std::string str = "");
}
