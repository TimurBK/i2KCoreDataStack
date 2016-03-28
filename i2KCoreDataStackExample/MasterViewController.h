//
//  MasterViewController.h
//  i2KCoreDataStack
//
//  Created by Timur Kuchkarov on 28.03.16.
//  Copyright © 2016 Timur Kuchkarov. All rights reserved.
//

#import <UIKit/UIKit.h>

@class DetailViewController;

@interface MasterViewController : UITableViewController

@property (strong, nonatomic) DetailViewController *detailViewController;


@end

