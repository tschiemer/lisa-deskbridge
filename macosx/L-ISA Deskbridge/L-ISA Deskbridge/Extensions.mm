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

#import "Extensions.h"

#include <arpa/inet.h>

// https://stackoverflow.com/questions/1679152/how-to-validate-an-ip-address-with-regular-expression-in-objective-c
@implementation NSString (IPValidation)

- (BOOL)isValidIPAddress
{
    const char *utf8 = [self UTF8String];
    int success;

    struct in_addr dst;
    success = inet_pton(AF_INET, utf8, &dst);
    if (success != 1) {
        struct in6_addr dst6;
        success = inet_pton(AF_INET6, utf8, &dst6);
    }

    return success == 1;
}

- (BOOL)isValidPort {

    int i = self.intValue;

    return 0 < i && i < 0xffff;
}

@end

@implementation NSTextField (ErrorIndication)

-(void)showError:(BOOL)yesOrNo {

    if (yesOrNo){
        self.backgroundColor = [NSColor redColor];
    } else {
        self.backgroundColor = [NSColor clearColor];
    }
}

@end


@implementation NSTextField (CopyPaste)

- (BOOL) performKeyEquivalent:(NSEvent *) event {
    if (event.type == NSEventTypeKeyDown &&
        (event.modifierFlags & NSEventModifierFlagDeviceIndependentFlagsMask) == NSEventModifierFlagCommand){

        if ([event.charactersIgnoringModifiers isEqualToString:@"a"]){
            [[NSApplication sharedApplication] sendAction:@selector(selectAll:) to:nil from:self];
            return YES;
        }

        if ([event.charactersIgnoringModifiers isEqualToString:@"x"]){
            [[NSApplication sharedApplication] sendAction:@selector(cut:) to:nil from:self];
            return YES;
        }

        if ([event.charactersIgnoringModifiers isEqualToString:@"c"]){
            [[NSApplication sharedApplication] sendAction:@selector(copy:) to:nil from:self];
            return YES;
        }

        if ([event.charactersIgnoringModifiers isEqualToString:@"v"]){
            [[NSApplication sharedApplication] sendAction:@selector(paste:) to:nil from:self];
            return YES;
        }


//        if ([event.charactersIgnoringModifiers isEqualToString:@"z"]){
//            [[NSApplication sharedApplication] sendAction:@selector(undo) to:nil from:self];
//            return YES;
//        }
    }

//    NSLog(@"%@ %@ %d", event, event.charactersIgnoringModifiers, event.keyCode);

    return [super performKeyEquivalent:event];
}

@end
