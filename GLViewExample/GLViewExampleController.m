//
//  GLViewViewController.m
//
//  Created by Nick Lockwood on 09/07/2011.
//  Copyright 2011 Charcoal Design. All rights reserved.
//

#import "GLViewExampleController.h"


@implementation GLViewExampleController

@synthesize imageView1;
@synthesize imageView2;


- (void)viewDidLoad
{
    [super viewDidLoad];
    
    imageView1.image = [GLImage imageNamed:@"logo.pvr"];
	imageView2.image = [GLImage imageNamed:@"Image.png"];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation
{
    return YES;
}

- (void)dealloc
{
    [imageView1 release];
    [imageView2 release];
    [super dealloc];
}

- (IBAction) onSlider:(id)sender{
    UISlider *slider = (UISlider*)sender;
        
    float value = slider.value;
    NSLog(@"%s %f ",__PRETTY_FUNCTION__,value);

    
    [imageView2 drawView:value];
}

@end
