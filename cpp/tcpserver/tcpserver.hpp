/* SPDX-License-Identifier: MIT */
/**
 * @file tcpserver.hpp
 *
 * tcpserver example
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

#ifndef _TCPSERVER_HPP_
#define _TCPSERVER_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <string>
#include <memory>
#include <list>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <future>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "iostatus.hpp"
#include "session.hpp"

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define TCPSERVER_DEFAULT_ADDRESS "0.0.0.0"
#define TCPSERVER_DEFAULT_PORT    ((int)8888)
#define TCPSERVER_MAX_SESSIONS    5

/*===========================================================================*\
 * global types definitions
\*===========================================================================*/
namespace lts
{

/**
 * @class tcpserver
 *
 * An example TCP server.
 *
 * All public methods (including constructor and destructor)
 * must be called in the context of the same thread.
 * In other words, usage of this class' member functions is not thread safe.
 */
class tcpserver {
public:
    /**
     * @fn tcpserver::tcpserver
     *
     * @brief Creates a tcpserver instance.
     *
     * @param[in] address Address to bind to.
     * @param[in] port TCP port number used to listen for incoming connections.
     * @param[in] max_sessions Specifies maximum number of sessions this server can handle concurently.
     */
    tcpserver(const std::string& address = TCPSERVER_DEFAULT_ADDRESS,
              int port = TCPSERVER_DEFAULT_PORT,
              int max_sessions = TCPSERVER_MAX_SESSIONS);
    ~tcpserver();

    // server shall be non-copyable and non-movable
    tcpserver(const tcpserver&) = delete;
    tcpserver(tcpserver&&) = delete;
    tcpserver& operator=(const tcpserver&) = delete;
    tcpserver& operator=(tcpserver&&) = delete;

    /**
     * @fn tcpserver::start
     *
     * @brief Starts the server. From now on the server
     *        will be accepting incominng connections.
     *
     * @return true when start has succeeded, false otherwise.
     */
    bool start();

    /**
     * @fn tcpserver::stop
     *
     * @brief Stops the server.
     *        This call will block until all active sessions are terminated.
     *
     * @return none
     */
    void stop();

    /**
     * @fn tcpserver::join
     *
     * @brief Joins the server thread.
     *
     * @return Execution status.
     */
    iostatus join();

    std::string to_string() const
    {
        return m_address + ":" + std::to_string(m_port);
    }

    operator std::string() const
    {
        return to_string();
    }

private:
    iostatus init();
    void session_create();
    void session_destroy(std::shared_ptr<lts::session> session);
    iostatus worker();
    void thread_function(std::promise<iostatus>&& promise);

private:
    const std::string m_address;
    const int m_port;
    const int m_max_sessions;
    int m_sockfd;
    int m_pipefds[2];
    std::list<std::shared_ptr<session>> m_sessions;
    std::mutex m_sessions_mutex;
    std::condition_variable m_condvar;
    std::unique_ptr<std::jthread> m_thread;
    std::future<iostatus> m_future;
};

} /* end of namespace lts */

/*===========================================================================*\
 * inline function/variable definitions
\*===========================================================================*/
namespace lts
{
} /* end of namespace lts */

/*===========================================================================*\
 * global (external linkage) objects declarations
\*===========================================================================*/
namespace lts
{
} /* end of namespace lts */

/*===========================================================================*\
 * function forward declarations (external linkage)
\*===========================================================================*/
namespace lts
{
} /* end of namespace lts */

#endif /* _TCPSERVER_HPP_ */
