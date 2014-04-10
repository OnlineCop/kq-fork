/*! \page License
   KQ is Copyright (C) 2002 by Josh Bolduc

   This file is part of KQ... a freeware RPG.

   KQ is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 2, or (at your
   option) any later version.

   KQ is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with KQ; see the file COPYING.  If not, write to
   the Free Software Foundation,
       675 Mass Ave, Cambridge, MA 02139, USA.
*/


/*! \file
 * \brief Specifics for Mac platforms
 *
 * \author PH
 * \date 20030723
 *
 * This includes any bits which are specific for Mac OSX  platforms
 */

#import <Foundation/Foundation.h>
#include <string.h>

#include <allegro.h>
#include "platform.h"

static NSString *user_dir;
static NSString *game_dir;
static BOOL init_path = NO;
static NSString* get_resource_file_path(NSString* base, NSString* subdir, NSString* file);
static NSString* get_lua_file_path(NSString* base, NSString* file);


/*! \brief Returns the full path for this file
 *
 * This function first checks if the file can be found in the user's
 * directory. If it can not, it checks the relevant game directory
 * (data, music, lib, etc)
 *
 * \param base The first part of the string, assuming the file can't be
 * found in user_dir (eg. "/usr/local/share/kq")
 * \param subdir The second part of the string (eg. "maps")
 * \param file The filename
 * \returns the combined path
 */
static NSString* get_resource_file_path(NSString* base, NSString* subdir, NSString* file)
{
   NSFileManager* fm = [NSFileManager defaultManager];
   NSString* fullpath = [NSString pathWithComponents: [NSArray arrayWithObjects:user_dir, subdir, file, nil]];

   if ([fm fileExistsAtPath:fullpath]) {
      return fullpath;
   }

   fullpath = [NSString pathWithComponents: [NSArray arrayWithObjects:base, subdir, file, nil]];

   /* return this, even if it doesn't exist */
   return fullpath;
}



/*! \brief Returns the full path for this lua file
 *
 * This function first checks if the lua file can be found in the user's
 * directory. If it can not, it checks the relavent game directory
 * (data, music, lib, etc). For each directory, it first checks for a lob
 * file, and then it checks for a lua file. This function is similar to
 * get_resource_file_path, but takes special considerations for lua files.
 * Whereas get_resource_file_path takes the full filename (eg. "main.map"),
 * this function takes the filename without extension (eg "main").
 *
 * \param base The first part of the string, assuming the file can't be
 * found in user_dir (eg. "/usr/local/lib/kq")
 * \param file The filename
 * \returns the combined path
 */
NSString* get_lua_file_path(NSString* base, NSString* file) {
   NSFileManager* fm = [NSFileManager defaultManager];
   NSString* path = [NSString pathWithComponents: [NSArray arrayWithObjects:user_dir, @"scripts", file, nil]];
   NSString* fullpath = [path stringByAppendingPathExtension: @"lua"];

   if ([fm fileExistsAtPath:fullpath]) {
      return fullpath;
   }

   fullpath = [path stringByAppendingPathExtension: @"lob"];
   if ([fm fileExistsAtPath:fullpath]) {
      return fullpath;
   }

   path = [NSString pathWithComponents: [NSArray arrayWithObjects:base, @"scripts", file, nil]];
   fullpath = [path stringByAppendingPathExtension: @"lua"];
   if ([fm fileExistsAtPath:fullpath]) {
      return fullpath;
   }

   fullpath = [path stringByAppendingPathExtension: @"lob"];
   if ([fm fileExistsAtPath:fullpath]) {
      return fullpath;
   }
   return nil;
}



/*! \brief Return the name of 'significant' directories.
 *
 * \param dir Enumerated constant for directory type  \sa DATA_DIR et al.
 * \param file File name below that directory.
 * \returns The combined path
 */
const char *kqres (enum eDirectories dir, const char *file)
{
   static char ans[PATH_MAX];
   NSString* found;
   NSString* nsfile = [NSString stringWithUTF8String:file];
   if (init_path == NO) {
      /* Get home directory */

      NSArray* arr = NSSearchPathForDirectoriesInDomains (NSLibraryDirectory,
                     NSUserDomainMask, YES);
      user_dir = [[arr objectAtIndex:0] stringByAppendingPathComponent: @"KQ"];
      [user_dir retain];
      [[NSFileManager defaultManager] createDirectoryAtPath:user_dir
         withIntermediateDirectories:YES attributes:nil error:nil];

      /* Now the data directory */
      game_dir =[[[NSBundle mainBundle] bundlePath] stringByAppendingPathComponent: @"Contents/Resources"];
      [game_dir retain];
/*    NSLog(@"Putting user data in %@, game data in %@\n", user_dir, game_dir); */
      init_path = YES;
   }

   switch (dir) {
      case DATA_DIR:
         found = get_resource_file_path(game_dir, @"data", nsfile);
         break;
      case MUSIC_DIR:
         found = get_resource_file_path(game_dir, @"music", nsfile);
         break;
      case MAP_DIR:
         found = get_resource_file_path(game_dir, @"maps", nsfile);
         break;
      case SAVE_DIR:
      case SETTINGS_DIR:
         found = get_resource_file_path(user_dir, @".", nsfile);
         break;
      case SCRIPT_DIR:
         found = get_lua_file_path(game_dir, nsfile);
         break;
      default:
         found = nil;
         break;
   }
   /* Return UTF8 string for Allegro */
   return (found == nil ? NULL : strncpy (ans, [found UTF8String], sizeof(ans)));
}
