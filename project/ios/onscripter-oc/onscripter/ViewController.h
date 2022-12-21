//
//  ViewController.h
//  onscripter
//
//  Created by zero on 2022/12/20.
//

#import <UIKit/UIKit.h>

@interface ViewController : UIViewController<UITableViewDataSource,UITableViewDelegate> {
    IBOutlet UITableView *tableView;
    NSMutableArray *data;
}

@end

