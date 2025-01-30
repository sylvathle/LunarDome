import numpy as np
import pandas as pd
from os import listdir
from os.path import isfile, join
import sys,os

from preprocess import brut_to_dose, pre_process_doses, pre_process_Icru, toZA

def extract_prefixes(lst):
    prefixes = set()
    for string in lst:
        prefix = string.split('_')[0]
        prefixes.add(prefix)
    return list(prefixes)

def extract_column_names(csv_file):
    column_names = []
    with open(csv_file, 'r') as f:
        line_num = 1
        for line in f:
            if len(line) > 0 and line[0]=='#':
                # Assuming the column names are in the 5th line after the symbol '#'
                if line_num >= 5:
                    column_names.append(line.split()[-1])  # Last word of the line contains the column names
            line_num+=1                
    return column_names




  
  
# Number of packet of particles used to computed a standard deviation
brut_to_dose("../results/ICRP145/")
df = pre_process_doses('../results/ICRP145/dose.csv')
df.to_csv("ICRPdose.csv",index=False)


#########################################
#    ICRU SPhere part
#########################################
brut_to_dose("../results/IcruSphere/")
df_Icru = pre_process_Icru('../results/IcruSphere/dose.csv')
df_Icru.to_csv("Icrudose.csv",index=False)

