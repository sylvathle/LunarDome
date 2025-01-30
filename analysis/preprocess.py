import pandas as pd
#import matplotlib.pyplot as plt
import sys,os
from os import listdir
from os.path import isfile, join
import numpy as np

pd.options.mode.copy_on_write = True

Nsample = 10

toZA= {"H":[1,1], "He":[2,4], "Li":[3,7], "Be":[4,9], "B":[5,11], "C":[6,12], "N":[7,14], "O":[8,16], "F":[9,19], "Ne":[10,21], "Na":[11,23], "Mg":[12,24], "Al":[13,27], "Si":[14,28], "P":[15,30], "S":[16,32], "Cl":[17,35], "Ar":[18,40], "K":[19,39], "Ca":[20,40], "Sc":[21,45], "Ti":[22,48], "V":[23,51], "Cr":[24,52], "Mn":[25,55], "Fe":[26,56], "Co":[27,59], "Ni":[28,59]}

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


def brut_to_dose(res_dir):
    list_exp = [x[0] for x in os.walk(res_dir)][1:]
    csv_types = {"flux":"InnerFlux.csv", "npart":"NParticles.csv","dose":"Doses.csv","wP":"weightParticle.csv","body":"TotalBody.csv"}
    
    dict_dose = {"thick":[],"eventId":[],"ipart":[],"iN":[],"organId":[],"DE":[],"Dose":[]}
    
    for dir_path in list_exp:
    
      dir_path_split = dir_path.split("/")
      dir_exp = dir_path_split[-1]
    
      rego_thickness = 0
      #if "Rego" in dir_exp: rego_thickness = int(dir_exp.split("-")[-1].replace("Rego",""))
      if "Rego" in dir_exp: 
        if "IcruSphere" in dir_path: rego_thickness = int(dir_exp.split("-")[-1].replace("Rego",""))
        elif "ICRP145" in dir_path: rego_thickness = int(dir_exp.split("_")[-1].replace("Rego",""))
    
    
    
      csv_files =  [f for f in listdir(dir_path) if isfile(join(dir_path, f))]
    
      runprefix = extract_prefixes(csv_files)
    
      last_event = 0
    
    
      #for k,name in csv_types.items():
      for run in runprefix:
        csv_nparticle = run+"_nt_"+csv_types["npart"]
        csv_npart_path = dir_path+"/"+csv_nparticle
        if not os.path.exists(csv_npart_path): continue
        cols = extract_column_names(csv_npart_path)
        df_npart = pd.read_csv(csv_npart_path,names=cols,skiprows=len(cols)+4)
    
        csv_dose = run+"_nt_"+csv_types["dose"]
        csv_dose_path = dir_path+"/"+csv_dose
        if not os.path.exists(csv_dose_path): continue
        cols = extract_column_names(csv_dose_path)
        df_dose = pd.read_csv(csv_dose_path,names=cols,skiprows=len(cols)+4)
    
        #print (csv_nparticle)
        #print (df_dose)
    
       # for index, row in df_dose.iterrows():
        for index, row in df_dose.iterrows():
          for ion in df_npart.columns:
            if row["idEvent"] not in df_npart.index: continue
            iN = df_npart[ion].loc[row["idEvent"]]
            if iN==0: continue
            dict_dose["thick"].append(rego_thickness)
            dict_dose["eventId"].append(row["idEvent"]+last_event)
            dict_dose["ipart"].append(ion)
            dict_dose["iN"].append(iN)
            dict_dose["organId"].append(row["organId"])
            dict_dose["DE"].append(row[ion+"_EDE"])
            dict_dose["Dose"].append(row[ion+"_Dose"])
    
        #print (dict_dose)
        if len(dict_dose["eventId"])==0: continue
        last_event = max(dict_dose["eventId"])+1
    
    df_dose_out = pd.DataFrame(dict_dose)
    df_dose_out.to_csv(res_dir+"/dose.csv",index=False)


def get_stats(group):

    de_mean = (group['DE'] * group['iN']).sum() / group['iN'].sum()
    dose_mean = (group['Dose'] * group['iN']).sum() / group['iN'].sum()

    low_de_group = group[group['DE']<de_mean]
    de_low_std = np.sqrt((low_de_group['iN'] * (low_de_group['DE'] - de_mean) ** 2).sum() / low_de_group['iN'].sum())

    up_de_group = group[group['DE']>=de_mean]
    de_up_std = np.sqrt((up_de_group['iN'] * (up_de_group['DE'] - de_mean) ** 2).sum() / up_de_group['iN'].sum())

    low_dose_group = group[group['Dose']<dose_mean]
    dose_low_std = np.sqrt((low_dose_group['iN'] * (low_dose_group['Dose'] - dose_mean) ** 2).sum() / low_dose_group['iN'].sum())

    up_dose_group = group[group['Dose']>=dose_mean]
    dose_up_std = np.sqrt((up_dose_group['iN'] * (up_dose_group['Dose'] - dose_mean) ** 2).sum() / up_dose_group['iN'].sum())

    return pd.Series({'iN':group['iN'].sum(),'DE': de_mean, 'Dose': dose_mean,'DE_low_std': de_low_std,'DE_up_std': de_up_std,'Dose_low_std': dose_low_std,'Dose_up_std': dose_up_std})


# Calculate the weighted standard deviation function
def weighted_std(group):
    w_mean = (group['DE'] * group['iN']).sum() / group['iN'].sum()
    d_mean = (group['Dose'] * group['iN']).sum() / group['iN'].sum()
    w_var = (group['iN'] * (group['DE'] - w_mean) ** 2).sum() / group['iN'].sum()
    d_var = (group['iN'] * (group['Dose'] - d_mean) ** 2).sum() / group['iN'].sum()
    w_std = np.sqrt(w_var)
    dose_std = np.sqrt(d_var)
    return pd.Series({'DE_std': w_std,'Dose_std':dose_std,'iN':group['iN'].sum()})

# Function that redefine the eventId value based on the number of sampled wished to compute the standard deviation
def regroup_events(group):
    group['eventId'] = [i for i in range(len(group))]
    group['eventId'] = group['eventId'] % Nsample
    group['eventId'] = group['eventId'].astype(int)
    return group






def pre_process_doses(source):

    # organs file from where we take the weithing tissue factors
    df_organs = pd.read_csv("organDoses.csv")
    df_organs.dropna(subset=["organId"],inplace=True)

    # Read dose file and add columns using id of organs as pivot, group name of organ, the mass of each organ
    df = pd.read_csv(source)
    df = df.merge(df_organs[['organId', 'group', 'wT','mass[g]']], on='organId', how='left')

    # Sort content of doses to get runs with more simulated particles on top.
    df  = df.sort_values(by=["thick","group","iN"],ascending=False)

    # Regroup runs applying the regroup_events funtion, this just adds a label 
    #    ensuring we get groups of runs with similar number of simulated particles
    df = df.groupby(['thick','organId',"wT","group","ipart"],as_index=False).apply(regroup_events)

    # Preparing to average the doses for each regrouped sample weighted by the number of simulated primaries
    df["DE"] = df["DE"]*df["iN"]
    df["Dose"] = df["Dose"]*df["iN"]


    # Average over eventId grouped samples, number of particles simulated is summed.
    df = df.groupby(['thick','organId',"wT","group","eventId","mass[g]","ipart"],as_index=False).agg({'DE': 'mean', 'Dose':'mean','iN': ['sum','count']}).reset_index()

    #  Adapt columns names
    new_columns = []
    for cols in df.columns:
        if cols[1]!='count': new_columns.append(cols[0])
        else: new_columns.append(cols[1])

    df.columns = new_columns

    # Sum all the doses belonging to the same group, we should obtain a dose per organId, per primary particle population
    df["DE"] = df["DE"]/df["iN"]*df['count']
    df["Dose"] = df["Dose"]/df["iN"]*df['count']

    # In order to properly regroup organs with bigger group ('group') doses are multiplied by masses 
    #        to be redivded by the mass of the group organ
    df['DE'] = df['DE']*df['mass[g]']
    df['Dose'] = df['Dose']*df['mass[g]']

    # Group organs, just sum values of doses and Deq
    df = df.groupby(by=["thick","group","eventId",'iN','ipart'],as_index=False).sum()

    # Redivide masses
    df["DE"] = df["DE"]/df['mass[g]']
    df["Dose"] = df["Dose"]/df['mass[g]']

    partlist = df["ipart"].tolist()
    Zlist = [toZA[part][0] for part in partlist]
    df["Z"] = Zlist

    # Merge all eventId
    df = df.groupby(by=["thick","group","mass[g]","wT","ipart","Z"],as_index=False).apply(lambda x: pd.concat([get_stats(x)], axis=0))
    df["iN"] = df["iN"].astype(int)   

    return df

def pre_process_Icru(source):
    df = pd.read_csv(source)

    df = df.groupby(['thick','ipart'],as_index=False).apply(regroup_events)
    df = df.groupby(by=["thick","ipart","eventId"],as_index=False).agg({'DE': 'mean', 'Dose':'mean','iN': ['sum','count']}).reset_index()
    
    #  Adapt columns names
    new_columns = []
    for cols in df.columns:
        if cols[1]!='count': new_columns.append(cols[0])
        else: new_columns.append(cols[1])
    df.columns = new_columns

    partlist = df["ipart"].tolist()
    Zlist = [toZA[part][0] for part in partlist]
    df["Z"] = Zlist


    df = df.groupby(by=["thick","ipart","Z"],as_index=False).apply(lambda x: pd.concat([get_stats(x)], axis=0))
    df["iN"] = df["iN"].astype(int)   

    return df
