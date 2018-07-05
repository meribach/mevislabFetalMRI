isEmpty(mevisFetalMRI_MRUser_PRI_INCLUDED) {
  message ( loading mevisFetalMRI_MRUser.pri )
}
# **InsertLicense** code
# -----------------------------------------------------------------------------
# mevisFetalMRI_MRUser prifile
#
# \file    mevisFetalMRI_MRUser.pri
# \author  MIAL
# \date    2017-12-12
#
# FetalMRI superresolution 
#
# -----------------------------------------------------------------------------

# include guard against multiple inclusion
isEmpty(mevisFetalMRI_MRUser_PRI_INCLUDED) {

mevisFetalMRI_MRUser_PRI_INCLUDED = 1

# -- System -------------------------------------------------------------

include( $(MLAB_MeVis_Foundation)/Configuration/SystemInit.pri )

# -- Define local PACKAGE variables -------------------------------------

PACKAGE_ROOT    = $$(MLAB_mevisFetalMRI_MRUser)
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
   unix:LIBS +=	 libmialsrtkOrientImageLib.a$${d} \
                 libmialsrtkOrientImageWrapper.a$${d} \
                 libbtkNLMDenoisingLib.a$${d} \
                 libbtkNLMDenoisingWrapper.a$${d} \  
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
   unix:LIBS +=	 libmialsrtkCorrectSliceIntensityLib.a$${d} \
                 libmialsrtkCorrectSliceIntensityWrapper.a$${d} \
                 libmialsrtkIntensityStandardizationLib.a$${d} \
                 libmialsrtkIntensityStandardizationWrapper.a$${d} \
                 libmialsrtkSliceBySliceCorrectBiasFieldLib.a$${d} \
                 libmialsrtkSliceBySliceCorrectBiasFieldWrapper.a$${d} \
                 libmialsrtkSliceBySliceN4BiasFieldCorrectionLib.a$${d} \
                 libmialsrtkSliceBySliceN4BiasFieldCorrectionWrapper.a$${d}\		 
}

MyImageReconstruction {
   CONFIG_FOUND += MyImageReconstruction
   INCLUDEPATH += $${PACKAGE_SOURCES}/Shared/MyImageReconstruction
   QMAKE_LIBDIR += $${PACKAGE_LIB}/MyImageReconstruction 
   win32:LIBS += mialsrtkImageReconstructionLib.lib$${d} \
                 mialsrtkImageReconstructionWrapper.lib$${d} 
   unix:LIBS += -l/libmialsrtkImageReconstructionLib.a$${d} \
                -l/libmialsrtkImageReconstructionWrapper.a$${d} 
}

MyTVSuperResolution {
   CONFIG_FOUND += MyTVSuperResolution 
   INCLUDEPATH += $${PACKAGE_SOURCES}/Shared/MyTVSuperResolution 
   QMAKE_LIBDIR += $${PACKAGE_LIB}/MyTVSuperResolution
   win32:LIBS += mialsrtkTVSuperResolutionLib.lib$${d} \
                 mialsrtkTVSuperResolutionWrapper.lib$${d} 
   unix:LIBS += libmialsrtkTVSuperResolutionLib.a$${d} \
                libmialsrtkTVSuperResolutionWrapper.a$${d} \
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

		
  unix:LIBS += libITKIOSiemens-4.13.a \
               libITKBiasCorrection-4.13.a \
               libITKIOSpatialObjects-4.13.a \
               libITKBioCell-4.13.a \    
               libITKIOStimulate-4.13.a \
               libITKCommon-4.13.a \     
               libITKIOTIFF-4.13.a \
               libITKDICOMParser-4.13.a \       
               libITKIOTransformBase-4.13.a \
               libitkdouble-conversion-4.13.a \
               libITKIOTransformHDF5-4.13.a \
               libITKEXPAT-4.13.a \ 
               libITKIOTransformInsightLegacy-4.13.a \
               libITKFEM-4.13.a \
               libITKIOTransformMatlab-4.13.a \
               libitkgdcmcharls-4.13.a \
               libITKIOVTK-4.13.a \
               libitkgdcmCommon-4.13.a \        
               libITKIOXML-4.13.a \
               libitkgdcmDICT-4.13.a \        
               libitkjpeg-4.13.a \
               libitkgdcmDSED-4.13.a \          
               libITKKLMRegionGrowing-4.13.a \
               libitkgdcmIOD-4.13.a \        
               libITKLabelMap-4.13.a \
               libitkgdcmjpeg12-4.13.a \
               libitklbfgs-4.13.a \
               libitkgdcmjpeg16-4.13.a \       
               libITKMesh-4.13.a \
               libitkgdcmjpeg8-4.13.a \        
               libITKMetaIO-4.13.a \
               libitkgdcmMEXD-4.13.a \         
               libitkminc2-4.13.a \
               libitkgdcmMSFF-4.13.a \          
               libitknetlib-4.13.a \
               libitkgdcmopenjp2-4.13.a \       
               libitkNetlibSlatec-4.13.a \
               libitkgdcmsocketxx-4.13.a \     
               libITKniftiio-4.13.a \
               libitkgdcmuuid-4.13.a \      
               libITKNrrdIO-4.13.a \
               libITKgiftiio-4.13.a \          
               libITKOptimizers-4.13.a \
               libitkgtest-4.13.a \      
               libITKOptimizersv4-4.13.a \
               libitkgtest_main-4.13.a \    
               libITKPath-4.13.a \
               libitkhdf5.a \        
               libitkpng-4.13.a \
               libitkhdf5_cpp.a \             
               libITKPolynomials-4.13.a \
               libITKIOBioRad-4.13.a \     
               libITKQuadEdgeMesh-4.13.a \
               libITKIOBMP-4.13.a \    
               libITKSpatialObjects-4.13.a \
               libITKIOBruker-4.13.a \  
               libITKStatistics-4.13.a \
               libITKIOCSV-4.13.a \      
               libitksys-4.13.a \
               libITKIOGDCM-4.13.a \            
               libitktestlib-4.13.a \
               libITKIOGE-4.13.a \          
               libitktiff-4.13.a \
               libITKIOGIPL-4.13.a \            
               libITKTransform-4.13.a \
               libITKIOHDF5-4.13.a \       
               libITKTransformFactory-4.13.a \
               libITKIOImageBase-4.13.a \
               libitkv3p_netlib-4.13.a \
               libITKIOIPL-4.13.a \        
               libitkvcl-4.13.a \
               libITKIOJPEG-4.13.a \           
               libITKVideoCore-4.13.a \
               libITKIOLSM-4.13.a \       
               libITKVideoIO-4.13.a \
               libITKIOMesh-4.13.a \         
               libitkvnl-4.13.a \
               libITKIOMeta-4.13.a \           
               libitkvnl_algo-4.13.a \
               libITKIOMINC-4.13.a \        
               libITKVNLInstantiation-4.13.a \
               libITKIOMRC-4.13.a \
               libITKVTK-4.13.a \
               libITKIONIFTI-4.13.a \           
               libITKWatersheds-4.13.a \
               libITKIONRRD-4.13.a \      
               libitkzlib-4.13.a \
               libITKIOPNG-4.13.a \            
               libITKznz-4.13.a \
}

message($$INCLUDEPATH)
message($$LIBS)

# -----------------------------------------------------------------------------

# -- ML Projects -------------------------------------------------------------

# -- Inventor Projects -------------------------------------------------------

# -- Shared Projects ---------------------------------------------------------

# End of projects ------------------------------------------------------------

}