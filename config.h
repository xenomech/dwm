/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx = 1; /* border pixel of windows */
static const unsigned int snap = 30;	/* snap pixel */
static const unsigned int gappih = 10;	/* horiz inner gap between windows */
static const unsigned int gappiv = 10;	/* vert inner gap between windows */
static const unsigned int gappoh = 10;	/* horiz outer gap between windows and screen edge */
static const unsigned int gappov = 10;	/* vert outer gap between windows and screen edge */
static int smartgaps = 0;				/* 1 means no outer gap when there is only one window */
static const int showbar = 1;			/* 0 means no bar */
static const int topbar = 0;			/* 0 means bottom bar */
static const int vertpad = 10;			/* vertical padding of bar */
static const int sidepad = 10;			/* horizontal padding of bar */
static const int user_bh = 0;			/* 0 means that dwm will calculate bar height, >= 1 means dwm will user_bh as bar height */
static const char *fonts[] = {"Source Code Pro:size=14", "UbuntuMono Nerd Font:pixelsize=22:antialias=true:autohint=true" ,"JoyPixels:pixelsize=14:antialias=true:autohint=true"};
static const char dmenufont[] = "Source Code Pro:size=14";
static char normbgcolor[] = "#222222";
static char normbordercolor[] = "#444444";
static char normfgcolor[] = "#bbbbbb";
static char selfgcolor[] = "#eeeeee";
static char selbordercolor[] = "#005577";
static char selbgcolor[] = "#005577";
static char *colors[][3] = {
	/*               fg           bg           border   */
	[SchemeNorm] = {normfgcolor, normbgcolor, normbordercolor},
	[SchemeSel] = {selfgcolor, selbgcolor, selbordercolor},
};

/* tagging */
static const char *tags[] = {"1", "2", "3", "4", "5", "6", "7", "8"};

/* grid of tags */
#define DRAWCLASSICTAGS 1 << 0
#define DRAWTAGGRID 1 << 1

#define SWITCHTAG_UP 1 << 0
#define SWITCHTAG_DOWN 1 << 1
#define SWITCHTAG_LEFT 1 << 2
#define SWITCHTAG_RIGHT 1 << 3
#define SWITCHTAG_TOGGLETAG 1 << 4
#define SWITCHTAG_TAG 1 << 5
#define SWITCHTAG_VIEW 1 << 6
#define SWITCHTAG_TOGGLEVIEW 1 << 7

static const unsigned int drawtagmask = DRAWTAGGRID; /* | DRAWCLASSICTAGS to show classic row of tags */
static const int tagrows = 2;

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{"Gimp", NULL, NULL, 0, 1, -1},
	{"Firefox", NULL, NULL, 1 << 8, 0, -1},
};

/* layout(s) */
static const float mfact = 0.50;  /* factor of master area size [0.05..0.95] */
static const int nmaster = 1;	  /* number of clients in master area */
static const int resizehints = 1; /* 1 means respect size hints in tiled resizals */
#define FORCE_VSPLIT 1			  /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"

static const char *upvol[] = {"/usr/bin/pactl", "set-sink-volume", "0", "+5%", NULL};
static const char *downvol[] = {"/usr/bin/pactl", "set-sink-volume", "0", "-5%", NULL};
static const char *mutevol[] = {"/usr/bin/pactl", "set-sink-mute", "0", "toggle", NULL};

static const Layout layouts[] = {

 	{ "   ",	tile },		/* 0 Default*/
	{ "  ﮆ ",	bstack },	/*1*/	

	{ "  北 ",	spiral },	/*2*/
	{ "  ﭕ ",	dwindle },	/*3*/	

	{ "   ",	deck },		/*4*/	
 	{ "   ",	monocle },	/*5*/	

	{ "   ",	centeredmaster }, /*6*/		
	{ "     ",	centeredfloatingmaster },	/*7*/

	{"  﩯 ", grid},  /*8*/
	{"  ﱖ ", gaplessgrid}, /*9*/

	{"  יִ ", horizgrid}, /*10*/

	{ "   ",	NULL },	/*11*/		/* no layout function means floating behavior */

	{ NULL,		NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY, TAG)                                          			  \
		{MODKEY, 							KEY, view, {.ui = 1 << TAG}},      \
		{MODKEY | ControlMask,				KEY, toggleview, {.ui = 1 << TAG}}, \
		{MODKEY | ShiftMask,				KEY, tag, {.ui = 1 << TAG}},         \
		{MODKEY | ControlMask | ShiftMask,  KEY, toggletag, {.ui = 1 << TAG}},

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd)                                           \
	{                                                        \
		.v = (const char *[]) { "/bin/sh", "-c", cmd, NULL } \
	}

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = {"dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbordercolor, "-sf", selfgcolor, NULL};
static const char *termcmd[] = {"st", NULL};

static Key keys[] = {
	/* modifier                     key        function        argument */
	{MODKEY, 								XK_p, spawn, {.v = dmenucmd}},
	{MODKEY | ShiftMask, 					XK_Return, spawn, {.v = termcmd}},
	{MODKEY, 								XK_b, togglebar, {0}},
	{MODKEY | ShiftMask, 					XK_j, rotatestack, {.i = +1}},
	{MODKEY | ShiftMask, 					XK_k, rotatestack, {.i = -1}},
	{MODKEY, 								XK_j, focusstack, {.i = +1}},
	{MODKEY, 								XK_k, focusstack, {.i = -1}},
	{MODKEY, 								XK_i, incnmaster, {.i = +1}},
	{MODKEY, 								XK_d, incnmaster, {.i = -1}},
	{MODKEY, 								XK_h, setmfact, {.f = -0.05}},
	{MODKEY, 								XK_l, setmfact, {.f = +0.05}},
	{MODKEY | ShiftMask, 					XK_h, setcfact, {.f = +0.25}},
	{MODKEY | ShiftMask, 					XK_l, setcfact, {.f = -0.25}},
	{MODKEY | ShiftMask, 					XK_o, setcfact, {.f = 0.00}},
	{MODKEY, 								XK_Return, zoom, {0}},

	{MODKEY | ControlMask, 					XK_k, incrgaps, {.i = +1}},
	{MODKEY | ControlMask, 					XK_j, incrgaps, {.i = -1}},

	{MODKEY | ControlMask | ShiftMask, 		XK_equal, incrigaps, {.i = +1}},
	{MODKEY | ControlMask | ShiftMask, 		XK_minus, incrigaps, {.i = -1}},

	{MODKEY | Mod4Mask, 					XK_equal, incrogaps, {.i = +1}},
	{MODKEY | Mod4Mask | ShiftMask, 		XK_minus, incrogaps, {.i = -1}},

	// {MODKEY | Mod1Mask, 					XK_equal, incrihgaps, {.i = +1}},
	// {MODKEY | Mod1Mask | ShiftMask, 		XK_minus, incrihgaps, {.i = -1}},

	// {MODKEY | Mod1Mask, 					XK_equal, incrivgaps, {.i = +1}},
	// {MODKEY | Mod1Mask | ShiftMask, 		XK_minus, incrivgaps, {.i = -1}},

	// {MODKEY | Mod1Mask, 					XK_equal, incrohgaps, {.i = +1}},
	// {MODKEY | Mod1Mask | ShiftMask, 		XK_minus, incrohgaps, {.i = -1}},

	// {MODKEY | Mod1Mask, 					XK_equal, incrovgaps, {.i = +1}},
	// {MODKEY | Mod1Mask | ShiftMask, 		XK_minus, incrovgaps, {.i = -1}},

	{MODKEY | ControlMask, 					XK_0, togglegaps, {0}},
	{MODKEY | ControlMask | ShiftMask, 		XK_0, defaultgaps, {0}},
	{MODKEY, 								XK_Tab, view, {0}},
	{MODKEY, 								XK_q, killclient, {0}},


// must say this is a lot
	{MODKEY, 								XK_t, setlayout, {.v = &layouts[0]}}, /* tile */
	{MODKEY | ShiftMask, 					XK_t, setlayout, {.v = &layouts[1]}}, /* bstack */
	{MODKEY, 								XK_y, setlayout, {.v = &layouts[2]}}, /* spiral */
	{MODKEY | ShiftMask, 					XK_y, setlayout, {.v = &layouts[3]}}, /* dwindle */
	{MODKEY, 								XK_u, setlayout, {.v = &layouts[4]}}, /* deck */
	{MODKEY | ShiftMask, 					XK_u, setlayout, {.v = &layouts[5]}}, /* monocle */
	{MODKEY, 								XK_i, setlayout, {.v = &layouts[6]}}, /* centeredmaster */
	{MODKEY | ShiftMask, 					XK_i, setlayout, {.v = &layouts[7]}}, /* centeredfloatingmaster */
	{MODKEY, 								XK_g, setlayout, {.v = &layouts[8]}}, /* Grid */
	{MODKEY | ShiftMask, 					XK_g, setlayout, {.v = &layouts[9]}}, /* Gapless Grid */
	{MODKEY, 								XK_r, setlayout, {.v = &layouts[10]}}, /* Grid */
	{MODKEY, 								XK_f, setlayout, {.v = &layouts[11]}}, /* Gapless Grid */

	
	{MODKEY, 								XK_space, setlayout, {0}},
	{MODKEY | ShiftMask, 					XK_space, togglefloating, {0}},

	{MODKEY | ShiftMask, 					XK_f, fullscreen, {0}},

	{MODKEY, 								XK_0, view, {.ui = ~0}},
	{MODKEY | ShiftMask, 					XK_0, tag, {.ui = ~0}},
	{MODKEY, 								XK_comma, focusmon, {.i = -1}},
	{MODKEY, 								XK_period, focusmon, {.i = +1}},
	{MODKEY | ShiftMask, 					XK_comma, tagmon, {.i = -1}},
	{MODKEY | ShiftMask, 					XK_period, tagmon, {.i = +1}},
	{MODKEY | ShiftMask, 					XK_c, xrdb, {.v = NULL}},

	{MODKEY | ShiftMask, 					XK_x, spawn, SHCMD("slock")},

	// { 0,                         XF86XK_AudioLowerVolume, spawn, {.v = downvol } },
	// { 0,                         XF86XK_AudioMute, spawn, {.v = mutevol } },
	// { 0,                         XF86XK_AudioRaiseVolume, spawn, {.v = upvol   } },

	{0, 								XF86XK_AudioMute, spawn, SHCMD("pamixer -t; kill -44 $(pidof dwmblocks)")},
	{0, 								XF86XK_AudioRaiseVolume, spawn, SHCMD("pamixer --allow-boost -i 3; kill -44 $(pidof dwmblocks)")},
	{0, 								XF86XK_AudioLowerVolume, spawn, SHCMD("pamixer --allow-boost -d 3; kill -44 $(pidof dwmblocks)")},
	{0, 								XF86XK_AudioPrev, spawn, SHCMD("playerctl previous")},
	{0, 								XF86XK_AudioNext, spawn, SHCMD("playerctl next")},
	{0, 								XF86XK_AudioPause, spawn, SHCMD("playerctl play-pause")},
	{0, 								XF86XK_AudioPlay, spawn, SHCMD("playerctl play-pause")},
	{0, 								XF86XK_AudioMicMute, spawn, SHCMD("pactl set-source-mute @DEFAULT_SOURCE@ toggle")},

	{MODKEY, 								XK_F11, spawn, {.v = downvol}},
	{MODKEY, 								XK_F9, spawn, {.v = mutevol}},
	{MODKEY, 								XK_F12, spawn, {.v = upvol}},

	{MODKEY | ControlMask, 					XK_n, spawn, SHCMD("st -e nmtui")},
	{MODKEY | ControlMask, 					XK_c, spawn, SHCMD("chromium")},
	{MODKEY | ControlMask, 					XK_f, spawn, SHCMD("firefox")},
	{MODKEY | ControlMask,					XK_g, spawn, SHCMD("brave")},
	{MODKEY | ControlMask, 					XK_b, spawn, SHCMD("blueman-manager")},

	{0, 								XK_Print, spawn, SHCMD("maim ~/screenshots/pic-full-$(date '+%y%m%d-%H%M-%S').png")},

	TAGKEYS(XK_1, 0)
		TAGKEYS(XK_2, 1)
			TAGKEYS(XK_3, 2)
				TAGKEYS(XK_4, 3)
					TAGKEYS(XK_5, 4)
						TAGKEYS(XK_6, 5)
							TAGKEYS(XK_7, 6)
								TAGKEYS(XK_8, 7)
									TAGKEYS(XK_9, 8){MODKEY | ShiftMask, XK_q, quit, {0}},

	{MODKEY | ControlMask, 					XK_Up, switchtag, {.ui = SWITCHTAG_UP | SWITCHTAG_VIEW}},
	{MODKEY | ControlMask, 					XK_Down, switchtag, {.ui = SWITCHTAG_DOWN | SWITCHTAG_VIEW}},
	{MODKEY | ControlMask, 					XK_Right, switchtag, {.ui = SWITCHTAG_RIGHT | SWITCHTAG_VIEW}},
	{MODKEY | ControlMask, 					XK_Left, switchtag, {.ui = SWITCHTAG_LEFT | SWITCHTAG_VIEW}},

	{MODKEY | Mod4Mask, 					XK_Up, switchtag, {.ui = SWITCHTAG_UP | SWITCHTAG_TAG | SWITCHTAG_VIEW}},
	{MODKEY | Mod4Mask, 					XK_Down, switchtag, {.ui = SWITCHTAG_DOWN | SWITCHTAG_TAG | SWITCHTAG_VIEW}},
	{MODKEY | Mod4Mask, 					XK_Right, switchtag, {.ui = SWITCHTAG_RIGHT | SWITCHTAG_TAG | SWITCHTAG_VIEW}},
	{MODKEY | Mod4Mask, 					XK_Left, switchtag, {.ui = SWITCHTAG_LEFT | SWITCHTAG_TAG | SWITCHTAG_VIEW}},
};
/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ClkLtSymbol, 0, Button1, setlayout, {0}},
	{ClkLtSymbol, 0, Button3, setlayout, {.v = &layouts[2]}},
	{ClkStatusText, 0, Button2, spawn, {.v = termcmd}},
	{ClkClientWin, MODKEY, Button1, movemouse, {0}},
	{ClkClientWin, MODKEY, Button2, togglefloating, {0}},
	{ClkClientWin, MODKEY, Button3, resizemouse, {0}},
	{ClkTagBar, 0, Button1, view, {0}},
	{ClkTagBar, 0, Button3, toggleview, {0}},
	{ClkTagBar, MODKEY, Button1, tag, {0}},
	{ClkTagBar, MODKEY, Button3, toggletag, {0}},
};
