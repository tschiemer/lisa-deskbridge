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
#include <iostream>

#include <syslog.h>

#include "lisa-deskbridge/log.h"
#include "sqmixmitm/log.h"

#include "lisa-deskbridge/Bridge.h"
#include "lisa-deskbridge/bridges/Generic.h"
#include "lisa-deskbridge/bridges/SQMidi.h"
#include "lisa-deskbridge/bridges/SQMitm.h"

static char * argv0 = nullptr;

LisaDeskbridge::Bridge * bridge;

static struct {
    int logLevel;
    std::string bridgeName;
    LisaDeskbridge::Bridge::BridgeOpts bridgeOpts;
    unsigned short localPort;
    std::basic_string_view<char> lisaHost;
    unsigned short lisaPort;
} opts = {
    .logLevel = LisaDeskbridge::LogLevelInfo,
    .bridgeName = "",
    .localPort = LisaDeskbridge::kDevicePortDefault,
    .lisaHost = "127.0.0.1",
    .lisaPort = LisaDeskbridge::kLisaControllerPortDefault
};


static void help(){
    fprintf(stdout,
        "Usage: %s [-h|-?] [-v<verbosity>] [(-o|--bridge-opt <key1>=<value1>)*] [...] [<bridge>]\n"
        "Bridge different custom control elements to comfortably control L-ISA Controller.\n"
        "For further documentation see https://github.com/tschiemer/lisa-deskbridge\n"
        "\nArguments:\n"
        "\t <bridge>                Bridge to use: %s, %s, %s\n"
        "\nOptions:\n"
        "\t -h, -?                  Show this help\n"
        "\t -v<verbosity>           Verbose output (in 0 (none), 1 (error), 2 (info = default), 3 (debug)\n"
        "\t --lisa-ip               L-ISA Controller ip (default: %s)\n"
        "\t --lisa-port             L-ISA Controller port (default: %hu)\n"
        "\t --device-ip             Own OSC target IP as will be registered with L-ISA Controller (default: 127.0.0.1)\n"
        "\t --device-port           Own OSC target port as will be registered with L-ISA Controller (default: %hu)\n"
        "\t --device-id             Registration ID of this device with L-ISA Controller (range 1-10, default 1)\n"
        "\t --device-name           Registration Name this device (default: L-ISA Deskbridge)\n"
        "\t --claim-level-control   Claims for itself level control (can only be occupied by one device)\n"
        "\t -o,--bridge-opt         Pass (multiple) options to bridge using form 'key=value'\n"
        "\nCommon bridge options:\n"
        "%s\n"
        "Specific bridge options:\n"
        "%s\n"
        "%s\n"
        "%s\n"
        "\nExamples:\n"
        "%s SQ-Midi #to use SQ-Midi bridge with default options\n"
        "%s -p 9000 --lisa-port 8880 --lisa-host 127.0.0.1 -o \"midiin=MIDI Control 1\" -o \"midiout=MIDI Control 1\" SQ-Midi # to use SQ-Midi bridge with custom options (which happen to be the default ones)\n"
        "%s -v2 -o mixer-ip=10.0.0.100 SQ-Mitm # SQ-Mitm bridge with INFO-level verbosity\n"
        , argv0,
            LisaDeskbridge::Bridges::Generic::kName, LisaDeskbridge::Bridges::SQMidi::kName, LisaDeskbridge::Bridges::SQMitm::kName,
             LisaDeskbridge::kLisaControllerIpDefault, LisaDeskbridge::kLisaControllerPortDefault,
            LisaDeskbridge::kDevicePortDefault,
            LisaDeskbridge::Bridge::helpOpts,
            LisaDeskbridge::Bridges::Generic::helpOpts,
            LisaDeskbridge::Bridges::SQMidi::helpOpts,
            LisaDeskbridge::Bridges::SQMitm::helpOpts,
            argv0, argv0, argv0 // examples
    );
}

int main(int argc, char * argv[]) {

    argv0 = argv[0];

    // parse options
    while (1) {
        int c;

        int option_index = 0;
        static struct option long_options[] = {
                {"lisa-ip",required_argument,0,1},
                {"lisa-port",required_argument,0,2},
                {"device-ip",required_argument,0,3},
                {"device-port",required_argument,0,4},
                {"device-id",required_argument,0,5},
                {"device-name",required_argument,0,6},
                {"claim-level-control",required_argument,0,7},
                {"bridge-opt", required_argument,0,'o'},
                {0,0,0,0}
        };

        c = getopt_long(argc, argv, "?hv:o:p:",
                        long_options, &option_index);
        if (c == -1)
            break;

        std::string arg, key, value;
        unsigned int pos;

        switch (c) {

            case 1: // --lisa-ip
                opts.bridgeOpts["lisa-controller-ip"] = optarg;
                break;

            case 2: // --lisa-port
                opts.bridgeOpts["lisa-controller-port"] = optarg;
                break;

            case 3: // --device-id
                opts.bridgeOpts["device-ip"] = optarg;
                break;

            case 4: // --device-name
                opts.bridgeOpts["device-port"] = optarg;
                break;

            case 5: // --device-id
                opts.bridgeOpts["device-id"] = optarg;
                break;

            case 6: // --device-name
                opts.bridgeOpts["device-name"] = optarg;
                break;

            case 7: // --claim-level-control
                opts.bridgeOpts["claim-level-control"] = optarg;
                break;

            case 'o': // bridge specific options
                arg = optarg;
                pos = arg.find_first_of('=');
                key = arg.substr(0, pos);
                value = arg.substr(pos+1);
                opts.bridgeOpts[key] = value;
                break;

            case 'v':
                opts.logLevel = (LisaDeskbridge::LogLevel)std::atoi(optarg);
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

    // setup logging components
    LisaDeskbridge::setLogLevel((LisaDeskbridge::LogLevel)opts.logLevel);

    SQMixMitm::setLogLevel((SQMixMitm::LogLevel)opts.logLevel);
    SQMixMitm::setLogFunction((SQMixMitm::LogFunction)LisaDeskbridge::log);


    opts.bridgeName = argv[optind];

    bridge = LisaDeskbridge::Bridge::factory(opts.bridgeName, opts.bridgeOpts);
    if (bridge == nullptr){
        std::cout << "Invalid bridge: " << opts.bridgeName << std::endl;
        return EXIT_FAILURE;
    }
    LisaDeskbridge::log(LisaDeskbridge::LogLevelInfo, "Using bridge: %s", opts.bridgeName.data());

    if (bridge->start() == false){
        return EXIT_FAILURE;
    }

    // This will run in a process loop until done
    bridge->runloop();

    bridge->stop();

    return EXIT_SUCCESS;
}
