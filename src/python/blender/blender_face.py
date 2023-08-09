#Written by Ammar Qammaz 2022-2023
#This is a Blender Python script that upon loaded can facilitate animating a skinned model created by
#the MakeHuman plugin for Blender ( http://static.makehumancommunity.org/mpfb.html )
mnetPluginVersion=float(0.34)

import bpy
from bpy.props import EnumProperty

import os
import random
import math
import gc
import numpy as np
import array
import csv


#Steps to generate a good dataset!
#Load a BVH file and point to it
#Load an armature and point to it
#Run this python script in blender
#Run : sobolRandomDistributionForFace.py to generate a sobol/quasi-random dataset
#Point to the target dataset directory in Dataset Path 
#Point to your dataset in Dataset Path: and click Create Dataset from CSV file
# Either : 
#   run  mediapipeDumpHead2DFromRGB.py --from your dataset path
# or 
#   run  associate2DFiles.py to remake associations , update them here and rely on them as 2D data
csvResolutionErrors = 0


class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

class vertexHolder():
  def __init__(self,v,vID):
               self.co    = v.co
               self.index = vID

def timeDuration(startTimeInSeconds,endTimeInSeconds):
        timeElapsed = endTimeInSeconds - startTimeInSeconds
        timeUnit    = "seconds"
        if (timeElapsed>86400):
             timeElapsed = timeElapsed / 86400
             timeUnit    = "days"
        elif (timeElapsed>3600):
             timeElapsed = timeElapsed / 3600
             timeUnit    = "hours"
        return timeElapsed,timeUnit

def printSelectedVertex():
    mode = bpy.context.active_object.mode
    # Keep track of previous mode
    bpy.ops.object.mode_set(mode='OBJECT')
    # Go into object mode to update the selected vertices

    obj = bpy.context.object
    # Get the currently select object
    sel = np.zeros(len(obj.data.vertices), dtype=np.bool)
    # Create a numpy array with empty values for each vertex

    obj.data.vertices.foreach_get('select', sel)
    # Populate the array with True/False if the vertex is selected

    for ind in np.where(sel==True)[0]:
        # Loop over each currently selected vertex
        v = obj.data.vertices[ind]
        print(bcolors.OKGREEN)
        print('Vertex {} at position {} is selected'.format(v.index, v.co))
        print(bcolors.ENDC)
        # If you just want the first one you can break directly here
        # break

    bpy.ops.object.mode_set(mode=mode)


def combineCSVFiles(outputCSVPath,inputCSVPathList):
    targetPath = bpy.context.scene.datasetPath
    filenames = list()
    #=================================================
    for csvF in inputCSVPathList.keys():
          #print(sys.argv[i])
          csvFilename = "%s/2d_face_all_blender_%s.csv" % (targetPath,csvF)
          filenames.append(csvFilename)
          
    numberOfFiles = len(filenames)
    #=================================================
    files = list() 
    for i in range(0,numberOfFiles):
          f = open(filenames[i],'r')
          files.append(f)
    #=================================================

    #=================================================
    f = open(outputCSVPath, 'w')
    #=================================================
    readingMoreLines = True
    while readingMoreLines:
      thisTotalLine = ""
      for i in range(0,numberOfFiles):
          line = files[i].readline().replace('\n', '')
          #-------------------------------- 
          if (i!=0):
             f.write(",")
          f.write(line)
          #-------------------------------- 
          if not line:
             readingMoreLines = False
             break
      if (readingMoreLines):
          f.write("\n")
    #=================================================
    f.close()
    #=================================================
    for i in range(0,numberOfFiles):
          files[i].close()
    #=================================================

    return
    #----------------------------------------------------------------- 


def write_csv_2d_data_header(csvFilename,vertices,verticeCSVWhitelist=dict()):
    f        = open(csvFilename, 'w')
    print(csvFilename," Number of vertices :",len(vertices))
    for v in range(0,len(vertices)):
        if (v>0): 
          f.write(',')
        if 'label' in verticeCSVWhitelist:
           f.write('2DX_%s,2DY_%s,visible_%s' % (verticeCSVWhitelist['label'][v],verticeCSVWhitelist['label'][v],verticeCSVWhitelist['label'][v]))
        else:
           f.write('2DX_v%u,2DY_v%u,visible_%u' % (v,v,v))
    f.write('\n')
    f.close()    

def write_vertex_csv_2d_data(objName="",baseDirectory="/home/ammar/",fID=0,csvFile=True,svgFile=True,verticeCSVWhitelistForAllObjects=dict()):
    import bpy_extras
    from bpy_extras.object_utils import world_to_camera_view
    
    #---------------------------------------------------------------------------    
    if (objName==""):
         print("Cannot write_vertex_csv_2d_data without an obj name!")
         return
    #---------------------------------------------------------------------------    
    
    #---------------------------------------------------------------------------    
    verticeCSVWhitelist = dict()
    if (len(verticeCSVWhitelistForAllObjects.keys())>0 ): #There is a whitelist declared a.k.a. dict not empty!
      if objName in verticeCSVWhitelistForAllObjects:
        verticeCSVWhitelist = verticeCSVWhitelistForAllObjects[objName]
      else:
        #If vertice whitelists exist but not for the particular object then
        #we completely ignore the object and just return..
        #print("No vertex whitelist for ",objName,end=" ")
        #print("We assume that this means that the whole object is blacklisted! ")
        return
    #if there is no white list declared we go on as usual dumping everything..
    #---------------------------------------------------------------------------    
    
    # Get the active object
    scene    = bpy.context.scene
    camera   = scene.objects.get("Camera") #bpy.context.scene.camera
    obj      = scene.objects.get(objName) # "newgirl.body" 
    
    #Apply modifiers
    dg   = bpy.context.evaluated_depsgraph_get()
    obj  = obj.evaluated_get(dg)
    mesh = obj.to_mesh(preserve_all_data_layers=True, depsgraph=dg)
    
    #Point vertices to either all vertices or a specific list of vertices
    if ('label' in verticeCSVWhitelist) and ('body' in verticeCSVWhitelist):
        #We have an active list of vertices to select/transform so we will be economic
        #print(verticeCSVWhitelist['label'])
        numberOfVertices = len(verticeCSVWhitelist['label'])
        vertices = list()
        for i in range (0,numberOfVertices):
            vertexID = int(verticeCSVWhitelist['body'][0][i])
            newV     = vertexHolder(mesh.vertices[vertexID],vertexID)
            newV.co  = obj.matrix_world @ newV.co
            vertices.append(newV)
    else:
        #Just transform all vertices and pass them all
        mesh.transform(obj.matrix_world) # apply loc/rot/scale
        vertices = mesh.vertices     
    
    render_scale = scene.render.resolution_percentage / 100
    render_size  = (int(scene.render.resolution_x * render_scale),int(scene.render.resolution_y * render_scale),)    
    width  = render_size[0]
    height = render_size[1]

    #Zoomed Debugging Vertices
    zoom   = bpy.context.scene.zoomSVG    
    if (zoom):
       width  = 10*width
       height = 10*height
        
    #----------------------------------------------------------------------
    csvFilename = "%s/2d_face_all_blender_%s.csv" % (baseDirectory,objName)
    if (fID==0):
         write_csv_2d_data_header(csvFilename=csvFilename,vertices=vertices,verticeCSVWhitelist=verticeCSVWhitelist)
    if (csvFile):
      fCSV  = open(csvFilename, 'a')
    #----------------------------------------------------------------------
    if (svgFile):
      f = open('%s/blender_%s_face_dataset_%04u.svg'%(baseDirectory,objName,fID), 'w')
      f.write('<svg height="%u" width="%u" >\n'%(height,width))    
      f.write('<rect fill="#fff" width="%u" height="%u" />\n'%(width,height))
    #----------------------------------------------------------------------


    vNum = 0
    for v in vertices:  
        co_final= v.co# @ obj.matrix_world
        # Get the 2D projection of the vertex
        coords_2d    = bpy_extras.object_utils.world_to_camera_view(bpy.context.scene,camera,co_final)
        #print("world_to_camera_view :",coords_2d)
        x = coords_2d.x * width
        y = (1.0-coords_2d.y) * height
        visible = 0.0
        if (0<x) and (x<width) and (0<y) and (y<height):
               visible = 1.0
        #print("Pixel Coords :",x,y," - ",vNum)
        #-------------
        if (x<=width) and  (y<=height):
           if (svgFile):
              f.write('<circle cx="%u" cy="%u" r="2" stroke="black" stroke-width="3" fill="red" />'%(round(x),round(y)));
              if ('label' in verticeCSVWhitelist):
                    f.write('<text x="%u" y="%u" font-size="10px">%u %s(%u)</text>\n' % (round(x+2),round(y),vNum,verticeCSVWhitelist['label'][vNum],v.index));
              else:
                    f.write('<text x="%u" y="%u" font-size="10px">%u</text>\n' % (round(x),round(y),vNum));
              f.write('<!-- ID %u %u %u -->\n' % (x,y,vNum));
        #-------------
        if (csvFile):
          if (vNum>0): 
            fCSV.write(',')
          fCSV.write('%f,%f,%0.1f' % (coords_2d.x,(1.0-coords_2d.y),visible))
        #-------------
        vNum = vNum + 1
        
    if (svgFile):
       f.write('</svg>\n')
       f.close()
    #---------------------------------------------------------  
    if (csvFile):
      fCSV.write('\n')  
      fCSV.close()
    #print("fID ",fID," number of vertices :",len(vertices))
    return True


def write_csv_2d_data_all_objects(baseDirectory="/home/ammar/",fID=0,csvFile=True,svgFile=True,verticeCSVWhitelistForAllObjects=dict()):
  skinnedObjectName = bpy.context.scene.mnetTarget
  for obj in bpy.data.objects[skinnedObjectName].children:
    #print(skinnedObjectName," has a child ",obj.name)
    write_vertex_csv_2d_data(
                              objName=obj.name,
                              baseDirectory=baseDirectory,
                              fID=fID,
                              csvFile=csvFile,
                              svgFile=svgFile,
                              verticeCSVWhitelistForAllObjects=verticeCSVWhitelistForAllObjects
                            )

def write_csv_3d_data_header(filename):
    scene    = bpy.context.scene
    obj      = scene.objects.get("newgirl.body") 
    f        = open(filename, 'w')
    vertices = obj.data.vertices
    print("number of vertices :",len(vertices))
    for v in range(0,len(vertices)):
        if (v>0): 
          f.write(',')
        f.write('3DX_v%u,3DY_v%u,3DZ_v%u' % (v,v,v))
    f.write('\n')
    f.close()    

def write_vertex_csv_3d_data(filename="/home/ammar/",fID=0):
    return get_vertex_2d_projection(filename=filename,fID=fID)
    import bpy_extras
    from bpy_extras.object_utils import world_to_camera_view
    #-----------------------------------------------------------------
    scene    = bpy.context.scene
    camera   = scene.objects.get("Camera") #bpy.context.scene.camera
    obj      = scene.objects.get("newgirl.body")
    vertices = obj.data.vertices
    #Apply modifiers
    dg  = bpy.context.evaluated_depsgraph_get()
    obj = obj.evaluated_get(dg)

    mesh = obj.to_mesh(preserve_all_data_layers=True, depsgraph=dg)
    #co = mesh.vertices[0].co
    #co_final = obj.matrix_world @ co
    
    #mesh = obj.to_mesh(scene, True, 'PREVIEW') # apply modifiers with preview settings
    mesh.transform(obj.matrix_world) # apply loc/rot/scale
    vertices = mesh.vertices 
    #-----------------------------------------------------------------
    """
    bpy.ops.export_scene.obj(
 filepath="%s/blender_%04u.obj"%(filename,fID),
 check_existing=False,
 axis_forward='-Z',
 axis_up='Y',
 filter_glob="*.obj;*.mtl",
 use_selection=False,
 use_animation=False,
 use_mesh_modifiers=True,
 use_edges=True,
 use_smooth_groups=False,
 use_smooth_groups_bitflags=False,
 use_normals=True,
 use_uvs=True,
 use_materials=True,
 use_triangles=False,
 use_nurbs=False,
 use_vertex_groups=False,
 use_blen_objects=True,
 group_by_object=False,
 group_by_material=False,
 keep_vertex_order=False,
 global_scale=1,
 path_mode='AUTO'
)"""
    #-----------------------------------------------------------------
    csvFileName = '%s/blender.csv'%filename 
    if (fID==0):
       write_csv_3d_data_header(csvFileName)
    #-----------------------------------------------------------------    
    f    = open(csvFileName,'a')
    vNum = 0 
    for v in vertices:
        print("v :",v.co)
        #-----------------------------------------------------------------
        if (vNum!=0):
           f.write(',')
        f.write('%f,%f,%f' % (v.co.x,v.co.y,v.co.z))
        #-----------------------------------------------------------------
        vNum = vNum + 1
    #-----------------------------------------------------------------
    f.write('\n')
    f.close()    
    print("number of vertices :",len(vertices))
    return True


# ===================================================================================================================
# ===================================================================================================================
# ===================================================================================================================
# ===================================================================================================================
def resolveCSVRowColumn(data,label,sampleID):
    #---------------------------
    column = 0
    labelLowerCase = label.lower()
    for columnLabel in data['label']:
        if (columnLabel.lower()==labelLowerCase):
            return float(data['body'][sampleID][column])
        column = column+1
    #---------------------------
    global csvResolutionErrors 
    csvResolutionErrors += 1 
    if (csvResolutionErrors < 100): 
      print("Could not resolve ",label," sample ",sampleID)
    elif (csvResolutionErrors == 100): 
      print("Could not resolve ",label," sample ",sampleID)
      print("From now on will supress error output to speed up computation ")
    elif (csvResolutionErrors % 30000 == 0): 
      print("Reminder : Could not resolve ",label," sample ",sampleID," surpressed ",csvResolutionErrors," errors.. ")
      
    return float(0.0)

def convert_bytes(num):
    """
    this function will convert bytes to MB.... GB... etc
    """
    step_unit = 1000.0 #1024 bad the size

    for x in ['bytes', 'KB', 'MB', 'GB', 'TB']:
        if num < step_unit:
            return "%3.1f %s" % (num, x)
        num /= step_unit

def getNumberOfLines(filename):
    print("Counting number of lines in file ",filename)
    with open(filename) as f:
        return sum(1 for line in f) 

def checkIfPathExists(filename):
    import os
    return os.path.exists(filename) 

def checkIfFileExists(filename):
    import os
    return os.path.isfile(filename)

def readCSVFile(filename,memPercentage=1.0,useHalfFloats=0):
    import os
    import time
    print("CSV file :",filename,"..\n")

    if (not checkIfFileExists(filename)):
          print( bcolors.FAIL + "Input file "+filename+" does not exist, cannot read ground truth.." +  bcolors.ENDC)
          print("Current Directory was "+os.getcwd())
          return dict()
    start = time.time()
     
    dtypeSelected=np.dtype(np.float32)  
    dtypeSelectedByteSize=int(dtypeSelected.itemsize) 
    if (useHalfFloats):
       dtypeSelected=np.dtype(np.float16) 
       dtypeSelectedByteSize=int(dtypeSelected.itemsize)

    progress=0.0
    sampleNumber=0
    receivedHeader=False
    inputNumberOfColumns=0 
    outputNumberOfColumns=0 

    inputLabels=list() 

    #-------------------------------------------------------------------------------------------------
    numberOfSamplesInput=getNumberOfLines(filename)-1
    print(" Input file has ",numberOfSamplesInput," training samples\n")
    #-------------------------------------------------------------------------------------------------


    numberOfSamples = numberOfSamplesInput
    numberOfSamplesLimit=int(numberOfSamples*memPercentage)
    #------------------------------------------------------------------------------------------------- 
    if (memPercentage==0.0):
        print("readCSVFile was asked to occupy 0 memory so this probably means we just want one record")    
        numberOfSamplesLimit=2
    if (memPercentage>1.0):
        print("Memory Limit will be interpreted as a raw value..")     
        numberOfSamplesLimit=int(memPercentage)
    #-------------------------------------------------------------------------------------------------

 
    #---------------------------------  
    thisInput = array.array('f')  
    #---------------------------------  

    fi = open(filename, "r") 
    readerIn  = csv.reader( fi , delimiter =',', skipinitialspace=True) 
    for rowIn in readerIn: 
        #------------------------------------------------------
        if (not receivedHeader): #use header to get labels
           #------------------------------------------------------
           inputNumberOfColumns=len(rowIn)
           inputLabels = list(rowIn[i] for i in range(0,inputNumberOfColumns) )
           print("Number of Input elements : ",len(inputLabels))
           #------------------------------------------------------

           if (memPercentage==0):
               print("Will only return labels\n")
               return {'label':inputLabels}; 

           #---------------------------------  
           #         Allocate Lists 
           #---------------------------------
           for i in range(inputNumberOfColumns):
               thisInput.append(0.0)
           #---------------------------------  


           #---------------------------------  
           #      Allocate Numpy Arrays 
           #---------------------------------  
           inputSize=0
           startCompressed=0
           inputSize=inputNumberOfColumns
           startCompressed=inputNumberOfColumns  
           
           npInputBytesize=0+numberOfSamplesLimit * inputSize * dtypeSelectedByteSize
           print(" Input file on disk has a shape of [",numberOfSamples,",",inputSize,"]")  
           print(" Input we will read has a shape of [",numberOfSamplesLimit,",",inputSize,"]")  
           print(" Input will occupy ",convert_bytes(npInputBytesize)," of RAM\n")  
           npInput = np.full([numberOfSamplesLimit,inputSize],fill_value=0,dtype=dtypeSelected,order='C')
           #----------------------------------------------------------------------------------------------------------
           receivedHeader=True
           #----------------------------------------------------------------------------------------------------------
        else:
           #-------------------------------------------
           #  First convert our string INPUT to floats   
           #-------------------------------------------
           for i in range(inputNumberOfColumns):
                  try:
                    thisInput[i]=float(rowIn[i])
                  except:
                    thisInput[i]=0.0
           #------------------------------------------- 
           for num in range(0,inputNumberOfColumns):
                  npInput[sampleNumber,num]=float(thisInput[num]); 
           #-------------------------------------------
           sampleNumber=sampleNumber+1

        if (numberOfSamples>0):
           progress=sampleNumber/numberOfSamplesLimit 

        if (sampleNumber%1000==0) :
           progressString = "%0.2f"%float(100*progress)  
           print("\rReading from disk (",sampleNumber,") - ",progressString," %      \r", end="", flush=True)  

        if (numberOfSamplesLimit<=sampleNumber):
           print("\rStopping reading file to obey memory limit given by parameter --mem ",memPercentage,"\n")
           break
    #-------------------------------------------
    fi.close()
    del readerIn
    gc.collect()


    print("\n read, Samples: ",sampleNumber,", was expecting ",numberOfSamples," samples\n") 
    print(npInput.shape)

    totalNumberOfBytes=npInput.nbytes;
    totalNumberOfGigaBytes=totalNumberOfBytes/1073741824; 
    print("GPU Size Occupied by data = ",totalNumberOfGigaBytes," GB \n")

    end = time.time()
    print("Time elapsed : ",(end-start)/60," mins")
    #---------------------------------------------------------------------
    return {'label':inputLabels, 'body':npInput };

# ===================================================================================================================
# ===================================================================================================================
# ===================================================================================================================
# ===================================================================================================================



def retrieveSkinToBVHAssotiationDict(doFace=False):
  r = dict()
  if (doFace):
     r["root"]="hip"    
     r["neck1"]="neck1"      
     r["head"]="head"
     #r["__jaw"]="__jaw"
     r["jaw"]="jaw"
     #r["special04"]="special04"
     #r["oris02"]="oris02"
     r["oris01"]="oris01" #<--
     r["oris06.L"]="oris06.l"  #####
     r["oris07.L"]="oris07.l"
     r["oris06.R"]="oris06.r"  #####
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
     r["oris05"]="oris05" #<--
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
     #r["oculi02.L"]="oculi02.l" ##
     r["oculi01.L"]="oculi01.l"
     #r["__temporalis01.R"]="__temporalis01.r"
     #r["temporalis01.R"]="temporalis01.r"
     #r["oculi02.R"]="oculi02.r" ##
     r["oculi01.R"]="oculi01.r"
     #r["__temporalis02.L"]="__temporalis02.l"
     #r["temporalis02.L"]="temporalis02.l"
     #r["risorius02.L"]="risorius02.l"
     #r["risorius03.L"]="risorius03.l" ##
     #r["__temporalis02.R"]="__temporalis02.r"
     #r["temporalis02.R"]="temporalis02.r"
     #r["risorius02.R"]="risorius02.r"
     #r["risorius03.R"]="risorius03.r" ##
  return r

def degToRad(degrees):
    return degrees * math.pi / 180

def randomize_property(propName):
    #prop = bpy.data.scenes[0][propName]
    scene = bpy.data.scenes[0]
     
    prop = None
    for p in scene.bl_rna.properties:
          #print(p.name)
          if (p.name == propName):
              #print("FOUND ",propName)
              prop=p
              break
          #else:
          #      print("`%s`!=`%s`" % (p.name,propName))

    #print(type(prop))
    if isinstance(prop, bpy.types.FloatProperty): # and prop.has_min and prop.has_max:
        # Property is a float with a defined range, use min and max attributes
        min_value = prop.soft_min
        max_value = prop.soft_max
        random_value = random.uniform(min_value, max_value)
        # Set the property to the random value
        #print(propName,"randomize(%0.1f,%0.1f) = %0.2f "%(min_value,max_value,random_value))
        return random_value
    else:
        # Property is a different type, handle it differently
        # (for example, generate a random value within a reasonable range for the property type)
        print("Unable to use min/max for prop ",propName)
        return 0.0

def dumpBVHFile(r,targetPath,frameID):
  if (frameID==0):
      f = open('%s/bvh_face_all.csv' % targetPath, 'w')
      i=0
      for joint in r.keys():
        if (i>0):
           f.write(',')  
        f.write(joint)
        i=i+1
      f.write('\n')  
      f.close()    
  f = open('%s/bvh_face_all.csv' % targetPath, 'a')
  i=0
  for joint in r.keys():
        if (i>0):
           f.write(',')  
        f.write("%0.2f"%r[joint])
        i=i+1
  f.write('\n')  
  f.close()    
    
def setSkeletonRaw(jointName,z,x,y):        
        context = bpy.context
        scene   = context.scene        
        #-------------------------------------------------------
        skinnedObjectName = bpy.context.scene.mnetSource
        jointName         = jointName.lower()
        armature_obj      = bpy.context.scene.objects.get(bpy.context.scene.mnetSource) 
        #skinnedObjectName = bpy.context.scene.mnetTarget
        #print("skinnedObjectName",skinnedObjectName)
        #-------------------------------------------------------
        
        
        skinnedObject = scene.objects.get(skinnedObjectName) 
        if (skinnedObject is not None) :
           # Get the joint object
           armature = bpy.data.objects[skinnedObjectName]
           bone     = armature.pose.bones[jointName]

           # Set the rotation mode to ZXY
           bone.rotation_mode = 'ZXY'

           # Set the rotation values
           bone.rotation_euler = (degToRad(z),degToRad(x),degToRad(y))
           
           #Animation set
           #--------------------------------------------------------------------    
           # Set the joint values for the current frame
           armature_obj.pose.bones[jointName].rotation_euler = (degToRad(z),degToRad(x),degToRad(y))
           # Add a keyframe for the joint values
           armature_obj.pose.bones[jointName].keyframe_insert(data_path="rotation_euler", index=-1)
           

def setSkeletonPositionRaw(jointName,x,y,z):        
        context = bpy.context
        scene = context.scene        
        #-------------------------------------------------------
        skinnedObjectName = bpy.context.scene.mnetSource
        jointName = jointName.lower()
        armature_obj = bpy.context.scene.objects.get(bpy.context.scene.mnetSource) 
        #skinnedObjectName = bpy.context.scene.mnetTarget
        #print("skinnedObjectName",skinnedObjectName)
        #-------------------------------------------------------

        skinnedObject = scene.objects.get(skinnedObjectName) 
        if (skinnedObject is not None) :
           # Get the joint object
           armature = bpy.data.objects[skinnedObjectName]
           bone = armature.pose.bones[jointName]

           # Set the rotation mode to ZXY
           #bone.rotation_mode = 'ZXY' 
           # Set the rotation values
           #bone.rotation_euler = (degToRad(90.0),degToRad(0.0),degToRad(0.0))
           
           #Animation set
           #--------------------------------------------------------------------    
           # Set the joint values for the current frame
           armature_obj.pose.bones[jointName].location = (x,y,z)
           # Add a keyframe for the joint values
           armature_obj.pose.bones[jointName].keyframe_insert(data_path="location", index=-1)
           
    


class FaceBVHAnimationPanel(bpy.types.Panel):
    """Creates a Panel in the Object properties window"""
    bl_label       = "Face BVH Animation Helper"
    bl_idname      = "OBJECT_PT_face_panel"
    bl_space_type  = 'PROPERTIES'
    bl_region_type = 'WINDOW'
    bl_context     = "object"

    def draw(self, context):
        context = bpy.context
        scene = context.scene
        layout = self.layout

        obj = context.object
        
        #layout = layout.split(factor=0.96, align=True)
        #------------------------------------------------------------------
        #------------------------------------------------------------------
        row = layout.row()
        row.label(text="Face BVH MocapNET Helper v%0.2f" % mnetPluginVersion, icon='WORLD_DATA')
        #------------------------------------------------------------------
        row = layout.row()
        row.label(text="BVH file to use as source: ")
        row = layout.row()
        row.prop_search(scene, "mnetSource", scene, "objects", icon='ARMATURE_DATA')
        row = layout.row()
        row.operator("face.face_op",text='Link BVH').action='LINKBVH'
        #------------------------------------------------------------------
        row = layout.row()
        row.label(text="Skinned Body to use as target: ")
        row = layout.row()
        row.prop_search(scene, "mnetTarget", scene, "objects", icon='OUTLINER_OB_ARMATURE')
        #------------------------------------------------------------------
        row = layout.row()
        row.label(text="Parts of armature to animate: ")
        row = layout.row()
        row.operator("face.face_op",text='Open Mouth').action='OPENMOUTH'
        row.operator("face.face_op",text='Close Mouth').action='CLOSEMOUTH'
        row = layout.row()
        row.label(text="Positional Component : ")
        row = layout.row()
        row.operator("face.face_op",text='Open Eyes').action='OPENEYES'
        row.operator("face.face_op",text='Close Eyes').action='CLOSEEYES'
        
        
        row = layout.row()
        row.label(text="Depth : ")
        row = layout.row()
        row.prop(scene, 'posX', slider=True) 
        row = layout.row()
        row.prop(scene, 'posY', slider=True) 
        row = layout.row()
        row.prop(scene, 'depth', slider=True) 
        
        row = layout.row()
        row.label(text="Neck : ")
        row = layout.row()
        row.prop(scene, 'neck1Z', slider=True) 
        row = layout.row()
        row.prop(scene, 'neck1X', slider=True)
        row = layout.row()
        row.prop(scene, 'neck1Y', slider=True) 
        
        
        row = layout.row()
        row.label(text="Eyes : ")
        row = layout.row()
        row.prop(scene, 'eyelidLUD', slider=True) 
        row = layout.row()
        row.prop(scene, 'eyelidRUD', slider=True)
        row = layout.row()
        row.prop(scene, 'eyeLR', slider=True) 
        row = layout.row()
        row.prop(scene, 'eyeUD', slider=True)
        

        row = layout.row()
        row.label(text="Nose : ")
        row = layout.row()
        row.prop(scene, 'noseLR', slider=True)


        row = layout.row()
        row.prop(scene, 'REyebrowInUD', slider=True)
        row = layout.row()
        row.prop(scene, 'LEyebrowInUD', slider=True)
        
        row = layout.row()
        row.label(text="Mouth : ")
        row = layout.row()
        row.prop(scene, 'smileAD', slider=True) 
        row = layout.row()
        row.prop(scene, 'mouthUD', slider=True) 
        row = layout.row()
        row.prop(scene, 'mouthLR', slider=True) 
        row = layout.row()
        row.prop(scene, 'mouthOC', slider=True) 
        row = layout.row()
        row.prop(scene, 'moustacheLUD', slider=True)
        row = layout.row()
        row.prop(scene, 'moustacheRUD', slider=True)
        
        row = layout.row()
        row.prop(scene, 'mouthTopL', slider=True)
        row = layout.row()
        row.prop(scene, 'mouthTopR', slider=True)
        
        row = layout.row()
        row.prop(scene, 'mouthBotL', slider=True)
        row = layout.row()
        row.prop(scene, 'mouthBotR', slider=True)


        row = layout.row()
        row.label(text="Export controls : ")
        row = layout.row()
        row.prop(scene, 'dumpSVG')
        row.prop(scene, 'zoomSVG')
        row = layout.row()
        row.prop(scene, 'dumpPNG')
        row = layout.row()
        row.prop(scene, 'dump2D')
        row.prop(scene, 'dump3D')
        row = layout.row()
        row.prop(scene, 'dumpSpecificVertices')

        row = layout.row()
        row.operator("face.face_op",text='Take Picture').action='PHOTO'
        row = layout.row()
        row.label(text="Path to store generated dataset : ")
        row = layout.row()
        row.prop_search(scene, "datasetPath", scene, "objects", icon='FILE_FOLDER')
        row = layout.row()
        row.prop(scene, 'randomFramesNumber', slider=True) 
        row = layout.row()
        row.operator("face.face_op",text='Create Randomized Dataset').action='RANDOM'
        
        row = layout.row()
        row.label(text="Path to load pre-generated dataset : ")
        row = layout.row()
        row.prop_search(scene, "readDatasetCSVPath", scene, "objects", icon='FILE_HIDDEN')
        row = layout.row()
        row.operator("face.face_op",text='Create Dataset from CSV file').action='LOADCSV'
        row = layout.row()
        row.operator("face.face_op",text='Just Render CSV Dataset').action='RENDERCSV'
        
        
        
                
class FaceBVHAnimation(bpy.types.Operator):
    """Creates a Panel in the Object properties window"""
    bl_label = "Face BVH Animation"
    bl_idname = "face.face_op"
    bl_description = 'MocapNET operation control'
    bl_options = {'REGISTER', 'UNDO'}
    
    action: EnumProperty(
        items=[
               ('LOADCSV',    'Load Pose Data From CSV File', 'Load Pose Data From CSV File'),
               ('RENDERCSV',    'Render Pose Data From CSV File', 'Render Pose Data From CSV File'),
               ('RANDOM',     'Create Randomized Data', 'Create Randomized Data'),
               ('PHOTO',      'Take a Picture', 'Take a Picture'),
               ('LINKBVH',    'Link BVH File to Face', 'Link BVH File to Face'),
               ('OPENMOUTH',  'Link MocapNET to Skinned Model', 'Link MocapNET to Skinned Model'),
               ('CLOSEMOUTH', 'Link MocapNET to Upper Body Only', 'Link MocapNET to Upper Body Only'),
               ('OPENEYES',   'Link MocapNET to Face', 'Link MocapNET to Face'),
               ('CLOSEEYES',  'Link MocapNET positional component', 'Link MocapNET positional component')
              ]
    )

    @staticmethod
    def add_cube(context):
        bpy.ops.mesh.primitive_cube_add()

    @staticmethod
    def add_sphere(context):
        bpy.ops.mesh.primitive_uv_sphere_add() 


    @staticmethod
    def cameraLightAction(context):
        #bpy.context.scene.camera object and set its properties such as focal_length, sensor_width, and sensor_height.
        bpy.context.scene.camera.location = 0.0,0.0,0.7
        bpy.context.scene.camera.rotation_mode = 'ZXY'
        bpy.context.scene.camera.rotation_euler = degToRad(90),degToRad(0),degToRad(0)
        #bpy.ops.object.lens_distort

        light = bpy.data.objects['Light']
        light.location.x = 0.0
        light.location.y = -5.0
        light.location.z = 1.0

    @staticmethod
    def takePicture(self,context):
        print("takePicture called")

        targetPath = bpy.context.scene.datasetPath

        import os
        if (checkIfPathExists(targetPath)):
           os.system("rm %s/blender_face_dataset_*.jpg" % (targetPath))
           os.system("rm %s/blender_*_face_dataset_*.svg" % (targetPath))
        else:
           print("Cannot take picture, given path %s does not exist" % (targetPath))
           return;
       
        bpy.context.scene.frame_set(0) # Always revert to first frame on dataset generation
        #-------------------------------------------------------------
        dumpSVG              = bpy.context.scene.dumpSVG
        dumpPNG              = bpy.context.scene.dumpPNG
        dump2D               = bpy.context.scene.dump2D
        dump3D               = bpy.context.scene.dump3D
        dumpSpecificVertices = bpy.context.scene.dumpSpecificVertices
        #-------------------------------------------------------------        
        printSelectedVertex()
        #-------------------------------------------------------------
        csvVertexWhitelist=dict()
        if(dumpSpecificVertices):
          skinnedObjectName = bpy.context.scene.mnetTarget
          for obj in bpy.data.objects[skinnedObjectName].children:
           if (checkIfPathExists("%s/vertexWhitelist_%s.csv"%(targetPath,obj.name))):
            csvVertexWhitelist[obj.name] = readCSVFile("%s/vertexWhitelist_%s.csv"%(targetPath,obj.name))
           else:
            print("Could not find %s/vertexWhitelist_%s.csv "%(targetPath,obj.name))
        #-------------------------------------------------------------
        if (dump2D or dumpSVG):
          write_csv_2d_data_all_objects(baseDirectory=targetPath,fID=0,csvFile=dump2D,svgFile=dumpSVG,verticeCSVWhitelistForAllObjects=csvVertexWhitelist)
        if(dump3D):
          write_vertex_csv_3d_data(filename="/home/ammar/",fID=0)
        #-------------------------------------------------------------
        self.cameraLightAction(context=context)
        #-------------------------------------------------------------                
        bpy.context.scene.render.image_settings.file_format='JPEG'
        bpy.context.scene.render.filepath = "/home/ammar/test.jpg"
        bpy.ops.render.render(write_still = True)
        bpy.data.images['Render Result'].save_render 

    @staticmethod
    def generateRandomDataset(self,context,useCSV=False,useFFMPEG=False):
        randomFramesNumber = bpy.context.scene.randomFramesNumber
        print("generateRandomDataset called ",randomFramesNumber)
        self.cameraLightAction(context=context)

        targetPath = bpy.context.scene.datasetPath
        import time
        startAt = time.time()
        import os
        if (checkIfPathExists(targetPath)):
           os.system("rm %s/blender_face_dataset_*.jpg" % (targetPath))
           os.system("rm %s/blender_*_face_dataset_*.svg" % (targetPath))
        else:
           print("Cannot generate random dataset, given path %s does not exist" % (targetPath))
           return;

        bpy.context.scene.frame_set(0) # Always revert to first frame on start of dataset generation
        #---------------------------------------------------------------------------
        dumpSVG              = bpy.context.scene.dumpSVG
        dumpPNG              = bpy.context.scene.dumpPNG
        dump2D               = bpy.context.scene.dump2D
        dump3D               = bpy.context.scene.dump3D
        dumpSpecificVertices = bpy.context.scene.dumpSpecificVertices
        #---------------------------------------------------------------------------
        csvVertexWhitelist=dict()
        if(dumpSpecificVertices):
          skinnedObjectName = bpy.context.scene.mnetTarget
          for obj in bpy.data.objects[skinnedObjectName].children:
           if (checkIfPathExists("%s/vertexWhitelist_%s.csv"%(targetPath,obj.name))):
            csvVertexWhitelist[obj.name] = readCSVFile("%s/vertexWhitelist_%s.csv"%(targetPath,obj.name))
           else:
            print("Could not find %s/vertexWhitelist_%s.csv "%(targetPath,obj.name))
        #print(csvVertexWhitelist)
        #---------------------------------------------------------------------------
        wm = bpy.context.window_manager
        #--------------------------------------------------------------------------- 
        armature_obj = bpy.context.scene.objects.get(bpy.context.scene.mnetSource) 
        target_obj   = bpy.context.scene.objects.get(bpy.context.scene.mnetTarget)
        #--------------------------------------------------------------------------- 
         
        if armature_obj and target_obj:
            armature_mod        = target_obj.modifiers.new(name='Armature', type='ARMATURE')
        #--------------------------------------------------------------------------- 
        if (useCSV):
          #In this mode we will use the random poses found in the readDatasetCSVPath given by the user
          csvFile = bpy.context.scene.readDatasetCSVPath
          csvData = readCSVFile(csvFile) #,memPercentage=100 <- to test
          randomFramesNumber = csvData["body"].shape[0]
          print("Will now attempt to transmit ",randomFramesNumber," frames from ",csvFile)
          wm.progress_begin(0,randomFramesNumber) 
          for fID in range(0,randomFramesNumber):
            if (randomFramesNumber<10000) or (fID%1000==0):
               wm.progress_update(fID)          # Update mouse pointer progress in a conservative way to avoid X-Server error(?)
            if (dumpPNG):
               bpy.context.scene.frame_set(fID) # Set the current frame
            # Set the joint values for the current frame
            thisFaceConfig = self.retrieveFaceControls(self=self,context=context,csvData=csvData,fID=fID)
            dumpBVHFile(thisFaceConfig,targetPath,fID)
            if (dump2D or dumpSVG):
              write_csv_2d_data_all_objects(baseDirectory=targetPath,fID=fID,csvFile=dump2D,svgFile=dumpSVG,verticeCSVWhitelistForAllObjects=csvVertexWhitelist)
            if (dump3D):
              write_vertex_csv_3d_data(filename="/home/ammar/",fID=0)
        else:
          wm.progress_begin(0,randomFramesNumber) 
          #In this more we will generate random poses
          for fID in range(0,randomFramesNumber):
            if (randomFramesNumber<10000) or (fID%1000==0):
               wm.progress_update(fID)          # Update mouse pointer progress in a conservative way to avoid X-Server error(?)
            if (dumpPNG):
               bpy.context.scene.frame_set(fID) # Set the current frame
            # Set the joint values for the current frame
            thisFaceConfig = self.retrieveFaceControls(self=self,context=context)
            dumpBVHFile(thisFaceConfig,targetPath,fID)
            if (dump2D or dumpSVG):
              write_csv_2d_data_all_objects(baseDirectory=targetPath,fID=fID,csvFile=dump2D,svgFile=dumpSVG,verticeCSVWhitelistForAllObjects=csvVertexWhitelist)
            if(dump3D):
              write_vertex_csv_3d_data(filename="/home/ammar/",fID=0)
        #--------------------------------------------------------------------------- 

        if(dumpSpecificVertices):
            combineCSVFiles("%s/2d_face_all.csv"%(targetPath),csvVertexWhitelist)

        #At this point we have added all new states to animation 
        #it has happened that after a lot of hours errors like #X Error of failed request:  BadWindow (invalid Window parameter) Major opcode of failed request:  18 (X_ChangeProperty)
        #might occur so let's save our blend file to make sure we can re-render if something goes wrong!
        os.system("rm %s/faceRandomized.blend" % (targetPath))
        bpy.ops.wm.save_as_mainfile(filepath="%s/faceRandomized.blend" % (targetPath))
        
        if (dumpPNG):
          print("Will now attempt to render ",randomFramesNumber," frames ")
          renderAsAnimation=True
          #--------------------------------------------------------------------------- 
          if (renderAsAnimation):
            wm.progress_end() 
            # Set the render engine and animation settings
            #bpy.context.scene.render.engine = "CYCLES"
            bpy.context.scene.render.image_settings.file_format='JPEG'
            bpy.context.scene.render.filepath = "%s/blender_face_dataset_" % (targetPath)
            bpy.context.scene.frame_start = 0
            bpy.context.scene.frame_end = randomFramesNumber-1

            # Click the Render Animation button
            bpy.ops.render.render('INVOKE_AREA',use_viewport = True, animation=True)
          else:  
            #Then playback animation and save each frame as jpeg
            for fID in range(0,randomFramesNumber):
               bpy.context.scene.frame_set(fID)
               wm.progress_update(fID)
               bpy.context.view_layer.update() #function to update the view layer and trigger a redraw of the UI. 
               bpy.context.scene.render.image_settings.file_format='JPEG'
               bpy.context.scene.render.filepath = "%s/blender_face_dataset_%04u.jpg" % (targetPath,fID)
               bpy.ops.render.render(write_still = True)   
               bpy.data.images['Render Result'].save_render
               if (fID%1000==0):
                    gc.collect() #Do garbage collection to help with memory leaks ?
               wm.progress_end()
          #--------------------------------------------------------------------------- 
          if (useFFMPEG):
            print("Will attempt to execute : ")
            print("ffmpeg -framerate 30 -i %s/blender_face_dataset_%%04d.jpg -s 1200x720  -y -r 30 -pix_fmt yuv420p -threads 8 %s/blender.mp4"% (targetPath,targetPath))
            os.system("ffmpeg -framerate 30 -i %s/blender_face_dataset_%%04d.jpg -s 1200x720  -y -r 30 -pix_fmt yuv420p -threads 8 %s/blender.mp4" % (targetPath,targetPath))
        #---------------------------------------------------------------------------
        endAt                = time.time()
        timeElapsed,timeUnit = timeDuration(startAt,endAt)
        print("Time required to generate dataset was ",timeElapsed,timeUnit)
        #---------------------------------------------------------------------------
        #ffmpeg -framerate 30 -i blender_face_dataset_%04d.jpg -s 1200x720  -y -r 30 -pix_fmt yuv420p -threads 8 livelastRun3DHiRes.mp4
        #ffmpeg -i /media/ammar/CVRL2/ammar/frames/ammarFaceFar.mp4-data/colorFrame_0_%05d.jpg -i /media/ammar/CVRL2/ammar/rendering/blender_face_dataset_%04d.jpg -filter_complex '[1:v]colorkey=0x464646:0.01:0.02[ckout];[0:v][ckout]overlay[out]' -map '[out]' output.mp4
        # or 
        #ffmpeg -i /media/ammar/games/ammarFaceFar.mp4-data/colorFrame_0_%05d.jpg -i /media/ammar/games/render/blender_face_dataset_%04d.jpg -filter_complex '[1:v]colorkey=0x464646:0.01:0.02[ckout];[0:v][ckout]overlay[out]' -map '[out]' output.mp4

    @staticmethod
    def setSkeleton(context,jointName,z,x,y):      
        setSkeletonRaw(jointName,z,x,y)

    @staticmethod
    def retrieveConstantControls(self,context):
      r = dict()
      r["orbicularis03.R_Yrotation"]=172.0
      r["orbicularis04.R_Yrotation"]=172.0
      r["orbicularis03.L_Yrotation"]=-172.0
      r["orbicularis04.L_Yrotation"]=172.0
      r["levator06.L_Yrotation"]=-247.0
      r["levator06.R_Yrotation"]=247.0
      r["oris03.L_Xrotation"]=-40.0
      r["oris03.L_Yrotation"]=172.0
      r["oris07.L_Yrotation"]=172.0
      r["oris03.R_Xrotation"]=-40.0
      r["oris03.R_Yrotation"]=179.0
      r["oris07.R_Yrotation"]=172.0
      r["oris05_Xrotation"]=-35.0
      r["oris05_Yrotation"]=-176.0
      return r

    @staticmethod
    def retrieveFaceControlsI(self,context):
      scene = bpy.data.scenes[0]
      #--------------------------------------------------------
      r = dict()
      #--------------------------------------------------------
      r["hip_Xposition"]             = randomize_property("Pos X")
      r["hip_Yposition"]             = randomize_property("Pos Y")
      r["hip_Zposition"]             = randomize_property("Depth")
      #--------------------------------------------------------
      r["neck1_Zrotation"]           = randomize_property("Neck Z")
      r["neck1_Xrotation"]           = randomize_property("Neck X")
      r["neck1_Yrotation"]           = randomize_property("Neck Y")
      #--------------------------------------------------------
      r["eye.R_Zrotation"]           = randomize_property("Eye Gaze L/R")
      r["eye.R_Xrotation"]           = randomize_property("Eye Gaze U/D")
      r["eye.L_Zrotation"]           = r["eye.R_Zrotation"]
      r["eye.L_Xrotation"]           = r["eye.R_Xrotation"]
      #--------------------------------------------------------
      r["oculi01.R_Zrotation"]       = randomize_property("R Eyebrow In U/D")
      r["oculi01.L_Zrotation"]       = randomize_property("L Eyebrow In U/D")
      #--------------------------------------------------------
      r["orbicularis03.R_Xrotation"] = randomize_property("Eye Lid R U/D")
      r["orbicularis04.R_Xrotation"] = -r["orbicularis03.R_Xrotation"]
      r["orbicularis03.L_Xrotation"] = randomize_property("Eye Lid L U/D")
      r["orbicularis04.L_Xrotation"] = -r["orbicularis03.L_Xrotation"]
      blinkRand = random.uniform(0,1.0)
      if (blinkRand<0.85):
         r["orbicularis04.R_Xrotation"]=r["orbicularis04.L_Xrotation"]
      #--------------------------------------------------------
      r["levator06.L_Xrotation"]     = randomize_property("Nose L/R")
      r["levator06.R_Xrotation"]     = r["levator06.L_Xrotation"]
      #--------------------------------------------------------
      r["levator03.L_Zrotation"]     =  randomize_property("Smile Active/Deactivated")
      r["levator03.R_Zrotation"]     = -r["levator03.L_Zrotation"]
      #--------------------------------------------------------
      r["oris03.L_Zrotation"]        = randomize_property("Mouth Top L")
      r["oris07.L_Zrotation"]        = min(randomize_property("Mouth Sides U/D"),0)
      r["oris03.R_Zrotation"]        = randomize_property("Mouth Top R")
      r["oris07.R_Zrotation"]        = r["oris07.L_Zrotation"]
      #--------------------------------------------------------
      r["jaw_Xrotation"]             = randomize_property("Mouth Open/Close")
      r["jaw_Yrotation"]             = randomize_property("Mouth L/R")
      #--------------------------------------------------------
      r["oris04.L_Zrotation"]        = randomize_property("Moustache L U/D")
      r["oris04.R_Zrotation"]        = -randomize_property("Moustache R U/D")
      #--------------------------------------------------------
      r["oris06.L_Zrotation"]        =  randomize_property("Mouth Bot L")
      r["oris06.R_Zrotation"]        = -randomize_property("Mouth Bot R")
      #--------------------------------------------------------
      return r

    @staticmethod
    def retrieveFaceControlsFromCSV(self,context,csvData,fID):
      scene = bpy.data.scenes[0]
      #--------------------------------------------------------
      r = dict()
      #--------------------------------------------------------
      r["hip_Xposition"]             = resolveCSVRowColumn(csvData,"hip_Xposition",fID)
      r["hip_Yposition"]             = resolveCSVRowColumn(csvData,"hip_Yposition",fID)
      r["hip_Zposition"]             = resolveCSVRowColumn(csvData,"hip_Zposition",fID)
      #--------------------------------------------------------
      r["neck1_Zrotation"]           = resolveCSVRowColumn(csvData,"neck1_Zrotation",fID)
      r["neck1_Xrotation"]           = resolveCSVRowColumn(csvData,"neck1_Xrotation",fID)
      r["neck1_Yrotation"]           = resolveCSVRowColumn(csvData,"neck1_Yrotation",fID)
      #--------------------------------------------------------
      r["eye.R_Zrotation"]           = resolveCSVRowColumn(csvData,"eye.R_Zrotation",fID)
      r["eye.R_Xrotation"]           = resolveCSVRowColumn(csvData,"eye.R_Xrotation",fID)
      r["eye.L_Zrotation"]           = r["eye.R_Zrotation"]
      r["eye.L_Xrotation"]           = r["eye.R_Xrotation"]
      #--------------------------------------------------------
      r["oculi01.R_Zrotation"]       = resolveCSVRowColumn(csvData,"oculi01.R_Zrotation",fID)
      r["oculi01.L_Zrotation"]       = resolveCSVRowColumn(csvData,"oculi01.L_Zrotation",fID)
      #--------------------------------------------------------
      r["orbicularis03.R_Xrotation"] = resolveCSVRowColumn(csvData,"orbicularis03.R_Xrotation",fID)
      r["orbicularis04.R_Xrotation"] = -r["orbicularis03.R_Xrotation"]
      r["orbicularis03.L_Xrotation"] = resolveCSVRowColumn(csvData,"orbicularis03.L_Xrotation",fID)
      r["orbicularis04.L_Xrotation"] = -r["orbicularis03.L_Xrotation"]
      #--------------------------------------------------------
      r["levator06.L_Xrotation"]     = resolveCSVRowColumn(csvData,"levator06.L_Xrotation",fID)
      r["levator06.R_Xrotation"]     = r["levator06.L_Xrotation"]
      #--------------------------------------------------------
      r["levator03.L_Zrotation"]     = resolveCSVRowColumn(csvData,"levator03.L_Zrotation",fID)
      r["levator03.R_Zrotation"]     = -r["levator03.L_Zrotation"]
      #--------------------------------------------------------
      r["oris03.L_Zrotation"]        = resolveCSVRowColumn(csvData,"oris03.L_Zrotation",fID)
      r["oris07.L_Zrotation"]        = resolveCSVRowColumn(csvData,"oris07.L_Zrotation",fID)
      r["oris03.R_Zrotation"]        = resolveCSVRowColumn(csvData,"oris03.R_Zrotation",fID)
      r["oris07.R_Zrotation"]        = resolveCSVRowColumn(csvData,"oris07.R_Zrotation",fID)
      #--------------------------------------------------------
      r["jaw_Xrotation"]             = resolveCSVRowColumn(csvData,"jaw_Xrotation",fID)
      r["jaw_Yrotation"]             = resolveCSVRowColumn(csvData,"jaw_Yrotation",fID)
      #--------------------------------------------------------
      r["oris04.L_Zrotation"]        = resolveCSVRowColumn(csvData,"oris04.L_Zrotation",fID)
      r["oris04.R_Zrotation"]        = resolveCSVRowColumn(csvData,"oris04.R_Zrotation",fID)
      #--------------------------------------------------------
      r["oris06.L_Zrotation"]        = resolveCSVRowColumn(csvData,"oris06.L_Zrotation",fID)
      r["oris06.R_Zrotation"]        = resolveCSVRowColumn(csvData,"oris06.R_Zrotation",fID)
      #--------------------------------------------------------
      return r


    @staticmethod
    def retrieveFaceControls(self,context,csvData=dict(),fID=0):
     scene = bpy.data.scenes[0]
     doIt=True
     r = dict()
     if (doIt):
      #-----------------------------------------------------------------
      if ("body" in csvData):
         r.update(self.retrieveFaceControlsFromCSV(self=self,context=context,csvData=csvData,fID=fID))
      else:
         r.update(self.retrieveFaceControlsI(self=self,context=context))
      #-----------------------------------------------------------------
      scene['posX']           = r["hip_Xposition"]
      scene['posY']           = r["hip_Yposition"]
      scene['depth']          = r["hip_Zposition"]
      bpy.context.scene.depth = scene['depth']
      #-----------------------------------------------------------------
      scene['neck1Z'] = r["neck1_Zrotation"]
      scene['neck1X'] = r["neck1_Xrotation"]
      scene['neck1Y'] = r["neck1_Yrotation"]
      bpy.context.scene.neckZ = scene['neck1Z']
      bpy.context.scene.neckX = scene['neck1X']
      bpy.context.scene.neckY = scene['neck1Y']
      #-----------------------------------------------------------------
      scene['eyeLR'] = r["eye.R_Zrotation"]
      scene['eyeUD'] = r["eye.R_Xrotation"]
      bpy.context.scene.eyeLR = scene['eyeLR']
      bpy.context.scene.eyeUD = scene['eyeUD']
      eyeLR = bpy.context.scene.eyeLR
      eyeUD = bpy.context.scene.eyeUD
      #-----------------------------------------------------------------
      scene['REyebrowInUD'] = -r["oculi01.R_Zrotation"]
      scene['LEyebrowInUD'] =  r["oculi01.L_Zrotation"]
      bpy.context.scene.REyebrowInUD = scene['REyebrowInUD']
      bpy.context.scene.LEyebrowInUD = scene['LEyebrowInUD']
      REyebrowInUD = bpy.context.scene.REyebrowInUD
      LEyebrowInUD = bpy.context.scene.LEyebrowInUD
      #-----------------------------------------------------------------
      scene['eyelidLUD'] = r["orbicularis04.L_Xrotation"]
      scene['eyelidRUD'] = r["orbicularis04.R_Xrotation"]
      bpy.context.scene.eyelidLUD = scene['eyelidLUD']
      bpy.context.scene.eyelidRUD = scene['eyelidRUD']
      eyelidRUD = bpy.context.scene.eyelidRUD
      eyelidLUD = bpy.context.scene.eyelidLUD
      #-----------------------------------------------------------------
      scene['noseLR'] = r["levator06.L_Xrotation"]
      bpy.context.scene.noseLR = scene['noseLR']
      noseLR = bpy.context.scene.noseLR
      #-----------------------------------------------------------------
      scene['smileAD'] = r["levator03.L_Zrotation"]
      bpy.context.scene.smileAD = scene['smileAD']
      smileAD = bpy.context.scene.smileAD
      #-----------------------------------------------------------------
      scene['mouthTopL'] = r["oris03.L_Zrotation"]
      scene['mouthTopR'] = -r["oris03.R_Zrotation"]
      bpy.context.scene.mouthTopL    = scene['mouthTopL']
      bpy.context.scene.mouthTopR    = scene['mouthTopR']
      mouthTopL = bpy.context.scene.mouthTopL
      mouthTopR = bpy.context.scene.mouthTopR
      scene['mouthUD'] = r["oris07.L_Zrotation"]
      bpy.context.scene.mouthUD = scene['mouthUD']
      mouthUD = bpy.context.scene.mouthUD
      #-----------------------------------------------------------------
      scene['mouthLR'] = r["jaw_Yrotation"]
      bpy.context.scene.mouthLR    = scene['mouthLR']
      mouthLR = bpy.context.scene.mouthLR
      scene['mouthOC'] = r["jaw_Xrotation"]
      bpy.context.scene.mouthOC    = scene['mouthOC'] 
      mouthOC = bpy.context.scene.mouthOC
      #-----------------------------------------------------------------
      scene['moustacheLUD'] =  r["oris04.L_Zrotation"]
      scene['moustacheRUD'] = -r["oris04.R_Zrotation"]
      bpy.context.scene.moustacheLUD    = scene['moustacheLUD']
      bpy.context.scene.moustacheRUD    = scene['moustacheRUD']
      moustacheLUD = bpy.context.scene.moustacheLUD
      moustacheRUD = bpy.context.scene.moustacheRUD
      #-----------------------------------------------------------------
      scene['mouthBotL'] =  r["oris06.L_Zrotation"]
      scene['mouthBotR'] = -r["oris06.R_Zrotation"]
      bpy.context.scene.mouthBotL = scene['mouthBotL']
      bpy.context.scene.mouthBotR = scene['mouthBotR']
      mouthBotL = bpy.context.scene.mouthBotL
      mouthBotR = bpy.context.scene.mouthBotR
      #-----------------------------------------------------------------
      r.update(self.retrieveConstantControls(self=self,context=context))
      #-----------------------------------------------------------------
      self.neckUpdate(self=self,context=context)
      self.eyeGazeUpdate(self=self,context=context)
      self.noseUpdate(self=self,context=context)
      self.mouthUpdate(self=self,context=context)
     return r


    @staticmethod
    def eyeGazeUpdate(self, context):
            eyeLR = bpy.context.scene.eyeLR
            eyeUD = bpy.context.scene.eyeUD              #x   #y   #z  
            FaceBVHAnimation.setSkeleton(context,"eye.R",eyeUD,0,eyeLR)
            FaceBVHAnimation.setSkeleton(context,"eye.L",eyeUD,0,eyeLR)
            #-----------------------------------------------------------------
            REyebrowInUD = bpy.context.scene.REyebrowInUD
            LEyebrowInUD = bpy.context.scene.LEyebrowInUD   #x #y  #z  
            FaceBVHAnimation.setSkeleton(context,"oculi01.R",0,0,-REyebrowInUD)
            FaceBVHAnimation.setSkeleton(context,"oculi01.L",0,0,LEyebrowInUD)
            #-----------------------------------------------------------------
            eyelidRUD = bpy.context.scene.eyelidRUD
            eyelidLUD = bpy.context.scene.eyelidLUD                #x     #y     #z  
            FaceBVHAnimation.setSkeleton(context,"orbicularis03.R",-eyelidRUD,172,0)
            FaceBVHAnimation.setSkeleton(context,"orbicularis04.R",eyelidRUD,172,0)
            FaceBVHAnimation.setSkeleton(context,"orbicularis03.L",-eyelidLUD,-172,0)
            FaceBVHAnimation.setSkeleton(context,"orbicularis04.L",eyelidLUD,172,0)

    @staticmethod
    def noseUpdate(self, context):
            noseLR = bpy.context.scene.noseLR                 #x     #y     #z
            FaceBVHAnimation.setSkeleton(context,"levator06.L",noseLR,-247,0)
            FaceBVHAnimation.setSkeleton(context,"levator06.R",noseLR,+247,0)

    @staticmethod
    def neckUpdate(self, context):
            neckZ = bpy.context.scene.neckZ                 #x     #y     #z
            neckX = bpy.context.scene.neckX                 #x     #y     #z
            neckY = bpy.context.scene.neckY                 #x     #y     #z
            FaceBVHAnimation.setSkeleton(context,"neck1",neckX,neckY,neckZ)           
            target_obj   = bpy.context.scene.objects.get(bpy.context.scene.mnetTarget) 
            target_obj.location.x = 0.0
            target_obj.location.y = 0.0 + bpy.context.scene.depth
            target_obj.location.z = 0.0
            target_obj.rotation_mode = 'ZXY' 
            target_obj.rotation_euler = (degToRad(0.0),degToRad(0.0),degToRad(0.0))
            setSkeletonPositionRaw("hip",bpy.context.scene.posX,bpy.context.scene.posY,bpy.context.scene.depth) 

    @staticmethod
    def mouthUpdate(self, context):
            mouthTopL = bpy.context.scene.mouthTopL
            mouthTopR = bpy.context.scene.mouthTopR        
            mouthUD   = bpy.context.scene.mouthUD            #x     #y     #z
            FaceBVHAnimation.setSkeleton(context,"oris03.L",-40,172,mouthUD+mouthTopL)
            FaceBVHAnimation.setSkeleton(context,"oris07.L",0,172,max(mouthUD,0))   
            FaceBVHAnimation.setSkeleton(context,"oris03.R",-40,179,-mouthUD+mouthTopR)
            FaceBVHAnimation.setSkeleton(context,"oris07.R",0,172,min(mouthUD,0))
            #-----------------------------------------------------------------
            mouthOC = bpy.context.scene.mouthOC   
            mouthLR = bpy.context.scene.mouthLR   
            FaceBVHAnimation.setSkeleton(context,"jaw",mouthOC,mouthLR,0)
            #-----------------------------------------------------------------
            FaceBVHAnimation.setSkeleton(context,"oris05",-35,-176,0)  
            #-----------------------------------------------------------------
            moustacheLUD = bpy.context.scene.moustacheLUD
            moustacheRUD = bpy.context.scene.moustacheRUD  #x #y  #z
            FaceBVHAnimation.setSkeleton(context,"oris04.L",0,0,moustacheLUD)    
            FaceBVHAnimation.setSkeleton(context,"oris04.R",0,0,moustacheRUD)
            #-----------------------------------------------------------------
            mouthBotL = bpy.context.scene.mouthBotL
            mouthBotR = bpy.context.scene.mouthBotR        #x #y  #z
            FaceBVHAnimation.setSkeleton(context,"oris06.L",0,0,mouthBotL)    
            FaceBVHAnimation.setSkeleton(context,"oris06.R",0,0,mouthBotR)
            #-----------------------------------------------------------------
            smileAD = bpy.context.scene.smileAD               #x #y  #z
            FaceBVHAnimation.setSkeleton(context,"levator03.L",0,0,smileAD)    
            FaceBVHAnimation.setSkeleton(context,"levator03.R",0,0,-smileAD) 
            #-----------------------------------------------------------------

    @staticmethod
    def copyFaceConstraints(context,doPosition=False,doRotation=True,doReverse=False):
        FaceBVHAnimation.cameraLightAction(context=context)
        context = bpy.context
        scene = context.scene        
        #-------------------------------------------------------
        associations = retrieveSkinToBVHAssotiationDict(doFace=True)
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
                
                if (doReverse):
                 # give it a copy rotation constraint
                 if (skinnedBone is not None) and (bvhBone is not None):
                   if (len(skinnedBone.constraints)>0):   
                     for c in bvhBone.constraints:
                       bvhBone.constraints.remove(c)  # Remove constraint
                   if (skinnedBoneName=="root") and (doPosition):
                      crc = bvhBone.constraints.new('COPY_LOCATION')
                      crc.target    = skinnedObject
                      crc.subtarget = skinnedBoneName
                   elif (skinnedBoneName!="root") and (doRotation):
                      crc = bvhBone.constraints.new('COPY_ROTATION')
                      crc.target    = skinnedObject
                      crc.subtarget = skinnedBoneName
                else:
                 # give it a copy rotation constraint
                 if (skinnedBone is not None) and (bvhBone is not None):
                   if (len(skinnedBone.constraints)>0):   
                     for c in skinnedBone.constraints:
                       skinnedBone.constraints.remove(c)  # Remove constraint
                   if (skinnedBoneName=="root") and (doPosition):
                      crc = skinnedBone.constraints.new('COPY_LOCATION')
                      crc.target    = bvhObject
                      crc.subtarget = bvhBoneName
                   elif (skinnedBoneName!="root") and (doRotation):
                      crc = skinnedBone.constraints.new('COPY_ROTATION')
                      crc.target    = bvhObject
                      crc.subtarget = bvhBoneName
        #-------------------------------------------------------

    def execute(self, context):
        if self.action == 'LOADCSV':
            self.generateRandomDataset(self=self,context=context,useCSV=True)
        elif self.action == 'RENDERCSV':
            dumpSVG              = bpy.context.scene.dumpSVG;  bpy.context.scene.dumpSVG = False
            dumpPNG              = bpy.context.scene.dumpPNG;  bpy.context.scene.dumpPNG = True
            dump2D               = bpy.context.scene.dump2D;   bpy.context.scene.dump2D  = False
            dump3D               = bpy.context.scene.dump3D;   bpy.context.scene.dump3D  = False
            dumpSpecificVertices = bpy.context.scene.dumpSpecificVertices; bpy.context.scene.dumpSpecificVertices  = False         
            self.generateRandomDataset(self=self,context=context,useCSV=True,useFFMPEG=True)
        elif self.action == 'RANDOM':
            self.generateRandomDataset(self=self,context=context)
        elif self.action == 'LINKBVH':
            self.copyFaceConstraints(context=context,doPosition=True)
            self.eyeGazeUpdate(self=self,context=context)
            self.noseUpdate(self=self,context=context)
            self.neckUpdate(self=self,context=context)
            self.mouthUpdate(self=self,context=context)
        elif self.action == 'REVERSELINKBVH':
            self.copyFaceConstraints(context=context,doReverse=True)
        elif self.action == 'PHOTO':
            self.takePicture(self=self,context=context)
        elif self.action == 'OPENMOUTH':
            self.setSkeleton(context,"jaw",20,0,0)
        elif self.action == 'CLOSEMOUTH':
            self.setSkeleton(context,"jaw",0,0,0)
        elif self.action == 'OPENEYES':
            self.setSkeleton(context,"orbicularis03.R",0,172,0)
            self.setSkeleton(context,"orbicularis04.R",0,172,0)
            self.setSkeleton(context,"orbicularis03.L",0,172,0)
            self.setSkeleton(context,"orbicularis04.L",0,172,0)
            self.eyeGazeUpdate(self=self,context=context)
        elif self.action == 'CLOSEEYES':
            self.setSkeleton(context,"orbicularis03.R",-15,149,0)
            self.setSkeleton(context,"orbicularis04.R",15,172,0)
            self.setSkeleton(context,"orbicularis03.L",-15,193,0)
            self.setSkeleton(context,"orbicularis04.L",15,172,0)
        return {'FINISHED'}

classes = (FaceBVHAnimationPanel,FaceBVHAnimation)

def register():
    for cls in classes:
        bpy.utils.register_class(cls)
        
    bpy.types.Scene.datasetPath  = bpy.props.StringProperty(name="Dataset Path", default="~/", subtype="DIR_PATH")
    bpy.types.Scene.readDatasetCSVPath = bpy.props.StringProperty(name="Dataset Path", default="~/", subtype="FILE_PATH")
    bpy.types.Scene.mnetSource   = bpy.props.StringProperty(name="Source BVH", default="Select Armature Object")
    bpy.types.Scene.mnetTarget   = bpy.props.StringProperty(name="Target Obj", default="Select Skinned Object")
    #--------------------------------------------------------------------------------------------------------------------------------------------
    bpy.types.Scene.zoomSVG      = bpy.props.BoolProperty(name="Zoom SVG", default=False)
    bpy.types.Scene.dumpSVG      = bpy.props.BoolProperty(name="Dump SVG", default=False)
    bpy.types.Scene.dumpPNG      = bpy.props.BoolProperty(name="Dump PNG", default=True)
    bpy.types.Scene.dumpSpecificVertices  = bpy.props.BoolProperty(name="Only Dump 2D/3D for Specific Vertices", default=True)
    bpy.types.Scene.dump2D       = bpy.props.BoolProperty(name="Dump 2D CSV", default=True)
    bpy.types.Scene.dump3D       = bpy.props.BoolProperty(name="Dump 3D CSV", default=False)
    #--------------------------------------------------------------------------------------------------------------------------------------------
    bpy.types.Scene.neckZ        = bpy.props.FloatProperty(name="Neck Z", default=0.0, min=-20.0, max=20.0, update=FaceBVHAnimation.neckUpdate)
    bpy.types.Scene.neckX        = bpy.props.FloatProperty(name="Neck X", default=0.0, min=-20.0, max=20.0, update=FaceBVHAnimation.neckUpdate)
    bpy.types.Scene.neckY        = bpy.props.FloatProperty(name="Neck Y", default=0.0, min=-30.0, max=30.0, update=FaceBVHAnimation.neckUpdate)
    #--------------------------------------------------------------------------------------------------------------------------------------------
    bpy.types.Scene.posX         = bpy.props.FloatProperty(name="Pos X", default=0.0, min=-0.24, max=0.24, update=FaceBVHAnimation.neckUpdate)
    bpy.types.Scene.posY         = bpy.props.FloatProperty(name="Pos Y", default=0.0, min=-0.1, max=0.1, update=FaceBVHAnimation.neckUpdate)
    bpy.types.Scene.depth        = bpy.props.FloatProperty(name="Depth", default=-1.0, min=-2.4, max=-1.0, update=FaceBVHAnimation.neckUpdate)
    #--------------------------------------------------------------------------------------------------------------------------------------------
    bpy.types.Scene.eyeLR        = bpy.props.FloatProperty(name="Eye Gaze L/R", default=0.0, min=-45.36, max=45.36, update=FaceBVHAnimation.eyeGazeUpdate)
    bpy.types.Scene.eyeUD        = bpy.props.FloatProperty(name="Eye Gaze U/D", default=0.0, min=-10.0, max=16.0, update=FaceBVHAnimation.eyeGazeUpdate)
    bpy.types.Scene.eyelidLUD    = bpy.props.FloatProperty(name="Eye Lid L U/D", default=0.0, min=-15.0, max=15.0, update=FaceBVHAnimation.eyeGazeUpdate)
    bpy.types.Scene.eyelidRUD    = bpy.props.FloatProperty(name="Eye Lid R U/D", default=0.0, min=-15.0, max=15.0, update=FaceBVHAnimation.eyeGazeUpdate)
    #--------------------------------------------------------------------------------------------------------------------------------------------    
    bpy.types.Scene.noseLR       = bpy.props.FloatProperty(name="Nose L/R", default=0.0, min=-9.0, max=9.0, update=FaceBVHAnimation.noseUpdate)
    #--------------------------------------------------------------------------------------------------------------------------------------------
    bpy.types.Scene.mouthUD      = bpy.props.FloatProperty(name="Mouth Sides U/D", default=0.0, min=-30.0, max=0.0, update=FaceBVHAnimation.mouthUpdate)
    bpy.types.Scene.mouthLR      = bpy.props.FloatProperty(name="Mouth L/R", default=0.0, min=-15.0, max=15.0, update=FaceBVHAnimation.mouthUpdate)
    bpy.types.Scene.mouthOC      = bpy.props.FloatProperty(name="Mouth Open/Close", default=0.0, min=-4.0, max=20.0, update=FaceBVHAnimation.mouthUpdate)
    bpy.types.Scene.moustacheLUD = bpy.props.FloatProperty(name="Moustache L U/D", default=0.0, min=-30.0, max=0.0, update=FaceBVHAnimation.mouthUpdate)
    bpy.types.Scene.moustacheRUD = bpy.props.FloatProperty(name="Moustache R U/D", default=0.0, min=0.0, max=30.0, update=FaceBVHAnimation.mouthUpdate)
    bpy.types.Scene.mouthTopL    = bpy.props.FloatProperty(name="Mouth Top L", default=0.0, min=-30.0, max=30.0, update=FaceBVHAnimation.mouthUpdate)
    bpy.types.Scene.mouthTopR    = bpy.props.FloatProperty(name="Mouth Top R", default=0.0, min=-30.0, max=30.0, update=FaceBVHAnimation.mouthUpdate)
    bpy.types.Scene.mouthBotL    = bpy.props.FloatProperty(name="Mouth Bot L", default=0.0, min=-30.0, max=30.0, update=FaceBVHAnimation.mouthUpdate)
    bpy.types.Scene.mouthBotR    = bpy.props.FloatProperty(name="Mouth Bot R", default=0.0, min=-30.0, max=30.0, update=FaceBVHAnimation.mouthUpdate)
    #--------------------------------------------------------------------------------------------------------------------------------------------
    bpy.types.Scene.smileAD      = bpy.props.FloatProperty(name="Smile Active/Deactivated", default=0.0, min=-8.0, max=9.0, update=FaceBVHAnimation.mouthUpdate)
    #--------------------------------------------------------------------------------------------------------------------------------------------    
    bpy.types.Scene.randomFramesNumber = bpy.props.IntProperty(name="Random Frames", default=0, min=0, max=200000) #, update=FaceBVHAnimation.generateRandomDataset    
    #--------------------------------------------------------------------------------------------------------------------------------------------
    bpy.types.Scene.REyebrowInUD = bpy.props.FloatProperty(name="R Eyebrow In U/D", default=0.0, min=-20.0, max=20.0, update=FaceBVHAnimation.eyeGazeUpdate)
    bpy.types.Scene.LEyebrowInUD = bpy.props.FloatProperty(name="L Eyebrow In U/D", default=0.0, min=-20.0, max=20.0, update=FaceBVHAnimation.eyeGazeUpdate)

def unregister():
    for cls in classes:
        bpy.utils.unregister_class(cls)
        
    del bpy.types.Scene.datasetPath
    del bpy.types.Scene.readDatasetCSVPath
    del bpy.types.Scene.mnetSource
    del bpy.types.Scene.mnetTarget
    #--------------------------------------------------------------------------------------------------------------------------------------------
    del bpy.types.Scene.zoomSVG
    del bpy.types.Scene.dumpSVG
    del bpy.types.Scene.dumpPNG
    del bpy.types.Scene.dumpSpecificVertices
    del bpy.types.Scene.dump2D
    del bpy.types.Scene.dump3D     
    #--------------------------------------------------------------------------------------------------------------------------------------------
    del bpy.types.Scene.neck1Z
    del bpy.types.Scene.neck1X
    del bpy.types.Scene.neck1Y
    #--------------------------------------------------------------------------------------------------------------------------------------------
    del bpy.types.Scene.posX
    del bpy.types.Scene.posY
    del bpy.types.Scene.depth
    del bpy.types.Scene.eyeLR
    del bpy.types.Scene.eyeUD
    del bpy.types.Scene.eyelidLUD
    del bpy.types.Scene.eyelidRUD
    #--------------------------------------------------------------------------------------------------------------------------------------------
    del bpy.types.Scene.noseLR
    #--------------------------------------------------------------------------------------------------------------------------------------------
    del bpy.types.Scene.mouthUD
    del bpy.types.Scene.mouthLR
    del bpy.types.Scene.mouthOC
    del bpy.types.Scene.moustacheLUD
    del bpy.types.Scene.moustacheRUD
    del bpy.types.Scene.mouthTopL
    del bpy.types.Scene.mouthTopR
    del bpy.types.Scene.mouthBotL
    del bpy.types.Scene.mouthBotR
    #--------------------------------------------------------------------------------------------------------------------------------------------
    del bpy.types.Scene.smileAD
    #--------------------------------------------------------------------------------------------------------------------------------------------
    del bpy.types.Scene.randomFramesNumber
    #--------------------------------------------------------------------------------------------------------------------------------------------
    del bpy.types.Scene.REyebrowInUD
    del bpy.types.Scene.LEyebrowInUD
    
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
