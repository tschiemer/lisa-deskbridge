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

#import "Settings.h"

#include "lisa-deskbridge/LisaController.h"

#include "lisa-deskbridge/Bridge.h"

static Settings * singleton_ = nullptr;

//static NSString* ctons(char * str) {
////    NSStringEncoding * enc = [NSStringEncoding defaultCStringEncoding];
//    return [NSString stringWithCString:str encoding:defaultCStringEncoding];//[NSString stringWithCString:str encoding:[NSStringEncoding defaultCStringEncoding]];
//};

@implementation Settings

+(void)init {
    singleton_ = [[Settings alloc] init];

#if defined(_NO_DEMO_)
    [NSUserDefaults.standardUserDefaults registerDefaults:@{
            @"lisa-deskbridge": @{
                    @"bridge": @"",
                    @"mixer-ip": @"",
                    @"lisa-controller-ip": [NSString stringWithCString:LisaDeskbridge::kLisaControllerIpDefault encoding:NSString.defaultCStringEncoding],
                    @"lisa-controller-port": [NSString stringWithFormat:@"%d", LisaDeskbridge::kLisaControllerPortDefault],
                    @"device-id": @"5",
                    @"device-port": [NSString stringWithFormat:@"%d", LisaDeskbridge::kDevicePortDefault],
                    @"device-name": @"L-ISA Deskbridge"
            }
    }];
#endif

        
    singleton_.dictionary = [NSMutableDictionary dictionaryWithDictionary:[NSUserDefaults.standardUserDefaults dictionaryForKey:@"lisa-deskbridge"]];
}


+(Settings*)singleton {
    return singleton_;
}

+(void)save {
    [NSUserDefaults.standardUserDefaults setObject:singleton_.dictionary forKey:@"lisa-deskbridge"];
}

+(NSString*)get:(NSString *)key{
    return singleton_.dictionary[key];
}
+(void)set:(NSString *)key value:(NSString*)value {
    singleton_.dictionary[key] = value;
    [self save];
}

+(NSDictionary*)getAll {
    return singleton_.dictionary;
}

@end
