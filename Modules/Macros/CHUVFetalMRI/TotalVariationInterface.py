# ----------------------------------------------------------------------------

# 
#  \file    TotalVariationInterface.py
#  \author  FetalMRI
#  \date    2018-07-17
#
#  

# ----------------------------------------------------------------------------

from mevis import *
import re
import math
import os

g_TVGraphicsView = None

def initTotalVariationInterface(view):
  global g_TVGraphicsView
  g_TVGraphicsView = view
  print("##### initTVGraphicsView")
  updateInterface()
  
  
def updateInterface():
  
  g_sceneTV = g_TVGraphicsView.scene()
  g_sceneTV.clear()
  
  mdlToSet = ""
  
  try:
   inImages = ctx.field("inImageInfos").object()
   listLRImage = [keyImage for keyImage in list(inImages.keys()) if "Image" in keyImage]
   listImage=sort_human(listLRImage)
   numImage = len(listLRImage)
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
        
    
  g_sceneTV.addMDL("Vertical {" + mdlToSet + "}")
  

  
  
def buttonPressedAxial():
  print("clicked axial")
  
def buttonPressedCoronal():
  print("clicked coronal")
  
def buttonPressedSagittal():
  print("clicked sagittal")
  
  
def RunSuperResolution():
  print("run SuperVariation")
  inImages = ctx.field("inImageInfos").object()
  
def resetTV():
  print("resetTVImages")

def insertTVSuperResolution():
    
  if not ctx.field("mialTVSuperResolution.outputSucceed").value:
    return
  
  inImages = ctx.field("inImageInfos").object()
  
  if inImages is None:
    return
  
  inImages.update({"SRTV_ITER%i"%ctx.field("NumberIteration").value:os.path.join(os.path.dirname(inImages["Image0"]["file"]),"SRTV_ITER%i.nii.gz"%ctx.field("NumberIteration").value)})
  
  ctx.field("inImageInfos").setObject(inImages)
  updateTotalVariationInterface()
  convertToDicom("SRTV_ITER%i"%ctx.field("NumberIteration").value)
  
  if ctx.field("NumberIteration").value < ctx.field("NbIterationToDo").value:
    print("continue")
  
  
def convertToDicom(WhatToConvert):

  print("nifti to dicom conversion")
  inImages = ctx.field("inImageInfos").object()
  #(0028,1052) RescaleIntercept:  0
  #(0028,1053) RescaleSlope:  100
  #(0028,1054) RescaleType:  US
  # out-folder
  
  ctx.field("itkImageFileReader1.fileName").setStringValue(WhatToConvert)
  
  if ctx.field("FromFrontier").value:
    #we use dicom tool from testinstall, dicomSend
    print("via Frontier")
    _frontier = ctx.module("parent:FrontierSyngoInterface").object()
    ctx.field("parent:DicomExport.exportBaseDir").setStringValue(_frontier.getOutgoingDicomDirectory())
    DicomToolToUse = ctx.module("parent:DicomExport")
    print(ctx.field("parent:DicomExport.exportBaseDir").value)
  else:
    #we use dicom tool from TotalVariationInterface, dicomSave
    print("not via Frontier")
    DicomToolToUse = ctx.module("DicomTool")
    ctx.field("DicomTool.exportBaseDir").setStringValue(os.path.join(os.path.dirname(inImages["Image0"]["file"]),"Results"))
  
  ctx.field("DicomTagModify.tagValue3").setValue(WhatToConvert)
  ctx.field("DicomTagModify.apply").touch()
  DicomToolToUse.field("exportNameTemplate").setStringValue("$S/"+WhatToConvert+"$T.dcm")
  DicomToolToUse.field("saveSlices").touch()

def updateTotalVariationInterface():
  inImages = ctx.field("inImageInfos").object()
  listSRTV_ITER=[]
  if inImages is not None:
    for inImagesIter in inImages.keys():
     if inImagesIter.startswith("SRTV_ITER"):
       listSRTV_ITER.append(inImagesIter)
    
  if len(listSRTV_ITER)>0:
    ctx.field("NumberIteration").setValue(len(listSRTV_ITER))
    print("iter Number : %i"%ctx.field("NumberIteration").value)
    lastIter = sort_human(listSRTV_ITER)[-1]  
    ctx.field("itkImageFileReader.fileName").setStringValue(inImages[lastIter])
    convertToDicom(lastIter)
    
    #and now we run the next steps ?
  
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


def stopSuperResolution():
  print("stop SuperResolution")
  ctx.field("StopProcess").setBoolValue(True)

def showHelp():
  print("showHelp")
  if not ctx.field("FromFrontier").value:
    import webbrowser
    print(webbrowser.browser)
    print(MLABFileManager.exists(ctx.expandFilename("$(MLAB_mevisFetalMRI_MRUser)/Documentation/Publish/ModuleReference/TotalVariationInterface.html")))
    webbrowser.open_new(ctx.expandFilename("$(MLAB_mevisFetalMRI_MRUser)/Documentation/Publish/ModuleReference/TotalVariationInterface.html"))

  else:
    global _frontier
    _frontier = ctx.module("parent:FrontierSyngoInterface").object()
    url = ctx.expandFilename("$(MLAB_mevisFetalMRI_MRUser)/Documentation/Publish/ModuleReference/TotalVariationInterface.html")
    _frontier._syngoVia.call("FE.AppHosting.ShowUrl", url)


def sort_human(l):
  convert = lambda text: float(text) if text.isdigit() else text
  alphanum = lambda key: [ convert(c) for c in re.split('([-+]?[0-9]*\.?[0-9]*)', key) ]
  l.sort( key=alphanum )
  return l    
  