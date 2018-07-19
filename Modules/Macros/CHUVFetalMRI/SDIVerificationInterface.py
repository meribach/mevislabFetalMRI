# ----------------------------------------------------------------------------

# 
#  \file    SDIVerificationInterface.py
#  \author  FetalMRI
#  \date    2018-06-22
#
#  

# ----------------------------------------------------------------------------

from mevis import *
import re
import math

g_SDIGraphicsView = None

def initSDIVerificationGraphicsView(view):
  global g_SDIGraphicsView
  g_SDIGraphicsView = view
  
  updateInterface()
  
def updateInterface():
  
  g_sceneSDI = g_SDIGraphicsView.scene()
  g_sceneSDI.clear()
  
  mdlToSet = ""
  
  
  try:
   inImages = ctx.field("inImageInfos").object()
   listImage=sort_human(inImages.keys())
   numImage = len(inImages.keys())
   GetInfoInput=True
   
  except:
   numImage = ctx.field("NumberImages").value
   GetInfoInput=False
  
  print(numImage)
  if numImage<=6:
    for i in range(numImage):
    
      if GetInfoInput:
        checkBoxDefinition = "CheckBox {name = checkImage%i title = %s checked = True}"%(i,listImage[i])
      else:
        checkBoxDefinition = "CheckBox {name = checkImage%i title = \'Image%i\' checked = True}"%(i,i)
      
      mdlToSet += """Horizontal { """ + checkBoxDefinition + """ } """
  else:
    for i in [0,2,4]:
      if GetInfoInput:
        checkBoxDefinition = "CheckBox {name = checkImage%i title = %s checked = True}"%(i,listImage[i])
        checkBoxDefinition2 = "CheckBox {name = checkImage%i title = %s checked = True}"%(i+1,listImage[i+1])
      else:
        checkBoxDefinition = "CheckBox {name = checkImage%i title = \'Image%i\' checked = True}"%(i,i)
        checkBoxDefinition2 = "CheckBox {name = checkImage%i title = \'Image%i\' checked = True}"%(i+1,i+1)
        
      mdlToSet += """Horizontal { """ + checkBoxDefinition + checkBoxDefinition2 + """ } """
      
    listToFinish= range(6,numImage,2)
    for i in listToFinish:
      if GetInfoInput:
        checkBoxDefinition = "CheckBox {name = checkImage%i title = %s checked = True}"%(i,listImage[i])
        if (i+1)<numImage:
          checkBoxDefinition2 = "CheckBox {name = checkImage%i title = %s checked = True}"%(i+1,listImage[i+1])
        else:
          checkBoxDefinition2 = ""
      else:
        checkBoxDefinition = "CheckBox {name = checkImage%i title = \'Image%i\' checked = True}"%(i,i)
        if (i+1)<numImage:
          checkBoxDefinition2 = "CheckBox {name = checkImage%i title = \'Image%i\' checked = True}"%(i+1,i+1)
        else:
          checkBoxDefinition2 = ""
      
      mdlToSet += """Horizontal { """ + checkBoxDefinition + checkBoxDefinition2 + """ } """      
        
    
  g_sceneSDI.addMDL("Vertical {" + mdlToSet + "}")
  

def OutlierRejectionStatusChanged():
  
  print("et pataplif")
  
  
def buttonPressedAxial():
  print("clicked axial")
  
def buttonPressedCoronal():
  print("clicked coronal")
  
def buttonPressedSagittal():
  print("clicked sagittal")
  

def ReRunImageReconstruction():
  print("reRunImageReconstruction")
  

def RunSuperResolution():
  print("run SuperVariation")
  
  
def resetSDI():
  print("resetSDIImages")
  
def updateSDI():
  inImages = ctx.field("inImageInfos").object()
  #if in inImages.keys():
    
  #  ctx.field("itkImageFileReader.fileName").setStringValue()
  
def updateImage():
  ctx.field("CreateBoundingVolumeAxial.add").touch()
  ctx.field("ReformatAxial.apply").touch()
  ctx.field("BoundingBoxAxial.update").touch()
  ctx.field("CreateBoundingVolumeSagittal.add").touch()
  ctx.field("ReformatSagittal.apply").touch()
  ctx.field("BoundingBoxSagittal.update").touch()
  ctx.field("CreateBoundingVolumeCoronal.add").touch()
  ctx.field("ReformatCoronal.apply").touch()
  ctx.field("BoundingBoxCoronal.update").touch()


def sort_human(l):
  convert = lambda text: float(text) if text.isdigit() else text
  alphanum = lambda key: [ convert(c) for c in re.split('([-+]?[0-9]*\.?[0-9]*)', key) ]
  l.sort( key=alphanum )
  return l  