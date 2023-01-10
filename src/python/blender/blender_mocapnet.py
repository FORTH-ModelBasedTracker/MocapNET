#Written by Ammar Qammaz 2022-2023
#This is a Blender Python script that upon loaded can facilitate animating a skinned model created by
#the MakeHuman plugin for Blender ( http://static.makehumancommunity.org/mpfb.html )
mnetPluginVersion=float(0.02)

import bpy
from bpy.props import EnumProperty


def retrieveSkinToBVHAssotiationDict(doBody=True,doHands=True,doFeet=True,doFace=False):
  r = dict()
  if (doBody):     
     r["root"]="Hip" #This should be hip not Hip but for some reason (ZYX rotation) there is a discrepancy
     r["spine03"]="abdomen"
     r["spine04"]="chest"
     #---------------------------
     r["shoulder01.L"]="lCollar"
     r["upperarm01.L"]="lShldr"
     r["lowerarm01.L"]="lForeArm"
     r["wrist.L"]="lHand"
     #---------------------------
     r["shoulder01.R"]="rCollar"
     r["upperarm01.R"]="rShldr"
     r["lowerarm01.R"]="rForeArm"
     r["wrist.R"]="rHand"
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
     r["upperleg02.L"]="lThigh"
     r["lowerleg01.L"]="lShin"
     r["foot.L"]="lFoot"
     #---------------------------
     r["upperleg02.R"]="rThigh"
     r["lowerleg01.R"]="rShin"
     r["foot.R"]="rFoot"
     
     #---------------------------
     # L Foot
     #---------------------------
     r["toe1-1.L"]="toe1-1.L"
     r["toe1-2.L"]="toe1-2.L"
     for toe in range(2,6):
        for part in range(1,4):
          r["toe%u-%u.L"%(toe,part)]="toe%u-%u.L"%(toe,part)             
     #---------------------------
     
     #---------------------------
     # R Foot
     #---------------------------
     r["toe1-1.R"]="toe1-1.R"
     r["toe1-2.R"]="toe1-2.R"
     for toe in range(2,6):
        for part in range(1,4):
          r["toe%u-%u.R"%(toe,part)]="toe%u-%u.R"%(toe,part)             
     #---------------------------
     
     
     #---------------------------
     # Face
     #---------------------------           
     #---------------------------
  if (doFace):
     r["neck01"]="neck1"      
     r["head"]="head"
     r["__jaw"]="__jaw"
     r["jaw"]="jaw"
     r["special04"]="special04"
     r["oris02"]="oris02"
     r["oris01"]="oris01"
     r["oris06.L"]="oris06.l"
     r["oris07.L"]="oris07.l"
     r["oris06.R"]="oris06.r"
     r["oris07.R"]="oris07.r"
     r["tongue00"]="tongue00"
     r["tongue01"]="tongue01"
     r["tongue02"]="tongue02"
     r["tongue03"]="tongue03"
     r["__tongue04"]="__tongue04"
     r["tongue04"]="tongue04"
     r["tongue07.L"]="tongue07.l"
     r["tongue07.R"]="tongue07.r"
     r["tongue06.L"]="tongue06.l"
     r["tongue06.R"]="tongue06.r"
     r["tongue05.L"]="tongue05.l"
     r["tongue05.R"]="tongue05.r"
     r["__levator02.L"]="__levator02.l"
     r["levator02.L"]="levator02.l"
     r["levator03.L"]="levator03.l"
     r["levator04.L"]="levator04.l"
     r["levator05.L"]="levator05.l"
     r["__levator02.R"]="__levator02.r"
     r["levator02.R"]="levator02.r"
     r["levator03.R"]="levator03.r"
     r["levator04.R"]="levator04.r"
     r["levator05.R"]="levator05.r"
     r["__special01"]="__special01"
     r["special01"]="special01"
     r["oris04.L"]="oris04.l"
     r["oris03.L"]="oris03.l"
     r["oris04.R"]="oris04.r"
     r["oris03.R"]="oris03.r"
     r["oris06"]="oris06"
     r["oris05"]="oris05"
     r["__special03"]="__special03"
     r["special03"]="special03"
     r["__levator06.L"]="__levator06.l"
     r["levator06.L"]="levator06.l"
     r["__levator06.R"]="__levator06.r"
     r["levator06.R"]="levator06.r"
     r["special06.L"]="special06.l"
     r["special05.L"]="special05.l"
     r["eye.L"]="eye.l"
     r["orbicularis03.L"]="orbicularis03.l"
     r["orbicularis04.L"]="orbicularis04.l"
     r["special06.R"]="special06.r"
     r["special05.R"]="special05.r"
     r["eye.R"]="eye.r"
     r["orbicularis03.R"]="orbicularis03.r"
     r["orbicularis04.R"]="orbicularis04.r"
     r["__temporalis01.L"]="__temporalis01.l"
     r["temporalis01.L"]="temporalis01.l"
     r["oculi02.L"]="oculi02.l"
     r["oculi01.L"]="oculi01.l"
     r["__temporalis01.R"]="__temporalis01.r"
     r["temporalis01.R"]="temporalis01.r"
     r["oculi02.R"]="oculi02.r"
     r["oculi01.R"]="oculi01.r"
     r["__temporalis02.L"]="__temporalis02.l"
     r["temporalis02.L"]="temporalis02.l"
     r["risorius02.L"]="risorius02.l"
     r["risorius03.L"]="risorius03.l"
     r["__temporalis02.R"]="__temporalis02.r"
     r["temporalis02.R"]="temporalis02.r"
     r["risorius02.R"]="risorius02.r"
     r["risorius03.R"]="risorius03.r"
     
     
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
        associations = retrieveSkinToBVHAssotiationDict(doBody=doBody,doHands=doHands,doFeet=doFeet,doFace=doFace)
        #-------------------------------------------------------
        bvhObjectName     = bpy.context.scene.mnetSource
        skinnedObjectName = bpy.context.scene.mnetTarget
        print("bvhObjectName",bvhObjectName)
        print("skinnedObjectName",skinnedObjectName)
        #-------------------------------------------------------
        skinnedObject = scene.objects.get(skinnedObjectName)
        bvhObject     = scene.objects.get(bvhObjectName)
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
           MocapNETBVHAnimation)
        
def register():
    for cls in classes:
        bpy.utils.register_class(cls)
    bpy.types.Scene.mnetSource = bpy.props.StringProperty(name="Source BVH", default="Select BVH Object")
    bpy.types.Scene.mnetTarget = bpy.props.StringProperty(name="Target Obj", default="Select Skinned Object")
    


def unregister():
    for cls in classes:
        bpy.utils.unregister_class(cls)
    del bpy.types.Scene.mnetSource
    del bpy.types.Scene.mnetTarget

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
         
