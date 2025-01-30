# This is a regolith model dependence of the EDE in HP
import sys
import numpy as np

# Variables for the 3 scenarios simulated
# DE = Dose Equivalent
# EDE = Effective dose equivalent
# ICRU corresponds to ICRU sphere
# IFHP corresponds to the ICRP 145 female human phantom
variables = {
	"DE_ICRU":{"h0":322.74172359616847,"q":0.0005018739771447014,"p":0.017194342934466315,"k":0.0445674750923133,"lambda":0.0074959031591236},
	"DE_IFHP":{"h0":332.6746015203524,"q":0.000642952424075348,"p":0.030066975592318915,"k":0.0602171010678517,"lambda":0.007761865813053806},
	"EDE_IFHP":{"h0":291.0883292613871,"q":0.0006891268978707298,"p":0.035171705157773535,"k":0.06293506203801,"lambda":0.007773601094636541}
}

# Function taking as input the areal thickness in g/cm2 (float), the name of the scenario (string)
# It return the Dose Equivalent or Effective Dose Equivalent rate in mSv/y
def HE(a,scenario="DE_ICRU"):
  params = variables[scenario]
  return params["h0"]*(params["q"]*a**2 + params["p"]*a + 1.0)/(params["k"]*a+1.0)*np.exp(-params["lambda"]*a)

a = float(sys.argv[1])
scen = sys.argv[2]
print (a,scen)
print ("HE = ",str(round(HE(a,scen),3))+" mSv/y")

