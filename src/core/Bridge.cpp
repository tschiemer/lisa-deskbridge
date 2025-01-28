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

#include "bridges/SQ6.h"

#if defined(__APPLE__)
#include <CoreFoundation/CoreFoundation.h>
#endif

namespace LisaDeskbridge {


#if defined(__APPLE__)
    static CFRunLoopRef runLoopRef = nullptr;

    static void signal_handler(int i){
        CFRunLoopStop(runLoopRef);
    }
#endif

    Bridge::Bridge(BridgeOpts &opts) : lisaControllerProxy(this){

    }

    Bridge *Bridge::factory(std::basic_string_view<char> &name, BridgeOpts &opts) {

        Bridge * bridge = nullptr;
        if (name.compare(Bridges::SQ6::kName) == 0){
            bridge = new Bridges::SQ6(opts);
        }
        return bridge;
    }

    bool Bridge::init(){

        if (startLisaControllerProxy() == false){
            return false;
        }

        return true;
    }

    void Bridge::runloop(){

        std::cerr << "Starting run loop.." << std::endl;

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
#endif

    }

    void Bridge::stop(){
#if defined(__APPLE__)
        CFRunLoopStop(runLoopRef);
#else
        std::raise(SIGTERM);
#endif
    }

    void Bridge::deinit(){

        stopLisaControllerProxy();
    }

    bool Bridge::startLisaControllerProxy(){
        std::cerr << "Starting L-ISA Controller Proxy.." << std::endl;
        try {
            lisaControllerProxy.start(localPort, lisaControllerHost, lisaControllerPort);
        } catch (const std::exception& e){
            std::cerr << e.what() << std::endl;
            return false;
        }

        return true;
    }

    void Bridge::stopLisaControllerProxy(){
        std::cerr << "Stopping L-ISA Controller Proxy.." << std::endl;
        lisaControllerProxy.stop();
    }

}
