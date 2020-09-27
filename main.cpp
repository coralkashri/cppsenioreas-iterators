/**
 * All examples are taken from C++ Senioreas blog
 * https://cppsenioreas.wordpress.com/
 *
 * Examples from:
 * cpp_senioreas::maintain_your_iterations_the_iterators_secret_part_1
 * https://cppsenioreas.wordpress.com/2020/09/21/maintain-your-iterations-the-iterators-secret-part-1/
 *
 * cpp_senioreas::maintain_your_iterations_insecure_iterations_part_2
 * https://cppsenioreas.wordpress.com/2020/09/27/maintain-your-iterations-insecure-iterations-part-2/
 */

#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <thread>
#include <mutex>

#define ARTICLE_PART 2

#if ARTICLE_PART == 1
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
void print_container(const Iterator &first, const Iterator &last) {
    for (Iterator current = first; first != last; current++) {
        std::cout << *current << " ";
    }
    std::cout << std::endl;
}
#endif
#endif

#if ARTICLE_PART == 2

class my_system {
public:
    explicit my_system(int val) {
        //values.emplace_back(val);
        value = val;
    };
    ~my_system() = default;
    my_system(const my_system&) = default;
    my_system(my_system&&) = default;
    my_system& operator=(const my_system &ref) = default;
    my_system& operator=(my_system &&ref) = default;

    void func() {
        //std::cout << values[0] << std::endl; // 500 as expected
        std::cout << value << std::endl; // 500 as expected
        std::this_thread::sleep_for(std::chrono::seconds(1));
        //std::cout << values[0] << std::endl; // A garbage value
        std::cout << value << std::endl; // A garbage value
    }

private:
    //std::vector<int> values; // This will make the program crash
    int value; // Make it a vector
};

class my_collection {
public:
    void add_system() {
        std::vector<my_system>::iterator it;
        {
            std::lock_guard g(guard);
            my_systems.emplace_back(my_systems.size() + 500);
            it = std::prev(my_systems.end());
        }
        it->func();
    }

private:
    std::vector<my_system> my_systems;
    std::mutex guard;
};
#endif

int main() {
#if ARTICLE_PART == 1
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
#endif
#if ARTICLE_PART == 2
    my_collection mc;
    std::thread t([&] { mc.add_system(); });
    std::thread t1([&] { mc.add_system(); });

    t.join();
    t1.join();
#endif
    return EXIT_SUCCESS;
}