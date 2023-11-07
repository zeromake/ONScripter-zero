//
//  ViewController.m
//  onscripter
//
//  Created by zero on 2022/12/20.
//

#import "ViewController.h"
#include <SDL2/SDL.h>

@interface ViewController ()

@end

@implementation ViewController

- (NSString*) get_script_dir{
    return script_dir;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    data = [NSMutableArray arrayWithObjects:@"1", nil];
    [self updateFileList];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:
  (NSInteger)section{
    return [data count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:
  (NSIndexPath *)indexPath{
    static NSString *cellIdentifier = @"cellID";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:
    cellIdentifier];
    if (cell == nil) {
        cell = [[UITableViewCell alloc]initWithStyle:
        UITableViewCellStyleDefault reuseIdentifier:cellIdentifier];
    }
    [cell.textLabel setText:[data objectAtIndex:indexPath.row]];
    return cell;
}
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath*)indexPath {
    NSString *name = [data objectAtIndex:indexPath.row];
    script_dir = [docpath stringByAppendingPathComponent:name];
    SDL_Event event;
    event.type = SDL_QUIT;
    SDL_PushEvent(&event);
}

- (void) updateFileList{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    docpath = [[paths objectAtIndex:0] stringByAppendingPathComponent:@"game"];
    printf("docpath: %s\n", [docpath UTF8String]);
    NSFileManager *fm = [NSFileManager defaultManager];
    if (![fm fileExistsAtPath:docpath]) {
        return;
    }
    [data removeAllObjects];
    NSArray *arr = [fm contentsOfDirectoryAtPath:docpath error:nil];
    for (NSString *item in arr){
        NSString *path = [docpath stringByAppendingPathComponent:item];
        BOOL is_dir;
        [fm fileExistsAtPath:path isDirectory:&is_dir];
        if (!is_dir) continue;
        if ([fm fileExistsAtPath:[path stringByAppendingPathComponent:@"0.txt"]] ||
            [fm fileExistsAtPath:[path stringByAppendingPathComponent:@"00.txt"]] ||
            [fm fileExistsAtPath:[path stringByAppendingPathComponent:@"nscr_sec.dat"]] ||
            [fm fileExistsAtPath:[path stringByAppendingPathComponent:@"nscript.___"]] ||
            [fm fileExistsAtPath:[path stringByAppendingPathComponent:@"nscript.dat"]] ||
            [fm fileExistsAtPath:[path stringByAppendingPathComponent:@"onscript.nt"]] ||
            [fm fileExistsAtPath:[path stringByAppendingPathComponent:@"onscript.nt2"]] ||
            [fm fileExistsAtPath:[path stringByAppendingPathComponent:@"onscript.nt3"]])
            [data addObject:item];
    }
}
@end
