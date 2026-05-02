module;

#include <d3d12.h>
#undef FAILED
#undef SUCCEEDED
#undef CreateEventEx
#undef TRUE
#undef FALSE

#ifdef UNICODE
#define REDEF_FUNC(ret, name, params, args)                                                                                \
export inline ret name params { return name##W args; }
#else
#define REDEF_FUNC(ret, name, params, args)                                                                                \
export inline ret name params { return name##A args; }
#endif

export module dx_wrapper.external.win32_dx_common;

export constexpr int FALSE = 0;
export constexpr int TRUE  = 1;

export using ::HRESULT;

export inline bool FAILED(HRESULT hr) { return hr < 0; }
export inline bool SUCCEEDED(HRESULT hr) { return hr >= 0; }

export using ::GetLastError;
export using ::WaitForSingleObject;

REDEF_FUNC(HANDLE, CreateEventEx,
		   (LPSECURITY_ATTRIBUTES lpEventAttributes, LPCSTR lpName, DWORD dwFlags, DWORD dwDesiredAccess),
		   (lpEventAttributes, lpName, dwFlags, dwDesiredAccess));

// Values
static constexpr int INFINITE_val = INFINITE;
#undef INFINITE
export constexpr int INFINITE = INFINITE_val;

static constexpr int EVENT_MODIFY_STATE_val = EVENT_MODIFY_STATE;
#undef EVENT_MODIFY_STATE
export constexpr int EVENT_MODIFY_STATE = EVENT_MODIFY_STATE_val;

static constexpr int SYNCHRONIZE_val = SYNCHRONIZE;
#undef SYNCHRONIZE
export constexpr int SYNCHRONIZE = SYNCHRONIZE_val;

static constexpr int WAIT_OBJECT_0_val = WAIT_OBJECT_0;
#undef WAIT_OBJECT_0
export constexpr int WAIT_OBJECT_0 = WAIT_OBJECT_0_val;

static constexpr int WAIT_FAILED_val = WAIT_FAILED;
#undef WAIT_FAILED
export constexpr int WAIT_FAILED = WAIT_FAILED_val;
