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

#include "Midi.h"

#include <iostream>

namespace LisaDeskbridge {

    void MidiReceiverDelegate::receivedMessage(const libremidi::message& message){

//        std::cout << "MESSAGE type(" << (int)message.get_message_type() << ") channel(" << message.get_channel() << ")" << std::endl;

        int i14 = 0;
        switch(message.get_message_type()){
            case libremidi::message_type::NOTE_ON:
                if (message.bytes[2] == 0){ // a note on message with velocity 0 is considered a note off
                    receivedNoteOff(message.get_channel(), message.bytes[1], message.bytes[2]);
                } else {
                    receivedNoteOn(message.get_channel(), message.bytes[1], message.bytes[2]);
                }
                break;
            case libremidi::message_type::NOTE_OFF:
                receivedNoteOff(message.get_channel(), message.bytes[1], message.bytes[2]);
                break;
            case libremidi::message_type::CONTROL_CHANGE:
                receivedControlChange(message.get_channel(), message.bytes[1], message.bytes[2]);
                break;
            case libremidi::message_type::POLY_PRESSURE:
                receivedPolyPressure(message.get_channel(), message.bytes[1], message.bytes[2]);
                break;
            case libremidi::message_type::PROGRAM_CHANGE:
                receivedProgramChange(message.get_channel(), message.bytes[1]);
                break;
            case libremidi::message_type::AFTERTOUCH:
                receivedAftertouch(message.get_channel(), message.bytes[1]);
                break;
            case libremidi::message_type::PITCH_BEND:
                // least significant bytes first...
                i14 = (((int)message.bytes[2]) << 7) + ((int)message.bytes[1]);
                receivedPitchBend(message.get_channel(), i14);
                break;
            default:
                // unprocessed message
                break;
        }
    }
}