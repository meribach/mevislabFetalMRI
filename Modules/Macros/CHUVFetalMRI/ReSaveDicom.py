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
  ctx.field("DirectDicomImport.dplImport").touch()
  
  for iterImage in range(ctx.field("DirectDicomImport.numVolumes").value):
    
    ctx.field("MultiFileVolumeListImageOutput.outVolIdx").setIntValue(iterImage)
    print(ctx.field("DicomTagViewer.tagValue0").value)
    
    print(ctx.field("DicomTagViewer.tagValue1").value)
    PatientName=ctx.field("DicomTagViewer.tagValue0").value
    if PatientName.startswith("MARTI"):
      ctx.field("DicomTool.saveSlices").touch()
  
  
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