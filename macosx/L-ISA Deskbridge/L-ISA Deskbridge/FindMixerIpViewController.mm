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

#import "FindMixerIpViewController.h"
#import "MainViewController.h"


#if defined(_NO_DEMO_)
#include "sqmixmitm/Discovery.h"
#endif


#if defined(_NO_DEMO_)
struct OpaqueDiscovery {
    SQMixMitm::Discovery discovery;
};

#else
struct OpaqueDiscovery {
    int dummy;
};
#endif



@implementation FindMixerIpWindowController
@end

@implementation FindMixerIpSegue

- (instancetype) initWithIdentifier:(NSStoryboardSegueIdentifier) identifier
                             source:(id) sourceController
                        destination:(id) destinationController {
    
    NSObject<FindMixerIpResultReceiver> * receiver = sourceController;
    
    NSWindowController * dstW = destinationController;
    FindMixerIpViewController * findVC = (FindMixerIpViewController *)dstW.contentViewController;
    
    findVC.findMixerIpResultReceiver = receiver;
    
    
    return [super initWithIdentifier:identifier source:sourceController destination:destinationController];
}

@end

@implementation FindMixerIpViewController

//-(instancetype)init {
//    printf(" init\n" );
//    _opaque = new OpaqueDiscovery;
//
//    return [super init];
//}
//- (instancetype) initWithNibName:(NSNibName) nibNameOrNil
//                          bundle:(NSBundle *) nibBundleOrNil {
//    printf("initWithNibName \n" );
//    _opaque = new OpaqueDiscovery;
//    return [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
//}

- (void)viewWillAppear {
    [super viewWillAppear];
    
    _tableView.doubleAction = @selector(selectedMixerIp:);


    _opaque = new OpaqueDiscovery;

#if defined(_NO_DEMO_)
    
    _opaque->discovery.onFound(^(SQMixMitm::Discovery::Mixer &mixer){

        dispatch_async(dispatch_get_main_queue(), ^{
            [self.tableView reloadData];
        });
    });
    
    _opaque->discovery.onTimeout(^(SQMixMitm::Discovery::Mixer &mixer){

        dispatch_async(dispatch_get_main_queue(), ^{
            [self.tableView reloadData];
        });
    });
    
    if (_opaque->discovery.start()){
        
    }
#endif
}

- (void)viewDidAppear {
    [super viewDidAppear];

    [_tableView reloadData];
}

- (void)viewWillDisappear {
    
#if defined(_NO_DEMO_)
    if (_opaque->discovery.state() == SQMixMitm::Discovery::Running){
        _opaque->discovery.stop();
    }
#endif
    
    [super viewWillDisappear];
}

- (void)viewDidDisappear {
    delete _opaque;
    
    [super viewDidDisappear];
}


-(void)selectedMixerIp:(id)sender {
//    NSLog(@"selected mixerip %d", (int)_tableView.clickedRow);
    
    NSString * ip;
    NSString * name;
    
#if defined(_NO_DEMO_)
    SQMixMitm::Discovery::Mixer * mixer = _opaque->discovery.mixerList()->at(_tableView.clickedRow);

    ip = [NSString stringWithFormat:@"%s", mixer->ip.data()];
    name = [NSString stringWithFormat:@"%s", mixer->name.data()];
#else
    ip = [NSString stringWithFormat:@"10.0.0.%d", (int)_tableView.clickedRow];
    name = [NSString stringWithFormat:@"SQ6 %d", (int)_tableView.clickedRow];
#endif
    
    [_findMixerIpResultReceiver didSelectMixerIp:ip withName:name];
        
    [NSApp.modalWindow close];
}

#pragma mark - Table view delegate


#pragma mark - Table view data source

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView {
#if defined(_NO_DEMO_)
    if (_opaque == nullptr){
        return 0;
    }
    return _opaque->discovery.mixerList()->size();
#else
    // just dummy
    return 3;
#endif
}

-(NSView *)tableView:(NSTableView *)tableView viewForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
    NSString *columnid = tableColumn.identifier;
    NSTableCellView *cell = [tableView makeViewWithIdentifier:columnid owner:self];
    
#if defined(_NO_DEMO_)
    SQMixMitm::Discovery::Mixer * mixer = _opaque->discovery.mixerList()->at(row);
    if ([columnid isEqualToString:@"IP"]) {
        cell.textField.stringValue = [NSString stringWithFormat:@"%s",mixer->name.data()];
    }
    else if ([columnid isEqualToString:@"Name"]) {
        cell.textField.stringValue = [NSString stringWithFormat:@"%s", mixer->ip.data()];
    }
#else

    if ([columnid isEqualToString:@"IP"]) {
        cell.textField.stringValue = [NSString stringWithFormat:@"10.0.0.%d",(int)row];
    }
    else if ([columnid isEqualToString:@"Name"]) {
        cell.textField.stringValue = [NSString stringWithFormat:@"SQ6 %d", (int)row];
    }
#endif
    
    return cell;
}

@end
