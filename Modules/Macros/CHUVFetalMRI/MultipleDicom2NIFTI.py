# ----------------------------------------------------------------------------

# 
#  \file    MultipleDicom2NIFTI.py
#  \author  FetalMRI
#  \date    2018-02-14
#
#  

# ----------------------------------------------------------------------------

from mevis import MLAB, MLABFileManager, MLABFileDialog
from LocalFilePathSupport import populateComboBoxWithDefaultPathVariables

import os
import unicodedata


def fileDialog():
  #exp = ctx.expandFilename(ctx.field("AtlasPath").stringValue())
  filename = MLABFileDialog.getExistingDirectory(ctx.field("name").stringValue(), "Select a Directory", MLABFileDialog.ShowDirsOnly)
  if filename:
    ctx.field("name").value = filename
    #ctx.field("name").value = ctx.unexpandFilename(filename)
    
def fileDialog2():
  #exp = ctx.expandFilename(ctx.field("AtlasPath").stringValue())
  filename = MLABFileDialog.getExistingDirectory(ctx.field("name2").stringValue(), "Select a Directory", MLABFileDialog.ShowDirsOnly)
  if filename:
    ctx.field("name2").value = filename    
    
    
def populateComboBox():
  comboBox = ctx.control("pathChoices")
  populateComboBoxWithDefaultPathVariables(comboBox, "$Home")
  
def populateComboBox2():
  comboBox = ctx.control("pathChoices2")
  populateComboBoxWithDefaultPathVariables(comboBox, "$Home")  
  
  
def execute():
  
  print("conversion start")
  ctx.field("DirectDicomImport.source").setStringValue(str(ctx.field("name").value))
  ctx.field("DirectDicomImport.dplImport").touch()
  numVolu = ctx.field("DirectDicomImport.numVolumes").value
  for iterVol in range(numVolu):
    ctx.field("MultiFileVolumeListImageOutput.outVolIdx").setValue(iterVol)
    #ctx.field("itkImageFileWriter.fileName").setValue(ctx.expandFilename(ctx.field("name2").value+os.sep+"Image%i.nii"%iterVol))
    print(ctx.field("itkImageFileWriter.fileName").value)
    ctx.field("itkImageFileWriter.save").touch()
  print("conversion done")
  
  
def modify_itksavename():
  print("update itk savename")
  #ctx.field("DicomTagViewer.tagValue0").value
  #ctx.field("DicomTagViewer.tagValue1").value
  newtag1 = unicodedata.normalize('NFKD', ctx.field("DicomTagViewer.tagValue1").value).encode('ASCII', 'ignore')
  ctx.field("itkImageFileWriter.fileName").setValue(ctx.expandFilename(ctx.field("name2").value+os.sep+str(ctx.field("MultiFileVolumeListImageOutput.outVolIdx").value)+str(ctx.field("DicomTagViewer.tagValue0").value).replace(" ","")+"_"+newtag1.replace(" ","")+".nii"))