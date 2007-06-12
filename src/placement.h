/*      $Id$

        This program is free software; you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation; either version 2, or (at your option)
        any later version.

        This program is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.

        You should have received a copy of the GNU General Public License
        along with this program; if not, write to the Free Software
        Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

        xfwm4    - (c) 2002-2007 Olivier Fourdan

 */

#ifndef INC_PLACEMENT_H
#define INC_PLACEMENT_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib.h>
#include "client.h"

#define CLIENT_CONSTRAINED_TOP     1<<0
#define CLIENT_CONSTRAINED_BOTTOM  1<<1
#define CLIENT_CONSTRAINED_LEFT    1<<2
#define CLIENT_CONSTRAINED_RIGHT   1<<3

void                     clientMaxSpace                         (ScreenInfo *,
                                                                 int *,
                                                                 int *,
                                                                 int *,
                                                                 int *);
gboolean                 clientCkeckTitle                       (Client *);
unsigned int             clientConstrainPos                     (Client *,
                                                                 gboolean);
void                     clientInitPosition                     (Client *);

#endif /* INC_PLACEMENT_H */
