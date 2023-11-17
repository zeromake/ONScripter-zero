//
//  ViewController.h
//  onscripter
//
//  Created by zero on 2022/12/20.
//

#import <UIKit/UIKit.h>

@interface ViewController : UIViewController<UITableViewDataSource,UITableViewDelegate> {
    IBOutlet UITableView *tableView;
}
@property (nonatomic, strong) NSMutableArray *data;
@property (nonatomic, copy) NSString *docpath;
@property (nonatomic, copy) NSString *script_dir;
-(NSString*) get_script_dir;
@end
