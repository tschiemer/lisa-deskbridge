/**
* L-ISA Deskbridge
* Copyright (C) 2025  Philip Tschiemer, https://github.com/tschiemer/lisa-deskbridge
*
* This program is free software: you can redistribute it and/or modify
        * it under the terms of the GNU Affero General Public License as published by
        * the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
        * but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
        * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <getopt.h>

#include "Bridge.h"
#include "bridges/SQ6.h"

static char * argv0 = nullptr;

LisaDeskbridge::Bridge * bridge;

static struct {
    bool verbose;
    std::basic_string_view<char> bridgeName;
    LisaDeskbridge::Bridge::BridgeOpts bridgeOpts;
    unsigned short localPort;
    std::basic_string_view<char> lisaHost;
    unsigned short lisaPort;
} opts = {
    .verbose = false,
    .bridgeName = "",
    .localPort = LisaDeskbridge::kRemotePortDefault,
    .lisaHost = "127.0.0.1",
    .lisaPort = LisaDeskbridge::kLisaControllerPortDefault
};


static void help(){
    fprintf(stdout,
        "Usage: %s [-h|-?] [-v] [(-o|--bridge-opt <key1>=<value1>)*] [...] [<bridge>]\n"
        "Bridge different custom control elements to comfortably control L-ISA Controller.\n"
        "For further documentation see https://github.com/tschiemer/lisa-deskbridge\n"
        "\nArguments:\n"
        "\t <bridge>                Bridge to use: SQ6\n"
        "\nOptions:\n"
        "\t -h, -?                  Show this help\n"
        "\t -v                      Verbose output\n"
        "\t -p,--local-port         Local port to receive L-ISA Controller OSC messages (default: %hu)\n"
        "\t --lisa-host             L-ISA Controller host/ip (default: %s)\n"
        "\t --lisa-port             L-ISA Controller port (default: %hu\n"
        "\t -b,--bridge-opt         Pass (multiple) options to bridge using form 'key=value'\n"
        "\nBridge options:\n"
        "%s\n"
        "\nExamples:\n"
        "%s SQ6 #to use SQ6 bridge with default options\n"
        "%s -p 9000 --lisa-port 8880 --lisa-host 127.0.0.1 -o \"midiin=MIDI Control 1\" -o \"midiout=MIDI Control 1\" SQ6 # to use SQ6 bridge with custom options (which happen to be the default ones)\n"
        , argv0,
        LisaDeskbridge::kRemotePortDefault, LisaDeskbridge::kLisaControllerHostDefault.data(), LisaDeskbridge::kLisaControllerPortDefault,
        LisaDeskbridge::Bridges::SQ6::helpOpts,
        argv0, argv0 // examples
    );
}

int main(int argc, char * argv[]) {

    argv0 = argv[0];

    // parse options
    while (1) {
        int c;

        int option_index = 0;
        static struct option long_options[] = {
                {"local-port", required_argument, 0, 'p'},
                {"lisa-host",required_argument,0,1},
                {"lisa-port",required_argument,0,1},
                {"bridge-opt", required_argument,0,'o'},
                {0,0,0,0}
        };

        c = getopt_long(argc, argv, "?hvo:p:",
                        long_options, &option_index);
        if (c == -1)
            break;

        std::basic_string_view<char> arg, key, value;
        unsigned int pos;

        switch (c) {

            case 'p': // local listen port for L-ISA controller messages
                break;

            case 1: // --lisa-host
                opts.lisaHost = optarg;
                break;

            case 2: // --lisa-port
                opts.lisaPort = std::atoi(optarg);
                break;

            case 'o': // bridge specific options
                arg = optarg;
                pos = arg.find_first_of('=');
                key = arg.substr(0, pos);
                value = arg.substr(pos+1);
                opts.bridgeOpts[key] = value;
                break;

            case 'v':
                opts.verbose = true;
                break;

            case '?':
            case 'h':
                help();
                return EXIT_SUCCESS;

            default:
                std::cout << "Unrecognized option " << c << std::endl;
                help();
                return EXIT_FAILURE;
        }
    }

    if (optind == argc){
        std::cout << "Missing argument <bridgeName>" << std::endl;
        help();
        return EXIT_FAILURE;
    } else if (optind + 1 < argc){
        std::cout << "Too many arguments?!" << std::endl;
        help();
        return EXIT_FAILURE;
    }

    opts.bridgeName = argv[optind];

    bridge = LisaDeskbridge::Bridge::factory(opts.bridgeName, opts.bridgeOpts);
    if (bridge == nullptr){
        std::cerr << "Invalid bridgeName: " << opts.bridgeName << std::endl;
        return EXIT_FAILURE;
    }
    std::cerr << "Using bridge: " << opts.bridgeName << std::endl;

    bridge->setLisaControllerOpts(opts.localPort, opts.lisaHost, opts.lisaPort);

    if (bridge->init() == false){
        return EXIT_FAILURE;
    }

    // This will run in a process loop until done
    bridge->runloop();

    bridge->deinit();

    return EXIT_SUCCESS;
}
