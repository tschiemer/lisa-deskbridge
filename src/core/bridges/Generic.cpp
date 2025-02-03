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

#include "bridges/Generic.h"

#include <iostream>

#include "log.h"

namespace LisaDeskbridge {
    namespace Bridges {

        Generic::Generic(BridgeOpts &opts) :
                Bridge(opts),
                virtualMidiDevice(*this),
                midiClient(*this)
        {
                if (opts.contains("midiin")){
                    midiInPortName = opts["midiin"];
                }
                if (opts.contains("midiout")){
                    midiOutPortName = opts["midiout"];
                }
        }

        bool Generic::startVirtualMidiDevice(){
            log(LogLevelInfo, "Starting virtual MIDI Device '%s' .." , VirtualMidiDevice::kDefaultPortName);

            try {
                virtualMidiDevice.start();
            } catch (const std::exception & e){
                std::cerr << e.what() << std::endl;
                return false;
            }

            return true;
        }

        void Generic::stopVirtualMidiDevice(){
            log(LogLevelInfo, "Stopping virtual MIDI Device .." );
            virtualMidiDevice.stop();
        }

        bool Generic::startMidiClient() {
            if (midiInPortName.length() == 0 && midiOutPortName == 0){
//                error("No midi in- or out-port defined!");
                return true;
            }

            log(LogLevelInfo, "Starting MIDI Client.." );
            try {
                midiClient.start(midiInPortName, midiOutPortName);
            } catch (const std::exception & e){
                error("starting MIDI Client: %s", e.what() );
                return false;
            }

            return true;
        }

        void Generic::stopMidiClient() {
            if (midiInPortName.length() == 0 && midiOutPortName == 0){
                return;
            }

            log(LogLevelInfo, "Stopping MIDI Client.." );
            midiClient.stop();
        }

        bool Generic::init() {

            if (state == State_Started){
                return true;
            }

            state = State_Starting;

            if (Bridge::init() == false){
                state = State_Stopped;
                return false;
            }

            if (startVirtualMidiDevice() == false){
                Bridge::deinit();
                state = State_Stopped;
                return false;
            }

            if (startMidiClient() == false){
                stopVirtualMidiDevice();
                Bridge::deinit();
                state = State_Stopped;
                return false;
            }

            state = State_Started;

            return true;
        }

        void Generic::deinit() {

            if (state != State_Started){
                return;
            }

            state = State_Stopping;

            stopMidiClient();
            stopVirtualMidiDevice();

            Bridge::deinit();

            state = State_Stopped;
        }

        void Generic::selectedChannelAction(int i){

            // select source based on...
            // - select single
            // - add source to selection
            // - remove source from selection
        }

        // LisaDeskbridge::MidiReceiver::Delegate

        void Generic::receivedNoteOn(int channel, int note, int velocity){
            // only process if completely started
            if (state != State_Started){
                return;
            }

            log(LogLevelDebug, "NOTE ON ch(%d) note(%d) velocity(%d)", channel, note, velocity);

            if (channel == 1){ // select source
                if (!isValidDeviceId(note)){
                    return;
                }

                selectedChannelAction(note);
            }
            else if (channel == 2){ // select group
                if (!isValidGroupId(note)){
                    return;
                }

                lisaControllerProxy.selectGroup(note);
            }
            else if (channel == 3){ // Fire snapshot
                if (!isValidSnapshotId(note)){
                    return;
                }

                lisaControllerProxy.fireSnapshot(note);
            }
            else if (channel == 4){ // L-ISA Controller Options

            }
            else if (channel == 5){ // Generic bridge options

            }

        }
        void Generic::receivedNoteOff(int channel, int note, int velocity){
            // only process if completely started
            if (state != State_Started){
                return;
            }

            log(LogLevelDebug, "NOTE OFF ch(%d) note(%d) velocity(%d)", channel, note, velocity);

        }
        void Generic::receivedControlChange(int channel, int cc, int value){
            // only process if completely started
            if (state != State_Started){
                return;
            }

            log(LogLevelDebug,"CC ch(%d) cc(%d) value(%d)", channel, cc, value);

            if (channel == 1){

            }
            else if (channel == 2){ // faders
                if (cc == 1){
                    lisaControllerProxy.setMasterFaderPos((float)value / 127.0);
                }
                else if (cc == 2){
                    lisaControllerProxy.setReverbFaderPos((float)value / 127.0);
                }
                else if (cc == 3){
                    lisaControllerProxy.setMonitorFaderPos((float)value / 127.0);
                }
                else if (cc == 4){
                    lisaControllerProxy.setUserFaderNPos(1,(float)value / 127.0);
                }
                else if (cc == 5){
                    lisaControllerProxy.setUserFaderNPos(2,(float)value / 127.0);
                }
            }

        }


        // LisaDeskbridge::LisaControllerProxy::Delegate

        void Generic::receivedMasterFaderPos(float pos){
            // only process if completely started
            if (state != State_Started){
                return;
            }

            int value = (int)(127.0 * pos);

            virtualMidiDevice.sendControlChange(1, 0, value);
            midiClient.sendControlChange(1,0,value);
        }

        void Generic::receivedReverbFaderPos(float pos){
            // only process if completely started
            if (state != State_Started){
                return;
            }

            int value = (int)(127.0 * pos);

            virtualMidiDevice.sendControlChange(1,1,value);
            midiClient.sendControlChange(1,1,value);
        }
    }
}
