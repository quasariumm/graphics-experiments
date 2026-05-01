module;

#include <bitset>
#include <windows.h>

export module dx_wrapper.core.input;
export import dx_wrapper.core.input_enums;
import dx_wrapper.external.glm;

export class Input
{
public:
	
	Input() = default;
	
	/** Gets a key state */
	bool GetKeyboardKey(Key key) const;
	
	/** Gets a key state only wen first pressed */
	bool GetKeyboardKeyOnce(Key key) const;
	
	/** Gets a mouse button state */
	bool GetMouseButton(MouseButton button) const;
	
	/** Gets a mouse button state only when first clicked */
	bool GetMouseButtonOnce(MouseButton button) const;
	
	/** Gets the mouse delta in DIP (Device Independent Pixels) */
	[[nodiscard]] glm::vec2 GetMouseDelta() const { return m_mouseDelta; }
	
	/** Gets the mouse position in DIP (Device Independent Pixels) */
	[[nodiscard]] glm::vec2 GetMousePos() const { return m_mousePos; }
	
	[[nodiscard]] float GetMouseScrollX() const { return m_mouseScroll.x; }
	[[nodiscard]] float GetMouseScrollY() const { return m_mouseScroll.y; }
	
	/** Updates the states */
	void Update();
	
private:
	
	// Key, mouse button and controller button state
	std::bitset<0xff> m_state;
	std::bitset<0xff> m_prevState;
	
	// Mouse position
	glm::vec2 m_mousePos;
	glm::vec2 m_mouseDelta;
	
	glm::vec2 m_mouseScroll;
	
};