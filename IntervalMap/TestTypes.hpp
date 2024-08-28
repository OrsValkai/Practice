/* Copyright (C) 2024, Valkai-Németh Béla-Örs */

#ifndef INTERVAL_MAP_TEST_TYPES_HPP
#define INTERVAL_MAP_TEST_TYPES_HPP

// Some very constrained test types to use with the interval_map.
#include <ostream>

class TestKey
{
public:
    TestKey(int value) : m_value(value) {}
    TestKey(const TestKey& cp) : m_value(cp.m_value) {}

    TestKey& operator=(const TestKey& rhs)
    {
        m_value = rhs.m_value;
        return *this;
    }

    bool operator<(const TestKey& rhs) const
    {
        return m_value < rhs.m_value;
    }

    int m_value;
};

class TestValue
{
public:
    TestValue(char value) : m_value(value) {}
    TestValue(const TestValue& cp) : m_value(cp.m_value) {}

    TestValue& operator=(const TestValue& rhs)
    {
        m_value = rhs.m_value;
        return *this;
    }

    bool operator==(const TestValue& rhs) const
    {
        return m_value == rhs.m_value;
    }

    char m_value;
};

#endif // INTERVAL_MAP_TEST_TYPES_HPP