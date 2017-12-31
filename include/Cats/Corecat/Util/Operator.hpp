/*
 *
 * MIT License
 *
 * Copyright (c) 2016-2017 The Cats Project
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

#ifndef CATS_CORECAT_UTIL_OPERATOR_HPP
#define CATS_CORECAT_UTIL_OPERATOR_HPP


namespace Cats {
namespace Corecat {
inline namespace Util {

template <typename T>
struct EqualityOperator {
    
    friend bool operator !=(const T& a, const T& b) noexcept(noexcept(a == b)) { return !(a == b); }
    
};
template <typename T>
struct RelationalOperator {
    
    friend bool operator >(const T& a, const T& b) noexcept(noexcept(b < a)) { return b < a; }
    friend bool operator <=(const T& a, const T& b) noexcept(noexcept(b < a)) { return !(b < a); }
    friend bool operator >=(const T& a, const T& b) noexcept(noexcept(a < b)) { return !(a < b); }
    
};

}
}
}


#endif
