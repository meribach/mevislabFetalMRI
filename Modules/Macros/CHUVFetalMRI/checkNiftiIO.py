# ----------------------------------------------------------------------------

# 
#  \file    checkNiftiIO.py
#  \author  FetalMRI
#  \date    2018-04-16
#
#  

# ----------------------------------------------------------------------------

from mevis import *
from mevis import MLAB, MLABFileManager, MLABFileDialog
import os



def PyExecute():
  print("Start")
  fout = open(ctx.field("folderPath").value + os.path.sep + "checkVoxelType.txt","w")
  for file in os.listdir(ctx.field("folderPath").value):
    if file.endswith(".nii") or file.endswith(".nii.gz"):
      
      
      ctx.field("itkImageFileReader.unresolvedFileName").setStringValue(ctx.field("folderPath").value+os.path.sep+file)
      
      ctx.field("itkImageFileReader.fileType").value
      ctx.field("itkImageFileReader.pixelType").value
      ctx.field("itkImageFileReader.componentType").value
      #print("%s is detected as a %s type image"%(file,ctx.field("itkImageFileReader.componentType").value))
      fout.write("%s is detected as a %s type image\n"%(file,ctx.field("itkImageFileReader.componentType").value))
      
      
    else:
      print("%s is not a nifti file"%file)
  
  fout.close()
  print("Done")
  
def fileDialog():
  #exp = ctx.expandFilename(ctx.field("AtlasPath").stringValue())
  filename = MLABFileDialog.getExistingDirectory(ctx.field("folderPath").stringValue(), "Select the directory", MLABFileDialog.ShowDirsOnly)
  if filename:
    ctx.field("folderPath").value = filename