/* Copyright (C) 2007 L. Donnie Smith <wiimote@abstrakraft.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *  ChangeLog:
 *  03/02/2007 L. Donnie Smith <cwiid@abstrakraft.rg>
 *  * Initial ChangeLog
 *  * type audit (stdint, const, char booleans)
 */

#ifndef CONF_H
#define CONF_H

#include <stdint.h>
#include <linux/input.h>
#include <linux/uinput.h>

#include "wiimote.h"
#include "wmplugin.h"
#include "y.tab.h"

#define CONF_WM	1
#define CONF_NC	2
#define CONF_CC	3

#define CONF_WM_BTN_UP		0
#define CONF_WM_BTN_DOWN	1
#define CONF_WM_BTN_LEFT	2
#define CONF_WM_BTN_RIGHT	3
#define CONF_WM_BTN_A		4
#define CONF_WM_BTN_B		5
#define CONF_WM_BTN_MINUS	6
#define CONF_WM_BTN_PLUS	7
#define CONF_WM_BTN_HOME	8
#define CONF_WM_BTN_1		9
#define CONF_WM_BTN_2		10

#define CONF_NC_BTN_C		0
#define CONF_NC_BTN_Z		1

#define CONF_CC_BTN_UP		0
#define CONF_CC_BTN_DOWN	1
#define CONF_CC_BTN_LEFT	2
#define CONF_CC_BTN_RIGHT	3
#define CONF_CC_BTN_MINUS	4
#define CONF_CC_BTN_PLUS	5
#define CONF_CC_BTN_HOME	6
#define CONF_CC_BTN_A		7
#define CONF_CC_BTN_B		8
#define CONF_CC_BTN_X		9
#define CONF_CC_BTN_Y		10
#define CONF_CC_BTN_ZL		11
#define CONF_CC_BTN_ZR		12
#define CONF_CC_BTN_L		13
#define CONF_CC_BTN_R		14

#define CONF_WM_BTN_COUNT	11
#define CONF_NC_BTN_COUNT	2
#define CONF_CC_BTN_COUNT	15

#define CONF_WM_AXIS_DPAD_X		0
#define CONF_WM_AXIS_DPAD_Y		1
#define CONF_NC_AXIS_STICK_X	2
#define CONF_NC_AXIS_STICK_Y	3
#define CONF_CC_AXIS_DPAD_X		4
#define CONF_CC_AXIS_DPAD_Y		5
#define CONF_CC_AXIS_L_STICK_X	6
#define CONF_CC_AXIS_L_STICK_Y	7
#define CONF_CC_AXIS_R_STICK_X	8
#define CONF_CC_AXIS_R_STICK_Y	9
#define CONF_CC_AXIS_L			10
#define CONF_CC_AXIS_R			11

#define CONF_AXIS_COUNT		12

#define CONF_MAX_PLUGINS	6

#define CONF_ABS	EV_ABS
#define CONF_REL	EV_REL

/* flags */
#define CONF_INVERT		0x01
#define CONF_POINTER	0x02

#define UINPUT_NAME		"Nintendo Wiimote"
#ifdef BUS_BLUETOOTH
#define UINPUT_BUSTYPE	BUS_BLUETOOTH
#else
#define UINPUT_BUSTYPE BUS_USB
#endif
#define UINPUT_VENDOR	0x0001
#define UINPUT_PRODUCT	0x0001
#define UINPUT_VERSION	0x0001

struct lookup_enum {
	const char *name;
	__u16 value;
};

struct btn_map {
	unsigned char active;
	uint16_t mask;
	__u16 action;
};

struct axis_map {
	unsigned char active;
	__u16 axis_type;
	__u16 action;
	uint8_t flags;
};

struct plugin {
	char *name;
	struct wmplugin_info *info;
	void *handle;
	wmplugin_init_t *init;
	wmplugin_exec_t *exec;
	uint8_t rpt_mode_flags;
	uint16_t prev_buttons;
	struct btn_map bmap[WMPLUGIN_MAX_BUTTON_COUNT];
	struct axis_map amap[WMPLUGIN_MAX_AXIS_COUNT];
};

#define CONF_MAX_INCLUDE_DEPTH	10

struct conf {
	int fd;
	char **config_search_dirs;
	char **plugin_search_dirs;
	char *current_config_filename;
	int stack_index;
	char *config_filename_stack[CONF_MAX_INCLUDE_DEPTH];
	YYLTYPE yyloc_stack[CONF_MAX_INCLUDE_DEPTH];
	uint8_t rpt_mode_flags;
	struct uinput_user_dev dev;
	unsigned char ff;
	struct btn_map wiimote_bmap[CONF_WM_BTN_COUNT];
	struct btn_map nunchuk_bmap[CONF_NC_BTN_COUNT];
	struct btn_map classic_bmap[CONF_CC_BTN_COUNT];
	struct axis_map amap[CONF_AXIS_COUNT];
	struct plugin plugins[CONF_MAX_PLUGINS];
};

struct uinput_listen_data {
	wiimote_t *wiimote;
	struct conf *conf;
};

int conf_load(struct conf *conf, const char *conf_name,
              char *config_search_dirs[], char *plugin_search_dirs[]);
int conf_unload(struct conf *conf);

int conf_ff(struct conf *conf, unsigned char enabled);
int conf_button(struct conf *conf, int source, __u16 button, __u16 action);
int conf_axis(struct conf *conf, int axis, __u16 axis_type, __u16 action,
              char flags);
int conf_plugin_button(struct conf *conf, const char *name, const char *button,
                       __u16 action);
int conf_plugin_axis(struct conf *conf, const char *name, const char *axis,
                     __u16 axis_type, __u16 action, char flags);

void conf_init(struct conf *conf);
FILE *conf_push_config(struct conf *conf, const char *filename, YYLTYPE *yyloc);
int conf_pop_config(struct conf *conf, YYLTYPE *yyloc);
int lookup_action(const char *str_action);

int uinput_open(struct conf *conf);
int uinput_close(struct conf *conf);
int send_event(struct conf *conf, __u16 type, __u16 code, __s32 value);
void *uinput_listen(struct uinput_listen_data *data);

#endif

