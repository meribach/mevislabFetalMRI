# ----------------------------------------------------------------------------

# 
#  \file    GetAtlasMacro.py
#  \author  pierre
#  \date    2017-12-14
#
#  get atlas for filename and week

# ----------------------------------------------------------------------------

from mevis import MLAB, MLABFileManager, MLABFileDialog
from LocalFilePathSupport import populateComboBoxWithDefaultPathVariables

import os

def init():
  print("test")


#def fileDropped(filename):
#  ctx.field("name").value = ctx.unexpandFilename(filename)

def AtlasPathChanged():
  print("atlaspath changed")
  exp = ctx.expandFilename(ctx.field("name").stringValue())
  if MLABFileManager.exists(exp):
    updateAtlas(newPath=exp)
  
def weeksChanged():
  print("weeks changed")
  valueWeek = ctx.field("currentWeek").value
  #print("number of week : %i"%valueWeek)
  updateAtlas(newWeek=valueWeek)

def updateAtlas(newPath=None,newWeek=None):

   for file in os.listdir(ctx.field("AtlasPath").value):
     if file.endswith(".nii.gz"):
       if not "parc" in file and not "LogicalMask" in file:
         if "STA%i"%ctx.field("currentWeek").value in file:
           print(os.path.join(ctx.field("AtlasPath").value,file))
           ctx.field("itkImageFileReader.fileName").setStringValue(os.path.join(ctx.field("AtlasPath").value,file))
       if "LogicalMask" in file:
         if "STA%i"%ctx.field("currentWeek").value in file:
           ctx.field("itkImageFileReaderMask.fileName").setStringValue(os.path.join(ctx.field("AtlasPath").value,file))

def populateComboBox():
  comboBox = ctx.control("pathChoices")
  populateComboBoxWithDefaultPathVariables(comboBox, "$Home")

def fileDialog():
  #exp = ctx.expandFilename(ctx.field("AtlasPath").stringValue())
  filename = MLABFileDialog.getExistingDirectory(ctx.field("AtlasPath").stringValue(), "Select a Directory", MLABFileDialog.ShowDirsOnly)
  if filename:
    ctx.field("AtlasPath").value = filename
    ctx.field("name").value = ctx.unexpandFilename(filename)