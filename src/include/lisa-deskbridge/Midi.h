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

#ifndef LISA_DESKBRIDGE_MIDI_H
#define LISA_DESKBRIDGE_MIDI_H

#include <libremidi/libremidi.hpp>

namespace LisaDeskbridge {

    class MidiReceiverDelegate {

            friend class MidiClient;
            friend class VirtualMidiDevice;

        protected:
            void receivedMessage(const libremidi::message& message) ;

        public:
            virtual void receivedNoteOn(int channel, int note, int velocity){}
            virtual void receivedNoteOff(int channel, int note, int velocity){}
            virtual void receivedControlChange(int channel, int cc, int value){}
            virtual void receivedPolyPressure(int channel, int note, int pressure){}
            virtual void receivedProgramChange(int channel, int program){}
            virtual void receivedAftertouch(int channel, int pressure){}
            virtual void receivedPitchBend(int channel, int bend){}
    };

    class MidiSender {
        public:
            virtual void sendNoteOn(int channel, int note, int velocity){};
            virtual void sendNoteOff(int channel, int note, int velocity){};
            virtual void sendControlChange(int channel, int cc, int value){};
            virtual void sendAftertouch(int channel, int note, int pressure){};
            virtual void sendProgramChange(int channel, int program){};
            virtual void sendChannelPressure(int channel, int pressure){};
            virtual void sendPitchBend(int channel, int bend){};
    };

}


#endif //LISA_DESKBRIDGE_MIDI_H
