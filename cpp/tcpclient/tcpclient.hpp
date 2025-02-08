/* SPDX-License-Identifier: MIT */
/**
 * @file tcpclient.hpp
 *
 * tcpclient example
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

#ifndef _TCPCLIENT_HPP_
#define _TCPCLIENT_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <string>
#include <memory>
#include <thread>
#include <future>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "charbuffer.hpp"
#include "iostatus.hpp"

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define TCPCLIENT_DEFAULT_ADDRESS "127.0.0.1"
#define TCPCLIENT_DEFAULT_PORT    ((int)8888)

/*===========================================================================*\
 * global types definitions
\*===========================================================================*/
namespace lts
{

/**
 * @class tcpclient
 *
 * An example TCP client.
 *
 * A bit about thread safety.
 * In principle all methods (including constructor and destructor)
 * shall be called from the context of the same thread.
 * The one and only method which does not follow this rule and is allowed
 * to be concurrently invoked from multiple threads is the 'stop()' method.
 */
class tcpclient {
public:
    /**
     * @fn tcpclient::tcpclient
     *
     * @brief Creates a tcpclient instance.
     *
     * @param[in] name Name of the client.
     * @param[in] address Address to connect to.
     * @param[in] port TCP port number to connect to.
     */
    tcpclient(const std::string& name,
              const std::string& address = TCPCLIENT_DEFAULT_ADDRESS,
              int port = TCPCLIENT_DEFAULT_PORT);
    ~tcpclient();

    // client shall be non-copyable and non-movable
    tcpclient(const tcpclient&) = delete;
    tcpclient(tcpclient&&) = delete;
    tcpclient& operator=(const tcpclient&) = delete;
    tcpclient& operator=(tcpclient&&) = delete;

    /**
     * @fn tcpclient::start
     *
     * @brief Starts the client.
     *
     * @return true when start has succeeded, false otherwise.
     */
    bool start();

    /**
     * @fn tcpclient::stop
     *
     * @brief Stops the client.
     *
     * @return none
     * @note Can be concurrently invoked from multiple threads.
     */
    void stop();

    /**
     * @fn tcpclient::join
     *
     * @brief Joins the client thread.
     *
     * @return Execution status.
     */
    iostatus join();

    std::string to_string() const
    {
        return m_address + ":" + std::to_string(m_port) + "/" + m_name;
    }

    operator std::string() const
    {
        return to_string();
    }

private:
    iostatus init();
    iostatus write(int sockfd, const std::string& text);
    iostatus read(int sockfd, charbuffer& buffer);
    iostatus worker(int sockfd);
    void thread_function(std::promise<iostatus>&& promise);

private:
    const std::string m_name;
    const std::string m_address;
    const int m_port;
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

#endif /* _TCPCLIENT_HPP_ */
