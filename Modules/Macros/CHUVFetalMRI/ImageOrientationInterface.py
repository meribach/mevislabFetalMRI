# ----------------------------------------------------------------------------

# 
#  \file    ImageOrientationInterface.py
#  \author  FetalMRI
#  \date    2018-01-16
#
#  

# ----------------------------------------------------------------------------

from mevis import *
from mevis import MLAB, MLABFileManager, MLABFileDialog
from PythonQt import QtGui
import numpy
import math
import os
import time

activePositioning = None
activeMasking = None
currentPositioning = None
currentImage = None
g_layoutImageOrientation = None 
g_sceneImageOrientation = None 
g_ImageOrientationGraphicsView = None
g_HorizontalControl = {}
activeShowPosition = 1
ToggleLoadPreviousData = 0

ctx.field("Switch.currentInput").setValue(0)
ctx.field("SoInteractionMapping1.ignoreOtherCommandActions").setBoolValue(False)


def setSceneBackgroundColor(scene):
  scene.setBackgroundBrush(QtGui.QColor(0,0,0,255))

def initImageOrientationGraphicsView(view):
  print("##### initImageOrientationGraphicsView")
  global g_ImageOrientationGraphicsView
  g_ImageOrientationGraphicsView = view
  global TempObj
  
  global activePositioning
  activePositioning = False
  global activeMasking
  activeMasking = False
  global ToggleLoadPreviousData
  ToggleLoadPreviousData = 0
  
  global runAllFirstBackgroundTask
  runAllFirstBackgroundTask = False
  print("Init ImageOrientation")
  ctx.field("itkImageFileReader1.unresolvedFileName").setStringValue("")
  ctx.field("AlreadyModifiedMaskReader.unresolvedFileName").setStringValue("")
  ctx.field("AlreadyModifiedMaskReader.fileName").setStringValue("")
  ctx.field("itkImageFileReader1.fileName").setStringValue("")
  ctx.field("itkImageFileReader1.close").touch()
  ctx.field("AlreadyModifiedMaskReader.close").touch()
  ctx.field("SwitchAlreadyRegistered.currentInput").setValue(0)
  ctx.field("AlreadyModifiedMask.currentInput").setValue(0)
  ctx.field("Switch.currentInput").setValue(0)
  ctx.field("GetAtlasMacro.itkImageFileReader.open").touch()
  ctx.field("GetAtlasMacro.itkImageFileReaderMask.open").touch()
  ctx.field("ToggleBrainMask.on").setValue(False)
  ctx.field("BackgroundTaskRunning").setBoolValue(False)
  ctx.field("SwitchAlreadyRegistered.currentInput").setValue(0)
  ctx.field("AlreadyModifiedMask.currentInput").setValue(0)
  ctx.field("adaptTemplateMask.SoToggleMaskEditor.on").setBoolValue(False)
  ctx.field("mialImageReconstruction.ImageBaseOfRecon").setIntValue(ctx.field("RefImageFiled").value)
  ctx.field("WaitForReconstruction").setBoolValue(False)
  ctx.field("WaitForSliceIntensity").setBoolValue(False)
  ctx.field("insertOrientDone").setValue(False)
  ctx.field("insertOrientNLMDone").setValue(False)
  ctx.field("insertOrientMaskDone").setValue(False)
  ctx.field("FirstSDIDone").setValue(False)
  ctx.field("outputSucceed").setObject(ctx.field("FirstSDIDone"))
  
  if MLABFileManager.exists(ctx.expandFilename("$(MLAB_mevisFetalMRI_MRUser)\Projects\TestInterface2\Data\CRL_Fetal_Brain_Atlas_2017")):
    ctx.field("GetAtlasMacro.name").setStringValue("$(MLAB_mevisFetalMRI_MRUser)\Projects\TestInterface2\Data\CRL_Fetal_Brain_Atlas_2017")
    ctx.field("GetAtlasMacro.AtlasPath").setStringValue("$(MLAB_mevisFetalMRI_MRUser)\Projects\TestInterface2\Data\CRL_Fetal_Brain_Atlas_2017")
    ctx.module("GetAtlasMacro").call("updateAtlas")
  showImageOrientationInterface()
  resetZoom()
  TempObj=temporaryObject()
  
  
def showImageOrientationInterface():
  
  global g_layoutImageOrientation
  global g_sceneImageOrientation
  global g_ImageOrientationGraphicsView
  global g_HorizontalControl
  global ExportMode
  global ShouldRunBackgroundTask
  ShouldRunBackgroundTask = False
  
  g_HorizontalControl = {}
  g_sceneImageOrientation = g_ImageOrientationGraphicsView.scene()
  g_sceneImageOrientation.clear()
  #setSceneBackgroundColor(g_sceneImageOrientation)
  #layer = g_sceneImageOrientation.addLayer()

  #g_layoutImageOrientation = g_sceneImageOrientation.createVerticalLayout(layer)
  
  #MainMDL = g_sceneImageOrientation.addMDL("Panel { module=parent:ImageOrientationInterface window=ImageOrientationPanel }")
  #ctx.control("GraphicsView" ).setProperty("expandX",False)
  mdlToSet = ""
  numIm = ctx.field("NumberImages").value
  print("number of images: %s"%numIm)
  tooltipButtonPosition = "Set IH T B A P, by putting the mouse over the posterior comissure and then press I, any position on the inter hemisphere plan Top/Bottom/Anterior/Posterior position and then press T/B/A/P. you can set the position in whatever order; enter to validate"
  tooltipCheckBox = "All images checked will be taken as input for AllBackgroundTasks button"
  tooltipComboBox = "to be set at the beginning: the orientation of the brain"
  tooltipGenerateMask = "if you want to modify the automatically generated brain mask: left click mouse button and draw, 4,6,2,8 to translate in antero-posterior and top-bottom direction, 1,9 to rotate, backspace to delete the mask on the slice, enter to validate "
  tooltipResetMask = "delete all mask editing and redraw brain mask according to positioning and gestational age"
  for i in range(numIm):
    buttonDefinition = """Button {expandX = No title = \"Image %i\" name = buttonImage%i command = "py: updateImage(\'Image%i\')"}"""%(i,i,i)
    #buttonImage = g_sceneImageOrientation.addMDL(buttonDefinition, True)
    buttonPositioningDef = """Button {expandX = No title = "Set IH T B A P positons" name = buttonPositioning%i tooltip = "%s" command = "py: setPositioning(\'Image%i\')"}"""%(i,tooltipButtonPosition,i)
    #buttonPositioning = g_sceneImageOrientation.addMDL(buttonPositioningDef, True)
    buttonResetBrainMaskDef = """Button {expandX = No title ="Reset Brain Mask" name = buttonResetBrainMask%i tooltip = "%s" enabled = False command = "py: resetBrainMask(\'Image%i\')"}"""%(i,tooltipResetMask,i)
    #buttonManualPositioning = g_sceneImageOrientation.addMDL(buttonManualPositioningDef, True)
    buttonGenerateMaskDef = """Button {expandX = No title = "Edit Brain Mask" name = GenerateBrainMask%i tooltip = "%s" enabled = False command = "py: generateBrainMask(\'Image%i\')"}"""%(i,tooltipGenerateMask,i)
    #buttonGenerateMask = g_sceneImageOrientation.addMDL(buttonGenerateMaskDef, True)
    checkBoxDefinition = """CheckBox {name = checkImage%i tooltip = "%s"}"""%(i,tooltipCheckBox)
    #checkBoxImage = g_sceneImageOrientation.addMDL(checkBoxDefinition,True)
    comboboxDefinition = """ComboBox {expandX = No name = \"comboImage%i\" items {item = unknown item = axial item = sagittal item = coronal} tooltip = "%s" textChangedCommand = "py: registerplaneOrientation(\'Image%i\')"}"""%(i,tooltipComboBox,i)
    LabelOrder = """ Label Order:%i {name = LabelImage%i}"""%(i,i)
    ButtonUpDefinition = """Button {name = moveUpImage%i image = $(MLAB_mevisFetalMRI_MRUser)/Modules/Graphics/up-arrow-symbol-icon-68695.png command = "py: changeOrder(\'Image%i\',+1)"}"""%(i,i)
    ButtonDownDefinition = """Button {name = moveDownImage%i image = $(MLAB_mevisFetalMRI_MRUser)/Modules/Graphics/down-arrow-symbol-icon-68695.png command = "py: changeOrder(\'Image%i\',-1)"}"""%(i,i)
    
    #mdlToSet +="""Horizontal { name = \"horizontal%i\"  "+ buttonDefinition + buttonPositioningDef + buttonManualPositioningDef + buttonGenerateMaskDef + comboboxDefinition + checkBoxDefinition " Execute = "py: getHorizontalControl(\'horizontal%i\')" } """%(i,i)
    mdlToSet +="""Horizontal { name = \"horizontal%i\"  """%i + buttonDefinition + buttonPositioningDef + buttonGenerateMaskDef + buttonResetBrainMaskDef+ comboboxDefinition + checkBoxDefinition + LabelOrder + ButtonUpDefinition + ButtonDownDefinition + """ Execute = "py: getHorizontalControl(\'Image%i\',\'horizontal%i\')" } """%(i,i)
    #mdlPanel = g_sceneImageOrientation.addMDL(mdlToSet)
    #g_layoutImageOrientation.addItem(mdlPanel)
    #comboboxImage = g_sceneImageOrientation.addMDL(comboboxDefinition,True)
    #comboboxImage.setProperty("expandX",False)
    #buttonPositioning.setProperty("expandX",False)
    #buttonImage.setProperty("expandX",False)
    #comboboxImage.setProperty("expandY",True)
    #g_layoutImageOrientation.addItem(buttonImage,i,0)
    #g_layoutImageOrientation.addItem(buttonPositioning,i,1)
    #g_layoutImageOrientation.addItem(buttonManualPositioning,i,2)
    #g_layoutImageOrientation.addItem(comboboxImage,i,3)
    #g_layoutImageOrientation.addItem(buttonGenerateMask,i,4)
    #g_layoutImageOrientation.addItem(checkBoxImage,i,5)
  
  #FieldRefImage = """Field RefImageFiled {step = 1 title = "refImage: should be the one with the less movement"}"""
  
  buttonResetIm = """Button {expandX = No title = "Reset Images" name = buttonResetImage command = "py: resetImages()"}"""
  
  buttonAllBackgroundTasks = """Button {expandX = No title = "NextStep (1st SDI)" name = AllBackgroundTasks command = "py: runAllFirstSetBackgroundTasks()" dependsOn = !BackgroundTaskRunning}"""
  
  if ctx.field("ExpertMode").value:
     buttonRunDenoising = """Button {expandX = No title = "Denoise Images" name = buttonDenoiseImage command = "py: denoiseImages()" dependsOn = !BackgroundTaskRunning}"""
     comboboxRegistration = """ComboBox RegistrationMetric {expandX = No textChangedCommand = "py: updateParameterRegistration()"}"""
     FieldPresentRegistration = """Field FieldPresentRegistration {format = STRING title = "Registration Param:" edit = no}"""
     FieldGradient = """Field RegistrationGradient {title = "Gradient"}"""
     FieldMinStep = """Field RegistrationMinStep {title = "Min Step"}"""
     FieldMaxStep = """Field RegistrationMaxStep {title = "Max Step"}"""
     FieldRegNbIt = """Field RegistrationNbIt {title = "Nb It"}"""
     FieldRelaxFactor = """Field RegistrationRelaxationFactor {title = "Relaxation Factor"}"""
     buttonLoadPreviousData = """Button {expandX = No title = "Load Raw Data" name = buttonLoadPreProcessed command = "py: loadPreProcessedAlready()"}"""
     
  FieldStatus = """Label {title = "BackgroundTaskInfo" name = StatusField}"""
  
  #mdlToSet += """Horizontal { name = horizontalRef """ + FieldRefImage + """ Execute = "py: getHorizontalControl(\'refButtons\',\'horizontalRef\')" } """
  mdlToSet += """Horizontal { name = horizontalFinal """ + buttonResetIm + buttonAllBackgroundTasks + FieldStatus + """ Execute = "py: getHorizontalControl(\'LastButtons\',\'horizontalFinal\')" } """
  
  if ctx.field("ExpertMode").value:
    mdlToSet += """Horizontal { name = horizontalExportMode alignX = Left """ + buttonRunDenoising + """ Execute = "py: getHorizontalControl(\'ExpertButtons\',\'horizontalExportMode\')" } """
    mdlToSet += """Horizontal { name = horizontalExportMode2 alignX = Left """ + FieldPresentRegistration + """ Execute = "py: getHorizontalControl(\'ExpertButtons2\',\'horizontalExportMode2\')" } """
    mdlToSet += """Horizontal { name = horizontalExportMode3 alignX = Left """ + comboboxRegistration + FieldGradient + FieldMinStep + FieldMaxStep + FieldRegNbIt + FieldRelaxFactor + """ Execute = "py: getHorizontalControl(\'ExpertButtons3\',\'horizontalExportMode3\')" } """
    mdlToSet += """Horizontal { name = horizontalExportMode4 alignX = Left """ + buttonLoadPreviousData + """ Execute = "py: getHorizontalControl(\'ExpertButtons4\',\'horizontalExportMode4\')" } """
    #mdlToSet += buttonRunDenoising
  #mdlToSet += FieldStatus
  g_sceneImageOrientation.addMDL("Vertical {" + mdlToSet + "}")
  
  updateComboBox()
  
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


def updateComboBox():
    
  global g_HorizontalControl
  try:
    inImages = ctx.field("inImageInfos").object()
    if inImages is not None:
      print("set orientation plane")
      dictItem = {"unknown":0,"axial":1,"sagittal":2,"coronal":3}  
      
      for imiter in inImages.keys():
       if "Image" in imiter:
        if 'file' in inImages[imiter].keys():
          if ctx.hasControl("button%s"%imiter):
            try:
              ctx.control("button%s"%imiter).setStyleSheetFromString('QPushButton { background-color: "blue"; }')
            except Exception as err:
              print("test background has control didn't work")
          else:
            try:
              g_HorizontalControl[imiter].control("button%s"%imiter).setStyleSheetFromString('QPushButton { background-color: "blue"; }')
            except Exception as err:
              print("test background g_horizontalcontrol didn't work")
        
        if 'planeOrientation' in inImages[imiter].keys():
            if ctx.hasControl("combo%s"%imiter):
             try:
               ctx.control("combo%s"%imiter).setCurrentItem(dictItem[inImages[imiter]['planeOrientation']])
             except Exception as err:
               ctx.control("combo%s"%imiter).setCurrentItem(0)
            else:
              valIm = imiter.split('Image')[-1]
              try:
                g_HorizontalControl[imiter].control("comboImage%s"%valIm).setCurrentItem(dictItem[inImages[imiter]['planeOrientation']])
              except Exception as err:
                g_HorizontalControl[imiter].control("comboImage%s"%valIm).setCurrentItem(0)
                
        if 'mask' in inImages[imiter].keys():
          valIm = imiter.split('Image')[-1]
          if ctx.hasControl("GenerateBrainMask%s"%valIm):
            try:
              ctx.control("GenerateBrainMask%s"%valIm).setStyleSheetFromString('QPushButton { background-color: "blue"; }')
              ctx.control("buttonResetBrainMask%s"%valIm).setEnabled(True)
            except Exception as err:
              print("test background has control didn't work")
          else:
            try:
              g_HorizontalControl[imiter].control("GenerateBrainMask%s"%valIm).setStyleSheetFromString('QPushButton { background-color: "blue"; }')
              g_HorizontalControl[imiter].control("buttonResetBrainMask%s"%valIm).setEnabled(True)
            except Exception as err:
              print("test background g_horizontalcontrol didn't work")
         
        if "WorldChanged" in inImages[imiter].keys():
          valIm = imiter.split('Image')[-1]
          if ctx.hasControl("GenerateBrainMask%s"%valIm):
            try:
              ctx.control("GenerateBrainMask%s"%valIm).setEnabled(True)
            except Exception as err:
              print("don't know")
          else:
            try:
              g_HorizontalControl[imiter].control("GenerateBrainMask%s"%valIm).setEnabled(True)
            except Exception as err:
              print("don't know")
                
    #Set the FieldStatus in Red
    g_HorizontalControl["LastButtons"].control("StatusField").setStyleSheetFromString('QLabel { color: "red"; }')
    
    
  except Exception as err:
    pass
 
  #g_layoutImageOrientation.addItem(buttonResetIm)
  #g_layoutImageOrientation.addItem(MainMDL,0,4)

def getAtlasManually():
  
  exp = ctx.expandFilename(ctx.field("AtlasImage").stringValue())
  filename = MLABFileDialog.getOpenFileName(exp, "", "Open file")
  
  ctx.field("GetAtlasMacro.itkImageFileReader.fileName").setStringValue(filename)

def resetImages():

  ctx.field("itkImageFileReader.fileName").setStringValue("")
  resetZoom()
  ctx.field("Switch.currentInput").setValue(0)
  ctx.field("AlreadyModifiedMask.currentInput").setValue(0)
  ctx.field("SwitchAlreadyRegistered.currentInput").setValue(0)
  
  global g_HorizontalControl  
  
  for iterIm in range(ctx.field("NumberImages").value):
    print("reset combo box")
    if ctx.hasControl("comboImage%i"%iterIm):
      ctx.control("comboImage%i"%iterIm).setCurrentItem(0)
    else:
      g_HorizontalControl["Image%i"%iterIm].control("comboImage%i"%iterIm).setCurrentItem(0)

  inImages = None
  ctx.field("inImageInfos").setObject(inImages)
  ctx.field("outImagesInfosStep1").setObject(inImages)
  print("images reseted")
  
  
  for iterbutton in range(ctx.field("NumberImages").value):
         
    if ctx.hasControl("buttonImage%i"%iterbutton):
      try:
        ctx.control("buttonImage%i"%iterbutton).setStyleSheetFromString('QPushButton { background-color: "white"; }')
      except Exception as err:
        print("test background has control didn't work")
    else:
      try:
        g_HorizontalControl["Image%i"%iterbutton].control("buttonImage%i"%iterbutton).setStyleSheetFromString('QPushButton { background-color: "white"; }')
      except Exception as err:
        print("test background g_horizontalcontrol didn't work")

    if ctx.hasControl("GenerateBrainMask%s"%iterbutton):
      try:
        ctx.control("GenerateBrainMask%s"%iterbutton).setStyleSheetFromString('QPushButton { background-color: "white"; }')
        ctx.control("GenerateBrainMask%s"%iterbutton).setEnabled(False)
      except Exception as err:
        print("test background has control didn't work")
    else:
      try:
        g_HorizontalControl[imiter].control("GenerateBrainMask%s"%iterbutton).setStyleSheetFromString('QPushButton { background-color: "white"; }')
        g_HorizontalControl[currentImage].control("GenerateBrainMask%s"%iterbutton).setEnabled(False)
      except Exception as err:
        print("test background g_horizontalcontrol didn't work")   
        
    if ctx.hasControl("buttonResetBrainMask%s"%iterbutton):
      try:
         ctx.control("buttonResetBrainMask%s"%iterbutton).setEnabled(False)
      except Exception as err:
         print("don't know")
    else:
      try:
         g_HorizontalControl[currentImage].control("buttonResetBrainMask%s"%iterbutton).setEnabled(False)
      except Exception as err:
         print("don't know")    

def updateImage(Image="Image0"):
  
  #first backup current cso if there is one
  global TempObj
  global currentImage
  global g_HorizontalControl
  #if currentImage !=  None:
  #  TempObj.getTempCSOs()
  
  currentImage = Image
  global activePositioning
  activePositioning = False
  inImages = ctx.field("inImageInfos").object()
  print("New Image : %s"%Image)
  try:
    filename = inImages[Image]["file"]
  except Exception as err:
    print("inImages must be a dictionnary with 'Image0' etc as keys and inImage['Image0'] must be a dictionnary with at least 'file' as key")
    
    if ToggleLoadPreviousData == 0:
      print("select manually a file for debug")
      if inImages == None:
        exp = ctx.expandFilename(ctx.field("AtlasImage").stringValue())
      else:
        exp = os.path.dirname(inImages[list(inImages.keys())[0]]['file'])
      filename = MLABFileDialog.getOpenFileName(exp, "", "Open file")
      if filename == "":
        print("no file selected")
        return
      if inImages != None:
        inImages.update({Image:{"file":filename}})
      else:
        inImages={Image:{"file":filename}}
    elif ToggleLoadPreviousData == 1:
      print("select manually files")
      #Raw Image
      if inImages == None:
        exp = ctx.expandFilename(ctx.field("AtlasImage").stringValue())
      else:
        exp = os.path.dirname(inImages[list(inImages.keys())[0]]['file'])
      filename = MLABFileDialog.getOpenFileName(exp, "", "Open file ex: Axial2.nii.gz")
      if filename == "":
        print("no file selected")
        return
      if inImages != None:
        inImages.update({Image:{"file":filename}})
      else:
        inImages={Image:{"file":filename}}
      #Reoriented Image
      exp = os.path.dirname(inImages[list(inImages.keys())[0]]['file'])
      filename = MLABFileDialog.getOpenFileName(exp, "", "Open WorldChanged ex: Axial2_worldmatrixModified_lr.nii.gz")
      if filename == "":
        print("no WorldChanged selected")
        return
      inImages[Image].update({"WorldChanged":filename})
      
      #NLM Image
      exp = os.path.dirname(inImages[list(inImages.keys())[0]]['file'])
      filename = MLABFileDialog.getOpenFileName(exp, "", "Open NLMWorldChanged ex: Axial2_worldmatrixModified_lr_NLM.nii.gz")
      if filename != "":
        inImages[Image].update({"NLMWorldChanged":filename})
      else:
        print("no NLMWorldChanged selected")
      
      #Mask
      exp = os.path.dirname(inImages[list(inImages.keys())[0]]['file'])
      filename = MLABFileDialog.getOpenFileName(exp, "", "Open mask ex: Axial2_worldmatrixModified_lr_brain_mask.nii.gz")
      if filename == "":
        print("no mask selected")
        return
      inImages[Image].update({"mask":filename})
      valIm = currentImage.split('Image')[-1]
      g_HorizontalControl[Image].control("GenerateBrainMask%s"%valIm).setStyleSheetFromString('QPushButton { background-color: "blue"; }')
      g_HorizontalControl[Image].control("GenerateBrainMask%s"%valIm).setEnabled(True)
      g_HorizontalControl[Image].control("buttonResetBrainMask%s"%valIm).setEnabled(True)
      ctx.control("checkImage%s"%valIm).setChecked(True)
      
      
    #return
  
  ctx.field("itkImageFileReader.fileName").setStringValue(filename)
  ctx.field("inImageInfos").setObject(inImages)
  ctx.field("outImagesInfosStep1").setObject(inImages)
  
  if ctx.hasControl("button%s"%Image):
    try:
      ctx.control("button%s"%Image).setStyleSheetFromString('QPushButton { background-color: "blue"; }')
    except Exception as err:
      print("test background has control didn't work")
  else:
    try:
      g_HorizontalControl[Image].control("button%s"%Image).setStyleSheetFromString('QPushButton { background-color: "blue"; }')
    except Exception as err:
      print("test background g_horizontalcontrol didn't work")
  
  #la il faut aussi gerer les labels anatomics qu'on affiche
  if "Positioning" in inImages[Image].keys():
    if "A" in inImages[Image]["Positioning"].keys():
      ctx.field("LabelA.worldPosition").setValue(inImages[Image]["Positioning"]["A"])
      ctx.field("LabelA.drawingOn").setValue(True)
    else:
      ctx.field("LabelA.drawingOn").setValue(False)
    
    if "P" in inImages[Image]["Positioning"].keys():
      ctx.field("LabelP.worldPosition").setValue(inImages[Image]["Positioning"]["P"])
      ctx.field("LabelP.drawingOn").setValue(True)
    else:
      ctx.field("LabelP.drawingOn").setValue(False)
    
    if "IH" in inImages[Image]["Positioning"].keys():
      ctx.field("LabelIH.worldPosition").setValue(inImages[Image]["Positioning"]["IH"])
      ctx.field("LabelIH.drawingOn").setValue(True)
    else:
      ctx.field("LabelIH.drawingOn").setValue(False)
    
    if "T" in inImages[Image]["Positioning"].keys():
      ctx.field("LabelT.worldPosition").setValue(inImages[Image]["Positioning"]["T"])
      ctx.field("LabelT.drawingOn").setValue(True)
    else:
      ctx.field("LabelT.drawingOn").setValue(False)
    
    if "B" in inImages[Image]["Positioning"].keys():
      ctx.field("LabelB.worldPosition").setValue(inImages[Image]["Positioning"]["B"])
      ctx.field("LabelB.drawingOn").setValue(True)
    else:
      ctx.field("LabelB.drawingOn").setValue(False)
  else:
    ctx.field("LabelIH.drawingOn").setValue(False)
    ctx.field("LabelA.drawingOn").setValue(False)
    ctx.field("LabelP.drawingOn").setValue(False)
    ctx.field("LabelT.drawingOn").setValue(False)
    ctx.field("LabelB.drawingOn").setValue(False)
    ctx.field("ToggleBrainMask.on").setBoolValue(False)
    print("No label to show")  
  
  
  if "WorldChanged" in inImages[Image].keys():
      ctx.field("itkImageFileReader1.unresolvedFileName").setValue(inImages[Image]["WorldChanged"])
      ctx.field("SwitchAlreadyRegistered.currentInput").setValue(1)
  else:
      ctx.field("SwitchAlreadyRegistered.currentInput").setValue(0)
      ctx.field("Switch.currentInput").setValue(0)
    
    #here I do, if all positioning are present I toggle the brain cso but it means that I reload the transfo in setworldmatrix and transformworldmatrix 
  if "WorldChanged" in inImages[Image].keys() and "planeOrientation" in inImages[Image].keys():
      if inImages[Image]["planeOrientation"] == 'axial':
        ctx.field("OrthoSwapFlip.view").setValue("Transversal")
      elif inImages[Image]["planeOrientation"] == 'coronal':
        ctx.field("OrthoSwapFlip.view").setValue("Coronal")
      elif inImages[Image]["planeOrientation"] == 'sagittal':
        ctx.field("OrthoSwapFlip.view").setValue("Sagittal")
      else:
        print("orientation unknown")
      ctx.field("Switch.currentInput").setValue(1)  
      #ctx.field("adaptTemplateMask.updateCSOButton").touch()
      ctx.field("ToggleBrainMask.on").setBoolValue(True)
  else:
      if "WorldChanged" in inImages[Image].keys():
        ctx.field("ToggleBrainMask.on").setBoolValue(True)
        ctx.field("Switch.currentInput").setValue(1)
      else:  
        ctx.field("ToggleBrainMask.on").setBoolValue(False)
        ctx.field("Switch.currentInput").setValue(0)
        print("Switch set to 0")
        
  if "mask" in inImages[Image].keys():
     ctx.field("AlreadyModifiedMaskReader.fileName").setStringValue(inImages[Image]["mask"])
     ctx.field("AlreadyModifiedMask.currentInput").setValue(1)
     ctx.field("adaptTemplateMask.updateCSOButton").touch()
     
  else:
     ctx.field("AlreadyModifiedMask.currentInput").setValue(0)
     ctx.field("adaptTemplateMask.updateCSOButton").touch()
     
  if "planeOrientation" in inImages[Image].keys():
    dictItem = {"unknown":0,"axial":1,"sagittal":2,"coronal":3}
    if ctx.hasControl("combo%s"%Image):
      if inImages[Image]["planeOrientation"] != ctx.control("combo%s"%Image).currentText():
        ctx.control("combo%s"%Image).setCurrentItem(dictItem[inImages[Image]['planeOrientation']])
    else:
      print("why the f... this combo doesn't have any control")

  ctx.field("SoInteractionMapping1.ignoreOtherCommandActions").setBoolValue(False)
  ctx.field("adaptTemplateMask.SoToggleMaskEditor.on").setBoolValue(False)
  ctx.field("LabelViewerMode.text").setStringValue("Viewer Mode %s"%Image)
  ctx.field("View2DExtensions.lut.relative").setBoolValue(True)
  if ctx.hasControl("button%s"%Image):
    print("has control")
  else:
      print("doesn't have control")
  print("label updated")
  
def setPositioning(Image="Image0"):
  
  updateImage(Image)
  ctx.field("SwitchAlreadyRegistered.currentInput").setValue(0)
  ctx.field("ToggleBrainMask.on").setBoolValue(False)
  ctx.field("Switch.currentInput").setValue(0)
  print("New Image : %s"%Image)
  
  #faire une message box, click IH puis T puis B puis A puis P ? ou faire message box, click IH, puis message box, click T etc ...
  #ctx.field("MessageBoxImOrient.message").setStringValue("click anywhere on the Inter Hemisphere Plan")
  
  #ctx.module("MessageBoxImOrient").showWindow("dialog")
  global activePositioning
  activePositioning = True
  global currentPositioning
  currentPositioning = Image
  #ctx.field("View2DExtensions.annotation.wantsKeyEvents").setBoolValue(False)
  ctx.field("View2DExtensions.annotation.editingOn").setBoolValue(False)
  ctx.field("SoView2D.useManagedInteraction").setBoolValue(True)
  ctx.field("eventChoice").setValue(1)
  ctx.field("LabelViewerMode.text").setStringValue("Set Positioning Mode")
  #while tuppleWorldPosition == None:
  #    tuppleWorldPosition = button1PressedImOrient() #ctx.field("SyngoViaView2DOverlay.annoReadPix.worldPosition").value
  #ctx.showModalDialog("ViewerOnly")
  #print("modal")
  #button1PressedImOrient()
  #print(tuppleWorldPosition)

  
def registerplaneOrientation(Image="Image0"):
  
  if not ctx.hasControl("combo%s"%Image):
    return
  
  inImages = ctx.field("inImageInfos").object()
  
  try:
    if inImages == None:
      inImages={Image:{"planeOrientation":ctx.control("combo%s"%Image).currentText()}}
    
    else:
      if Image in inImages.keys():
        inImages[Image].update({"planeOrientation":ctx.control("combo%s"%Image).currentText()})
      else:
        inImages.update({Image:{"planeOrientation":ctx.control("combo%s"%Image).currentText()}})

  except Exception as err:
    pass
  
  #a actualiser seulement si Image == currentImage
  global currentImage
  if currentImage != None:
    if currentImage == Image:
      if ctx.control("combo%s"%Image).currentText() == 'axial':
        ctx.field("OrthoSwapFlip.view").setValue("Transversal")
      elif ctx.control("combo%s"%Image).currentText() == 'coronal':
        ctx.field("OrthoSwapFlip.view").setValue("Coronal")
      elif ctx.control("combo%s"%Image).currentText() == 'sagittal':
        ctx.field("OrthoSwapFlip.view").setValue("Sagittal")
      else:
        print("orientation unknown")
      
      ctx.field("adaptTemplateMask.updateCSOButton").touch()
      updateImage(Image)
      
    
  ctx.field("inImageInfos").setObject(inImages)
  ctx.field("outImagesInfosStep1").setObject(inImages)


def button1PressedImOrient(event,control):
  
  global activeShowPosition
  global activePositioning
  #print activeShowPosition
  if event["type"] != "MouseButtonPress":
    control.setEatEvent(True)
  
  #if activeShowPosition == 0:
  #  ctx.field("cornerMenuIteraction").setValue(1)
  
  inImages = ctx.field("inImageInfos").object()  

  #print("eventChoice button1PressedImOrient")
  #print(ctx.field("eventChoice").value)
  if activePositioning == True:
    if event["type"] == "MouseButtonPress":
      if event["button"] == "left":
        if ctx.field("View2DExtensions.annoReadPix.outputValue").value == 0:
          print("not within the brain")
          return
        
        #print("ecriture label")
        position = ctx.field("View2DExtensions.annoReadPix.worldPosition").value
        #est-ce qu'on check que l'output value est differente de 0 ? ctx.field("SyngoViaView2DOverlay.annoReadPix.outpoutValue").value
        print(position)
        #ctx.field("LabelIH.worldPosition").setValue(position)
      
      #if event["button"] == "right":
      #  print("validate position")
      #  return
    
    elif event["type"] == "KeyPress":    
      print(event["key"])
      if event["key"] == "I":
        positionI = ctx.field("View2DExtensions.annoReadPix.worldPosition").value
        positionIvox = ctx.field("View2DExtensions.annoReadPix.voxelPosition").value
        ctx.field("LabelIH.worldPosition").setValue(positionI)
        if inImages == None:
          inImages={currentPositioning:{"Positioning":{"IH":ctx.field("LabelIH.worldPosition").value,"IHvox":positionIvox}}}
        else:
          if currentPositioning in inImages.keys():
            if "Positioning" in inImages[currentImage].keys():
               inImages[currentPositioning]["Positioning"].update({"IH":ctx.field("LabelIH.worldPosition").value,"IHvox":positionIvox})
            else:
               inImages[currentPositioning].update({"Positioning":{"IH":ctx.field("LabelIH.worldPosition").value,"IHvox":positionIvox}})
          else:
            inImages.update({currentPositioning:{"Positioning":{"IH":ctx.field("LabelIH.worldPosition").value,"IHvox":positionIvox}}})
        ctx.field("LabelIH.drawingOn").setValue(True)
        
      elif event["key"] == "A":
        positionA = ctx.field("View2DExtensions.annoReadPix.worldPosition").value
        positionAvox = ctx.field("View2DExtensions.annoReadPix.voxelPosition").value
        ctx.field("LabelA.worldPosition").setValue(positionA)
        if inImages == None:
          inImages={currentPositioning:{"Positioning":{"A":ctx.field("LabelA.worldPosition").value,"Avox":positionAvox}}}
        else:
          if currentPositioning in inImages.keys():
            if "Positioning" in inImages[currentImage].keys():
               inImages[currentPositioning]["Positioning"].update({"A":ctx.field("LabelA.worldPosition").value,"Avox":positionAvox})
            else:
               inImages[currentPositioning].update({"Positioning":{"A":ctx.field("LabelA.worldPosition").value,"Avox":positionAvox}})
          else:
            inImages.update({currentPositioning:{"Positioning":{"A":ctx.field("LabelA.worldPosition").value,"Avox":positionAvox}}})
        ctx.field("LabelA.drawingOn").setValue(True)
      elif event["key"] == "P":
        positionP = ctx.field("View2DExtensions.annoReadPix.worldPosition").value
        positionPvox = ctx.field("View2DExtensions.annoReadPix.voxelPosition").value
        ctx.field("LabelP.worldPosition").setValue(positionP)
        if inImages == None:
          inImages={currentPositioning:{"Positioning":{"P":ctx.field("LabelP.worldPosition").value,"Pvox":positionPvox}}}
        else:
          if currentPositioning in inImages.keys():
            if "Positioning" in inImages[currentImage].keys():
               inImages[currentPositioning]["Positioning"].update({"P":ctx.field("LabelP.worldPosition").value,"Pvox":positionPvox})
            else:   
              inImages[currentPositioning].update({"Positioning":{"P":ctx.field("LabelP.worldPosition").value,"Pvox":positionPvox}})
          else:
            inImages.update({currentPositioning:{"Positioning":{"P":ctx.field("LabelP.worldPosition").value,"Pvox":positionPvox}}})
        ctx.field("LabelP.drawingOn").setValue(True)  
      elif event["key"] == "T":
        positionT = ctx.field("View2DExtensions.annoReadPix.worldPosition").value
        positionTvox = ctx.field("View2DExtensions.annoReadPix.voxelPosition").value
        ctx.field("LabelT.worldPosition").setValue(positionT)
        if inImages == None:
          inImages={currentPositioning:{"Positioning":{"T":ctx.field("LabelT.worldPosition").value,"Tvox":positionTvox}}}
        else:
          if currentPositioning in inImages.keys():
            if "Positioning" in inImages[currentImage].keys():
               inImages[currentPositioning]["Positioning"].update({"T":ctx.field("LabelT.worldPosition").value,"Tvox":positionTvox})
            else:
              inImages[currentPositioning].update({"Positioning":{"T":ctx.field("LabelT.worldPosition").value,"Tvox":positionTvox}})
          else:
            inImages.update({currentPositioning:{"Positioning":{"T":ctx.field("LabelT.worldPosition").value,"Tvox":positionTvox}}})
        ctx.field("LabelT.drawingOn").setValue(True)    
      elif event["key"] == "B":
        positionB = ctx.field("View2DExtensions.annoReadPix.worldPosition").value
        positionBvox = ctx.field("View2DExtensions.annoReadPix.voxelPosition").value
        ctx.field("LabelB.worldPosition").setValue(positionB)
        if inImages == None:
          inImages={currentPositioning:{"Positioning":{"B":ctx.field("LabelB.worldPosition").value,"Bvox":positionBvox}}}
        else:
          if currentPositioning in inImages.keys():
            if "Positioning" in inImages[currentImage].keys():
              inImages[currentPositioning]["Positioning"].update({"B":ctx.field("LabelB.worldPosition").value,"Bvox":positionBvox})
            else:  
              inImages[currentPositioning].update({"Positioning":{"B":ctx.field("LabelB.worldPosition").value,"Bvox":positionBvox}})
          else:
            inImages.update({currentPositioning:{"Positioning":{"B":ctx.field("LabelB.worldPosition").value,"Bvox":positionBvox}}})
        ctx.field("LabelB.drawingOn").setValue(True)    
      elif event["key"] == "Return":
        print("Enter")
        if (("IH" in inImages[currentPositioning]["Positioning"].keys()) and ("A" in inImages[currentPositioning]["Positioning"].keys()) and ("P" in inImages[currentPositioning]["Positioning"].keys()) and ("T" in inImages[currentPositioning]["Positioning"].keys()) and ("B" in inImages[currentPositioning]["Positioning"].keys())):
          PerformReorientation()            
          #modify mask
          ctx.module("GetAtlasMacro").call("updateAtlas")
          generateBrainMask(currentImage)

        #voxelworldmatrix = imagetorotate.voxelToWorldMatrix()
        
  elif activePositioning ==False:
    global activeMasking
    #print("activeMaskting")
    #print(activeMasking)
    if activeMasking == True:
      button1PressedMaskRefine(event)
    #else:
      
      #print("j'en veux pas de ton click souris")

def PerformReorientation():
  
  inImages = ctx.field("inImageInfos").object()  
  ctx.field("eventChoice").setValue(0)
  ctx.field("View2DExtensions.annotation.editingOn").setBoolValue(True)
  ctx.field("SoView2D.useManagedInteraction").setBoolValue(False)
  activePositioning = False
  print("anatomic position registered")  
  print("modifying voxel to world transformation matrix")
  if (("IH" in inImages[currentPositioning]["Positioning"].keys()) and ("A" in inImages[currentPositioning]["Positioning"].keys()) and ("P" in inImages[currentPositioning]["Positioning"].keys()) and ("T" in inImages[currentPositioning]["Positioning"].keys()) and ("B" in inImages[currentPositioning]["Positioning"].keys())):
    imagetorotate = ctx.field("itkImageFileReader.output0")
    voxelSize = imagetorotate.voxelSize()
  
    #reset of the world matrix which correspond to the mother and not the foetus
    ctx.field("SetWorldMatrix.scale").setValue(voxelSize)
    ctx.field("SetWorldMatrix.translation").setValue([0,0,0])
    ctx.field("SetWorldMatrix.center").setValue([0,0,0])
    ctx.field("SetWorldMatrix.scaleOrientation").setValue([0,0,1,0])
    ctx.field("SetWorldMatrix.rotation").setValue([0,0,0])
    #ctx.field("SetWorldMatrix.rotation").setValue()
    #ca fait quoi si je prend pas en compte la voxel size?
    PA = numpy.subtract(numpy.multiply(inImages[currentPositioning]["Positioning"]["Avox"],voxelSize),numpy.multiply(inImages[currentPositioning]["Positioning"]["Pvox"],voxelSize))
    PA = PA/numpy.linalg.norm(PA)
    orientPA = numpy.cross(PA,numpy.array([0,-1,0]))
          
    #I think I have to recalculate T or B to make the vector orthogonal to PA. I think that's why sometimes I get a resulting image with A and P flipped. because the vector BT make a rotation along the axis.
    BT = numpy.subtract(numpy.multiply(inImages[currentPositioning]["Positioning"]["Bvox"],voxelSize),numpy.multiply(inImages[currentPositioning]["Positioning"]["Tvox"],voxelSize))
    lengthBT = numpy.linalg.norm(BT)
    BT=BT/numpy.linalg.norm(BT)
    vectorOrthoBoth = numpy.cross(PA,BT)
    recalculateBTortho = numpy.cross(vectorOrthoBoth,PA)
    newBvox= inImages[currentPositioning]["Positioning"]["Tvox"]+numpy.multiply(recalculateBTortho*lengthBT,numpy.array([1/voxelSize[0],1/voxelSize[1],1/voxelSize[2]]))
    #orientBT = numpy.cross(BT,numpy.array([0,0,1]))
          
    if numpy.linalg.norm(orientPA) != 0:
      skewMatrixPA = numpy.array(((0,-orientPA[2],orientPA[1]),(orientPA[2],0,-orientPA[0]),(-orientPA[1],orientPA[0],0)))
      #skewMatrixBT = numpy.array(((0,-orientBT[2],orientBT[1]),(orientBT[2],0,-orientBT[0]),(-orientBT[1],orientBT[0],0)))
          
      SquaredPA = numpy.dot(skewMatrixPA,skewMatrixPA)*1/(1+numpy.dot(PA,numpy.array([0,-1,0])))
      #SquaredBT = numpy.dot(skewMatrixBT,skewMatrixBT)*1/(1+numpy.dot(BT,numpy.array([0,1,0])))
          
      #FullRotation = numpy.array(((1,0,0),(0,1,0),(0,0,1))) + skewMatrixAP  + SquaredAP + SquaredBT + skewMatrixBT
      #ca ne marche pas je ne peux pas composer des matrices de rotations comme ca
      #PA
      RotationPA = numpy.array(((1,0,0),(0,1,0),(0,0,1))) + skewMatrixPA  + SquaredPA
      FullRotationPA = numpy.identity(4)
      FullRotationPA[0:3,0:3]=RotationPA
    else:
      if numpy.sign(PA[1])==-1:
        FullRotationPA = numpy.identity(4)
      elif numpy.sign(PA[1])==1:
        FullRotationPA = numpy.identity(4)
        FullRotationPA[1,1]=-1

    #la le determinant est bien a 1, numpy.linalg.det(FullRotationAP)
          
    #BT
    #RotationBT = numpy.array(((1,0,0),(0,1,0),(0,0,1))) + skewMatrixBT  + SquaredBT
    #FullRotationBT = numpy.identity(4)
    #FullRotationBT[0:3,0:3]=RotationBT          
          
    #FullRotation1[:-1,-1]=-numpy.multiply(numpy.array(inImages[currentPositioning]["Positioning"]['IHvox']),voxelSize)
    Translation = numpy.identity(4)
    Translation[:-1,-1]=-numpy.multiply(numpy.array(inImages[currentPositioning]["Positioning"]['IHvox']),voxelSize)
    RotationPAautour0 = numpy.dot(numpy.dot(numpy.linalg.inv(Translation),FullRotationPA),Translation)
          
    FirstTransformation = numpy.dot(Translation,RotationPAautour0)
    #sens2 = numpy.dot(RotationAPautour0,Translation)
          
    #ca ca marche pas il faut que je recalcul ma nouvelle position de IH ? et B et T aussi en fait.... 
    IHinter=list(inImages[currentPositioning]["Positioning"]['IHvox']) #should I use the voxel size ? but normaly not ...
    IHinter.append(1)
    newIH = numpy.dot(FirstTransformation,numpy.multiply(IHinter,numpy.array([voxelSize[0],voxelSize[1],voxelSize[2],1])))
    print("newIH : %s"%str(newIH))
    Binter=list(newBvox)
    Binter.append(1)
    newB = numpy.dot(FirstTransformation,numpy.multiply(Binter,numpy.array([voxelSize[0],voxelSize[1],voxelSize[2],1])))
    print("newB : %s"%str(newB))
    Tinter=list(inImages[currentPositioning]["Positioning"]['Tvox'])
    Tinter.append(1)          
    newT = numpy.dot(FirstTransformation,numpy.multiply(Tinter,numpy.array([voxelSize[0],voxelSize[1],voxelSize[2],1])))
    print("newT : %s"%str(newT))
    ctx.field("SoView2DLabel.worldPosition").setValue([newB[0],newB[1],newB[2]])
    ctx.field("SoView2DLabel1.worldPosition").setValue([newT[0],newT[1],newT[2]])
    newBT = numpy.subtract(newB[0:3],newT[0:3])
    newBT=newBT/numpy.linalg.norm(newBT)
    print("new BT")
    print(newBT)
    orientBT = numpy.cross(newBT,numpy.array([0,0,-1]))
    if numpy.linalg.norm(orientBT) != 0:
      skewMatrixBT = numpy.array(((0,-orientBT[2],orientBT[1]),(orientBT[2],0,-orientBT[0]),(-orientBT[1],orientBT[0],0)))
      SquaredBT = numpy.dot(skewMatrixBT,skewMatrixBT)*1/(1+numpy.dot(newBT,numpy.array([0,0,-1])))
          
      RotationBT = numpy.array(((1,0,0),(0,1,0),(0,0,1))) + skewMatrixBT  + SquaredBT
      FullRotationBT = numpy.identity(4)
      FullRotationBT[0:3,0:3]=RotationBT
    else:
      if numpy.sign(newBT[2])==-1:
        FullRotationBT = numpy.identity(4)
      elif numpy.sign(newBT[2])==1:
        FullRotationBT = numpy.identity(4)
        FullRotationBT[2,2]=-1
        print(FullRotationBT)
          
    ctx.field("TransformWorldMatrixBT.transformation").setValue(FullRotationBT)
    #on fait pareil avec juste AP pour comprendre
    ctx.field("TransformWorldMatrixTransAndAP.transformation").setValue(FirstTransformation)
          
    #ctx.field("TransformWorldMatrix.transformation").setValue(FullTransfo)
    WorldChangedName= inImages[currentImage]['file']
    WorldChangedNameFull=WorldChangedName.replace('.nii','_worldmatrixModified_lr.nii')
    #WorldChangedNameAP=WorldChangedName.replace('.nii','_worldmatrixModifiedAP.nii')
          
    ctx.field("itkImageFileWriterWorldMatrixChanged.fileName").setValue(WorldChangedNameFull)
    ctx.field("itkImageFileWriterWorldMatrixChanged.save").touch()
    #ctx.field("itkImageFileWriterTransAndAP.unresolvedFileName").setValue(WorldChangedNameAP)
    #ctx.field("itkImageFileWriterTransAndAP.save").touch()
    inImages[currentImage].update({"WorldChanged":WorldChangedNameFull})
          
    #to change
    #inImages[currentImage].update({"newWorldMatrix":FullTransfo})
    ctx.field("inImageInfos").setObject(inImages)
    ctx.field("outImagesInfosStep1").setObject(inImages)
    valIm = currentImage.split('Image')[-1]
    if ctx.hasControl("GenerateBrainMask%s"%valIm):
       try:
         ctx.control("GenerateBrainMask%s"%valIm).setEnabled(True)
         ctx.control("buttonResetBrainMask%s"%valIm).setEnabled(True)
       except Exception as err:
         print("don't know")
    else:
      try:
         g_HorizontalControl[currentImage].control("GenerateBrainMask%s"%valIm).setEnabled(True)
         g_HorizontalControl[currentImage].control("buttonResetBrainMask%s"%valIm).setEnabled(True)
      except Exception as err:
         print("don't know")
  else:
    return
  
def generateBrainMask(Image="Image0"): 
  updateImage(Image)
  print("Mask Generation %s"%Image)
  ctx.field("View2DExtensions.annotation.editingOn").setBoolValue(False)
  ctx.field("SoView2D.useManagedInteraction").setBoolValue(True)
  ctx.field("eventChoice").setValue(2)
  ctx.field("Switch.currentInput").setValue(1)
  ctx.field("ToggleBrainMask.on").setValue(True)
  print("update CSO")
  global activeMasking
  activeMasking = True
  ctx.field("adaptTemplateMask.updateCSOButton").touch()
  ctx.field("adaptTemplateMask.SoToggleMaskEditor.on").setBoolValue(True)
  ctx.field("SoInteractionMapping1.ignoreOtherCommandActions").setBoolValue(True)
  ctx.field("LabelViewerMode.text").setStringValue("Editing Brain Mask Mode")
  
  #remove blue background
  
  #detection of 2,8 4,6 1,9 on the keypad enter to validate
  
def button1PressedMaskRefine(event):

  global currentImage
  
  inImages = ctx.field("inImageInfos").object()
  if event["type"]== "KeyPress":
    if event["key"] != "Return" or event["key"]!="Enter":
      if 'planeOrientation' not in inImages[currentImage].keys():
        print("need to define a plante orientation first")
        return
  
  
  worldToVoxelMatrix = ctx.field("SwitchAlreadyRegistered.output0").worldToVoxelMatrix()
  csoId2Modify = []
  if event["type"] == "KeyPress":
    if event["key"] != "Return" or event["key"]!="Enter":
    #ctx.field("adaptTemplateMask.updateCSOButton").touch()
      CSOobj = ctx.field("adaptTemplateMask.CSOManager1.outCSOList").object()
      for csobj in CSOobj.getCSOs():
        if csobj.isInPlane:
          if ctx.field("SoView2D.startSlice").value == int(csobj.getVoxelBoundingBox(worldToVoxelMatrix)[5]):
              csoId2Modify.append(str(csobj.id))
      csoId2Modifystr=" ".join(csoId2Modify)
      #print(csoId2Modifystr)        
    if event["key"]=="2":   
      #ctx.field("adaptTemplateMask.AffineMatrixComposition.inTranslation").value
      #ctx.field("adaptTemplateMask.AffineMatrixComposition.inTranslation").setValue(numpy.array(ctx.field("adaptTemplateMask.AffineMatrixComposition.inTranslation").value) + numpy.array([0,-1,0,0]))
      #ctx.field("AffineMatrixComposition.update").touch()  
      #which orientation am I ? coronal/sagital 2 is -1 z
      #axial, 2 is -1 y
      if inImages[currentImage]['planeOrientation'] == "coronal" or inImages[currentImage]['planeOrientation'] == "sagittal":
         newMatrix=numpy.array([[1,0,0,0],[0,1,0,0],[0,0,1,-0.5],[0,0,0,1]])
      elif inImages[currentImage]['planeOrientation'] == "axial" :
         newMatrix=numpy.array([[1,0,0,0],[0,1,0,-0.5],[0,0,1,0],[0,0,0,1]])
         
      ctx.field("adaptTemplateMask.CSOAffineTransformationModificator1.csoIdList").setValue(csoId2Modifystr)
      ctx.field("adaptTemplateMask.CSOAffineTransformationModificator1.matrix").setValue(newMatrix)
      ctx.field("adaptTemplateMask.CSOAffineTransformationModificator1.apply").touch()
    
    elif event["key"]=="8":
      if inImages[currentImage]['planeOrientation'] == "coronal" or inImages[currentImage]['planeOrientation'] == "sagittal":
         newMatrix=numpy.array([[1,0,0,0],[0,1,0,0],[0,0,1,0.5],[0,0,0,1]])
      elif inImages[currentImage]['planeOrientation'] == "axial" :
         newMatrix=numpy.array([[1,0,0,0],[0,1,0,0.5],[0,0,1,0],[0,0,0,1]])
         
      ctx.field("adaptTemplateMask.CSOAffineTransformationModificator1.csoIdList").setValue(csoId2Modifystr)
      ctx.field("adaptTemplateMask.CSOAffineTransformationModificator1.matrix").setValue(newMatrix)
      ctx.field("adaptTemplateMask.CSOAffineTransformationModificator1.apply").touch()    
      
    elif event["key"]=="4":
      if inImages[currentImage]['planeOrientation'] == "coronal" or inImages[currentImage]['planeOrientation'] == "axial":
         newMatrix=numpy.array([[1,0,0,-0.5],[0,1,0,0],[0,0,1,0],[0,0,0,1]])
      elif inImages[currentImage]['planeOrientation'] == "sagittal" :
         newMatrix=numpy.array([[1,0,0,0],[0,1,0,-0.5],[0,0,1,0],[0,0,0,1]])
         
      ctx.field("adaptTemplateMask.CSOAffineTransformationModificator1.csoIdList").setValue(csoId2Modifystr)
      ctx.field("adaptTemplateMask.CSOAffineTransformationModificator1.matrix").setValue(newMatrix)
      ctx.field("adaptTemplateMask.CSOAffineTransformationModificator1.apply").touch()    

    elif event["key"]=="6":
      if inImages[currentImage]['planeOrientation'] == "coronal" or inImages[currentImage]['planeOrientation'] == "axial":
         newMatrix=numpy.array([[1,0,0,0.5],[0,1,0,0],[0,0,1,0],[0,0,0,1]])
      elif inImages[currentImage]['planeOrientation'] == "sagittal" :
         newMatrix=numpy.array([[1,0,0,0],[0,1,0,0.5],[0,0,1,0],[0,0,0,1]])
         
      ctx.field("adaptTemplateMask.CSOAffineTransformationModificator1.csoIdList").setValue(csoId2Modifystr)
      ctx.field("adaptTemplateMask.CSOAffineTransformationModificator1.matrix").setValue(newMatrix)
      ctx.field("adaptTemplateMask.CSOAffineTransformationModificator1.apply").touch()    

    elif event["key"]=="1":
      
      if inImages[currentImage]['planeOrientation'] == "coronal":
         newMatrix=numpy.array([[0.99984769502584625,0,0.017452413916200969,0],[0,1,0,0],[-0.017452413916200969,0,0.99984769502584625,0],[0,0,0,1]])
      elif inImages[currentImage]['planeOrientation'] == "axial":
         newMatrix=numpy.array([[0.99984769502584625,-0.017452413916200969,0,0],[0.017452413916200969,0.99984769502584625,0,0],[0,0,1,0],[0,0,0,1]])
      elif inImages[currentImage]['planeOrientation'] == "sagittal" :
         newMatrix=numpy.array([[1,0,0,0],[0,0.99984769502584625,-0.017452413916200969,0],[0,0.017452413916200969,0.99984769502584625,0],[0,0,0,1]])
         
      ctx.field("adaptTemplateMask.CSOAffineTransformationModificator1.csoIdList").setValue(csoId2Modifystr)
      ctx.field("adaptTemplateMask.CSOAffineTransformationModificator1.matrix").setValue(newMatrix)
      ctx.field("adaptTemplateMask.CSOAffineTransformationModificator1.apply").touch()    

      
    elif event["key"]=="9":
      
      if inImages[currentImage]['planeOrientation'] == "coronal":
         newMatrix=numpy.array([[0.99984769502584625,0,-0.017452413916200969,0],[0,1,0,0],[0.017452413916200969,0,0.99984769502584625,0],[0,0,0,1]])
      elif inImages[currentImage]['planeOrientation'] == "axial":
         newMatrix=numpy.array([[0.99984769502584625,0.017452413916200969,0,0],[-0.017452413916200969,0.99984769502584625,0,0],[0,0,1,0],[0,0,0,1]])
      elif inImages[currentImage]['planeOrientation'] == "sagittal" :
         newMatrix=numpy.array([[1,0,0,0],[0,0.99984769502584625,0.017452413916200969,0],[0,-0.017452413916200969,0.99984769502584625,0],[0,0,0,1]])
         
      ctx.field("adaptTemplateMask.CSOAffineTransformationModificator1.csoIdList").setValue(csoId2Modifystr)
      ctx.field("adaptTemplateMask.CSOAffineTransformationModificator1.matrix").setValue(newMatrix)
      ctx.field("adaptTemplateMask.CSOAffineTransformationModificator1.apply").touch()        
    
    
    elif event["key"]=="r" or event["key"]=="R":
      print("should reset only current slice")
      #AffineMatrixComposition.inScale

    elif event["key"]=="Minus":
      print("decrease")
      #ctx.field("adaptTemplateMask.CSOAffineTransformationModificator1.csoIdList").setValue(csoId2Modifystr)
      #if inImages[currentImage]['planeOrientation'] == "coronal":
      #   newMatrix=numpy.array([[0.9,0,0,0],[0,1,0,0],[0,0,0.9,0],[0,0,0,1]])
      #elif inImages[currentImage]['planeOrientation'] == "axial":
      #   newMatrix=numpy.array([[0.9,0,0,0],[0,0.9,0,0],[0,0,1,0],[0,0,0,1]])
      #elif inImages[currentImage]['planeOrientation'] == "sagittal" :
      #   newMatrix=numpy.array([[1,0,0,0],[0,0.9,0,0],[0,0,0.9,0],[0,0,0,1]])
      #print(newMatrix)
      #ctx.field("adaptTemplateMask.CSOAffineTransformationModificator1.apply").touch()
    
    elif event["key"]=="Plus":
      print("increase")
      #ctx.field("adaptTemplateMask.CSOAffineTransformationModificator1.csoIdList").setValue(csoId2Modifystr)
      #if inImages[currentImage]['planeOrientation'] == "coronal":
      #   newMatrix=numpy.array([[1.1,0,0,0],[0,1,0,0],[0,0,1.1,0],[0,0,0,1]])
      #elif inImages[currentImage]['planeOrientation'] == "axial":
      #   newMatrix=numpy.array([[1.1,0,0,0],[0,1.1,0,0],[0,0,1,0],[0,0,0,1]])
      #elif inImages[currentImage]['planeOrientation'] == "sagittal" :
      #   newMatrix=numpy.array([[1,0,0,0],[0,1.1,0,0],[0,0,1.1,0],[0,0,0,1]])
      #print(newMatrix)
      #ctx.field("adaptTemplateMask.CSOAffineTransformationModificator1.apply").touch()
    
    elif event["key"]=="Backspace":
      print("delete current CSO")
      
      isCSOmouse = CSOobj.getCSOCurrentlyUnderMouse()
      
      if isCSOmouse is not None:
         idundermouse = isCSOmouse.id
         ctx.field("adaptTemplateMask.CSOManager1.csoSelectedItems").setValue(idundermouse) 
      else:
         ctx.field("adaptTemplateMask.CSOManager1.csoSelectedItems").setValue(csoId2Modifystr) 
      ctx.field("adaptTemplateMask.CSOManager1.csoRemoveSelected").touch()
    
    elif event["key"] == "Return" or event["key"]=="Enter":
      print(event["key"])
      ctx.field("eventChoice").setValue(0)
      ctx.field("View2DExtensions.annotation.editingOn").setBoolValue(True)
      ctx.field("SoView2D.useManagedInteraction").setBoolValue(False)
      ctx.field("adaptTemplateMask.CSOConvertToImage.apply").touch()
      #if "planeOrientation" in inImages[currentImage]:
      nameInter = inImages[currentImage]['WorldChanged'] 
      NameFull=nameInter.replace('.nii','_brain_mask.nii')
      ctx.field("itkImageFileWriterMask.unresolvedFileName").setStringValue(NameFull)
      ctx.field("itkImageFileWriterMask.save").touch()
      
      #we convert them to dicom as well:
      print("convert Mask To Dicom")
      ctx.field("DicomTagModify.tagValue0").setValue("BrainMask")
      
      if ctx.field("FromFrontier").value:
        #we use dicom tool from testinstall, dicomSend
        print("via Frontier")
        ctx.field("DicomTagModify.tagValue1").setValue(inImages[currentImage]["StudyDescription"])
        ctx.field("DicomTagModify.tagValue2").setValue(inImages[currentImage]["PatientName"])
        ctx.field("DicomTagModify.tagValue3").setValue(inImages[currentImage]["PatientID"])
        ctx.field("DicomTagModify.tagValue5").setValue(time.strftime("%H%M%S"))
        ctx.field("DicomTagModify.tagValue4").setValue(time.strftime("%Y%m%d"))
        
        ctx.field("DicomTagModify.apply").touch()
        _frontier = ctx.module("parent:FrontierSyngoInterface").object()
        ctx.field("parent:DicomExport.exportBaseDir").setStringValue(_frontier.getOutgoingDicomDirectory())
        DicomToolToUse = ctx.module("parent:DicomExport") #ctx.module("DicomTool") #
        print(DicomToolToUse.field("exportBaseDir").value)
        #ctx.connectField("parent:DicomExport.inImage","DicomTagModify.output0")
      else:
        #we use dicom tool from TotalVariationInterface, dicomSave
        print("not via Frontier")
        ctx.field("DicomTagModify.tagValue5").setValue(time.strftime("%H%M%S"))
        ctx.field("DicomTagModify.tagValue4").setValue(time.strftime("%Y%m%d"))
        ctx.field("DicomTagModify.apply").touch()
        DicomToolToUse = ctx.module("DicomTool")
        ctx.field("DicomTool.exportBaseDir").setStringValue(os.path.join(os.path.dirname(inImages["Image0"]["file"]),"Results"))
  
      originalTree = ctx.field("SetDicomTreeOnImage.input0").getDicomTree()
      mutableTree = originalTree.createDerivedTree()
      mutableTree.setPrivateTag(0x07a1, "pdeman", 0x43, inImages[currentImage]["Positioning"]["IH"], "FD")
      mutableTree.setPrivateTag(0x07a1, "pdeman", 0x44, inImages[currentImage]["Positioning"]["A"], "FD")
      mutableTree.setPrivateTag(0x07a1, "pdeman", 0x45, inImages[currentImage]["Positioning"]["P"], "FD")
      mutableTree.setPrivateTag(0x07a1, "pdeman", 0x46, inImages[currentImage]["Positioning"]["B"], "FD")
      mutableTree.setPrivateTag(0x07a1, "pdeman", 0x47, inImages[currentImage]["Positioning"]["T"], "FD")
      mutableTree.setPrivateTag(0x07a1, "pdeman", 0x48, inImages[currentImage]["Positioning"]["IHvox"], "FD")
      mutableTree.setPrivateTag(0x07a1, "pdeman", 0x49, inImages[currentImage]["Positioning"]["Avox"], "FD")
      mutableTree.setPrivateTag(0x07a1, "pdeman", 0x4a, inImages[currentImage]["Positioning"]["Pvox"], "FD")
      mutableTree.setPrivateTag(0x07a1, "pdeman", 0x4b, inImages[currentImage]["Positioning"]["Bvox"], "FD")
      mutableTree.setPrivateTag(0x07a1, "pdeman", 0x4c, inImages[currentImage]["Positioning"]["Tvox"], "FD")
      mutableTree.setPrivateTag(0x07a1, "pdeman", 0x4d, inImages[currentImage]['planeOrientation'],"SH")
      
      if ctx.field("FromFrontier").value:
        mutableTree.setPrivateTag(0x07a1, "pdeman", 0x42, inImages[currentImage]["SeriesInstanceUID"], "UI")
      
      ctx.field("SetDicomTreeOnImage.inDicomTree").setObject(mutableTree)
      
      if ctx.field("FromFrontier").value:
        ctx.connectField("parent:DicomExport.inImage","SetDicomTreeOnImage.output0")
      
      DicomToolToUse.field("exportNameTemplate").setStringValue("$S/"+"brainMask"+currentImage+"$T.dcm")
      DicomToolToUse.field("saveSlices").touch()
      print("DicomTool done")
      
      if currentImage in inImages.keys():
        inImages[currentImage].update({"mask":NameFull})
      
      ctx.field("AlreadyModifiedMaskReader.fileName").setStringValue(NameFull)
      
      ctx.field("AlreadyModifiedMask.currentInput").setValue(1)
      
      ctx.field("inImageInfos").setObject(inImages)
      ctx.field("outImagesInfosStep1").setObject(inImages)
      ctx.field("SoInteractionMapping1.ignoreOtherCommandActions").setBoolValue(False)
      ctx.field("adaptTemplateMask.SoToggleMaskEditor.on").setBoolValue(False)
      ctx.field("LabelViewerMode.text").setStringValue("Viewer Mode %s"%currentImage)
      
      valIm = currentImage.split('Image')[-1]
      if ctx.hasControl("GenerateBrainMask%s"%valIm):
        try:
          ctx.control("GenerateBrainMask%s"%valIm).setStyleSheetFromString('QPushButton { background-color: "blue"; }')
        except Exception as err:
          print("test background has control didn't work")
      else:
        global g_HorizontalControl
        try:
          g_HorizontalControl[imiter].control("GenerateBrainMask%s"%valIm).setStyleSheetFromString('QPushButton { background-color: "blue"; }')
        except Exception as err:
          print("test background g_horizontalcontrol didn't work")  
      
      
      ctx.control("checkImage%s"%valIm).setChecked(True)
      updateImage(currentImage)
      #else:
      #  ctx.field("parent:MessageBox.message").setStringValue("you have to precise the plane orientation to be able to save")
      #  ctx.parent().module("MessageBox").showModalDialog("dialog")
      #  return

      
      #we set a inImages object mask done to show the mask when updateDisplay
         
def modifyImageNumber(x):
  ctx.field("NumberImages").setValue(int(ctx.field("NumberImages").value)+x)
  global g_ImageOrientationGraphicsView
  inImages = ctx.field("inImageInfos").object()
  if inImages is not None:
    for ii in range(0,20):
      if "Image%i"%(ctx.field("NumberImages").value+ii) in inImages.keys():
        del inImages["Image%i"%(int(ctx.field("NumberImages").value))]
        
      
  ctx.field("inImageInfos").setObject(inImages)
  ctx.field("outImagesInfosStep1").setObject(inImages)
  initImageOrientationGraphicsView(g_ImageOrientationGraphicsView)
  #updateComboBox()
  #ctx.updateLayout()

def getHorizontalControl(image,horizon):
  
  #print("get %s control"%horizon)
  global g_HorizontalControl
  g_HorizontalControl.update({image:ctx.control(horizon)})
  
def resetBrainMask(Image):

  print("reset brain mask")
  inImages = ctx.field("inImageInfos").object()
  if "mask" in inImages[Image].keys():
    del inImages[Image]["mask"]
  
    valIm = Image.split('Image')[-1]
    if ctx.hasControl("GenerateBrainMask%s"%valIm):
      try:
        ctx.control("GenerateBrainMask%s"%valIm).setStyleSheetFromString('QPushButton { background-color: "white"; }')
      except Exception as err:
        print("test background has control didn't work")
    else:
      global g_HorizontalControl
      try:
        g_HorizontalControl[Image].control("GenerateBrainMask%s"%valIm).setStyleSheetFromString('QPushButton { background-color: "white"; }')
      except Exception as err:
        print("test background g_horizontalcontrol didn't work") 
  
  ctx.field("inImageInfos").setObject(inImages)
  ctx.field("outImagesInfosStep1").setObject(inImages)
  generateBrainMask(Image)
  
def updateCSOfromAdapt():
    ctx.field("adaptTemplateMask.updateCSOButton").touch()
    
def resetZoom():
   ctx.field("SoView2D.unzoom").touch()
   

def runAllFirstSetBackgroundTasks():
    
    global runAllFirstBackgroundTask
    global g_HorizontalControl
    runAllFirstBackgroundTask = True
    inImages = ctx.field("inImageInfos").object()
    global ImagesToDoBackgroundTasks
    
    if inImages == None:
      print("no images to work on")
      return

    #should set all outputSucceed to False (normally it's already done by the clear functions of the different ML module but it can't hurt)
    ctx.field("mialOrientImage.outputSucceed").setBoolValue(False)
    ctx.field("mialOrientImageNLM.outputSucceed").setBoolValue(False)
    ctx.field("mialOrientImageMask.outputSucceed").setBoolValue(False)
    ctx.field("mialCorrectSliceIntensity.outputSucceed").setBoolValue(False)
    ctx.field("mialCorrectSliceIntensityNLM.outputSucceed").setBoolValue(False)
    ctx.field("mialCorrectSliceIntensityNLMPostBiasCorrection.outputSucceed").setBoolValue(False)
    ctx.field("mialCorrectSliceIntensityPostBiasCorrection.outputSucceed").setBoolValue(False)
    ctx.field("mialSliceBySliceBiasEstimation.outputSucceed").setBoolValue(False)
    ctx.field("mialSliceBySliceBiasFieldCorrection.outputSucceed").setBoolValue(False)
    ctx.field("mialIntensityStandardization.outputSucceed").setBoolValue(False)
    ctx.field("mialIntensityStandardizationNLM.outputSucceed").setBoolValue(False)
    ctx.field("mialIntensityStandardizationNLMBis.outputSucceed").setBoolValue(False)
    ctx.field("mialIntensityStandardizationBis.outputSucceed").setBoolValue(False)
    ctx.field("mialHistogramNormalization.outputSucceed").setBoolValue(False)
    ctx.field("mialHistogramNormalizationNLM.outputSucceed").setBoolValue(False)
    ctx.field("mialImageReconstruction.outputSucceed").setBoolValue(False)
    ctx.field("mialsrtkMaskImage.outputSucceed").setBoolValue(False)
    ctx.field("insertOrientDone").setValue(False)
    ctx.field("insertOrientNLMDone").setValue(False)
    ctx.field("insertOrientMaskDone").setValue(False)
    ctx.field("FirstSDIDone").setValue(False)
    
    listImageToSendBackgroundTasks=[]
    for imageIter in inImages:
      if "Image" in imageIter:
        if g_HorizontalControl[imageIter].control("check%s"%imageIter).isChecked():
          #if ctx.control("check%s"%imageIter).isChecked():
          listImageToSendBackgroundTasks.append(imageIter)
        
    ImagesToDoBackgroundTasks = listImageToSendBackgroundTasks
    if "UsedFromStart" in inImages.keys():
      inImages.update({"UsedFromStart":list(set().union(ImagesToDoBackgroundTasks,inImages["UsedFromStart"]))})
    else:
      inImages.update({"UsedFromStart":ImagesToDoBackgroundTasks})
    #if !ctx.field("mevisbtkDenoising.outputSucceed").value
    #if denoise images as run on native image we have to reorient them
    SomeToDenoise=False
    for imageIter in listImageToSendBackgroundTasks:
      if 'NLMWorldChanged' in inImages[imageIter]:
        print("nothing to do")
      elif 'NLM' in inImages[imageIter]:
        ctx.field("updateWorldOrientation.itkImageFileReader.fileName").setStringValue(inImages[imageIter]['NLM'])
        ctx.field("updateWorldOrientation.itkImageFileReaderModel.fileName").setStringValue(inImages[imageIter]['WorldChanged'])
        newName=inImages[imageIter]['WorldChanged'].replace('.nii','_NLM.nii')
        ctx.field("updateWorldOrientation.itkImageFileWriter.fileName").setStringValue(newName)
        ctx.field("updateWorldOrientation.itkImageFileWriter.save").touch()
        #have to add to inImages now
        inImages[imageIter].update({'NLMWorldChanged':newName})

        
      else:
        SomeToDenoise=True
      
    if SomeToDenoise:
      denoiseImages(BackgroundTask=True,listImages = listImageToSendBackgroundTasks)
      return
        #denoiseImages(NextStep=runAllFirstSetBackgroundTasks)
    
    
    ImageToOrient=False
    for imageIter in listImageToSendBackgroundTasks:
      #if "ImReOriented" not in vv:
        ImageToOrient=True
    
    if ImageToOrient:
      OrientImages(WhatToOrient="Mask",listImages=listImageToSendBackgroundTasks)
      OrientImages(WhatToOrient="NativeImage",listImages=listImageToSendBackgroundTasks)
      OrientImages(WhatToOrient="Denoised",listImages=listImageToSendBackgroundTasks)
      

        
        
    ImageDenoisedToOrient=False
    ctx.field("inImageInfos").setObject(inImages)
    ctx.field("outImagesInfosStep1").setObject(inImages)       
    
    #while (not ctx.field("StopBackgroundTask").value):
    #  #Normalement denoise est deja fait
    #  for imageIter in inImages:
    #    if ctx.control("check%i"%imageIter).isChecked():
    #      if inputsDenoise != "":
    #        inputsDenoise = inputsDenoise+"--"
    #        outputsDenoise = outputsDenoise+"--"
    #      inputsDenoise=inputsDenoise+inImages[imageIter]["file"]
    #      newOutput = inImages[imageIter]["file"].replace('.nii','NLM.nii')
    #      outputsDenoise=outputsDenoise+newOutput
    #  #set all input file name with -- as delimeter
    #  ctx.field("mevisbtkDenoising.inputFileName").setStringValue(inputsDenoise)
    #  ctx.field("mevisbtkDenoising.outputFileName").setStringValue(outputsDenoise)
    #  ctx.field("mevisbtkDenoising.startTask").touch()
    #  i=i+1
        

def denoiseImages(BackgroundTask=True,listImages=None):

    
    inImages = ctx.field("inImageInfos").object()
    if inImages == None:
      print("no images to denoise")
      return
    
    if listImages is None:
      ImagestoDo = [keyImage for keyImage in list(inImages.keys()) if "Image" in keyImage]
    else:
      ImagestoDo = listImages
      
    inputsDenoise=""
    outputsDenoise=""
    #if worldmatrix exists we run denoise image on world matrix.
    #else on native
    checkWorldMatrix = True
    for imageIter in ImagestoDo:
       if "WorldChanged" not in inImages[imageIter]:
         checkWorldMatrix = False
    
    if checkWorldMatrix:
      print("run denoising on reoriented image directly")
    else:
      print("run denoising on native image, will have to reorient them when possible")
      
    for imageIter in ImagestoDo:
      
      if inputsDenoise != "":
        inputsDenoise = inputsDenoise+"--"
        outputsDenoise = outputsDenoise+"--"
      
      if checkWorldMatrix:
        inputsDenoise=inputsDenoise+inImages[imageIter]["WorldChanged"]
        newOutput = inImages[imageIter]["WorldChanged"].replace('.nii','_NLM.nii')
      else:
        inputsDenoise=inputsDenoise+inImages[imageIter]["file"]
        newOutput = inImages[imageIter]["file"].replace('.nii','_NLM.nii')
      
      outputsDenoise=outputsDenoise+newOutput
    #set all input file name with -- as delimeter
    ctx.field("mevisbtkDenoising.inputFileName").setStringValue(inputsDenoise)
    ctx.field("mevisbtkDenoising.outputFileName").setStringValue(outputsDenoise)
    
    if BackgroundTask:
      ctx.field("mevisbtkDenoising.startTask").touch()
    else:
      ctx.field("mevisbtkDenoising.startTaskModal").touch()

    #set all outpout file name with -- as delimeter
    


def OrientImages(WhatToOrient,listImages=None):
  
  inImages = ctx.field("inImageInfos").object()
  if inImages == None:
    print("no images to denoise")
    return
  
  if listImages is None:
      ImagestoDo = list(inImages.keys())
  else:
      ImagestoDo = listImages
      
  inputsOrient=""
  outputsOrient=""
  inputOrientation=""
  
  if WhatToOrient=="NativeImage":
    WhichMial = "mialOrientImage"
  elif WhatToOrient=="Mask":
    WhichMial = "mialOrientImageMask"
  elif WhatToOrient=="Denoised":
    WhichMial = "mialOrientImageNLM"
    
  for imageIter in ImagestoDo:
     
    if inputsOrient != "":
      inputsOrient = inputsOrient+"--"
      outputsOrient = outputsOrient+"--"
      inputOrientation = inputOrientation+"--"
    
    if WhatToOrient=="NativeImage":
     if "WorldChanged" in inImages[imageIter]:
        if "planeOrientation" in inImages[imageIter]:  
          inputsOrient=inputsOrient+inImages[imageIter]["WorldChanged"]
          newOutput = inImages[imageIter]["WorldChanged"].replace('.nii','_reoriented.nii')
          outputsOrient=outputsOrient+newOutput
          inputOrientation=inputOrientation+inImages[imageIter]["planeOrientation"]
    elif WhatToOrient=="Mask":
      if "planeOrientation" in inImages[imageIter]:  
          inputsOrient=inputsOrient+inImages[imageIter]["mask"]
          newOutput = inImages[imageIter]["mask"].replace('.nii','_reoriented.nii')
          outputsOrient=outputsOrient+newOutput
          inputOrientation=inputOrientation+inImages[imageIter]["planeOrientation"]
    elif WhatToOrient=="Denoised":
      if "planeOrientation" in inImages[imageIter]:  
          inputsOrient=inputsOrient+inImages[imageIter]["NLMWorldChanged"]
          newOutput = inImages[imageIter]["NLMWorldChanged"].replace('.nii','_reoriented.nii')
          outputsOrient=outputsOrient+newOutput
          inputOrientation=inputOrientation+inImages[imageIter]["planeOrientation"]
          
  ctx.field("%s.inputFileName"%WhichMial).setStringValue(inputsOrient)
  ctx.field("%s.outputFileName"%WhichMial).setStringValue(outputsOrient)
  ctx.field("%s.orientation"%WhichMial).setStringValue(inputOrientation)
  print("WhichMial : %s"%WhichMial)
  ctx.field("%s.startTask"%WhichMial).touch()
  
  MLAB.processEvents()
  
def insertReOrient(WhatToInsert):
  
  global ImagesToDoBackgroundTasks 
  
  if WhatToInsert=="RawImage":  
    if not ctx.field("mialOrientImage.outputSucceed").value:
      return
  elif WhatToInsert=="MaskImage":
    if not ctx.field("mialOrientImageMask.outputSucceed").value:
      return    
  elif WhatToInsert=="NLMImage":
    if not ctx.field("mialOrientImageNLM.outputSucceed").value:
      return  
  inImages = ctx.field("inImageInfos").object()
  if inImages == None:
    print("no images to denoise")
    return
  
  for imageIter in ImagesToDoBackgroundTasks:
    if WhatToInsert=="RawImage":  
      inImages[imageIter].update({"ImReOriented":inImages[imageIter]["WorldChanged"].replace('.nii','_reoriented.nii')})
    elif WhatToInsert=="MaskImage":
      inImages[imageIter].update({"MaskReOriented":inImages[imageIter]["mask"].replace('.nii','_reoriented.nii')})
    elif WhatToInsert=="NLMImage":
      inImages[imageIter].update({"ImNLMReOriented":inImages[imageIter]["NLMWorldChanged"].replace('.nii','_reoriented.nii')})
   
   
  ctx.field("inImageInfos").setObject(inImages)
  ctx.field("outImagesInfosStep1").setObject(inImages)
  
  #if WhatToInsert=="RawImage": 
  #  runCorrectSliceIntensity('RawImage')
  #
  #if WhatToInsert=="NLMImage": 
  #  runCorrectSliceIntensity('NLMImage')
  if WhatToInsert=="RawImage": 
    ctx.field("insertOrientDone").setValue(True)
  elif WhatToInsert=="NLMImage":  
    ctx.field("insertOrientNLMDone").setValue(True)
  elif WhatToInsert=="MaskImage":
    ctx.field("insertOrientMaskDone").setValue(True)

  WaitForCorrectSliceIntensity()
  
def WaitForCorrectSliceIntensity():
  
  ctx.field("WaitForSliceIntensity").setBoolValue(ctx.field("insertOrientDone").value & ctx.field("insertOrientNLMDone").value & ctx.field("insertOrientMaskDone").value)
  print(ctx.field("insertOrientDone").value)
  print(ctx.field("insertOrientNLMDone").value)
  print(ctx.field("insertOrientMaskDone").value)
  print("bool value")
  print(ctx.field("WaitForSliceIntensity").value)
  print("end bool value")
  if not ctx.field("WaitForSliceIntensity").value:
    MLAB.processEvents()
    return
  else:
    print("run SliceIntensity")
    print(ctx.field("mialOrientImageMask.outputSucceed").value)
    print(ctx.field("mialOrientImage.outputSucceed").value)
    print(ctx.field("mialOrientImageNLM.outputSucceed").value)
    runCorrectSliceIntensity('RawImage')
    runCorrectSliceIntensity('NLMImage')

def runCorrectSliceIntensity(WhatToCorrect):
  
  print("HEEEEERRRRRRREEEEE")
  global ImagesToDoBackgroundTasks
  
  inImages = ctx.field("inImageInfos").object()
  if inImages == None:
    print("no images to denoise")
    return
  
  inputsCorrectSliceintensity=""
  masksCorrectSliceintensity=""
  outputsCorrectSliceintensity=""
  
  if WhatToCorrect=="RawImage":
    WhichCorrectSlice="mialCorrectSliceIntensity"
  elif WhatToCorrect=="NLMImage":
    WhichCorrectSlice="mialCorrectSliceIntensityNLM"
  elif WhatToCorrect=="NLMPostBiasCorrection":
    WhichCorrectSlice="mialCorrectSliceIntensityNLMPostBiasCorrection"
  elif WhatToCorrect=="PostBiasCorrection":
    WhichCorrectSlice="mialCorrectSliceIntensityPostBiasCorrection"
    
  for imageIter in ImagesToDoBackgroundTasks:
     
    if inputsCorrectSliceintensity != "":
      inputsCorrectSliceintensity = inputsCorrectSliceintensity+"--"
      masksCorrectSliceintensity = masksCorrectSliceintensity+"--"
      outputsCorrectSliceintensity = outputsCorrectSliceintensity+"--"
      
    if WhatToCorrect=="RawImage":
      inputsCorrectSliceintensity = inputsCorrectSliceintensity+inImages[imageIter]["ImReOriented"]
      masksCorrectSliceintensity = masksCorrectSliceintensity+inImages[imageIter]["MaskReOriented"]
      outputsCorrectSliceintensity = outputsCorrectSliceintensity+inImages[imageIter]["ImReOriented"].replace(".nii","_uni.nii")
    elif WhatToCorrect=="NLMImage":
      inputsCorrectSliceintensity = inputsCorrectSliceintensity+inImages[imageIter]["ImNLMReOriented"]
      masksCorrectSliceintensity = masksCorrectSliceintensity+inImages[imageIter]["MaskReOriented"]
      outputsCorrectSliceintensity = outputsCorrectSliceintensity+inImages[imageIter]["ImNLMReOriented"].replace(".nii","_uni.nii")
    elif WhatToCorrect=="NLMPostBiasCorrection":
      inputsCorrectSliceintensity = inputsCorrectSliceintensity+inImages[imageIter]["NLMBCorr"]
      masksCorrectSliceintensity = masksCorrectSliceintensity+inImages[imageIter]["MaskReOriented"]
      outputsCorrectSliceintensity = outputsCorrectSliceintensity+inImages[imageIter]["NLMBCorr"]     
    elif WhatToCorrect=="PostBiasCorrection":  
      inputsCorrectSliceintensity = inputsCorrectSliceintensity+inImages[imageIter]["BCorr"]
      masksCorrectSliceintensity = masksCorrectSliceintensity+inImages[imageIter]["MaskReOriented"]
      outputsCorrectSliceintensity = outputsCorrectSliceintensity+inImages[imageIter]["BCorr"]
      
      
  ctx.field("%s.inputFileName"%WhichCorrectSlice).setStringValue(inputsCorrectSliceintensity)
  ctx.field("%s.outputFileName"%WhichCorrectSlice).setStringValue(outputsCorrectSliceintensity)
  ctx.field("%s.maskFileName"%WhichCorrectSlice).setStringValue(masksCorrectSliceintensity)
  ctx.field("%s.startTask"%WhichCorrectSlice).touch()
  
def inserCorrectSliceIntensity(WhatToInsert):
  
  global ImagesToDoBackgroundTasks
  
  if WhatToInsert=="RawImage":  
    if not ctx.field("mialCorrectSliceIntensity.outputSucceed").value:
      return
  elif WhatToInsert=="NLMImage":  
    if not ctx.field("mialCorrectSliceIntensityNLM.outputSucceed").value:
      return
  elif WhatToInsert=="NLMPostBiasCorrection":
    if not ctx.field("mialCorrectSliceIntensityNLMPostBiasCorrection.outputSucceed").value:
      return    
  elif WhatToInsert=="PostBiasCorrection":  
    if not ctx.field("mialCorrectSliceIntensityPostBiasCorrection.outputSucceed").value:
      return
    
  inImages = ctx.field("inImageInfos").object()
  if inImages == None:
    print("no images to denoise")
    return
  
  for imageIter in ImagesToDoBackgroundTasks: 
    if WhatToInsert=="RawImage":
      inImages[imageIter].update({"FirstUni":inImages[imageIter]["ImReOriented"].replace(".nii","_uni.nii")})
    elif WhatToInsert=="NLMImage":
      inImages[imageIter].update({"FirstNLMUni":inImages[imageIter]["ImNLMReOriented"].replace(".nii","_uni.nii")})
    #elif WhatToInsert=="NLMPostBiasCorrection":
    #  print("nothing to insert")
    #elif WhatToInsert=="PostBiasCorrection":   
    #  print("nothing to insert")
  ctx.field("inImageInfos").setObject(inImages)
  ctx.field("outImagesInfosStep1").setObject(inImages)
  
  if WhatToInsert=="NLMImage":
    runSliceBySliceBiasEstimation()
  elif WhatToInsert=="NLMPostBiasCorrection":
    runIntensityStandardization("NLMPostBiasCorrection")
  elif WhatToInsert=="PostBiasCorrection":
    runIntensityStandardization("PostBiasCorrection")
    
  MLAB.processEvents()  

def runSliceBySliceBiasEstimation():
  
  global ImagesToDoBackgroundTasks
  
  inImages = ctx.field("inImageInfos").object()
  if inImages == None:
    print("no images to denoise")
    return
  
  inputsBiasEstimation=""
  masksBiasEstimation=""
  outputsBiasEstimation=""
  fieldBiasEstimation=""
   
  for imageIter in ImagesToDoBackgroundTasks:
     
    if inputsBiasEstimation != "":
      inputsBiasEstimation = inputsBiasEstimation+"--"
      masksBiasEstimation = masksBiasEstimation+"--"
      outputsBiasEstimation = outputsBiasEstimation+"--"
      fieldBiasEstimation = fieldBiasEstimation+"--"
      
    if "FirstNLMUni" in inImages[imageIter]:
      inputsBiasEstimation = inputsBiasEstimation + inImages[imageIter]["FirstNLMUni"]
      masksBiasEstimation = masksBiasEstimation + inImages[imageIter]["MaskReOriented"]
      outputsBiasEstimation = outputsBiasEstimation + inImages[imageIter]["FirstNLMUni"].replace(".nii","_bcorr.nii")
      fieldBiasEstimation = fieldBiasEstimation + inImages[imageIter]["FirstNLMUni"].replace(".nii","_n4bias.nii")
      
  
  ctx.field("mialSliceBySliceBiasEstimation.inputImageFile").setStringValue(inputsBiasEstimation)
  ctx.field("mialSliceBySliceBiasEstimation.inputMaskFile").setStringValue(masksBiasEstimation)
  ctx.field("mialSliceBySliceBiasEstimation.outputImageFile").setStringValue(outputsBiasEstimation)
  ctx.field("mialSliceBySliceBiasEstimation.outputBiasFieldFile").setStringValue(fieldBiasEstimation)
  ctx.field("mialSliceBySliceBiasEstimation.startTask").touch()
  
def insertSliceBiasEstimation():
  
  global ImagesToDoBackgroundTasks
  
  if not ctx.field("mialSliceBySliceBiasEstimation.outputSucceed").value:
    return
    
  inImages = ctx.field("inImageInfos").object()
  if inImages == None:
    print("no images to denoise")
    return
  
  for imageIter in ImagesToDoBackgroundTasks:
    inImages[imageIter].update({"NLMBCorr":inImages[imageIter]["FirstNLMUni"].replace(".nii","_bcorr.nii")})
    inImages[imageIter].update({"BiasField":inImages[imageIter]["FirstNLMUni"].replace(".nii","_n4bias.nii")})

  ctx.field("inImageInfos").setObject(inImages)
  ctx.field("outImagesInfosStep1").setObject(inImages)
  
  runSliceBySliceBiasFieldCorrection()
  MLAB.processEvents()

def runSliceBySliceBiasFieldCorrection():
  
  global ImagesToDoBackgroundTasks
  
  inImages = ctx.field("inImageInfos").object()
  if inImages == None:
    print("no images to denoise")
    return
  
  inImages = ctx.field("inImageInfos").object()
  if inImages == None:
    print("no images to denoise")
    return
  
  inputsBiasCorrection=""
  masksBiasCorrection=""
  fieldBiasCorrection=""
  outputBiasCorrection=""
  
  for imageIter in ImagesToDoBackgroundTasks:
     
    if inputsBiasCorrection != "":
      inputsBiasCorrection = inputsBiasCorrection+"--"
      masksBiasCorrection = masksBiasCorrection+"--"
      fieldBiasCorrection = fieldBiasCorrection+"--"
      outputBiasCorrection = outputBiasCorrection+"--"
      
    if "FirstUni" in inImages[imageIter]:
      inputsBiasCorrection = inputsBiasCorrection+inImages[imageIter]["FirstUni"]
      masksBiasCorrection = masksBiasCorrection+inImages[imageIter]["MaskReOriented"]
      fieldBiasCorrection = fieldBiasCorrection+inImages[imageIter]["BiasField"]
      outputBiasCorrection = outputBiasCorrection+inImages[imageIter]["FirstUni"].replace(".nii","_bcorr.nii")
   
  ctx.field("mialSliceBySliceBiasFieldCorrection.inputImageFile").setStringValue(inputsBiasCorrection)
  ctx.field("mialSliceBySliceBiasFieldCorrection.inputMaskFile").setStringValue(masksBiasCorrection)
  ctx.field("mialSliceBySliceBiasFieldCorrection.outputImageFile").setStringValue(outputBiasCorrection)
  ctx.field("mialSliceBySliceBiasFieldCorrection.inputBiasFieldFile").setStringValue(fieldBiasCorrection)
  ctx.field("mialSliceBySliceBiasFieldCorrection.startTask").touch()
  
def insertSliceBiasCorrection():  
  
  global ImagesToDoBackgroundTasks
  
  inImages = ctx.field("inImageInfos").object()
  
  if not ctx.field("mialSliceBySliceBiasFieldCorrection.outputSucceed").value:
    return
  
  for imageIter in ImagesToDoBackgroundTasks:
    inImages[imageIter].update({"BCorr":inImages[imageIter]["FirstUni"].replace(".nii","_bcorr.nii")})
    
  ctx.field("inImageInfos").setObject(inImages)
  ctx.field("outImagesInfosStep1").setObject(inImages)
  
  runCorrectSliceIntensity("NLMPostBiasCorrection")
  runCorrectSliceIntensity("PostBiasCorrection")
  MLAB.processEvents()


def runIntensityStandardization(WhatToRun):
  
  global ImagesToDoBackgroundTasks
  print("runIntensityStandardization")
  print(WhatToRun)
  inImages = ctx.field("inImageInfos").object()
  
  
  if WhatToRun == "NLMPostBiasCorrection":
     WhichIntensityStandardization="mialIntensityStandardizationNLM"
  elif WhatToRun == "PostBiasCorrection":
    WhichIntensityStandardization="mialIntensityStandardization"
  elif WhatToRun == "postHistoNorm":
    WhichIntensityStandardization="mialIntensityStandardizationBis"
  elif WhatToRun == "postHistoNormNLM":  
    WhichIntensityStandardization="mialIntensityStandardizationNLMBis"
    
  inputsIntensityStandardization=""
  outputsIntensityStandardization=""
  
  for imageIter in ImagesToDoBackgroundTasks:
    if inputsIntensityStandardization!="":
      inputsIntensityStandardization=inputsIntensityStandardization+"--"
      outputsIntensityStandardization=outputsIntensityStandardization+"--"
      
    if WhatToRun == "NLMPostBiasCorrection" or WhatToRun == "postHistoNormNLM": 
      inputsIntensityStandardization=inputsIntensityStandardization+inImages[imageIter]["NLMBCorr"]
      outputsIntensityStandardization=outputsIntensityStandardization+inImages[imageIter]["NLMBCorr"]
    elif WhatToRun == "PostBiasCorrection" or WhatToRun == "postHistoNorm":
      inputsIntensityStandardization=inputsIntensityStandardization+inImages[imageIter]["BCorr"]
      outputsIntensityStandardization=outputsIntensityStandardization+inImages[imageIter]["BCorr"]
      
  ctx.field("%s.inputFileNames"%WhichIntensityStandardization).setStringValue(inputsIntensityStandardization)
  ctx.field("%s.outputFileNames"%WhichIntensityStandardization).setStringValue(outputsIntensityStandardization)
  ctx.field("%s.startTask"%WhichIntensityStandardization).touch()
    

def runHistogramNormalization(WhatToRun):
  
  
  if WhatToRun=="RawImage":
    if not ctx.field("mialIntensityStandardization.outputSucceed").value:
      return
  
  if WhatToRun=="DenoisedImage":
    if not ctx.field("mialIntensityStandardizationNLM.outputSucceed").value:
      return   
  
  if WhatToRun=="RawImage":
    WHichHistoNormToRun="mialHistogramNormalization"
  elif WhatToRun=="DenoisedImage":
    WHichHistoNormToRun="mialHistogramNormalizationNLM"
    
  print(WHichHistoNormToRun)
  ctx.field("%s.validate"%WHichHistoNormToRun).touch()
  MLAB.processEvents()
  #ctx.module(WHichHistoNormToRun).call("runHistoNormalization")
  
def insertHistogramNormalization(WhatToRun):
 
  print("insert HistoNormal")
  print(WhatToRun)
  
  if WhatToRun == "RawImages":
    if not ctx.field("mialHistogramNormalization.outputSucceed").value:
      print("problem here ?")
      return
  if WhatToRun == "DenoisedImage":
    if not ctx.field("mialHistogramNormalizationNLM.outputSucceed").value:
      return
  
  if WhatToRun=="RawImages":
      print("RawImages?")
      runIntensityStandardization("postHistoNorm")
  elif WhatToRun=="DenoisedImage":
      runIntensityStandardization("postHistoNormNLM")
   
  MLAB.processEvents()

def updateBackgroundTaskStatus(Task):
  
  listOfPossibleTask = {"Denoising":ctx.field("mevisbtkDenoising.status"),"mialOrientImage":ctx.field("mialOrientImage.status"),"mialCorrectSliceIntensity":ctx.field("mialCorrectSliceIntensity.status"),"mialBiasEstimation":ctx.field("mialSliceBySliceBiasEstimation.status"),"mialBiasCorrection":ctx.field("mialSliceBySliceBiasFieldCorrection.status"),"mialIntensityStandardization":ctx.field("mialIntensityStandardization.status"),"mialImageReconstruction":ctx.field("mialImageReconstruction.status")}
  
  newMessage =listOfPossibleTask[Task].stringValue()
  g_HorizontalControl["LastButtons"].control("StatusField").setTitle(newMessage)
  
  if Task=="mialCorrectSliceIntensity":
    checkBackgroundTask()
  
def updateBackgroundTaskRunningField():
  
  ctx.field("BackgroundTaskRunning").setBoolValue(ctx.field("mevisbtkDenoising.inProgress").value | ctx.field("mialOrientImage.inProgress").value | ctx.field("mialCorrectSliceIntensity.inProgress").value | ctx.field("mialSliceBySliceBiasEstimation.inProgress").value | ctx.field("mialSliceBySliceBiasFieldCorrection.inProgress").value | ctx.field("mialIntensityStandardization.inProgress").value | ctx.field("mialOrientImageMask.inProgress").value | ctx.field("mialOrientImageNLM.inProgress").value | ctx.field("mialCorrectSliceIntensityNLM.inProgress").value |ctx.field("mialCorrectSliceIntensityNLMPostBiasCorrection.inProgress").value | ctx.field("mialIntensityStandardizationNLM.inProgress").value | ctx.field("mialCorrectSliceIntensityPostBiasCorrection.inProgress").value | ctx.field("mialImageReconstruction.inProgress").value )
  print(ctx.field("BackgroundTaskRunning").value)
 
def checkBackgroundTask():
  
  listOfPossibleTaskProgress = {"Denoising":ctx.field("mevisbtkDenoising.inProgress").value,"mialOrientImage":ctx.field("mialOrientImage.inProgress").value,"mialCorrectSliceIntensity":ctx.field("mialCorrectSliceIntensity.inProgress").value,"mialBiasEstimation":ctx.field("mialSliceBySliceBiasEstimation.inProgress").value,"mialBiasCorrection":ctx.field("mialSliceBySliceBiasFieldCorrection.inProgress").value,"mialIntensityStandardization":ctx.field("mialIntensityStandardization.inProgress").value,"mialImageReconstruction":ctx.field("mialImageReconstruction.inProgress").value}
  listOfPossibleTaskStatus = {"Denoising":ctx.field("mevisbtkDenoising.status"),"mialOrientImage":ctx.field("mialOrientImage.status"),"mialCorrectSliceIntensity":ctx.field("mialCorrectSliceIntensity.status"),"mialBiasEstimation":ctx.field("mialSliceBySliceBiasEstimation.status"),"mialBiasCorrection":ctx.field("mialSliceBySliceBiasFieldCorrection.status"),"mialIntensityStandardization":ctx.field("mialIntensityStandardization.status"),"mialImageReconstruction":ctx.field("mialImageReconstruction.status")}
  
  for iterTask in listOfPossibleTaskProgress.keys():
    if listOfPossibleTaskProgress[iterTask]:
      newMessage =listOfPossibleTaskStatus[iterTask].stringValue()
      g_HorizontalControl["LastButtons"].control("StatusField").setTitle(newMessage)

def runMaskImage():
  
  if not ctx.field("mialIntensityStandardizationNLMBis.outputSucceed").value:
    return
  
  ctx.field("mialsrtkMaskImage.startTask").touch()

def updateWaitForReconstrucion():
  
  ctx.field("WaitForReconstruction").setBoolValue(ctx.field("mialsrtkMaskImage.outputSucceed").value & ctx.field("mialIntensityStandardizationBis.outputSucceed").value)
  print("updateWaitForREconstruction")
  print(ctx.field("WaitForReconstruction").value)
  MLAB.processEvents()
  
def runImageReconstruction():
  
  global ImagesToDoBackgroundTasks
  print("run Image Reconstruction")
  if not ctx.field("WaitForReconstruction").value :
    print("not here :(")
    return
      
  inImages = ctx.field("inImageInfos").object()
  
  numIm = ctx.field("NumberImages").value
  outTransform = ""
  inputFiles = ""
  maskFiles = ""
  
  orderList = {};
  for imageIter in ImagesToDoBackgroundTasks:
    orderList.update({int(g_HorizontalControl[imageIter].control("Label%s"%imageIter).title().split(":")[-1]):imageIter})
  
  sorted_orderList = sorted(orderList.items(),  key=lambda kv: kv[0])
  sorted_orderListValues = [vv for (kk,vv) in sorted_orderList]
  inImages.update({"UsedForSDI":sorted_orderListValues})
  for iterList in range(len(sorted_orderList)):
    if sorted_orderList[iterList][1] in ImagesToDoBackgroundTasks:
      if outTransform!="":
        outTransform=outTransform+"--"
        inputFiles=inputFiles+"--"
        maskFiles=maskFiles+"--"
      
      outTransform = outTransform +inImages[sorted_orderList[iterList][1]]["ImReOriented"].split(".nii")[0]+"_transform_%iV_1.txt"%len(inImages["UsedForSDI"])
      inputFiles = inputFiles + inImages[sorted_orderList[iterList][1]]["NLMBCorr"]
      maskFiles=maskFiles + inImages[sorted_orderList[iterList][1]]["MaskReOriented"]

  
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
    inImages[imageIter].update({"Transform":inImages[imageIter]["ImReOriented"].split(".nii")[0]+"_transform_%iV_1.txt"%len(inImages["UsedForSDI"])})
  
  ctx.field("inImageInfos").setObject(inImages)
  ctx.field("outImagesInfosStep1").setObject(inImages)
  
  ctx.field("FirstSDIDone").setValue(True)
  ctx.field("outputSucceed").setObject(ctx.field("FirstSDIDone"))
  
  ##we convert them to dicom as well:
  print("convert SDI To Dicom")
  ctx.field("NiftiToDicomFetalMRI.itkImageFileReader.fileName").setStringValue(inImages["SDI_ITER1"])
  ctx.field("NiftiToDicomFetalMRI.DicomTagModify.tagValue0").setValue("SDI_ITER1")
   
  if ctx.field("FromFrontier").value:
    #we use dicom tool from testinstall, dicomSend
    print("via Frontier")
    ctx.field("NiftiToDicomFetalMRI.DicomTagModify.tagValue1").setValue(inImages["Image0"]["StudyDescription"])
    ctx.field("NiftiToDicomFetalMRI.DicomTagModify.tagValue2").setValue(inImages["Image0"]["PatientName"])
    ctx.field("NiftiToDicomFetalMRI.DicomTagModify.tagValue3").setValue(inImages["Image0"]["PatientID"])
    ctx.field("NiftiToDicomFetalMRI.DicomTagModify1.tagValue1").setValue(time.strftime("%H%M%S"))
    ctx.field("NiftiToDicomFetalMRI.DicomTagModify1.tagValue0").setValue(time.strftime("%Y%m%d"))
    ctx.field("NiftiToDicomFetalMRI.DicomTagModify.apply").touch()
    originalTree = ctx.field("NiftiToDicomFetalMRI.SetDicomTreeOnImage.input0").getDicomTree()
    mutableTree = originalTree.createDerivedTree()
    _frontier = ctx.module("parent:FrontierSyngoInterface").object()
    ctx.field("parent:DicomExport.exportBaseDir").setStringValue(_frontier.getOutgoingDicomDirectory())
    DicomToolToUse = ctx.module("parent:DicomExport") #ctx.module("DicomTool") #
    print(DicomToolToUse.field("exportBaseDir").value)
    listUID = [inImages[imageIter]["SeriesInstanceUID"] for imageIter in inImages["UsedForSDI"]]
    transfoInfo = [open(inImages[imageIter]["Transform"],"r").read() for imageIter in inImages["UsedForSDI"]]
    mutableTree.setPrivateTag(0x07a1, "pdeman", 0x44, transfoInfo, "UT")
    mutableTree.setPrivateTag(0x07a1, "pdeman", listUID , "UI")
    mutableTree.setPrivateTag(0x07a1, "pdeman", 0x42, 1 , "SS")
    #ctx.connectField("parent:DicomExport.inImage","DicomTagModify.output0")
    
  else:
    #we use dicom tool from TotalVariationInterface, dicomSave
    print("not via Frontier")
    ctx.field("NiftiToDicomFetalMRI.DicomTagModify1.tagValue1").setValue(time.strftime("%H%M%S"))
    ctx.field("NiftiToDicomFetalMRI.DicomTagModify1.tagValue0").setValue(time.strftime("%Y%m%d"))
    ctx.field("NiftiToDicomFetalMRI.DicomTagModify.apply").touch()
    originalTree = ctx.field("NiftiToDicomFetalMRI.SetDicomTreeOnImage.input0").getDicomTree()
    mutableTree = originalTree.createDerivedTree()
    mutableTree.setPrivateTag(0x07a1, "pdeman", 0x43, inImages["UsedForSDI"] , "LO")
    mutableTree.setPrivateTag(0x07a1, "pdeman", 0x42, 1 , "SS")
    transfoInfo = [open(inImages[imageIter]["Transform"],"r").read() for imageIter in inImages["UsedForSDI"]]
    mutableTree.setPrivateTag(0x07a1, "pdeman", 0x44, transfoInfo, "UT")
    DicomToolToUse = ctx.module("DicomToolSDI1")
    ctx.field("DicomToolSDI1.exportBaseDir").setStringValue(os.path.join(os.path.dirname(inImages["Image0"]["file"]),"Results"))
    
  
  ctx.field("NiftiToDicomFetalMRI.SetDicomTreeOnImage.inDicomTree").setObject(mutableTree)
  
  if ctx.field("FromFrontier").value:
    ctx.connectField("parent:DicomExport.inImage","NiftiToDicomFetalMRI.SetDicomTreeOnImage.output0")
  
  DicomToolToUse.field("exportNameTemplate").setStringValue("$S/"+"SDI_ITER1"+"$T.dcm")
  DicomToolToUse.field("saveSlices").touch()
  
  MLAB.processEvents()
  #print("DicomTool done")

def insertNLMDenoisingResults():
  
  global runAllFirstBackgroundTask
  global ImagesToDoBackgroundTasks
  
  if not ctx.field("mevisbtkDenoising.outputSucceed").value:
    #print("denoising failed")
    #runAllFirstBackgroundTask = False
    return
  
  getFileName = ctx.field("mevisbtkDenoising.outputFileName").stringValue()
  getInputFileName = ctx.field("mevisbtkDenoising.inputFileName").stringValue()
  
  splitNames = getFileName.split("--")
  splitInputNames = getInputFileName.split("--")

  if splitNames[0].find("_worldmatrixModified")>=0:
    print("denoised images already in the good referential")
    insertNLMOriented=True     
  else:
    print("will have to reorient latter")
    insertNLMOriented= False 
    
  inImages = ctx.field("inImageInfos").object()
  for iterImage in range(len(splitNames)):
    for kk,vv in inImages.items():
      if "Image" in kk:
        if insertNLMOriented:
          if splitInputNames[iterImage]==vv['WorldChanged']:
            inImages[kk].update({'NLMWorldChanged':splitNames[iterImage]})
        else:
          if splitInputNames[iterImage]==vv['file']:
             inImages[kk].update({'NLM':splitNames[iterImage]})
          
  ctx.field("inImageInfos").setObject(inImages)
  ctx.field("outImagesInfosStep1").setObject(inImages)
  
  if runAllFirstBackgroundTask:
    runAllFirstSetBackgroundTasks()

    
def updateRefImageReconstruction():

  ctx.field("mialImageReconstruction.ImageBaseOfRecon").setIntValue(ctx.field("RefImageFiled").value)
  


def updateParameterRegistration():
  
  #ctx.field("mialImageReconstruction.MetricToUse")
  print("update Registration parameters")


#can't do one function dealing with every condition, have to split in functions depending on which process is done
#def nextStepBackgroundTask(Task):
#  
#  listOfPossibleTask = {"Denoising":ctx.field("mevisbtkDenoising.startTask"),"mialOrientImage":ctx.field("mialOrientImage.startTask"),"mialCorrectSliceIntensity":ctx.field("mialCorrectSliceIntensity.startTask"),"mialBiasEstimation":ctx.field("mialSliceBySliceBiasEstimation.startTask"),"mialBiasCorrection":ctx.field("mialSliceBySliceBiasFieldCorrection.startTask"),"mialIntensityStandardization":ctx.field("mialIntensityStandardization.startTask")}
#  listOfPossibleTask[Task].touch()

def showHelp():
  print("showHelp")
  #if not ctx.field("FromFrontier").value:
  import webbrowser
  print(webbrowser.browser)
  print(MLABFileManager.exists(ctx.expandFilename("$(MLAB_mevisFetalMRI_MRUser)/Documentation/Publish/ModuleReference/ImageOrientationInterface.html")))
  webbrowser.open_new(ctx.expandFilename("$(MLAB_mevisFetalMRI_MRUser)/Documentation/Publish/ModuleReference/ImageOrientationInterface.html"))

  #else:
  #  global _frontier
  #  _frontier = ctx.module("parent:FrontierSyngoInterface").object()
  #  url = "/".join(["frontier_server", "user_manuals", ctx.field("parent:FrontierSyngoInterface.applicationName").value])
  #  print(url)
  #  #url = ctx.expandFilename("$(MLAB_mevisFetalMRI_MRUser)/Documentation/Publish/ModuleReference/ImageOrientationInterface.html")
  #  _frontier._syngoVia.call("FE.AppHosting.ShowUrl", url)

def loadPreProcessedAlready():
  global ToggleLoadPreviousData
  if ToggleLoadPreviousData==0:
    ToggleLoadPreviousData = 1
    g_HorizontalControl['ExpertButtons4'].control("buttonLoadPreProcessed").setTitle("Load Previous Data")
  elif ToggleLoadPreviousData==1:
    ToggleLoadPreviousData = 0
    g_HorizontalControl['ExpertButtons4'].control("buttonLoadPreProcessed").setTitle("Load Raw Data")


class temporaryObject():
  def __init__(self):
    self.tempCSOs= {}
  
  def getTempCSOs(self):
    self.tempCSOs.update({currentImage:ctx.field("adaptTemplateMask.CSOManager1.outCSOList").object()})
    
  def setTempCSOs(self):
    ctx.field("adaptTemplateMask.CSOManager1.outCSOList").setObject(self.tempCSOs[currentImage])
    