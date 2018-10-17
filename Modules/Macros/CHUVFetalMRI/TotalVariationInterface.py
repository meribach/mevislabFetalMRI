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
  ctx.field("Lambda").setValue(0.1)
  ctx.field("DeltaT").setValue(0.1)
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
  
  updateTotalVariationInterface()
  

  
  
def buttonPressedAxial():
  print("clicked axial")
  
def buttonPressedCoronal():
  print("clicked coronal")
  
def buttonPressedSagittal():
  print("clicked sagittal")
  
  
def RunSuperResolution():
  print("run SuperResolution")
  inImages = ctx.field("inImageInfos").object()
  
  inTransforms = ""
  inputFiles = ""
  maskFiles = ""
  ImagesToDoBackgroundTasks = []
  
  for imageIter in inImages["UsedForSDI"]:
    if "Image" in imageIter:
        if inTransforms!="":
          inTransforms=inTransforms+"--"
          inputFiles=inputFiles+"--"
          maskFiles=maskFiles+"--"
          
        try:
          inTransforms=inTransforms+inImages[imageIter]["Transform"]
        except Exception as err:
          print("error, transform has not been estimated for imageIter, reRun SDI including this image")
        inputFiles=inputFiles+inImages[imageIter]["BCorr"]
        maskFiles=maskFiles+inImages[imageIter]["MaskReOriented"]
        
  iterNumber = ctx.field("NumberIteration").value
  ctx.field("mialTVSuperResolution.outputFile").setStringValue(os.path.join(os.path.dirname(inImages["Image0"]["file"]),"SRTV_ITER%i.nii.gz"%iterNumber))
  ctx.field("mialTVSuperResolution.refFile").setStringValue(inImages["SDI_ITER1"])
  ctx.field("mialTVSuperResolution.maskFiles").setStringValue(maskFiles)
  ctx.field("mialTVSuperResolution.inputFiles").setStringValue(inputFiles)
  ctx.field("mialTVSuperResolution.transformFilesInput").setStringValue(inTransforms)
  
  ctx.field("mialTVSuperResolution.startTask").touch()
  
  
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
  #updateTotalVariationInterface()
  #convertToDicom("SRTV_ITER%i"%ctx.field("NumberIteration").value)
  
  #convert to dicom
  runRefineMask()
  
  
  #a mettre plus loin
  if ctx.field("NumberIteration").value < ctx.field("NbIterationToDo").value:
    print("continue")
  
  
def runRefineMask():
  
  inImages = ctx.field("inImageInfos").object()
  iterNumber = ctx.field("NumberIteration").value
  
  inTransforms = ""
  inputFiles = ""
  maskFiles = ""
  outputLRFiles = ""
  
  for imageIter in inImages["UsedForSDI"]:
    if inTransforms!="":
       inTransforms=inTransforms+"--"
       inputFiles=inputFiles+"--"
       maskFiles=maskFiles+"--"
       outputLRFiles =outputLRFiles+"--"

    inTransforms=inTransforms+inImages[imageIter]["Transform"]
    inputFiles=inputFiles+inImages[imageIter]["BCorr"]
    maskFiles=maskFiles+inImages[imageIter]["MaskReOriented"]
    outputLRFiles =outputLRFiles+inImages[imageIter]["MaskReOriented"].replace(".nii","LR_ITER_%i.nii"%iterNumber)
    
  ctx.field("mialRefineMask.inputFiles").setStringValue(inputFiles)
  ctx.field("mialRefineMask.transformIn").setStringValue(inTransforms)
  ctx.field("mialRefineMask.maskFiles").setStringValue(maskFiles)
  ctx.field("mialRefineMask.outputHRFile").setStringValue(os.path.join(os.path.dirname(inImages["SDI_ITER%i"%iterNumber]),"brainmaskHR_ITER_%i.nii.gz"%iterNumber))
  ctx.field("mialRefineMask.outputLRFiles").setStringValue(outputLRFiles)
  ctx.field("mialRefineMask.referenceFile").setStringValue(inImages["SDI_ITER%i"%iterNumber])
  
  ctx.field("mialRefineMask.startTask").touch()


def insertRefinedMask():
 
 if not ctx.field("mialRefineMask.outputSucceed").value:
   return
  
 inImages = ctx.field("inImageInfos").object()
 iterNumber = ctx.field("NumberIteration").value
 
 for imageIter in inImages["UsedForSDI"]:
   inImages[imageIter].update({"BrainMaskLR_Iter%i"%iterNumber:inImages[imageIter]["MaskReOriented"].replace(".nii","LR_ITER_%i.nii"%iterNumber)})  
   
 inImages.update({"BrainMaskHR_Iter%i"%iterNumber:os.path.join(os.path.dirname(inImages["SDI_ITER%i"%iterNumber]),"brainmaskHR_ITER_%i.nii.gz"%iterNumber)})
 
 ctx.field("inImageInfos").setObject(inImages)
 ctx.field("outImagesInfosStep3").setObject(inImages)
 print("should run BiasField")
 runN4BiasFieldCorrection()
 
def runN4BiasFieldCorrection():
  
  
  inImages = ctx.field("inImageInfos").object()
  iterNumber = ctx.field("NumberIteration").value
  
  ctx.field("mialN4BiasField.inputFile").setStringValue(inImages["SRTV_ITER%i"%iterNumber])
  ctx.field("mialN4BiasField.maskFile").setStringValue(inImages["BrainMaskHR_Iter%i"%iterNumber])
  ctx.field("mialN4BiasField.outputFile").setStringValue(inImages["SRTV_ITER%i"%iterNumber].replace(".nii","_gbcorr.nii"))
  ctx.field("mialN4BiasField.outputBiasField").setStringValue(inImages["SRTV_ITER%i"%iterNumber].replace(".nii","_gbcorrfield.nii"))
  print("should run BiasField2")
  ctx.field("mialN4BiasField.startTask").touch()
  

def insertN4BiasFieldCorrectedHRImage():
  
  if not ctx.field("mialN4BiasField.outputSucceed").value:
    return
  
  inImages = ctx.field("inImageInfos").object()
  iterNumber = ctx.field("NumberIteration").value
  
  inImages.update({"SRTV_ITER%i_BCorr"%iterNumber:inImages["SRTV_ITER%i"%iterNumber].replace(".nii","_gbcorr.nii")})
  inImages.update({"SRTV_ITER%i_BCorrField"%iterNumber:inImages["SRTV_ITER%i"%iterNumber].replace(".nii","_gbcorrfield.nii")})
  
  ctx.field("inImageInfos").setObject(inImages)
  ctx.field("outImagesInfosStep3").setObject(inImages)
  
  #convert to dicom ?
  ##we convert them to dicom as well:
  print("convert SDI To Dicom") 
  ctx.field("NiftiToDicomFetalMRI.itkImageFileReader.fileName").setStringValue(inImages["SRTV_ITER%i_BCorr"%iterNumber])
  ctx.field("NiftiToDicomFetalMRI.DicomTagModify.tagValue0").setValue("SRTV_ITER%i"%iterNumber)
   
  if ctx.field("FromFrontier").value:
    #we use dicom tool from testinstall, dicomSend
    print("via Frontier")
    ctx.field("NiftiToDicomFetalMRI.DicomTagModify.tagValue1").setValue(inImages["Image0"]["StudyDescription"])
    ctx.field("NiftiToDicomFetalMRI.DicomTagModify.tagValue2").setValue(inImages["Image0"]["PatientName"])
    ctx.field("NiftiToDicomFetalMRI.DicomTagModify.tagValue3").setValue(inImages["Image0"]["PatientID"])
    ctx.field("NiftiToDicomFetalMRI.DicomTagModify.apply").touch()
    originalTree = ctx.field("NiftiToDicomFetalMRI.SetDicomTreeOnImage.input0").getDicomTree()
    mutableTree = originalTree.createDerivedTree()
    _frontier = ctx.module("parent:FrontierSyngoInterface").object()
    ctx.field("parent:DicomExport.exportBaseDir").setStringValue(_frontier.getOutgoingDicomDirectory())
    DicomToolToUse = ctx.module("parent:DicomExport") #ctx.module("DicomTool") #
    print(DicomToolToUse.field("exportBaseDir").value)
    listUID = [inImages[imageIter]["SeriesInstanceUID"] for imageIter in inImages["UsedForSDI"]]
    mutableTree.setPrivateTag(0x07a1, ctx.field("NiftiToDicomFetalMRI.NamePrivateTage").value, 0x43, listUID , "UI")
    mutableTree.setPrivateTag(0x07a1, ctx.field("NiftiToDicomFetalMRI.NamePrivateTage").value, 0x42, iterNumber , "SS")
    mutableTree.setPrivateTag(0x07a1, ctx.field("NiftiToDicomFetalMRI.NamePrivateTage").value, 0x44, ctx.field("mialTVSuperResolution.Lambda").value, "FD")
    mutableTree.setPrivateTag(0x07a1, ctx.field("NiftiToDicomFetalMRI.NamePrivateTage").value, 0x45, ctx.field("mialTVSuperResolution.DeltaT").value , "FD")
    #ctx.connectField("parent:DicomExport.inImage","DicomTagModify.output0")
    
  else:
    #we use dicom tool from TotalVariationInterface, dicomSave
    print("not via Frontier")
    ctx.field("NiftiToDicomFetalMRI.DicomTagModify.apply").touch()
    originalTree = ctx.field("NiftiToDicomFetalMRI.SetDicomTreeOnImage.input0").getDicomTree()
    mutableTree = originalTree.createDerivedTree()
    mutableTree.setPrivateTag(0x07a1, "pdeman", 0x43, inImages["UsedForSDI"] , "LO")
    mutableTree.setPrivateTag(0x07a1, "pdeman", 0x42, iterNumber , "SS")
    #transfoInfo = [open(inImages[imageIter]["Transform"],"r").read() for imageIter in inImages["UsedForSDI"]]
    #mutableTree.setPrivateTag(0x07a1, "pdeman", 0x44, transfoInfo, "UT")
    mutableTree.setPrivateTag(0x07a1, "pdeman", 0x44, ctx.field("mialTVSuperResolution.Lambda").value, "FD")
    mutableTree.setPrivateTag(0x07a1, "pdeman", 0x45, ctx.field("mialTVSuperResolution.DeltaT").value , "FD")    
    DicomToolToUse = ctx.module("DicomTool")
    ctx.field("DicomTool.exportBaseDir").setStringValue(os.path.join(os.path.dirname(inImages["Image0"]["file"]),"Results"))
    
  
  ctx.field("NiftiToDicomFetalMRI.SetDicomTreeOnImage.inDicomTree").setObject(mutableTree)
  
  if ctx.field("FromFrontier").value:
    ctx.connectField("parent:DicomExport.inImage","NiftiToDicomFetalMRI.SetDicomTreeOnImage.output0")
  
  DicomToolToUse.field("exportNameTemplate").setStringValue("$S/"+"SRTV_ITER%i_BCorr"%iterNumber+"$T.dcm")
  DicomToolToUse.field("saveSlices").touch()
  
  print("SRTV DICOM Saved")


#def convertToDicom(WhatToConvert):
#
#  print("nifti to dicom conversion")
#  inImages = ctx.field("inImageInfos").object()
#  #(0028,1052) RescaleIntercept:  0
#  #(0028,1053) RescaleSlope:  100
#  #(0028,1054) RescaleType:  US
#  # out-folder
#  
#  ctx.field("itkImageFileReader1.fileName").setStringValue(WhatToConvert)
#  
#  if ctx.field("FromFrontier").value:
#    #we use dicom tool from testinstall, dicomSend
#    print("via Frontier")
#    _frontier = ctx.module("parent:FrontierSyngoInterface").object()
#    ctx.field("parent:DicomExport.exportBaseDir").setStringValue(_frontier.getOutgoingDicomDirectory())
#    DicomToolToUse = ctx.module("parent:DicomExport")
#    print(ctx.field("parent:DicomExport.exportBaseDir").value)
#  else:
#    #we use dicom tool from TotalVariationInterface, dicomSave
#    print("not via Frontier")
#    DicomToolToUse = ctx.module("DicomTool")
#    ctx.field("DicomTool.exportBaseDir").setStringValue(os.path.join(os.path.dirname(inImages["Image0"]["file"]),"Results"))
#  
#  ctx.field("DicomTagModify.tagValue3").setValue(WhatToConvert)
#  ctx.field("DicomTagModify.apply").touch()
#  DicomToolToUse.field("exportNameTemplate").setStringValue("$S/"+WhatToConvert+"$T.dcm")
#  DicomToolToUse.field("saveSlices").touch()

def updateTotalVariationInterface():
  print("Here")
  inImages = ctx.field("inImageInfos").object()
  listSRTV_ITER=[]
  if inImages is not None:
    for inImagesIter in inImages.keys():
     if inImagesIter.startswith("SRTV_ITER") & inImagesIter.endswith("BCorr"):
       listSRTV_ITER.append(inImagesIter)
    
  if len(listSRTV_ITER)>0:
    ctx.field("NumberIteration").setValue(len(listSRTV_ITER))
    print("iter Number : %i"%ctx.field("NumberIteration").value)
    lastIter = sort_human(listSRTV_ITER)[-1]  
    ctx.field("itkImageFileReader.fileName").setStringValue(inImages[lastIter])
    ctx.field("LabelViewerMode.text").setStringValue(os.path.basename(inImages[lastIter]))
    ctx.field("NumberIteration").setMaxValue(len(listSRTV_ITER)+1)
    #convertToDicom(lastIter)
    
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


def updateBackgroundTaskRunningField():
  print("update backgroundTask")
  ctx.field("BackgroundTaskRunning").setBoolValue(ctx.field("mialTVSuperResolution.inProgress").value | ctx.field("mialRefineMask.inProgress").value | ctx.field("mialsrtkMaskImage.inProgress").value | ctx.field("mialN4BiasField.inProgress").value)

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
  