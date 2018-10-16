# ----------------------------------------------------------------------------

# 
#  \file    ReSaveDicom.py
#  \author     neuropsynov
#  \date    2018-10-16
#
#  

# ----------------------------------------------------------------------------

from mevis import *


def main():
  print("main")
  citx.field("DirectDicomImport.dplImport").touch()
  
def fileDialog(InOut):
  #exp = ctx.expandFilename(ctx.field("AtlasPath").stringValue())
  filename = MLABFileDialog.getExistingDirectory(ctx.field(InOut).stringValue(), "Select a Directory", MLABFileDialog.ShowDirsOnly)
  if filename:
    ctx.field(InOut).value = filename
    print(filename)
    if InOut == "InputDirectory":
      ctx.field("inputLabel").value = filename
    elif InOut == "OutputDirectory":
      ctx.field("outputLabel").value = filename