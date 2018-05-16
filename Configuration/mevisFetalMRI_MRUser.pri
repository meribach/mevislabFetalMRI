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
				 btkNLMDenoisingWrapperLib.lib \
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
}

message($$INCLUDEPATH)
message($$LIBS)

# -----------------------------------------------------------------------------

# -- ML Projects -------------------------------------------------------------

# -- Inventor Projects -------------------------------------------------------

# -- Shared Projects ---------------------------------------------------------

# End of projects ------------------------------------------------------------

}