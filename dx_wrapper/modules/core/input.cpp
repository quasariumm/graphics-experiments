module;

module dx_wrapper.core.input;
import std;

inline static std::bitset<0xff> current_input_state{};	// NOLINT
inline static glm::vec2			current_mouse_pos{};	// NOLINT
inline static glm::vec2			current_scroll_delta{}; // NOLINT

bool Input::GetKeyboardKey(Key key) const { return m_state.test(static_cast<std::size_t>(key)); }
bool Input::GetKeyboardKeyOnce(Key key) const
{ return m_state.test(static_cast<std::size_t>(key)) && !m_prevState.test(static_cast<std::size_t>(key)); }

bool Input::GetMouseButton(MouseButton button) const { return m_state.test(static_cast<std::size_t>(button)); }
bool Input::GetMouseButtonOnce(MouseButton button) const
{ return m_state.test(static_cast<std::size_t>(button)) && !m_prevState.test(static_cast<std::size_t>(button)); }

void Input::Update()
{
	m_prevState = m_state;
	m_state		= current_input_state;

	m_mouseDelta = current_mouse_pos - m_mousePos;
	m_mousePos	 = current_mouse_pos;
}

// WindowProc for input
extern "C" LRESULT InputWindowProc(HWND hwnd, const UINT msg, const WPARAM wp, const LPARAM lp)
{
	float dpi = GetDpiForWindow(hwnd) / 96.0f;
	
	switch (msg)
	{
		/*
	 * Keyboard
	 */
	case WM::KeyDown:
		current_input_state.set(LOWORD(wp), true);
		break;
	case WM::KeyUp:
		current_input_state.set(LOWORD(wp), false);
		break;
		/*
		 * Mouse
		 */
	case WM::LButtonDown:
		current_input_state.set(static_cast<std::size_t>(MouseButton::Left), true);
		SetCapture(hwnd);
		break;
	case WM::RButtonDown:
		current_input_state.set(static_cast<std::size_t>(MouseButton::Right), true);
		break;
	case WM::MButtonDown:
		current_input_state.set(static_cast<std::size_t>(MouseButton::Middle), true);
		break;
	case WM::XButtonDown:
	{
		const UINT button = GET_XBUTTON_WPARAM(wp);
		current_input_state.set(static_cast<std::size_t>(button == XBUTTON1 ? MouseButton::X1 : MouseButton::X2), true);
		break;
	}
	case WM::LButtonUp:
		current_input_state.set(static_cast<std::size_t>(MouseButton::Left), false);
		ReleaseCapture();
		break;
	case WM::RButtonUp:
		current_input_state.set(static_cast<std::size_t>(MouseButton::Right), false);
		break;
	case WM::MButtonUp:
		current_input_state.set(static_cast<std::size_t>(MouseButton::Middle), false);
		break;
	case WM::XButtonUp:
	{
		const UINT button = GET_XBUTTON_WPARAM(wp);
		current_input_state.set(static_cast<std::size_t>(button == XBUTTON1 ? MouseButton::X1 : MouseButton::X2), false);
		break;
	}
	case WM::MouseMove:
		current_mouse_pos.x = static_cast<float>(LOWORD(lp)) / dpi;
		current_mouse_pos.y = static_cast<float>(HIWORD(lp)) / dpi;
		break;
	case WM::MouseWheel:
		current_scroll_delta.y = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wp)) / 120.0f;
		break;
	case WM::MouseHWheel:
		current_scroll_delta.x = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wp)) / 120.0f;
		break;
	default:
		break;
	}
	
	return FALSE;
}