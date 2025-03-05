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

#include "bridges/SQMitm.h"

#include "log.h"
//#include "sqmixmitm/log.h"

#include "libremidi/message.hpp"

namespace LisaDeskbridge {
    namespace Bridges {


        SQMitm::SQMitm(BridgeOpts &opts) :
                Bridge(opts){
            if (opts.contains("mixer-ip")){
                mixerIp_ = opts["mixer-ip"];
                //TODO validate
//                printf("mixer ip = %s\n", mixerIp_.data());
            }
            else {
                throw std::invalid_argument("Missing option 'mixer-ip'");
            }

            if (opts.contains("mitm-name")){
                mitmName_ = opts["mitm-name"];
            }
        }


        void SQMitm::initMitm() {

            mitm_.onConnectionStateChanged([&](SQMixMitm::MixMitm::ConnectionState state, SQMixMitm::Version &version){
                if (state == SQMixMitm::MixMitm::Connected) {

                    log(LogLevelInfo,"Connected to mixer (firmware %d.%d.%d r%d)\n",
                        version.major(), version.minor(), version.patch(), version.build());
                } else {

                }
            });

            mitm_.onEvent(SQMixMitm::Event::Type::ChannelSelect, [&](SQMixMitm::Event &event){

                log(LogLevelDebug, "event ChannelSelect physical %d virtual %d state %d", event.ChannelSelect_physical_strip(), event.ChannelSelect_channel(), event.ChannelSelect_onoff());

                if (mitm_.connectionState() != SQMixMitm::MixMitm::Connected || mitm_.state() != SQMixMitm::MixMitm::Running){
                    return;
                }

                // only channel select events with an ON state have an actually meaningful channel/source
                if (!event.ChannelSelect_onoff()){
                    return;
                }

                onSelectedChannel(event.ChannelSelect_channel());
            });

            mitm_.onEvent(SQMixMitm::Event::Type::MidiSoftRotary, [&](SQMixMitm::Event &event){

                log(LogLevelDebug, "event MidiSoftRotary", event.ChannelSelect_channel());

                if (mitm_.connectionState() != SQMixMitm::MixMitm::Connected || mitm_.state() != SQMixMitm::MixMitm::Running){
                    return;
                }

                onMidiEvent(event.MidiSoftKey_channel(), event.MidiSoftRotary_type(),
                            event.MidiSoftRotary_value1(), event.MidiSoftRotary_value2());
            });

            mitm_.onEvent(SQMixMitm::Event::Type::MidiSoftKey, [&](SQMixMitm::Event &event){

                log(LogLevelDebug, "event MidiSoftKey", event.ChannelSelect_channel());

                if (mitm_.connectionState() != SQMixMitm::MixMitm::Connected || mitm_.state() != SQMixMitm::MixMitm::Running){
                    return;
                }

                onMidiEvent(event.MidiSoftKey_channel(), event.MidiSoftKey_type(), event.MidiSoftKey_value1(),
                            event.MidiSoftKey_value2());
            });

            mitm_.onEvent(SQMixMitm::Event::Type::MidiFaderLevel, [&](SQMixMitm::Event &event){

                log(LogLevelDebug, "event MidiFaderLevel ch %d %d", event.MidiFaderLevel_channel(), event.MidiFaderLevel_value());

                if (mitm_.connectionState() != SQMixMitm::MixMitm::Connected || mitm_.state() != SQMixMitm::MixMitm::Running){
                    return;
                }

                onMidiFaderLevel(event.MidiFaderLevel_channel(), event.MidiFaderLevel_value());
            });

//                mitm_.onEvent(SQMixMitm::Event::Types::MidiFaderMute, [&](SQMixMitm::Event &event){
//
//                    log(LogLevelDebug, "event MidiFaderMute ch %d %d %d %d", event.MidiFaderMute_channel(), event.databyte1(), event.databyte2(), event.databyte3());
//
//                    if (mitm_.connectionState() != SQMixMitm::MixMitm::Connected || mitm_.state() != SQMixMitm::MixMitm::Running){
//                        return;
//                    }
//
//                    onMidiFaderLevel(event.MidiFaderMute_channel(), event.MidiFaderLevel_value());
//                });

        }

        bool SQMitm::startImpl(){

            initMitm();

            log(LogLevelInfo, "Starting SQ MITM Service for mixer at %s", mixerIp_.data());

            if (mitm_.start((char*)mixerIp_.data())){
                return false;
            }

            // set MITM service name as it will appear for app
            discoveryResponder_.name((char*)mitmName_.data());

            log(LogLevelInfo, "Starting SQ Discovery Responder using name %s", mitmName_.data());
            if (discoveryResponder_.start()){
                mitm_.stop();
                return false;
            }


            enableLisaControllerSendingToSelf(true);
            enableLisaControllerReceivingFromSelf(true);

            return true;
        }


    void SQMitm::stopImpl() {

        log(LogLevelInfo, "Stopping SQ Discovery Responder..");
        discoveryResponder_.stop();

        log(LogLevelInfo, "Stopping SQ MITM Service..");
        mitm_.stop();
    }

        void SQMitm::onSelectedChannel(int channel){

//            | Console Channel | ID (as per event) |
//            |-----------------|-------------------|
//            | 1-40            | 0 - 39            | 1-40
//            | ST1             | 40                | 41
//            | ST2             | 42                | 43
//            | ST3             | 44                | 45
//            | USB             | 46                | 47
//            | FX Ret 1-8      | 64-71             | -> 49 - 56
//            | Group 1-12      | 72 - 83           | -> 57 - 68
//            | Aux 1-12        | 88 - 99           | -> 69 - 80
//            | FX Send 1-4     | 107-110           | -> 81 -84
//            | MainLR          | 104               | -> 85
//            | Matrix 1-3      | 115-117           | -> 87 -

            if (0 <= channel && channel <= 47){ // inputs (incl ST1-3+USB)
                channel += 1;
            } else if (64 <= channel && channel <= 71) { // FX Ret 1-8
                channel -= 15;
            } else if (72 <= channel && channel <= 83) { // Groups 1-12
                channel -= 15;
            } else if (88 <= channel && channel <= 99) { // Aux 1-12
                channel -= 19;
            } else if (107 <= channel && channel <= 110) { // FX Send 1-4
                channel -= 26;
            } else if (channel == 104) { // Main LR
                channel = 85;
            } else if (115 <= channel && channel <= 120) { // Matrix 1-6
                channel -= 28;
            } else {
                return;
            }

            if (!isValidSourceId(channel)){
                return;
            }

            if ((softBtn1_ == Pressed || followSelect_) &&
                softBtn2_ == Released &&
                softBtn3_ == Released &&
                softBtn4_ == Released){
                    lisaControllerProxy_.selectSource(channel);
            }
            else if (softBtn1_ == Released &&
                     softBtn2_ == Pressed &&
                     softBtn3_ == Released){
                if (softBtn4_ == Released){
                    lisaControllerProxy_.addSourceToSelection(channel);
                } else {
                    lisaControllerProxy_.removeSourceFromSelection(channel);
                }
            }
            else if (softBtn1_ == Released &&
                     softBtn2_ == Released &&
                     softBtn3_ == Pressed &&
                     softBtn4_ == Released){
                lisaControllerProxy_.snapSourceToSpeaker(channel);
            }
            else if (softBtn1_ == Released &&
                     softBtn3_ == Pressed &&
                     softBtn4_ == Pressed){
                if (softBtn2_ == Released){
                    lisaControllerProxy_.setSourceSolo(channel, true);
                } else {
                    lisaControllerProxy_.setSourceSolo(channel, false);
                }
            }
        }

        void SQMitm::onMidiEvent(int channel, int type, int value1, int value2){

            // let's use channel range 1-16
            channel += 1;

            if (type == (int)libremidi::message_type::NOTE_ON){
                onMidiNoteOn(channel, value1, value2);
            }
            else if (type == (int)libremidi::message_type::NOTE_OFF){
                onMidiNoteOff(channel, value1, value2);
            }
            else if (type == (int)libremidi::message_type::CONTROL_CHANGE){
                onMidiControlChange(channel, value1, value2);
            }
            else if (type == (int)libremidi::message_type::PROGRAM_CHANGE){
                onMidiProgramChange(channel, value1);
            }
        }

        void SQMitm::onMidiNoteOn(int channel, int note, int velocity){

            log(LogLevelDebug, "midi Note On ch(%d) note(%d) velocity(%d)", channel, note, velocity);

            if (channel == 1){
                if (!isValidDeviceId(note)){
                    return;
                }

                if (note == 1){
                    softBtn1_ = Pressed;

                    // if ALT button is pressed toggle the auto-follow
                    if (softBtn4_ == Pressed){
                        followSelect_ = !followSelect_;
                    }
                }
                else if (note == 2){
                    softBtn2_ = Pressed;
//                selectionMode = SelectionMode_Add;
                }
                else if (note == 3){
                    softBtn3_ = Pressed;
                }
                else if (note == 4){
                    softBtn4_ = Pressed;
                }
            }

            else if (channel == 2){ // select group
                if (!isValidGroupId(note)){
                    return;
                }

                lisaControllerProxy_.selectGroup(note);
            }
            else if (channel == 3){ // Fire snapshot
                if (!isValidSnapshotId(note)){
                    return;
                }

                lisaControllerProxy_.fireSnapshot(note);
            }
            else if (channel == 4){
            } // channel == 3
        }

        void SQMitm::onMidiNoteOff(int channel, int note, int velocity){

            log(LogLevelDebug, "midi Note Off ch(%d) note(%d) velocity(%d)", channel, note, velocity);

            if (channel == 1 && note == 1){
                softBtn1_ = Released;
            }
            else if (channel == 1 && note == 2){
                softBtn2_ = Released;
            }
            else if (channel == 1 && note == 3){
                softBtn3_ = Released;
            }
            else if (channel == 1 && note == 4){
                softBtn4_ = Released;
            }
        }

        void SQMitm::onMidiControlChange(int channel, int cc, int value){

            log(LogLevelDebug,"midi CC ch(%d) cc(%d) value(%d)", channel, cc, value);

            if (channel == 1){
                if (1 <= cc && cc <= 8){
                    // turn a signed 7-bit value into a proper int value we can work with
                    int i = (int)(signed char)(value | ((value & 0b01000000) << 1));
                    float r = (float)i * kDefaultRelativeStepSize;
                    if (cc == 1){
                        if (softBtn4_ == Released) {
                            lisaControllerProxy_.setSelectedSourcesRelativePan(r);
                        } else {
                            // do nothing
                        }
                    } else if (cc == 2){
                        if (softBtn4_ == Released) {
                            lisaControllerProxy_.setSelectedSourcesRelativePanSpread(r);
                        } else {
                            lisaControllerProxy_.setSelectedSourcesRelativeWidth(r);
                        }
                    } else if (cc == 3){
                        if (softBtn4_ == Released) {
                            lisaControllerProxy_.setSelectedSourcesRelativeDistance(r);
                        } else {
                        }
                    } else if (cc == 4){
                        if (softBtn4_ == Released) {
                            lisaControllerProxy_.setSelectedSourcesRelativeElevation(r);
                        } else {
                            lisaControllerProxy_.setSelectedSourceRelativeAuxSend(r);
                        }
                    } else if (cc == 5){
                        // not used
                    } else if (cc == 6){
                        if (softBtn4_ == Released) {
                            lisaControllerProxy_.setSelectedSourcesRelativeWidth(r);
                        } else if (cc == 7){
                            // not used
                        }
                    } else if (cc == 8){
                        lisaControllerProxy_.setSelectedSourceRelativeAuxSend(r);
                    }
                }
            } // channel == 1
            else if (channel == 2){
                if (cc == 0){
                    lisaControllerProxy_.setMasterFaderPos((float)value / 127.0);
                }
                else if (cc == 1){
                    lisaControllerProxy_.setReverbFaderPos((float)value / 127.0);
                }
                else if (cc == 2){
                    lisaControllerProxy_.setMonitorFaderPos((float)value / 127.0);
                }
                else if (cc == 3){
                    lisaControllerProxy_.setUserFaderNPos(1, (float)value / 127.0);
                }
                else if (cc == 4){
                    lisaControllerProxy_.setUserFaderNPos(2, (float)value / 127.0);
                }
            } // channel == 2
        }

        void SQMitm::onMidiProgramChange(int channel, int program){

            log(LogLevelDebug,"midi PC ch(%d) program(%d)", channel, program);

        }

        void SQMitm::onMidiFaderLevel(int channel, int value){

            channel += 1;

            if (channel == 1){
                lisaControllerProxy_.setMasterFaderPos((float)value / 255.0);
            }
            else if (channel == 2){
                lisaControllerProxy_.setReverbFaderPos((float)value / 255.0);
            }
            else if (channel == 3){
                lisaControllerProxy_.setMonitorFaderPos((float)value / 255.0);
            }
            else if (channel == 4){
                lisaControllerProxy_.setUserFaderNPos(1, (float)value / 255.0);
            }
            else if (channel == 5){
                lisaControllerProxy_.setUserFaderNPos(2, (float)value / 255.0);
            }
        }

        void SQMitm::onMidiFaderMute(int channel){

        }

        void SQMitm::receivedMasterFaderPos(float pos){
            // only process if completely started
            if (state != State_Started){
                return;
            }

//            SQMixMitm::Command cmd = SQMixMitm::Command::midiFaderLevel()
            //TODO
//            sqMidiControlClient.sendControlChange(1,0,(int)(127.0 * pos));
        }

        void SQMitm::receivedReverbFaderPos(float pos){
            // only process if completely started
            if (state != State_Started){
                return;
            }

            //TODO
//            sqMidiControlClient.sendControlChange(1,1,(int)(127.0 * pos));
        }


    }
}