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

#ifndef CATS_CORECAT_ANY_HPP
#define CATS_CORECAT_ANY_HPP


#include <cstdlib>

#include <type_traits>
#include <typeinfo>
#include <utility>


namespace Cats {
namespace Corecat {

class Any {
    
public:
    
    static constexpr std::size_t BUFFER_SIZE = 64;
    
private:
    
    class HolderBase {
        
    public:
        
        virtual ~HolderBase() {}
        
        virtual void* get() noexcept = 0;
        const void* get() const noexcept { return const_cast<HolderBase*>(this)->get(); }
        virtual std::size_t getSize() const noexcept = 0;
        virtual const std::type_info& getType() const noexcept = 0;
        virtual void copy(void* dst) const = 0;
        virtual void move(void* dst) = 0;
        
    };
    
    template <typename T>
    class Holder : public HolderBase {
        
    private:
        
        T t;
        
    public:
        
        Holder(const Holder& src) : t(src.t) {}
        Holder(Holder&& src) : t(std::move(src.t)) {}
        ~Holder() final = default;
        
        Holder() = delete;
        Holder(T&& t_) : t(std::move(t_)) {}
        
        void* get() noexcept final { return &t; }
        std::size_t getSize() const noexcept final { return sizeof(Holder); }
        const std::type_info& getType() const noexcept final { return typeid(T); }
        void copy(void* dst) const final { new(static_cast<Holder*>(dst)) Holder<T>(*this); }
        void move(void* dst) final { new(static_cast<Holder*>(dst)) Holder(std::move(t)); };
        
    };
    
private:
    
    char* data;
    std::size_t capacity;
    bool empty;
    char buffer[BUFFER_SIZE];
    
private:
    
    void allocate(std::size_t cap) {
        
        if(capacity < cap) {
            
            if(data != buffer) delete[] data;
            data = new char[cap];
            capacity = cap;
            
        }
        
    }
    
public:
    
    Any(const Any& src) : Any() {
        
        if(!src.empty) {
            
            auto holder = reinterpret_cast<const HolderBase*>(src.data);
            allocate(holder->getSize());
            holder->copy(data);
            empty = false;
            
        }
        
    }
    Any(Any&& src) : Any() { swap(src); }
    ~Any() {}
    
    Any() noexcept : data(buffer), capacity(BUFFER_SIZE), empty(true) {}
    template <typename T, typename std::enable_if<!std::is_same<typename std::decay<T>::type, Any>::value, int>::type = 0>
    Any(T&& t) : Any() { emplace<T>(std::move(t)); }
    
    Any& operator =(const Any& src) {
        
        clear();
        if(!src.empty) {
            
            auto holder = reinterpret_cast<const HolderBase*>(src.data);
            allocate(holder->getSize());
            holder->copy(data);
            empty = false;
            
        }
        return *this;
        
    }
    Any& operator =(Any&& src) { swap(src); return *this; }
    
    template <typename T, typename std::enable_if<!std::is_same<typename std::decay<T>::type, Any>::value, int>::type = 0>
    Any& operator =(T&& t) { emplace<T>(std::move(t)); return *this; }
    
    template <typename T>
    T& get() {
        
        if(getType() != typeid(T)) throw std::bad_cast();
        return *static_cast<T*>(reinterpret_cast<HolderBase*>(data)->get());
        
    }
    template <typename T>
    const T& get() const {
        
        if(getType() != typeid(T)) throw std::bad_cast();
        return static_cast<const T&>(*static_cast<const HolderBase*>(data)->get());
        
    }
    const std::type_info& getType() const { return empty ? typeid(void) : reinterpret_cast<HolderBase*>(data)->getType(); }
    
    bool isEmpty() const noexcept { return empty; }
    
    void clear() { if(!empty) { reinterpret_cast<HolderBase*>(data)->~HolderBase(); empty = false; } }
    
    template <typename T, typename... Arg>
    void emplace(Arg&&... arg) {
        
        clear();
        allocate(sizeof(Holder<T>));
        new(data) Holder<T>(std::move(T(std::forward<Arg>(arg)...)));
        empty = false;
        
    }
    
    void swap(Any& src) noexcept {
        
        if(empty) {
            
            if(!src.empty) {
                
                if(data == buffer)
                    if(src.data == src.buffer) reinterpret_cast<HolderBase*>(src.buffer)->move(buffer);
                    else {
                        
                        data = src.data; capacity = src.capacity;
                        src.data = src.buffer; src.capacity = BUFFER_SIZE;
                        
                    }
                else
                    if(src.data == src.buffer) {
                        
                        reinterpret_cast<HolderBase*>(src.buffer)->move(buffer);
                        src.data = data; src.capacity = capacity;
                        data = buffer; capacity = BUFFER_SIZE;
                        
                    } else { std::swap(data, src.data); std::swap(capacity, src.capacity); }
                empty = false; src.empty = true;
                
            }
            
        } else {
            
            if(src.empty) {
                
                if(data == buffer)
                    if(src.data == src.buffer) {
                        
                        char tmp[BUFFER_SIZE];
                        reinterpret_cast<HolderBase*>(buffer)->move(tmp);
                        reinterpret_cast<HolderBase*>(src.buffer)->move(buffer);
                        reinterpret_cast<HolderBase*>(tmp)->move(buffer);
                        
                    } else {
                        
                        reinterpret_cast<HolderBase*>(buffer)->move(src.buffer);
                        data = src.data; capacity = src.capacity;
                        src.data = src.buffer; src.capacity = BUFFER_SIZE;
                        
                    }
                else
                    if(src.data == src.buffer) {
                        
                        reinterpret_cast<HolderBase*>(src.buffer)->move(buffer);
                        src.data = data; src.capacity = capacity;
                        data = buffer; capacity = BUFFER_SIZE;
                        
                    } else { std::swap(data, src.data); std::swap(capacity, src.capacity); }
                
            } else {
                
                if(data == buffer)
                    if(src.data == src.buffer) reinterpret_cast<HolderBase*>(buffer)->move(src.buffer);
                    else {
                        
                        reinterpret_cast<HolderBase*>(buffer)->move(src.buffer);
                        data = src.data; capacity = src.capacity;
                        src.data = src.buffer; src.capacity = BUFFER_SIZE;
                        
                    }
                else
                    if(src.data == src.buffer) {
                        
                        src.data = data; src.capacity = capacity;
                        data = buffer; capacity = BUFFER_SIZE;
                        
                    } else { std::swap(data, src.data); std::swap(capacity, src.capacity); }
                empty = true; src.empty = false;
                
            }
            
        }
        
    }
    
};

}
}


#endif
