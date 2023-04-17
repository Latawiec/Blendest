Ok, I figured out what I was doing wrong. Here is my final cuda_setup.py contents:
```
import bpy

def enable_gpus(device_type, use_cpus=False):
    preferences = bpy.context.preferences
    cycles_preferences = preferences.addons["cycles"].preferences
    cuda_devices, opencl_devices = cycles_preferences.get_devices()

    if device_type == "CUDA":
        devices = cuda_devices
    elif device_type == "OPENCL":
        devices = opencl_devices
    else:
        raise RuntimeError("Unsupported device type")

    activated_gpus = []

    for device in devices:
        print("Device properties: {}".format(dir(device)))
        print("Device: {}".format(device.name))
        print("Device type: {}".format(device.type))
        if device.type == "CPU":
            device.use = use_cpus
        else:
            device.use = True
            activated_gpus.append(device.name)

    cycles_preferences.compute_device_type = device_type
    bpy.context.scene.cycles.device = "GPU"

    return activated_gpus


def main():
   activated_gpu_list = enable_gpus("CUDA")
   print("Activated GPUs: {}".format(activated_gpu_list))
   bpy.data.scenes["Scene"].render.filepath = "./output_####.png"
   bpy.ops.render.render(animation=True)

main()
```
And I rendered with the following command:

blender -noaudio -b *.blend -E CYCLES -t 0 -P cuda_setup.py

This is what I think I've learned:

Adding the "-f 1" or "-a" before the "-P <python>" makes it so that frames are rendered before entering the python script, so I had to get rid of that

The bpy.ops.render.render(animation=True) is needed in the python script to make the render use the python settings

The "-b <blend_file>" is needed before the "-P <python>" so that the python script references the proper blender project