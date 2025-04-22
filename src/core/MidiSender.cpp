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

#include "MidiSender.h"

#include "log.h"
#include <iostream>

namespace LisaDeskbridge {

    void MidiSender_Single_Impl::sendNoteOn(int channel, int note, int velocity) {
        assert(0 <= channel && channel <= 15);
        assert(0 <= note && note <= 127);
        assert(0 <= velocity && velocity <= 127);

        if (!midiOut.is_port_open()){
            return;
        }

        log(LogLevelDebug,"TX midi note on ch(%d) note(%d) vel(%d)", channel, note, velocity);

        midiOut.send_message( ((int)libremidi::message_type::NOTE_ON | channel), note, velocity);
    }

    void MidiSender_Single_Impl::sendNoteOff(int channel, int note, int velocity) {
        assert(0 <= channel && channel <= 15);
        assert(0 <= note && note <= 127);
        assert(0 <= velocity && velocity <= 127);

        if (!midiOut.is_port_open()){
            return;
        }

        log(LogLevelDebug,"TX midi note off ch(%d) note(%d) vel(%d)", channel, note, velocity);

        midiOut.send_message( ((int)libremidi::message_type::NOTE_OFF | channel), note, velocity);
    }

    void MidiSender_Single_Impl::sendControlChange(int channel, int cc, int value){
        assert(0 <= channel && channel <= 15);
        assert(0 <= cc && cc <= 127);
        assert(0 <= value && value <= 127);

        if (!midiOut.is_port_open()){
            return;
        }

        log(LogLevelDebug,"TX midi cc ch(%d) cc(%d) val(%d)", channel, cc, value);

        midiOut.send_message( ((int)libremidi::message_type::CONTROL_CHANGE | channel), cc, value);
    }
    void MidiSender_Single_Impl::sendAftertouch(int channel, int note, int pressure) {
        assert(0 <= channel && channel <= 15);
        assert(0 <= note && note <= 127);
        assert(0 <= pressure && pressure <= 127);

        if (!midiOut.is_port_open()){
            return;
        }

        midiOut.send_message( ((int)libremidi::message_type::POLY_PRESSURE | channel), note, pressure);
    }

    void MidiSender_Single_Impl::sendProgramChange(int channel, int program) {
        assert(0 <= channel && channel <= 15);
        assert(0 <= program && program <= 127);

        if (!midiOut.is_port_open()){
            return;
        }

        midiOut.send_message( ((int)libremidi::message_type::PROGRAM_CHANGE | channel), program);
    }

    void MidiSender_Single_Impl::sendChannelPressure(int channel, int pressure) {
        assert(0 <= channel && channel <= 15);
        assert(0 <= pressure && pressure <= 127);

        if (!midiOut.is_port_open()){
            return;
        }
        midiOut.send_message( ((int)libremidi::message_type::AFTERTOUCH | channel), pressure);
    }

    void MidiSender_Single_Impl::sendPitchBend(int channel, int bend) {
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