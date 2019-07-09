- (void)postFinishLaunch
{
    /* Hide the launch screen the next time the run loop is run. SDL apps will
     * have a chance to load resources while the launch screen is still up. */
    [self performSelector:@selector(hideLaunchScreen) withObject:nil afterDelay:0.0];

    // we check at application start to see if the mpq is present.
    // if it is, we pass that full path to the SDL_main event loop
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    NSArray *filePathsArray = [[NSFileManager defaultManager] subpathsOfDirectoryAtPath:documentsDirectory  error:nil];
    
    if ([filePathsArray count] > 0){
        
        NSString* fullPath = [NSString pathWithComponents:@[documentsDirectory, filePathsArray[0]]];
        
        SDL_iPhoneSetEventPump(SDL_TRUE);
        
        char * custom_array[2];
        custom_array[1] = [fullPath UTF8String];
        
        exit_status = SDL_main(2, custom_array);
        
    } else {
        
        SDL_iPhoneSetEventPump(SDL_TRUE);
        exit_status = SDL_main(forward_argc, forward_argv);
        
    }
    SDL_iPhoneSetEventPump(SDL_FALSE);

    if (launchWindow) {
        launchWindow.hidden = YES;
        launchWindow = nil;
    }

    /* exit, passing the return status from the user's application */
    /* We don't actually exit to support applications that do setup in their
     * main function and then allow the Cocoa event loop to run. */
     exit(exit_status);
}