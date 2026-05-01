import dx_wrapper.core;

int main()
{
	DxDevice device;
	
	while (!device.ShouldClose())
	{
		device.BeginFrame();
		device.EndFrame();
	}
	
	return 0;
}