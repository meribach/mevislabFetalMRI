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
   
  except Exception as err:
   numImage = ctx.field("NumberImages").value
   GetInfoInput=False
  
  print(numImage)
  if numImage<=6:
    for i in range(numImage):
      if GetInfoInput:
        if "UsedFromStart" in inImages.keys():
          print("checked if previous step done")
          if listImage[i] in inImages["UsedFromStart"]:
            if "UsedForSDI" in inImages.keys():
              if listImage[i] in inImages["UsedForSDI"]:
                checkBoxDefinition = "CheckBox {name = checkImage%i title = %s checked = True}"%(i,listImage[i])
              else:
                checkBoxDefinition = "CheckBox {name = checkImage%i title = %s checked = False}"%(i,listImage[i])
            else:
              checkBoxDefinition = "CheckBox {name = checkImage%i title = %s checked = True}"%(i,listImage[i])
          else:
             checkBoxDefinition = "CheckBox {name = checkImage%i title = %s checked = False enabled = False}"%(i,listImage[i])
        else:
          checkBoxDefinition = "CheckBox {name = checkImage%i title = %s checked = True}"%(i,listImage[i])
      else:
        checkBoxDefinition = "CheckBox {name = checkImage%i title = \'Image%i\' checked = True}"%(i,i)
      
      LabelOrder = """ Label Order:%i {name = Label%s}"""%(i,listImage[i])
      ButtonUpDefinition = """Button {name = moveUp%s image = $(MLAB_mevisFetalMRI_MRUser)/Modules/Graphics/up-arrow-symbol-icon-68695.png command = "py: changeOrder(\'%s\',+1)"}"""%(listImage[i],listImage[i])
      ButtonDownDefinition = """Button {name = moveUp%s image = $(MLAB_mevisFetalMRI_MRUser)/Modules/Graphics/down-arrow-symbol-icon-68695.png command = "py: changeOrder(\'%s\',-1)"}"""%(listImage[i],listImage[i])
      mdlToSet += """Horizontal { name = \"horizontal%i\"  """%i + checkBoxDefinition +LabelOrder+ ButtonUpDefinition + ButtonDownDefinition + """ Execute = "py: getHorizontalControl(\'Image%i\',\'horizontal%i\')" } """%(i,i)
  else:
    for i in [0,2,4]:
      if GetInfoInput:
        if "UsedFromStart" in inImages.keys():
          print("checked if previous step done")
          if listImage[i] in inImages["UsedFromStart"]:
            if "UsedForSDI" in inImages.keys():
              if listImage[i] in inImages["UsedForSDI"]:
                 checkBoxDefinition = "CheckBox {name = checkImage%i title = %s checked = True}"%(i,listImage[i])
              else:
                checkBoxDefinition = "CheckBox {name = checkImage%i title = %s checked = False}"%(i,listImage[i])
            else:
               checkBoxDefinition = "CheckBox {name = checkImage%i title = %s checked = True}"%(i,listImage[i])
          else:
           checkBoxDefinition = "CheckBox {name = checkImage%i title = %s checked = False enabled = False}"%(i,listImage[i])
          if listImage[i+1] in inImages["UsedFromStart"]:
            if "UsedForSDI" in inImages.keys():
              if listImage[i+1] in inImages["UsedForSDI"]:       
                 checkBoxDefinition2 = "CheckBox {name = checkImage%i title = %s checked = True}"%(i+1,listImage[i+1])
              else:
                 checkBoxDefinition2 = "CheckBox {name = checkImage%i title = %s checked = False}"%(i+1,listImage[i+1])
            else:
              checkBoxDefinition2 = "CheckBox {name = checkImage%i title = %s checked = True}"%(i+1,listImage[i+1])
          else:
            checkBoxDefinition2 = "CheckBox {name = checkImage%i title = %s checked = False enabled = False}"%(i,listImage[i])
            
        else:
          checkBoxDefinition = "CheckBox {name = checkImage%i title = %s checked = True}"%(i,listImage[i])
          checkBoxDefinition2 = "CheckBox {name = checkImage%i title = %s checked = True}"%(i+1,listImage[i+1])
      else:
        checkBoxDefinition = "CheckBox {name = checkImage%i title = \'Image%i\' checked = True}"%(i,i)
        checkBoxDefinition2 = "CheckBox {name = checkImage%i title = \'Image%i\' checked = True}"%(i+1,i+1)
        
      Button1UpDefinition = """Button {name = moveUp%s image = $(MLAB_mevisFetalMRI_MRUser)/Modules/Graphics/up-arrow-symbol-icon-68695.png command = "py: changeOrder(\'%s\',+1)"}"""%(listImage[i],listImage[i])
      Button1DownDefinition = """Button {name = moveUp%s image = $(MLAB_mevisFetalMRI_MRUser)/Modules/Graphics/down-arrow-symbol-icon-68695.png command = "py: changeOrder(\'%s\',-1)"}"""%(listImage[i],listImage[i])
      Button2UpDefinition = """Button {name = moveUp%s image = $(MLAB_mevisFetalMRI_MRUser)/Modules/Graphics/up-arrow-symbol-icon-68695.png command = "py: changeOrder(\'%s\',+1)"}"""%(listImage[i+1],listImage[i+1])
      Button2DownDefinition = """Button {name = moveUp%s image = $(MLAB_mevisFetalMRI_MRUser)/Modules/Graphics/down-arrow-symbol-icon-68695.png command = "py: changeOrder(\'%s\',-1)"}"""%(listImage[i+1],listImage[i+1])
      
      mdlToSet += """Horizontal { name = \"horizontal%i\"  """%i + checkBoxDefinition + Button1UpDefinition + Button1DownDefinition  + checkBoxDefinition2 + Button2UpDefinition + Button2DownDefinition + """ Execute = "py: getHorizontalControl(\'Image%i\',\'horizontal%i\')" } """%(i,i)
      
    listToFinish= range(6,numImage,2)
    for i in listToFinish:
      if GetInfoInput:
        if "UsedFromStart" in inImages.keys():
          print("checked if previous step done")
          if listImage[i] in inImages["UsedFromStart"]:
            if "UsedForSDI" in inImages.keys():
              if listImage[i] in inImages["UsedForSDI"]:
                  checkBoxDefinition = "CheckBox {name = checkImage%i title = %s checked = True}"%(i,listImage[i])
              else:
                  checkBoxDefinition = "CheckBox {name = checkImage%i title = %s checked = False}"%(i,listImage[i])
            else:
              checkBoxDefinition = "CheckBox {name = checkImage%i title = %s checked = True}"%(i,listImage[i])
          else:
            checkBoxDefinition = "CheckBox {name = checkImage%i title = %s checked = False enabled = False}"%(i,listImage[i])
        else:
          checkBoxDefinition = "CheckBox {name = checkImage%i title = %s checked = True}"%(i,listImage[i])
          
        if (i+1)<numImage:
          checkBoxDefinition2 = "CheckBox {name = checkImage%i title = %s checked = True}"%(i+1,listImage[i+1])
        else:
          checkBoxDefinition2 = ""
      else:
        checkBoxDefinition = "CheckBox {name = checkImage%i title = \'Image%i\' checked = True}"%(i,i)
        
      Button1UpDefinition = """Button {name = moveUp%s image = $(MLAB_mevisFetalMRI_MRUser)/Modules/Graphics/up-arrow-symbol-icon-68695.png command = "py: changeOrder(\'%s\',+1)"}"""%(listImage[i],listImage[i])
      Button1DownDefinition = """Button {name = moveUp%s image = $(MLAB_mevisFetalMRI_MRUser)/Modules/Graphics/down-arrow-symbol-icon-68695.png command = "py: changeOrder(\'%s\',-1)"}"""%(listImage[i],listImage[i])
      
      if (i+1)<numImage:
        if GetInfoInput:
          if "UsedFromStart" in inImages.keys():
            print("checked if previous step done")
            if listImage[i+1] in inImages["UsedFromStart"]:
              if "UsedForSDI" in inImages.keys():
                if listImage[i+1] in inImages["UsedForSDI"]:
                   checkBoxDefinition2 = "CheckBox {name = checkImage%i title = \'Image%i\' checked = True}"%(i+1,i+1)
                else:
                   checkBoxDefinition2 = "CheckBox {name = checkImage%i title = \'Image%i\' checked = False}"%(i+1,i+1)
              else:
                checkBoxDefinition2 = "CheckBox {name = checkImage%i title = \'Image%i\' checked = True}"%(i+1,i+1)
            else:
              checkBoxDefinition2 = "CheckBox {name = checkImage%i title = \'Image%i\' checked = False enabled = False}"%(i+1,i+1)    
          else:
            checkBoxDefinition2 = "CheckBox {name = checkImage%i title = \'Image%i\' checked = True}"%(i+1,i+1)
        
        Button2UpDefinition = """Button {name = moveUp%s image = $(MLAB_mevisFetalMRI_MRUser)/Modules/Graphics/up-arrow-symbol-icon-68695.png command = "py: changeOrder(\'%s\',+1)"}"""%(listImage[i+1],listImage[i+1])
        Button2DownDefinition = """Button {name = moveUp%s image = $(MLAB_mevisFetalMRI_MRUser)/Modules/Graphics/down-arrow-symbol-icon-68695.png command = "py: changeOrder(\'%s\',-1)"}"""%(listImage[i+1],listImage[i+1])

      else:
        checkBoxDefinition2 = ""
        Button2UpDefinition = ""
        Button2DownDefinition = ""
      
      mdlToSet += """Horizontal {name = \"horizontal%i\"  """%i + checkBoxDefinition + Button1UpDefinition + Button1DownDefinition  + checkBoxDefinition2 + Button2UpDefinition + Button2DownDefinition + """ Execute = "py: getHorizontalControl(\'Image%i\',\'horizontal%i\')" } """%(i,i)   
        
        
  g_sceneSDI.addMDL("Vertical {" + mdlToSet + "}")
  
  
  updateSDI()


def changeOrder(Image,Orientation):
  print("change order")
  inImages = ctx.field("inImageInfos").object()
  previousValue=int(g_HorizontalControl[Image].control("Label%s"%Image).title().split(":")[-1])
  print(previousValue)
  if int( g_HorizontalControl[Image].control("Label%s"%Image).title().split(":")[-1])+Orientation < ctx.field("NumberImages").value and int( g_HorizontalControl[Image].control("Label%s"%Image).title().split(":")[-1])+Orientation >=0:
    g_HorizontalControl[Image].control("Label%s"%Image).setTitle("Order:%i"%(int( g_HorizontalControl[Image].control("Label%s"%Image).title().split(":")[-1])+Orientation))
  
  for imageIter in inImages:
    if "Image" in imageIter:
      if imageIter != Image:
        if int(g_HorizontalControl[imageIter].control("Label%s"%imageIter).title().split(":")[-1])==int(g_HorizontalControl[Image].control("Label%s"%Image).title().split(":")[-1]):
          g_HorizontalControl[imageIter].control("Label%s"%imageIter).setTitle("Order:%i"%(previousValue))
      
def OutlierRejectionStatusChanged():
  
  print("et pataplif")
  
  
def buttonPressedAxial():
  print("clicked axial")
  
def buttonPressedCoronal():
  print("clicked coronal")
  
def buttonPressedSagittal():
  print("clicked sagittal")
  

def RunSuperResolution():
  
  global ImagesToDoBackgroundTasks
  
  print("run SuperVariation")
  inImages = ctx.field("inImageInfos").object()
  
  numIm = ctx.field("NumberImages").value
  inTransforms = ""
  inputFiles = ""
  maskFiles = ""
  ImagesToDoBackgroundTasks = []
  
  for imageIter in inImages:
    if "Image" in imageIter:
      if g_HorizontalControl[imageIter].control("check%s"%imageIter).isChecked():
        ImagesToDoBackgroundTasks.append(imageIter)
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
        
  ctx.field("mialTVSuperResolution.outputFile").setStringValue(os.path.join(os.path.dirname(inImages["Image0"]["file"]),"SRTV_ITER1.nii.gz"))
  ctx.field("mialTVSuperResolution.refFile").setStringValue(inImages["SDI_ITER1"])
  ctx.field("mialTVSuperResolution.maskFiles").setStringValue(maskFiles)
  ctx.field("mialTVSuperResolution.inputFiles").setStringValue(inputFiles)
  ctx.field("mialTVSuperResolution.transformFilesInput").setStringValue(inTransforms)
  
  ctx.field("mialTVSuperResolution.startTask").touch()
  
  
def insertTVSuperResolution():
  
  if not ctx.field("mialTVSuperResolution.outputSucceed").value:
    return
  
  inImages = ctx.field("inImageInfos").object()
  
  if inImages is None:
    return
  
  
  inImages.update({"SRTV_ITER1":os.path.join(os.path.dirname(inImages["Image0"]["file"]),"SRTV_ITER1.nii.gz")})
  
  ctx.field("inImageInfos").setObject(inImages)
  ctx.field("outImagesInfosStep2").setObject(inImages)
  
  runRefineMask()

def runRefineMask():
  
  global ImagesToDoBackgroundTasks
  
  inImages = ctx.field("inImageInfos").object()
  
  inTransforms = ""
  inputFiles = ""
  maskFiles = ""
  outputLRFiles = ""
  
  for imageIter in ImagesToDoBackgroundTasks:
    if inTransforms!="":
       inTransforms=inTransforms+"--"
       inputFiles=inputFiles+"--"
       maskFiles=maskFiles+"--"
       outputLRFiles =outputLRFiles+"--"

    inTransforms=inTransforms+inImages[imageIter]["Transform"]
    inputFiles=inputFiles+inImages[imageIter]["BCorr"]
    maskFiles=maskFiles+inImages[imageIter]["MaskReOriented"]
    outputLRFiles =outputLRFiles+inImages[imageIter]["MaskReOriented"].replace(".nii.gz","LR_ITER_1.nii.gz")
    
  ctx.field("mialRefineMask.inputFiles").setStringValue(inputFiles)
  ctx.field("mialRefineMask.transformIn").setStringValue(inTransforms)
  ctx.field("mialRefineMask.maskFiles").setStringValue(maskFiles)
  ctx.field("mialRefineMask.outputHRFile").setStringValue(os.path.join(os.path.dirname(inImages["SDI_ITER1"]),"brainmaskHR_ITER_1.nii.gz"))
  ctx.field("mialRefineMask.outputLRFiles").setStringValue(outputLRFiles)
  ctx.field("mialRefineMask.referenceFile").setStringValue(inImages["SDI_ITER1"])
  
  ctx.field("mialRefineMask.startTask").touch()
    
def insertRefinedMask():
  
 global ImagesToDoBackgroundTasks
 if not ctx.field("mialRefineMask.outputSucceed").value:
   return
  
 inImages = ctx.field("inImageInfos").object()
 
 for imageIter in ImagesToDoBackgroundTasks:
   inImages[imageIter].update({["BrainMaskLR_Iter1"]:inImages[imageIter]["MaskReOriented"].replace(".nii.gz","LR_ITER_1.nii.gz")})  
   
 inImages.update({["BrainMaskHR_Iter1"]:os.path.join(os.path.dirname(inImages["SDI_ITER1"]),"brainmaskHR_ITER_1.nii.gz")})
 
 ctx.field("inImageInfos").setObject(inImages)
 ctx.field("outImagesInfosStep2").setObject(inImages)
 print("should run BiasField")
 runN4BiasFieldCorrection()
 
def runN4BiasFieldCorrection():
  
  global ImagesToDoBackgroundTasks
  
  inImages = ctx.field("inImageInfos").object()
  
  ctx.field("mialN4BiasField.inputFile").setStringValue(inImages["SRTV_ITER1"])
  ctx.field("mialN4BiasField.maskFile").setStringValue(inImages["BrainMaskHR_Iter1"])
  ctx.field("mialN4BiasField.outputFile").setStringValue(inImages["SRTV_ITER1"].replace(".nii.gz","_gbcorr.nii.gz"))
  ctx.field("mialN4BiasField.outputBiasField").setStringValue(inImages["SRTV_ITER1"].replace(".nii.gz","_gbcorrfield.nii.gz"))
  print("should run BiasField2")
  ctx.field("mialN4BiasField.startTask").touch()
  
  
def insertN4BiasFieldCorrectedHRImage():
  
  if not ctx.field("mialN4BiasField.outputSucceed").value:
    return
  
  inImages = ctx.field("inImageInfos").object()
  
  
  inImages.update({"SRTV_ITER1_BCorr":inImages["SRTV_ITER1"].replace(".nii.gz","_gbcorr.nii.gz")})
  inImages.update({"SRTV_ITER1_BCorrField":inImages["SRTV_ITER1"].replace(".nii.gz","_gbcorrfield.nii.gz")})
  
  ctx.field("inImageInfos").setObject(inImages)
  ctx.field("outImagesInfosStep2").setObject(inImages)
  
  #convert to dicom ?


#def updateRefImageReconstruction():
#
#  ctx.field("mialImageReconstruction.ImageBaseOfRecon").setIntValue(ctx.field("RefImageFiled").value)

def runMaskImage():
  print("run MaskImage")

def insertMaskImage():
  print("insert MaskImage")

def ReRunImageReconstruction():
  
  global ImagesToDoBackgroundTasks
  
  print("reRunImageReconstruction")
  inImages = ctx.field("inImageInfos").object()
  
  ImagesToDoBackgroundTasks = []
  
  numIm = ctx.field("NumberImages").value
  outTransform = ""
  inputFiles = ""
  maskFiles = ""
  
  #get the order
  orderList = {};
  for imageIter in inImages:
    if "Image" in imageIter:
      orderList.update({int(g_HorizontalControl[imageIter].control("Label%s"%imageIter).title().split(":")[-1]):imageIter})
  
  sorted_orderList = sorted(orderList.items(),  key=lambda kv: kv[0])
  #test = sort_human(orderList)
  for imageIter in inImages:
    if "Image" in imageIter:
      if g_HorizontalControl[imageIter].control("check%s"%imageIter).isChecked():
        ImagesToDoBackgroundTasks.append(imageIter)
        if outTransform!="":
          outTransform=outTransform+"--"
          inputFiles=inputFiles+"--"
          maskFiles=maskFiles+"--"
      
        outTransform = outTransform +inImages[imageIter]["ImReOriented"].replace(".nii.gz","_transform_%iV_1.txt"%numIm)
        inputFiles = inputFiles + inImages[imageIter]["NLMBCorr"]
        maskFiles = maskFiles + inImages[imageIter]["MaskReOriented"]
    
  inImages.update({"UsedForSDI":ImagesToDoBackgroundTasks})
  
  ctx.field("mialImageReconstruction.inputFiles").setStringValue(inputFiles)
  ctx.field("mialImageReconstruction.maskFiles").setStringValue(maskFiles)
  ctx.field("mialImageReconstruction.transformoutFiles").setStringValue(outTransform)
  ctx.field("mialImageReconstruction.outputFile").setStringValue(os.path.join(os.path.dirname(inImages["Image0"]["file"]),"SDI_ITER1.nii.gz"))
  ctx.field("mialImageReconstruction.startTask").touch()
 

def insertImageReconstruction():
  
  if not ctx.field("mialImageReconstruction.outputSucceed").value:
    return
  
  inImages = ctx.field("inImageInfos").object()
  numIm = ctx.field("NumberImages").value
  print("insertImageReconstruction")
  inImages.update({"SDI_ITER1":os.path.join(os.path.dirname(inImages["Image0"]["file"]),"SDI_ITER1.nii.gz")})
  
  global ImagesToDoBackgroundTasks
  
  for imageIter in ImagesToDoBackgroundTasks:   
    inImages[imageIter].update({"Transform":inImages[imageIter]["ImReOriented"].replace(".nii.gz","_transform_%iV_1.txt"%numIm)})
  
  ctx.field("inImageInfos").setObject(inImages)
  ctx.field("outImagesInfosStep2").setObject(inImages)
  
  updateSDI()
  MLAB.processEvents()

def updateSDI():
  
  inImageOrientation = ctx.field("ImageOrientationSucceed").object()
  inImages = ctx.field("inImageInfos").object()
  if inImages is not None:
    listSDI_ITER=[]
    for inImagesIter in inImages.keys():
      if inImagesIter.startswith("SDI_ITER"):
        listSDI_ITER.append(inImagesIter)
        
    if len(listSDI_ITER)>0:
      ctx.field("NumberIteration").setValue(len(listSDI_ITER))
      print("iter Number : %i"%ctx.field("NumberIteration").value)
      lastIter = sort_human(listSDI_ITER)[-1]  
      ctx.field("itkImageFileReader.fileName").setStringValue(inImages[lastIter])
      ctx.field("NumberIteration").setMaxValue(len(listSDI_ITER)+1)
      #  convertToDicom(lastIter)  
  
  #should check which image were taken
  print("test here")
  if len(g_HorizontalControl):
    for iterImage in g_HorizontalControl.keys():
      if "UsedForSDI" in inImages.keys():
         if iterImage in inImages["UsedForSDI"]:
           g_HorizontalControl[iterImage].control("check%s"%iterImage).setChecked(True)
         else:
           g_HorizontalControl[iterImage].control("check%s"%iterImage).setChecked(False)
      
  
def updateImage():
  print("enter updateImage")
  ctx.field("CreateBoundingVolumeAxial.add").touch()
  ctx.field("ReformatAxial.apply").touch()
  ctx.field("BoundingBoxAxial.update").touch()
  ctx.field("CreateBoundingVolumeSagittal.add").touch()
  ctx.field("ReformatSagittal.apply").touch()
  ctx.field("BoundingBoxSagittal.update").touch()
  ctx.field("CreateBoundingVolumeCoronal.add").touch()
  ctx.field("ReformatCoronal.apply").touch()
  ctx.field("BoundingBoxCoronal.update").touch()
  ctx.field("OrthoView2DAxial.worldPosition").setValue([0,0,0])

def showHelp():
  print("showHelp")
  if not ctx.field("FromFrontier").value:
    import webbrowser
    print(webbrowser.browser)
    print(MLABFileManager.exists(ctx.expandFilename("$(MLAB_CHUV_FetalMRI)/Documentation/Publish/ModuleReference/SDIVerificationInterface.html")))
    webbrowser.open_new(ctx.expandFilename("$(MLAB_CHUV_FetalMRI)/Documentation/Publish/ModuleReference/SDIVerificationInterface.html"))

  else:
    global _frontier
    _frontier = ctx.module("parent:FrontierSyngoInterface").object()
    url = ctx.expandFilename("$(MLAB_mevisFetalMRI_MRUser)/Documentation/Publish/ModuleReference/SDIVerificationInterface.html")
    _frontier._syngoVia.call("FE.AppHosting.ShowUrl", url)
    
def updateBackgroundTaskRunningField():
  print("update backgroundTask")
  ctx.field("BackgroundTaskRunning").setBoolValue(ctx.field("mialImageReconstruction.inProgress").value | ctx.field("mialTVSuperResolution.inProgress").value | ctx.field("mialRefineMask.inProgress").value | ctx.field("mialsrtkMaskImage.inProgress").value | ctx.field("mialN4BiasField.inProgress").value)

def getHorizontalControl(image,horizon):
  
  #print("get %s control"%horizon)
  global g_HorizontalControl
  g_HorizontalControl.update({image:ctx.control(horizon)})

def sort_human(l):
  convert = lambda text: float(text) if text.isdigit() else text
  alphanum = lambda key: [ convert(c) for c in re.split('([-+]?[0-9]*\.?[0-9]*)', key) ]
  l.sort( key=alphanum )
  return l  