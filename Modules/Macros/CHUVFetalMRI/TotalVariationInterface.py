# ----------------------------------------------------------------------------

# 
#  \file    TotalVariationInterface.py
#  \author  FetalMRI
#  \date    2018-07-17
#
#  

# ----------------------------------------------------------------------------

from mevis import *
import re
import math

g_TVGraphicsView = None

def initTotalVariationInterface(view):
  global g_TVGraphicsView
  g_TVGraphicsView = view
  print("##### initTVGraphicsView")
  updateInterface()
  
  
def updateInterface():
  
  g_sceneTV = g_TVGraphicsView.scene()
  g_sceneTV.clear()
  
  mdlToSet = ""
  
  try:
   inImages = ctx.field("inImageInfos").object()
   listLRImage = [keyImage for keyImage in list(inImages.keys()) if "Image" in keyImage]
   listImage=sort_human(listLRImage)
   numImage = len(listLRImage)
   GetInfoInput=True
   
  except:
   numImage = ctx.field("NumberImages").value
   GetInfoInput=False
  
  print(numImage)
  if numImage<=6:
    for i in range(numImage):
    
      if GetInfoInput:
        checkBoxDefinition = "CheckBox {name = checkImage%i title = %s checked = True}"%(i,listImage[i])
      else:
        checkBoxDefinition = "CheckBox {name = checkImage%i title = \'Image%i\' checked = True}"%(i,i)
      
      mdlToSet += """Horizontal { """ + checkBoxDefinition + """ } """
  else:
    for i in [0,2,4]:
      if GetInfoInput:
        checkBoxDefinition = "CheckBox {name = checkImage%i title = %s checked = True}"%(i,listImage[i])
        checkBoxDefinition2 = "CheckBox {name = checkImage%i title = %s checked = True}"%(i+1,listImage[i+1])
      else:
        checkBoxDefinition = "CheckBox {name = checkImage%i title = \'Image%i\' checked = True}"%(i,i)
        checkBoxDefinition2 = "CheckBox {name = checkImage%i title = \'Image%i\' checked = True}"%(i+1,i+1)
        
      mdlToSet += """Horizontal { """ + checkBoxDefinition + checkBoxDefinition2 + """ } """
      
    listToFinish= range(6,numImage,2)
    for i in listToFinish:
      if GetInfoInput:
        checkBoxDefinition = "CheckBox {name = checkImage%i title = %s checked = True}"%(i,listImage[i])
        if (i+1)<numImage:
          checkBoxDefinition2 = "CheckBox {name = checkImage%i title = %s checked = True}"%(i+1,listImage[i+1])
        else:
          checkBoxDefinition2 = ""
      else:
        checkBoxDefinition = "CheckBox {name = checkImage%i title = \'Image%i\' checked = True}"%(i,i)
        if (i+1)<numImage:
          checkBoxDefinition2 = "CheckBox {name = checkImage%i title = \'Image%i\' checked = True}"%(i+1,i+1)
        else:
          checkBoxDefinition2 = ""
      
      mdlToSet += """Horizontal { """ + checkBoxDefinition + checkBoxDefinition2 + """ } """      
        
    
  g_sceneTV.addMDL("Vertical {" + mdlToSet + "}")
  

  
  
def buttonPressedAxial():
  print("clicked axial")
  
def buttonPressedCoronal():
  print("clicked coronal")
  
def buttonPressedSagittal():
  print("clicked sagittal")
  
  
def RunSuperResolution():
  print("run SuperVariation")
  
  
def resetTV():
  print("resetTVImages")
  
def updateTotalVariationInterface():
  inImages = ctx.field("inImageInfos").object()
  listSRTV_ITER=[]
  if inImages is not None:
    for inImagesIter in inImages.keys():
     if inImagesIter.startswith("SRTV_ITER"):
       listSRTV_ITER.append(inImagesIter)
    
  if len(listSRTV_ITER)>0:
    lastIter = sort_human(listSRTV_ITER)[-1]  
    ctx.field("itkImageFileReader.fileName").setStringValue(inImages[lastIter])
    
    #and now we run the next steps ?
  
def updateImage():
  ctx.field("CreateBoundingVolumeAxial.add").touch()
  ctx.field("ReformatAxial.apply").touch()
  ctx.field("BoundingBoxAxial.update").touch()
  ctx.field("CreateBoundingVolumeSagittal.add").touch()
  ctx.field("ReformatSagittal.apply").touch()
  ctx.field("BoundingBoxSagittal.update").touch()
  ctx.field("CreateBoundingVolumeCoronal.add").touch()
  ctx.field("ReformatCoronal.apply").touch()
  ctx.field("BoundingBoxCoronal.update").touch()


def showHelp():
  print("showHelp")
  if not ctx.field("FromFrontier").value:
    import webbrowser
    print(webbrowser.browser)
    print(MLABFileManager.exists(ctx.expandFilename("$(MLAB_mevisFetalMRI_MRUser)/Documentation/Publish/ModuleReference/TotalVariationInterface.html")))
    webbrowser.open_new(ctx.expandFilename("$(MLAB_mevisFetalMRI_MRUser)/Documentation/Publish/ModuleReference/TotalVariationInterface.html"))

  else:
    global _frontier
    _frontier = ctx.module("parent:FrontierSyngoInterface").object()
    url = ctx.expandFilename("$(MLAB_mevisFetalMRI_MRUser)/Documentation/Publish/ModuleReference/TotalVariationInterface.html")
    _frontier._syngoVia.call("FE.AppHosting.ShowUrl", url)


def sort_human(l):
  convert = lambda text: float(text) if text.isdigit() else text
  alphanum = lambda key: [ convert(c) for c in re.split('([-+]?[0-9]*\.?[0-9]*)', key) ]
  l.sort( key=alphanum )
  return l    
  