# ----------------------------------------------------------------------------

# 
#  \file    mialHistogramNormalization.py
#  \author     neuropsynov
#  \date    2018-06-08
#
#  

# ----------------------------------------------------------------------------

from mevis import *
import numpy as np
#from matplotlib import pyplot
import glob


#from scipy.ndimage.filters import percentile_nonzero_filter

import sys
import os
import re
import fnmatch
import ntpath
import copy



def runHistoNormalization():
  
  interface = ctx.module("PythonImage").call("getInterface")
  
  inputFileNames = ctx.field("inputFiles").value
  allinputNames = inputFileNames.split("--")
  
  for indexInput in range(len(allinputNames)):
    allinputNames[indexInput] = allinputNames[indexInput].strip()
  
  outputFileNames = ctx.field("outputFiles").value
  if outputFileNames != "":
    alloutputNames = outputFileNames.split("--")
  
    for indexOutput in range(len(alloutputNames)):
      alloutputNames[indexOutput] = alloutputNames[indexOutput].strip()
    
    if len(alloutputNames) != len(allinputNames):
      print("error not the same name of inputfile and outputfile names")
      return
    
  else:
    alloutputNames=[]
    for indexInput in range(len(allinputNames)):
     alloutputNames.append(allinputNames[indexInput].replace(".nii","_histnorm.nii"))
     
  maskFileNames = ctx.field("maskFiles").value
  allmaskNames = maskFileNames.split("--")
  for indexMask in range(len(allmaskNames)):
    allmaskNames[indexMask] = allmaskNames[indexMask].strip()
 
  if len(allmaskNames) != len(allinputNames):
    print("error not the same number of inputfile and maskfile names")
    return
  
  
  s1=1
  list_landmarks=[]
  AllImagesInfo = []
  AllmaskedImage = []
  for iterImages in range(len(allinputNames)):
    ctx.field("itkImageFileReader.fileName").setStringValue(allinputNames[iterImages])
    AllImagesInfo.append([ctx.field("itkImageFileReader.output0").image().imageExtent(),ctx.field("itkImageFileReader.output0").image().voxelToWorldMatrix()])
    matrixImage=np.array(ctx.field("itkImageFileReader.output0").image().getTile((0,0,0,0,0,0), ctx.field("itkImageFileReader.output0").image().imageExtent()))
    ctx.field("itkImageFileReader.fileName").setStringValue(allmaskNames[iterImages])
    mask = ctx.field("itkImageFileReader.output0").image()
    matrixMask=np.array(mask.getTile((0,0,0,0,0,0), mask.imageExtent()))
    
    AllmaskedImage.append(np.reshape(matrixImage*matrixMask,matrixImage.shape[3]*matrixImage.shape[4]*matrixImage.shape[5]))
    #displayHistogram(AllmaskedImage[iterImages],1,0)
    list_landmarks.append(extractImageLandmarks(AllmaskedImage[iterImages]))
  
  list_landmarks,dS = trainImageLandmarks(list_landmarks)
  s2 = np.ceil(dS - s1)
  list_landmarks_mapped = mapImageLandmarks(list_landmarks,s1,s2)
  mean_landmarks = computeMeanMapImageLandmarks(list_landmarks_mapped)
  
  for iterImages in range(len(allinputNames)):
   
    maskedImageMapped = mapImage(AllmaskedImage[iterImages],mean_landmarks,list_landmarks[iterImages]['quartiles'],s1,s2,list_landmarks[iterImages]['p1'],list_landmarks[iterImages]['p2'])
    #displayHistogram(maskedImageMapped,image_name,1,0)
    o2o=verifyOne2OneMapping(s1,s2,list_landmarks[iterImages],mean_landmarks)
    dimX = AllImagesInfo[iterImages][0][0];
    dimY = AllImagesInfo[iterImages][0][1];
    dimZ = AllImagesInfo[iterImages][0][2];
    
    tile = np.ndarray((dimX,dimY,dimZ), np.float32)
    tile=np.reshape(maskedImageMapped,np.array([dimZ,dimY,dimX]))
    interface.setImage(tile, minMaxValues = (maskedImageMapped.min(), maskedImageMapped.max()),voxelToWorldMatrix = AllImagesInfo[iterImages][1])
    ctx.field("itkImageFileWriter.unresolvedFileName").setStringValue(alloutputNames[iterImages])
    ctx.field("itkImageFileWriter.save").touch()
    
  
  print("histonormalization done")
  
  
def percentile_nonzero(image, percentile_nonzero):
    #pdb.set_trace()
    if len(image) < 1:
        value = None
    elif (percentile_nonzero >= 100):
        sys.stderr.write('ERROR: percentile_nonzero must be < 100.  you supplied: %s\n'% percentile_nonzero)
        value = None
    else:
        image_nonzero=image[image!=0]
        element_idx = int(len(image_nonzero) * (percentile_nonzero / 100.0))
        image_nonzero.sort()
        value = image_nonzero[element_idx]
    return value

def mean_nonzero(image):
    image_nonzero=image[image!=0]
    mean=np.sum(image_nonzero)/len(image_nonzero)
    return mean

def intensityNormalization(image,landmarks):
    print 'min ='+str(landmarks['p1'])
    print 'max (99.8%) ='+str(landmarks['p2'])
        #print 'mean ='+str(landmarks['mean'])
    print 'quartiles [25%,50%,75%] ='+str(landmarks['quartiles'])
    return 1

def displayHistogram(image,loffset,roffset):
    bins = np.round(np.arange(loffset,np.max(image)-roffset,40))
    histo, bins = np.histogram(image, bins=bins) 
    bins_center = 0.5*(bins[1:] + bins[:-1])
    #pyplot.plot(bins_center,histo,alpha=0.5)
    ##pyplot.hist(fit(np.random.uniform(x[0],x[-1],len(image))),bins=y)
    ##pyplot.hist(image,bins,histtype='step',alpha=0.5,label=image_name)
    return 1

def extractImageLandmarks(image):
    landmarks={}
    landmarks['p1']=percentile_nonzero(image,0)
    landmarks['p2']=percentile_nonzero(image,99.8)
    #landmarks['mean']=mean_nonzero(image)
    landmarks['quartiles']=[percentile_nonzero(image,25),percentile_nonzero(image,50),percentile_nonzero(image,75)]
    #landmarks['quartiles']=[percentile_nonzero(image,10),percentile_nonzero(image,20),percentile_nonzero(image,30),percentile_nonzero(image,40),percentile_nonzero(image,50),percentile_nonzero(image,60),percentile_nonzero(image,70),percentile_nonzero(image,80),percentile_nonzero(image,90)]
    #pdb.set_trace()
    return landmarks

def trainImageLandmarks(list_landmarks):
    mup_l=[]
    mup_L=[]
    mup_r=[]
    mup_R=[]
    maxLR=[]
    index=0
    while index<len(list_landmarks):
        landmarks=list_landmarks[index]['quartiles']
        mup_l.append(np.min(landmarks-list_landmarks[index]['p1']))
        mup_L.append(np.max(landmarks-list_landmarks[index]['p1']))
        mup_r.append(np.min(list_landmarks[index]['p2']-landmarks))
        mup_R.append(np.max(list_landmarks[index]['p2']-landmarks))
        maxLR.append(np.max([float(mup_L[index])/mup_l[index],float(mup_R[index])/mup_r[index]]))
        #print 'mup_l  =  '+str(mup_l[index])
        #print 'mup_L  =  '+str(mup_L[index])
        #print 'mup_r  =  '+str(mup_r[index])
        #print 'mup_R  =  '+str(mup_R[index])
        #print 'maxLR  =  '+str(maxLR[index])
        index+=1
    ymax=np.max(maxLR)
    ymax_index=maxLR.index(max(maxLR))
    dS = float(ymax*(mup_L[ymax_index]+mup_R[ymax_index]))
    print 'Ymax  =  '+str(ymax)+'  at position '+str(ymax_index)+'  ,  dS = '+str(dS)+' (=s2 when s1=0)'
    return list_landmarks,dS

def mapImageLandmarks(list_landmarks,s1,s2):
    list_landmarks_mapped = copy.deepcopy(list_landmarks)
    index=0
    while index<len(list_landmarks):
        land_index=0
        print 'Image index: '+str(index)
        while land_index<len(list_landmarks[index]['quartiles']):
            print 'old landmark: '+str(list_landmarks_mapped[index]['quartiles'][land_index])
            list_landmarks_mapped[index]['quartiles'][land_index]=s1+float((list_landmarks_mapped[index]['quartiles'][land_index]-list_landmarks_mapped[index]['p1'])/float(list_landmarks_mapped[index]['p2']-list_landmarks_mapped[index]['p1']))*float((s2-s1))
            print 'new landmark: '+str(list_landmarks_mapped[index]['quartiles'][land_index])
            land_index+=1
        print 'p1, p2 = '+str(list_landmarks_mapped[index]['p1'])+', '+str(list_landmarks_mapped[index]['p2'])
        index+=1
    return list_landmarks_mapped

def verifyOne2OneMapping(s1,s2,list_landmarks,lmap_mean):
    landmarks=list_landmarks['quartiles']
    mup_L=np.max(landmarks-list_landmarks['p1'])
    mup_R=np.max(list_landmarks['p2']-landmarks)
    #print 'mup_L  =  '+str(mup_L)
    #print 'mup_R  =  '+str(mup_R)

    land_index=0
    while land_index<len(lmap_mean):
        #pdb.set_trace()
        if np.logical_and((lmap_mean[str(0)]-s1)>=mup_L,(s2-lmap_mean[str(len(lmap_mean)-1)])>=mup_R):
            cond=1;
        else:
            cond=0;
        land_index+=1
    return cond

def mapImage(image,lmap_mean,list_landmarks,s1,s2,p1,p2):
    image_out=image.copy().astype('float')
    tmp=image.copy().astype('float')
    index=0
    ##pyplot.figure(2)
    #pdb.set_trace()
    while index < len(lmap_mean)+1:
        if index ==0:
            x=np.array([int(p1),int(list_landmarks[index])])
            y=np.array([int(s1),int(lmap_mean[str(index)])])
            coefs=np.polyfit(x,y,1)
            mask=np.logical_and(image > 0, image <= list_landmarks[index])
            image_out[mask]=coefs[0]*image[mask]+coefs[1]
            ##pyplot.plot(x,y,marker='o', linestyle='--');
        else:
            if index==(len(lmap_mean)):
                x=np.array([int(list_landmarks[index-1]),int(p2)])
                y=np.array([int(lmap_mean[str(index-1)]),int(s2)])
                coefs=np.polyfit(x,y,1)
                mask=image>list_landmarks[index-1]
                image_out[mask]=coefs[0]*image[mask]+coefs[1]
                ##pyplot.plot(x,y,marker='o', linestyle='--');
            else:
                x=np.array([int(list_landmarks[index-1]),int(list_landmarks[index])])
                y=np.array([int(lmap_mean[str(index-1)]),int(lmap_mean[str(index)])])
                coefs=np.polyfit(x,y,1)
                mask=np.logical_and(image>list_landmarks[index-1], image<=list_landmarks[index])
                image_out[mask]=coefs[0]*image[mask]+coefs[1]
                ##pyplot.plot(x,y,marker='o', linestyle='--');
        index+=1
    ##pyplot.show()
    return image_out

def computeMeanMapImageLandmarks(list_landmarks):
    mean_landmarks={}
    index=0
    while index < len(list_landmarks):
        land_index=0
        while land_index < len(list_landmarks[index]['quartiles']):
            if(index==0):
                mean_landmarks[str(land_index)] = list_landmarks[index]['quartiles'][land_index];
            else:
                mean_landmarks[str(land_index)]+= list_landmarks[index]['quartiles'][land_index];
            land_index+=1
        index+=1

    land_index=0
    while land_index < len(mean_landmarks):
        mean_landmarks[str(land_index)] = mean_landmarks[str(land_index)] / len(list_landmarks)
        land_index+=1

    print 'Final landmark average : '
    print mean_landmarks
    return mean_landmarks
