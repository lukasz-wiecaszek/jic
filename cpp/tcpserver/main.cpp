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

#include <unistd.h>
#include <getopt.h>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "strtointeger.hpp"
#include "tcpserver.hpp"

/*===========================================================================*\
 * 'using namespace' section
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/

/*===========================================================================*\
 * local types definitions
\*===========================================================================*/
namespace
{
} // end of anonymous namespace

/*===========================================================================*\
 * local (internal linkage) objects definitions
\*===========================================================================*/
static std::unique_ptr<lts::tcpserver> server;

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
    const char* address = TCPSERVER_DEFAULT_ADDRESS;
    int port = TCPSERVER_DEFAULT_PORT;

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

    server = std::make_unique<lts::tcpserver>(address, port);
    if (server->start() == false)
        std::exit(EXIT_FAILURE);

    // following two lines are helpful when testing stopping behaviour
    // std::this_thread::sleep_for(std::chrono::seconds(10));
    // server->stop();

    server->join();

    return 0;
}
