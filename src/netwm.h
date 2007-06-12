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

#ifndef INC_NETWM_H
#define INC_NETWM_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <glib.h>
#include "screen.h"
#include "client.h"

void                     clientSetNetState                      (Client *);
void                     clientGetNetState                      (Client *);
void                     clientUpdateNetState                   (Client *,
                                                                 XClientMessageEvent *);
void                     clientNetMoveResize                    (Client *,
                                                                 XClientMessageEvent *);
void                     clientUpdateFullscreenState            (Client *);
void                     clientGetNetWmType                     (Client *);
void                     clientGetInitialNetWmDesktop           (Client *);
void                     clientSetNetClientList                 (ScreenInfo *,
                                                                 Atom,
                                                                 GList *);
gboolean                 clientValidateNetStrut                 (Client *);
gboolean                 clientGetNetStruts                     (Client *);
void                     clientSetNetActions                    (Client *);
void                     clientWindowType                       (Client *);
void                     clientUpdateAboveState                 (Client *);
void                     clientUpdateBelowState                 (Client *);

#endif /* INC_NETWM_H */
