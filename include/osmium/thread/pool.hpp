#ifndef OSMIUM_THREAD_POOL_HPP
#define OSMIUM_THREAD_POOL_HPP

/*

This file is part of Osmium (http://osmcode.org/osmium).

Copyright 2013 Jochen Topf <jochen@topf.org> and others (see README).

Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

#include <atomic>
#include <functional>
#include <thread>
#include <utility>
#include <vector>

#include <osmium/thread/queue.hpp>

namespace osmium {

    /**
     * @brief Namespace for threading-related code.
     */
    namespace thread {

        /**
         *  Thread pool.
         */
        class Pool {

            // This class makes sure pool threads are joined when the pool is destructed
            class thread_joiner {

                std::vector<std::thread>& m_threads;

            public:

                explicit thread_joiner(std::vector<std::thread>& threads) :
                    m_threads(threads) {
                }

                ~thread_joiner() {
                    for (auto& thread : m_threads) {
                        if (thread.joinable()) {
                            thread.join();
                        }
                    }
                }

            };

            std::atomic<bool> m_done;
            osmium::thread::Queue<std::function<void()>> m_work_queue;
            std::vector<std::thread> m_threads;
            thread_joiner m_joiner;

            void worker_thread() {
                while (!m_done) {
                    std::function<void()> task;
                    m_work_queue.wait_and_pop_with_timeout(task);
                    if (task) {
                        task();
                    }
                }
            }

        public:

            Pool(unsigned const int thread_count):
                m_done(false),
                m_threads(),
                m_joiner(m_threads) {
                try {
                    for (unsigned i = 0; i < thread_count; ++i) {
                        m_threads.push_back(std::thread(&Pool::worker_thread, this));
                    }
                } catch (...) {
                    m_done = true;
                    throw;
                }
            }

            ~Pool() {
                m_done=true;
            }

            size_t queue_size() const {
                return m_work_queue.size();
            }

            bool work_queue_empty() const {
                return m_work_queue.empty();
            }

            size_t submit(std::function<void()>&& func) {
                return m_work_queue.push_and_get_size(std::forward<std::function<void()>>(func));
            }

        };  // Pool

    } // namespace thread

} // namespace osmium

#endif // OSMIUM_THREAD_POOL_HPP
