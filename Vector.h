#pragma once

template<typename T>
class Vector{
public:
    Vector()
    {
        //allocate a initialize memory
        ReAlloc(2);
    }
    Vector(size_t n)
    {
        ReAlloc(n);
    }

    size_t Size() const{ return m_Size; }

    size_t Capacity() const{ return m_Capacity; }

    T& operator[](size_t index){
        if(index > m_Capacity - 1)
            throw std::exception();
        return m_Data[index];
    }
    const T& operator[](size_t index) const { return m_Data[index]; }


    void PushBack(const T& value)
    {
        if(m_Size >= m_Capacity)
            ReAlloc(m_Capacity + m_Capacity / 2);

        m_Data[m_Size++] = value;
    }
    void PushBack(T&& value)
    {
        if(m_Size >= m_Capacity)
            ReAlloc(m_Capacity + m_Capacity / 2);

        m_Data[m_Size++] = std::move(value);  // called the move constructor(which is different from copy constructor)
    }

    template<typename... Args>  // A variadic function template can be called with any number of function arguments
    T& EmplaceBack(Args&&... args)
    {
        if(m_Size >= m_Capacity)
            ReAlloc(m_Capacity + m_Capacity / 2);
        // 还是有区别，这里的临时T很快就释放掉了，而main函数中的T只有程序结束后才会释放
        // 就地创建一个临时的T 移动赋值给m_Data[m_Size] 好像和main函数创建然后移动没区别？
        // m_Data[m_Size] = T(std::forward<Args>(args)...);
        // 我的理解是 std::forward能保持目标原有的模样（左值或者右值)
        // 而std::move 能让左值转换为右值 这里的...表示unpack the args 解压
        new(&m_Data[m_Size]) T(std::forward<Args>(args)...);
        // this is an optimization which allocate the T directly in the location of the m_Data[m_Size]

        return m_Data[m_Size++];
    }

    void PopBack()
    {
        if(m_Size > 0)
        {
            m_Size--;
            m_Data[m_Size].~T();   //call the destructor of the "T" object
        }
    }

    void Clear()
    {
        for(size_t i = 0; i < m_Size; i++)
            m_Data[i].~T();

        m_Size = 0;
    }


    ~Vector()
    {
        // delete[] m_Data;  // here must use the "[]"
        Clear(); //call the destructor of every single "T" element and resize to 0
        ::operator delete(m_Data, m_Capacity * sizeof(T));
        // delete the whole capacity of the vector
        // to make sure the memory was totally clean
    }
private:
    void ReAlloc(size_t newCapacity)
    {
        // 1. allocate a new block of memory
        // 2. copy / move old elements into new block
        // 3. delete
        // 在这里实际上不需要调用 T的constructor
        // so we can transform the "new T[newCapacity]" to "(T*)::operator new(newCapacity * sizeof(T))"
        // T* newBlock = new T[newCapacity];
        T* newBlock = (T*)::operator new(newCapacity * sizeof(T));

        /* actually, memcpy can't be used here, because
           to make sure the constructor works, and here
           is a deep copy,we must use the for loop
           memcpy(tmp,m_Data,m_Capacity);
        */

        // to enable to downsize
        if(newCapacity < m_Size)
            m_Size = newCapacity;

        for(size_t i = 0; i < m_Size ; i++)
            newBlock[i] = std::move(m_Data[i]);

        for(size_t i = 0; i < m_Size; i++)
            m_Data[i].~T();

        ::operator delete(m_Data, m_Capacity * sizeof(T));
        m_Data = newBlock;
        m_Capacity = newCapacity;
    }

private:
    T* m_Data = nullptr;
    size_t m_Size = 0;
    size_t m_Capacity = 0;
};