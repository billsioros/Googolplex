/* C++ Heap implementation by Vasileios Sioros */

#ifndef __HEAP__
#define __HEAP__

template <typename T>
class heap
{
    bool (*cmp)(const T&, const T&);

    unsigned size, max;
    T * items;

public:
    
    static bool less(const T&, const T&);
    static bool greater(const T&, const T&);

    heap(const unsigned, bool (*cmp)(const T&, const T&));
    ~heap();

    bool push(const T&);
    bool pop(T&);
};

template <typename T>
bool heap<T>::less(const T& a, const T& b)
{
    return a < b;
}

template <typename T>
bool heap<T>::greater(const T& a, const T& b)
{
    return a > b;
}

// Heap Implementation:
template <typename T>
heap<T>::heap(const unsigned max, bool (*cmp)(const T&, const T&))
:
cmp(cmp), size(0), max(max + 1), items(new T[max + 1])
{
}

template <typename T>
heap<T>::~heap()
{
    delete[] items;
}

template <typename T>
bool heap<T>::push(const T& item)
{
    if(size == max)
        return false;

    unsigned child = ++size, parent = child / 2;
    items[child] = item;

    while (parent)
    {
        if (!cmp(items[child], items[parent]))
            break;

        T tmp = items[child];
        items[child] = items[parent];
        items[parent] = tmp;

        child = parent;
        parent = child / 2;
    }

    return true;
}

template <typename T>
bool heap<T>::pop(T& item)
{
    if(!size)
        return false;

    item = items[1];
    items[1] = items[size--];

    unsigned current = 1, child = 2 * current;
    while (child <= size)
    {
        unsigned next;
        if (child + 1 <= size && cmp(items[child + 1], items[child]))
            next = child + 1;
        else
            next = child;

        if (cmp(items[current], items[next]))
            break;

        T tmp = items[current];
        items[current] = items[next];
        items[next] = tmp;

        current = next;
        child = 2 * next;
    }

    return true;
}

#endif
