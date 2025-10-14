/**
 * @file workqueue.hpp
 *
 * Definition of 'workqueue' design pattern.
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 */

#ifndef _WORKQUEUE_HPP_
#define _WORKQUEUE_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <string>
#include <sstream>

#if defined(DEBUG_WORKQUEUE)
#include <iostream>
#endif

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "../semaphores/counting/semaphore.hpp"
#include "work.hpp"

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
namespace lts
{

class workqueue
{
public:
    explicit workqueue(const std::string& idstr, std::size_t threads = 1) :
        m_idstring{idstr},
        m_threads{threads},
        m_queue{},
        m_worker_threads{nullptr},
        m_is_valid{false}
    {
        std::size_t created_workers = 0;

#if defined(DEBUG_WORKQUEUE)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        m_worker_threads = new (std::nothrow) worker_thread_ptr[m_threads];
        if (m_threads > 0) {
            for (std::size_t i = 0; i < m_threads; ++i) {
                std::ostringstream idstr;
                idstr << "worker #" << i;
                m_worker_threads[i] = new (std::nothrow) worker_thread(&m_queue, idstr.str());
                if (m_worker_threads[i])
                    created_workers++;
            }
        }

        m_is_valid = (created_workers == m_threads);
    }

    ~workqueue()
    {
#if defined(DEBUG_WORKQUEUE)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        /* cancel all worker threads */
        for (std::size_t i = 0; i < m_threads; ++i)
            m_worker_threads[i]->cancel();

#if defined(DEBUG_WORKQUEUE)
        std::cout << "all worker threads cancelled" << std::endl;
#endif

        /* now flush the queue and notify all threads so they can be woken up */
        m_queue.flush(m_threads);

#if defined(DEBUG_WORKQUEUE)
        std::cout << "queue flushed and all worker threads notified" << std::endl;
#endif

        /* join and delete all asynchronous threads */
        for (std::size_t i = 0; i < m_threads; ++i) {
            m_worker_threads[i]->join();
            delete m_worker_threads[i];
        }

        delete [] m_worker_threads;

#if defined(DEBUG_WORKQUEUE)
        std::cout << "all threads destroyed" << std::endl;
#endif
    }

    workqueue(const workqueue&) = delete;
    workqueue(workqueue&&) = delete;
    workqueue& operator = (const workqueue&) = delete;
    workqueue& operator = (workqueue&&) = delete;

    bool is_valid() const
    {
        return m_is_valid;
    }

    void push_work(const work_base_sptr& work)
    {
        m_queue.push_work(work);
    }

    std::string to_string() const
    {
        std::ostringstream stream;

        stream << "workqueue@";
        stream << std::hex << this;
        stream << " [";
        stream << m_idstring;
        stream << ", ";
        stream << std::dec << m_threads;
        stream << " thread(s) in a pool";
        stream << "]";

        return stream.str();
    }

    operator std::string () const
    {
        return to_string();
    }

private:
    class queue
    {
    public:
        explicit queue() :
            m_mutex(),
            m_semaphore(),
            m_fifo()
        {
        }

        ~queue() = default;

        void flush(const std::size_t threads)
        {
            m_mutex.lock();
            while (!m_fifo.empty())
                m_fifo.pop();
            m_mutex.unlock();

            for (std::size_t i = 0; i < threads; ++i)
                m_semaphore.post();
        }

        void push_work(const work_base_sptr& work)
        {
            m_mutex.lock();
            m_fifo.push(work);
            m_mutex.unlock();

            m_semaphore.post();
        }

        work_base_sptr fetch_work()
        {
            work_base_sptr work;

            m_semaphore.wait();

            m_mutex.lock();
            if (!m_fifo.empty()) {
                work = m_fifo.front();
                m_fifo.pop();
            }
            m_mutex.unlock();

            return work;
        }

    private:
        std::mutex m_mutex; // protects access to m_queue
        semaphore m_semaphore; // blocks threads trying to fetch from empty queue
        std::queue<work_base_sptr> m_fifo;
    };

    class worker_thread
    {
    public:
        worker_thread(queue* q, const std::string& idstr) :
            m_queue(q),
            m_idstring(idstr),
            m_running(true),
            m_thread(&worker_thread::worker, this)
        {
#if defined(DEBUG_WORKQUEUE)
            std::cout << __PRETTY_FUNCTION__ << ": " << m_idstring << std::endl;
#endif
        }

        ~worker_thread()
        {
#if defined(DEBUG_WORKQUEUE)
            std::cout << __PRETTY_FUNCTION__ << ": " << m_idstring << std::endl;
#endif
        }

        void cancel()
        {
            m_running = false;
        }

        void join()
        {
            m_thread.join();
        }

    private:
        void worker() const
        {
#if defined(DEBUG_WORKQUEUE)
            std::cout << __PRETTY_FUNCTION__ << ": started: " << m_idstring << std::endl;
#endif
            while (m_running)
            {
                const work_base_sptr work = m_queue->fetch_work();
                if (work)
                    work->run();
            }

#if defined(DEBUG_WORKQUEUE)
            std::cout << __PRETTY_FUNCTION__ << ": completed: " << m_idstring << std::endl;
#endif
       }

    private:
        queue* m_queue;
        std::string m_idstring;
        std::atomic<bool> m_running;
        std::thread m_thread;
    };

    typedef worker_thread* worker_thread_ptr;


private:
    std::string m_idstring;
    std::size_t m_threads;
    queue m_queue;
    worker_thread_ptr* m_worker_threads;
    bool m_is_valid;
};

} /* end of namespace lts */

/*===========================================================================*\
 * inline function/variable definitions
\*===========================================================================*/
namespace lts
{

} /* end of namespace lts */

/*===========================================================================*\
 * global object declarations
\*===========================================================================*/
namespace lts
{

} /* end of namespace lts */

/*===========================================================================*\
 * function forward declarations
\*===========================================================================*/
namespace lts
{

} /* end of namespace lts */

#endif /* _WORKQUEUE_HPP_ */
