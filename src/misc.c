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

        oroborus - (c) 2001 Ken Lynch
        xfwm4    - (c) 2002-2007 Olivier Fourdan

 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xmd.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <libxfce4util/libxfce4util.h>

#include "display.h"
#include "screen.h"
#include "mywindow.h"
#include "client.h"
#include "misc.h"

unsigned int
getMouseXY (ScreenInfo *screen_info, Window w, int *x2, int *y2)
{
    Window w1, w2;
    unsigned int mask;
    int x1, y1;

    TRACE ("entering getMouseXY");

    XQueryPointer (myScreenGetXDisplay (screen_info), w, &w1, &w2, &x1, &y1, x2, y2, &mask);
    return mask;
}

Window
getMouseWindow (ScreenInfo *screen_info, Window w)
{
    Window w1, w2;
    unsigned int mask;
    int x1, y1, x2, y2;

    TRACE ("entering getMouseWindow");

    XQueryPointer (myScreenGetXDisplay (screen_info), w, &w1, &w2, &x1, &y1, &x2, &y2, &mask);
    return w2;
}

GC
createGC (ScreenInfo *screen_info, char *col, int func, XFontStruct * font,
          int line_width, gboolean inc_sw)
{
    XGCValues gv;
    XColor xc1, xc2;
    GC gc;
    int mask;

    TRACE ("entering createGC");
    TRACE ("color=%s", col);

    mask = GCForeground | GCFunction;
    XAllocNamedColor (myScreenGetXDisplay (screen_info), screen_info->cmap, col, &xc1, &xc2);
    gv.foreground = xc2.pixel;
    gv.function = func;
    if (font)
    {
        gv.font = font->fid;
        mask = mask | GCFont;
    }
    if (inc_sw)
    {
        gv.subwindow_mode = IncludeInferiors;
        mask = mask | GCSubwindowMode;
    }
    if (line_width > -1)
    {
        gv.line_width = line_width;
        mask = mask | GCLineWidth;
    }
    gc = XCreateGC (myScreenGetXDisplay (screen_info), screen_info->xroot, mask, &gv);
    return gc;
}

void
sendClientMessage (ScreenInfo *screen_info, Window w, int atom_id, Time timestamp)
{
    DisplayInfo *display_info;
    XClientMessageEvent ev;

    g_return_if_fail ((atom_id > 0) && (atom_id < ATOM_COUNT));
    TRACE ("entering sendClientMessage");

    display_info = screen_info->display_info;
    ev.type = ClientMessage;
    ev.window = w;
    ev.message_type = display_info->atoms[WM_PROTOCOLS];
    ev.format = 32;
    ev.send_event = TRUE;
    ev.data.l[0] = display_info->atoms[atom_id];
    ev.data.l[1] = timestamp;
    XSendEvent (myScreenGetXDisplay (screen_info), w, FALSE, 0L, (XEvent *)&ev);
}

void
sendRootMessage (ScreenInfo *screen_info, int atom_id, long value, Time timestamp)
{
    DisplayInfo *display_info;
    XClientMessageEvent ev;

    g_return_if_fail ((atom_id > 0) && (atom_id < ATOM_COUNT));
    TRACE ("entering sendClientMessage");

    display_info = screen_info->display_info;
    ev.type = ClientMessage;
    ev.window = screen_info->xroot;
    ev.message_type = display_info->atoms[atom_id];
    ev.format = 32;
    ev.data.l[0] = value;
    ev.data.l[1] = timestamp;
    XSendEvent (myScreenGetXDisplay (screen_info), screen_info->xroot, FALSE,
                SubstructureRedirectMask | SubstructureNotifyMask, (XEvent *)&ev);
}

/*
 * it's safer to grab the display before calling this routine
 * Returns true if the given window is present and mapped on root
 */
gboolean
checkWindowOnRoot(ScreenInfo *screen_info, Window w)
{
    DisplayInfo *display_info;
    Window dummy_root, parent;
    Window *wins;
    Status test;
    unsigned int count;

    g_return_val_if_fail (screen_info != NULL, FALSE);
    g_return_val_if_fail (w != None, FALSE);

    display_info = screen_info->display_info;
    wins = NULL;

    gdk_error_trap_push ();
    test = XQueryTree(display_info->dpy, w, &dummy_root, &parent, &wins, &count);
    if (wins)
    {
        XFree (wins);
    }
    return (!gdk_error_trap_pop () && (test != 0) && (dummy_root == parent));
}

void
placeSidewalks(ScreenInfo *screen_info, gboolean activate)
{
    NetWmDesktopLayout l;

    g_return_if_fail (MYWINDOW_XWINDOW (screen_info->sidewalk[0]) != None);
    g_return_if_fail (MYWINDOW_XWINDOW (screen_info->sidewalk[1]) != None);
    g_return_if_fail (MYWINDOW_XWINDOW (screen_info->sidewalk[2]) != None);
    g_return_if_fail (MYWINDOW_XWINDOW (screen_info->sidewalk[3]) != None);

    l = screen_info->desktop_layout;
    if ((activate) && (l.cols > 1))
    {
        /*left*/
        xfwmWindowShow (&screen_info->sidewalk[0],
                        0, 0,
                        1, screen_info->height, FALSE);

        /*right*/
         xfwmWindowShow (&screen_info->sidewalk[1],
                        screen_info->width - 1, 0,
                        1, screen_info->height, FALSE);
    }
    else /* Place the windows off screen */
    {
        /*left*/
        xfwmWindowShow (&screen_info->sidewalk[0],
                        -1, 0,
                        1, screen_info->height, FALSE);

        /*right*/
        xfwmWindowShow (&screen_info->sidewalk[1],
                        screen_info->width, 0,
                        1, screen_info->height, FALSE);
    }

    if ((activate) && (l.rows > 1))
    {
        /*top*/
        xfwmWindowShow (&screen_info->sidewalk[2],
                        0, 0,
                        screen_info->width, 1, FALSE);

        /*bottom*/
        xfwmWindowShow (&screen_info->sidewalk[3],
                        0, screen_info->height - 1,
                        screen_info->width, 1, FALSE);
    }
    else /* Place the windows off screen */
    {
        /*top*/
        xfwmWindowShow (&screen_info->sidewalk[2],
                        0, -1,
                        screen_info->width, 1, FALSE);

        /*bottom*/
        xfwmWindowShow (&screen_info->sidewalk[3],
                        0, screen_info->height,
                        screen_info->width, 1, FALSE);
    }
}

/*
   gdk_screen_get_monitor_at_point () doesn't give accurate results
   when the point is off screen, use my own implementation from xfce 3
 */
gint
find_monitor_at_point (GdkScreen *screen, gint x, gint y)
{
    static gint cache_monitor = -1;
    static gint cache_x, cache_y;
    gint dx, dy, center_x, center_y;
    guint32 distsquare, min_distsquare;
    gint num_monitors, nearest_monitor, i;
    GdkRectangle monitor;

    g_return_val_if_fail (GDK_IS_SCREEN (screen), -1);

    /* Cache system */
    if ((cache_monitor >= 0) && (x == cache_x) && (y == cache_y))
    {
        return (cache_monitor);
    }

    cache_x = x;
    cache_y = y;

    /* No monitor has been eligible, use the closest one */

    min_distsquare = ((guint32) 0xffffffff);
    nearest_monitor = 0;

    num_monitors = gdk_screen_get_n_monitors (screen);
    for (i = 0; i < num_monitors; i++)
    {
        gdk_screen_get_monitor_geometry (screen, i, &monitor);

        if ((x >= monitor.x) && (x < monitor.x + monitor.width) &&
            (y >= monitor.y) && (y < (monitor.y + monitor.height)))
        {
            cache_monitor = i;
            return i;
        }

        center_x = monitor.x + (monitor.width / 2);
        center_y = monitor.y + (monitor.height / 2);

        dx = x - center_x;
        dy = y - center_y;

        distsquare = (dx * dx) + (dy * dy);

        if (distsquare < min_distsquare)
        {
            min_distsquare = distsquare;
            nearest_monitor = i;
        }
    }

    cache_monitor = nearest_monitor;
    return (nearest_monitor);
}
