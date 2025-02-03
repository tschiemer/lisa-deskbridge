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

#ifndef LISA_DESKBRIDGE_GENERIC_H
#define LISA_DESKBRIDGE_GENERIC_H

#include "Bridge.h"
#include "MidiReceiver.h"
#include "VirtualMidiDevice.h"
#include "MidiClient.h"

namespace LisaDeskbridge {
    namespace Bridges {

    class Generic : public LisaDeskbridge::Bridge, public LisaDeskbridge::MidiReceiver::Delegate {

        public:

            static constexpr std::basic_string_view<char> kName = "Generic";

            static constexpr float kDefaultRelativeStepSize = 0.0025;

            static constexpr char helpOpts[] = "\tGeneric Options:\n"
                                               "\t\t midiin    Name of MIDI In port to use\n"
                                               "\t\t midiout    Name of MIDI out port to use \n";

        protected:

            LisaDeskbridge::VirtualMidiDevice virtualMidiDevice;

            std::basic_string_view<char> midiInPortName = "";
            std::basic_string_view<char> midiOutPortName = "";

            MidiClient midiClient;


            bool startVirtualMidiDevice();
            void stopVirtualMidiDevice();

            bool startMidiClient();
            void stopMidiClient();

            void selectedChannelAction(int i);

        public: // LisaDeskbridge::Bridge

            Generic(BridgeOpts &opts);

            bool init();
            void deinit();

        public: // LisaDeskbridge::MidiReceiver::Delegate

            void receivedNoteOn(int channel, int note, int velocity);
            void receivedNoteOff(int channel, int note, int velocity);
            void receivedControlChange(int channel, int cc, int value);

        protected: // LisaDeskbridge::LisaControllerProxy::Delegate

            void receivedMasterFaderPos(float pos);
            void receivedReverbFaderPos(float pos);

        };
    }
}



#endif //LISA_DESKBRIDGE_GENERIC_H
