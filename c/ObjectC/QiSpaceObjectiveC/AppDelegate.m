//
//  AppDelegate.m
//  QiSpace_ObjectiveC_Demo
//
//  Created by Roy Zheng on 2022-06-12.
//

#import "AppDelegate.h"
#import "Sequr_Demo.h"
#import "Qeep_Demo.h"
#import "ViewController.h"

@interface AppDelegate ()

@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    return YES;
}


#pragma mark - UISceneSession lifecycle


- (UISceneConfiguration *)application:(UIApplication *)application configurationForConnectingSceneSession:(UISceneSession *)connectingSceneSession options:(UISceneConnectionOptions *)options {
    // Called when a new scene session is being created.
    // Use this method to select a configuration to create the new scene with.
    return [[UISceneConfiguration alloc] initWithName:@"Default Configuration" sessionRole:connectingSceneSession.role];
}


- (void)application:(UIApplication *)application didDiscardSceneSessions:(NSSet<UISceneSession *> *)sceneSessions {
    // Called when the user discards a scene session.
    // If any sessions were discarded while the application was not running, this will be called shortly after application:didFinishLaunchingWithOptions.
    // Use this method to release any resources that were specific to the discarded scenes, as they will not return.
}
-(void)applicationDidEnterBackground:(UIApplication *)application {
    //Free any remaining memory leaks
    NSLog(@"LEAVING...");
    if(subkey != NULL){
        free(subkey);
    }
    base64_cleanup();
    
}

@end
