#include <iostream>
#include <vector>
#include <list>
#include <set>

#define USE_ITERATORS true

#if !USE_ITERATORS
template <typename T>
class my_list : public std::list<T> {
public:
    T& operator[](size_t index) {
        auto current = this->begin();
        for (; current != this->end() && index--; current = std::next(current));
        return *current;
    }
};

template <typename T, template <typename...> typename Container>
void init_container(Container<T> &cont, size_t size, T val = T()) {
    while (size--) {
        cont.emplace(cont.begin(), val);
    }
}

template <typename T, template <typename...> typename Container>
void print_container(Container<T> &cont) {
    for (size_t i = 0; i < cont.size(); i++) {
        std::cout << cont[i] << " ";
    }
    std::cout << std::endl;
}
#else

#endif

int main() {
    my_list<int> i_list;
    init_container(i_list, 20);
    for (size_t i = 0; i < i_list.size(); i++) {
        i_list[i] = i;
        // ...
        for (size_t j = i; j < i_list.size(); j++) {
            i_list[j] *= i;
            // ...
        }
        // ...
    }
    print_container(i_list);
    return EXIT_SUCCESS;
}