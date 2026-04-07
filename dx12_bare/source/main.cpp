import dx_wrapper.core.dx_device;
import dx_wrapper.rendering.gltf;
import dx_bare.dx_render;
import dx_wrapper.rendering.dx_pipelinestate;

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