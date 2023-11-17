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
    return _script_dir;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    _data = [[NSMutableArray alloc] init];
    [self updateFileList];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:
  (NSInteger)section{
    return [_data count];
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
    [cell.textLabel setText:[_data objectAtIndex:indexPath.row]];
    return cell;
}
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath*)indexPath {
    NSString *name = [_data objectAtIndex:indexPath.row];
    _script_dir = [_docpath stringByAppendingPathComponent:name];
    SDL_Event event;
    event.type = SDL_QUIT;
    SDL_PushEvent(&event);
}

- (void) updateFileList{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    _docpath = [[paths objectAtIndex:0] stringByAppendingPathComponent:@"game"];
    printf("docpath: %s\n", [_docpath UTF8String]);
    NSFileManager *fm = [NSFileManager defaultManager];
    if (![fm fileExistsAtPath: _docpath]) {
        return;
    }
    [_data removeAllObjects];
    NSArray *arr = [fm contentsOfDirectoryAtPath: _docpath error:nil];
    for (NSString *item in arr){
        NSString *path = [_docpath stringByAppendingPathComponent:item];
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
            [_data addObject:item];
    }
}
@end
