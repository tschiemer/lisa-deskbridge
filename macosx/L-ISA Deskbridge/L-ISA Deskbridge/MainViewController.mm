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

#import "MainViewController.h"
#import "Extensions.h"
#import "Log.h"
#import "Settings.h"


//#define _NO_DEMO_

#if defined(_NO_DEMO_)
#include "lisa-deskbridge/Bridge.h"
#include "lisa-deskbridge/bridges/Generic.h"
#include "lisa-deskbridge/bridges/SQMidi.h"
#include "lisa-deskbridge/bridges/SQMitm.h"
#endif

#if defined(_NO_DEMO_)
struct OpaqueBridge {
    LisaDeskbridge::Bridge * bridge;
};
#else
struct OpaqueBridge {
    void * bridge;
};
#endif


@implementation MainViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.versionText.stringValue = [NSBundle.mainBundle objectForInfoDictionaryKey:@"CFBundleLongVersionString"];

    _opaque = new OpaqueBridge;
    _opaque->bridge = nullptr;

    // Initialize and load settings
    [Settings init];

    [self refreshSettingsView];
}

-(void)viewWillAppear {
    [super viewWillAppear];

    
}

- (void)viewWillDisappear {
#if defined(_NO_DEMO_)
    if (_opaque->bridge != nullptr){
        _opaque->bridge->stop();
    }
#endif
    
    [super viewWillDisappear];
}

- (void)viewDidDisappear {
    if (_opaque->bridge != nullptr){
#if defined(_NO_DEMO_)
        delete _opaque->bridge;
#endif
        _opaque->bridge = nullptr;
    }
    delete _opaque;
    
    [super viewDidDisappear];
}


//- (void)setRepresentedObject:(id)representedObject {
//    [super setRepresentedObject:representedObject];
//
//    // Update the view, if already loaded.
//}

- (void)showTab:(NSString*)identifier; {
    [_tabView selectTabViewItemWithIdentifier:identifier];
}

- (IBAction)onDebugModeChanged:(id)sender {
    if (_debugCheckbox.state){
        [Log setLogLevel:kLogLevelDebug];
    } else {
        [Log setLogLevel:kLogLevelInfo];
    }
}


- (IBAction)startButtonAction:(id)sender {
//    NSLog(@"startbutton" );



    if (self.opaque->bridge == nullptr) {
        [self start];
    } else {
        [self stop];
    }
}
-(void)start {
    
    NSLog(@"Starting...");
    
    _startButton.enabled = NO;
    [self enableSettings:NO];
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
    
#if defined(_NO_DEMO_)

        std::string bridgeName;
        std::map<std::string,std::string> bridgeOpts;
        
        NSDictionary * settings = [Settings getAll];

        bridgeName = [settings[@"bridge"] cStringUsingEncoding:NSString.defaultCStringEncoding];

        for(NSString * key in settings){
//            NSLog(@"key %@ = value %@", key, settings[key]);
            std::string ckey = [key cStringUsingEncoding:NSString.defaultCStringEncoding];
            std::string cvalue = [settings[key]  cStringUsingEncoding:NSString.defaultCStringEncoding];
            bridgeOpts[ ckey ] = cvalue;
        }

        try {
            _opaque->bridge = LisaDeskbridge::Bridge::factory(bridgeName, bridgeOpts);
        } catch (std::exception &e){
            NSLog(@"Error: %s", e.what());
        }
        if (_opaque->bridge == nullptr){
            // shouldnt happen
            NSLog(@"Failed to create factory");
            [self didStart:NO];
            return;
        }
        
        NSLog(@"Starting bridge");
    
    
        if (_opaque->bridge->start() == false){
            NSLog(@"start failed");
            delete _opaque->bridge;
            _opaque->bridge = nullptr;
            [self didStart:NO];
            return;
        }
        
            
        [self didStart:YES];

#else


        _opaque->bridge = (void*)1;

        sleep(1);

        [self didStart:YES];


#endif
        
    });
}

-(void)didStart:(BOOL)success {
    
    dispatch_async(dispatch_get_main_queue(), ^{
        if (success){
            _startButton.title = @"Stop";
        } else {
            _startButton.title = @"Start";
            [self enableSettings:YES];
        }
        _startButton.enabled = YES;
    });
}

-(void)stop {
    
    NSLog(@"Stopping...");
    
    _startButton.enabled = NO;
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
#if defined(_NO_DEMO_)


        _opaque->bridge->stop();

        delete _opaque->bridge;

        _opaque->bridge = nullptr;
        
#else


        sleep(1);

        _opaque->bridge = nullptr;


#endif
      
        [self didStop:YES];
    });
}

-(void)didStop:(BOOL)success {
    
    dispatch_async(dispatch_get_main_queue(), ^{
        if (success){
            _startButton.title = @"Start";
            [self enableSettings:YES];
        } else {
            _startButton.title = @"Stop";
        }
        _startButton.enabled = YES;
    });
}

#pragma Bridge Tab


- (IBAction)onBridgeChanged:(id)sender {

    [Settings set:@"bridge" value:_bridgeSelect.titleOfSelectedItem];
    
    [self enableSettingsForSelectedBridge:YES];

    [self settingsChanged:YES];
}

- (IBAction)onMixerIpChanged:(id)sender {
    
    BOOL valid = _mixerIpText.stringValue.length > 0 &&
                [_mixerIpText.stringValue isValidIPAddress];
    
    [_mixerIpText showError:!valid];
    
    if (valid){
        [Settings set:@"mixer-ip" value:_mixerIpText.stringValue];
    }

    [self settingsChanged:valid];
}

-(IBAction)onMixerIpFindButton:(id)sender {
    //
}

-(void)didSelectMixerIp:(NSString*)ip withName:(NSString*)name {
    NSLog(@"Selected mixer ip %@", ip);
    
    _mixerIpText.stringValue = ip;

    [Settings set:@"mixer-ip" value:ip];

    [self settingsChanged:YES];
}


#pragma L-ISA Controller Tab

- (IBAction)onLisaControllerIpChanged:(id)sender {
    BOOL valid = _lisaControllerIpText.stringValue.length > 0 &&
                [_lisaControllerIpText.stringValue isValidIPAddress];
    
    [_lisaControllerIpText showError:!valid];
    
    if (valid){
        [Settings set:@"lisa-controller-ip" value:_lisaControllerIpText.stringValue];
    }
    
    [self settingsChanged:valid];
}

- (IBAction)onLisaControllerPortChanged:(id)sender {

    BOOL valid = _lisaControllerPortText.stringValue.length > 0 &&
                 [_lisaControllerPortText.stringValue isValidPort];

    [_lisaControllerPortText showError:!valid];

    if (valid){
        [Settings set:@"lisa-controller-port" value:_lisaControllerPortText.stringValue];
    }

    [self settingsChanged:valid];
}

- (IBAction)onDeviceIpChanged:(id)sender {
    BOOL valid = _deviceIpText.stringValue.length > 0 &&
                [_deviceIpText.stringValue isValidIPAddress];
    
    [_deviceIpText showError:!valid];
    
    if (valid){
        [Settings set:@"device-ip" value:_deviceIpText.stringValue];
    }
    
    [self settingsChanged:valid];
}

- (IBAction)onDevicePortChanged:(id)sender {

    BOOL valid = _devicePortText.stringValue.length > 0 &&
                 [_devicePortText.stringValue isValidPort];

    [_devicePortText showError:!valid];

    if (valid){
        [Settings set:@"device-port" value:_devicePortText.stringValue];
    }

    [self settingsChanged:valid];
}

- (IBAction)onDeviceIDChanged:(id)sender {
    [Settings set:@"device-id" value:_deviceIDSelect.titleOfSelectedItem];

    [self settingsChanged:YES];
}

- (IBAction)onDeviceNameChanged:(id)sender {

    NSString * name = [_deviceNameText.stringValue stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
    BOOL valid = name.length > 0;

    [_deviceNameText showError:!valid];

    if (valid){
        [Settings set:@"device-name" value:name];
    }


    [self settingsChanged:valid];
}

- (IBAction)onClaimLevelControlChanged:(id)sender {
    NSString * state = [NSString stringWithFormat:@"%d", (int)_claimLevelControlCheckbox.state];
    [Settings set:@"claim-level-control" value:state];
}

#pragma About Tab

- (IBAction)onLink:(id)sender {
    NSButton * link = (NSButton*)sender;
    [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:link.title]];
    NSLog(@"opening? %@", link.title);
}


#pragma Settings

-(void)refreshSettingsView {
    
#if defined(_NO_DEMO_)
    [_bridgeSelect selectItemWithTitle:[Settings get:@"bridge"]];

    _mixerIpText.stringValue = [Settings get:@"mixer-ip"];

    _lisaControllerIpText.stringValue = [Settings get:@"lisa-controller-ip"];
    _lisaControllerPortText.stringValue = [Settings get:@"lisa-controller-port"];

    _deviceIpText.stringValue = [Settings get:@"device-ip"];
    _devicePortText.stringValue = [Settings get:@"device-port"];
    [self.deviceIDSelect selectItemWithTitle:[Settings get:@"device-id"]];
    _deviceNameText.stringValue = [Settings get:@"device-name"];
    
    _claimLevelControlCheckbox.state = [[Settings get:@"claim-level-control"] intValue] == 1;
#endif
    

    [self enableSettings:YES];

    [self settingsChanged:YES];
}

-(void)settingsChanged:(BOOL)valid {
    // argument := a changed UI element might have become invalid, but this was not stored
    
    if (valid){
        valid = self.settingsValidator;
    }

    _startButton.enabled = valid;
}

-(BOOL)settingsValidator {

    if (![[Settings get:@"bridge"] isEqualToString:@"Generic"] &&
            ![[Settings get:@"bridge"] isEqualToString:@"SQ-Mitm"] &&
            ![[Settings get:@"bridge"] isEqualToString:@"SQ-Midi"]){
        NSLog(@"invalid bridge");
        return NO;
    }

    if (![[Settings get:@"lisa-controller-ip"] isValidIPAddress]){
        NSLog(@"invalid lisac ip");
        return NO;
    }
    if (![[Settings get:@"lisa-controller-port"] isValidPort]){
        NSLog(@"invalid lisac port");
        return NO;
    }
    if (![[Settings get:@"device-ip"] isValidIPAddress]){
        NSLog(@"invalid device-ip");
        return NO;
    }
    if (![[Settings get:@"device-port"] isValidPort]){
        NSLog(@"invalid device-port");
        return NO;
    }
    if ([[Settings get:@"device-id"] intValue] < 1 || 10 < [[Settings get:@"device-id"] intValue]){
        NSLog(@"invalid device id %d", [[Settings get:@"device-id"] intValue]);
        return NO;
    }
    if ([[Settings get:@"device-name"] length] == 0){
        NSLog(@"invalid device name");
        return NO;
    }

    if ([[Settings get:@"bridge"] isEqualToString:@"SQ-Mitm"]){
        if (![[Settings get:@"mixer-ip"] isValidIPAddress]){
            NSLog(@"invalid mixer-ip");
            return NO;
        }
    }

    return YES;
}

-(void)enableSettings:(BOOL)enabled {

    // bridge tab
    _bridgeSelect.enabled = enabled;

    // l-isa controller tab
    _lisaControllerIpText.enabled = enabled;
    _lisaControllerPortText.enabled = enabled;

    _deviceIpText.enabled = enabled;
    _devicePortText.enabled = enabled;
    _deviceIDSelect.enabled = enabled;
    _deviceNameText.enabled = enabled;
    _claimLevelControlCheckbox.enabled = enabled;

    _debugCheckbox.enabled = enabled;

    [self enableSettingsForSelectedBridge:enabled];
}

-(void)enableSettingsForSelectedBridge:(BOOL)enabled {

     if ([[Settings get:@"bridge"] isEqualToString:@"SQ-Mitm"]){
        _mixerIpText.enabled = enabled;
        _findMixerIpButton.enabled = enabled;
    } else {
        _mixerIpText.enabled = false;
        _findMixerIpButton.enabled = false;
    }
}

@end
