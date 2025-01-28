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

#ifndef LISA_DESKBRIDGE_MIDICLIENT_H
#define LISA_DESKBRIDGE_MIDICLIENT_H

#include "Midi.h"

#include <libremidi/libremidi.hpp>
#include <iostream>

namespace LisaDeskbridge {

    class MidiClient : public MidiSender {

        protected:
            std::basic_string_view<char> mInPortName;
            std::basic_string_view<char> mOutPortName;

            MidiReceiverDelegate * midiReceiverDelegate;

            libremidi::observer * observer = nullptr;

            libremidi::midi_in midiIn;
            libremidi::midi_out midiOut;

        public:

            MidiClient(MidiReceiverDelegate &delegate) ;
//            ~MidiClient()

//            static void listPorts();

            void start(std::basic_string_view<char> inPortName, std::basic_string_view<char> outPortName);
            void stop();

            void sendNoteOn(int channel, int note, int velocity);
            void sendNoteOff(int channel, int note, int velocity);
            void sendControlChange(int channel, int cc, int value);
            void sendAftertouch(int channel, int note, int pressure);
            void sendProgramChange(int channel, int program);
            void sendChannelPressure(int channel, int pressure);
            void sendPitchBend(int channel, int bend);
    };

}

#endif //LISA_DESKBRIDGE_MIDICLIENT_H
