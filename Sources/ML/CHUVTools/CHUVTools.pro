# -----------------------------------------------------------------------------
# CHUVTools project profile
#
# \file
# \author  FetalMRI
# \date    2018-04-12
# -----------------------------------------------------------------------------


TEMPLATE   = lib
TARGET     = CHUVTools

DESTDIR    = $$(MLAB_CURRENT_PACKAGE_DIR)/lib
DLLDESTDIR = $$(MLAB_CURRENT_PACKAGE_DIR)/lib

# Set high warn level (warn 4 on MSVC)
WARN = HIGH

# Add used projects here (see included pri files below for available projects)
CONFIG += dll ML MLBackgroundTasks boost MyTools MyITK

MLAB_PACKAGES += CHUV_FetalMRI \
                 MeVisLab_Standard \

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += CHUVTOOLS_EXPORTS

# Enable ML deprecated API warnings. To completely disable the deprecated API, change WARN to DISABLE.
DEFINES += ML_WARN_DEPRECATED

#DEFINES += _UNICODE \
#      _AFXDLL \

win32 {
QMAKE_CXXFLAGS += /openmp
}

linux {
QMAKE_CXXFLAGS += -fopenmp
LIBS += -fopenmp
}



HEADERS += \
    CHUVToolsInit.h \
    CHUVToolsSystem.h \
    mlmialOrientImage.h \
	mlmevisbtkDenoising.h \

SOURCES += \
    CHUVToolsInit.cpp \
    mlmialOrientImage.cpp \
	mlmevisbtkDenoising.cpp \
