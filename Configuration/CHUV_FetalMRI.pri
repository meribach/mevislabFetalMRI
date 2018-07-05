isEmpty(CHUV_FetalMRI_PRI_INCLUDED) {
  message ( loading CHUV_FetalMRI.pri )
}
# **InsertLicense** code
# -----------------------------------------------------------------------------
# CHUV_FetalMRI prifile
#
# \file    CHUV_FetalMRI.pri
# \author  MIAL
# \date    2017-12-12
#
# FetalMRI superresolution 
#
# -----------------------------------------------------------------------------

# include guard against multiple inclusion
isEmpty(CHUV_FetalMRI_PRI_INCLUDED) {

CHUV_FetalMRI_PRI_INCLUDED = 1

# -- System -------------------------------------------------------------

include( $(MLAB_MeVis_Foundation)/Configuration/SystemInit.pri )

# -- Define local PACKAGE variables -------------------------------------

PACKAGE_ROOT    = $$(MLAB_CHUV_FetalMRI)
PACKAGE_SOURCES = "$${PACKAGE_ROOT}"/Sources
PACKAGE_PROJECTS = "$${PACKAGE_ROOT}"/Projects
PACKAGE_LIB = = "$${PACKAGE_ROOT}"/lib

# Add package library path
LIBS          += -L"$${PACKAGE_ROOT}"/lib

# -- Projects -------------------------------------------------------------

# NOTE: Add projects below to make them available to other projects via the CONFIG mechanism

# -----------------------------------------------------------------------------
# You can use these example templates for typical projects #
# -----------------------------------------------------------------------------

# --------------------------------
# For self-contained projects (located in the 'Projects' folder of the package):

#MLMySelfContainedProject {
#  CONFIG_FOUND += MLMySelfContainedProject
#  $include ( $${PACKAGE_PROJECTS}/MLMySelfContainedProject/Sources/MLMySelfContainedProject.pri ) 
#}

# and within $${PACKAGE_PROJECTS}/MLMySelfContainedProject/Sources/MLMySelfContainedProject.pri: 
#
#  INCLUDEPATH += $${PACKAGE_PROJECTS}/MLMySelfContainedProject/Sources
#  win32:LIBS += MLMySelfContainedProject$${d}.lib
#  unix:LIBS += -lMLMySelfContainedProject$${d}
#  CONFIG += ProjectMLMySelfContainedProjectDependsOn1 ProjectMLMySelfContainedProjectDependsOn2 ...

# --------------------------------
# For non-self-contained projects (old-style, distributed over Sources/Modules/... ):

#MLMyProject {
#  CONFIG_FOUND += MLMyProject
#  INCLUDEPATH += $${PACKAGE_SOURCES}/ML/MLMyProject
#  win32:LIBS += MLMyProject$${d}.lib
#  unix:LIBS += -lMLMyProject$${d}
#  CONFIG += ProjectMLMyProjectDependsOn1 ProjectMLMyProjectDependsOn2 ...
#}

MyTools {
   CONFIG_FOUND += MyTools
   INCLUDEPATH += $${PACKAGE_SOURCES}/Shared/MyTools 
   QMAKE_LIBDIR += $${PACKAGE_LIB}/MyTools
   win32:LIBS += mialsrtkOrientImageLib.lib$${d} \
                 mialsrtkOrientImageWrapper.lib$${d} \
                 btkNLMDenoisingLib.lib$${d} \
                 btkNLMDenoisingWrapper.lib$${d} 
   unix:LIBS +=	 -lmialsrtkOrientImageLib$${d} \
                 -lmialsrtkOrientImageWrapper$${d} \
                 -lbtkNLMDenoisingLib$${d} \
                 -lbtkNLMDenoisingWrapper$${d} \  
}

MyIntensityStandardization {
   CONFIG_FOUND += MyIntensityStandardization 
   INCLUDEPATH += $${PACKAGE_SOURCES}/Shared/MyIntensityStandardization  
   QMAKE_LIBDIR += $${PACKAGE_LIB}/MyIntensityStandardization 
   win32:LIBS += mialsrtkCorrectSliceIntensityLib.lib$${d} \
                 mialsrtkCorrectSliceIntensityWrapper.lib$${d} \
                 mialsrtkIntensityStandardizationLib.lib$${d} \
                 mialsrtkIntensityStandardizationWrapper.lib$${d} \
                 mialsrtkSliceBySliceCorrectBiasFieldLib.lib$${d} \
                 mialsrtkSliceBySliceCorrectBiasFieldWrapper.lib$${d} \
                 mialsrtkSliceBySliceN4BiasFieldCorrectionLib.lib$${d} \
                 mialsrtkSliceBySliceN4BiasFieldCorrectionWrapper.lib$${d} 
   unix:LIBS +=	 -lmialsrtkCorrectSliceIntensityLib$${d} \
                 -lmialsrtkCorrectSliceIntensityWrapper$${d} \
                 -lmialsrtkIntensityStandardizationLib$${d} \
                 -lmialsrtkIntensityStandardizationWrapper$${d} \
                 -lmialsrtkSliceBySliceCorrectBiasFieldLib$${d} \
                 -lmialsrtkSliceBySliceCorrectBiasFieldWrapper$${d} \
                 -lmialsrtkSliceBySliceN4BiasFieldCorrectionLib$${d} \
                 -lmialsrtkSliceBySliceN4BiasFieldCorrectionWrapper$${d}\		 
}

MyImageReconstruction {
   CONFIG_FOUND += MyImageReconstruction
   INCLUDEPATH += $${PACKAGE_SOURCES}/Shared/MyImageReconstruction
   QMAKE_LIBDIR += $${PACKAGE_LIB}/MyImageReconstruction 
   win32:LIBS += mialsrtkImageReconstructionLib.lib$${d} \
                 mialsrtkImageReconstructionWrapper.lib$${d} 
   unix:LIBS += -lmialsrtkImageReconstructionLib$${d} \
                -lmialsrtkImageReconstructionWrapper$${d} 
}

MyTVSuperResolution {
   CONFIG_FOUND += MyTVSuperResolution 
   INCLUDEPATH += $${PACKAGE_SOURCES}/Shared/MyTVSuperResolution 
   QMAKE_LIBDIR += $${PACKAGE_LIB}/MyTVSuperResolution
   win32:LIBS += mialsrtkTVSuperResolutionLib.lib$${d} \
                 mialsrtkTVSuperResolutionWrapper.lib$${d} 
   unix:LIBS += -lmialsrtkTVSuperResolutionLib$${d} \
                -lmialsrtkTVSuperResolutionWrapper$${d} \
}

MyITK {
  CONFIG_FOUND += MyITK
  QMAKE_LIBDIR += $${PACKAGE_LIB}/MyITK 
  win32:LIBS += ITKBiasCorrection-4.13.lib \
		ITKIOImageBase-4.13.lib \
		ITKNrrdIO-4.13.lib \
		itkgdcmMSFF-4.13.lib \
		ITKBioCell-4.13.lib \
		ITKIOJPEG-4.13.lib \
		ITKOptimizers-4.13.lib \
		itkgdcmcharls-4.13.lib \
		ITKCommon-4.13.lib \
		ITKIOLSM-4.13.lib \
		ITKOptimizersv4-4.13.lib \
		itkgdcmjpeg12-4.13.lib \
		ITKIOMINC-4.13.lib \
		ITKPath-4.13.lib \
		itkgdcmjpeg16-4.13.lib \
		ITKCommon1TestDriver.lib \
		ITKIOMRC-4.13.lib \
		ITKPolynomials-4.13.lib \
		itkgdcmjpeg8-4.13.lib \
		ITKDICOMParser-4.13.lib \
		ITKIOMesh-4.13.lib \
		ITKQuadEdgeMesh-4.13.lib \
		itkgdcmopenjp2-4.13.lib \
		ITKEXPAT-4.13.lib \
		ITKIOMeta-4.13.lib \
		ITKSpatialObjects-4.13.lib \
		itkgdcmsocketxx-4.13.lib \
		ITKFEM-4.13.lib \
		ITKIONIFTI-4.13.lib \
		ITKStatistics-4.13.lib \
		itkgtest-4.13.lib \
		ITKIONIFTI-TestSupport.lib \
		ITKTransform-4.13.lib \
		itkgtest_main-4.13.lib \
		ITKFEMHeaderTest1.lib \
		ITKIONRRD-4.13.lib \
		ITKTransformFactory-4.13.lib \
		itkjpeg-4.13.lib \
		ITKIOPNG-4.13.lib \
		ITKVNLInstantiation-4.13.lib \
		itklbfgs-4.13.lib \
		ITKFEMRegistrationHeaderTest1.lib \
		ITKIOSiemens-4.13.lib \
		ITKVTK-4.13.lib \
		itkminc2-4.13.lib \
		ITKIOSpatialObjects-4.13.lib \
		ITKVideoCore-4.13.lib \
		itknetlib-4.13.lib \
		ITKFEMRegistrationTestDriver.lib \
		ITKIOStimulate-4.13.lib \
		ITKVideoIO-4.13.lib \
		itkpng-4.13.lib \
		ITKIOTIFF-4.13.lib \
		ITKWatersheds-4.13.lib \
		itksys-4.13.lib \
		ITKFEMTestDriver.lib \
		ITKIOTransformBase-4.13.lib \
		ITKgiftiio-4.13.lib \
		itktestlib-4.13.lib \
		ITKIOBMP-4.13.lib \
		ITKIOTransformHDF5-4.13.lib \
		ITKniftiio-4.13.lib \
		itktiff-4.13.lib \
		ITKIOBioRad-4.13.lib \
		ITKIOTransformInsightLegacy-4.13.lib \
		ITKznz-4.13.lib \
		itkv3p_netlib-4.13.lib \
		ITKIOBruker-4.13.lib \
		ITKIOTransformMatlab-4.13.lib \
		itkNetlibSlatec-4.13.lib \
		itkvcl-4.13.lib \
		ITKIOCSV-4.13.lib \
		ITKIOVTK-4.13.lib \
		itkdouble-conversion-4.13.lib \
		itkvnl-4.13.lib \
		ITKIOGDCM-4.13.lib \
		ITKIOXML-4.13.lib \
		itkgdcmCommon-4.13.lib \
		itkvnl_algo-4.13.lib \
		ITKIOGE-4.13.lib \
		ITKKLMRegionGrowing-4.13.lib \
		itkgdcmDICT-4.13.lib \
		itkzlib-4.13.lib \
		ITKIOGIPL-4.13.lib \
		ITKLabelMap-4.13.lib \
		itkgdcmDSED-4.13.lib \
		libitkhdf5.lib \
		ITKIOHDF5-4.13.lib \
		ITKMesh-4.13.lib \
		itkgdcmIOD-4.13.lib \
		libitkhdf5_cpp.lib \
		ITKIOIPL-4.13.lib \
		ITKMetaIO-4.13.lib \
		itkgdcmMEXD-4.13.lib \
		Ws2_32.lib \
		Rpcrt4.lib \
		Snmpapi.lib \
		psapi.lib 

		
  unix:LIBS += -lITKIOSiemens-4.13 \
               -lITKBiasCorrection-4.13 \
               -lITKIOSpatialObjects-4.13 \
               -lITKBioCell-4.13 \    
               -lITKIOStimulate-4.13 \
               -lITKCommon-4.13 \     
               -lITKIOTIFF-4.13 \
               -lITKDICOMParser-4.13 \       
               -lITKIOTransformBase-4.13 \
               -litkdouble-conversion-4.13 \
               -lITKIOTransformHDF5-4.13 \
               -lITKEXPAT-4.13 \ 
               -lITKIOTransformInsightLegacy-4.13 \
               -lITKFEM-4.13 \
               -lITKIOTransformMatlab-4.13 \
               -litkgdcmcharls-4.13 \
               -lITKIOVTK-4.13 \
               -litkgdcmCommon-4.13 \        
               -lITKIOXML-4.13 \
               -litkgdcmDICT-4.13 \        
               -litkjpeg-4.13 \
               -litkgdcmDSED-4.13 \          
               -lITKKLMRegionGrowing-4.13 \
               -litkgdcmIOD-4.13 \        
               -lITKLabelMap-4.13 \
               -litkgdcmjpeg12-4.13 \
               -litklbfgs-4.13 \
               -litkgdcmjpeg16-4.13 \       
               -lITKMesh-4.13 \
               -litkgdcmjpeg8-4.13 \        
               -lITKMetaIO-4.13 \
               -litkgdcmMEXD-4.13 \         
               -litkminc2-4.13 \
               -litkgdcmMSFF-4.13 \          
               -litknetlib-4.13 \
               -litkgdcmopenjp2-4.13 \       
               -litkNetlibSlatec-4.13 \
               -litkgdcmsocketxx-4.13 \     
               -lITKniftiio-4.13 \
               -litkgdcmuuid-4.13 \      
               -lITKNrrdIO-4.13 \
               -lITKgiftiio-4.13 \          
               -lITKOptimizers-4.13 \
               -litkgtest-4.13 \      
               -lITKOptimizersv4-4.13 \
               -litkgtest_main-4.13 \    
               -lITKPath-4.13 \
               -litkhdf5 \        
               -litkpng-4.13 \
               -litkhdf5_cpp \             
               -lITKPolynomials-4.13 \
               -lITKIOBioRad-4.13 \     
               -lITKQuadEdgeMesh-4.13 \
               -lITKIOBMP-4.13 \    
               -lITKSpatialObjects-4.13 \
               -lITKIOBruker-4.13 \  
               -lITKStatistics-4.13 \
               -lITKIOCSV-4.13 \      
               -litksys-4.13 \
               -lITKIOGDCM-4.13 \            
               -litktestlib-4.13 \
               -lITKIOGE-4.13 \          
               -litktiff-4.13 \
               -lITKIOGIPL-4.13 \            
               -lITKTransform-4.13 \
               -lITKIOHDF5-4.13 \       
               -lITKTransformFactory-4.13 \
               -lITKIOImageBase-4.13 \
               -litkv3p_netlib-4.13 \
               -lITKIOIPL-4.13 \        
               -litkvcl-4.13 \
               -lITKIOJPEG-4.13 \           
               -lITKVideoCore-4.13 \
               -lITKIOLSM-4.13 \       
               -lITKVideoIO-4.13 \
               -lITKIOMesh-4.13 \         
               -litkvnl-4.13 \
               -lITKIOMeta-4.13 \           
               -litkvnl_algo-4.13 \
               -lITKIOMINC-4.13 \        
               -lITKVNLInstantiation-4.13 \
               -lITKIOMRC-4.13 \
               -lITKVTK-4.13 \
               -lITKIONIFTI-4.13 \           
               -lITKWatersheds-4.13 \
               -lITKIONRRD-4.13 \      
               -litkzlib-4.13 \
               -lITKIOPNG-4.13 \            
               -lITKznz-4.13 \
}

message($$INCLUDEPATH)
message("PAckage lib path")
message($$PACKAGE_LIB)  

# -----------------------------------------------------------------------------

# -- ML Projects -------------------------------------------------------------

# -- Inventor Projects -------------------------------------------------------

# -- Shared Projects ---------------------------------------------------------

# End of projects ------------------------------------------------------------

}