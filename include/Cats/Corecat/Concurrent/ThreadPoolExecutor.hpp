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

#ifndef CATS_CORECAT_CONCURRENT_THREADPOOLEXECUTOR_HPP
#define CATS_CORECAT_CONCURRENT_THREADPOOLEXECUTOR_HPP


#include <cstddef>

#include <atomic>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>


namespace Cats {
namespace Corecat {
inline namespace Concurrent {

class ThreadPoolExecutor {
    
private:
    
    std::size_t maxThread;
    
    std::mutex mutex;
    std::deque<std::function<void()>> workQueue;
    std::atomic<std::size_t> threadCount = {0};
    std::atomic<bool> shutdown = {false};
    
private:
    
    void createWorker() {
        
        std::thread([&] {
            
            while(true) {
                
                std::unique_lock<std::mutex> lock(mutex);
                if(workQueue.empty()) {
                    
                    lock.unlock();
                    if(shutdown) { --threadCount; break; }
                    else { std::this_thread::yield(); continue; }
                    
                }
                auto work = workQueue.front();
                workQueue.pop_front();
                lock.unlock();
                try { work(); } catch(...) {}
                
            }
            
        }).detach();
        ++threadCount;
        
    }
    
public:
    
    ThreadPoolExecutor(std::size_t maxThread_ = std::thread::hardware_concurrency()) : maxThread(maxThread_) {}
    ThreadPoolExecutor(const ThreadPoolExecutor& src) = delete;
    ~ThreadPoolExecutor() {
        
        shutdown = true;
        while(threadCount) std::this_thread::yield();
        
    }
    
    ThreadPoolExecutor& operator =(const ThreadPoolExecutor& src) = delete;
    
    template <typename F>
    void execute(F&& f) {
        
        std::lock_guard<std::mutex> lock(mutex);
        if(threadCount < maxThread) createWorker();
        workQueue.emplace_back(std::forward<F>(f));
        
    }
    
};

}
}
}


#endif
