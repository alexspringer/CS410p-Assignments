#ifndef SORT_HPP
#define SORT_HPP
#include<algorithm>
#include<vector>
#include<iterator>

template<typename FWD>
void bubble_sort(FWD begin, FWD end)
{
    for(FWD count = begin; count != end; count++)
    {
        FWD y = begin;
        FWD x = y++;
        while(y != end)
        {
            if(*x > *y)
            {
                std::swap(*x, *y);
            }
            ++x;
            ++y;
        }
    }
}

template<typename FWD>
FWD partition(FWD begin, FWD end)
{
    FWD pivot = begin;
    while(std::next(pivot) != end) ++pivot;

    FWD lo = begin;
    FWD hi = begin;
    while(hi != pivot)
    {
        if(*hi < *pivot)
        {
            std::swap(*lo,*hi);
            ++lo;
        }
        ++hi;
    }
    std::swap(*lo,*pivot);
    return lo;
}

template<typename FWD>
void quick_sort(FWD begin, FWD end)
{
    if(begin == end || std::next(begin) == end) return;

    FWD mid = partition(begin,end);

    quick_sort(begin,mid);
    quick_sort(std::next(mid),end);
}




template<typename FWD>
void best_sort(FWD begin, FWD end)
{
    auto e = *begin;
    std::vector<decltype(e)> v(begin, end);
    //std::vector<decltype(*begin)> v(begin, end);
    std::sort(v.begin(), v.end());
    std::copy(v.begin(), v.end(), begin);
}

#endif //SORT_HPP
