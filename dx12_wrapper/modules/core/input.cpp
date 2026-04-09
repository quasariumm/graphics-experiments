module;

#include <cstdint>

module dx_wrapper.core.input;

bool Input::GetKeyboardKey(Key key) const { return m_state.test(static_cast<size_t>(key)); }
bool Input::GetKeyboardKeyOnce(Key key) const
{ return m_state.test(static_cast<size_t>(key)) && !m_prevState.test(static_cast<size_t>(key)); }

bool Input::GetMouseButton(MouseButton button) const { return m_state.test(static_cast<size_t>(button)); }
bool Input::GetMouseButtonOnce(MouseButton button) const
{ return m_state.test(static_cast<size_t>(button)) && !m_prevState.test(static_cast<size_t>(button)); }

void Input::SetBit(const uint32_t idx, const bool val) { m_state.set(idx, val); }
void Input::BlitState() { m_prevState = m_state; }
