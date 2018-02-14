/*
 *
 * MIT License
 *
 * Copyright (c) 2016-2018 The Cats Project
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#ifndef CATS_CORECAT_UTIL_BENCHMARK_HPP
#define CATS_CORECAT_UTIL_BENCHMARK_HPP


#include <iostream>
#include <vector>

#include "Function.hpp"
#include "../Text/String.hpp"
#include "../Time/HighResolutionClock.hpp"


namespace Cats {
namespace Corecat {
inline namespace Util {

template <typename... Arg>
class Benchmark {
    
public:
    
    class BenchmarkRecoder {
        
    private:
        
        HighResolutionClock::time_point startTime;
        std::uint64_t* count;
        
    public:
        
        BenchmarkRecoder(HighResolutionClock::time_point startTime_, std::uint64_t& count_) : startTime(startTime_), count(&count_) {}
        
        operator bool() noexcept {
            
            if(HighResolutionClock::now() - startTime > std::chrono::duration<double>(1)) return false;
            ++*count;
            return true;
            
        }
        
    };
    
private:
    
    std::vector<std::pair<String8, std::function<void(BenchmarkRecoder, Arg...)>>> funcList;
    std::vector<std::tuple<Arg...>> argumentList;
    
public:
    
    Benchmark() = default;
    Benchmark(const Benchmark& src) = delete;
    Benchmark(Benchmark&& src) = default;
    
    Benchmark& operator =(const Benchmark& src) = delete;
    Benchmark& operator =(Benchmark&& src) = default;
    
    Benchmark& add(String8 name, std::function<void(BenchmarkRecoder, Arg...)> func) {
        
        funcList.emplace_back(std::move(name), std::move(func));
        return *this;
        
    }
    Benchmark& argument(Arg... arg) {
        
        argumentList.emplace_back(std::move(arg)...);
        return *this;
        
    }
    Benchmark& run() {
        
        for(auto&& x : argumentList) {
            
            for(auto&& y : funcList) {
                
                { 
                    auto startTime = Time::HighResolutionClock::now();
                    std::uint64_t count = 0;
                    BenchmarkRecoder recoder(startTime, count);
                    apply(y.second, std::tuple_cat(std::make_tuple(recoder), x));
                }
                {
                    auto startTime = Time::HighResolutionClock::now();
                    std::uint64_t count = 0;
                    BenchmarkRecoder recoder(startTime, count);
                    apply(y.second, std::tuple_cat(std::make_tuple(recoder), x));
                    auto endTime = Time::HighResolutionClock::now();
                    std::cout << y.first << ": " << (1000000.0 * (endTime - startTime).count() / count) << std::endl;
                } 
                
            }
            
        }
        return *this;
        
    }
    
};

}
}
}


#endif
