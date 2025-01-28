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

#include "osc/OscReceivedElements.h"
#include "osc/OscOutboundPacketStream.h"

#ifndef OUTPUT_BUFFER_SIZE
#define OUTPUT_BUFFER_SIZE 512
#endif


namespace LisaDeskbridge {

    //// Messages from Controller to External Device

    constexpr char kMsgRxMasterGain[]      = "/ext/master/gain";           // 0.0 - 1.0
    constexpr char kMsgRxMasterFaderPos[]  = "/ext/master/faderpos";       // 0.0 - 1.0

    constexpr char kMsgRxReverbGain[]      = "/ext/rev/master/gain";       // 0.0 - 1.0
    constexpr char kMsgRxReverbFaderPos[]  = "/ext/rev/master/faderpos";   // 0.0 - 1.0

    constexpr char kMsgRxSourcePan[]                   = "/ext/src/%u/p%n"; // 0.0 - 1.0
    constexpr char kMsgRxSourceWidth[]                 = "/ext/src/%u/w%n"; // 0.0 - 1.0
    constexpr char kMsgRxSourceDistance[]              = "/ext/src/%u/d%n"; // 0.0 - 1.0
    constexpr char kMsgRxSourceElevation[]             = "/ext/src/%u/e%n"; // 0.0 - 1.0
    constexpr char kMsgRxSourceAuxSend[]               = "/ext/src/%u/s%n"; // 0.0 - 1.0

    //// Messages from External Device to Controller

    // Source control flags

    constexpr char kMsgSetSourceControlPan[]            = "/ext/flag/src/%u/p"; // "off", "snap", "plug", "ext", "trkr"
    constexpr char kMsgSetSourceControlWidth[]          = "/ext/flag/src/%u/w"; // "off", "snap", "plug", "ext", "trkr"
    constexpr char kMsgSetSourceControlDistance[]       = "/ext/flag/src/%u/d"; // "off", "snap", "plug", "ext", "trkr"
    constexpr char kMsgSetSourceControlElevation[]      = "/ext/flag/src/%u/e"; // "off", "snap", "plug", "ext", "trkr"
    constexpr char kMsgSetSourceControlSub[]            = "/ext/flag/src/%u/s"; // "off", "snap", "plug", "ext"

    constexpr char kMsgSetAllSourcesControlPan[]            = "/ext/flag/src/*/p"; // "off", "snap", "plug", "ext", "trkr"
    constexpr char kMsgSetAllSourcesControlWidth[]          = "/ext/flag/src/*/w"; // "off", "snap", "plug", "ext", "trkr"
    constexpr char kMsgSetAllSourcesControlDistance[]       = "/ext/flag/src/*/d"; // "off", "snap", "plug", "ext", "trkr"
    constexpr char kMsgSetAllSourcesControlElevation[]      = "/ext/flag/src/*/e"; // "off", "snap", "plug", "ext", "trkr"
    constexpr char kMsgSetAllSourcesControlSub[]            = "/ext/flag/src/*/s"; // "off", "snap", "plug", "ext"

    // Source parameters

    constexpr char kMsgSetSourcePan[]                   = "/ext/src/%u/p"; // 0.0 - 1.0
    constexpr char kMsgSetSourceWidth[]                 = "/ext/src/%u/w"; // 0.0 - 1.0
    constexpr char kMsgSetSourceDistance[]              = "/ext/src/%u/d"; // 0.0 - 1.0
    constexpr char kMsgSetSourceElevation[]             = "/ext/src/%u/e"; // 0.0 - 1.0
    constexpr char kMsgSetSourcePanSpread[]             = "/ext/src/%u/v"; // 0.0 - 1.0
    constexpr char kMsgSetSourceAuxSend[]               = "/ext/src/%u/s"; // 0.0 - 1.0

    constexpr char kMsgSetSourceAllParameters[]                 = "/ext/src/%d/pwdes"; // ...

    constexpr char kMsgSetSourceRelativePan[]                   = "/ext/src/%u/rp"; // -1.0 - 1.0
    constexpr char kMsgSetSourceRelativeWidth[]                 = "/ext/src/%u/rw"; // -1.0 - 1.0
    constexpr char kMsgSetSourceRelativeDistance[]              = "/ext/src/%u/rd"; // -1.0 - 1.0
    constexpr char kMsgSetSourceRelativeElevation[]             = "/ext/src/%u/re"; // -1.0 - 1.0
    constexpr char kMsgSetSourceRelativePanSpread[]             = "/ext/src/%u/rv"; // -1.0 - 1.0
    constexpr char kMsgSetSourceRelativeAuxSend[]               = "/ext/src/%u/rs"; // -1.0 - 1.0

    constexpr char kMsgSetSourceFxIntensity[]                   = "/ext/src/%u/fx/%u/intensity"; // 0.0 - 1.0
    constexpr char kMsgSetSourceFxOn[]                          = "/ext/src/%u/fx/%u/active";    // 0, 1

    constexpr char kMsgSetSelectedSourcesRelativePan[]          = "/ext/selsrc/rp"; // -1.0 - 1.0
    constexpr char kMsgSetSelectedSourcesRelativeWidth[]        = "/ext/selsrc/rw"; // -1.0 - 1.0
    constexpr char kMsgSetSelectedSourcesRelativeDistance[]     = "/ext/selsrc/rd"; // -1.0 - 1.0
    constexpr char kMsgSetSelectedSourcesRelativeElevation[]    = "/ext/selsrc/re"; // -1.0 - 1.0
    constexpr char kMsgSetSelectedSourcesRelativePanSpread[]    = "/ext/selsrc/rv"; // -1.0 - 1.0

    // Source Solo, Snap, Delay

    constexpr char kMsgSetSourceSolo[]              = "/ext/solo/src/%u";
    constexpr char kMsgSetSourceStaticDelayValue[]  = "/ext/delayms/src/%u";
    constexpr char kMsgSnapSourceToSpeaker[]        = "/ext/spksnap/src/%u";

    // Source processing

    constexpr char kMsgSetSourceOptGain[]           = "/ext/config/src/%u/distatt/gain";    // 0, 1
    constexpr char kMsgSetSourceOptHpf[]            = "/ext/config/src/%u/distatt/hpf";     // 0, 1
    constexpr char kMsgSetSourceOptDelayEnabled[]   = "/ext/config/src/%u/delay/enable";    // 0, 1
    constexpr char kMsgSetSourceOptDelayMode[]      = "/ext/config/src/%u/delay/mode";      // "static", "dynamic"
    constexpr char kMsgSetSourceOptReverbEarly[]    = "/ext/config/src/%u/reverb/early";    // 0, 1
    constexpr char kMsgSetSourceOptReverbCluster[]  = "/ext/config/src/%u/reverb/cluster";  // 0, 1
    constexpr char kMsgSetSourceOptReverbLate[]     = "/ext/config/src/%u/reverb/late";     // 0, 1
    constexpr char kMsgSetSourceOptDirectSound[]    = "/ext/config/src/%u/direct";          // 0, 1

    // Group parameters

    constexpr char kMsgSetGroupPan[]        = "/ext/grp/%u/p"; //-1.0 - 1.0
    constexpr char kMsgSetGroupWidth[]      = "/ext/grp/%u/w"; //-1.0 - 1.0
    constexpr char kMsgSetGroupDistance[]   = "/ext/grp/%u/d"; //-1.0 - 1.0
    constexpr char kMsgSetGroupElevation[]  = "/ext/grp/%u/e"; //-1.0 - 1.0
    constexpr char kMsgSetGroupAuxLevel[]   = "/ext/grp/%u/a"; //-1.0 - 1.0
    constexpr char kMsgSetGroupPanSpread[]  = "/ext/grp/%u/v"; //0.0 - 1.0

    constexpr char kMsgSetGroupRelativePan[]        = "/ext/grp/%u/rp"; //-1.0 - 1.0
    constexpr char kMsgSetGroupRelativeWidth[]      = "/ext/grp/%u/rw"; //-1.0 - 1.0
    constexpr char kMsgSetGroupRelativeDistance[]   = "/ext/grp/%u/rd"; //-1.0 - 1.0
    constexpr char kMsgSetGroupRelativeElevation[]  = "/ext/grp/%u/re"; //-1.0 - 1.0
    constexpr char kMsgSetGroupRelativeAuxLevel[]   = "/ext/grp/%u/ra"; //-1.0 - 1.0

    //?? parameter range seems wrong according to docs
    constexpr char kMsgSetGroupRelativePanSpread[]  = "/ext/grp/%u/rv"; //0.0 - 1.0


    // Snapshots

    constexpr char kMsgFireSnapshot[]           = "/ext/snap/%u/f";
    constexpr char kMsgFirePreviousSnapshot[]   = "/ext/snap/p/f";
    constexpr char kMsgFireNextSnapshot[]       = "/ext/snap/n/f";
    constexpr char kMsgRefireCurrentSnapshot[]  = "/ext/snap/c/f";
    constexpr char kMsgSaveCurrentSnapshot[]    = "/ext/snap/c/s";
    constexpr char kMsgSaveAsNewSnapshot[]      = "/ext/snap/sn";

    // Reverbs

    constexpr char kMsgLoadReverbPreset[]   = "/ext/rev/%k/l";

    // FX

    constexpr char kMsgStartFx[]    = "/ext/fx/%u/start";
    constexpr char kMsgRestartFx[]  = "/ext/fx/%u/restart";
    constexpr char kMsgStopFx[]     = "/ext/fx/%u/stop";

    // BPM

    constexpr char kMsgLockBpmToMidiClock[] = "/ext/tempo/sync"; // 0, 1
    constexpr char kMsgSetBpm[]             = "/ext/tempo/bpm"; // 30.0 - 300.0
    constexpr char kMsgBpmTap[]             = "/ext/tempo/tap";

    // Master Fader

    constexpr char kMsgSetMasterGain[]      = "/ext/master/gain";           // 0.0 - 1.0
    constexpr char kMsgSetMasterFaderPos[]  = "/ext/master/faderpos";       // 0.0 - 1.0
    constexpr char kMsgSetMasterMute[]      = "/ext/master/mute";           // 0, 1

    // Reverb Fader

    constexpr char kMsgSetReverbGain[]      = "/ext/rev/master/gain";       // 0.0 - 1.0
    constexpr char kMsgSetReverbFaderPos[]  = "/ext/rev/master/faderpos";   // 0.0 - 1.0
    constexpr char kMsgSetReverbMute[]      = "/ext/rev/master/mute";       // 0, 1

    // Monitoring Fader

    constexpr char kMsgSetMonitorGain[]      = "/ext/mon/gain";           // 0.0 - 1.0
    constexpr char kMsgSetMonitorFaderPos[]  = "/ext/mon/faderpos";       // 0.0 - 1.0
    constexpr char kMsgSetMonitorMute[]      = "/ext/mon/mute";           // 0, 1

    // User Fader

    constexpr char kMsgSetUserFaderGain[]      = "/ext/fader%u/gain";           // 0.0 - 1.0
    constexpr char kMsgSetUserFaderPos[]       = "/ext/fader%u/faderpos";       // 0.0 - 1.0
    constexpr char kMsgSetUserFaderMute[]      = "/ext/fader%u/mute";           // 0, 1

    // Source selection

    constexpr char kMsgChangeSelectionOfSource[]    = "/ext/sel/src/%u";
    constexpr char kMsgSetSelectionToSource[]       = "/ext/sel/set/src/%u";
    constexpr char kMsgClearSelection[]             = "/ext/sel/set/none";

    // Group selection

    constexpr char kMsgSetSelectionToGroup[]        = "/ext/sel/set/grp/%u";

    // Headtracker

    constexpr char kMsgSetHeadtrackerOrientation[]  = "/ht/ypr"; // in radians: yaw [-3.141 - 3.141], pitch [-1.6 - 1.6], roll [-3.141 - 3.141]
    constexpr char kMsgResetHeadtracker[]           = "/ht/reset";
    constexpr char kMsgSetHeadtrackerType[]         = "/ht/type"; // "off", "midi", "osc"

    // OSC Devices

    constexpr char kMsgRegisterDevice[]             = "/ext/device/%u/register";    // "ipAddress" port
    constexpr char kMsgDeleteDevice[]               = "/ext/device/%u/delete";
    constexpr char kMsgSetDeviceName[]              = "/ext/device/%u/name";        // "name"
    constexpr char kMsgEnableSendingToDevice[]      = "/ext/device/%u/send";        // 0 = off, 1 = on
    constexpr char kMsgEnableReceivingFromDevice[]  = "/ext/device/%u/receive";     // 0 = off, 1 = on

    // ping

    constexpr char kMsgPing[]       = "/ext/ping"; // "ip" port


    void LisaControllerProxy::start(unsigned short listenPort, std::basic_string_view<char> & controllerAddress, unsigned short controllerPort){
        if (mIsRunning){
            return;
        }

        std::cerr << "Listening for L-ISA Controller messages on port " << listenPort << std::endl;

        udpListeningReceiveSocket = new UdpListeningReceiveSocket(
                IpEndpointName( IpEndpointName::ANY_ADDRESS, listenPort),
                this
                );

        thread = new std::thread([](UdpListeningReceiveSocket * socket){
            socket->Run();
        }, udpListeningReceiveSocket);

        std::cerr << "Sending to L-ISA Controller on host " << controllerAddress << " on port " << controllerPort << std::endl;

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

                std::cerr << "Received unknown packet: " << m.AddressPattern() << std::endl;
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
            if (type == BOOL_T){
                bool b = va_arg(args, int);
                msg << b;
            } else if (type == INT_T){
                int i = va_arg(args, int);
                msg << i;
            } else if (type == FLOAT_T){
                float d = va_arg(args, double);
                msg << d;
            } else {
                assert(false); // should not happen ...
            }
        }
        va_end(args);

        msg << osc::EndMessage;

        udpTransmitSocket->Send(msg.Data(), msg.Size());
    }

    void LisaControllerProxy::selectSource(SourceId_t id){
        assert(1 <= id && id <= 96);

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSelectionToSource, id);

        sendToController(msg, 0);

        lastSelectedSource = id;
    }

    void LisaControllerProxy::addSourceToSelection(SourceId_t id){
        assert(1 <= id && id <= 96);

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgChangeSelectionOfSource, id);

        sendToController(msg, 1, INT_T, 1);

        lastSelectedSource = 0;
    }

    void LisaControllerProxy::removeSourceFromSelection(SourceId_t id){
        assert(1 <= id && id <= 96);

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgChangeSelectionOfSource, id);

        sendToController(msg, 1, INT_T, 0);

        lastSelectedSource = 0;
    }

    void LisaControllerProxy::selectGroup(GroupId_t id){
        assert(1 <= id && id <= 96);

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSelectionToGroup, id);

        sendToController(msg, 0);

        lastSelectedSource = 0;
    }

    void LisaControllerProxy::clearSelection() {
        sendToController(kMsgClearSelection, 0);

        lastSelectedSource = 0;
    }

    void LisaControllerProxy::setSelectedSourcesRelativePan(float rpan) {
        assert(isRunning());
        assert(-1.0 <= rpan && rpan <= 1.0);

        sendToController(kMsgSetSelectedSourcesRelativePan, 1, FLOAT_T, rpan);
    }

    void LisaControllerProxy::setSelectedSourcesRelativeWidth(float rwidth) {
        assert(isRunning());
        assert(-1.0 <= rwidth && rwidth <= 1.0);

        sendToController(kMsgSetSelectedSourcesRelativeWidth, 1, FLOAT_T, rwidth);
    }

    void LisaControllerProxy::setSelectedSourcesRelativeDistance(float rdist) {
        assert(isRunning());
        assert(-1.0 <= rdist && rdist <= 1.0);

        sendToController(kMsgSetSelectedSourcesRelativeDistance, 1, FLOAT_T, rdist);
    }

    void LisaControllerProxy::setSelectedSourcesRelativeElevation(float relev) {
        assert(isRunning());
        assert(-1.0 <= relev && relev <= 1.0);

        sendToController(kMsgSetSelectedSourcesRelativeElevation, 1, FLOAT_T, relev);
    }

    void LisaControllerProxy::setSelectedSourcesRelativePanSpread(float rspread) {
        assert(isRunning());
        assert(-1.0 <= rspread && rspread <= 1.0);

        sendToController(kMsgSetSelectedSourcesRelativePanSpread, 1, FLOAT_T, rspread);
    }

    void LisaControllerProxy::setSourceRelativeAuxSend(SourceId_t id, float rsend){
        assert(isRunning());
        assert(1 <= id && id <= 96);
        assert(-1.0 <= rsend && rsend <= 1.0);

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourceRelativeAuxSend, id);

        sendToController(msg,1, FLOAT_T, rsend);
    }

    void LisaControllerProxy::setSelectedSourceRelativeAuxSend(float rsend){
        if (lastSelectedSource == 0){
            return;
        }
        setSourceRelativeAuxSend(lastSelectedSource, rsend);
    }

    void LisaControllerProxy::setSourceSolo(SourceId_t id, bool on){
        assert(1 <= id && id <= 96);

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSetSourceSolo, id);

        sendToController(msg,1, INT_T, (int)on);
    }

    void LisaControllerProxy::setSelectedSourceSolo(bool on) {
        if (lastSelectedSource == 0){
            return;
        }
        setSourceSolo(lastSelectedSource, on);
    }

    void LisaControllerProxy::snapSourceToSpeaker(SourceId_t id){
        assert(1 <= id && id <= 96);

        char msg[64];
        int l = std::snprintf(msg, sizeof(msg), (char*)kMsgSnapSourceToSpeaker, id);

        sendToController(msg,0);
    }

    void LisaControllerProxy::snapSelectedSourceToSpeaker() {
        if (lastSelectedSource == 0){
            return;
        }
        snapSourceToSpeaker(lastSelectedSource);
    }

    void LisaControllerProxy::setMasterGain(float gain) {
        assert(isRunning());
        assert(0.0 <= gain && gain <= 1.0);

        sendToController(kMsgSetMasterGain, 1, FLOAT_T, gain);
    }

    void LisaControllerProxy::setMasterFaderPos(float pos) {
        assert(isRunning());
        assert(0.0 <= pos && pos <= 1.0);

        sendToController(kMsgSetMasterFaderPos, 1, FLOAT_T, pos);
    }

    void LisaControllerProxy::setMasterMute(bool on) {
        assert(isRunning());

        sendToController(kMsgSetMasterMute, 1, INT_T, (int)on);
    }

    void LisaControllerProxy::setReverbGain(float gain) {
        assert(isRunning());
        assert(0.0 <= gain && gain <= 1.0);

        sendToController(kMsgSetReverbGain, 1, FLOAT_T, gain);
    }

    void LisaControllerProxy::setReverbFaderPos(float pos) {
        assert(isRunning());
        assert(0.0 <= pos && pos <= 1.0);

        sendToController(kMsgSetReverbFaderPos, 1, FLOAT_T, pos);
    }

    void LisaControllerProxy::setReverbMute(bool on) {
        assert(isRunning());

        sendToController(kMsgSetReverbMute, 1, INT_T, (int)on);
    }

} // LisaDeskbridge