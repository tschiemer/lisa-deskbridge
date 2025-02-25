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

#ifndef LISA_DESKBRIDGE_MIDISENDER_H
#define LISA_DESKBRIDGE_MIDISENDER_H

#include <libremidi/libremidi.hpp>

namespace LisaDeskbridge {

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

    class MidiSender_Single_Impl : public MidiSender {

        protected:

            libremidi::midi_out midiOut;

        public:

            void sendNoteOn(int channel, int note, int velocity);
            void sendNoteOff(int channel, int note, int velocity);
            void sendControlChange(int channel, int cc, int value);
            void sendAftertouch(int channel, int note, int pressure);
            void sendProgramChange(int channel, int program);
            void sendChannelPressure(int channel, int pressure);
            void sendPitchBend(int channel, int bend);

    };

}


#endif //LISA_DESKBRIDGE_MIDISENDER_H
