import pandas as pd
import sys, os
from os import listdir
from os.path import isfile, join
import numpy as np

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

toZA= {"H":[1,1], "He":[2,4], "Li":[3,7], "Be":[4,9], "B":[5,11], "C":[6,12], "N":[7,14], "O":[8,16], "F":[9,19], "Ne":[10,21], "Na":[11,23], "Mg":[12,24], "Al":[13,27], "Si":[14,28], "P":[15,30], "S":[16,32], "Cl":[17,35], "Ar":[18,40], "K":[19,39], "Ca":[20,40], "Sc":[21,45], "Ti":[22,48], "V":[23,51], "Cr":[24,52], "Mn":[25,55], "Fe":[26,56], "Co":[27,59], "Ni":[28,59], "proton":[1,1]}

def extendIonName(ion):
  return ion+str(toZA[ion][1])

def shortenIonName(ion):
  for II in toZA.keys():
    if ion==extendIonName(II):
      return II
  if ion=="proton": return "H"
  if ion=="alpha": return "He"
  return ""

def getZ(ion):
  for II in toZA.keys():
    if ion==extendIonName(II):
      return toZA[II][0]
  if ion=="proton": return 1
  if ion=="alpha": return 2
  return -1


#res_dir = sys.argv[1]

res_dir = "../results/ICRP145/"
#res_dir = "../results/IcruSphere/"

list_exp = [x[0] for x in os.walk(res_dir)][1:]
csv_types = {"flux":"InnerFlux.csv", "npart":"NParticles.csv","dose":"Doses.csv","wP":"weightParticle.csv","body":"TotalBody.csv"}
#dict_flux = {"thick":[],"ipart":[],"opart":[],"bin":[],"iN":[]}

#mission_factor = 365*24*3600  # duration
#radius = 0.5
radius = 2.0
surface = 2 * np.pi * radius**2

mission_factor =  3.0/4.0 * 0.5 / surface # cutting half sphere give flux in npart . m-2  s-1

df_out = pd.DataFrame()
df_in = pd.DataFrame()

#looked_spectra = ["proton","alpha","neutron"]

ndir=0


for dir_path in list_exp:

  dir_path_split = dir_path.split("/")
  dir_exp = dir_path_split[-1]

  rego_thickness = 0
  #if "Rego" in dir_exp: rego_thickness = int(dir_exp.split("-")[-1].replace("Rego",""))
  if "Rego" in dir_exp: rego_thickness = int(dir_exp.split("_")[-1].replace("Rego",""))
  #print ("thick = ",rego_thickness)
  #if rego_thickness!=0: continue

  csv_files =  [f for f in listdir(dir_path) if isfile(join(dir_path, f))]

  runprefix = extract_prefixes(csv_files)

  last_event = 0

  for run in runprefix:

    csv_nparticle = run+"_nt_"+csv_types["npart"]
    csv_npart_path = dir_path+"/"+csv_nparticle
    if not os.path.exists(csv_npart_path): continue
    cols = extract_column_names(csv_npart_path)
    df_npart = pd.read_csv(csv_npart_path,names=cols,skiprows=len(cols)+4)
    if len(df_npart)==0: continue
    if int(df_npart["Fe"].iloc[0])==0: continue

    csv_wp = run+"_nt_"+csv_types["wP"]
    csv_wp_path = dir_path+"/"+csv_wp
    if not os.path.exists(csv_wp_path): continue
    cols = extract_column_names(csv_wp_path)
    df_wp = pd.read_csv(csv_wp_path,names=cols,skiprows=len(cols)+4)
    if len(df_wp)==0: continue

    csv_flux = run+"_nt_"+csv_types["flux"]
    csv_flux_path = dir_path+"/"+csv_flux
    if not os.path.exists(csv_flux_path): continue
    cols = extract_column_names(csv_flux_path)
    if "okE" not in cols: continue
    df_flux = pd.read_csv(csv_flux_path,names=cols,skiprows=len(cols)+4)
    df_flux = df_flux[df_flux["count"]!=0]
    if len(df_flux)==0: continue
    #df_flux.set_index(["Iparticle","Oparticle","okE"],inplace=True)
    df_flux['thick'] = rego_thickness

    ions = df_flux["Iparticle"].tolist()
    df_flux["Z"] = [getZ(ion) for ion in ions]
    #df_flux = df_flux.merge(df_wp[['Z','wP']], on='Z', how='left')
    
    #df_flux["iN"]  = [df_npart[shortenIonName(ion)].sum() for ion in ions]
    df_npart = df_npart.transpose()
    df_npart.rename({0:rego_thickness},axis=1,inplace=True)
    if len(df_in)==0: df_in = df_npart.copy()
    elif rego_thickness in df_in.columns.tolist():
      df_in[rego_thickness] = df_in[rego_thickness]+df_npart[rego_thickness]
    else: 
      df_in[rego_thickness] = df_npart[rego_thickness]

    if len(df_out)==0: 
      df_out = df_flux.copy()
      #with pd.option_context('display.max_rows', None, 'display.max_columns', None):
      #  print (df_out[(df_out["Iparticle"]=="proton") & (df_out["Oparticle"]=="proton")])
    else:
      df_out = pd.merge(df_out, df_flux, on=["Z","Iparticle","Oparticle","okE","thick"], how='outer', suffixes=('_df1', '_df2'))
      df_out['count'] = df_out['count_df1'].fillna(0) + df_out['count_df2'].fillna(0)
      #df_out['iN'] = df_out['iN_df1'].fillna(0) + df_out['iN_df2'].fillna(0)
      df_out.drop(['count_df1', 'count_df2'], axis=1, inplace=True)
      #df_out.drop(['iN_df1', 'iN_df2'], axis=1, inplace=True)
    ndir=ndir+1
      
#    if ndir>100: break


particles = ["proton","alpha"]
df_in["Iparticle"] = particles + [v+str(toZA[v][1]) for v in df_in.index[2:]]

df_inm = pd.DataFrame()
for thick in df_in.columns:
  if thick=="Iparticle": continue
  df_temp = pd.DataFrame()
  df_temp["Iparticle"] = df_in["Iparticle"]
  df_temp["iN"] = df_in[thick]
  df_temp["thick"]=thick
  df_inm=pd.concat([df_inm,df_temp])
df_out = df_out.merge(df_wp[['Z','wP']], on='Z', how='left')
df_out = df_out.merge(df_inm, on=['Iparticle','thick'],how='left')
df_out["count"] = mission_factor * df_out["count"] * df_out["wP"] / df_out["iN"]
df_out.drop(["wP","iN"],inplace=True,axis=1)

#print (df_out)

df_out.to_csv(res_dir+"flux.csv",index=False)
