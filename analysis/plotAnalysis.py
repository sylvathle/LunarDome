import pandas as pd
import matplotlib.pyplot as plt
import sys,os
import numpy as np
from plotDoses import plot_icru_contributions,nsimul_particles,plot_DE_Q_Particles,plot_DE_Organs,model_dose,print_icru_stats,print_icrp_stats

plt.rcParams['xtick.labelsize'] = 13
plt.rcParams['ytick.labelsize'] = 13


# Load overall Icru sphere data simulated
df_Icru = pd.read_csv("data/Icrudose.csv")

print_icru_stats(df_Icru)

# First Graph in result section
plot_icru_contributions(df_Icru)



df_ICRP = pd.read_csv("data/ICRPdose.csv")

print_icrp_stats(df_ICRP)

# This 3 lines are just here to compute an overall Dose equivalent for the ICRP145 (to compare with effective dose).
df_group = df_ICRP.groupby(by=["thick","mass[g]","group","wT"],as_index=False).sum()
df_Deq_ICRP = df_group[df_group["thick"]==0]
df_Deq_ICRP['DE'] = df_Deq_ICRP["DE"]*df_Deq_ICRP["mass[g]"]
print ("Dose equivalent for the whole human body: ", df_Deq_ICRP["DE"].sum()/df_Deq_ICRP["mass[g]"].sum())


nsimul_particles(df_ICRP,df_Icru)
plot_DE_Q_Particles(df_ICRP)
df_cancer = plot_DE_Organs(df_ICRP)


#model_dose(df_cancer,df_Icru)
model_dose(df_ICRP,df_Icru)




