/* Copyright (C) 2024, Valkai-Németh Béla-Örs */

#include "interval_map.hpp"
#include "TestTypes.hpp"
#include <iostream>
#include <cassert>
#include <chrono>
#include <vector>

// Unintrusive unit testable implementation with our test types
class interval_map_ut : public interval_map<TestKey, TestValue>
{
public:
    interval_map_ut(TestValue const& val)
        : interval_map<TestKey, TestValue>(val)
    {}

    void AssertValidity()
    {
        assert(m_map.empty() || !(m_map.begin()->second == m_valBegin));

        assert(m_map.empty() || m_map.rbegin()->second == m_valBegin);

        // Check it's canonic
        for (auto it = m_map.begin(); it != m_map.end(); it++)
        {
            assert(std::next(it) == m_map.end() || !(it->second == std::next(it)->second));
        }
    }

    void clear()
    {
        m_map.clear();
    }
};

// Basic high resolution timer
class HR_Timer
{
public:
    void start()
    {
        m_time = std::chrono::microseconds(0);
        m_start = std::chrono::high_resolution_clock::now();
    }

    void stop()
    {
        m_time += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_start);
    }

    long long ms() const
    {
        return m_time.count();
    }

private:
    std::chrono::high_resolution_clock::time_point m_start;
    std::chrono::microseconds m_time{ 0 };
};

int main()
{
    std::cout << "Empty map" << std::endl;
    {
        interval_map_ut im{ 'A' };
        im.AssertValidity();
        assert(im[-99] == 'A');
        assert(im[0] == 'A');
        assert(im[99] == 'A');
    };

    std::cout << "Example map" << std::endl;
    {
        interval_map_ut im{ 'A' };
        im.assign(1, 3, 'B');
        im.AssertValidity();
        assert(im[-2] == 'A');
        assert(im[-1] == 'A');
        assert(im[0] == 'A');
        assert(im[1] == 'B');
        assert(im[2] == 'B');
        assert(im[3] == 'A');
        assert(im[4] == 'A');
        assert(im[5] == 'A');
    }

    std::cout << "Same as default value into empty map" << std::endl;
    {
        // Should do nothing
        interval_map_ut im{ 'A' };
        im.assign(4, 20, 'A');
        im.AssertValidity();
    }

    std::cout << "Assign into empty map" << std::endl;
    {
        interval_map_ut im{ 'A' };
        im.assign(4, 20, 'B');
        im.AssertValidity();
        assert(im[3] == 'A');
        assert(im[4] == 'B');
        assert(im[19] == 'B');
        assert(im[20] == 'A');
    }

    std::cout << "Assign after" << std::endl;
    {
        interval_map_ut im{ 'A' };
        im.assign(4, 20, 'B');
        im.AssertValidity();
        im.assign(30, 40, 'C');
        im.AssertValidity();
        assert(im[3] == 'A');
        assert(im[4] == 'B');
        assert(im[19] == 'B');
        assert(im[20] == 'A');
        assert(im[30] == 'C');
        assert(im[39] == 'C');
        assert(im[40] == 'A');
    }

    std::cout << "Assign after spilling into existing" << std::endl;
    {
        interval_map_ut im{ 'A' };
        im.assign(4, 20, 'B');
        im.AssertValidity();
        assert(im[19] == 'B');
        assert(im[20] == 'A');
        im.assign(15, 40, 'C');
        im.AssertValidity();
        assert(im[3] == 'A');
        assert(im[4] == 'B');
        assert(im[14] == 'B');
        assert(im[15] == 'C');
        assert(im[19] == 'C');
        assert(im[20] == 'C');
        assert(im[39] == 'C');
        assert(im[40] == 'A');
    }

    std::cout << "Assign before" << std::endl;
    {
        interval_map_ut im{ 'A' };
        im.assign(4, 20, 'B');
        im.AssertValidity();
        im.assign(0, 2, 'C');
        im.AssertValidity();
        assert(im[-1] == 'A');
        assert(im[1] == 'C');
        assert(im[2] == 'A');
        assert(im[3] == 'A');
        assert(im[4] == 'B');
        assert(im[19] == 'B');
        assert(im[20] == 'A');
    }

    std::cout << "Assign before right at edge" << std::endl;
    {
        interval_map_ut im{ 'A' };
        im.assign(4, 20, 'B');
        im.AssertValidity();
        im.assign(0, 4, 'C');
        im.AssertValidity();
        assert(im[-1] == 'A');
        assert(im[2] == 'C');
        assert(im[3] == 'C');
        assert(im[4] == 'B');
        assert(im[19] == 'B');
        assert(im[20] == 'A');
    }

    std::cout << "Assign before spilling into existing" << std::endl;
    {
        interval_map_ut im{ 'A' };
        im.assign(4, 20, 'B');
        im.AssertValidity();
        assert(im[3] == 'A');
        assert(im[4] == 'B');
        im.assign(0, 5, 'C');
        im.AssertValidity();
        assert(im[-1] == 'A');
        assert(im[3] == 'C');
        assert(im[4] == 'C');
        assert(im[5] == 'B');
        assert(im[19] == 'B');
        assert(im[20] == 'A');
    }

    std::cout << "Assign into existing" << std::endl;
    {
        interval_map_ut im{ 'A' };
        im.assign(-1, 15, 'B');
        im.AssertValidity();
        im.assign(6, 10, 'C');
        im.AssertValidity();
        assert(im[5] == 'B');
        assert(im[6] == 'C');
        assert(im[9] == 'C');
        assert(im[10] == 'B');
        assert(im[14] == 'B');
        assert(im[15] == 'A');
    }

    std::cout << "Assign around smaller" << std::endl;
    {
        interval_map_ut im{ 'A' };
        im.assign(6, 10, 'B');
        im.AssertValidity();
        assert(im[8] == 'B');
        im.assign(-1, 15, 'C');
        im.AssertValidity();
        assert(im[-2] == 'A');
        assert(im[-1] == 'C');
        assert(im[8] == 'C');
        assert(im[14] == 'C');
        assert(im[15] == 'A');
    }

    std::cout << "Assign inside larger with same value" << std::endl;
    {
        interval_map_ut im{ 'A' };
        im.assign(-10, 20, 'B');
        im.AssertValidity();
        im.assign(-5, 7, 'B');
        im.AssertValidity();
        assert(im[-11] == 'A');
        assert(im[-10] == 'B');
        assert(im[19] == 'B');
        assert(im[20] == 'A');
    }

    std::cout << "Assign on top of other" << std::endl;
    {
        interval_map_ut im{ 'A' };
        im.assign(-10, 20, 'B');
        im.AssertValidity();
        im.assign(-5, 10, 'C');
        im.AssertValidity();
        im.assign(-5, 10, 'D');
        im.AssertValidity();
        assert(im[-5] == 'D');
        assert(im[9] == 'D');
        assert(im[10] == 'B');
        assert(im[20] == 'A');
    }

    std::cout << "Set inner to same as outer" << std::endl;
    {
        interval_map_ut im{ 'A' };
        im.assign(-10, 20, 'B');
        im.AssertValidity();
        im.assign(-5, 10, 'C');
        im.AssertValidity();
        im.assign(-5, 10, 'B');
        im.AssertValidity();
        assert(im[-11] == 'A');
        assert(im[-5] == 'B');
        assert(im[9] == 'B');
        assert(im[10] == 'B');
        assert(im[20] == 'A');
    }

    std::cout << "Assign initial value at start of non initial" << std::endl;
    {
        interval_map_ut im{ 'A' };
        im.assign(5, 10, 'B');
        im.AssertValidity();
        im.assign(2, 7, 'A');
        im.AssertValidity();
        assert(im[5] == 'A');
        assert(im[7] == 'B');
        assert(im[9] == 'B');
        assert(im[10] == 'A');
    }

    std::cout << "Assign initial value at end of non initial" << std::endl;
    {
        interval_map_ut im{ 'A' };
        im.assign(-5, 10, 'B');
        im.AssertValidity();
        im.assign(2, 15, 'A');
        im.AssertValidity();
        assert(im[-6] == 'A');
        assert(im[-5] == 'B');
        assert(im[2] == 'A');
    }

    std::cout << "Assign outer with same as initial" << std::endl;
    {
        interval_map_ut im{ 'A' };
        im.assign(5, 10, 'B');
        im.AssertValidity();
        im.assign(2, 15, 'A');
        im.AssertValidity();
        assert(im[-6] == 'A');
        assert(im[-5] == 'A');
        assert(im[2] == 'A');
    }

    std::cout << "Redundant assign" << std::endl;
    {
        interval_map_ut im{ 'A' };
        im.assign(2, 15, 'B');
        im.AssertValidity();
        im.assign(3, 13, 'B');
        im.AssertValidity();
        assert(im[1] == 'A');
        assert(im[2] == 'B');
        assert(im[14] == 'B');
        assert(im[15] == 'A');
    }

    /*/////////////////////////////////////////////////////////////////////////////////
    std::cout << "Speed test" << std::endl;
    {
        interval_map_ut im{ 'A' };
        HR_Timer timer;
        long long avgTime=0;

        // Set srand() produce same inputs every time, since we want to time different
        // implementations against eachother, not variable inputs
        srand(0);
        std::vector<std::tuple<int, int, char>> inputs;

        const int RAND_N = 1000;
        for (int i = 0; i < RAND_N; i++)
        {
            int keyBegin = rand() % RAND_N;
            int keyEnd = keyBegin + (rand() % RAND_N);
            char c = 'A' + rand() % 26;
            inputs.emplace_back(keyBegin, keyEnd, c);
        }

        const int MAX_ITERATIONS = 1000;
        for (int i = 0; i < MAX_ITERATIONS; i++)
        {
            im.clear();

            timer.start();
            for (auto tuple : inputs)
            {
                im.assign(std::get<0>(tuple), std::get<1>(tuple), std::get<2>(tuple));
            }
            timer.stop();

            if (0 == i)
                im.AssertValidity();

            avgTime += timer.ms();
        }
        avgTime /= MAX_ITERATIONS;

        std::cout<<"Finished in "<<avgTime<<"ms"<< std::endl;
    }//*/
}