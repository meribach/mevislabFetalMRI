# ----------------------------------------------------------------------------

# 
#  \file    generateTemplateMask.py
#  \author  FetalMRI
#  \date    2018-02-26
#
#  

# ----------------------------------------------------------------------------

from mevis import MLAB, MLABFileManager, MLABFileDialog
from LocalFilePathSupport import populateComboBoxWithDefaultPathVariables

def main():
  print("plif")
  weekVariable = ctx.field("GetAtlasMacro.currentWeek")
  for weekIndex in range(int(weekVariable.minValue()),int(weekVariable.maxValue())+1):
    weekVariable.setValue(weekIndex)
    AtlasName =ctx.field("GetAtlasMacro.itkImageFileReader.fileName").value
    begin=AtlasName.split('.nii.gz')
    newName = begin[0]+"_LogicalMask.nii.gz"
    ctx.field("itkImageFileWriter.fileName").setStringValue(newName)
    ctx.field("itkImageFileWriter.save").touch()

def fileDialog():
  #exp = ctx.expandFilename(ctx.field("AtlasPath").stringValue())
  filename = MLABFileDialog.getExistingDirectory(ctx.field("AtlasPath").stringValue(), "Select a Directory", MLABFileDialog.ShowDirsOnly)
  if filename:
    ctx.field("AtlasPath").value = filename
    ctx.field("name").value = ctx.unexpandFilename(filename)
    
