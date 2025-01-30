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

#ifndef LISA_DESKBRIDGE_LISACONTROLLER_H
#define LISA_DESKBRIDGE_LISACONTROLLER_H

namespace LisaDeskbridge {

    constexpr uint16_t kLisaControllerPortDefault                       = 8880;
    constexpr std::basic_string_view<char> kLisaControllerHostDefault   = "127.0.0.1";

    constexpr uint16_t kRemotePortDefault                               = 9000;

    typedef unsigned int SourceId_t;
    typedef unsigned int GroupId_t;
    typedef unsigned int DeviceId_t;
    typedef unsigned int FxId_t;
    typedef unsigned int SnapshotId_t;
    typedef unsigned int ReverbId_t;

    inline bool isValidSourceId(SourceId_t id){
        return (1 <= id && id <= 96);
    }

    inline bool isValidGroupId(GroupId_t id){
        return (1 <= id && id <= 96);
    }

    inline bool isValidDeviceId(DeviceId_t id){
        return (1 <= id && id <= 10);
    }

    inline bool isValidFxId(FxId_t id){
        return (1 <= id);
    }

    inline bool isValidSnapshotId(SnapshotId_t id){
        return (1 <= id);
    }

    inline bool isValidReverbId(SnapshotId_t id){
        return (1 <= id);
    }

    inline bool isValidAbsoluteValue(float value){
        return (0.0 <= value && value <= 1.0);
    }

    inline bool isValidRelativeValue(float value){
        return (-1.0 <= value && value <= 1.0);
    }


    //// Messages from Controller to External Device

    constexpr char kMsgRxMasterGain[]                   = "/ext/master/gain";           // 0.0 - 1.0
    constexpr char kMsgRxMasterFaderPos[]               = "/ext/master/faderpos";       // 0.0 - 1.0

    constexpr char kMsgRxReverbGain[]                   = "/ext/rev/master/gain";       // 0.0 - 1.0
    constexpr char kMsgRxReverbFaderPos[]               = "/ext/rev/master/faderpos";   // 0.0 - 1.0

    constexpr char kMsgRxSourcePan[]                    = "/ext/src/%u/p%n"; // 0.0 - 1.0
    constexpr char kMsgRxSourceWidth[]                  = "/ext/src/%u/w%n"; // 0.0 - 1.0
    constexpr char kMsgRxSourceDistance[]               = "/ext/src/%u/d%n"; // 0.0 - 1.0
    constexpr char kMsgRxSourceElevation[]              = "/ext/src/%u/e%n"; // 0.0 - 1.0
    constexpr char kMsgRxSourceAuxSend[]                = "/ext/src/%u/s%n"; // 0.0 - 1.0

    //// Messages from External Device to Controller

    // Source control flags

    constexpr char kMsgSetSourceControlPan[]            = "/ext/flag/src/%u/p"; // "off", "snap", "plug", "ext", "trkr"
    constexpr char kMsgSetSourceControlWidth[]          = "/ext/flag/src/%u/w"; // "off", "snap", "plug", "ext", "trkr"
    constexpr char kMsgSetSourceControlDistance[]       = "/ext/flag/src/%u/d"; // "off", "snap", "plug", "ext", "trkr"
    constexpr char kMsgSetSourceControlElevation[]      = "/ext/flag/src/%u/e"; // "off", "snap", "plug", "ext", "trkr"
    constexpr char kMsgSetSourceControlAuxSend[]            = "/ext/flag/src/%u/s"; // "off", "snap", "plug", "ext"

    constexpr char kControlFlagOff[]    = "off";
    constexpr char kControlFlagSnap[]   = "snap";
    constexpr char kControlFlagPlugin[] = "plug";
    constexpr char kControlFlagExt[]    = "ext";
    constexpr char kControlFlagTrKr[]   = "trkr";

    constexpr const char* kControlFlags[] = {kControlFlagOff, kControlFlagSnap, kControlFlagPlugin, kControlFlagExt, kControlFlagTrKr};

    enum ControlFlag_t {ControlFlagOff = 0, ControlFlagSnap = 1, ControlFlagPlug = 2, ControlFlagExt = 3, ControlFlagTrKr = 4};

    inline bool isValidControlFlag(ControlFlag_t flag){
        return (flag == ControlFlagOff || flag == ControlFlagSnap || flag == ControlFlagPlug || flag == ControlFlagExt || flag == ControlFlagTrKr);
    }

//    constexpr char kMsgSetAllSourcesControlPan[]            = "/ext/flag/src/*/p"; // "off", "snap", "plug", "ext", "trkr"
//    constexpr char kMsgSetAllSourcesControlWidth[]          = "/ext/flag/src/*/w"; // "off", "snap", "plug", "ext", "trkr"
//    constexpr char kMsgSetAllSourcesControlDistance[]       = "/ext/flag/src/*/d"; // "off", "snap", "plug", "ext", "trkr"
//    constexpr char kMsgSetAllSourcesControlElevation[]      = "/ext/flag/src/*/e"; // "off", "snap", "plug", "ext", "trkr"
//    constexpr char kMsgSetAllSourcesControlSub[]            = "/ext/flag/src/*/s"; // "off", "snap", "plug", "ext"

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

    constexpr char kDelayModeStatic[]   = "static";
    constexpr char kDelayModeDynamic[]  = "dynamic";

    constexpr const char * kDelayModes[] = {kDelayModeStatic, kDelayModeDynamic};

    enum DelayMode_t {DelayModeStatic = 0, DelayModeDynamic = 1};

    inline bool isValidDelayMode(DelayMode_t mode){
        return (mode == DelayModeStatic || mode == DelayModeDynamic);
    }

    constexpr char kMsgSetSourceOptReverbEarly[]    = "/ext/config/src/%u/reverb/early";    // 0, 1
    constexpr char kMsgSetSourceOptReverbCluster[]  = "/ext/config/src/%u/reverb/cluster";  // 0, 1
    constexpr char kMsgSetSourceOptReverbLate[]     = "/ext/config/src/%u/reverb/late";     // 0, 1
    constexpr char kMsgSetSourceOptDirectSound[]    = "/ext/config/src/%u/direct";          // 0, 1

    // Group parameters

    constexpr char kMsgSetGroupPan[]        = "/ext/grp/%u/p"; //-1.0 - 1.0
    constexpr char kMsgSetGroupWidth[]      = "/ext/grp/%u/w"; //-1.0 - 1.0
    constexpr char kMsgSetGroupDistance[]   = "/ext/grp/%u/d"; //-1.0 - 1.0
    constexpr char kMsgSetGroupElevation[]  = "/ext/grp/%u/e"; //-1.0 - 1.0
    constexpr char kMsgSetGroupAuxSend[]   = "/ext/grp/%u/a"; //-1.0 - 1.0
    constexpr char kMsgSetGroupPanSpread[]  = "/ext/grp/%u/v"; //0.0 - 1.0

    constexpr char kMsgSetGroupRelativePan[]        = "/ext/grp/%u/rp"; //-1.0 - 1.0
    constexpr char kMsgSetGroupRelativeWidth[]      = "/ext/grp/%u/rw"; //-1.0 - 1.0
    constexpr char kMsgSetGroupRelativeDistance[]   = "/ext/grp/%u/rd"; //-1.0 - 1.0
    constexpr char kMsgSetGroupRelativeElevation[]  = "/ext/grp/%u/re"; //-1.0 - 1.0
    constexpr char kMsgSetGroupRelativeAuxSend[]   = "/ext/grp/%u/ra"; //-1.0 - 1.0
    constexpr char kMsgSetGroupRelativePanSpread[]  = "/ext/grp/%u/rv"; //-1.0 - 1.0


    // Snapshots

    constexpr char kMsgFireSnapshot[]           = "/ext/snap/%u/f";
    constexpr char kMsgFirePreviousSnapshot[]   = "/ext/snap/p/f";
    constexpr char kMsgFireNextSnapshot[]       = "/ext/snap/n/f";
    constexpr char kMsgRefireCurrentSnapshot[]  = "/ext/snap/c/f";
    constexpr char kMsgSaveCurrentSnapshot[]    = "/ext/snap/c/s";
    constexpr char kMsgSaveAsNewSnapshot[]      = "/ext/snap/sn";

    // Reverbs

    constexpr char kMsgLoadReverbPreset[]   = "/ext/rev/%u/l";

    // FX

    constexpr char kMsgStartFx[]    = "/ext/fx/%u/start";
    constexpr char kMsgRestartFx[]  = "/ext/fx/%u/restart";
    constexpr char kMsgStopFx[]     = "/ext/fx/%u/stop";

    // BPM

    constexpr char kMsgLockBpmToMidiClock[] = "/ext/tempo/sync"; // 0, 1
    constexpr char kMsgSetBpm[]             = "/ext/tempo/bpm"; // 30.0 - 300.0
    constexpr char kMsgBpmTap[]             = "/ext/tempo/tap";

    inline bool isValidBpm(float bpm){
        return (30.0 <= bpm && bpm <= 300.0);
    }

    // Master Fader

    inline bool isValidGain(float gain){
        return (0.0 <= gain && gain <= 1.0);
    }

    inline bool isValidFaderPos(float pos){
        return (0.0 <= pos && pos <= 1.0);
    }

    constexpr char kMsgSetMasterGain[]      = "/ext/master/gain";           // 0.0 - 1.0
    constexpr char kMsgSetMasterFaderPos[]  = "/ext/master/faderpos";       // 0.0 - 1.0
    constexpr char kMsgSetMasterMute[]      = "/ext/master/mute";           // 0, 1

    // Reverb Fader

    constexpr char kMsgSetReverbGain[]      = "/ext/rev/master/gain";       // 0.0 - 1.0
    constexpr char kMsgSetReverbFaderPos[]  = "/ext/rev/master/faderpos";   // 0.0 - 1.0
    constexpr char kMsgSetReverbMute[]      = "/ext/rev/master/mute";       // 0, 1

    // Monitor Fader

    constexpr char kMsgSetMonitorGain[]      = "/ext/mon/gain";           // 0.0 - 1.0
    constexpr char kMsgSetMonitorFaderPos[]  = "/ext/mon/faderpos";       // 0.0 - 1.0
    constexpr char kMsgSetMonitorMute[]      = "/ext/mon/mute";           // 0, 1

    // User Fader

    constexpr char kMsgSetUserFaderNGain[]      = "/ext/fader%u/gain";           // 0.0 - 1.0
    constexpr char kMsgSetUserFaderNPos[]       = "/ext/fader%u/faderpos";       // 0.0 - 1.0
    constexpr char kMsgSetUserFaderNMute[]      = "/ext/fader%u/mute";           // 0, 1

    // Source selection

    constexpr char kMsgChangeSelectionOfSource[]    = "/ext/sel/src/%u";
    constexpr char kMsgSetSelectionToSource[]       = "/ext/sel/set/src/%u";
    constexpr char kMsgClearSelection[]             = "/ext/sel/set/none";

    // Group selection

    constexpr char kMsgSetSelectionToGroup[]        = "/ext/sel/set/grp/%u";

    // Headtracker

    inline bool isValidYaw(float yaw){
        return (-3.141 <= yaw && yaw <= 3.141);
    }

    inline bool isValidPitch(float pitch){
        return (-1.6 <= pitch && pitch <= 1.6);
    }

    inline bool isValidRoll(float roll){
        return (-3.141 <= roll && roll <= 3.141);
    }

    enum HeadtrackerType_t {HeadtrackerTypeOff = 0, HeadtrackerTypeMIDI = 1, HeadtrackerTypeOSC = 2};

    inline bool isValidHeadtrackerType(HeadtrackerType_t type){
        return (type == HeadtrackerTypeOff || type == HeadtrackerTypeMIDI || type == HeadtrackerTypeOSC);
    }

    constexpr char kHeadtrackerTypeOff[] = "off";
    constexpr char kHeadtrackerTypeMIDI[] = "midi";
    constexpr char kHeadtrackerTypeOSC[] = "osc";

    constexpr const char * kHeadtrackerTypes[] = {kHeadtrackerTypeOff, kHeadtrackerTypeMIDI, kHeadtrackerTypeOSC};

    constexpr char kMsgSetHeadtrackerOrientation[]  = "/ht/ypr"; // in radians: yaw [-3.141 - 3.141], pitch [-1.6 - 1.6], roll [-3.141 - 3.141]
    constexpr char kMsgResetHeadtracker[]           = "/ht/reset";
    constexpr char kMsgSetHeadtrackerType[]         = "/ht/type"; // "off", "midi", "osc"

    // OSC Devices

    enum CoordFormat_t {CoordFormatLISA = 0, CoordFormatADM = 1};

    inline bool isValidCoordFormat(CoordFormat_t format){
        return (format == CoordFormatLISA || format == CoordFormatADM);
    }

    constexpr char kCoordFormatLISA[] = "lisa";
    constexpr char kCoordFormatADM[] = "adm";

    constexpr const char * kCoordFormats[] = {kCoordFormatLISA, kCoordFormatADM};

    constexpr char kMsgRegisterDevice[]             = "/ext/device/%u/register";    // "ipAddress" port
    constexpr char kMsgDeleteDevice[]               = "/ext/device/%u/delete";
    constexpr char kMsgSetDeviceName[]              = "/ext/device/%u/name";        // "name"
    constexpr char kMsgEnableSendingToDevice[]      = "/ext/device/%u/send";        // 0 = off, 1 = on
    constexpr char kMsgEnableReceivingFromDevice[]  = "/ext/device/%u/receive";     // 0 = off, 1 = on
    constexpr char kMsgSetDeviceCoordFormat[]          = "/ext/device/%u/format";      // "lisa","adm"
    constexpr char kMsgSetMasterGainControl[]       = "/ext/device/%u/mastergain/control"; // 0, 1

    // ping

    constexpr char kMsgPing[]       = "/ext/ping"; // "ip" port
}

#endif //LISA_DESKBRIDGE_LISACONTROLLER_H
