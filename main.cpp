#include <iostream>
#include <stdio.h>
#include <cstdlib>

const int DEFAULT_SIZE = 8;

template<typename T>
class ArrayList final {

public: // all stuff in class

    T* buff; // аллоцированная память
    int size_ = 0; // скок элементов
    int capacity_ = 0; // размер массива

    //void extend();
    //void insert(index, value);
    //void insert() // default in the end
    //void remove(index) //throw element from array

    class Iterator {};
    class ConstIterator {};

    T& operator[](int index) {
        return *(buff + index);
    }

    const T& operator[](int index) const {
        return *(buff + index);
    }

    int size() {
        return size_;
    };

public:

    // default constr
    ArrayList() : ArrayList(DEFAULT_SIZE) {};
    // constr
    explicit ArrayList(int size) {
        capacity_ = size;
        buff = (T*) malloc(sizeof(T) * size_);
    }

    // copy constr
    ArrayList(const ArrayList& rsh) {
        capacity_ = rsh.capacity_;
        buff = (T*) malloc(sizeof(T) * capacity_);
        size_ = rsh.size_;

        for (int i = 0; i < size_; i++) {
            new(buff + i)T(std::is_nothrow_move_constructible<T>::value ? std::move_if_noexcept(rsh[i]) : rsh[i]);
        }
    }

    // copy operator
    ArrayList& operator=(ArrayList const& smth) {
        if (this != &smth) {
            if (std::is_destructible<T>::value) {
                for (int i = 0; i < size_; i++) {
                    buff[i].~T();
                }
            }
            free(buff);
            size_ = smth.size_;
            capacity_ = smth.capacity_;

            buff = (T*) malloc(sizeof(T) * capacity_);
            for (int i = 0; i < size_; i++) {
                new(buff + i)T(std::is_nothrow_move_constructible<T>::value ? std::move_if_noexcept(smth.buff[i]) : smth.buff[i]);
            }
        }
        return *this;
    }

    // move constr
    ArrayList(ArrayList<T>&& rhs)  noexcept {
        capacity_ = rhs.capacity_;
        buff = rhs.buff;
        size_ = rhs.size_;

        rhs.buff = nullptr;
        rhs.size_ = 0;
    }

    // move operator
    ArrayList& operator=(ArrayList&& smth)  noexcept {
        if (this != &smth) {
            capacity_ = smth.capacity_;
            size_ = smth.size_;
            buff = smth.buff;

            smth.buff = nullptr;
            smth.capacity_ = 0;
            smth.size_ = 0;
        }
    }

    // destructor
    ~ArrayList() {
        for (int i = 0; i < capacity_; i++) {
            buff[i].~T();
        }
        free(buff);
        size_ = 0;
        capacity_ = 0;
    }

    // make array bigger if it is necessary
    void extend()
    {
        capacity_ *= 2;

        T* oldArray = buff;
        buff = (T*) malloc(sizeof(T) * capacity_);

        for (int i = 0; i < size_; i++)
        {
            new(buff + i)T(std::is_nothrow_move_constructible<T>::value ? std::move_if_noexcept(oldArray[i]) : oldArray[i]);
            oldArray[i].~T();
        }
        free(oldArray);
    }

    // insert element in existing place in array, do nothing if wrong index
    int insert(int index, const T& value)
    {
        if (index < 0 || index > size_) return -1;

        if (size_ == capacity_) extend();

        if (index < size_)
        {
            for (int i = size_; i > index; i--)
            {
                new(buff + i)T(std::is_nothrow_move_constructible<T>::value ? std::move_if_noexcept(buff[i-1]) : buff[i-1]);
                buff[i-1].~T();
            }
        }

        new(buff + index)T(value);
        size_++;

        return index;
    }

    int insert(const T& value)
    {
        return insert(size_, value);
    }

    void remove(int index) {
        if (index < 0 || index > size_) {
            return;
        }
        buff[index].~T();
        size_--;

        for (int i = index + 1; i < size_ + 1; i++) {
            new(buff + i - 1)T(std::is_nothrow_move_constructible<T>::value ? std::move_if_noexcept(buff[i]) : buff[i]);
            buff[i].~T();
        }
    }

    Iterator iterator () {
        return Iterator(*this);
    }
};

template<typename T>
class ConstIterator {
    const ArrayList<T> vector;
    int curr = 0;
    bool reverse;

    ConstIterator(const ArrayList<T> vector, bool isReversed) : vector(vector), reverse(isReversed) {};

    void set(const T& value) {
        new(curr)T(std::is_nothrow_move_constructible<T>::value ? std::move_if_noexcept(value) : value);
    }

    void toTheNext() {
        reverse ? curr-- : curr++;
    }

    void hasNext() {
        return reverse ? curr >=0 : curr < vector.size_;
    }

    const T& get() const {
        return vector[curr];
    }

    ConstIterator& operator++() {
        curr++;
        return *this;
    }

    ConstIterator& operator--() {
        curr--;
        return *this;
    }
};

template<typename T>
class Iterator {
    ArrayList<T> vector;
    int curr = 0;
    bool reverse;

    Iterator(ArrayList<T> vector, bool isReversed) : vector(vector), reverse(reverse) {};

    void set(const T& value) {
        new(curr)T(std::is_nothrow_move_constructible<T>::value ? std::move_if_noexcept(value) : value);
    }

    void toNext() {
        reverse ? curr-- : curr++;
    }

    void hasNext() {
        return reverse ? curr >=0 : curr < vector.size_;
    }

    const T& get() const {
        return vector[curr];
    }

    Iterator& operator++() {
        curr++;
        return *this;
    }

    Iterator& operator--() {
        curr--;
        return *this;
    }

    Iterator iterator() {
        return (*this);
    }
};

int main() {

    ArrayList<int> vec;
    vec.insert(0, 6);
    vec.insert(1, 7);
    vec.insert(2, 8);
    vec.insert(3, 8);
    vec.insert(2, 4);


    ArrayList<int> vec1(1);
    vec1.insert(0, 6);
    vec1.insert(1, 7);
    vec1.insert(2, 8);
    vec1.insert(3, 8);
    vec1.insert(4, 8);

    ArrayList<int> vec2;
    vec2.insert(0, 6);
    vec2.insert(1, 7);
    vec2.insert(2, 8);
    vec2.insert(3, 10);
    vec2.insert(4, 3);
    vec2.insert(5, 1);
    vec2.insert(6, 2);


    ArrayList<int> vec3 = vec;

    for (int i = 0; i < 3; i++) {
        std::cout << vec[i] << " " << vec1[i] << " " << vec2[i] << " " << vec3[i] << std::endl;
    }

    vec2.remove(1);
    std::cout << std::endl;

    for (int i = 0; i < vec2.size_; i++) {
        std::cout << vec2[i] << std::endl;
    }

    vec2.insert(1, 90);

    std::cout << std::endl;



    return 0;
}
