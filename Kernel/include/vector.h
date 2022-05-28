#pragma once
#include <serial.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
template <typename T>
class Vector {

public:
    T operator[](int index) {
        return Get(index);
    }
    int Length = 0;
    void Add(T item) {
        array[Length] = item;
        Length++;
    }
    void Remove(int index) {
        array[index] = {};
        Length--;
    }
    int GetPos(T item) {
        int idx = -1;
        for (size_t i = 0; i < Length; i++)
        {
            if(Get(i) == item) {
                idx = i;
            }
        }
        return idx;
        
    }

    T Get(int index) {
        return array[index];
    }

private:
    T *array;
};

template <typename V, typename N>
class Dictionary {
public:
    int Length;
    V operator[](N name) {
        return array[names.GetPos(name)];
        
    }
    void Add(N name, V item) {
        names.Add(name);
        array.Add(item);
        Length++;
    }

private:
    Vector<V> array;
    Vector<N> names;
};