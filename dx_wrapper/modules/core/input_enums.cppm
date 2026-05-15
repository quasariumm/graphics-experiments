module;

#include <windows.h>

export module dx_wrapper.core.input_enums;
import std;

export enum class MouseButton : std::uint8_t {
	Left   = VK_LBUTTON,  // 0x01
	Right  = VK_RBUTTON,  // 0x02
	Middle = VK_MBUTTON,  // 0x04
	X1	   = VK_XBUTTON1, // 0x05
	X2	   = VK_XBUTTON2, // 0x06
};

export enum class GamepadButton : std::uint8_t {
	A				= VK_GAMEPAD_A,						  // 0xC3
	B				= VK_GAMEPAD_B,						  // 0xC4
	X				= VK_GAMEPAD_X,						  // 0xC5
	Y				= VK_GAMEPAD_Y,						  // 0xC6
	RightShoulder	= VK_GAMEPAD_RIGHT_SHOULDER,		  // 0xC7
	LeftShoulder	= VK_GAMEPAD_LEFT_SHOULDER,			  // 0xC8
	LeftTrigger		= VK_GAMEPAD_LEFT_TRIGGER,			  // 0xC9
	RightTrigger	= VK_GAMEPAD_RIGHT_TRIGGER,			  // 0xCA
	DpadUp			= VK_GAMEPAD_DPAD_UP,				  // 0xCB
	DpadDown		= VK_GAMEPAD_DPAD_DOWN,				  // 0xCC
	DpadLeft		= VK_GAMEPAD_DPAD_LEFT,				  // 0xCD
	DpadRight		= VK_GAMEPAD_DPAD_RIGHT,			  // 0xCE
	Menu			= VK_GAMEPAD_MENU,					  // 0xCF
	View			= VK_GAMEPAD_VIEW,					  // 0xD0
	LeftThumb		= VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON,  // 0xD1
	RightThumb		= VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON, // 0xD2
	LeftThumbUp		= VK_GAMEPAD_LEFT_THUMBSTICK_UP,	  // 0xD3
	LeftThumbDown	= VK_GAMEPAD_LEFT_THUMBSTICK_DOWN,	  // 0xD4
	LeftThumbRight	= VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT,	  // 0xD5
	LeftThumbLeft	= VK_GAMEPAD_LEFT_THUMBSTICK_LEFT,	  // 0xD6
	RightThumbUp	= VK_GAMEPAD_RIGHT_THUMBSTICK_UP,	  // 0xD7
	RightThumbDown	= VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN,	  // 0xD8
	RightThumbRight = VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT,  // 0xD9
	RightThumbLeft	= VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT,	  // 0xDA
};

export enum class Key : std::uint8_t {
	// Control
	Backspace	= VK_BACK,	   // 0x08
	Tab			= VK_TAB,	   // 0x09
	Clear		= VK_CLEAR,	   // 0x0C
	Enter		= VK_RETURN,   // 0x0D
	Shift		= VK_SHIFT,	   // 0x10
	Ctrl		= VK_CONTROL,  // 0x11
	Alt			= VK_MENU,	   // 0x12
	Pause		= VK_PAUSE,	   // 0x13
	CapsLock	= VK_CAPITAL,  // 0x14
	Escape		= VK_ESCAPE,   // 0x1B
	Space		= VK_SPACE,	   // 0x20
	PageUp		= VK_PRIOR,	   // 0x21
	PageDown	= VK_NEXT,	   // 0x22
	End			= VK_END,	   // 0x23
	Home		= VK_HOME,	   // 0x24
	Left		= VK_LEFT,	   // 0x25
	Up			= VK_UP,	   // 0x26
	Right		= VK_RIGHT,	   // 0x27
	Down		= VK_DOWN,	   // 0x28
	PrintScreen = VK_SNAPSHOT, // 0x2C
	Insert		= VK_INSERT,   // 0x2D
	Delete		= VK_DELETE,   // 0x2E

	// Digits
	D0 = 0x30,
	D1 = 0x31,
	D2 = 0x32,
	D3 = 0x33,
	D4 = 0x34,
	D5 = 0x35,
	D6 = 0x36,
	D7 = 0x37,
	D8 = 0x38,
	D9 = 0x39,

	// Letters
	A = 0x41,
	B = 0x42,
	C = 0x43,
	D = 0x44,
	E = 0x45,
	F = 0x46,
	G = 0x47,
	H = 0x48,
	I = 0x49,
	J = 0x4a,
	K = 0x4b,
	L = 0x4c,
	M = 0x4d,
	N = 0x4e,
	O = 0x4f,
	P = 0x50,
	Q = 0x51,
	R = 0x52,
	S = 0x53,
	T = 0x54,
	U = 0x55,
	V = 0x56,
	W = 0x57,
	X = 0x58,
	Y = 0x59,
	Z = 0x5a,

	// System
	LWin  = VK_LWIN,  // 0x5B
	RWin  = VK_RWIN,  // 0x5C
	Apps  = VK_APPS,  // 0x5D
	Sleep = VK_SLEEP, // 0x5F

	// Numpad
	Num0		 = VK_NUMPAD0, // 0x60
	Num1		 = VK_NUMPAD1,
	Num2		 = VK_NUMPAD2,
	Num3		 = VK_NUMPAD3,
	Num4		 = VK_NUMPAD4,
	Num5		 = VK_NUMPAD5,
	Num6		 = VK_NUMPAD6,
	Num7		 = VK_NUMPAD7,
	Num8		 = VK_NUMPAD8,
	Num9		 = VK_NUMPAD9,	 // 0x69
	NumMultiply	 = VK_MULTIPLY,	 // 0x6A
	NumAdd		 = VK_ADD,		 // 0x6B
	NumSeparator = VK_SEPARATOR, // 0x6C
	NumSubtract	 = VK_SUBTRACT,	 // 0x6D
	NumDecimal	 = VK_DECIMAL,	 // 0x6E
	NumDivide	 = VK_DIVIDE,	 // 0x6F

	// Function keys
	F1	= VK_F1, // 0x70
	F2	= VK_F2,
	F3	= VK_F3,
	F4	= VK_F4,
	F5	= VK_F5,
	F6	= VK_F6,
	F7	= VK_F7,
	F8	= VK_F8,
	F9	= VK_F9,
	F10 = VK_F10,
	F11 = VK_F11,
	F12 = VK_F12,
	F13 = VK_F13,
	F14 = VK_F14,
	F15 = VK_F15,
	F16 = VK_F16,
	F17 = VK_F17,
	F18 = VK_F18,
	F19 = VK_F19,
	F20 = VK_F20,
	F21 = VK_F21,
	F22 = VK_F22,
	F23 = VK_F23,
	F24 = VK_F24, // 0x87

	// Lock
	NumLock	   = VK_NUMLOCK, // 0x90
	ScrollLock = VK_SCROLL,	 // 0x91

	// Sided modifiers
	LShift = VK_LSHIFT,	  // 0xA0
	RShift = VK_RSHIFT,	  // 0xA1
	LCtrl  = VK_LCONTROL, // 0xA2
	RCtrl  = VK_RCONTROL, // 0xA3
	LAlt   = VK_LMENU,	  // 0xA4
	RAlt   = VK_RMENU,	  // 0xA5

	// Browser
	BrowserBack		 = VK_BROWSER_BACK,		 // 0xA6
	BrowserForward	 = VK_BROWSER_FORWARD,	 // 0xA7
	BrowserRefresh	 = VK_BROWSER_REFRESH,	 // 0xA8
	BrowserStop		 = VK_BROWSER_STOP,		 // 0xA9
	BrowserSearch	 = VK_BROWSER_SEARCH,	 // 0xAA
	BrowserFavorites = VK_BROWSER_FAVORITES, // 0xAB
	BrowserHome		 = VK_BROWSER_HOME,		 // 0xAC

	// Media / Volume
	VolumeMute	   = VK_VOLUME_MUTE,	  // 0xAD
	VolumeDown	   = VK_VOLUME_DOWN,	  // 0xAE
	VolumeUp	   = VK_VOLUME_UP,		  // 0xAF
	MediaNext	   = VK_MEDIA_NEXT_TRACK, // 0xB0
	MediaPrev	   = VK_MEDIA_PREV_TRACK, // 0xB1
	MediaStop	   = VK_MEDIA_STOP,		  // 0xB2
	MediaPlayPause = VK_MEDIA_PLAY_PAUSE, // 0xB3

	// OEM (US ANSI)
	Semicolon  = VK_OEM_1,		// 0xBA  ;:
	Plus	   = VK_OEM_PLUS,	// 0xBB  =+
	Comma	   = VK_OEM_COMMA,	// 0xBC  ,<
	Minus	   = VK_OEM_MINUS,	// 0xBD  -_
	Period	   = VK_OEM_PERIOD, // 0xBE  .>
	Slash	   = VK_OEM_2,		// 0xBF  /?
	Tilde	   = VK_OEM_3,		// 0xC0  `~
	LBracket   = VK_OEM_4,		// 0xDB  [{
	Backslash  = VK_OEM_5,		// 0xDC  \|
	RBracket   = VK_OEM_6,		// 0xDD  ]}
	Apostrophe = VK_OEM_7,		// 0xDE  '"
	Angle	   = VK_OEM_102,	// 0xE2  \| ISO
};
