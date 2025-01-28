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

namespace LisaDeskbridge {

     MidiClient::MidiClient(MidiReceiverDelegate &delegate) : midiIn({
                   .on_message= [&](const libremidi::message& message) {
                       midiReceiverDelegate->receivedMessage(message);
                   }
           }){
         midiReceiverDelegate = &delegate;
    }


    void MidiClient::start(std::basic_string_view<char> inPortName, std::basic_string_view<char> outPortName){

        mInPortName = inPortName;
        mOutPortName = outPortName;

        std::cout << "Scanning for IN port = '" << mInPortName << "'." << std::endl;
        std::cout << "Scanning for OUT port = '" << mOutPortName << "'." << std::endl;

        observer = new libremidi::observer ({
                .track_hardware = true,
                .track_virtual = true,
                .input_added = [&](const libremidi::input_port &port){
//                     std::cout << "Added IN port = " << port.port_name << std::endl;

                    if (port.port_name == mInPortName){
                        std::cout << "Found MIDI IN port '" << mInPortName << "'. Opening.." << std:: endl;
                        if (midiIn.is_port_open()){
                            midiIn.close_port();
                        }
                        stdx::error e = midiIn.open_port(port, mInPortName);
                        if (e.is_set()){
                            std::cerr << "Error opening midi IN port: " << e.message().data() << std::endl;
                        }
                    }
                },
                .input_removed = [&](const libremidi::input_port &port){
                    // Well, on macOS port_name does not contain any info, so this callback here is meaningless
//                    std::cerr << "Removed IN port = " << port.port_name << std::endl;

                    if (port.port_name == mInPortName){
                        midiIn.close_port();
                        std::cerr << "Lost MIDI IN port '" << mInPortName << "'. Waiting for reconnection..." << std:: endl;
                    }
                },
                .output_added = [&](const libremidi::output_port &port){
//                    std::cerr << "Added OUT port = " << port.port_name << std::endl;

                    if (port.port_name == mOutPortName){
                        std::cout << "Found MIDI OUT port '" << mOutPortName << "'. Opening.." << std:: endl;
                        if (midiOut.is_port_open()){
                            midiOut.close_port();
                        }
                        stdx::error e = midiOut.open_port(port, mOutPortName);
                        if (e.is_set()){
                            std::cerr << "Error opening midi OUT port: " << e.message().data() << std::endl;
                        }
                    }
                },
                .output_removed = [&](const libremidi::output_port &port){
                    // Well, on macOS port_name does not contain any info, so this callback here is meaningless
//                    std::cerr << "Removed OUT port = " << port.port_name << std::endl;

                    if (port.port_name == mOutPortName){
                        midiOut.close_port();
                        std::cout << "Lost MIDI OUT port '" << mOutPortName << "'. Waiting for reconnection..." << std:: endl;
                    }
                }
        });

    }

    void MidiClient::stop(){
        midiIn.close_port();
        midiOut.close_port();

        delete observer;
    }

    void MidiClient::sendNoteOn(int channel, int note, int velocity) {
        assert(0 <= channel && channel <= 15);
        assert(0 <= note && note <= 127);
        assert(0 <= velocity && velocity <= 127);

        if (!midiOut.is_port_open()){
            return;
        }

        midiOut.send_message( ((int)libremidi::message_type::NOTE_ON | channel), note, velocity);
    }

    void MidiClient::sendNoteOff(int channel, int note, int velocity) {
        assert(0 <= channel && channel <= 15);
        assert(0 <= note && note <= 127);
        assert(0 <= velocity && velocity <= 127);

        if (!midiOut.is_port_open()){
            return;
        }

        midiOut.send_message( ((int)libremidi::message_type::NOTE_OFF | channel), note, velocity);
    }

    void MidiClient::sendControlChange(int channel, int cc, int value){
        assert(0 <= channel && channel <= 15);
        assert(0 <= cc && cc <= 127);
        assert(0 <= value && value <= 127);

        if (!midiOut.is_port_open()){
            return;
        }

        midiOut.send_message( ((int)libremidi::message_type::CONTROL_CHANGE | channel), cc, value);
    }
    void MidiClient::sendAftertouch(int channel, int note, int pressure) {
        assert(0 <= channel && channel <= 15);
        assert(0 <= note && note <= 127);
        assert(0 <= pressure && pressure <= 127);

        if (!midiOut.is_port_open()){
            return;
        }

        midiOut.send_message( ((int)libremidi::message_type::POLY_PRESSURE | channel), note, pressure);
    }

    void MidiClient::sendProgramChange(int channel, int program) {
        assert(0 <= channel && channel <= 15);
        assert(0 <= program && program <= 127);

        if (!midiOut.is_port_open()){
            return;
        }

        midiOut.send_message( ((int)libremidi::message_type::PROGRAM_CHANGE | channel), program);
    }

    void MidiClient::sendChannelPressure(int channel, int pressure) {
        assert(0 <= channel && channel <= 15);
        assert(0 <= pressure && pressure <= 127);

        if (!midiOut.is_port_open()){
            return;
        }
        midiOut.send_message( ((int)libremidi::message_type::AFTERTOUCH | channel), pressure);
    }

    void MidiClient::sendPitchBend(int channel, int bend) {
        assert(0 <= channel && channel <= 15);
        assert(0 <= bend && bend <= 16384);

        if (!midiOut.is_port_open()){
            return;
        }

        // least significant bytes first...
        int b1 = bend & 0b01111111;
        int b2 = (bend >> 7) & 0b01111111;

        midiOut.send_message( ((int)libremidi::message_type::POLY_PRESSURE | channel), b1, b2);
    }

}