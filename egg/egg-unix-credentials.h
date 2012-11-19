/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/* egg-unix-credentials.h - write and read unix credentials on socket

   Copyright (C) 2008 Stefan Walter

   Mate keyring is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
  
   Mate keyring is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
  
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.

   Author: Stef Walter <stef@memberwebs.com>
*/

#ifndef EGGUNIXCREDENTIALS_H_
#define EGGUNIXCREDENTIALS_H_

#include <unistd.h>

int        egg_unix_credentials_read           (int sock,
                                                pid_t *pid,
                                                uid_t *uid);

int        egg_unix_credentials_write          (int sock);

int        egg_unix_credentials_setup          (int sock);

char*      egg_unix_credentials_executable     (pid_t pid);

#endif /*EGGUNIXCREDENTIALS_H_*/
