#pragma once

#include <vector>

using namespace std;

template <class T>
class EditorResource
{
public:
    vector<T> references;

    void AddRef(T ref_target) { references.push_back(ref_target); }
    void RemoveRef(T remove_target) { remove(references.begin(), references.end(), remove_target); }
};