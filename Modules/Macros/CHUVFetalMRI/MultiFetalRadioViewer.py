# ----------------------------------------------------------------------------

# 
#  \file    MultiFetalRadioViewer.py
#  \author  FetalMRI
#  \date    2018-06-28
#
#  

# ----------------------------------------------------------------------------

from mevis import MLAB, MLABFileManager, MLABFileDialog



def initMultiFetalRadioGraphicsView(view):
  
  print("test")
  
  
def buttonPressedAxial():
  print("clicked axial")
  
def buttonPressedCoronal():
  print("clicked coronal")
  
def buttonPressedSagittal():
  print("clicked sagittal")
  
  
def fileDialog():
  
  print("get the files")
  filenames = MLABFileDialog.getOpenFileNames("Select nifti files (*.nii *.nii.gz)",ctx.field("ImagesPath").stringValue(), "Select the Images")
  if filenames:
    ctx.field("inImageInfos").setObject(filenames)
    ctx.field("NumberImages").setIntValue(len(filenames))


def updateImage():
  
  filenames = ctx.field("inImageInfos").object()
  ctx.field("itkImageFileReader.unresolvedFileName").setStringValue(filenames[ctx.field("CurrentImageNumber").value])
  ctx.field("CreateBoundingVolumeAxial.add").touch()
  ctx.field("CreateBoundingVolumeCoronal.add").touch()
  ctx.field("CreateBoundingVolumeSagittal.add").touch()
  ctx.field("LabelViewerMode.text").setStringValue(filenames[ctx.field("CurrentImageNumber").value])