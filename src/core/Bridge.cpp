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

#include "Bridge.h"

#include <iostream>
#include <csignal>

#include "bridges/Generic.h"
#include "bridges/SQMidi.h"
#include "bridges/SQMitm.h"

#include "log.h"

#if defined(__APPLE__)
#include <CoreFoundation/CoreFoundation.h>
#include <arpa/inet.h>

#endif

namespace LisaDeskbridge {


#if defined(__APPLE__)
    static CFRunLoopRef runLoopRef = nullptr;

    // singleton of bridge
    //TODO does it make sense to have a singleton only? likely yes.
    static Bridge * bridgeSingleton = nullptr;

    static void stop_runloop_signal_handler(int i){
        assert(bridgeSingleton != nullptr);

        CFRunLoopStop(runLoopRef);
    }
#endif

    Bridge::Bridge(BridgeOpts &opts) : lisaControllerProxy_(this){
//        if (bridgeSingleton != nullptr){
//            throw std::logic_error("Bridge is a singleton ");
//        }
//
//        bridgeSingleton = this;
    }

    Bridge::~Bridge(){
//        bridgeSingleton = nullptr;
    }


    Bridge *Bridge::factory(std::string &name, BridgeOpts &opts) {

        Bridge * bridge = nullptr;
        try {
            if (name.compare(Bridges::Generic::kName) == 0){
                bridge = new Bridges::Generic(opts);
            }
            else if (name.compare(Bridges::SQMidi::kName) == 0){
                bridge = new Bridges::SQMidi(opts);
            }
            else if (name.compare(Bridges::SQMitm::kName) == 0){
                bridge = new Bridges::SQMitm(opts);
            }

            if (opts.contains(kOptLisaControllerIp)){
                struct sockaddr_in addr;
                if (inet_aton(opts[kOptLisaControllerIp].data(), (struct in_addr*)&(addr.sin_addr.s_addr)) != 1){
                    throw std::invalid_argument("invalid lisa controller ip");
                }
                bridge->lisaControllerIp_ = opts[kOptLisaControllerIp];
            }
            if (opts.contains(kOptLisaControllerPort)){
                int i = atoi(opts[kOptLisaControllerPort].data());
                if (i < 1 || 0xffff < i){
                    throw std::invalid_argument("invalid lisa controller port");
                }
                bridge->lisaControllerPort_ = i;
            }
            if (opts.contains(kOptDeviceIP)){
                struct sockaddr_in addr;
                if (inet_aton(opts[kOptDeviceIP].data(), (struct in_addr*)&(addr.sin_addr.s_addr)) != 1){
                    throw std::invalid_argument("invalid lisa controller ip");
                }
                bridge->deviceIp_ = opts[kOptDeviceIP];
            }
            if (opts.contains(kOptDevicePort)){
                int i = atoi(opts[kOptDevicePort].data());
                if (i < 1 || 0xffff < i){
                    throw std::invalid_argument("invalid device port");
                }
                bridge->devicePort_ = i;
            }
            if (opts.contains(kOptDeviceID)){
                int i = atoi(opts[kOptDeviceID].data());
                if (i < 1 || 10 < i){
                    throw std::invalid_argument("device-id must be between 1 - 10");
                }
                bridge->deviceId_ = i;
            }
            if (opts.contains(kOptDeviceName) && opts[kOptDeviceName].length() > 0){
                bridge->deviceName_ = opts[kOptDeviceName];
            }
//            if (opts.contains("register")){
//                bridge->register_ = atoi(opts["register"].data()) == 1;
//            }
            if (opts.contains(kOptClaimLevelControl)){
                bridge->claimLevelControl_ = atoi(opts[kOptClaimLevelControl].data()) == 1;
            }

        } catch (std::exception &e){
            log(LogLevelDebug, "Exception when creating bridge: %s", e.what());
        }
        return bridge;
    }

    bool Bridge::start(){

        if (state == State_Started){
            return true;
        }

        state = State_Starting;

        log(LogLevelInfo, "Starting bridge..");

        if (startLisaControllerProxy() == false){
            state = State_Stopped;
            return false;
        }

        if (startImpl() == false){
            stopLisaControllerProxy();
            state = State_Stopped;
            return false;
        }

        lisaControllerProxy_.registerDevice(deviceId_, deviceIp_.data(), devicePort_);
        lisaControllerProxy_.setDeviceName(deviceId_, deviceName_.data());

        if (claimLevelControl_){
            claimLisaControllerLevelControl(true);
        }

        state = State_Started;

        return true;
    }

    void Bridge::runloop(){

//        if (bridgeSingleton != nullptr){
//            throw std::logic_error("Bridge is a singleton ");
//        }

        log(LogLevelInfo, "Running loops..");

#if defined(__APPLE__)
// On macOS, observation can *only* be done in the main thread
// with an active CFRunLoop.

        runLoopRef = CFRunLoopGetCurrent();

        std::signal(SIGHUP, stop_runloop_signal_handler);
        std::signal(SIGINT, stop_runloop_signal_handler);
        std::signal(SIGTERM, stop_runloop_signal_handler);

        CFRunLoopRun();
#else
        sigset_t wset;
        sigemptyset(&wset);
        sigaddset(&wset,SIGHUP);
        sigaddset(&wset,SIGINT);
        sigaddset(&wset,SIGTERM);
        int sig;
        sigwait(&wset,&sig);

        log(LogLevelInfo, "Stopping..");
#endif

    }

    void Bridge::stopRunloop(){

        log(LogLevelInfo, "Stopping runloop!");

#if defined(__APPLE__)
        CFRunLoopStop(runLoopRef);
#else
        std::raise(SIGTERM);
#endif

    }


    void Bridge::stop(){

        if (state != State_Started){
            return;
        }

        state = State_Stopping;

        stopImpl();

        stopLisaControllerProxy();

        state = State_Stopped;
    }

    bool Bridge::startLisaControllerProxy(){
        log(LogLevelInfo,  "Starting L-ISA Controller Proxy.." );

        try {
            lisaControllerProxy_.start(devicePort_, lisaControllerIp_, lisaControllerPort_);
        } catch (const std::exception& e){
            std::cout << e.what() << std::endl;
            return false;
        }

        return true;
    }

    void Bridge::stopLisaControllerProxy(){
        log(LogLevelInfo,  "Stopping L-ISA Controller Proxy.." );

        enableLisaControllerReceivingFromSelf(false);
        enableLisaControllerSendingToSelf(false);

        lisaControllerProxy_.stop();
    }


    void Bridge::enableLisaControllerReceivingFromSelf(bool enable){
        lisaControllerProxy_.enableReceivingFromDevice(deviceId_, enable);
    }

    void Bridge::enableLisaControllerSendingToSelf(bool enable){
        lisaControllerProxy_.enableSendingToDevice(deviceId_, enable);
    }

    void Bridge::claimLisaControllerLevelControl(bool claim){
        lisaControllerProxy_.setMasterGainControl(deviceId_,claim);
    }

}
