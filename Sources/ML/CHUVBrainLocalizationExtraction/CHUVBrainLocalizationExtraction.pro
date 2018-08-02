# -----------------------------------------------------------------------------
# CHUVBrainLocalizationExtraction project profile
#
# \file
# \author  FetalMRI
# \date    2018-07-17
# -----------------------------------------------------------------------------


TEMPLATE   = lib
TARGET     = CHUVBrainLocalizationExtraction

DESTDIR    = $$(MLAB_CURRENT_PACKAGE_DIR)/lib
DLLDESTDIR = $$(MLAB_CURRENT_PACKAGE_DIR)/lib

# Set high warn level (warn 4 on MSVC)
WARN = HIGH

# Add used projects here (see included pri files below for available projects)
CONFIG += dll ML MLBackgroundTasks boost MyBrainLocalization MyITK

MLAB_PACKAGES += mevisFetalMRI_MRUser \
                 MeVisLab_Standard

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += CHUVBRAINLOCALIZATIONEXTRACTION_EXPORTS

# Enable ML deprecated API warnings. To completely disable the deprecated API, change WARN to DISABLE.
DEFINES += ML_WARN_DEPRECATED

win32 {
QMAKE_CXXFLAGS += /openmp
}

linux {
QMAKE_CXXFLAGS += -fopenmp
LIBS += -fopenmp
}

HEADERS += \
    CHUVBrainLocalizationExtractionInit.h \
    CHUVBrainLocalizationExtractionSystem.h \
    mlmialRefineMask.h \

SOURCES += \
    CHUVBrainLocalizationExtractionInit.cpp \
    mlmialRefineMask.cpp \