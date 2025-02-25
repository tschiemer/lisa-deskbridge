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

#import <Cocoa/Cocoa.h>

// as used in storyboard
enum BridgeTag {
    kBridgeNone,
    kBridgeSQMitm = 1,
    kBridgeSQMidi = 2,
    kBridgeGeneric = 3
};


struct OpaqueBridge;


@interface MainViewController : NSViewController

@property (nonatomic) struct OpaqueBridge * opaque;


// General UI
@property (weak, nonatomic) IBOutlet NSTabView *tabView;
@property (weak, nonatomic) IBOutlet NSButton *startButton;
@property (weak, nonatomic) IBOutlet NSButton *debugCheckbox;

// Bridge Tab
@property (weak, nonatomic) IBOutlet NSPopUpButton *bridgeSelect;
@property (weak, nonatomic) IBOutlet NSTextField *mixerIpText;
@property (weak, nonatomic) IBOutlet NSButton *findMixerIpButton;

// L-ISA Controller Tab
@property (weak, nonatomic) IBOutlet NSTextField *lisaControllerIpText;
@property (weak, nonatomic) IBOutlet NSTextField *lisaControllerPortText;
@property (weak, nonatomic) IBOutlet NSTextField *deviceIpText;
@property (weak, nonatomic) IBOutlet NSTextField *devicePortText;
@property (weak, nonatomic) IBOutlet NSPopUpButton *deviceIDSelect;
@property (weak, nonatomic) IBOutlet NSTextField *deviceNameText;
@property (weak, nonatomic) IBOutlet NSButton *claimLevelControlCheckbox;


// About tab

@property (weak, nonatomic) IBOutlet NSTextField * versionText;


// General UI

- (void)showTab:(NSString*)identifier;

- (IBAction)onDebugModeChanged:(id)sender;


// Bridge Tab
//- (enum BridgeTag)selectedBridge;
- (IBAction)onBridgeChanged:(id)sender;


- (IBAction)onMixerIpChanged:(id)sender;

-(IBAction)onMixerIpFindButton:(id)sender;
-(void)didSelectMixerIp:(NSString*)ip withName:(NSString*)name;


// L-ISA Controller Tab

- (IBAction)onLisaControllerIpChanged:(id)sender;
- (IBAction)onLisaControllerPortChanged:(id)sender;
- (IBAction)onDeviceIpChanged:(id)sender;
- (IBAction)onDevicePortChanged:(id)sender;
- (IBAction)onDeviceIDChanged:(id)sender;
- (IBAction)onDeviceNameChanged:(id)sender;
- (IBAction)onClaimLevelControlChanged:(id)sender;

// About Tab

- (IBAction)onLink:(id)sender;

// settings
-(void)refreshSettingsView;

-(void)settingsChanged:(BOOL)valid;
-(BOOL)settingsValidator;

-(void)enableSettings:(BOOL)enabled;
-(void)enableSettingsForSelectedBridge:(BOOL)enabled;


// Running

- (IBAction)startButtonAction:(id)sender;
-(void)start;
-(void)didStart:(BOOL)success;
-(void)stop;
-(void)didStop:(BOOL)success;







@end

