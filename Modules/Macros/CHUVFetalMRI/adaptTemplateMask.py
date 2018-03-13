# ----------------------------------------------------------------------------

# 
#  \file    adaptTemplateMask.py
#  \author  FetalMRI
#  \date    2018-02-26
#
#  

# ----------------------------------------------------------------------------

from mevis import *

def updateCSO():
  
  print("update CSO")
  ctx.field("CSOManager1.removeAllCSOsAndGroups").touch()
  ctx.field("CSOIsoGenerator.apply").touch()
    