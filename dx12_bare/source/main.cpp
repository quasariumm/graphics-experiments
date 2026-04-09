import dx_wrapper.core;
import dx_wrapper.rendering;

import dx_bare.rendering.dx_render;

int main()
{
	DxDevice device;
	DxRenderer renderer{&device};
	
	renderer.AddModel("../assets/DamagedHelmet/DamagedHelmet.gltf");
	
	while (!device.ShouldClose())
	{
		device.BeginFrame();
		renderer.Render();
		device.EndFrame();
	}
	
	return 0;
}