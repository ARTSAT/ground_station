/*
 **      ARTSAT Project
 **
 **      Original Copyright (C) 2013 - 2014 Ron Hashimoto.
 **                                          http://h2so5.net/
 **                                          mail@h2so5.net
 **      Portions Copyright (C) <year> <author>
 **                                          <website>
 **                                          <e-mail>
 **      Version     Cocoa / Objective-C
 **      Website     http://artsat.jp/
 **      E-mail      info@artsat.jp
 **
 **      This source code is for Xcode.
 **      Xcode 4.6.2 (Apple LLVM compiler 4.2, LLVM GCC 4.2)
 **
 **      AppDelegate.m
 **
 **      ------------------------------------------------------------------------
 **
 **      The MIT License (MIT)
 **
 **      Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 **      associated documentation files (the "Software"), to deal in the Software without restriction,
 **      including without limitation the rights to use, copy, modify, merge, publish, distribute,
 **      sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 **      furnished to do so, subject to the following conditions:
 **      The above copyright notice and this permission notice shall be included in all copies or
 **      substantial portions of the Software.
 **      THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 **      BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 **      IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 **      WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 **      OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 **
 **      以下に定める条件に従い、本ソフトウェアおよび関連文書のファイル（以下「ソフトウェア」）の複製を
 **      取得するすべての人に対し、ソフトウェアを無制限に扱うことを無償で許可します。
 **      これには、ソフトウェアの複製を使用、複写、変更、結合、掲載、頒布、サブライセンス、および、または販売する権利、
 **      およびソフトウェアを提供する相手に同じことを許可する権利も無制限に含まれます。
 **      上記の著作権表示および本許諾表示を、ソフトウェアのすべての複製または重要な部分に記載するものとします。
 **      ソフトウェアは「現状のまま」で、明示であるか暗黙であるかを問わず、何らの保証もなく提供されます。
 **      ここでいう保証とは、商品性、特定の目的への適合性、および権利非侵害についての保証も含みますが、それに限定されるものではありません。
 **      作者または著作権者は、契約行為、不法行為、またはそれ以外であろうと、ソフトウェアに起因または関連し、
 **      あるいはソフトウェアの使用またはその他の扱いによって生じる一切の請求、損害、その他の義務について何らの責任も負わないものとします。
 */

#import "AppDelegate.h"

@interface AppDelegate ()
@property (weak) IBOutlet NSMenu *statusMenu;
@property (copy) NSString *session;
@end

@implementation AppDelegate

NSStatusItem *_statusItem;
NSTimer *_timer;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    [self setupStatusItem];
    
    self.session = [[[self request:@{@"jsonrpc": @"2.0", @"id": @0, @"method": @"observer.getVersion"}] objectForKey:@"result"] objectForKey:@"session"];
    _timer = [NSTimer scheduledTimerWithTimeInterval:1.0
                                              target:self
                                            selector:@selector(time:)
                                            userInfo:nil
                                             repeats:YES];
    [self time:_timer];
}

- (void)setupStatusItem
{
    NSStatusBar *systemStatusBar = [NSStatusBar systemStatusBar];
    _statusItem = [systemStatusBar statusItemWithLength:NSVariableStatusItemLength];
    [_statusItem setHighlightMode:YES];
    [_statusItem setTitle:@"ARTSAT"];
    [_statusItem setMenu:self.statusMenu];
}

- (void)time:(NSTimer*)timer
{
    NSMutableAttributedString *mutableAttributedString = [[NSMutableAttributedString alloc] init];
    NSDictionary *stdAttr   = @{
                                NSFontAttributeName: [NSFont systemFontOfSize:18.0f]
                                };
    
    NSDictionary *grayAttr  = @{
                                NSFontAttributeName: [NSFont systemFontOfSize:18.0f],
                                NSBackgroundColorAttributeName: [NSColor lightGrayColor]
                                };
    
    NSDictionary *blackAttr = @{
                                NSFontAttributeName: [NSFont systemFontOfSize:18.0f],
                                NSBackgroundColorAttributeName: [NSColor blackColor],
                                NSForegroundColorAttributeName: [NSColor whiteColor]
                                };
    
    NSDictionary *blueAttr  = @{
                                NSFontAttributeName: [NSFont systemFontOfSize:18.0f],
                                NSBackgroundColorAttributeName: [NSColor blueColor],
                                NSForegroundColorAttributeName: [NSColor whiteColor]
                                };
    
    NSDictionary *purpleAttr  = @{
                                NSFontAttributeName: [NSFont systemFontOfSize:18.0f],
                                NSBackgroundColorAttributeName: [NSColor purpleColor],
                                NSForegroundColorAttributeName: [NSColor whiteColor]
                                };
    
    NSDictionary *redAttr   = @{
                                NSFontAttributeName: [NSFont systemFontOfSize:18.0f],
                                NSForegroundColorAttributeName: [NSColor redColor]
                                };
    
    NSDictionary *smallAttr = @{
                                NSFontAttributeName: [NSFont systemFontOfSize:12.0f]
                                };
    
    [mutableAttributedString appendAttributedString:[[NSAttributedString alloc] initWithString:@"  exNorad " attributes:smallAttr]];
    int exnorad = [[[[self request:@{@"jsonrpc": @"2.0", @"id": @0, @"method": @"observer.getEXNORAD", @"params": @{@"session": self.session
                                                                                                                }}] objectForKey:@"result"] objectForKey:@"exnorad"] intValue];
    [mutableAttributedString appendAttributedString:[[NSAttributedString alloc] initWithString:[NSString stringWithFormat:@"%d  ", exnorad] attributes:stdAttr]];
    
    {
        NSString* mode = [[[self request:@{@"jsonrpc": @"2.0", @"id": @0, @"method": @"observer.getMode", @"params": @{@"session": self.session}}] objectForKey:@"result"] objectForKey:@"mode"];
        if ([mode length] == 0) mode = @"Idle";
        NSDictionary *attr;
        
        if ([mode isEqualToString:@"CW"]) {
            attr = blackAttr;
        }
        else if ([mode isEqualToString:@"FM"]) {
            attr = blueAttr;
        }
        else if ([mode isEqualToString:@"FM_TEST"]) {
            attr = purpleAttr;
        }
        else {
            attr = grayAttr;
        }
        [mutableAttributedString appendAttributedString:[[NSAttributedString alloc] initWithString:[NSString stringWithFormat:@"  %@  ", mode] attributes:attr]];
    }
    
    [mutableAttributedString appendAttributedString:[[NSAttributedString alloc] initWithString:@"   Elevation" attributes:smallAttr]];
    
    {
        double elevation = [[[[self request:@{@"jsonrpc": @"2.0", @"id": @0, @"method": @"observer.getSatelliteDirection", @"params": @{@"session": self.session}}] objectForKey:@"result"] objectForKey:@"elevation"] doubleValue];
        NSDictionary *attr = elevation >= 0.0 ? redAttr : stdAttr;
        [mutableAttributedString appendAttributedString:[[NSAttributedString alloc] initWithString:[NSString stringWithFormat:@" %.03f ", elevation] attributes:attr]];
    }
    
    [mutableAttributedString appendAttributedString:[[NSAttributedString alloc] initWithString:@"  Azimuth" attributes:smallAttr]];
    double azimuth = [[[[self request:@{@"jsonrpc": @"2.0", @"id": @0, @"method": @"observer.getSatelliteDirection", @"params": @{@"session": self.session}}] objectForKey:@"result"] objectForKey:@"azimuth"] doubleValue];
    [mutableAttributedString appendAttributedString:[[NSAttributedString alloc] initWithString:[NSString stringWithFormat:@" %.03f  ", azimuth] attributes:stdAttr]];
    
    [mutableAttributedString appendAttributedString:[[NSAttributedString alloc] initWithString:@" " attributes:grayAttr]];
    
    [mutableAttributedString appendAttributedString:[[NSAttributedString alloc] initWithString:@"   CW " attributes:smallAttr]];
    double beacon = [[[[self request:@{@"jsonrpc": @"2.0", @"id": @0, @"method": @"observer.getSatelliteFrequency", @"params": @{@"session": self.session}}] objectForKey:@"result"] objectForKey:@"beacon"] doubleValue] / 1000000.0;
    [mutableAttributedString appendAttributedString:[[NSAttributedString alloc] initWithString:[NSString stringWithFormat:@"%.03f", beacon] attributes:stdAttr]];
    [mutableAttributedString appendAttributedString:[[NSAttributedString alloc] initWithString:@"MHz " attributes:smallAttr]];
    
    [mutableAttributedString appendAttributedString:[[NSAttributedString alloc] initWithString:@"   FM⬆" attributes:smallAttr]];
    double sender = [[[[self request:@{@"jsonrpc": @"2.0", @"id": @0, @"method": @"observer.getSatelliteFrequency", @"params": @{@"session": self.session}}] objectForKey:@"result"] objectForKey:@"sender"] doubleValue] / 1000000.0;
    [mutableAttributedString appendAttributedString:[[NSAttributedString alloc] initWithString:[NSString stringWithFormat:@"%.03f", sender] attributes:stdAttr]];
    [mutableAttributedString appendAttributedString:[[NSAttributedString alloc] initWithString:@"MHz " attributes:smallAttr]];
    
    [mutableAttributedString appendAttributedString:[[NSAttributedString alloc] initWithString:@"   FM⬇" attributes:smallAttr]];
    double receiver = [[[[self request:@{@"jsonrpc": @"2.0", @"id": @0, @"method": @"observer.getSatelliteFrequency", @"params": @{@"session": self.session}}] objectForKey:@"result"] objectForKey:@"receiver"] doubleValue] / 1000000.0;
    [mutableAttributedString appendAttributedString:[[NSAttributedString alloc] initWithString:[NSString stringWithFormat:@"%.03f", receiver] attributes:stdAttr]];
    [mutableAttributedString appendAttributedString:[[NSAttributedString alloc] initWithString:@"MHz " attributes:smallAttr]];
    
    [_statusItem setAttributedTitle:mutableAttributedString];
}

- (NSMutableDictionary*)request:(NSDictionary*)dict
{
    if([NSJSONSerialization isValidJSONObject:dict]) {
        @autoreleasepool {
            NSError *error;
            NSData *data = [NSJSONSerialization dataWithJSONObject:dict options:0 error:&error];
            if (!error) {
                NSString *url = @"http://localhost:16782/rpc.json";
                NSMutableURLRequest *request = [[NSMutableURLRequest alloc]init];
                [request setURL:[NSURL URLWithString:url]];
                [request setHTTPMethod:@"POST"];
                [request setHTTPBody:data];
                
                NSURLResponse *response;
                NSData *result = [NSURLConnection sendSynchronousRequest:request
                                                       returningResponse:&response
                                                                   error:&error];
                
                if (!error) {
                    NSMutableDictionary *obj = [NSJSONSerialization JSONObjectWithData:result
                                                                               options:NSJSONReadingMutableContainers
                                                                                 error:&error];
                    if (!error) {
                        return obj;
                    }
                }
            }
        }
    }
    
    return nil;
}

@end
