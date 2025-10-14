/**
 * @file parser_h264_test.cpp
 *
 * Test procedures for 'parser_h264'.
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
#include "parser_h264.hpp"

/*===========================================================================*\
 * 'using namespace' section
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define READ_BUFFER_SIZE                  (4 * 1024)
#define PARSER_BUFFER_SIZE                (2 * 1024 * 1024)

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
static void parser_h264_parse(lts::parser_h264& parser);
static void parser_h264_feed(lts::parser_h264& parser, const uint8_t* data, std::size_t count);

/*===========================================================================*\
 * local object definitions
\*===========================================================================*/
static std::ofstream parser_h264_ofile;

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/
static inline void parser_h264_usage(const char* progname)
{
    std::cout << "usage: " << progname << " [-r | -a] [-o ofile] <h264file>" << std::endl;
    std::cout << " options: " << std::endl;
    std::cout << "  -r --rtp                : specifies that h264 stream is encapsulated by RTP Payload Format according to RFC 6184" << std::endl;
    std::cout << "  -a --annex-b            : specifies that h264 stream is encapsulated by ISO/IEC 14496-10 Annex B" << std::endl;
    std::cout << "  -o ofile --output=ofile : stores h264 stream (with Annex B encapsulation) in file depicted by ofile" << std::endl;
    std::cout << "  h264file                : filename carrying h264 stream to be parsed" << std::endl;
    std::cout << std::endl;
}

/*===========================================================================*\
 * public function definitions
\*===========================================================================*/
int main(int argc, char *argv[])
{
    struct {
        bool rtp;
        bool annex_b;
    } encapsulation = {};
    const char* ofile = nullptr;

    lts::parser_h264_container_e container = lts::parser_h264_container_e::NONE;

    static struct option long_options[] = {
        {"rtp",     no_argument,       0, 'r'},
        {"annex-b", no_argument,       0, 'a'},
        {"ofile",   required_argument, 0, 'o'},
        {0,         0,                 0,  0 }
    };

    for (;;) {
        int c = getopt_long(argc, argv, "rao:", long_options, 0);
        if (-1 == c)
            break;

        switch(c) {
            case 'r':
                encapsulation.rtp = true;
                break;

            case 'a':
                encapsulation.annex_b = true;
                container = lts::parser_h264_container_e::ANNEX_B;
                break;

            case 'o':
                ofile = optarg;
                break;

            default:
                /* does nothing */
                break;
        }
    }

    std::cout << "encapsulation:"
        << " rtp=" << (encapsulation.rtp ? "y" : "n")
        << " annex-b=" << (encapsulation.annex_b ? "y" : "n")
        << std::endl;
    std::cout << "container: " << to_string(container) << std::endl;

    const char* filename = argv[optind];
    if (!filename) {
         std::cerr << "error: missing h264 file" << std::endl;
         parser_h264_usage(argv[0]);
         exit(EXIT_FAILURE);
    }

    if (ofile != nullptr) {
        parser_h264_ofile = std::ofstream(ofile, std::ios::out | std::ios::binary);
        if (!parser_h264_ofile.is_open()) {
            std::cerr << "error: could not open '" << ofile << "'" << std::endl;
            parser_h264_usage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (encapsulation.rtp) {
    }

    std::chrono::time_point<std::chrono::high_resolution_clock> t1, t2;

    t1 = std::chrono::high_resolution_clock::now();

    std::size_t read_bytes = 0;
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (file.is_open()) {
        std::streamsize count;
        char* memblock;

        memblock = new char[READ_BUFFER_SIZE];

        lts::parser_h264 parser(PARSER_BUFFER_SIZE, container);

        do {
            count = file.readsome(memblock, READ_BUFFER_SIZE);
            read_bytes += count;
            if (count > 0)
                parser_h264_feed(parser, reinterpret_cast<uint8_t*>(memblock), count);
        } while (count > 0);

        file.close();
        std::cout << "read " << read_bytes << " bytes from '" << filename << "'" << std::endl;
    }
    else {
        std::cerr << "error: could not open '" << filename << "'" << std::endl;
    }

    parser_h264_ofile.close();

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
static void parser_h264_parse(lts::parser_h264& parser)
{
    lts::parser_h264_status_e status;

    do {
        status = parser.parse();

        switch (status) {
            case lts::parser_h264_status_e::NAL_UNIT_SKIPPED:
                break;

            case lts::parser_h264_status_e::NAL_UNIT_CORRUPTED:
                break;

            case lts::parser_h264_status_e::AUD_PARSED:
            {
                const lts::h264_structure* structure = parser.get_structure(
                        lts::parser_h264_structure_e::AUD,
                        H264_PARSER_STRUCTURE_ID_RECENT);
                if (structure)
                    std::cout << structure->to_string() << std::endl;
                else
                    std::cout << "Parser returned '" << lts::to_string(status) <<
                        "' but associated structure cannot be retrived" << std::endl;
                break;
            }

            case lts::parser_h264_status_e::SPS_PARSED:
            {
                const lts::h264_structure* structure = parser.get_structure(
                        lts::parser_h264_structure_e::SPS,
                        H264_PARSER_STRUCTURE_ID_RECENT);
                if (structure)
                    std::cout << structure->to_string() << std::endl;
                else
                    std::cout << "Parser returned '" << lts::to_string(status) <<
                        "' but associated structure cannot be retrived" << std::endl;
                break;
            }

            case lts::parser_h264_status_e::PPS_PARSED:
            {
                const lts::h264_structure* structure = parser.get_structure(
                        lts::parser_h264_structure_e::PPS,
                        H264_PARSER_STRUCTURE_ID_RECENT);
                if (structure)
                    std::cout << structure->to_string() << std::endl;
                else
                    std::cout << "Parser returned '" << lts::to_string(status) <<
                        "' but associated structure cannot be retrived" << std::endl;
                break;
            }

            case lts::parser_h264_status_e::SEI_PARSED:
            {
                const lts::h264_structure* structure = parser.get_structure(
                        lts::parser_h264_structure_e::SEI,
                        H264_PARSER_STRUCTURE_ID_RECENT);
                if (structure)
                    std::cout << structure->to_string() << std::endl;
                else
                    std::cout << "Parser returned '" << lts::to_string(status) <<
                        "' but associated structure cannot be retrived" << std::endl;
                break;
            }

            case lts::parser_h264_status_e::SLICE_PARSED:
            {
                const lts::h264_structure* structure = parser.get_structure(
                        lts::parser_h264_structure_e::SLICE_HEADER,
                        H264_PARSER_STRUCTURE_ID_RECENT);
                if (structure)
                    std::cout << structure->to_string() << std::endl;
                else
                    std::cout << "Parser returned '" << lts::to_string(status) <<
                        "' but associated structure cannot be retrived" << std::endl;
                break;
            }

            default:
                break;
       }
    } while (status != lts::parser_h264_status_e::NEED_BYTES);
}

static void parser_h264_feed(lts::parser_h264& parser, const uint8_t* data, std::size_t count)
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

        parser_h264_parse(parser);

        data += n_written;
        count -= n_written;
    } while (count > 0);
}
