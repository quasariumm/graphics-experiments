import core.dx_device;

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