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

#include <iostream>

#include "LisaControllerProxy.h"
#include "LisaController.h"

#include "osc/OscReceivedElements.h"
#include "osc/OscOutboundPacketStream.h"

#ifndef OUTPUT_BUFFER_SIZE
#define OUTPUT_BUFFER_SIZE 512
#endif


namespace LisaDeskbridge {

    void LisaControllerProxy::start(unsigned short listenPort, std::basic_string_view<char> & controllerAddress, unsigned short controllerPort){
        if (mIsRunning){
            return;
        }

        std::cout << "Listening for L-ISA Controller messages on port " << listenPort << std::endl;

        udpListeningReceiveSocket = new UdpListeningReceiveSocket(
                IpEndpointName( IpEndpointName::ANY_ADDRESS, listenPort),
                this
                );

        thread = new std::thread([](UdpListeningReceiveSocket * socket){
            socket->Run();
        }, udpListeningReceiveSocket);

        std::cout << "Sending to L-ISA Controller on host " << controllerAddress << " on port " << controllerPort << std::endl;

        udpTransmitSocket = new UdpTransmitSocket( IpEndpointName( controllerAddress.data(), controllerPort ) );

        mIsRunning = true;
    }

    void LisaControllerProxy::stop(){
        if (!mIsRunning){
            return;
        }

        udpListeningReceiveSocket->AsynchronousBreak();
        thread->join();

        delete udpTransmitSocket;
        delete udpListeningReceiveSocket;
        delete thread;

        mIsRunning = false;
    }

    void LisaControllerProxy::ProcessMessage( const osc::ReceivedMessage& m,
                                 const IpEndpointName& remoteEndpoint ) {
        (void) remoteEndpoint; // suppress unused parameter warning


        try{
            // example of parsing single messages. osc::OsckPacketListener
            // handles the bundle traversal.

            osc::ReceivedMessage::const_iterator args = m.ArgumentsBegin();

            SourceId_t src = 0;
            int  n = 0;

            if( std::strcmp( m.AddressPattern(), kMsgRxMasterGain ) == 0 ){
                float gain = (args++)->AsFloat();
                iDelegate->receivedMasterGain(gain);
            }
            else if( std::strcmp( m.AddressPattern(), kMsgRxMasterFaderPos ) == 0 ){
                float pos = (args++)->AsFloat();
                iDelegate->receivedMasterFaderPos(pos);
            }
            else if( std::strcmp( m.AddressPattern(), kMsgRxReverbGain ) == 0 ){
                float gain = (args++)->AsFloat();
                iDelegate->receivedReverbGain(gain);
            }
            else if( std::strcmp( m.AddressPattern(), kMsgRxReverbFaderPos ) == 0 ){
                float pos = (args++)->AsFloat();
                iDelegate->receivedReverbFaderPos(pos);
            }
            else if (sscanf(m.AddressPattern(), kMsgRxSourcePan, &src, &n) == 1 && n > 0){
                float pan = (args++)->AsFloat();
                iDelegate->receivedSourcePan(src, pan);
            }
            else if (sscanf(m.AddressPattern(), kMsgRxSourceWidth, &src, &n) == 1 && n > 0){
                float width = (args++)->AsFloat();
                iDelegate->receivedSourceWidth(src, width);
            }
            else if (sscanf(m.AddressPattern(), kMsgRxSourceDistance, &src, &n) == 1 && n > 0){
                float distance = (args++)->AsFloat();
                iDelegate->receivedSourceDepth(src, distance);
            }
            else if (sscanf(m.AddressPattern(), kMsgRxSourceElevation, &src, &n) == 1 && n > 0){
                float elevation = (args++)->AsFloat();
                iDelegate->receivedSourceElevation(src, elevation);
            }
            else if (sscanf(m.AddressPattern(), kMsgRxSourceAuxSend, &src, &n) == 1 && n > 0){
                float send = (args++)->AsFloat();
                iDelegate->receivedSourceAuxSend(src, send);
            }
            else {

                std::cout << "Received unknown packet: " << m.AddressPattern() << std::endl;
            }
        } catch( osc::Exception& e ){
            // any parsing errors such as unexpected argument types, or
            // missing arguments get thrown as exceptions.
            std::cerr << "error while parsing message: "
                      << m.AddressPattern() << ": " << e.what() << "\n";
        }
    }

    void LisaControllerProxy::sendToController(const char *address, int count, ...) {
        assert(address != nullptr);

        char buffer[OUTPUT_BUFFER_SIZE];
        osc::OutboundPacketStream msg( buffer, OUTPUT_BUFFER_SIZE );

        msg << osc::BeginMessage( address );

        va_list args;
        va_start(args, count);
        for (int i = 0; i < count; i+=2)
        {
            enum arg_t type = va_arg(args, arg_t);
             if (type == INT_T){
                int i = va_arg(args, int);
                msg << i;
            } else if (type == FLOAT_T){
                float d = va_arg(args, double);
                msg << d;
            } else if (type == STRING_T) {
                char * str = va_arg(args, char* );
                msg << str;
            } else {
                 assert(false); // should not happen ...
            }
        }
        va_end(args);

        msg << osc::EndMessage;

        udpTransmitSocket->Send(msg.Data(), msg.Size());
    }

    // Source control flags

    void LisaControllerProxy::setSourceControlFlagPan(SourceId_t src, ControlFlag_t flag){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));
        assert(isValidControlFlag(flag));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourceControlPan, src);

        sendToController(msg,1, STRING_T, kControlFlags[flag]);
    }
    void LisaControllerProxy::setSourceControlFlagWidth(SourceId_t src, ControlFlag_t flag){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));
        assert(isValidControlFlag(flag));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourceControlWidth, src);

        sendToController(msg,1, STRING_T, kControlFlags[flag]);
    }
    void LisaControllerProxy::setSourceControlFlagDistance(SourceId_t src, ControlFlag_t flag){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));
        assert(isValidControlFlag(flag));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourceControlDistance, src);

        sendToController(msg,1, STRING_T, kControlFlags[flag]);
    }
    void LisaControllerProxy::setSourceControlFlagElevation(SourceId_t src, ControlFlag_t flag){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));
        assert(isValidControlFlag(flag));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourceControlElevation, src);

        sendToController(msg,1, STRING_T, kControlFlags[flag]);
    }
    void LisaControllerProxy::setSourceControlFlagAuxSend(SourceId_t src, ControlFlag_t flag){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));
        assert(isValidControlFlag(flag));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourceControlAuxSend, src);

        sendToController(msg,1, STRING_T, kControlFlags[flag]);
    }


    // Source Parameters

    void LisaControllerProxy::setSourcePan(SourceId_t src, float value){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));
        assert(isValidAbsoluteValue(value));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourcePan, src);

        sendToController(msg,1, FLOAT_T, value);
    }
    void LisaControllerProxy::setSourceWidth(SourceId_t src, float value){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));
        assert(isValidAbsoluteValue(value));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourceWidth, src);

        sendToController(msg,1, FLOAT_T, value);
    }
    void LisaControllerProxy::setSourceDistance(SourceId_t src, float value){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));
        assert(isValidAbsoluteValue(value));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourceDistance, src);

        sendToController(msg,1, FLOAT_T, value);
    }
    void LisaControllerProxy::setSourceElevation(SourceId_t src, float value){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));
        assert(isValidAbsoluteValue(value));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourceElevation, src);

        sendToController(msg,1, FLOAT_T, value);
    }
    void LisaControllerProxy::setSourcePanSpread(SourceId_t src, float value){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));
        assert(isValidAbsoluteValue(value));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourcePanSpread, src);

        sendToController(msg,1, FLOAT_T, value);
    }
    void LisaControllerProxy::setSourceAuxSend(SourceId_t src, float value){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));
        assert(isValidAbsoluteValue(value));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourceAuxSend, src);

        sendToController(msg,1, FLOAT_T, value);
    }

    void LisaControllerProxy::setSourceAllParameters(SourceId_t src, float pan, float width, float depth, float elevation, float auxSend){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));
        assert(isValidAbsoluteValue(pan));
        assert(isValidAbsoluteValue(width));
        assert(isValidAbsoluteValue(depth));
        assert(isValidAbsoluteValue(elevation));
        assert(isValidAbsoluteValue(auxSend));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourceAllParameters, src);

        sendToController(msg,5, FLOAT_T, pan, FLOAT_T, width, FLOAT_T, depth, FLOAT_T, elevation, FLOAT_T, auxSend);
    }


    void LisaControllerProxy::setSourceRelativePan(SourceId_t src, float value){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));
        assert(isValidRelativeValue(value));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourceRelativePan, src);

        sendToController(msg,1, FLOAT_T, value);
    }
    void LisaControllerProxy::setSourceRelativeWidth(SourceId_t src, float value){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));
        assert(isValidRelativeValue(value));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourceRelativeWidth, src);

        sendToController(msg,1, FLOAT_T, value);
    }
    void LisaControllerProxy::setSourceRelativeDistance(SourceId_t src, float value) {
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));
        assert(isValidRelativeValue(value));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourceRelativeDistance, src);

        sendToController(msg,1, FLOAT_T, value);
    }
    void LisaControllerProxy::setSourceRelativeElevation(SourceId_t src, float value){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));
        assert(isValidRelativeValue(value));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourceRelativeElevation, src);

        sendToController(msg,1, FLOAT_T, value);
    }
    void LisaControllerProxy::setSourceRelativePanSpread(SourceId_t src, float value){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));
        assert(isValidRelativeValue(value));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourceRelativePanSpread, src);

        sendToController(msg,1, FLOAT_T, value);
    }
    void LisaControllerProxy::setSourceRelativeAuxSend(SourceId_t src, float value){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));
        assert(isValidRelativeValue(value));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourceRelativeAuxSend, src);

        sendToController(msg,1, FLOAT_T, value);
    }

    void LisaControllerProxy::setSourceFxIntensity(SourceId_t src, FxId_t fx, float value){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));
        assert(isValidFxId(fx));
        assert(isValidAbsoluteValue(value));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourceFxIntensity, src, fx);

        sendToController(msg,1, FLOAT_T, value);
    }
    void LisaControllerProxy::setSourceFxActive(SourceId_t src, FxId_t fx, bool on){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));
        assert(isValidFxId(fx));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourceFxOn, src, fx);

        sendToController(msg,1, INT_T, on);
    }


    void LisaControllerProxy::setSelectedSourceRelativeAuxSend(float value){
        if (lastSelectedSource == 0){
            return;
        }
        setSourceRelativeAuxSend(lastSelectedSource, value);
    }


    void LisaControllerProxy::setSelectedSourcesRelativePan(float value) {
        if (!isRunning()){
            return;
        }
        assert(isValidRelativeValue(value));

        sendToController(kMsgSetSelectedSourcesRelativePan, 1, FLOAT_T, value);
    }

    void LisaControllerProxy::setSelectedSourcesRelativeWidth(float value) {
        if (!isRunning()){
            return;
        }
        assert(isValidRelativeValue(value));

        sendToController(kMsgSetSelectedSourcesRelativeWidth, 1, FLOAT_T, value);
    }

    void LisaControllerProxy::setSelectedSourcesRelativeDistance(float value) {
        if (!isRunning()){
            return;
        }
        assert(isValidRelativeValue(value));

        sendToController(kMsgSetSelectedSourcesRelativeDistance, 1, FLOAT_T, value);
    }

    void LisaControllerProxy::setSelectedSourcesRelativeElevation(float value) {
        if (!isRunning()){
            return;
        }
        assert(isValidRelativeValue(value));

        sendToController(kMsgSetSelectedSourcesRelativeElevation, 1, FLOAT_T, value);
    }

    void LisaControllerProxy::setSelectedSourcesRelativePanSpread(float value) {
        if (!isRunning()){
            return;
        }
        assert(isValidRelativeValue(value));

        sendToController(kMsgSetSelectedSourcesRelativePanSpread, 1, FLOAT_T, value);
    }


    // Source Solo, Snap, Delay

    void LisaControllerProxy::setSourceSolo(SourceId_t src, bool on){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourceSolo, src);

        sendToController(msg,1, INT_T, (int)on);
    }

    void LisaControllerProxy::setSelectedSourceSolo(bool on) {
        if (lastSelectedSource == 0){
            return;
        }
        setSourceSolo(lastSelectedSource, on);
    }

    void LisaControllerProxy::setSourceStaticDelayValue(SourceId_t src, float value){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));
        assert(0.0 <= value && value <= 200.0);

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourceStaticDelayValue, src);

        sendToController(msg,1, FLOAT_T, value);
    }

    void LisaControllerProxy::setSelectedSourceStaticDelayValue(float value) {
        if (lastSelectedSource == 0){
            return;
        }
        setSourceStaticDelayValue(lastSelectedSource, value);
    }

    void LisaControllerProxy::snapSourceToSpeaker(SourceId_t src){
        assert(isValidSourceId(src));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSnapSourceToSpeaker, src);

        sendToController(msg,0);
    }

    void LisaControllerProxy::snapSelectedSourceToSpeaker() {
        if (lastSelectedSource == 0){
            return;
        }
        snapSourceToSpeaker(lastSelectedSource);
    }


    // Source processing

    void LisaControllerProxy::setSourceOptGain(SourceId_t src, bool on){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourceOptGain, src);

        sendToController(msg,1, INT_T, (int)on);
    }
    void LisaControllerProxy::setSourceOptHpf(SourceId_t src, bool on){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourceOptHpf, src);

        sendToController(msg,1, INT_T, (int)on);
    }
    void LisaControllerProxy::setSourceOptDelayEnabled(SourceId_t src, bool on){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourceOptDelayEnabled, src);

        sendToController(msg,1, INT_T, (int)on);
    }
    void LisaControllerProxy::setSourceOptDelayMode(SourceId_t src, DelayMode_t mode){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));
        assert(isValidDelayMode(mode));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourceOptDelayMode, src);

        sendToController(msg,1, STRING_T, kDelayModes[mode]);
    }
    void LisaControllerProxy::setSourceOptReverbEarly(SourceId_t src, bool on){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourceOptReverbEarly, src);

        sendToController(msg,1, INT_T, (int)on);
    }
    void LisaControllerProxy::setSourceOptReverbCluster(SourceId_t src, bool on){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourceOptReverbCluster, src);

        sendToController(msg,1, INT_T, (int)on);
    }
    void LisaControllerProxy::setSourceOptReverbLate(SourceId_t src, bool on){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourceOptReverbLate, src);

        sendToController(msg,1, INT_T, (int)on);
    }
    void LisaControllerProxy::setSourceOptDirectSound(SourceId_t src, bool on){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourceOptDirectSound, src);

        sendToController(msg,1, INT_T, (int)on);
    }


    // Group parameters

    void LisaControllerProxy::setGroupPan(GroupId_t grp, float value){
        if (!isRunning()){
            return;
        }
        assert(isValidGroupId(grp));
        assert(isValidAbsoluteValue(value));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetGroupPan, grp);

        sendToController(msg,1, FLOAT_T, value);
    }
    void LisaControllerProxy::setGroupWidth(GroupId_t grp, float value){
        if (!isRunning()){
            return;
        }
        assert(isValidGroupId(grp));
        assert(isValidAbsoluteValue(value));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetGroupWidth, grp);

        sendToController(msg,1, FLOAT_T, value);
    }
    void LisaControllerProxy::setGroupDistance(GroupId_t grp, float value){
        if (!isRunning()){
            return;
        }
        assert(isValidGroupId(grp));
        assert(isValidAbsoluteValue(value));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetGroupDistance, grp);

        sendToController(msg,1, FLOAT_T, value);
    }
    void LisaControllerProxy::setGroupElevation(GroupId_t grp, float value){
        if (!isRunning()){
            return;
        }
        assert(isValidGroupId(grp));
        assert(isValidAbsoluteValue(value));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetGroupElevation, grp);

        sendToController(msg,1, FLOAT_T, value);
    }
    void LisaControllerProxy::setGroupPanSpread(GroupId_t grp, float value){
        if (!isRunning()){
            return;
        }
        assert(isValidGroupId(grp));
        assert(isValidAbsoluteValue(value));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetGroupPanSpread, grp);

        sendToController(msg,1, FLOAT_T, value);
    }
    void LisaControllerProxy::setGroupAuxSend(GroupId_t grp, float value){
        if (!isRunning()){
            return;
        }
        assert(isValidGroupId(grp));
        assert(isValidAbsoluteValue(value));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetGroupAuxSend, grp);

        sendToController(msg,1, FLOAT_T, value);
    }

    void LisaControllerProxy::setGroupRelativePan(GroupId_t grp, float value){
        if (!isRunning()){
            return;
        }
        assert(isValidGroupId(grp));
        assert(isValidRelativeValue(value));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetGroupRelativePan, grp);

        sendToController(msg,1, FLOAT_T, value);
    }
    void LisaControllerProxy::setGroupRelativeWidth(GroupId_t grp, float value){
        if (!isRunning()){
            return;
        }
        assert(isValidGroupId(grp));
        assert(isValidRelativeValue(value));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetGroupRelativeWidth, grp);

        sendToController(msg,1, FLOAT_T, value);
    }
    void LisaControllerProxy::setGroupRelativeDistance(GroupId_t grp, float value){
        if (!isRunning()){
            return;
        }
        assert(isValidGroupId(grp));
        assert(isValidRelativeValue(value));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetGroupRelativeDistance, grp);

        sendToController(msg,1, FLOAT_T, value);
    }
    void LisaControllerProxy::setGroupRelativeElevation(GroupId_t grp, float value){
        if (!isRunning()){
            return;
        }
        assert(isValidGroupId(grp));
        assert(isValidRelativeValue(value));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetGroupRelativeElevation, grp);

        sendToController(msg,1, FLOAT_T, value);
    }
    void LisaControllerProxy::setGroupRelativePanSpread(GroupId_t grp, float value){
        if (!isRunning()){
            return;
        }
        assert(isValidGroupId(grp));
        assert(isValidRelativeValue(value));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetGroupRelativePanSpread, grp);

        sendToController(msg,1, FLOAT_T, value);
    }
    void LisaControllerProxy::setGroupRelativeAuxSend(GroupId_t grp, float value){
        if (!isRunning()){
            return;
        }
        assert(isValidGroupId(grp));
        assert(isValidRelativeValue(value));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetGroupRelativeAuxSend, grp);

        sendToController(msg,1, FLOAT_T, value);
    }

    // Snapshots

    void LisaControllerProxy::fireSnapshot(SnapshotId_t snapshot){
        if (!isRunning()){
            return;
        }
        assert(isValidSnapshotId(snapshot));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgFireSnapshot, snapshot);

        sendToController(msg,0);
    }
    void LisaControllerProxy::firePreviousSnapshot() {
        if (!isRunning()){
            return;
        }

        sendToController(kMsgFirePreviousSnapshot, 0);
    }
    void LisaControllerProxy::fireNextSnapshot() {
        if (!isRunning()){
            return;
        }

        sendToController(kMsgFireNextSnapshot, 0);
    }
    void LisaControllerProxy::refireCurrentSnapshot() {
        if (!isRunning()){
            return;
        }

        sendToController(kMsgRefireCurrentSnapshot, 0);
    }
    void LisaControllerProxy::saveCurrentSnapshot() {
        if (!isRunning()){
            return;
        }

        sendToController(kMsgSaveCurrentSnapshot, 0);
    }
    void LisaControllerProxy::saveAsNewSnapshot() {
        if (!isRunning()){
            return;
        }

        sendToController(kMsgSaveAsNewSnapshot, 0);
    }

    // Reverbs

    void LisaControllerProxy::loadReverbPreset(ReverbId_t reverb){
        if (!isRunning()){
            return;
        }
        assert(isValidReverbId(reverb));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgLoadReverbPreset, reverb);

        sendToController(msg,0);
    }

    // FX

    void LisaControllerProxy::startFx(FxId_t fx){
        if (!isRunning()){
            return;
        }
        assert(isValidFxId(fx));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgStartFx, fx);

        sendToController(msg,0);
    }
    void LisaControllerProxy::restartFx(FxId_t fx){
        if (!isRunning()){
            return;
        }
        assert(isValidFxId(fx));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgRestartFx, fx);

        sendToController(msg,0);
    }
    void LisaControllerProxy::stopFx(FxId_t fx){
        if (!isRunning()){
            return;
        }
        assert(isValidFxId(fx));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgStopFx, fx);

        sendToController(msg,0);
    }

    // BPM

    void LisaControllerProxy::lockBPMToMidiClock(bool on){
        if (!isRunning()){
            return;
        }

        sendToController(kMsgLockBpmToMidiClock, 1, INT_T, (int)on);
    }
    void LisaControllerProxy::setBPM(float bpm){
        if (!isRunning()){
            return;
        }
        assert(isValidBpm(bpm));

        sendToController(kMsgSetBpm, 1, FLOAT_T, bpm);
    }
    void LisaControllerProxy::tapTempo() {
        if (!isRunning()){
            return;
        }

        sendToController(kMsgBpmTap, 0);
    }

    // Master Fader

    void LisaControllerProxy::setMasterGain(float gain) {
        if (!isRunning()){
            return;
        }
        assert(isValidGain(gain));

        sendToController(kMsgSetMasterGain, 1, FLOAT_T, gain);
    }

    void LisaControllerProxy::setMasterFaderPos(float pos) {
        if (!isRunning()){
            return;
        }
        assert(isValidFaderPos(pos));

        sendToController(kMsgSetMasterFaderPos, 1, FLOAT_T, pos);
    }

    void LisaControllerProxy::setMasterMute(bool on) {
        if (!isRunning()){
            return;
        }

        sendToController(kMsgSetMasterMute, 1, INT_T, (int)on);
    }


    // Reverb Fader

    void LisaControllerProxy::setReverbGain(float gain) {
        if (!isRunning()){
            return;
        }
        assert(isValidGain(gain));

        sendToController(kMsgSetReverbGain, 1, FLOAT_T, gain);
    }

    void LisaControllerProxy::setReverbFaderPos(float pos) {
        if (!isRunning()){
            return;
        }
        assert(isValidFaderPos(pos));

        sendToController(kMsgSetReverbFaderPos, 1, FLOAT_T, pos);
    }

    void LisaControllerProxy::setReverbMute(bool on) {
        if (!isRunning()){
            return;
        }

        sendToController(kMsgSetReverbMute, 1, INT_T, (int)on);
    }


    // Monitor Fader

    void LisaControllerProxy::setMonitorGain(float gain){
        if (!isRunning()){
            return;
        }
        assert(isValidGain(gain));

        sendToController(kMsgSetMonitorGain, 1, FLOAT_T, gain);
    }
    void LisaControllerProxy::setMonitorFaderPos(float pos)  {
        if (!isRunning()){
            return;
        }
        assert(isValidFaderPos(pos));

        sendToController(kMsgSetMonitorFaderPos, 1, FLOAT_T, pos);
    }
    void LisaControllerProxy::setMonitorMute(bool on) {
        if (!isRunning()){
            return;
        }

        sendToController(kMsgSetMonitorMute, 1, INT_T, (int)on);
    }

    // User Fader

    void LisaControllerProxy::setUserFaderNGain(int fader, float gain) {
        if (!isRunning()){
            return;
        }
        assert(1 <= fader && fader <= 2);
        assert(isValidGain(gain));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetUserFaderNGain, fader);

        sendToController(msg, 1, FLOAT_T, gain);
    }
    void LisaControllerProxy::setUserFaderNPos(int fader, float pos) {
        if (!isRunning()){
            return;
        }
        assert(1 <= fader && fader <= 2);
        assert(isValidFaderPos(pos));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetUserFaderNPos, fader);

        sendToController(msg, 1, FLOAT_T, pos);
    }
    void LisaControllerProxy::setUserFaderNMute(int fader, bool on) {
        if (!isRunning()){
            return;
        }
        assert(1 <= fader && fader <= 2);

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetUserFaderNMute, fader);

        sendToController(msg, 1, INT_T, (int)on);
    }


    // Source + Group selection

    void LisaControllerProxy::selectSource(SourceId_t src){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSelectionToSource, src);

        sendToController(msg, 0);

        lastSelectedSource = src;
    }

    void LisaControllerProxy::addSourceToSelection(SourceId_t src){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgChangeSelectionOfSource, src);

        sendToController(msg, 1, INT_T, 1);

        lastSelectedSource = 0;
    }

    void LisaControllerProxy::removeSourceFromSelection(SourceId_t src){
        if (!isRunning()){
            return;
        }
        assert(isValidSourceId(src));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgChangeSelectionOfSource, src);

        sendToController(msg, 1, INT_T, 0);

        lastSelectedSource = 0;
    }

    void LisaControllerProxy::selectGroup(GroupId_t grp){
        if (!isRunning()){
            return;
        }
        assert(isValidGroupId(grp));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSelectionToGroup, grp);

        sendToController(msg, 0);

        lastSelectedSource = 0;
    }

    void LisaControllerProxy::clearSelection() {
        if (!isRunning()){
            return;
        }

        sendToController(kMsgClearSelection, 0);

        lastSelectedSource = 0;
    }

    // Headtracker

    void LisaControllerProxy::setHeadtrackerOrientation(float yaw, float pitch, float roll){
        if (!isRunning()){
            return;
        }
        assert(isValidYaw(yaw));
        assert(isValidPitch(pitch));
        assert(isValidRoll(roll));

        sendToController(kMsgSetHeadtrackerOrientation, 3, FLOAT_T, yaw, FLOAT_T, pitch, FLOAT_T, roll);
    }
    void LisaControllerProxy::resetHeadtracker(){
        if (!isRunning()){
            return;
        }

        sendToController(kMsgResetHeadtracker, 0);
    }
    void LisaControllerProxy::setHeadtrackerType(HeadtrackerType_t type){
        if (!isRunning()){
            return;
        }
        assert(isValidHeadtrackerType(type));

        sendToController(kMsgSetHeadtrackerType, 1, STRING_T, kHeadtrackerTypes[type]);
    }


    // OSC Devices

    void LisaControllerProxy::registerDevice(DeviceId_t device, const char * ipAddress, unsigned short port){
        if (!isRunning()){
            return;
        }
        assert(isValidDeviceId(device));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgRegisterDevice, device);

        sendToController(msg, 2, STRING_T, ipAddress, INT_T, port);
    }
    void LisaControllerProxy::unregisterDevice(DeviceId_t device){
        if (!isRunning()){
            return;
        }
        assert(isValidDeviceId(device));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgDeleteDevice, device);

        sendToController(msg, 0);
    }
    void LisaControllerProxy::setDeviceName(DeviceId_t device, const char name[]){
        if (!isRunning()){
            return;
        }
        assert(isValidDeviceId(device));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetDeviceName, device);

        sendToController(msg, 1, STRING_T, name);
    }
    void LisaControllerProxy::enableSendingToDevice(DeviceId_t device, bool enable){
        if (!isRunning()){
            return;
        }
        assert(isValidDeviceId(device));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgEnableSendingToDevice, device);

        sendToController(msg, 1, INT_T, (int)enable);
    }
    void LisaControllerProxy::enableReceivingFromDevice(DeviceId_t device, bool enable){
        if (!isRunning()){
            return;
        }
        assert(isValidDeviceId(device));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgEnableReceivingFromDevice, device);

        sendToController(msg, 1, INT_T, (int)enable);
    }
    void LisaControllerProxy::setDeviceCoordFormat(DeviceId_t device, CoordFormat_t format){
        if (!isRunning()){
            return;
        }
        assert(isValidDeviceId(device));
        assert(isValidCoordFormat(format));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetDeviceCoordFormat, device);

        sendToController(msg, 1, STRING_T, kCoordFormats[format]);
    }
    void LisaControllerProxy::setMasterGainControl(DeviceId_t device, bool on){
        if (!isRunning()){
            return;
        }
        assert(isValidDeviceId(device));

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetMasterGainControl, device);

        sendToController(msg, 1, INT_T, (int)on);
    }

    // ping

    void LisaControllerProxy::ping(const char ipAddress[], unsigned short port){
        if (!isRunning()){
            return;
        }

        sendToController(kMsgPing, 2, STRING_T, ipAddress, INT_T, port);
    }

} // LisaDeskbridge