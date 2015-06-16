/* Declarations for recur.c.
   Copyright (C) 1995, 1996, 1997 Free Software Foundation, Inc.

This file is part of Wget.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#ifndef RECUR_H
#define RECUR_H

void recursive_cleanup PARAMS ((void));
void recursive_reset PARAMS ((void));
uerr_t recursive_retrieve PARAMS ((const char *, const char *));

void convert_all_links PARAMS ((void));

#endif /* RECUR_H */
