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
#include <functional>

#define ARTICLE_PART 3

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

#if ARTICLE_PART == 3

/*
 * Concepts Declarations
 */

template <template <typename ...> typename Container, typename T>
concept IteratorsFriendlyContainerType = requires (Container<T> container) {
    typename decltype(container)::pointer;
    typename decltype(container)::const_pointer;
    typename decltype(container)::reference;
    typename decltype(container)::const_reference;
    typename decltype(container)::iterator;
    typename decltype(container)::const_iterator;
    typename decltype(container)::reverse_iterator;
    typename decltype(container)::const_reverse_iterator;
    typename decltype(container)::size_type;
};

template <typename T>
concept HasMul = requires() {
    { T() * T() };
};

template <typename T>
concept IteratorTag = std::is_base_of_v<std::input_iterator_tag, T> || std::is_base_of_v<std::output_iterator_tag, T>;

/*
 * Base iterators structures
 */

template <IteratorTag ItTag, typename T>
struct base_legacy_iterator {
    typedef ItTag	                        iterator_category;
    typedef T		                        value_type;
    typedef T*                              pointer;
    typedef T&	                            reference;
    typedef std::iter_difference_t<pointer> difference_type;
    // Iterator Concept: if (ItTag == random_access_iterator_tag) std::contiguous_iterator_tag; else ItTag;
    using iterator_concept = std::conditional<std::is_same_v<ItTag, std::random_access_iterator_tag>, std::contiguous_iterator_tag, ItTag>;
};

template <template <typename ...> typename Container, typename T>
requires IteratorsFriendlyContainerType<Container, T>
struct use_collection_iterator {
    typedef typename Container<T>::pointer pointer;
    typedef typename Container<T>::const_pointer const_pointer;
    typedef typename Container<T>::reference reference;
    typedef typename Container<T>::const_reference const_reference;
    typedef typename Container<T>::iterator iterator;
    typedef typename Container<T>::const_iterator const_iterator;
    typedef typename Container<T>::reverse_iterator reverse_iterator;
    typedef typename Container<T>::const_reverse_iterator const_reverse_iterator;
    typedef typename Container<T>::size_type size_type;
};

template <template <typename ...> typename IteratorType, typename Item>
requires (!IteratorsFriendlyContainerType<IteratorType, Item>)
struct use_collection_custom_iterator {
    typedef IteratorType<Item> iterator;
    typedef IteratorType<const Item> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef typename iterator::pointer pointer;
    typedef typename const_iterator::pointer const_pointer;
    typedef typename iterator::reference reference;
    typedef typename const_iterator::reference const_reference;
    typedef size_t size_type;
};

/*
 * Custom Iterators
 */

template <typename T>
class my_iterator : public base_legacy_iterator<std::random_access_iterator_tag, T> {
public:
    // A convenient access to inherited type members
    using base_it = base_legacy_iterator<std::random_access_iterator_tag, T>;

    // Ctors & Assign Operators & Dtor
    explicit my_iterator<T>(T* data = nullptr) : _data(data) {}
    my_iterator<T>(const my_iterator<T>&) = default;
    my_iterator<T>(my_iterator<T>&&) noexcept = default;
    my_iterator<T>& operator=(const my_iterator<T>&) = default;
    my_iterator<T>& operator=(my_iterator<T>&&) noexcept = default;
    my_iterator<T>& operator=(T* data) { _data = data; return *this; };
    ~my_iterator<T>() = default;

    // Is null
    explicit operator bool() const { return _data; }

    /* Required Operators For Iterator Category */

    // Required Compare Operators
    bool operator==(const my_iterator<T>& ref) const { return _data == ref._data; }
    bool operator!=(const my_iterator<T>& ref) const { return !(*this == ref); }
    typename base_it::difference_type operator-(const my_iterator<T>& ref) { return std::distance(ref._data, _data); }

    // Required Action Operators
    my_iterator<T>& operator+=(const typename base_it::difference_type& diff) { _data += diff; return *this; }
    my_iterator<T>& operator-=(const typename base_it::difference_type& diff) { _data -= diff; return *this; }
    my_iterator<T>& operator++() { ++_data; return *this; }
    my_iterator<T> operator++(int) { auto temp = *this; ++_data; return temp; }
    my_iterator<T>& operator--() { --_data; return *this; }
    my_iterator<T> operator--(int) { auto temp = *this; --_data; return temp; }
    my_iterator<T> operator+(const typename base_it::difference_type& diff) { auto temp = *this; temp._data += diff; return temp; }
    my_iterator<T> operator-(const typename base_it::difference_type& diff) { auto temp = *this; temp._data -= diff; return temp; }

    // Required Access Operators
    T& operator*() { return *_data; }
    const T& operator*() const  { return *_data; }
    T* operator->() const  { return _data; }

private:
    T* _data;
};

/*
 * Items & Collections
 */

template <HasMul T>
class my_item {
public:
    my_item(const T &a, const T &b) : value_a(a), value_b (b) {}

    T item() {
        return value_a * value_b;
    }

    void set_a(T a) {
        value_a = a;
    }

    void set_b(T b) {
        value_b = b;
    }

private:
    T value_a, value_b;
};

template <template <typename ...> typename Container, HasMul ItemT>
using my_collection_iterator = use_collection_iterator<Container, my_item<ItemT>>;

template <template <typename ...> typename Container, HasMul ItemT>
class my_item_collection : public my_collection_iterator<Container, ItemT> {
public:

    // A convenient access to inherited type members
    using base_types = my_collection_iterator<Container, ItemT>;

    /* Custom collection operations */

    void add_item(const ItemT &a, const ItemT &b) {
        items.emplace_back(a, b);
    }

    typename base_types::iterator get_item(size_t idx) {
        auto res = items.begin();
        return std::advance(res, idx);
    }

    /* Required Iterators Access Methods */

    typename base_types::iterator begin() { return items.begin(); }
    typename base_types::iterator end() { return items.end(); }
    [[nodiscard]] typename base_types::const_iterator cbegin() const { return items.cbegin(); }
    [[nodiscard]] typename base_types::const_iterator cend() const { return items.cend(); }
    typename base_types::reverse_iterator rbegin() { return items.rbegin(); }
    typename base_types::reverse_iterator rend() { return items.rend(); }
    [[nodiscard]] typename base_types::const_reverse_iterator crbegin() const { return items.crbegin(); }
    [[nodiscard]] typename base_types::const_reverse_iterator crend() const { return items.crend(); }

    [[nodiscard]] bool empty() const { return items.empty(); }
    [[nodiscard]] typename base_types::size_type size() const { return items.size(); }

private:
    Container<my_item<ItemT>> items;
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
#if ARTICLE_PART == 3
    my_item_collection<std::list, int> list_collection;
    my_item_collection<std::vector, int> vector_collection;
    list_collection.add_item(1, 2);
    list_collection.add_item(3, 4);
    list_collection.add_item(5, 6);

    vector_collection.add_item(7, 8);
    vector_collection.add_item(9, 10);
    vector_collection.add_item(11, 12);

    for (auto &elem : list_collection) {
        std::cout << elem.item() << std::endl;
    }

    for (auto &elem : vector_collection) {
        std::cout << elem.item() << std::endl;
    }
#endif
    return EXIT_SUCCESS;
}