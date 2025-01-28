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

#ifndef LISA_DESKBRIDGE_VIRTUALMIDIDEVICE_H
#define LISA_DESKBRIDGE_VIRTUALMIDIDEVICE_H

#include "Midi.h"

#include <libremidi/libremidi.hpp>

namespace LisaDeskbridge {

    class VirtualMidiDevice : MidiSender {

        public:

            static constexpr char kDefaultPortName[] = "L-ISA Deskbridge";

        protected:

            MidiReceiverDelegate * midiReceiverDelegate;

            libremidi::midi_in midiIn;
            libremidi::midi_out midiOut;

        public:

            VirtualMidiDevice(MidiReceiverDelegate &delegate);

            void start(std::basic_string_view<char> portName = kDefaultPortName);
            void stop();



    };

}


#endif //LISA_DESKBRIDGE_VIRTUALMIDIDEVICE_H
