# ----------------------------------------------------------------------------

# 
#  \file    GenerateBidsNameForNifti.py
#  \author  FetalMRI
#  \date    2018-11-13
#
#  

# ----------------------------------------------------------------------------

from mevis import MLAB, MLABFileManager, MLABFileDialog
from LocalFilePathSupport import populateComboBoxWithDefaultPathVariables

import os

def init():
  print("Init GenerateBidsNameForNifti")
  print(ctx.field("DataPath").stringValue())

def DataPathChanged():
  print("DataPath changed")
  exp = ctx.field("DataPath").stringValue()
  if MLABFileManager.exists(ctx.expandFilename(exp)):
    updateData()
  else:
    fileDialog()


def updateData():
  
  print("updateData")


def populateComboBox():
  comboBox = ctx.control("pathChoices")
  populateComboBoxWithDefaultPathVariables(comboBox, ctx.field("DataPath").stringValue())

def fileDialog():
  #exp = ctx.expandFilename(ctx.field("AtlasPath").stringValue())
  filename = MLABFileDialog.getExistingDirectory(ctx.expandFilename(ctx.field("DataPath").stringValue()), "Select the Patient Case Directory", MLABFileDialog.ShowDirsOnly)
  if filename:
    ctx.field("DataPath").value = ctx.unexpandFilename(filename)
    ctx.field("name").value = filename