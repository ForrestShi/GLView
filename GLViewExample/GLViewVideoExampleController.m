//
//  GLViewVideoExampleController.m
//  GLView
//
//  Created by Nick Lockwood on 19/07/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "GLViewVideoExampleController.h"


#define NUMBER_OF_FRAMES 604


@implementation GLViewVideoExampleController

@synthesize videoView;

- (void)viewDidLoad
{
    [super viewDidLoad];
    
	//create array of video frame names
	NSMutableArray *frames = [NSMutableArray arrayWithCapacity:NUMBER_OF_FRAMES];
	for (int i = 0; i < NUMBER_OF_FRAMES; i++)
	{
		[frames addObject:[NSString stringWithFormat:@"droplet%03i.pvr", i + 1]]; 
	}
	
	//add frames to image view
    videoView.animationImages = frames;
	[videoView startAnimating];
}

- (IBAction)play
{
	[videoView startAnimating];
}

- (IBAction)stop
{
	[videoView stopAnimating];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation
{
    return YES;
}

- (void)dealloc
{
    [videoView release];
    [super dealloc];
}

@end
