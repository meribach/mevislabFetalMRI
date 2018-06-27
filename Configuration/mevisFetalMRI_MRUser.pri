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
   win32:LIBS += mialsrtkOrientImageLib.lib \
                 mialsrtkOrientImageWrapper.lib \
		 btkNLMDenoisingLib.lib \
		 btkNLMDenoisingWrapper.lib \
}

MyIntensityStandardization {
   CONFIG_FOUND += MyIntensityStandardization 
   INCLUDEPATH += $${PACKAGE_SOURCES}/Shared/MyIntensityStandardization  
   QMAKE_LIBDIR += $${PACKAGE_LIB}/MyIntensityStandardization 
   win32:LIBS += mialsrtkCorrectSliceIntensityLib.lib \
                 mialsrtkCorrectSliceIntensityWrapper.lib \
                 mialsrtkIntensityStandardizationLib.lib \
                 mialsrtkIntensityStandardizationWrapper.lib \
                 mialsrtkSliceBySliceCorrectBiasFieldLib.lib \
                 mialsrtkSliceBySliceCorrectBiasFieldWrapper.lib \
                 mialsrtkSliceBySliceN4BiasFieldCorrectionLib.lib \
                 mialsrtkSliceBySliceN4BiasFieldCorrectionWrapper.lib \
}

MyImageReconstruction {
   CONFIG_FOUND += MyImageReconstruction
   INCLUDEPATH += $${PACKAGE_SOURCES}/Shared/MyImageReconstruction
   QMAKE_LIBDIR += $${PACKAGE_LIB}/MyImageReconstruction 
   win32:LIBS += mialsrtkImageReconstructionLib.lib \
                 mialsrtkImageReconstructionWrapper.lib \

}

MyTVSuperResolution {
   CONFIG_FOUND += MyTVSuperResolution 
   INCLUDEPATH += $${PACKAGE_SOURCES}/Shared/MyTVSuperResolution 
   QMAKE_LIBDIR += $${PACKAGE_LIB}/MyTVSuperResolution
   win32:LIBS += mialsrtkTVSuperResolutionLib.lib \
                 mialsrtkTVSuperResolutionWrapper.lib \
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
		psapi.lib \

		
  unix:LIBS += libITKIOSpatialObjects-5.0.a \
	       libITKBiasCorrection-5.0.a \
	       libITKIOStimulate-5.0.a \
	       libITKCommon-5.0.a \
	       libITKIOTIFF-5.0.a \
	       libITKDICOMParser-5.0.a \
	       libITKIOTransformBase-5.0.a \
	       libitkdouble-conversion-5.0.a \
	       libITKIOTransformHDF5-5.0.a \
	       libITKEXPAT-5.0.a \
	       libITKIOTransformInsightLegacy-5.0.a \
	       libITKFEM-5.0.a \
	       libITKIOTransformMatlab-5.0.a \
	       libitkgdcmcharls-5.0.a \
	       libITKIOVTK-5.0.a \
	       libitkgdcmCommon-5.0.a \
	       libITKIOXML-5.0.a \
	       libitkgdcmDICT-5.0.a \
	       libitkjpeg-5.0.a \
	       libitkgdcmDSED-5.0.a \
	       libITKKLMRegionGrowing-5.0.a \
	       libitkgdcmIOD-5.0.a \
	       libITKLabelMap-5.0.a \
	       libitkgdcmjpeg12-5.0.a \
	       libitklbfgs-5.0.a \
	       libitkgdcmjpeg16-5.0.a \
	       libITKMesh-5.0.a \
	       libitkgdcmjpeg8-5.0.a \
	       libITKMetaIO-5.0.a \
	       libitkgdcmMEXD-5.0.a \
	       libitkminc2-5.0.a \
	       libitkgdcmMSFF-5.0.a \
	       libitknetlib-5.0.a \
	       libitkgdcmopenjp2-5.0.a \
	       libitkNetlibSlatec-5.0.a \
	       libitkgdcmsocketxx-5.0.a \
	       libITKniftiio-5.0.a \
	       libitkgdcmuuid-5.0.a \
	       libITKNrrdIO-5.0.a \
	       libITKgiftiio-5.0.a \
	       libITKOptimizers-5.0.a \
	       libitkgtest-5.0.a \
	       libITKOptimizersv4-5.0.a \
	       libitkgtest_main-5.0.a \
	       libITKPath-5.0.a \
	       libitkhdf5.a \
	       libitkpng-5.0.a \
	       libitkhdf5_cpp.a \
	       libITKPolynomials-5.0.a \
	       libITKIOBioRad-5.0.a \
	       libITKQuadEdgeMesh-5.0.a \
	       libITKIOBMP-5.0.a \
	       libITKSpatialObjects-5.0.a \
	       libITKIOBruker-5.0.a \
	       libITKStatistics-5.0.a \
	       libITKIOCSV-5.0.a \
	       libitksys-5.0.a \
	       libITKIOGDCM-5.0.a \
	       libitktestlib-5.0.a \
	       libITKIOGE-5.0.a \ 
	       libitktiff-5.0.a \
	       libITKIOGIPL-5.0.a \
	       libITKTransform-5.0.a \
	       libITKIOHDF5-5.0.a \
	       libITKTransformFactory-5.0.a \
	       libITKIOImageBase-5.0.a \ 
	       libitkv3p_netlib-5.0.a \
	       libITKIOIPL-5.0.a \       
	       libitkvcl-5.0.a \
	       libITKIOJPEG-5.0.a \             
	       libITKVideoCore-5.0.a \
	       libITKIOLSM-5.0.a \       
	       libITKVideoIO-5.0.a \
	       libITKIOMesh-5.0.a \          
	       libitkvnl-5.0.a \
	       libITKIOMeta-5.0.a \  
	       libitkvnl_algo-5.0.a \
	       libITKIOMINC-5.0.a \         
	       libITKVNLInstantiation-5.0.a \
	       libITKIOMRC-5.0.a \           
	       libITKVTK-5.0.a \
	       libITKIONIFTI-5.0.a \            
	       libITKWatersheds-5.0.a \
	       libITKIONRRD-5.0.a \             
	       libitkzlib-5.0.a \
	       libITKIOPNG-5.0.a \
	       libITKznz-5.0.a \
	       libITKIOSiemens-5.0.a \  
}

message($$INCLUDEPATH)
message($$LIBS)

# -----------------------------------------------------------------------------

# -- ML Projects -------------------------------------------------------------

# -- Inventor Projects -------------------------------------------------------

# -- Shared Projects ---------------------------------------------------------

# End of projects ------------------------------------------------------------

}