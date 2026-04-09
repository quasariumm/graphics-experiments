module;

#include <bitset>

export module dx_wrapper.core.input;
export import dx_wrapper.core.input_enums;

export class Input
{
public:
	
	Input() = default;
	
	bool GetKeyboardKey(Key key) const;
	
	bool GetKeyboardKeyOnce(Key key) const;
	
	bool GetMouseButton(MouseButton button) const;
	
	bool GetMouseButtonOnce(MouseButton button) const;
	
	/** Internal use only. Expect to ruin things when you call this */
	void SetBit(uint32_t idx, bool val);
	/** Internal use only. Expect to ruin things when you call this */
	void BlitState();
	
private:
	
	std::bitset<0xff> m_state;
	std::bitset<0xff> m_prevState;
	
};