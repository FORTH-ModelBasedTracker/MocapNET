#Written by Ammar Qammaz 2022-2023
#This is a Blender Python script that upon loaded can facilitate animating a skinned model created by
#the MakeHuman plugin for Blender ( http://static.makehumancommunity.org/mpfb.html )
mnetPluginVersion=float(0.12)


import math
import bpy
from bpy.props import EnumProperty

def setup_camera_from_intrinsics(
    fx, fy, cx, cy,
    width, height,
    camera_location=(0.0, -3.0, 1.5),
    camera_rotation=(math.radians(90), 0.0, 0.0),
    clip_start=0.01,
    clip_end=100.0,
    sensor_fit='HORIZONTAL'
):
    """
    Configure Blender's render camera using OpenCV-style intrinsics.

    Args:
        fx, fy: focal lengths in pixels
        cx, cy: principal point (in pixels)
        width, height: image resolution in pixels
        camera_location: camera position in meters
        camera_rotation: rotation (Euler XYZ) in radians
        clip_start, clip_end: near/far clipping distances in meters
        sensor_fit: 'HORIZONTAL' or 'VERTICAL' (how Blender fits the sensor)
    """
    # Ensure there is a camera in the scene
    if "RenderCamera" in bpy.data.objects:
        cam_obj = bpy.data.objects["RenderCamera"]
        cam = cam_obj.data
    else:
        cam_data = bpy.data.cameras.new("RenderCamera")
        cam_obj = bpy.data.objects.new("RenderCamera", cam_data)
        bpy.context.collection.objects.link(cam_obj)
        cam = cam_data

    # Calculate sensor width/height in mm
    # Blender expects: focal_length_mm / sensor_width_mm = fx / width_in_pixels
    # We assume fx, fy in pixel units; pick sensor size that preserves aspect ratio
    sensor_width_mm = 36.0
    sensor_height_mm = sensor_width_mm * (height / width)

    # Compute focal length in mm
    focal_length_mm = fx * sensor_width_mm / width

    cam.lens = focal_length_mm
    cam.sensor_width = sensor_width_mm
    cam.sensor_height = sensor_height_mm
    cam.sensor_fit = sensor_fit
    cam.clip_start = clip_start
    cam.clip_end = clip_end

    # Set resolution
    scene = bpy.context.scene
    scene.render.resolution_x = width
    scene.render.resolution_y = height

    # Set the principal point (optical center)
    # Blender’s principal point offset is in normalized sensor coordinates (mm)
    cam.shift_x = -(cx - width / 2) / width
    cam.shift_y = (cy - height / 2) / height  # Blender Y shift has opposite sign to image space

    # Apply transform
    cam_obj.location = camera_location
    cam_obj.rotation_euler = camera_rotation

    # Activate camera
    scene.camera = cam_obj

    print(f"[INFO] Camera configured with fx={fx:.2f}, fy={fy:.2f}, cx={cx:.2f}, cy={cy:.2f}")
    print(f"[INFO] Sensor: {sensor_width_mm:.2f}mm x {sensor_height_mm:.2f}mm, Focal length: {focal_length_mm:.2f}mm")
    print(f"[INFO] Image: {width}x{height}, Shift: ({cam.shift_x:.4f}, {cam.shift_y:.4f})")

    return cam_obj



def set_eevee_background(scene=None,R=1.0,G=1.0,B=1.0):
    """
    Set the render background to pure white in Eevee, handling world nodes.
    """
    if scene is None:
        scene = bpy.context.scene

    if scene.world is None:
        world = bpy.data.worlds.new("World")
        scene.world = world

    world = scene.world

    if not world.use_nodes:
        world.use_nodes = True

    nodes = world.node_tree.nodes
    links = world.node_tree.links

    # Find or create Background node
    bg_node = next((n for n in nodes if n.type == 'BACKGROUND'), None)
    if bg_node is None:
        bg_node = nodes.new('ShaderNodeBackground')

    # Set color to white
    bg_node.inputs['Color'].default_value = (R, G, B, 1.0)

    # Find or create World Output node
    output_node = next((n for n in nodes if n.type == 'OUTPUT_WORLD'), None)
    if output_node is None:
        output_node = nodes.new('ShaderNodeOutputWorld')

    # Connect Background node to World Output if not already connected
    if not any(link.to_node == output_node for link in bg_node.outputs['Background'].links):
        links.new(bg_node.outputs['Background'], output_node.inputs['Surface'])

    # Optional: disable ambient lighting to prevent gray tint in Eevee
    #scene.eevee.use_gtao = False
    #scene.eevee.use_bloom = False
    #scene.eevee.use_ssr = False
    #scene.eevee.use_ssr_refraction = False
    
    

class MocapNETSetupCameraOperator(bpy.types.Operator):
    """Set up Blender Camera from Intrinsics"""
    bl_label = "Set Camera From Intrinsics"
    bl_idname = "mocapnet.setup_camera"
    bl_description = "Configure the active camera using fx, fy, cx, cy, width, height"
    bl_options = {'REGISTER', 'UNDO'}

    def execute(self, context):
        scene = context.scene

        fx = scene.mnet_fx
        fy = scene.mnet_fy
        cx = scene.mnet_cx
        cy = scene.mnet_cy
        width = scene.mnet_width
        height = scene.mnet_height

        setup_camera_from_intrinsics(
            fx=fx, fy=fy, cx=cx, cy=cy,
            width=width, height=height,
            camera_location=(0.0, -4.0, 1.7),
            camera_rotation=(math.radians(90), 0.0, 0.0)
        )

        
        # Set Eevee background to white
        set_eevee_background(scene,0.01,0.01,0.01)

        self.report({'INFO'}, f"Scene color set to {scene.world.color}")


        self.report({'INFO'}, f"Camera set with fx={fx}, fy={fy}, cx={cx}, cy={cy}, res={width}x{height}")
        return {'FINISHED'}
    
    
    

def decide_name(bvh_object, possible_names):
    if bvh_object is None or not hasattr(bvh_object, "pose"):
        return None

    for name in possible_names:
        if name in bvh_object.pose.bones:
            return name
    return None

def retrieveSkinToBVHAssotiationDict(bvhObj=None,doBody=True,doHands=True,doFeet=True,doFace=False):
  r = dict()
  if (doBody):     
     r["root"]="Hip" #This should be hip not Hip but for some reason (ZYX rotation) there is a discrepancy
     r["spine03"]="abdomen"
     r["spine04"]="chest"
     r["neck01"]="neck1"      
     r["neck03"]="neck2"      
     #---------------------------
     r["shoulder01.L"]=decide_name(bvhObj,["lCollar","lcollar"])
     r["upperarm01.L"]=decide_name(bvhObj,["lShldr","lshoulder"])
     r["lowerarm01.L"]=decide_name(bvhObj,["lForeArm","lelbow"])
     r["wrist.L"]     =decide_name(bvhObj,["lHand","lhand"])
     #---------------------------
     r["shoulder01.R"]=decide_name(bvhObj,["rCollar","rcollar"])
     r["upperarm01.R"]=decide_name(bvhObj,["rShldr","rshoulder"])
     r["lowerarm01.R"]=decide_name(bvhObj,["rForeArm","relbow"])
     r["wrist.R"]     =decide_name(bvhObj,["rHand","rhand"])
     #---------------------------

  if (doHands):     
     #---------------------------
     # L Hand
     #---------------------------
     r["finger1-1.L"]="lthumb"
     r["finger1-2.L"]="finger1-2.l"
     r["finger1-3.L"]="finger1-3.l"
     #---------------------------
     r["metacarpal1.L"]="metacarpal1.l"
     r["finger2-1.L"]="finger2-1.l"
     r["finger2-2.L"]="finger2-2.l"
     r["finger2-3.L"]="finger2-3.l"
     #---------------------------
     r["metacarpal2.L"]="metacarpal2.l"
     r["finger3-1.L"]="finger3-1.l"
     r["finger3-2.L"]="finger3-2.l"
     r["finger3-3.L"]="finger3-3.l"
     #---------------------------
     r["metacarpal3.L"]="metacarpal3.l"
     r["finger4-1.L"]="finger4-1.l"
     r["finger4-2.L"]="finger4-2.l"
     r["finger4-3.L"]="finger4-3.l"
     #---------------------------
     r["metacarpal4.L"]="metacarpal4.l"
     r["finger5-1.L"]="finger5-1.l"
     r["finger5-2.L"]="finger5-2.l"
     r["finger5-3.L"]="finger5-3.l"
     #---------------------------
     # R Hand
     #---------------------------
     r["finger1-1.R"]="rthumb"
     r["finger1-2.R"]="finger1-2.r"
     r["finger1-3.R"]="finger1-3.r"
     #---------------------------
     r["metacarpal1.R"]="metacarpal1.r"
     r["finger2-1.R"]="finger2-1.r"
     r["finger2-2.R"]="finger2-2.r"
     r["finger2-3.R"]="finger2-3.r"
     #---------------------------
     r["metacarpal2.R"]="metacarpal2.r"
     r["finger3-1.R"]="finger3-1.r"
     r["finger3-2.R"]="finger3-2.r"
     r["finger3-3.R"]="finger3-3.r"
     #---------------------------
     r["metacarpal3.R"]="metacarpal3.r"
     r["finger4-1.R"]="finger4-1.r"
     r["finger4-2.R"]="finger4-2.r"
     r["finger4-3.R"]="finger4-3.r"
     #---------------------------
     r["metacarpal4.R"]="metacarpal4.r"
     r["finger5-1.R"]="finger5-1.r"
     r["finger5-2.R"]="finger5-2.r"
     r["finger5-3.R"]="finger5-3.r"
     #---------------------------
     
  if (doFeet):
     r["upperleg02.L"]=decide_name(bvhObj,["lThigh","lhip"])
     r["lowerleg01.L"]=decide_name(bvhObj,["lShin","lknee"])
     r["foot.L"]      =decide_name(bvhObj,["lFoot","lfoot"]) 
     #---------------------------
     r["upperleg02.R"]=decide_name(bvhObj,["rThigh","rhip"])
     r["lowerleg01.R"]=decide_name(bvhObj,["rShin","rknee"])
     r["foot.R"]      =decide_name(bvhObj,["rFoot","rfoot"]) 
     
     #---------------------------
     # L Foot
     #---------------------------
     r["toe1-1.L"]=decide_name(bvhObj,["toe1-1.L","toe1-1.l"])
     r["toe1-2.L"]=decide_name(bvhObj,["toe1-2.L","toe1-2.l"])
     for toe in range(2,6):
        for part in range(1,4):
          r["toe%u-%u.L"%(toe,part)]=decide_name(bvhObj,[
                                                          "toe%u-%u.L"%(toe,part),
                                                          "toe%u-%u.l"%(toe,part) ] )
     #---------------------------
     
     #---------------------------
     # R Foot
     #---------------------------
     r["toe1-1.R"]=decide_name(bvhObj,["toe1-1.R","toe1-1.r"])
     r["toe1-2.R"]=decide_name(bvhObj,["toe1-2.R","toe1-2.r"])
     for toe in range(2,6):
        for part in range(1,4):
          r["toe%u-%u.R"%(toe,part)]=decide_name(bvhObj,[
                                                          "toe%u-%u.R"%(toe,part),
                                                          "toe%u-%u.r"%(toe,part) ] )            
     #---------------------------
     
     
     #---------------------------
     # Face
     #---------------------------           
     #---------------------------
  if (doFace):
     #r["head"]="head"
     #r["__jaw"]="__jaw"
     r["jaw"]="jaw"
     #r["special04"]="special04"
     #r["oris02"]="oris02"
     #r["oris01"]="oris01"
     ##r["oris06.L"]="oris06.l"
     r["oris07.L"]="oris07.l"
     ##r["oris06.R"]="oris06.r"
     r["oris07.R"]="oris07.r"
     #r["tongue00"]="tongue00"
     #r["tongue01"]="tongue01"
     #r["tongue02"]="tongue02"
     #r["tongue03"]="tongue03"
     #r["__tongue04"]="__tongue04"
     #r["tongue04"]="tongue04"
     #r["tongue07.L"]="tongue07.l"
     #r["tongue07.R"]="tongue07.r"
     #r["tongue06.L"]="tongue06.l"
     #r["tongue06.R"]="tongue06.r"
     #r["tongue05.L"]="tongue05.l"
     #r["tongue05.R"]="tongue05.r"
     #r["__levator02.L"]="__levator02.l"
     #r["levator02.L"]="levator02.l"
     r["levator03.L"]="levator03.l"
     #r["levator04.L"]="levator04.l"
     #r["levator05.L"]="levator05.l"
     #r["__levator02.R"]="__levator02.r"
     #r["levator02.R"]="levator02.r"
     r["levator03.R"]="levator03.r"
     #r["levator04.R"]="levator04.r"
     #r["levator05.R"]="levator05.r"
     #r["__special01"]="__special01"
     #r["special01"]="special01"
     r["oris04.L"]="oris04.l"
     r["oris03.L"]="oris03.l"
     r["oris04.R"]="oris04.r"
     r["oris03.R"]="oris03.r"
     #r["oris06"]="oris06"
     #r["oris05"]="oris05"
     #r["__special03"]="__special03"
     #r["special03"]="special03"
     #r["__levator06.L"]="__levator06.l"
     r["levator06.L"]="levator06.l"
     #r["__levator06.R"]="__levator06.r"
     r["levator06.R"]="levator06.r"
     #r["special06.L"]="special06.l"
     #r["special05.L"]="special05.l"
     r["eye.L"]="eye.l"
     r["orbicularis03.L"]="orbicularis03.l"
     r["orbicularis04.L"]="orbicularis04.l"
     #r["special06.R"]="special06.r"
     #r["special05.R"]="special05.r"
     r["eye.R"]="eye.r"
     r["orbicularis03.R"]="orbicularis03.r"
     r["orbicularis04.R"]="orbicularis04.r"
     #r["__temporalis01.L"]="__temporalis01.l"
     #r["temporalis01.L"]="temporalis01.l"
     #r["oculi02.L"]="oculi02.l"
     r["oculi01.L"]="oculi01.l"
     #r["__temporalis01.R"]="__temporalis01.r"
     #r["temporalis01.R"]="temporalis01.r"
     #r["oculi02.R"]="oculi02.r"
     r["oculi01.R"]="oculi01.r"
     #r["__temporalis02.L"]="__temporalis02.l"
     #r["temporalis02.L"]="temporalis02.l"
     #r["risorius02.L"]="risorius02.l"
     #r["risorius03.L"]="risorius03.l"
     #r["__temporalis02.R"]="__temporalis02.r"
     #r["temporalis02.R"]="temporalis02.r"
     #r["risorius02.R"]="risorius02.r"
     #r["risorius03.R"]="risorius03.r"
  return r



class MocapNETBVHAnimationPanel(bpy.types.Panel):
    """Creates a Panel in the Object properties window"""
    bl_label = "MocapNET BVH Animation Helper"
    bl_idname = "OBJECT_PT_mocapnet_panel"
    bl_space_type = 'PROPERTIES'
    bl_region_type = 'WINDOW'
    bl_context = "object"

    def draw(self, context):
        context = bpy.context
        scene = context.scene
        layout = self.layout
        
        
        obj = context.object
        
        #layout = layout.split(factor=0.96, align=True)
        #------------------------------------------------------------------
        #------------------------------------------------------------------
        row = layout.row()
        row.label(text="MocapNET BVH Animation Helper  v%0.2f" % mnetPluginVersion, icon='WORLD_DATA')
        #------------------------------------------------------------------
        row = layout.row()
        row.label(text="BVH file to use as source: ")
        row = layout.row()
        row.prop_search(scene, "mnetSource", scene, "objects", icon='ARMATURE_DATA')
        #------------------------------------------------------------------
        row = layout.row()
        row.label(text="Skinned Body to use as target: ")
        row = layout.row()
        row.prop_search(scene, "mnetTarget", scene, "objects", icon='OUTLINER_OB_ARMATURE')
        #------------------------------------------------------------------
        row = layout.row()
        row.label(text="Parts of MocapNET BVH file to link: ")
        row = layout.row()
        row.operator("mocapnet.mocapnet_op",text='Automatic').action='LINK'
        row.operator("mocapnet.mocapnet_op",text='Upperbody').action='LINKUP'
        row.operator("mocapnet.mocapnet_op",text='Face').action='LINKFACE'
        row = layout.row()
        row.label(text="Positional Component : ")
        row = layout.row()
        row.operator("mocapnet.mocapnet_op",text='Link Position').action='LINKPOS'
        
        
        #------------------------------------------------------------------
        row = layout.row()
        row.label(text="Camera Intrinsics (OpenCV style):", icon='CAMERA_DATA')

        col = layout.column(align=True)
        col.prop(scene, "mnet_fx", text="fx (px)")
        col.prop(scene, "mnet_fy", text="fy (px)")
        col.prop(scene, "mnet_cx", text="cx (px)")
        col.prop(scene, "mnet_cy", text="cy (px)")
        col.prop(scene, "mnet_width", text="Width (px)")
        col.prop(scene, "mnet_height", text="Height (px)")

        row = layout.row()
        row.operator("mocapnet.setup_camera", text="Set Camera From Intrinsics", icon='CAMERA_DATA')

        
        
class MocapNETBVHAnimation(bpy.types.Operator):
    """Creates a Panel in the Object properties window"""
    bl_label = "MocapNET BVH Animation"
    bl_idname = "mocapnet.mocapnet_op"
    bl_description = 'MocapNET operation control'
    bl_options = {'REGISTER', 'UNDO'}
    
    action: EnumProperty(
        items=[
            ('LINK', 'Link MocapNET to Skinned Model', 'Link MocapNET to Skinned Model'),
            ('LINKUP', 'Link MocapNET to Upper Body Only', 'Link MocapNET to Upper Body Only'),
            ('LINKFACE', 'Link MocapNET to Face', 'Link MocapNET to Face'),
            ('LINKPOS', 'Link MocapNET positional component', 'Link MocapNET positional component')
        ]
    )

    @staticmethod
    def add_cube(context):
        bpy.ops.mesh.primitive_cube_add()

    @staticmethod
    def add_sphere(context):
        bpy.ops.mesh.primitive_uv_sphere_add()

    @staticmethod
    def copyPosition(context,doBody=True,doHands=True,doFeet=True,doFace=False):        
        context = bpy.context
        scene = context.scene        
        #-------------------------------------------------------
        #associations = retrieveSkinToBVHAssotiationDict(doBody=doBody,doHands=doHands,doFeet=doFeet,doFace=doFace)
        #-------------------------------------------------------
        bvhObjectName     = bpy.context.scene.mnetSource
        skinnedObjectName = bpy.context.scene.mnetTarget
        print("bvhObjectName",bvhObjectName)
        print("skinnedObjectName",skinnedObjectName)
        #-------------------------------------------------------
        skinnedObject = scene.objects.get(skinnedObjectName)
        bvhObject     = scene.objects.get(bvhObjectName)
        if (skinnedObject is not None) and (bvhObject is not None):
                skinnedBoneName = "root"
                bvhBoneName     = "hip"
                #------------------------------------------------
                #bvhBoneName = associations[skinnedBoneName]
                #------------------------------------------------
                skinnedBone = skinnedObject.pose.bones.get(skinnedBoneName) 
                bvhBone     = bvhObject.pose.bones.get(bvhBoneName)
                print("pos associate ",skinnedBoneName," -> ",bvhBoneName)
                # give it a copy rotation constraint
                if (skinnedBone is not None) and (bvhBone is not None):
                    if (skinnedBoneName=="root"):
                      crc = skinnedBone.constraints.new('COPY_LOCATION')
                      crc.target    = bvhObject
                      crc.subtarget = bvhBoneName
                      print("DONE ",skinnedBoneName)



    @staticmethod
    def copySkeletonConstraints(context,doBody=True,doHands=True,doFeet=True,doFace=False,doPosition=False,doRotation=True):        
        context = bpy.context
        scene = context.scene        
        #-------------------------------------------------------
        bvhObjectName     = bpy.context.scene.mnetSource
        skinnedObjectName = bpy.context.scene.mnetTarget
        print("bvhObjectName",bvhObjectName)
        print("skinnedObjectName",skinnedObjectName)
        #-------------------------------------------------------
        skinnedObject = scene.objects.get(skinnedObjectName)
        bvhObject     = scene.objects.get(bvhObjectName)
        #-------------------------------------------------------
        associations = retrieveSkinToBVHAssotiationDict(bvhObj=bvhObject,doBody=doBody,doHands=doHands,doFeet=doFeet,doFace=doFace)
        #-------------------------------------------------------
        if (skinnedObject is not None) and (bvhObject is not None):
            for skinnedBoneName in associations:
                #------------------------------------------------
                bvhBoneName = associations[skinnedBoneName]
                #------------------------------------------------
                skinnedBone = skinnedObject.pose.bones.get(skinnedBoneName) 
                bvhBone     = bvhObject.pose.bones.get(bvhBoneName)
                # give it a copy rotation constraint
                if (skinnedBone is not None) and (bvhBone is not None):
                   if (len(skinnedBone.constraints)>0):   
                     for c in skinnedBone.constraints:
                       skinnedBone.constraints.remove(c)  # Remove constraint
                   if (skinnedBoneName=="root") and (doPosition):
                      crc = skinnedBone.constraints.new('COPY_LOCATION')
                      crc.target    = bvhObject
                      crc.subtarget = bvhBoneName
                   if (doRotation):
                      crc = skinnedBone.constraints.new('COPY_ROTATION')
                      crc.target    = bvhObject
                      crc.subtarget = bvhBoneName

    def execute(self, context):
        if self.action == 'LINK':
            self.copySkeletonConstraints(context=context,doBody=True,doHands=True,doFeet=True,doFace=False)
        elif self.action == 'LINKUP':
            self.copySkeletonConstraints(context=context,doBody=True,doHands=True,doFeet=False,doFace=False)
        elif self.action == 'LINKFACE':
            self.copySkeletonConstraints(context=context,doBody=False,doHands=False,doFeet=False,doFace=True)
        elif self.action == 'LINKPOS':
            self.copyPosition(context=context,doBody=True,doHands=False,doFeet=False,doFace=False)
        return {'FINISHED'}
    

classes = (MocapNETBVHAnimationPanel,
           MocapNETBVHAnimation,
           MocapNETSetupCameraOperator)
        
def register():
    for cls in classes:
        bpy.utils.register_class(cls)
    bpy.types.Scene.mnetSource = bpy.props.StringProperty(name="Source BVH", default="Select BVH Object")
    bpy.types.Scene.mnetTarget = bpy.props.StringProperty(name="Target Obj", default="Select Skinned Object")
    
    bpy.types.Scene.mnet_fx = bpy.props.FloatProperty(name="fx", default=1200.0)
    bpy.types.Scene.mnet_fy = bpy.props.FloatProperty(name="fy", default=1200.0)
    bpy.types.Scene.mnet_cx = bpy.props.FloatProperty(name="cx", default=960.0)
    bpy.types.Scene.mnet_cy = bpy.props.FloatProperty(name="cy", default=540.0)
    bpy.types.Scene.mnet_width = bpy.props.IntProperty(name="Width", default=1920)
    bpy.types.Scene.mnet_height = bpy.props.IntProperty(name="Height", default=1080)

def unregister():
    for cls in classes:
        bpy.utils.unregister_class(cls)
    del bpy.types.Scene.mnetSource
    del bpy.types.Scene.mnetTarget
    
    del bpy.types.Scene.mnet_fx
    del bpy.types.Scene.mnet_fy
    del bpy.types.Scene.mnet_cx
    del bpy.types.Scene.mnet_cy
    del bpy.types.Scene.mnet_width
    del bpy.types.Scene.mnet_height    

if __name__ == "__main__":
    register()

    # Get a list of all add-ons that are currently activated
    activated_addons = [addon.module for addon in bpy.context.preferences.addons if addon]

    haveMPFB = False
    # Print the name of each activated add-on
    for addon in activated_addons:
       print(addon)
       if (addon=="mpfb"):
           haveMPFB = True
           print("We already have MPFB!")
       elif (addon=="bl_ext.user_default.mpfb"):
           haveMPFB = True
           print("We already have MPFB!")
    
    if (not haveMPFB):
         import os
         print("Receiving a fresh copy of MPFB!")
         current_directory = os.getcwd()
         print("Working from ",current_directory," directory")
         os.system("wget http://download.tuxfamily.org/makehuman/plugins/mpfb2-latest.zip")
         print(" Downloaded mpfb2-latest.zip and will now auto-install it for your convinience ")
         bpy.ops.preferences.addon_install(filepath='%s/mpfb2-latest.zip' % os.getcwd())
         bpy.ops.preferences.addon_enable(module='mpfb')
         bpy.ops.wm.save_userpref()
         
         # Get the path to the user preferences file
         prefs_file = bpy.utils.user_resource('CONFIG') #bpy.context.preferences.filepath

         # Get the directory that contains the preferences file
         prefs_dir = os.path.dirname(prefs_file)

         print(" Also installing the makehuman system assets!")
         os.system("cd %s/mpfb/data && wget http://files.makehumancommunity.org/asset_packs/makehuman_system_assets/makehuman_system_assets_cc0.zip && unzip makehuman_system_assets_cc0.zip && rm makehuman_system_assets_cc0.zip" % prefs_dir)
         
