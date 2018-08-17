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
import os

g_SDIGraphicsView = None
g_HorizontalControl = {}

def initSDIVerificationGraphicsView(view):
  global g_SDIGraphicsView
  g_SDIGraphicsView = view
  print("##### initSDIVerificationGraphicsView")
  updateInterface()
  
def updateInterface():
  
  g_sceneSDI = g_SDIGraphicsView.scene()
  g_sceneSDI.clear()
  
  mdlToSet = ""
  
  try:
   inImages = ctx.field("inImageInfos").object()
   listLRImage = [keyImage for keyImage in list(inImages.keys()) if "Image" in keyImage]
   listImage=sort_human(listLRImage)
   numImage = len(listLRImage)
   ctx.field("NumberImages").setIntValue(numImage)
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
      
      mdlToSet += """Horizontal { name = \"horizontal%i\"  """%i + checkBoxDefinition + """ Execute = "py: getHorizontalControl(\'Image%i\',\'horizontal%i\')" } """%(i,i)
  else:
    for i in [0,2,4]:
      if GetInfoInput:
        checkBoxDefinition = "CheckBox {name = checkImage%i title = %s checked = True}"%(i,listImage[i])
        checkBoxDefinition2 = "CheckBox {name = checkImage%i title = %s checked = True}"%(i+1,listImage[i+1])
      else:
        checkBoxDefinition = "CheckBox {name = checkImage%i title = \'Image%i\' checked = True}"%(i,i)
        checkBoxDefinition2 = "CheckBox {name = checkImage%i title = \'Image%i\' checked = True}"%(i+1,i+1)
        
      mdlToSet += """Horizontal { name = \"horizontal%i\"  """%i + checkBoxDefinition + checkBoxDefinition2 + """ Execute = "py: getHorizontalControl(\'Image%i\',\'horizontal%i\')" } """%(i,i)
      
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
      
      mdlToSet += """Horizontal {name = \"horizontal%i\"  """%i + checkBoxDefinition + checkBoxDefinition2 + """ Execute = "py: getHorizontalControl(\'Image%i\',\'horizontal%i\')" } """%(i,i)   
        
        
  g_sceneSDI.addMDL("Vertical {" + mdlToSet + "}")
  

def OutlierRejectionStatusChanged():
  
  print("et pataplif")
  
  
def buttonPressedAxial():
  print("clicked axial")
  
def buttonPressedCoronal():
  print("clicked coronal")
  
def buttonPressedSagittal():
  print("clicked sagittal")
  

def RunSuperResolution():
  print("run SuperVariation")
  inImages = ctx.field("inImageInfos").object()
  
  
def ReRunImageReconstruction():
  print("reRunImageReconstruction")
  inImages = ctx.field("inImageInfos").object()
  
  numIm = ctx.field("NumberImages").value
  outTransform = ""
  inputFiles = ""
  maskFiles = ""
  for imageIter in inImages:
    if "Image" in imageIter:
      if g_HorizontalControl[imageIter].control("check%s"%imageIter).isChecked():
        if outTransform!="":
          outTransform=outTransform+"--"
          inputFiles=inputFiles+"--"
          maskFiles=maskFiles+"--"
      
        outTransform = outTransform +inImages[imageIter]["ImReOriented"].replace(".nii.gz","_transform_%iV_1.txt"%numIm)
        inputFiles = inputFiles + inImages[imageIter]["NLMBCorr"]
        maskFiles = maskFiles + inImages[imageIter]["MaskReOriented"]
    
  ctx.field("mialImageReconstruction.inputFiles").setStringValue(inputFiles)
  ctx.field("mialImageReconstruction.maskFiles").setStringValue(maskFiles)
  ctx.field("mialImageReconstruction.transformoutFiles").setStringValue(outTransform)
  ctx.field("mialImageReconstruction.outputFile").setStringValue(os.path.join(os.path.dirname(inImages["Image0"]["file"]),"SDI_ITER1.nii.gz"))
  ctx.field("mialImageReconstruction.startTask").touch()
 

def insertImageReconstruction():
  inImages = ctx.field("inImageInfos").object()
  print("insertImageReconstruction")
  inImages.update({"SDI_ITER1":os.path.join(os.path.dirname(inImages["Image0"]["file"]),"SDI_ITER1.nii.gz")})
  updateSDI()
  MLAB.processEvents()

def updateSDI():
  inImages = ctx.field("inImageInfos").object()
  if inImages is not None:
    if "SDI_ITER1" in inImages.keys():
      ctx.field("itkImageFileReader.fileName").setStringValue(inImages["SDI_ITER1"])
  
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

def showHelp():
  import webbrowser
  webbrowser.open_new(ctx.expandFilename("$(MLAB_CHUV_FetalMRI)/Documentation/Publish/ModuleReference/SDIVerificationInterface.html"))

def getHorizontalControl(image,horizon):
  
  #print("get %s control"%horizon)
  global g_HorizontalControl
  g_HorizontalControl.update({image:ctx.control(horizon)})

def sort_human(l):
  convert = lambda text: float(text) if text.isdigit() else text
  alphanum = lambda key: [ convert(c) for c in re.split('([-+]?[0-9]*\.?[0-9]*)', key) ]
  l.sort( key=alphanum )
  return l  