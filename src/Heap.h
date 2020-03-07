#pragma once

#include <vector>
#include <optional>
#include <functional>

template<typename T>
class Heap
{
public:
    Heap()
    : mSize(0), mTree(), m_comp(nullptr)
    {

    }

    void set_comp(const std::function<bool(T,T)>& comp)
    {
      m_comp = comp;
    }

    std::optional<const T&> top() const
    {
      return mSize ? make_optional(mTree[0]) : std::nullopt;
    }

    void push(const T& data)
    {
        int i = mSize++; mTree.push_back(data);
        while (i != 0 && m_comp(mTree[(i - 1) / 2], data)) {
            mTree[i] = mTree[(i - 1) / 2];
            i = (i - 1) / 2;
        }
        mTree[i] = data;

        return;
    }

    void pop()
    {
        if (mSize == 0) return;

        T back_elem = mTree[--mSize];

        int i = 0; int ci = i * 2 + 1;
        while (ci < mSize - 1) {
            if (m_comp(mTree[ci], mTree[ci + 1]))   ci++;

            if (m_comp(back_elem, mTree[ci])) {
                mTree[i] = mTree[ci];
                i = ci;
                ci = ci * 2 + 1;
            }
            else
                break;
        }

        mTree[i] = back_elem;
        mTree.pop_back();

        return;
    }

    void replace(const T& data)
    {
        if (mSize == 0) return;

        int i = 0; int ci = i * 2 + 1;
        while (ci < mSize - 1) {
            if (m_comp(mTree[ci], mTree[ci + 1]))  ci++;

            if (m_comp(data, mTree[ci])) {
                mTree[i] = mTree[ci];
                i = ci;
                ci = ci * 2 + 1;
            }
            else
                break;
        }

        mTree[i] = data;

        return;
    }

    size_t size() const
    {
      return mSize;
    }

    bool empty() const
    {
      return mSize == 0;
    }

private:
    size_t mSize;
    vector<T> mTree;
    std::function<bool(T,T)> m_comp;
};