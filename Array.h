#pragma once

template<typename T, size_t S>
class Array{
public:
    constexpr size_t Size() const { return S; }  // constexpr means a constant variable

    T& operator[](size_t index) { return m_Data[index]; }
    //why return a reference? to avoid the allocation when T is object like String.
    //Also to make sure the return value can be modified.
    const T& operator[](size_t index) const { return m_Data[index]; }

    T* Data() { return m_Data; }
    const T* Data() const { return m_Data; }

private:
    T m_Data[S];
};
