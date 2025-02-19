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

#include "MidiClient.h"

#include <iostream>
#include "log.h"

namespace LisaDeskbridge {

     MidiClient::MidiClient(MidiReceiver::Delegate &delegate) :
             MidiReceiver_Single_Impl(delegate){
         // do nothing
    }


    void MidiClient::start(std::basic_string_view<char> inPortName, std::basic_string_view<char> outPortName){

        mInPortName = inPortName;
        mOutPortName = outPortName;

        log(LogLevelInfo, "Scanning for IN port = '%s'", mInPortName);
        log(LogLevelInfo, "Scanning for OUT port = '%s'", mOutPortName);

        observer = new libremidi::observer ({
                .track_hardware = true,
                .track_virtual = true,
                .input_added = [&](const libremidi::input_port &port){
//                     std::cout << "Added IN port = " << port.port_name << std::endl;

                    if (mInPortName.length() > 0 && port.port_name == mInPortName){
                        log(LogLevelInfo, "Found MIDI IN port '%s'. Opening..", mInPortName );
                        if (midiIn.is_port_open()){
                            midiIn.close_port();
                        }
                        stdx::error e = midiIn.open_port(port, mInPortName);
                        if (e.is_set()){
                            log(LogLevelError,"opening midi IN port: %s", e.message().data() );
                        }
                    }
                },
                .input_removed = [&](const libremidi::input_port &port){
                    // Well, on macOS port_name does not contain any info, so this callback here is meaningless
//                    std::cerr << "Removed IN port = " << port.port_name << std::endl;

                    if (mInPortName.length() > 0 && port.port_name == mInPortName){
                        midiIn.close_port();
                        log(LogLevelInfo, "Lost MIDI IN port '%s'. Waiting for reconnection..", mInPortName );
                    }
                },
                .output_added = [&](const libremidi::output_port &port){
//                    std::cerr << "Added OUT port = " << port.port_name << std::endl;

                    if (mOutPortName.length() > 0 && port.port_name == mOutPortName){
                        log(LogLevelInfo, "Found MIDI OUT port '%s'. Opening..", mOutPortName );
                        if (midiOut.is_port_open()){
                            midiOut.close_port();
                        }
                        stdx::error e = midiOut.open_port(port, mOutPortName);
                        if (e.is_set()){
                            log(LogLevelError,"opening midi OUT port: %s", e.message().data() );
                        }
                    }
                },
                .output_removed = [&](const libremidi::output_port &port){
                    // Well, on macOS port_name does not contain any info, so this callback here is meaningless
//                    std::cerr << "Removed OUT port = " << port.port_name << std::endl;

                    if (mOutPortName.length() > 0 && port.port_name == mOutPortName){
                        midiOut.close_port();
                        log(LogLevelInfo, "Lost MIDI OUT port '%s'. Waiting for reconnection..", mOutPortName );
                    }
                }
        });

    }

    void MidiClient::stop(){
        delete observer;

        midiIn.close_port();
        midiOut.close_port();
    }

}