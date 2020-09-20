/**
 * All examples are taken from C++ Senioreas blog
 * https://cppsenioreas.wordpress.com/
 *
 * Examples from:
 * cpp_senioreas::maintain_your_iterations_the_iterators_secret_part_1
 */

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
        cont.push_back(val);
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
template <typename T, template <typename...> typename Container>
void init_container(Container<T> &cont, size_t size, T val = T()) {
    while (size--) {
        cont.emplace_back(val);
    }
}

template <typename Iterator>
void print_container(Iterator first, Iterator last) {
    for (; first != last; first++) {
        std::cout << *first << " ";
    }
    std::cout << std::endl;
}
#endif

int main() {
    std::list<int> i_list;
    init_container(i_list, 20, 1);
    size_t counter = 1;
    for (auto current = i_list.begin(); current != i_list.end(); current++) {
        *current = counter;
        // ...
        for (auto inner_current = current; inner_current != i_list.end(); inner_current++) {
            *inner_current *= counter;
            // ...
        }
        // ...
        counter++;
    }
    print_container(i_list.begin(), i_list.end());
    return EXIT_SUCCESS;
}