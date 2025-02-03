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

#include "bridges/SQ6.h"

#include <iostream>

#include "log.h"

namespace LisaDeskbridge {
    namespace Bridges {


        SQ6::SQ6(BridgeOpts &opts) :
            Bridge(opts),
            mixingStationDelegate(*this), mixingStationVirtualMidiDevice(mixingStationDelegate),
            sqMidiControlDelegate(*this), sqMidiControlClient(sqMidiControlDelegate){

            if (opts.contains("midiin")){
                midiInPortName = opts["midiin"];
            }
            if (opts.contains("midiout")){
                midiOutPortName = opts["midiout"];
            }
        }

        bool SQ6::startMixingStationVirtualMidiDevice(){
            log(LogLevelInfo, "Starting virtual MIDI Device '%s' for channel selection ..",VirtualMidiDevice::kDefaultPortName );

            try {
                mixingStationVirtualMidiDevice.start();
            } catch (const std::exception & e){
                std::cerr << e.what() << std::endl;
                return false;
            }

            return true;
        }

        void SQ6::stopMixingStationVirtualMidiDevice(){
            log(LogLevelInfo, "Stopping virtual MIDI Device for channel selection ..");

            mixingStationVirtualMidiDevice.stop();
        }

        bool SQ6::startSQMidiControlClient() {
            log(LogLevelInfo, "Starting MIDI Client..");

            try {
                sqMidiControlClient.start(midiInPortName, midiOutPortName);
            } catch (const std::exception & e){
                std::cerr << e.what() << std::endl;
                return false;
            }

            return true;
        }

        void SQ6::stopSQMidiControlClient() {
            log(LogLevelInfo, "Stopping MIDI Client..");

            sqMidiControlClient.stop();
        }

        bool SQ6::init() {

            if (state == State_Started){
                return true;
            }

            state = State_Starting;

            if (Bridge::init() == false){
                state = State_Stopped;
                return false;
            }

            if (startMixingStationVirtualMidiDevice() == false){
                stopLisaControllerProxy();
                state = State_Stopped;
                return false;
            }

            if (startSQMidiControlClient() == false){
                stopMixingStationVirtualMidiDevice();
                stopLisaControllerProxy();
                state = State_Stopped;
                return false;
            }

            state = State_Started;

            return true;
        }

        void SQ6::deinit() {

            if (state != State_Started){
                return;
            }

            state = State_Stopping;

            stopSQMidiControlClient();
            stopMixingStationVirtualMidiDevice();

            Bridge::deinit();

            state = State_Stopped;
        }

        void SQ6::MixingStationDelegate::receivedNoteOn(int channel, int note, int velocity){
            // only process if completely started
            if (sq6->state != State_Started){
                return;
            }

            log(LogLevelDebug,"MIX NOTE ON ch(%d) note(%d) velocity(%d)", channel, note, velocity);

            // select source (channel == 1 (valid source range)
            if (channel == 1 && 1 <= note && note <= 96){

                // btn  btn btn alt
                // 1    2   3   4
                //
                // 0    0   0   0    if follow -> select 1
                // 0    0   0   1
                // 0    0   1   0   snap
                // 0    0   1   1   solo on
                // 0    1   0   0   add 1
                // 0    1   0   1   remove 1
                // 0    1   1   0
                // 0    1   1   1   solo off
                // 1    0   0   0   select 1
                // 1    0   0   1
                // 1    0   1   0
                // 1    0   1   1
                // 1    1   0   0
                // 1    1   0   1
                // 1    1   1   0
                // 1    1   1   1

                if ((sq6->softBtn1 == ButtonState_Pressed || sq6->followSelect) &&
                    sq6->softBtn2 == ButtonState_Released &&
                    sq6->softBtn3 == ButtonState_Released &&
                    sq6->softBtn4 == ButtonState_Released){
                        sq6->lisaControllerProxy.selectSource(note);
                }
                else if (sq6->softBtn1 == ButtonState_Released &&
                         sq6->softBtn2 == ButtonState_Pressed &&
                         sq6->softBtn3 == ButtonState_Released){
                    if (sq6->softBtn4 == ButtonState_Released){
                        sq6->lisaControllerProxy.addSourceToSelection(note);
                    } else {
                        sq6->lisaControllerProxy.removeSourceFromSelection(note);
                    }
                }
                else if (sq6->softBtn1 == ButtonState_Released &&
                         sq6->softBtn2 == ButtonState_Released &&
                         sq6->softBtn3 == ButtonState_Pressed &&
                         sq6->softBtn4 == ButtonState_Released){
                    sq6->lisaControllerProxy.snapSourceToSpeaker(note);
                }
                else if (sq6->softBtn1 == ButtonState_Released &&
                         sq6->softBtn3 == ButtonState_Pressed &&
                         sq6->softBtn4 == ButtonState_Pressed){
                    if (sq6->softBtn2 == ButtonState_Released){
                        sq6->lisaControllerProxy.setSourceSolo(note, true);
                    } else {
                        sq6->lisaControllerProxy.setSourceSolo(note, false);
                    }
                }

            }
        }


        void SQ6::SQMidiControlDelegate::receivedNoteOn(int channel, int note, int velocity){
            // only process if completely started
            if (sq6->state != State_Started){
                return;
            }

            log(LogLevelDebug, "SQ NOTE ON ch(%d) note(%d) velocity(%d)", channel, note, velocity);

            if (channel == 1 && note == 1){
                sq6->softBtn1 = ButtonState_Pressed;

                // if ALT button is pressed toggle the auto-follow
                if (sq6->softBtn4 == ButtonState_Pressed){
                    sq6->followSelect = !sq6->followSelect;
                }
            }
            else if (channel == 1 && note == 2){
                sq6->softBtn2 = ButtonState_Pressed;
//                sq6->selectionMode = SelectionMode_Add;
            }
            else if (channel == 1 && note == 3){
                sq6->softBtn3 = ButtonState_Pressed;
            }
            else if (channel == 1 && note == 4){
                sq6->softBtn4 = ButtonState_Pressed;
            }
            else if (channel == 2 && isValidGroupId(note)){
                sq6->lisaControllerProxy.selectGroup(note);
            }
            else if (channel == 3 && isValidSnapshotId(note)){
                sq6->lisaControllerProxy.fireSnapshot(note);
            }
            else if (channel == 4){
                if (note == 1){
                    sq6->lisaControllerProxy.setAllSourcesControlByOSC();
                }
                else if (note == 2){
                    sq6->lisaControllerProxy.setAllSourcesControlBySnapshots();
                }
                else if (note == 11){
                    sq6->lisaControllerProxy.firePreviousSnapshot();
                }
                else if (note == 12){
                    sq6->lisaControllerProxy.refireCurrentSnapshot();
                }
                else if (note == 13){
                    sq6->lisaControllerProxy.fireNextSnapshot();
                }
            } // channel == 3

        }
        void SQ6::SQMidiControlDelegate::receivedNoteOff(int channel, int note, int velocity){
            // only process if completely started
            if (sq6->state != State_Started){
                return;
            }

            log(LogLevelDebug, "SQ NOTE OFF ch(%d) note(%d) velocity(%d)", channel, note, velocity);

            if (channel == 1 && note == 1){
                sq6->softBtn1 = ButtonState_Released;
            }
            else if (channel == 1 && note == 2){
                sq6->softBtn2 = ButtonState_Released;
            }
            else if (channel == 1 && note == 3){
                sq6->softBtn3 = ButtonState_Released;
            }
            else if (channel == 1 && note == 4){
                sq6->softBtn4 = ButtonState_Released;
            }
        }
        void SQ6::SQMidiControlDelegate::receivedControlChange(int channel, int cc, int value){
            // only process if completely started
            if (sq6->state != State_Started){
                return;
            }

            log(LogLevelDebug,"SQ CC ch(%d) cc(%d) value(%d)", channel, cc, value);

            if (channel == 1){
                if (1 <= cc && cc <= 8){
                    // turn a signed 7-bit value into a proper int value we can work with
                    int i = (int)(signed char)(value | ((value & 0b01000000) << 1));
                    float r = (float)i * kDefaultRelativeStepSize;
                    if (cc == 1){
                        if (sq6->softBtn4 == ButtonState_Released) {
                            sq6->lisaControllerProxy.setSelectedSourcesRelativePan(r);
                        } else {
                            // do nothing
                        }
                    } else if (cc == 2){
                        if (sq6->softBtn4 == ButtonState_Released) {
                            sq6->lisaControllerProxy.setSelectedSourcesRelativePanSpread(r);
                        } else {
                            sq6->lisaControllerProxy.setSelectedSourcesRelativeWidth(r);
                        }
                    } else if (cc == 3){
                        if (sq6->softBtn4 == ButtonState_Released) {
                            sq6->lisaControllerProxy.setSelectedSourcesRelativeDistance(r);
                        } else {
                        }
                    } else if (cc == 4){
                        if (sq6->softBtn4 == ButtonState_Released) {
                            sq6->lisaControllerProxy.setSelectedSourcesRelativeElevation(r);
                        } else {
                            sq6->lisaControllerProxy.setSelectedSourceRelativeAuxSend(r);
                        }
                    } else if (cc == 5){
                        // not used
                    } else if (cc == 6){
                        if (sq6->softBtn4 == ButtonState_Released) {
                            sq6->lisaControllerProxy.setSelectedSourcesRelativeWidth(r);
                    } else if (cc == 7){
                            // not used
                        }
                    } else if (cc == 8){
                        sq6->lisaControllerProxy.setSelectedSourceRelativeAuxSend(r);
                    }
                }
            } // channel == 1
            else if (channel == 2){
                if (cc == 0){
                    sq6->lisaControllerProxy.setMasterFaderPos((float)value / 127.0);
                }
                else if (cc == 1){
                    sq6->lisaControllerProxy.setReverbFaderPos((float)value / 127.0);
                }
                else if (cc == 2){
                    sq6->lisaControllerProxy.setMonitorFaderPos((float)value / 127.0);
                }
                else if (cc == 3){
                    sq6->lisaControllerProxy.setUserFaderNPos(1,(float)value / 127.0);
                }
                else if (cc == 4){
                    sq6->lisaControllerProxy.setUserFaderNPos(2,(float)value / 127.0);
                }
            } // channel == 2
        }
        void SQ6::SQMidiControlDelegate::receivedProgramChange(int channel, int program){
            // only process if completely started
            if (sq6->state != State_Started){
                return;
            }

            log(LogLevelDebug,"SQ PCH ch(%d) p(%d)", channel, program);

            if (channel == 1){
                if (isValidSnapshotId(program)){
                    sq6->lisaControllerProxy.fireSnapshot(program);
                }
            }
        }

        void SQ6::receivedMasterFaderPos(float pos){
            // only process if completely started
            if (state != State_Started){
                return;
            }

            sqMidiControlClient.sendControlChange(1,0,(int)(127.0 * pos));
        }

        void SQ6::receivedReverbFaderPos(float pos){
            // only process if completely started
            if (state != State_Started){
                return;
            }

            sqMidiControlClient.sendControlChange(1,1,(int)(127.0 * pos));
        }

    }
}