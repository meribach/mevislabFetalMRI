# -----------------------------------------------------------------------------
# CHUVImageReconstruction project profile
#
# \file
# \author  FetalMRI
# \date    2018-06-11
# -----------------------------------------------------------------------------


TEMPLATE   = lib
TARGET     = CHUVImageReconstruction

DESTDIR    = $$(MLAB_CURRENT_PACKAGE_DIR)/lib
DLLDESTDIR = $$(MLAB_CURRENT_PACKAGE_DIR)/lib

# Set high warn level (warn 4 on MSVC)
WARN = HIGH

# Add used projects here (see included pri files below for available projects)
CONFIG += dll ML MLBackgroundTasks boost MyImageReconstruction MyITK

MLAB_PACKAGES += mevisFetalMRI_MRUser \
                 MeVisLab_Standard

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += CHUVIMAGERECONSTRUCTION_EXPORTS

# Enable ML deprecated API warnings. To completely disable the deprecated API, change WARN to DISABLE.
DEFINES += ML_WARN_DEPRECATED

win32 {
QMAKE_CXXFLAGS += /openmp
}


HEADERS += \
    CHUVImageReconstructionInit.h \
    CHUVImageReconstructionSystem.h \
    mlmialImageReconstruction.h \

SOURCES += \
    CHUVImageReconstructionInit.cpp \
    mlmialImageReconstruction.cpp \