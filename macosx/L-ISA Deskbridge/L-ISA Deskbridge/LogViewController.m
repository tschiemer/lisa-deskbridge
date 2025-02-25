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

#import "LogViewController.h"

@implementation LogViewController


- (void)viewDidAppear {
    [super viewDidAppear];

    [Log singleton].observer = self;

    self.timestampFormatter = [[NSDateFormatter alloc] init];
    self.timestampFormatter.dateFormat = @"yyyy-MM-dd HH:mm:ss.SSS";

    [_tableView reloadData];
}

- (void)viewWillDisappear {
    [Log singleton].observer = nil;

    [super viewWillDisappear];
}


-(void)logHasNewEntry:(LogEntry*)logEntry {
    [_tableView reloadData];
}

#pragma mark - Table view delegate


#pragma mark - Table view data source

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView {
    return [[[Log singleton] messages] count];
}

-(NSView *)tableView:(NSTableView *)tableView viewForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
    NSString *columnid = tableColumn.identifier;
    NSTableCellView *cell = [tableView makeViewWithIdentifier:columnid owner:self];
    
    LogEntry * entry = [[[Log singleton] messages] objectAtIndex:row];

    if ([columnid isEqualToString:@"Time"]) {
        cell.textField.stringValue = [self.timestampFormatter stringFromDate:entry.timestamp];
    }
    else if ([columnid isEqualToString:@"Level"]) {
        cell.textField.stringValue = entry.levelLabel;
    }
    else if ([columnid isEqualToString:@"Component"]) {
        cell.textField.stringValue = entry.component;
    }
    else if ([columnid isEqualToString:@"Message"]) {
        cell.textField.stringValue = entry.message;
    }



    switch(entry.logLevel){
        case kLogLevelError:
            cell.textField.textColor = [NSColor redColor];
            break;
        case kLogLevelDebug:
            cell.textField.textColor = [NSColor grayColor];
            break;
        case kLogLevelInfo:
        default:
            cell.textField.textColor = [NSColor textColor];
            break;
    }
    
    return cell;
}

@end
