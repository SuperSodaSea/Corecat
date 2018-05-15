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

#include <array>

#include "Cats/Corecat/Util/Benchmark.hpp"


using namespace Cats::Corecat;


int main() {
    
    std::vector<std::array<std::int32_t, 3>> v(16384, {{87654321, 12345678}});
    Benchmark<> benchmark;
    benchmark
        .add("nop", [&](auto _) { while(_) for(auto&& x : v) x[2] = x[0]; })
        .add("add", [&](auto _) { while(_) for(auto&& x : v) x[2] = x[0] + x[1]; })
        .add("sub", [&](auto _) { while(_) for(auto&& x : v) x[2] = x[0] - x[1]; })
        .add("mul", [&](auto _) { while(_) for(auto&& x : v) x[2] = x[0] * x[1]; })
        .add("div", [&](auto _) { while(_) for(auto&& x : v) x[2] = x[0] / x[1]; })
        .argument()
        .run();
        
    return 0;
    
}
