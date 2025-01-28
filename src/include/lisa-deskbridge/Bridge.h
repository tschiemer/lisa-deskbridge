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

#ifndef LISA_DESKBRIDGE_BRIDGE_H
#define LISA_DESKBRIDGE_BRIDGE_H

#include "LisaControllerProxy.h"

#include <string>
#include <map>

namespace LisaDeskbridge {

    class Bridge : public LisaDeskbridge::LisaControllerProxy::IDelegate {

        public:

            enum State {State_Stopped, State_Starting, State_Started, State_Stopping};

            typedef std::map<std::basic_string_view<char>,std::basic_string_view<char>> BridgeOpts;

        protected:

            enum State state = State_Stopped;

            Bridge(BridgeOpts &opts);

        public:

            static Bridge * factory(std::basic_string_view<char> &name, BridgeOpts &conf);

            virtual bool init();

            virtual void runloop();

            virtual void stop();

            virtual void deinit();


        protected:

            uint16_t localPort                                  = LisaDeskbridge::kRemotePortDefault;
            std::basic_string_view<char> lisaControllerHost     = LisaDeskbridge::kLisaControllerHostDefault;
            uint16_t lisaControllerPort                         = LisaDeskbridge::kLisaControllerPortDefault;

            LisaControllerProxy lisaControllerProxy;

            bool startLisaControllerProxy();
            void stopLisaControllerProxy();

        public:

            void setLisaControllerOpts(uint16_t local_port, std::basic_string_view<char> &controllerHost, uint16_t controllerPort){

                localPort = local_port;
                lisaControllerHost = controllerHost;
                lisaControllerPort = controllerPort;
            }

    };

}

#endif //LISA_DESKBRIDGE_BRIDGE_H
