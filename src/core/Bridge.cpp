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
#include "bridges/SQ6.h"
#include "bridges/SQmitm.h"

#include "log.h"

#if defined(__APPLE__)
#include <CoreFoundation/CoreFoundation.h>
#endif

namespace LisaDeskbridge {


#if defined(__APPLE__)
    static CFRunLoopRef runLoopRef = nullptr;

    // singleton of bridge
    //TODO does it make sense to have a singleton only? likely yes.
    static Bridge * bridgeSingleton = nullptr;

    static void signal_handler(int i){
        assert(bridgeSingleton != nullptr);

        bridgeSingleton->stop();
    }
#endif

    Bridge::Bridge(BridgeOpts &opts) : lisaControllerProxy(this){

    }

    Bridge *Bridge::factory(std::basic_string_view<char> &name, BridgeOpts &opts) {

        Bridge * bridge = nullptr;
        try {
            if (name.compare(Bridges::Generic::kName) == 0){
                bridge = new Bridges::Generic(opts);
            }
            else if (name.compare(Bridges::SQ6::kName) == 0){
                bridge = new Bridges::SQ6(opts);
            }
            else if (name.compare(Bridges::SQmitm::kName) == 0){
                bridge = new Bridges::SQmitm(opts);
            }

        } catch (std::exception &e){
            log(LogLevelDebug, "Exception when creating bridge: %s", e.what());
        }
        return bridge;
    }

    bool Bridge::init(){

#if defined(__APPLE__)
      assert(bridgeSingleton == nullptr);
        bridgeSingleton = this;
#endif

        if (startLisaControllerProxy() == false){
            return false;
        }

        return true;
    }

    void Bridge::runloop(){

        log(LogLevelInfo, "Starting run loop..");

#if defined(__APPLE__)
// On macOS, observation can *only* be done in the main thread
// with an active CFRunLoop.

        runLoopRef = CFRunLoopGetCurrent();

        std::signal(SIGHUP, signal_handler);
        std::signal(SIGINT, signal_handler);
        std::signal(SIGTERM, signal_handler);

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

    void Bridge::stop(){

        log(LogLevelInfo, "Stopping!");

#if defined(__APPLE__)
        CFRunLoopStop(runLoopRef);
#else
        std::raise(SIGTERM);
#endif
    }

    void Bridge::deinit(){

        stopLisaControllerProxy();

#if defined(__APPLE__)
      bridgeSingleton = nullptr;
#endif
    }

    bool Bridge::startLisaControllerProxy(){
        log(LogLevelInfo,  "Starting L-ISA Controller Proxy.." );

        try {
            lisaControllerProxy.start(localPort, lisaControllerHost, lisaControllerPort);
        } catch (const std::exception& e){
            std::cout << e.what() << std::endl;
            return false;
        }

        return true;
    }

    void Bridge::stopLisaControllerProxy(){
        log(LogLevelInfo,  "Stopping L-ISA Controller Proxy.." );

        lisaControllerProxy.stop();
    }

}
