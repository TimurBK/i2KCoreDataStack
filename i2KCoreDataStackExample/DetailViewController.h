//
//  DetailViewController.h
//  i2KCoreDataStack
//
//  Created by Timur Kuchkarov on 28.03.16.
//  Copyright © 2016 Timur Kuchkarov. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface DetailViewController : UIViewController

@property (strong, nonatomic) id detailItem;
@property (weak, nonatomic) IBOutlet UILabel *detailDescriptionLabel;

@end

