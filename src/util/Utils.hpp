#include <vector>
#include <algorithm>

namespace utils {
    template<typename T>
    void remove_all(std::vector<T>& vec, const T& value) {
        vec.erase(std::remove(vec.begin(), vec.end(), value), vec.end());
    }

    template<typename T>
    void remove_at(std::vector<T>& vec, size_t index) {
        if (index < vec.size())
            vec.erase(vec.begin() + index);
    }

    template<typename T, typename Predicate>
    void remove_if(std::vector<T>& vec, Predicate pred) {
        vec.erase(std::remove_if(vec.begin(), vec.end(), pred), vec.end());
    }
}