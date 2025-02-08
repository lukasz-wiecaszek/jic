/* SPDX-License-Identifier: MIT */
/**
 * @file main.cpp
 *
 * Entry point of tcpserver example.
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <memory>
#include <chrono>
#include <thread>
#include <future>
#include <array>

#include <unistd.h>
#include <getopt.h>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "strtointeger.hpp"
#include "tcpclient.hpp"

/*===========================================================================*\
 * 'using namespace' section
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define SLEEP_TIME_SECONDS 10

/*===========================================================================*\
 * local types definitions
\*===========================================================================*/
namespace
{

struct client {
    std::shared_ptr<lts::tcpclient> connection;
    lts::iostatus status;
};

} // end of anonymous namespace

/*===========================================================================*\
 * local (internal linkage) objects definitions
\*===========================================================================*/
static std::array<client, 1> clients;

/*===========================================================================*\
 * global (external linkage) objects definitions
\*===========================================================================*/

/*===========================================================================*\
 * local (internal linkage) functions definitions
\*===========================================================================*/

/*===========================================================================*\
 * class public functions definitions
\*===========================================================================*/

/*===========================================================================*\
 * class protected functions definitions
\*===========================================================================*/

/*===========================================================================*\
 * class private functions definitions
\*===========================================================================*/

/*===========================================================================*\
 * global (external linkage) functions definitions
\*===========================================================================*/
int main(int argc, char* argv[])
{
    int status;
    int c;
    const char* address = TCPCLIENT_DEFAULT_ADDRESS;
    int port = TCPCLIENT_DEFAULT_PORT;

    static struct option long_options[] = {
        {"address", required_argument, 0, 'a'},
        {   "port", required_argument, 0, 'p'},
        {        0,                 0, 0,   0}
    };

    fprintf(stdout, "%s: pid: %d, tid: %d\n", argv[0], getpid(), gettid());

    do {
        c = getopt_long(argc, argv, "a:p:", long_options, 0);
        if (c != -1) {
            switch (c) {
                case 'a':
                {
                    address = optarg;
                } break;

                case 'p':
                {
                    if (lts::strtointeger(optarg, port) != lts::strtointeger_conversion_status_e::success) {
                        std::fprintf(stderr, "Cannot convert '%s' to corresponding port value\n", optarg);
                        std::exit(EXIT_FAILURE);
                    }
                } break;

                default:
                {
                    /* do nothing */
                } break;
            }
        }
    } while (c != -1);

    struct sigaction action;

    std::memset(&action, 0, sizeof(action));
    action.sa_handler = SIG_IGN;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    status = ::sigaction(SIGPIPE, &action, NULL);
    if (status != 0) {
        std::fprintf(stderr, "sigaction(SIGPIPE) failed with code %d (%s)\n", errno, strerror(errno));
        std::exit(EXIT_FAILURE);
    }

    int i = 0;
    for (auto&& client : clients) {
        client.connection = std::make_shared<lts::tcpclient>(std::to_string(i), address, port);
        if (client.connection->start() == false)
            std::exit(EXIT_FAILURE);
        i++;
    }

    std::thread{[]() {
        std::this_thread::sleep_for(std::chrono::seconds(SLEEP_TIME_SECONDS));
        for (auto client : clients) /* copy be value - this is deliberate to make another copy of shared_ptr(s) */
            client.connection->stop();
    }}.detach();

    for (auto&& client : clients)
        client.status = client.connection->join();

    for (auto&& client : clients) {
        if (client.status.has_value())
            std::fprintf(stdout,
                         "[%s] client terminated with success (performed %d send/receive iterations in %d second(s))\n",
                         client.connection->to_string().c_str(),
                         client.status.value(),
                         SLEEP_TIME_SECONDS);
        else
            std::fprintf(stdout,
                         "[%s] client failed with code %d (%s)\n",
                         client.connection->to_string().c_str(),
                         client.status.error(),
                         strerror(client.status.error()));
    }

    return 0;
}
