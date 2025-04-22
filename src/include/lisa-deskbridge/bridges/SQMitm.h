/**
* lisa-deskbridge
* Copyright (C) 2025  Philip Tschiemer
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

#ifndef LISA_DESKBRIDGE_SQMITM_H
#define LISA_DESKBRIDGE_SQMITM_H


#include "../Bridge.h"
#include "sqmixmitm/DiscoveryResponder.h"
#include "sqmixmitm/MixMitm.h"
#include "../MidiClient.h"

namespace LisaDeskbridge {
    namespace Bridges {

        class SQMitm : public Bridge, public LisaDeskbridge::MidiReceiver::Delegate {

        public:

            static constexpr char kName[] = "SQ-Mitm";

            static constexpr float kDefaultRelativeStepSize = 0.0025;

            static constexpr char helpOpts[] = "\tSQ-Mitm Options:\n"
                                               "\t\t mixer-ip=<mixer-ip>               IP of mixer (REQUIRED)\n"
                                               "\t\t mitm-name=<name-of-mitm-service>  Name visible to mixing apps (default: L-ISA Deskbridge)\n"
                                               "\t\t midiin    Name of MIDI In port to use\n"
                                               "\t\t midiout    Name of MIDI out port to use \n";

        protected:

            enum ButtonState {Released, Pressed};

        protected:

            std::basic_string_view<char> mixerIp_     = "";
            std::basic_string_view<char> mitmName_    = "L-ISA Deskbridge";

            SQMixMitm::DiscoveryResponder discoveryResponder_;
            SQMixMitm::MixMitm mitm_;


            std::string midiInPortName_ = "";
            std::string midiOutPortName_ = "";

            MidiClient midiClient_;

        protected:  // Controller logic

            bool followSelect_ = true;

            enum ButtonState softBtn1_ = Released;
            enum ButtonState softBtn2_ = Released;
            enum ButtonState softBtn3_ = Released;
            enum ButtonState softBtn4_ = Released;


        public: // Controller interface

            SQMitm(BridgeOpts &opts);

            void setFollowSelect(bool enabled);

        protected:
            void initMitm();

            bool startMidiClient();
            void stopMidiClient();

            bool startImpl();
            void stopImpl();

        public:

            void onSelectedChannel(int channel);

            void onMidiEvent(int channel, int type, int value1, int value2);

            void onMidiNoteOn(int channel, int note, int velocity);
            void onMidiNoteOff(int channel, int note, int velocity);
            void onMidiControlChange(int channel, int cc, int value);
            void onMidiProgramChange(int channel, int program);

            void onMidiFaderLevel(int channel, int value);
            void onMidiFaderMute(int channel);

        public: // LisaDeskbridge::MidiReceiver::Delegate

            void receivedNoteOn(int channel, int note, int velocity){
                onMidiNoteOn(channel, note, velocity);
            }

            void receivedNoteOff(int channel, int note, int velocity){
                onMidiNoteOff(channel, note, velocity);
            }

            void receivedControlChange(int channel, int cc, int value){
                onMidiControlChange(channel, cc, value);
            }

            void receivedPitchBend(int channel, int bend);


        protected: // LisaDeskbridge::LisaControllerProxy::Delegate

//                void receivedMasterGain(float gain);
            void receivedMasterFaderPos(float pos);
//                void receivedReverbGain(float gain);
            void receivedReverbFaderPos(float pos);

        };

    }
}


#endif //LISA_DESKBRIDGE_SQMITM_H
