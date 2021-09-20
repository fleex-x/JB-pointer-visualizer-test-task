#ifndef RANDOM_CONTAINER_H_
#define RANDOM_CONTAINER_H_


#include <random>
#include <unordered_map>
#include <unordered_set>
#include <cstddef>
#include <utility>
#include <iostream>

namespace test_task {

struct my_randomizer {
    std::size_t operator()(std::size_t left_bound,
                           std::size_t right_bound) {
        static std::mt19937_64 rnd(0);
        return left_bound + (static_cast<std::size_t>(rnd() % (right_bound - left_bound)));
    }
};

template<typename T, 
         typename Randomizer = my_randomizer, 
         typename Hasher = std::hash<T>,
         typename SizeTHasher = std::hash<std::size_t>>
class random_container {
private:
    Randomizer randomizer;
    std::vector<T> elements;
    std::unordered_map<T, std::unordered_set<std::size_t, SizeTHasher>, Hasher> indicies;
public:

    random_container() = default;

    random_container(Randomizer randomizer_, 
                     Hasher hasher = Hasher()) : randomizer(randomizer_),
                                                 indicies(0, hasher) {  
    }
    
    random_container(const random_container &) = default;
    random_container(random_container &&) = default;
    random_container &operator=(const random_container &) = default;
    random_container &operator=(random_container &&) = default;

    void insert(const T& new_elem) {
        elements.push_back(new_elem);
        indicies[new_elem].insert(elements.size() - 1);
    }

    void insert(T &&new_elem) {
        elements.push_back(std::move(new_elem));
        indicies[new_elem].insert(elements.size() - 1);
    }

    template <typename Iter>
    void insert(Iter first, Iter last) {
        elements.reserve(elements.size() + std::distance(first, last));
        for (Iter current_elem = first; current_elem != last; ++current_elem) {
            insert(*current_elem);
        }
    }

    void erase(const T& to_erase) {
        if (indicies[to_erase].empty()) {
            return;
        }

        std::size_t ind_of_to_erase_element = *(indicies[to_erase].begin());
        indicies[to_erase].erase(indicies[to_erase].begin());
        if (ind_of_to_erase_element + 1 == elements.size()) {
            elements.pop_back();
            return;
        }
        
        T &last_elem = elements.back();
        indicies[last_elem].erase(elements.size() - 1);
        indicies[last_elem].insert(ind_of_to_erase_element);
        
        using std::swap;
        swap(elements[ind_of_to_erase_element], elements.back());
        
        elements.pop_back();
    }

    [[nodiscard]] const T &random_element() const & {
        return elements[randomizer(0, elements.size())];
    }
    
    [[nodiscard]] T &random_element() & {
        return elements[randomizer(0, elements.size())];
    }

    [[nodiscard]] T &&random_element() && {
        return std::move(elements[randomizer(0, elements.size())]);
    }

    [[nodiscard]] std::size_t size() const {
        return elements.size();
    }

    [[nodiscard]] bool empty() const {
        return elements.empty();
    }

    void clear() {
        elements.clear();
        indicies.clear();
    }

    class iterator {
    private:
        std::size_t pos = 0;
        random_container *owner = nullptr;
        friend random_container;
        iterator(std::size_t pos_, random_container *owner_) : pos(pos_),
                                                               owner(owner_) {
        }
    public:

        iterator() = default;
        iterator(const iterator &) = default;
        iterator(iterator &&) = default;
        iterator &operator=(const iterator &) = default;
        iterator &operator=(iterator &&) = default;

        T &operator*() const {
            return owner->elements[pos];
        }
        T *operator->() const {
            return *(owner->elements[pos]);
        }

        iterator& operator++() {
            ++pos;
            return *this;
        }

        iterator& operator--() {
            --pos;
            return *this;
        }

        iterator operator++(int) {
            iterator res = *this;
            ++pos;
            return res;
        }

        iterator operator--(int) {
            iterator res = *this;
            --pos;
            return res;
        }

        bool operator==(const iterator &other) {
            return pos == other.pos && owner == other.owner;
        } 
        bool operator!=(const iterator &other) {
            return !(*this == other);
        } 

        explicit operator bool() {
            return owner != nullptr && pos < owner->elements.size();
        }
    }; // iterator invalidates after random_container changes

    iterator begin() {
        return iterator(0, this);
    }

    iterator end() {
        return iterator(size(), this);
    }

    ~random_container() = default;
};

}

#endif //RANDOM_CONTAINER_H_