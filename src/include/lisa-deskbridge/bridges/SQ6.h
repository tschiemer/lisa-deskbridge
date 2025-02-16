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

#ifndef LISA_DESKBRIDGE_SQ6_H
#define LISA_DESKBRIDGE_SQ6_H

#include "Bridge.h"

#include "VirtualMidiDevice.h"
#include "MidiClient.h"

namespace LisaDeskbridge {
    namespace Bridges {
        class SQ6 : public Bridge {

            public:

                static constexpr std::basic_string_view<char> kName = "SQ6";

                static constexpr std::basic_string_view<char> kSQ6MidiControlPortName = "MIDI Control 1";
                static constexpr float kDefaultRelativeStepSize = 0.0025;

                static constexpr char helpOpts[] = "\tSQ6 Options:\n"
                                                   "\t\t midiin    Name of MIDI In port to use (default: 'MIDI Control 1')\n"
                                                   "\t\t midiout    Name of MIDI out port to use (default: 'MIDI Control 1')\n";


            protected:

                enum ButtonState {ButtonState_Released, ButtonState_Pressed};

                class MixingStationDelegate : public LisaDeskbridge::MidiReceiver::Delegate {

                    protected:

                        SQ6 * sq6 = nullptr;

                    public:

                        MixingStationDelegate(SQ6 &sq){
                            sq6 = &sq;
                        }

                        void receivedNoteOn(int channel, int note, int velocity);
//                        void receivedNoteOff(int channel, int note, int velocity);
//                        void receivedControlChange(int channel, int cc, int value);
                };

                class SQMidiControlDelegate : public LisaDeskbridge::MidiReceiver::Delegate {

                    protected:

                        SQ6 * sq6 = nullptr;

                    public:

                        SQMidiControlDelegate(SQ6 &sq){
                            sq6 = &sq;
                        }

                        void receivedNoteOn(int channel, int note, int velocity);
                        void receivedNoteOff(int channel, int note, int velocity);
                        void receivedControlChange(int channel, int cc, int value);
                        void receivedProgramChange(int channel, int program);
                };


            protected:


                MixingStationDelegate mixingStationDelegate;
                LisaDeskbridge::VirtualMidiDevice mixingStationVirtualMidiDevice;

                std::basic_string_view<char> midiInPortName = kSQ6MidiControlPortName;
                std::basic_string_view<char> midiOutPortName = kSQ6MidiControlPortName;

                SQMidiControlDelegate sqMidiControlDelegate;
                MidiClient sqMidiControlClient;

                bool startMixingStationVirtualMidiDevice();
                void stopMixingStationVirtualMidiDevice();

                bool startSQMidiControlClient();
                void stopSQMidiControlClient();

            public: // Controller interface

                SQ6(BridgeOpts &opts);

                bool init();
                void deinit();

            protected:  // Controller logic

                bool followSelect = false;

                enum ButtonState softBtn1 = ButtonState_Released;
                enum ButtonState softBtn2 = ButtonState_Released;
                enum ButtonState softBtn3 = ButtonState_Released;
                enum ButtonState softBtn4 = ButtonState_Released;


        protected: // LisaDeskbridge::LisaControllerProxy::Delegate

//                void receivedMasterGain(float gain);
                void receivedMasterFaderPos(float pos);
//                void receivedReverbGain(float gain);
                void receivedReverbFaderPos(float pos);
        };
    }
}



#endif //LISA_DESKBRIDGE_SQ6_H
