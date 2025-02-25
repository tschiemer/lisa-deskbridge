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

#ifndef LISA_DESKBRIDGE_LISACONTROLLERPROXY_H
#define LISA_DESKBRIDGE_LISACONTROLLERPROXY_H

#include <thread>

#include "LisaController.h"

#include "osc/OscPacketListener.h"
#include "ip/UdpSocket.h"

namespace LisaDeskbridge {


    class LisaControllerProxy : public osc::OscPacketListener {

        public:

            class Delegate {
                public:
                    virtual void receivedSourcePan(SourceId_t src, float pan){}
                    virtual void receivedSourceWidth(SourceId_t src, float width){}
                    virtual void receivedSourceDepth(SourceId_t src, float depth){}
                    virtual void receivedSourceElevation(SourceId_t src, float elevation){}
                    virtual void receivedSourceAuxSend(SourceId_t src, float send){}

                    virtual void receivedMasterGain(float gain){}
                    virtual void receivedMasterFaderPos(float pos){}

                    virtual void receivedReverbGain(float gain){}
                    virtual void receivedReverbFaderPos(float pos){}
            };


        protected:

            enum arg_t {INT_T, FLOAT_T, STRING_T};

            Delegate * iDelegate = nullptr;
            UdpListeningReceiveSocket * udpListeningReceiveSocket = nullptr;
            UdpTransmitSocket * udpTransmitSocket = nullptr;

            bool mIsRunning = false;

            std::thread * thread = nullptr;

            SourceId_t lastSelectedSource = 0;

            void sendToController(const char * address, int count, ...);

        public:

            LisaControllerProxy(Delegate * delegate){
                iDelegate = delegate;
            }

            bool isRunning(){ return mIsRunning; }
            void start(unsigned short listenPort, std::string &controllerAddress, unsigned short controllerPort);
            void stop();

            virtual void ProcessMessage( const osc::ReceivedMessage& m, const IpEndpointName& remoteEndpoint );

        public:


            // Source control flags

            void setSourceControlFlagPan(SourceId_t src, ControlFlag_t flag);
            void setSourceControlFlagWidth(SourceId_t src, ControlFlag_t flag);
            void setSourceControlFlagDistance(SourceId_t src, ControlFlag_t flag);
            void setSourceControlFlagElevation(SourceId_t src, ControlFlag_t flag);
            void setSourceControlFlagAuxSend(SourceId_t src, ControlFlag_t flag);

            void setAllSourcesControlFlags(ControlFlag_t flag);

            void setAllSourcesControlBySnapshots();
            void setAllSourcesControlByOSC();

            // Source parameters

            void setSourcePan(SourceId_t src, float value);
            void setSourceWidth(SourceId_t src, float value);
            void setSourceDistance(SourceId_t src, float value);
            void setSourceElevation(SourceId_t src, float value);
            void setSourcePanSpread(SourceId_t src, float value);
            void setSourceAuxSend(SourceId_t src, float value);

            void setSourceAllParameters(SourceId_t src, float pan, float width, float depth, float elevation, float auxSend);

            void setSourceRelativePan(SourceId_t src, float value);
            void setSourceRelativeWidth(SourceId_t src, float value);
            void setSourceRelativeDistance(SourceId_t src, float value);
            void setSourceRelativeElevation(SourceId_t src, float value);
            void setSourceRelativePanSpread(SourceId_t src, float value);
            void setSourceRelativeAuxSend(SourceId_t src, float value);

            void setSelectedSourceRelativeAuxSend(float rsend);

            void setSourceFxIntensity(SourceId_t src, FxId_t fx, float value);
            void setSourceFxActive(SourceId_t src, FxId_t fx, bool on);

            void setSelectedSourcesRelativePan(float rpan);
            void setSelectedSourcesRelativeWidth(float rwidth);
            void setSelectedSourcesRelativeDistance(float rdist);
            void setSelectedSourcesRelativeElevation(float relev);
            void setSelectedSourcesRelativePanSpread(float rspread);


            // Source Solo, Snap, Delay

            void setSourceSolo(SourceId_t src, bool on);
            void setSelectedSourceSolo(bool on);
            void setSourceStaticDelayValue(SourceId_t src, float value);
            void setSelectedSourceStaticDelayValue(float value);
            void snapSourceToSpeaker(SourceId_t src);
            void snapSelectedSourceToSpeaker();


            // Source processing

            void setSourceOptGain(SourceId_t src, bool on);
            void setSourceOptHpf(SourceId_t src, bool on);
            void setSourceOptDelayEnabled(SourceId_t src, bool on);
            void setSourceOptDelayMode(SourceId_t src, DelayMode_t mode);
            void setSourceOptReverbEarly(SourceId_t src, bool on);
            void setSourceOptReverbCluster(SourceId_t src, bool on);
            void setSourceOptReverbLate(SourceId_t src, bool on);
            void setSourceOptDirectSound(SourceId_t src, bool on);


            // Group parameters

            void setGroupPan(GroupId_t grp, float value);
            void setGroupWidth(GroupId_t grp, float value);
            void setGroupDistance(GroupId_t grp, float value);
            void setGroupElevation(GroupId_t grp, float value);
            void setGroupAuxSend(GroupId_t grp, float value);
            void setGroupPanSpread(GroupId_t grp, float value);

            void setGroupRelativePan(GroupId_t grp, float value);
            void setGroupRelativeWidth(GroupId_t grp, float value);
            void setGroupRelativeDistance(GroupId_t grp, float value);
            void setGroupRelativeElevation(GroupId_t grp, float value);
            void setGroupRelativeAuxSend(GroupId_t grp, float value);
            void setGroupRelativePanSpread(GroupId_t grp, float value);


            // Snapshots

            void fireSnapshot(SnapshotId_t snp);
            void firePreviousSnapshot();
            void fireNextSnapshot();
            void refireCurrentSnapshot();
            void saveCurrentSnapshot();
            void saveAsNewSnapshot();

            // Reverbs

            void loadReverbPreset(ReverbId_t reverb);

            // FX

            void startFx(FxId_t fx);
            void restartFx(FxId_t fx);
            void stopFx(FxId_t fx);

            // BPM

            void lockBPMToMidiClock(bool on);
            void setBPM(float bpm);
            void tapTempo();

            // Master Fader

            void setMasterGain(float gain);
            void setMasterFaderPos(float pos);
            void setMasterMute(bool on);

            // Reverb Fader

            void setReverbGain(float gain);
            void setReverbFaderPos(float pos);
            void setReverbMute(bool on);

            // Monitor Fader

            void setMonitorGain(float gain);
            void setMonitorFaderPos(float pos);
            void setMonitorMute(bool on);

            // User Fader

            void setUserFaderNGain(int fader, float gain);
            void setUserFaderNPos(int fader, float pos);
            void setUserFaderNMute(int fader, bool on);

            // Source + Group selection

            SourceId_t getLastSelectedSource(){ return lastSelectedSource; };

            void selectSource(SourceId_t src);
            void addSourceToSelection(SourceId_t src);
            void removeSourceFromSelection(SourceId_t src);
            void selectGroup(SourceId_t src);
            void clearSelection();

            // Headtracker

            void setHeadtrackerOrientation(float yaw, float pitch, float roll);
            void resetHeadtracker();
            void setHeadtrackerType(HeadtrackerType_t type);

            // OSC Devices

            void registerDevice(DeviceId_t device_id, const char ipAddress[], unsigned short port);
            void unregisterDevice(DeviceId_t device_id);
            void setDeviceName(DeviceId_t device_id, const char name[]);
            void enableSendingToDevice(DeviceId_t device_id, bool enable);
            void enableReceivingFromDevice(DeviceId_t device_id, bool enable);
            void setDeviceCoordFormat(DeviceId_t device_id, CoordFormat_t format);
            void setMasterGainControl(DeviceId_t deviceId, bool on);

            // ping

            void ping(const char ipAddress[], unsigned short port);

    };

} // LisaDeskbridge

#endif //LISA_DESKBRIDGE_LISACONTROLLERPROXY_H
