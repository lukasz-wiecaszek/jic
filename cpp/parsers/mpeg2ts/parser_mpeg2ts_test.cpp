/**
 * @file parser_mpeg2ts_test.cpp
 *
 * Test procedures for 'parser_mpeg2ts'.
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

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <iostream>
#include <fstream>
#include <chrono>
#include <map>

#include <cstdlib>
#include <cassert>

extern "C" {
    #include <unistd.h>
    #include <getopt.h>
}

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "../../utils/utilities.hpp"
#include "../../utils/ioutilities.hpp"
#include "../../utils/strtointeger.hpp"
#include "mpeg2ts_pid.hpp"
#include "parser_mpeg2ts.hpp"

/*===========================================================================*\
 * 'using namespace' section
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define READ_BUFFER_SIZE                  (4 * 1024)
#define PARSER_BUFFER_SIZE                (2 * READ_BUFFER_SIZE)

/*===========================================================================*\
 * local type definitions
\*===========================================================================*/
namespace
{

} // end of anonymous namespace

/*===========================================================================*\
 * global object definitions
\*===========================================================================*/

/*===========================================================================*\
 * local function declarations
\*===========================================================================*/
static void parser_mpeg2ts_parse(lts::parser_mpeg2ts& parser);
static void parser_mpeg2ts_feed(lts::parser_mpeg2ts& parser, const uint8_t* data, std::size_t count);

/*===========================================================================*\
 * local object definitions
\*===========================================================================*/
static lts::mpeg2ts_pid parser_mpeg2ts_pid = MPEG2TS_PID_INVALID;
static std::ofstream parser_mpeg2ts_ofile;

static std::map<lts::mpeg2ts_pid, std::size_t> parser_mpeg2ts_pids;

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/
static inline void parser_mpeg2ts_usage(const char* progname)
{
    std::cout << "usage: " << progname << " [-p pid] [-o ofile] <tsfile>" << std::endl;
    std::cout << " options: " << std::endl;
    std::cout << "  -p pid --pid=pid        : selects packets with id equal to pid" << std::endl;
    std::cout << "  -o ofile --output=ofile : stores selected packets in file depicted by ofile" << std::endl;
    std::cout << "  tsfile                  : filename carrying mpeg2 transport stream to be parsed" << std::endl;
    std::cout << std::endl;
}

/*===========================================================================*\
 * public function definitions
\*===========================================================================*/
int main(int argc, char *argv[])
{
    bool status;
    const char* ofile = nullptr;
    uint16_t pid;

    static struct option long_options[] = {
        {"pid",     required_argument, 0, 'p'},
        {"ofile",   required_argument, 0, 'o'},
        {0,         0,                 0,  0 }
    };

    for (;;) {
        int c = getopt_long(argc, argv, "p:o:", long_options, 0);
        if (-1 == c)
            break;

        switch(c) {
            case 'p':
                status = (lts::strtointeger_conversion_status_e::success == lts::strtointeger(optarg, pid));
                if (!status) {
                    std::cerr << "error: cannot convert '" << optarg << "' to integer" << std::endl;
                    parser_mpeg2ts_usage(argv[0]);
                    exit(EXIT_FAILURE);
                }
                parser_mpeg2ts_pid = pid;
                break;

            case 'o':
                ofile = optarg;
                break;

            default:
                /* does nothing */
                break;
        }
    }

    const char* filename = argv[optind];
    if (!filename) {
         std::cerr << "error: missing tranport stream file" << std::endl;
         parser_mpeg2ts_usage(argv[0]);
         exit(EXIT_FAILURE);
    }

   if (ofile != nullptr) {
      parser_mpeg2ts_ofile = std::ofstream(ofile, std::ios::out | std::ios::binary);
      if (!parser_mpeg2ts_ofile.is_open()) {
         std::cerr << "error: could not open '" << ofile << "'" << std::endl;
         parser_mpeg2ts_usage(argv[0]);
         exit(EXIT_FAILURE);
      }
   }

    std::chrono::time_point<std::chrono::high_resolution_clock> t1, t2;

    t1 = std::chrono::high_resolution_clock::now();

    std::size_t read_bytes = 0;
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (file.is_open()) {
        std::streamsize count;
        char* memblock;

        memblock = new char[READ_BUFFER_SIZE];

        lts::parser_mpeg2ts parser(PARSER_BUFFER_SIZE);

        do {
            count = file.readsome(memblock, READ_BUFFER_SIZE);
            read_bytes += count;
            if (count > 0)
                parser_mpeg2ts_feed(parser, reinterpret_cast<uint8_t*>(memblock), count);
        } while (count > 0);

        file.close();
        std::cout << "read " << read_bytes << " bytes from '" << filename << "'" << std::endl;
    }
    else {
        std::cerr << "error: could not open '" << filename << "'" << std::endl;
    }

    parser_mpeg2ts_ofile.close();

    for (const auto& i : parser_mpeg2ts_pids)
        std::cout << i.first.to_string() << ": " << i.second << std::endl;

    t2 = std::chrono::high_resolution_clock::now();
    uint64_t duration = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count());

    std::cout << "test took " << duration << "ms" << std::endl;

    return 0;
}

/*===========================================================================*\
 * protected function definitions
\*===========================================================================*/

/*===========================================================================*\
 * private function definitions
\*===========================================================================*/

/*===========================================================================*\
 * local function definitions
\*===========================================================================*/
static void parser_mpeg2ts_parse(lts::parser_mpeg2ts& parser)
{
    lts::parser_mpeg2ts_status_e status;
    const uint8_t *tspacket;

    do {
        status = parser.parse();

        switch (status) {
            case lts::parser_mpeg2ts_status_e::SYNC_GAINED:
                std::cout << to_string(status) << std::endl;
                break;

            case lts::parser_mpeg2ts_status_e::SYNCHRONIZED:
                tspacket = parser.get_tspacket();
                if (parser_mpeg2ts_ofile.is_open())
                    if (!parser_mpeg2ts_pid.is_valid() || (lts::parser_mpeg2ts_get_pid(tspacket) == parser_mpeg2ts_pid))
                        parser_mpeg2ts_ofile.write(reinterpret_cast<const char*>(tspacket), PARSER_MPEG2TS_PACKET_SIZE);

                parser_mpeg2ts_pids[lts::parser_mpeg2ts_get_pid(tspacket)]++;
                break;

            case lts::parser_mpeg2ts_status_e::SYNC_LOST:
                std::cout << to_string(status) << std::endl;
                break;

            case lts::parser_mpeg2ts_status_e::NOT_SYNCHRONIZED:
                break;

            case lts::parser_mpeg2ts_status_e::TRANSPORT_RATE_DETECTED:
                std::cout << to_string(status) << std::endl;
                std::cout << "transport rate: " << parser.get_tspacket_rate() << " packets per second" << std::endl;
                break;

            default:
                break;
       }
    } while (status != lts::parser_mpeg2ts_status_e::NEED_BYTES);
}

static void parser_mpeg2ts_feed(lts::parser_mpeg2ts& parser, const uint8_t* data, std::size_t count)
{
    std::size_t n_written;

    do {
        n_written = parser.write(data, count);
        if (0 == n_written) {
            parser.reset();
            continue;
        }

        if (n_written > count) {
            std::cerr << "error: parser swallowed more data then requested (this is really bad)" << std::endl;
            exit(EXIT_FAILURE);
        }

        parser_mpeg2ts_parse(parser);

        data += n_written;
        count -= n_written;
    } while (count > 0);
}
