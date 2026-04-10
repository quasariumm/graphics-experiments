module;

#include <bitset>

module dx_wrapper.core.input;

extern std::bitset<0xff> current_input_state; // NOLINT
extern glm::vec2 current_mouse_pos; // NOLINT
extern glm::vec2 current_scroll_delta; // NOLINT

bool Input::GetKeyboardKey(Key key) const { return m_state.test(static_cast<size_t>(key)); }
bool Input::GetKeyboardKeyOnce(Key key) const
{ return m_state.test(static_cast<size_t>(key)) && !m_prevState.test(static_cast<size_t>(key)); }

bool Input::GetMouseButton(MouseButton button) const { return m_state.test(static_cast<size_t>(button)); }
bool Input::GetMouseButtonOnce(MouseButton button) const
{ return m_state.test(static_cast<size_t>(button)) && !m_prevState.test(static_cast<size_t>(button)); }

void Input::Update()
{
	m_prevState = m_state;
	m_state = current_input_state;
	
	m_mouseDelta = current_mouse_pos - m_mousePos;
	m_mousePos = current_mouse_pos;
}
