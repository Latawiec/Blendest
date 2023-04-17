import bpy

scene = bpy.context.scene
print(
'''
Scene: %r
First frame: %d
Last frame: %d
Total frames: %d
''' %
(scene.name,
scene.frame_start,
scene.frame_end,
scene.frame_end - scene.frame_start + 1
)
)