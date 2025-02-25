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
#import "Log.h"

@interface LogViewController : NSViewController<NSTableViewDataSource, NSTableViewDelegate, LogObserver>

@property (weak, nonatomic) IBOutlet NSTableView *tableView;

@property (strong,nonatomic) NSDateFormatter * timestampFormatter;

-(void)logHasNewEntry:(LogEntry*)logEntry;

@end
