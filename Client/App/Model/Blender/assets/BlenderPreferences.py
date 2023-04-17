import bpy

preferences = bpy.context.preferences
cyclesPreferences = preferences.addons["cycles"].preferences

cyclesPreferences.refresh_devices()
deviceTypes = cyclesPreferences.get_device_types(bpy.context)

availableDevicesForType = {}

for deviceType in deviceTypes:
    availableDevicesForType[deviceType[0]] = cyclesPreferences.get_devices_for_type(deviceType[0])

for deviceType, devices in availableDevicesForType.items():
    if devices:
        print(f'{deviceType}:')
        for device in devices:
            print(f'-{device.name}')


