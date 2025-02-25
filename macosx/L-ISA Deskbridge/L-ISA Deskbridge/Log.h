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

enum LogLevel {
    kLogLevelError  = 1,
    kLogLevelInfo   = 2,
    kLogLevelDebug  = 3
};

@interface LogEntry : NSObject
@property (nonnull,strong) NSDate * timestamp;
@property () enum LogLevel logLevel;
@property (nonnull, strong) NSString * component;
@property (nonnull,strong) NSString * message;
-(_Nonnull instancetype)initWithLevel:(enum LogLevel)level withComponent:(NSString* _Nonnull)component withMessage:(NSString * _Nonnull)str;
-(NSString* _Nonnull )levelLabel;
@end

@protocol LogObserver
-(void)logHasNewEntry:(LogEntry* _Nonnull)logEntry;
@end

@interface Log : NSObject

@property (nonatomic,weak) NSObject<LogObserver> * observer;

@property (nonatomic) enum LogLevel logLevel;
@property (nonatomic, strong, nonnull) NSMutableArray<LogEntry*> * messages;

-(_Nonnull instancetype)init;

+(Log* _Nonnull )singleton;

+(void)init;
+(void)setLogLevel:(enum LogLevel)level;

@end
