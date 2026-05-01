// NOLINTBEGIN
module;

#include <combaseapi.h>
#include <windef.h>
#include <winuser.h>
#undef DefWindowProc
#undef RegisterClassEx
#undef LoadCursor
#undef GetModuleHandle
#undef DispatchMessage
#undef PeekMessage
#undef SendMessage
#undef LoadImage
#undef SetWindowLong
#undef SetWindowText
#undef CreateWindowEx
#undef FormatMessage
#undef LoadLibrary
#undef LOWORD
#undef HIWORD
#undef LOBYTE
#undef HIBYTE
#undef GET_X_PARAM
#undef GET_Y_PARAM
#undef GET_XBUTTON_WPARAM
#undef GET_WHEEL_DELTA_WPARAM
#undef TRUE
#undef FALSE

#ifdef UNICODE
	#define REDEF_FUNC(ret, name, params, args)                                                                                \
		export inline ret name params { return name##W args; }
#else
	#define REDEF_FUNC(ret, name, params, args)                                                                                \
		export inline ret name params { return name##A args; }
#endif

export module dx_wrapper.external.win32;
export import dx_wrapper.external.win32_dx_common;

export constexpr int FALSE = 0;
export constexpr int TRUE  = 1;

// Numerical
export using ::BYTE;
export using ::CHAR;
export using ::UINT;
export using ::UINT8;
export using ::UINT16;
export using ::UINT32;
export using ::UINT64;
export using ::INT;
export using ::INT8;
export using ::INT16;
export using ::INT32;
export using ::INT64;
export using ::FLOAT;
export using ::FLOAT128;
export using ::WORD;
export using ::DWORD;
export using ::DWORD32;
export using ::DWORD64;

// Params
export using ::LPARAM;
export using ::WPARAM;

// Handle
export using ::HWND;
export using ::HANDLE;
export using ::HANDLE64;
export using ::HINSTANCE;
export using ::HMODULE;
export using ::HICON;

// Result
export using ::LRESULT;

// Strings
export using ::PCSTR;
export using ::PCWSTR;
export using ::LPCSTR;
export using ::LPCWSTR;

// WindowProc things
export using ::WNDCLASSEX;
export using ::RECT;
export using ::POINT;
export using ::MSG;
export using ::WINDOWPLACEMENT;
export using ::TranslateMessage;
export using ::ShowCursor;
export using ::ClipCursor;
export using ::GetClientRect;
export using ::MapWindowPoints;
export using ::GetSystemMetrics;
export using ::SetCapture;
export using ::ReleaseCapture;
export using ::SetWindowPos;
export using ::GetWindowPlacement;
export using ::SetWindowPlacement;
export using ::ShowWindow;
export using ::AdjustWindowRect;

export inline WORD LOWORD(DWORD_PTR l) { return static_cast<WORD>(l & 0xffff); }
export inline WORD HIWORD(DWORD_PTR l) { return static_cast<WORD>((l >> 16) & 0xffff); }
export inline WORD LOBYTE(WORD w) { return static_cast<BYTE>(w & 0xff); }
export inline BYTE HIBYTE(WORD w) { return static_cast<BYTE>((w >> 8) & 0xff); }

export inline int GET_X_LPARAM(LPARAM lp) { return static_cast<int>(static_cast<short>(LOWORD(lp))); }
export inline int GET_Y_LPARAM(LPARAM lp) { return static_cast<int>(static_cast<short>(HIWORD(lp))); }

export inline WORD	GET_XBUTTON_WPARAM(WPARAM wParam) { return HIWORD(wParam); }
export inline short GET_WHEEL_DELTA_WPARAM(WPARAM wParam) { return static_cast<short>(HIWORD(wParam)); }

static constexpr int PM_REMOVE_val = PM_REMOVE;
#undef PM_REMOVE
export constexpr int PM_REMOVE = PM_REMOVE_val;

static constexpr int ICON_SMALL_val = ICON_SMALL;
#undef ICON_SMALL
export constexpr int ICON_SMALL = ICON_SMALL_val;

static constexpr int ICON_BIG_val = ICON_BIG;
#undef ICON_BIG
export constexpr int ICON_BIG = ICON_BIG_val;

static constexpr int IMAGE_ICON_val = IMAGE_ICON;
#undef IMAGE_ICON
export constexpr int IMAGE_ICON = IMAGE_ICON_val;

static constexpr int SM_CYSMICON_val = SM_CYSMICON;
#undef SM_CYSMICON
export constexpr int SM_CYSMICON = SM_CYSMICON_val;

static constexpr int SM_CXSMICON_val = SM_CXSMICON;
#undef SM_CXSMICON
export constexpr int SM_CXSMICON = SM_CXSMICON_val;

static constexpr int SM_CXICON_val = SM_CXICON;
#undef SM_CXICON
export constexpr int SM_CXICON = SM_CXICON_val;

static constexpr int SM_CYICON_val = SM_CYICON;
#undef SM_CYICON
export constexpr int SM_CYICON = SM_CYICON_val;

static constexpr int LR_LOADFROMFILE_val = LR_LOADFROMFILE;
#undef LR_LOADFROMFILE
export constexpr int LR_LOADFROMFILE = LR_LOADFROMFILE_val;

static LPSTR IDC_ARROW_val = IDC_ARROW;
#undef IDC_ARROW
export LPSTR IDC_ARROW = IDC_ARROW_val;

static constexpr UINT WS_OVERLAPPEDWINDOW_val = WS_OVERLAPPEDWINDOW;
#undef WS_OVERLAPPEDWINDOW
export constexpr UINT WS_OVERLAPPEDWINDOW = WS_OVERLAPPEDWINDOW_val;

static constexpr int CW_USEDEFAULT_val = CW_USEDEFAULT;
#undef CW_USEDEFAULT
export constexpr int CW_USEDEFAULT = CW_USEDEFAULT_val;

static constexpr int SW_SHOW_val = SW_SHOW;
#undef SW_SHOW
export constexpr int SW_SHOW = SW_SHOW_val;

static constexpr int XBUTTON1_val = XBUTTON1;
#undef XBUTTON1
export constexpr int XBUTTON1 = XBUTTON1_val;

static constexpr int XBUTTON2_val = XBUTTON2;
#undef XBUTTON2
export constexpr int XBUTTON2 = XBUTTON2_val;

static HWND HWND_TOPMOST_val = HWND_TOPMOST;
#undef HWND_TOPMOST
export HWND HWND_TOPMOST = HWND_TOPMOST_val;

static HWND HWND_TOP_val = HWND_TOP;
#undef HWND_TOP
export HWND HWND_TOP = HWND_TOP_val;

static constexpr int GWL_STYLE_val = GWL_STYLE;
#undef GWL_STYLE
export constexpr int GWL_STYLE = GWL_STYLE_val;

static constexpr int WS_POPUP_val = WS_POPUP;
#undef WS_POPUP
export constexpr int WS_POPUP = WS_POPUP_val;

static constexpr int WS_VISIBLE_val = WS_VISIBLE;
#undef WS_VISIBLE
export constexpr int WS_VISIBLE = WS_VISIBLE_val;

static constexpr int SWP_FRAMECHANGED_val = SWP_FRAMECHANGED;
#undef SWP_FRAMECHANGED
export constexpr int SWP_FRAMECHANGED = SWP_FRAMECHANGED_val;

static constexpr int SWP_NOACTIVATE_val = SWP_NOACTIVATE;
#undef SWP_NOACTIVATE
export constexpr int SWP_NOACTIVATE = SWP_NOACTIVATE_val;

static constexpr int SWP_NOMOVE_val = SWP_NOMOVE;
#undef SWP_NOMOVE
export constexpr int SWP_NOMOVE = SWP_NOMOVE_val;

static constexpr int SWP_NOSIZE_val = SWP_NOSIZE;
#undef SWP_NOSIZE
export constexpr int SWP_NOSIZE = SWP_NOSIZE_val;

static constexpr int SWP_NOZORDER_val = SWP_NOZORDER;
#undef SWP_NOZORDER
export constexpr int SWP_NOZORDER = SWP_NOZORDER_val;

static constexpr int SW_MAXIMIZE_val = SW_MAXIMIZE;
#undef SW_MAXIMIZE
export constexpr int SW_MAXIMIZE = SW_MAXIMIZE_val;

static constexpr int SW_NORMAL_val = SW_NORMAL;
#undef SW_NORMAL
export constexpr int SW_NORMAL = SW_NORMAL_val;

static constexpr int FORMAT_MESSAGE_FROM_SYSTEM_val = FORMAT_MESSAGE_FROM_SYSTEM;
#undef FORMAT_MESSAGE_FROM_SYSTEM
export constexpr int FORMAT_MESSAGE_FROM_SYSTEM = FORMAT_MESSAGE_FROM_SYSTEM_val;

static constexpr int FORMAT_MESSAGE_IGNORE_INSERTS_val = FORMAT_MESSAGE_IGNORE_INSERTS;
#undef FORMAT_MESSAGE_IGNORE_INSERTS
export constexpr int FORMAT_MESSAGE_IGNORE_INSERTS = FORMAT_MESSAGE_IGNORE_INSERTS_val;

REDEF_FUNC(HMODULE, LoadLibrary, (LPCSTR lpModuleName), (lpModuleName));
REDEF_FUNC(BOOL, PeekMessage, (LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg),
		   (lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg))
REDEF_FUNC(BOOL, SendMessage, (HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam), (hWnd, Msg, wParam, lParam))
REDEF_FUNC(LRESULT, DispatchMessage, (const MSG* lpMsg), (lpMsg))
REDEF_FUNC(HMODULE, GetModuleHandle, (LPCSTR lpModuleName), (lpModuleName))
REDEF_FUNC(HCURSOR, LoadCursor, (HINSTANCE hInstance, LPCSTR lpCursorName), (hInstance, lpCursorName))
REDEF_FUNC(ATOM, RegisterClassEx, (const WNDCLASSEXA* wc), (wc))
REDEF_FUNC(LRESULT, DefWindowProc, (HWND hWnd, UINT msg, WPARAM w, LPARAM l), (hWnd, msg, w, l))
REDEF_FUNC(HANDLE, LoadImage, (HINSTANCE hInst, LPCSTR name, UINT type, int cx, int cy, UINT fuLoad),
		   (hInst, name, type, cx, cy, fuLoad))
REDEF_FUNC(LONG, SetWindowLong, (HWND hWnd, int nIndex, LONG dwNewLong), (hWnd, nIndex, dwNewLong))
REDEF_FUNC(LONG, SetWindowText, (HWND hWnd, LPCSTR lpString), (hWnd, lpString))
REDEF_FUNC(HWND, CreateWindowEx,
		   (DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
			HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam),
		   (dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam))
REDEF_FUNC(DWORD, FormatMessage,
		   (DWORD dwFlags, LPCVOID lpSource, DWORD dwMessageId, DWORD dwLanguageId, LPSTR lpBuffer, DWORD nSize,
			va_list* Arguments),
		   (dwFlags, lpSource, dwMessageId, dwLanguageId, lpBuffer, nSize, Arguments))

export using ::GetDpiForWindow;
export using ::PostQuitMessage;
export using ::GetProcAddress;

// Window events
#undef GetObject
export namespace WM
{
enum : UINT {
	// Core
	Null				   = WM_NULL,
	Create				   = WM_CREATE,
	Destroy				   = WM_DESTROY,
	Move				   = WM_MOVE,
	Size				   = WM_SIZE,
	Activate			   = WM_ACTIVATE,
	SetFocus			   = WM_SETFOCUS,
	KillFocus			   = WM_KILLFOCUS,
	Enable				   = WM_ENABLE,
	SetRedraw			   = WM_SETREDRAW,
	SetText				   = WM_SETTEXT,
	GetText				   = WM_GETTEXT,
	GetTextLength		   = WM_GETTEXTLENGTH,
	Paint				   = WM_PAINT,
	Close				   = WM_CLOSE,
	QueryEndSession		   = WM_QUERYENDSESSION,
	QueryOpen			   = WM_QUERYOPEN,
	EndSession			   = WM_ENDSESSION,
	Quit				   = WM_QUIT,
	EraseBkgnd			   = WM_ERASEBKGND,
	SysColorChange		   = WM_SYSCOLORCHANGE,
	ShowWindow			   = WM_SHOWWINDOW,
	SettingChange		   = WM_SETTINGCHANGE,
	DevModeChange		   = WM_DEVMODECHANGE,
	ActivateApp			   = WM_ACTIVATEAPP,
	FontChange			   = WM_FONTCHANGE,
	TimeChange			   = WM_TIMECHANGE,
	CancelMode			   = WM_CANCELMODE,
	SetCursor			   = WM_SETCURSOR,
	MouseActivate		   = WM_MOUSEACTIVATE,
	ChildActivate		   = WM_CHILDACTIVATE,
	QueueSync			   = WM_QUEUESYNC,
	GetMinMaxInfo		   = WM_GETMINMAXINFO,
	PaintIcon			   = WM_PAINTICON,
	IconEraseBkgnd		   = WM_ICONERASEBKGND,
	NextDlgCtl			   = WM_NEXTDLGCTL,
	SpoolerStatus		   = WM_SPOOLERSTATUS,
	DrawItem			   = WM_DRAWITEM,
	MeasureItem			   = WM_MEASUREITEM,
	DeleteItem			   = WM_DELETEITEM,
	VKeyToItem			   = WM_VKEYTOITEM,
	CharToItem			   = WM_CHARTOITEM,
	SetFont				   = WM_SETFONT,
	GetFont				   = WM_GETFONT,
	SetHotKey			   = WM_SETHOTKEY,
	GetHotKey			   = WM_GETHOTKEY,
	QueryDragIcon		   = WM_QUERYDRAGICON,
	CompareItem			   = WM_COMPAREITEM,
	GetObject			   = WM_GETOBJECT,
	Compacting			   = WM_COMPACTING,
	WindowPosChanging	   = WM_WINDOWPOSCHANGING,
	WindowPosChanged	   = WM_WINDOWPOSCHANGED,
	Power				   = WM_POWER,
	CopyData			   = WM_COPYDATA,
	CancelJournal		   = WM_CANCELJOURNAL,
	Notify				   = WM_NOTIFY,
	InputLangChangeRequest = WM_INPUTLANGCHANGEREQUEST,
	InputLangChange		   = WM_INPUTLANGCHANGE,
	TCard				   = WM_TCARD,
	Help				   = WM_HELP,
	UserChanged			   = WM_USERCHANGED,
	NotifyFormat		   = WM_NOTIFYFORMAT,
	ContextMenu			   = WM_CONTEXTMENU,
	StyleChanging		   = WM_STYLECHANGING,
	StyleChanged		   = WM_STYLECHANGED,
	DisplayChange		   = WM_DISPLAYCHANGE,
	GetIcon				   = WM_GETICON,
	SetIcon				   = WM_SETICON,

	// Non-client
	NcCreate		= WM_NCCREATE,
	NcDestroy		= WM_NCDESTROY,
	NcCalcSize		= WM_NCCALCSIZE,
	NcHitTest		= WM_NCHITTEST,
	NcPaint			= WM_NCPAINT,
	NcActivate		= WM_NCACTIVATE,
	GetDlgCode		= WM_GETDLGCODE,
	SyncPaint		= WM_SYNCPAINT,
	NcMouseMove		= WM_NCMOUSEMOVE,
	NcLButtonDown	= WM_NCLBUTTONDOWN,
	NcLButtonUp		= WM_NCLBUTTONUP,
	NcLButtonDblClk = WM_NCLBUTTONDBLCLK,
	NcRButtonDown	= WM_NCRBUTTONDOWN,
	NcRButtonUp		= WM_NCRBUTTONUP,
	NcRButtonDblClk = WM_NCRBUTTONDBLCLK,
	NcMButtonDown	= WM_NCMBUTTONDOWN,
	NcMButtonUp		= WM_NCMBUTTONUP,
	NcMButtonDblClk = WM_NCMBUTTONDBLCLK,
	NcXButtonDown	= WM_NCXBUTTONDOWN,
	NcXButtonUp		= WM_NCXBUTTONUP,
	NcXButtonDblClk = WM_NCXBUTTONDBLCLK,

	// Input
	InputDeviceChange = WM_INPUT_DEVICE_CHANGE,
	Input			  = WM_INPUT,
	KeyDown			  = WM_KEYDOWN,
	KeyUp			  = WM_KEYUP,
	Char			  = WM_CHAR,
	DeadChar		  = WM_DEADCHAR,
	SysKeyDown		  = WM_SYSKEYDOWN,
	SysKeyUp		  = WM_SYSKEYUP,
	SysChar			  = WM_SYSCHAR,
	SysDeadChar		  = WM_SYSDEADCHAR,
	UniChar			  = WM_UNICHAR,

	// IME
	ImeStartComposition = WM_IME_STARTCOMPOSITION,
	ImeEndComposition	= WM_IME_ENDCOMPOSITION,
	ImeComposition		= WM_IME_COMPOSITION,
	ImeSetContext		= WM_IME_SETCONTEXT,
	ImeNotify			= WM_IME_NOTIFY,
	ImeControl			= WM_IME_CONTROL,
	ImeCompositionFull	= WM_IME_COMPOSITIONFULL,
	ImeSelect			= WM_IME_SELECT,
	ImeChar				= WM_IME_CHAR,
	ImeRequest			= WM_IME_REQUEST,
	ImeKeyDown			= WM_IME_KEYDOWN,
	ImeKeyUp			= WM_IME_KEYUP,

	// Dialog / Menu
	InitDialog		= WM_INITDIALOG,
	Command			= WM_COMMAND,
	SysCommand		= WM_SYSCOMMAND,
	Timer			= WM_TIMER,
	HScroll			= WM_HSCROLL,
	VScroll			= WM_VSCROLL,
	InitMenu		= WM_INITMENU,
	InitMenuPopup	= WM_INITMENUPOPUP,
	Gesture			= WM_GESTURE,
	GestureNotify	= WM_GESTURENOTIFY,
	MenuSelect		= WM_MENUSELECT,
	MenuChar		= WM_MENUCHAR,
	EnterIdle		= WM_ENTERIDLE,
	MenuRButtonUp	= WM_MENURBUTTONUP,
	MenuDrag		= WM_MENUDRAG,
	MenuGetObject	= WM_MENUGETOBJECT,
	UninitMenuPopup = WM_UNINITMENUPOPUP,
	MenuCommand		= WM_MENUCOMMAND,
	ChangeUiState	= WM_CHANGEUISTATE,
	UpdateUiState	= WM_UPDATEUISTATE,
	QueryUiState	= WM_QUERYUISTATE,

	// Control colors
	CtlColorMsgBox	  = WM_CTLCOLORMSGBOX,
	CtlColorEdit	  = WM_CTLCOLOREDIT,
	CtlColorListBox	  = WM_CTLCOLORLISTBOX,
	CtlColorBtn		  = WM_CTLCOLORBTN,
	CtlColorDlg		  = WM_CTLCOLORDLG,
	CtlColorScrollBar = WM_CTLCOLORSCROLLBAR,
	CtlColorStatic	  = WM_CTLCOLORSTATIC,

	// Mouse
	MouseMove	  = WM_MOUSEMOVE,
	LButtonDown	  = WM_LBUTTONDOWN,
	LButtonUp	  = WM_LBUTTONUP,
	LButtonDblClk = WM_LBUTTONDBLCLK,
	RButtonDown	  = WM_RBUTTONDOWN,
	RButtonUp	  = WM_RBUTTONUP,
	RButtonDblClk = WM_RBUTTONDBLCLK,
	MButtonDown	  = WM_MBUTTONDOWN,
	MButtonUp	  = WM_MBUTTONUP,
	MButtonDblClk = WM_MBUTTONDBLCLK,
	MouseWheel	  = WM_MOUSEWHEEL,
	XButtonDown	  = WM_XBUTTONDOWN,
	XButtonUp	  = WM_XBUTTONUP,
	XButtonDblClk = WM_XBUTTONDBLCLK,
	MouseHWheel	  = WM_MOUSEHWHEEL,
	MouseHover	  = WM_MOUSEHOVER,
	MouseLeave	  = WM_MOUSELEAVE,
	NcMouseHover  = WM_NCMOUSEHOVER,
	NcMouseLeave  = WM_NCMOUSELEAVE,

	// Parent / sizing
	ParentNotify   = WM_PARENTNOTIFY,
	EnterMenuLoop  = WM_ENTERMENULOOP,
	ExitMenuLoop   = WM_EXITMENULOOP,
	NextMenu	   = WM_NEXTMENU,
	Sizing		   = WM_SIZING,
	CaptureChanged = WM_CAPTURECHANGED,
	Moving		   = WM_MOVING,
	PowerBroadcast = WM_POWERBROADCAST,
	DeviceChange   = WM_DEVICECHANGE,

	// MDI
	MdiCreate	   = WM_MDICREATE,
	MdiDestroy	   = WM_MDIDESTROY,
	MdiActivate	   = WM_MDIACTIVATE,
	MdiRestore	   = WM_MDIRESTORE,
	MdiNext		   = WM_MDINEXT,
	MdiMaximize	   = WM_MDIMAXIMIZE,
	MdiTile		   = WM_MDITILE,
	MdiCascade	   = WM_MDICASCADE,
	MdiIconArrange = WM_MDIICONARRANGE,
	MdiGetActive   = WM_MDIGETACTIVE,
	MdiSetMenu	   = WM_MDISETMENU,
	EnterSizeMove  = WM_ENTERSIZEMOVE,
	ExitSizeMove   = WM_EXITSIZEMOVE,
	DropFiles	   = WM_DROPFILES,
	MdiRefreshMenu = WM_MDIREFRESHMENU,

	// Touch / Pointer
	Touch				  = WM_TOUCH,
	NcPointerUpdate		  = WM_NCPOINTERUPDATE,
	NcPointerDown		  = WM_NCPOINTERDOWN,
	NcPointerUp			  = WM_NCPOINTERUP,
	PointerUpdate		  = WM_POINTERUPDATE,
	PointerDown			  = WM_POINTERDOWN,
	PointerUp			  = WM_POINTERUP,
	PointerEnter		  = WM_POINTERENTER,
	PointerLeave		  = WM_POINTERLEAVE,
	PointerActivate		  = WM_POINTERACTIVATE,
	PointerCaptureChanged = WM_POINTERCAPTURECHANGED,
	TouchHitTesting		  = WM_TOUCHHITTESTING,
	PointerWheel		  = WM_POINTERWHEEL,
	PointerHWheel		  = WM_POINTERHWHEEL,
	PointerRoutedTo		  = WM_POINTERROUTEDTO,
	PointerRoutedAway	  = WM_POINTERROUTEDAWAY,
	PointerRoutedReleased = WM_POINTERROUTEDRELEASED,

	// Clipboard
	Cut				  = WM_CUT,
	Copy			  = WM_COPY,
	Paste			  = WM_PASTE,
	Clear			  = WM_CLEAR,
	Undo			  = WM_UNDO,
	RenderFormat	  = WM_RENDERFORMAT,
	RenderAllFormats  = WM_RENDERALLFORMATS,
	DestroyClipboard  = WM_DESTROYCLIPBOARD,
	DrawClipboard	  = WM_DRAWCLIPBOARD,
	PaintClipboard	  = WM_PAINTCLIPBOARD,
	VScrollClipboard  = WM_VSCROLLCLIPBOARD,
	SizeClipboard	  = WM_SIZECLIPBOARD,
	AskCbFormatName	  = WM_ASKCBFORMATNAME,
	ChangeCbChain	  = WM_CHANGECBCHAIN,
	HScrollClipboard  = WM_HSCROLLCLIPBOARD,
	QueryNewPalette	  = WM_QUERYNEWPALETTE,
	PaletteIsChanging = WM_PALETTEISCHANGING,
	PaletteChanged	  = WM_PALETTECHANGED,

	// Misc
	HotKey			 = WM_HOTKEY,
	Print			 = WM_PRINT,
	PrintClient		 = WM_PRINTCLIENT,
	AppCommand		 = WM_APPCOMMAND,
	ThemeChanged	 = WM_THEMECHANGED,
	ClipboardUpdate	 = WM_CLIPBOARDUPDATE,
	WtsSessionChange = WM_WTSSESSION_CHANGE,

	// DWM
	DwmCompositionChanged		   = WM_DWMCOMPOSITIONCHANGED,
	DwmNcRenderingChanged		   = WM_DWMNCRENDERINGCHANGED,
	DwmColorizationColorChanged	   = WM_DWMCOLORIZATIONCOLORCHANGED,
	DwmWindowMaximizedChange	   = WM_DWMWINDOWMAXIMIZEDCHANGE,
	DwmSendIconicThumbnail		   = WM_DWMSENDICONICTHUMBNAIL,
	DwmSendIconicLivePreviewBitmap = WM_DWMSENDICONICLIVEPREVIEWBITMAP,

	// DPI
	DpiChanged			   = WM_DPICHANGED,
	DpiChangedBeforeParent = WM_DPICHANGED_BEFOREPARENT,
	DpiChangedAfterParent  = WM_DPICHANGED_AFTERPARENT,
	GetDpiScaledSize	   = WM_GETDPISCALEDSIZE,

	GetTitleBarInfoEx = WM_GETTITLEBARINFOEX,

	// Ranges — kept as sentinels, useful for bounds checking
	User = WM_USER, // 0x0400
	App	 = WM_APP,	// 0x8000
};
};

// Combase
export using ::CoInitializeEx;
export using ::COINIT_MULTITHREADED;

// NOLINTEND
