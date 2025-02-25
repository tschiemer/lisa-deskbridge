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

    class Bridge : public LisaDeskbridge::LisaControllerProxy::Delegate {

        public:

            enum State {State_Stopped, State_Starting, State_Started, State_Stopping};

            typedef std::map<std::string,std::string> BridgeOpts;

        public:

            static constexpr char kOptLisaControllerIp[]    = "lisa-controller-ip";
            static constexpr char kOptLisaControllerPort[]  = "lisa-controller-port";

            static constexpr char kOptDeviceIP[]            = "device-ip";
            static constexpr char kOptDevicePort[]          = "device-port";
            static constexpr char kOptDeviceID[]            = "device-id";
            static constexpr char kOptDeviceName[]          = "device-name";

            static constexpr char kOptClaimLevelControl[]   = "claim-level-control";

            static constexpr char helpOpts[] = "\n"
                                               "\t lisa-controller-ip\n"
                                               "\t lisa-controller-port\n"
                                               "\t device-ip\n"
                                               "\t device-port\n"
                                               "\t device-id\n"
                                               "\t device-name\n"
                                               "\t claim-level-control\n";

        protected: // Core

            enum State state = State_Stopped;

            LisaControllerProxy lisaControllerProxy_;

        protected: // Settings

            std::string lisaControllerIp_                       = LisaDeskbridge::kLisaControllerIpDefault;
            uint16_t lisaControllerPort_                        = LisaDeskbridge::kLisaControllerPortDefault;

            std::string deviceIp_                               = "127.0.0.1";
            uint16_t devicePort_                                = LisaDeskbridge::kDevicePortDefault;
            uint8_t deviceId_                                   = 1;
            std::string deviceName_                             = "L-ISA Deskbridge";

//            bool register_                                      = false;
            bool claimLevelControl_                             = true;


        protected:

            Bridge(BridgeOpts &opts);

        public:

            ~Bridge();

        public:

            static Bridge * singleton();

            static Bridge * factory(std::string &name, BridgeOpts &conf);

            /**
             * Attempts to start bridge.
             * Please only override startImpl()
             * @return success?
             */
            bool start();

            /**
             * Stops/shuts down bridge
             * Please only override stopImpl();
             */
            void stop();

            /**
             * Runloop: platform dependent code to do necessary event handling.
             * Not always needed... (on macos, if there is a main runloop anyways, not)
             *
             * Does not return until stopRunloop() is called.
             */
            void runloop();
            void stopRunloop();


        protected:

            /**
             * Runs any implementation specific start code.
             * @return
             */
            virtual bool startImpl() { return true; };

            /**
             * Runs any implementations specific stop code.
             */
            virtual void stopImpl() { };

            void enableLisaControllerReceivingFromSelf(bool enable);
            void enableLisaControllerSendingToSelf(bool enable);
            void claimLisaControllerLevelControl(bool claim);

        private:

            bool startLisaControllerProxy();
            void stopLisaControllerProxy();

    };

}

#endif //LISA_DESKBRIDGE_BRIDGE_H
