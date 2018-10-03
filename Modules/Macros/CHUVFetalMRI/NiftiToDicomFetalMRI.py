# ----------------------------------------------------------------------------

# 
#  \file    NiftiToDicomFetalMRI.py
#  \author  FetalMRI
#  \date    2018-09-26
#
#  

# ----------------------------------------------------------------------------

from mevis import *


def DoConversion():
  print("convert Mask To Dicom")
  #ctx.field("DicomTagModify.tagValue0").setValue("BrainMask")
  #ctx.field("DicomTagModify.tagValue1").setValue(inImages[currentImage]["StudyDescription"])
  #ctx.field("DicomTagModify.tagValue2").setValue(inImages[currentImage]["PatientName"])
  #ctx.field("DicomTagModify.tagValue3").setValue(inImages[currentImage]["PatientID"])
  #ctx.field("DicomTagModify.apply").touch()
  #originalTree = ctx.field("SetDicomTreeOnImage.input0").getDicomTree()
  #mutableTree = originalTree.createDerivedTree()
  #mutableTree.setPrivateTag(0x07a1, "pdeman", 0x43, inImages[currentImage]["Positioning"]["IH"], "FD")
  #mutableTree.setPrivateTag(0x07a1, "pdeman", 0x44, inImages[currentImage]["Positioning"]["A"], "FD")
  #mutableTree.setPrivateTag(0x07a1, "pdeman", 0x45, inImages[currentImage]["Positioning"]["P"], "FD")
  #mutableTree.setPrivateTag(0x07a1, "pdeman", 0x46, inImages[currentImage]["Positioning"]["B"], "FD")
  #mutableTree.setPrivateTag(0x07a1, "pdeman", 0x47, inImages[currentImage]["Positioning"]["T"], "FD")
  #mutableTree.setPrivateTag(0x07a1, "pdeman", 0x48, inImages[currentImage]["Positioning"]["IHvox"], "FD")
  #mutableTree.setPrivateTag(0x07a1, "pdeman", 0x49, inImages[currentImage]["Positioning"]["Avox"], "FD")
  #mutableTree.setPrivateTag(0x07a1, "pdeman", 0x4a, inImages[currentImage]["Positioning"]["Pvox"], "FD")
  #mutableTree.setPrivateTag(0x07a1, "pdeman", 0x4b, inImages[currentImage]["Positioning"]["Bvox"], "FD")
  #mutableTree.setPrivateTag(0x07a1, "pdeman", 0x4c, inImages[currentImage]["Positioning"]["Tvox"], "FD")
  #mutableTree.setPrivateTag(0x07a1, "pdeman", 0x4d, inImages[currentImage]['planeOrientation'],"SH")
      
  #ctx.field("SetDicomTreeOnImage.inDicomTree").setObject(mutableTree)
    
  #if ctx.field("FromFrontier").value:
    #we use dicom tool from testinstall, dicomSend
  #  print("via Frontier")
  #  _frontier = ctx.module("parent:FrontierSyngoInterface").object()
  #  ctx.field("parent:DicomExport.exportBaseDir").setStringValue(_frontier.getOutgoingDicomDirectory())
  #  DicomToolToUse = ctx.module("parent:DicomExport") #ctx.module("DicomTool") #
  #  print(DicomToolToUse.field("exportBaseDir").value)
  #  mutableTree.setPrivateTag(0x07a1, "pdeman", 0x42, inImages[currentImage]["SeriesInstanceUID"], "UI")
  #  #ctx.connectField("parent:DicomExport.inImage","DicomTagModify.output0")
  #  ctx.connectField("parent:DicomExport.inImage","SetDicomTreeOnImage.output0")
  #else:
  #  #we use dicom tool from TotalVariationInterface, dicomSave
  #  print("not via Frontier")
  #  DicomToolToUse = ctx.module("DicomTool")
  #  ctx.field("DicomTool.exportBaseDir").setStringValue(os.path.join(os.path.dirname(inImages["Image0"]["file"]),"Results"))
  

  #DicomToolToUse.field("exportNameTemplate").setStringValue("$S/"+"brainMask"+currentImage+"$T.dcm")
  #DicomToolToUse.field("saveSlices").touch()
  #print("DicomTool done")