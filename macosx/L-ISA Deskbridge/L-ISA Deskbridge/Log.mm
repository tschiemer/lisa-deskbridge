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

#import "Log.h"


#include "sqmixmitm/log.h"
#include "lisa-deskbridge/log.h"

static Log * singleton_ = nullptr;

static void logFromSQMixMitm(SQMixMitm::LogLevel level, const char * msg, ...){

    if (singleton_.logLevel == kLogLevelInfo && level > SQMixMitm::LogLevelInfo){
        return;
    }

    va_list args;
    va_start(args, msg);

//    NSLogv([NSString stringWithCString:msg], args);

    LogEntry * logEntry = [[LogEntry alloc] initWithLevel:(LogLevel)level
                                            withComponent:@"SqMixMitm"
                                              withMessage:[[NSString alloc] initWithFormat:[NSString stringWithFormat:@"%s",msg] arguments:args]];

    va_end(args);

    [singleton_.messages addObject:logEntry];
    [singleton_.observer logHasNewEntry:logEntry];
}

static void logFromLisaDeskbridge(LisaDeskbridge::LogLevel level, const char * msg, ...){

    if (singleton_.logLevel == kLogLevelInfo && level > LisaDeskbridge::LogLevelInfo){
        return;
    }

    va_list args;
    va_start(args, msg);

//    NSLogv([NSString stringWithCString:msg], args);

    LogEntry * logEntry = [[LogEntry alloc] initWithLevel:(LogLevel)level
                                            withComponent:@"LisaDeskbridge"
                                              withMessage:[[NSString alloc] initWithFormat:[NSString stringWithFormat:@"%s",msg] arguments:args]];

    va_end(args);

    [singleton_.messages addObject:logEntry];
    [singleton_.observer logHasNewEntry:logEntry];
}

@implementation LogEntry

-(instancetype)initWithLevel:(enum LogLevel)level withComponent:(NSString*)component withMessage:(NSString *)str{
    LogEntry * logEntry = [super init];

    logEntry.timestamp = [NSDate now];
    logEntry.logLevel = level;
    logEntry.component = component;
    logEntry.message = str;

    return logEntry;
}

-(NSString*)levelLabel {
    if (self.logLevel == kLogLevelInfo){
        return @"INFO";
    } else if (self.logLevel == kLogLevelError){
        return @"ERROR";
    } else  if (self.logLevel == kLogLevelDebug){
        return @"DEBUG";
    }
    return [NSString stringWithFormat:@"%d", self.logLevel];
}

@end

@implementation Log

-(instancetype)init {
    Log * l = [super init];

    l.logLevel = kLogLevelInfo;
    l.messages = [[NSMutableArray<LogEntry*> alloc] init];
    
    return l;
}

+(Log*)singleton {
    return singleton_;
}

+(void)init {
    singleton_ = [[Log alloc] init];
    
//    [self setLogLevel:kLogLevelInfo];
    
    SQMixMitm::setLogFunction(logFromSQMixMitm);
    LisaDeskbridge::setLogFunction(logFromLisaDeskbridge);
}

+(void)setLogLevel:(enum LogLevel)level {
    singleton_.logLevel = level;
//    switch(level){
//
//        case kLogLevelDebug:
//
//            SQMixMitm::setLogLevel(SQMixMitm::LogLevelDebug);
//            LisaDeskbridge::setLogLevel(LisaDeskbridge::LogLevelDebug);
//            break;
//
//        default:
//        case kLogLevelInfo:
//            SQMixMitm::setLogLevel(SQMixMitm::LogLevelInfo);
//            LisaDeskbridge::setLogLevel(LisaDeskbridge::LogLevelInfo);
//            break;
//    }
//
}

@end
