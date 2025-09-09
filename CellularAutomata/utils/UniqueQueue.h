#pragma once
#include <queue>
#include <set>

template <typename T>
class UniqueQueue
{
public:
    void push(const T& element);
    void pop();
    void clear();
    const T& front() const;
    bool empty() const;
    bool contains(const T& element) const;
private:
    std::queue<T> queue;
    std::set<T> set;
};

template <typename T>
void UniqueQueue<T>::push(const T& element)
{
    if (set.find(element) == set.end())
    {
        set.insert(element);
        queue.push(element);
    }
}

template <typename T>
void UniqueQueue<T>::pop()
{
    if (queue.empty())
    {
        return;
    }

    const T& elem = front();
    auto it = set.find(elem);
    if (it == set.end())
    {
        return;
    }

    set.erase(it);
    queue.pop();
}

template <typename T>
void UniqueQueue<T>::clear()
{
    queue = std::queue<T>();
    set.clear();
}

template <typename T>
const T& UniqueQueue<T>::front() const
{
    return queue.front();
}

template <typename T>
bool UniqueQueue<T>::empty() const
{
    return queue.empty();
}

template <typename T>
bool UniqueQueue<T>::contains(const T& element) const
{
    return set.find(element) != set.end();
}
