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

#include "osc/OscPacketListener.h"
#include "ip/UdpSocket.h"

namespace LisaDeskbridge {

    constexpr uint16_t kLisaControllerPortDefault = 8880;
    constexpr std::basic_string_view<char> kLisaControllerHostDefault = "127.0.0.1";
    constexpr uint16_t kRemotePortDefault = 9000;

    class LisaControllerProxy : public osc::OscPacketListener {

        public:

            typedef unsigned int SourceId_t;
            typedef unsigned int GroupId_t;
            typedef unsigned int DeviceId_t;

            enum CoordFormat_t {CoordFormatLISA, CoordFormatADM};

            class IDelegate {
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

            enum arg_t {BOOL_T, INT_T, FLOAT_T};

            IDelegate * iDelegate = nullptr;
            UdpListeningReceiveSocket * udpListeningReceiveSocket = nullptr;
            UdpTransmitSocket * udpTransmitSocket = nullptr;

            bool mIsRunning = false;

            std::thread * thread = nullptr;

            SourceId_t lastSelectedSource = 0;

            void sendToController(const char * address, int count, ...);

        public:

            LisaControllerProxy(IDelegate * delegate){
                iDelegate = delegate;
            }

            bool isRunning(){ return mIsRunning; }
            void start(unsigned short listenPort, std::basic_string_view<char> &controllerAddress, unsigned short controllerPort);
            void stop();

            virtual void ProcessMessage( const osc::ReceivedMessage& m, const IpEndpointName& remoteEndpoint );

            SourceId_t getLastSelectedSource(){ return lastSelectedSource; };

            void selectSource(SourceId_t id);
            void addSourceToSelection(SourceId_t id);
            void removeSourceFromSelection(SourceId_t id);
            void selectGroup(SourceId_t id);

            void clearSelection();

            /**
             * Set relative pan of selected sources
             * @param rpan [-1.0-1.0]
             */
            void setSelectedSourcesRelativePan(float rpan);

            /**
             * Set relative width of selected sources
             * @param rwidth [-1.0-1.0]
             */
            void setSelectedSourcesRelativeWidth(float rwidth);

            /**
             * Set relative distance of selected sources
             * @param rdist [-1.0-1.0]
             */
            void setSelectedSourcesRelativeDistance(float rdist);

            /**
             * Set relative elevation of selected sources
             * @param relev [-1.0-1.0]
             */
            void setSelectedSourcesRelativeElevation(float relev);

            /**
             * Set relative pan spread of selected sources
             * @param rspread
             */
            void setSelectedSourcesRelativePanSpread(float rspread);

            void setSourceRelativeAuxSend(SourceId_t id, float rsend);

            void setSelectedSourceRelativeAuxSend(float rsend);

            /**
             * Set source solo
             * @param src
             */
            void setSourceSolo(SourceId_t id, bool on);

            void setSelectedSourceSolo(bool on);

            /**
             * Snap selected source to speaker
             * @param src
             */
            void snapSourceToSpeaker(SourceId_t id);

            void snapSelectedSourceToSpeaker();

            /**
             * Set master gain
             *
             * @param gain
             */
            void setMasterGain(float gain);

            /**
             * Set master fader position
             * @param pos
             */
            void setMasterFaderPos(float pos);

            /**
             * Mute/Unmute master
             * @param on
             */
            void setMasterMute(bool on);

            /**
             * Set reverb gain
             * @param gain
             */
            void setReverbGain(float gain);

            /**
             * Set reverb fader position
             * @param pos
             */
            void setReverbFaderPos(float pos);

            /**
             * Mute/Unmute reverb
             * @param on
             */
            void setReverbMute(bool on);

            void registerDevice(DeviceId_t device_id, const char * ip, unsigned short port);
            void unregisterDevice(DeviceId_t device_id);
            void setDeviceName(DeviceId_t device_id, const char * name);
            void enableSendingToDevice(DeviceId_t device_id, bool enable);
            void enableReceivingFromDevice(DeviceId_t device_id, bool enable);
            void setDeviceCoordFormat(DeviceId_t device_id, CoordFormat_t format);
    };

} // LisaDeskbridge

#endif //LISA_DESKBRIDGE_LISACONTROLLERPROXY_H
