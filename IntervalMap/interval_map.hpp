/* Copyright (C) 2024, Valkai-Németh Béla-Örs */

#ifndef INTERVAL_MAP_HPP
#define INTERVAL_MAP_HPP

#include <map>

template<typename K, typename V>
class interval_map
{
protected:
    V m_valBegin;
    std::map<K, V> m_map;

    ~interval_map() = default;

public:
    // constructor associates whole range of K with val
    interval_map(V const& val)
        : m_valBegin(val)
    {}

    // Assign value val to interval [keyBegin, keyEnd).
    // Overwrite previous values in this interval.
    void assign(K const& keyBegin, K const& keyEnd, V const& val)
    {
        if (!(keyBegin < keyEnd))
        {
            return;
        }

        // Picked lower_bound since it also helps us correctly hint at where to insert
        auto it = m_map.lower_bound(keyBegin);
        if (m_map.end() == it)
        {
            if (val == m_valBegin)
            {
                return;
            }

            // Insert into empty map or at end of map
            m_map.emplace_hint(it, keyBegin, val);
            m_map.emplace_hint(m_map.end(), keyEnd, m_valBegin);
        } else {
            // Since we don't have == implemented we do the 2 compares instead for foundEntryAtKeyBegin
            const bool foundEntryAtKeyBegin = !(keyBegin < it->first) && !(it->first < keyBegin);
            const V valueBeforeKeyBegin = m_map.begin() == it ? m_valBegin : std::prev(it)->second;
            V valueForKeyEnd = foundEntryAtKeyBegin ? it->second : valueBeforeKeyBegin;

            if (!(val == valueBeforeKeyBegin))
            {
                if (!foundEntryAtKeyBegin)
                {
                    if (keyEnd < it->first)
                    {
                        // Non conflicting insert
                        it = m_map.emplace_hint(it, keyBegin, val);
                        m_map.emplace_hint(it, keyEnd, valueBeforeKeyBegin);
                        return;
                    } else if (!(it->first < keyEnd)) { //&& !(keyEnd < it->first)) {
                        // Conflicts at end, but with right value
                        m_map.emplace_hint(it, keyBegin, val);
                        return;
                    }

                    it = m_map.emplace_hint(it, keyBegin, val);
                } else {
                    it->second = val;
                }//*/

                //it = m_map.insert_or_assign(it, keyBegin, val);
            } else if (foundEntryAtKeyBegin) {
                // We need to erase here since it wouldn't remain canonic if we don't
                it = m_map.erase(it);
            }

            if (m_map.end() != it && !(keyBegin < it->first))
            {
                // Move past keyBegin if we aren't already
                it++;
            }

            // Erase entries <= keyEnd since we want to overwrite them
            while (m_map.end() != it && !(keyEnd < it->first))
            {
                const auto nextIt = std::next(it);
                if (m_map.end() == nextIt || keyEnd < nextIt->first)
                {
                    valueForKeyEnd = it->second;
                }

                it = m_map.erase(it);
            }

            if (!(val == valueForKeyEnd))
            {
                m_map.emplace_hint(it, keyEnd, valueForKeyEnd);
            }
        }
    }

    // look-up of the value associated with key
    V const& operator[](K const& key) const
    {
        auto it = m_map.upper_bound(key);
        if (it == m_map.begin())
        {
            return m_valBegin;
        }
        else
        {
            return (--it)->second;
        }
    }
};

#endif // INTERVAL_MAP_HPP

