import sys
import pandas as pd
import numpy as np
#import seaborn as sns
import matplotlib.pyplot as plt

from preprocess import toZA
from colors import Ioncolors,color_thick,get_thick_color
from scipy.optimize import curve_fit

pd.options.mode.copy_on_write = True

# Type of statistical information used in the analysis:
#    - lower standard deviation
#    - upper standard deviation
#    - average (labelled with empty string)
stat_info = ["_low_std","_up_std",""]
part_label = ["H","He","C","O","Ne","Mg","Si","Ar","S","Ti","Cr","Ca","Fe","Ni"]
part_ytxt = [0 for ip in range(len(part_label))]

thick_col = ['#ff0000', '#bf0040', '#7f007f', '#4000bf', '#0000ff']
#thick_col = [
#    "#FF0000",  # Red
#    "#FF3F3F",  # Lighter red
#    "#FF7F7F",  # Light red
#    "#FFBFBF",  # Very light red
#    "#BFBFFF",  # Light blue
#    "#7F7FFF",  # Light blue
#    "#0000FF"   # Blue
#]


def label_thick(thick):
     if thick==0: return '1.136 $g.cm^{-2}$ Al-2219'
     return 'Al + '+str(int(thick*1.5))+' $g.cm^{-2}$ regolith'

def print_icru_stats(df_ICRU):

  # Get list of thicknesses
  list_icru_thick = df_ICRU["thick"].unique().tolist()
  print ("\n*********************************************\n")
  print ("\t Stats icru sphere")
  print ("\n*********************************************\n")
  print (df_ICRU)
  #df_sum =  df_ICRU.groupby(by=["thick"]).sum()
  pd.options.display.float_format = '{:.3f}'.format
  for thick in list_icru_thick:
    print ("\n*********************************************\n")
    df_thick = df_ICRU[df_ICRU["thick"]==thick][["thick","ipart","iN","DE","DE_low_std","DE_up_std"]]
    total = float(df_thick["DE"].sum())
    std_up_total = float(df_thick["DE_up_std"].sum())
    std_low_total = float(df_thick["DE_low_std"].sum())
    print ("----------------------")
    print ("thickness = " ,thick,"cm")
    print ("----------------------")
    print (" -> Nsim = " ,df_thick["iN"].sum())
    print (" -> DE   = ",'{:.2f}'.format(total),"-",str('{:.2f}'.format(std_low_total))+" mSv","+",str('{:.2f}'.format(std_up_total))+" mSv")
    print ("Main std :")
    df_thick["dev(%)"] = (abs(df_thick["DE_low_std"])+abs(df_thick["DE_up_std"]))/total*100
    df_thick.sort_values(by=["dev(%)","thick"],ascending=False,inplace=True)
    df_thick = df_thick[df_thick["dev(%)"]>0.5][["ipart","iN","DE","dev(%)"]]
    df_thick.set_index("ipart",inplace=True)
    df_thick.index.name = None
    #df_thick = df_thick.transpose()
    print (df_thick)
  print ("\n\n ")

def plot_icru_contributions(df_ICRU):

  # Sort per thickness and ion charge
  df_ICRU.sort_values(by=["thick","Z"],ascending=True,inplace=True)
  df_ICRU.iN = df_ICRU.iN.astype(int)
  
  # Get list of primary ions
  list_ions = df_ICRU["ipart"].unique().tolist()
  # Get list of thicknesses
  list_icru_thick = df_ICRU["thick"].unique().tolist()

  
  # Prepare dataframe with percentage of contribution per ion.
  dfp_ICRU = pd.DataFrame()
  dfp_ICRU["thick"] = list_icru_thick #df_ICRU["thick"].unique().tolist()
  dfp_ICRU.set_index("thick",inplace=True)
  
  # Prepare information of total dose equivalent
  for stat in stat_info:
    dfp_ICRU["Total"+stat] = 0
  
  for ion in list_ions:
    dft = df_ICRU[df_ICRU["ipart"]==ion]
    dft.set_index("thick",inplace=True)
    for stat in stat_info:
      dfp_ICRU[ion+stat] = dft["DE"+stat]
      dfp_ICRU["Total"+stat] = dfp_ICRU["Total"+stat]+dft["DE"+stat]
  
  
  list_ions_tobe_plotted = ["H","He","O","Fe"]
  list_ions_plotted = []
  for ion in list_ions_tobe_plotted: 
    if ion in list_ions: 
      list_ions_plotted.append(ion)

  # Concat data of not plotted ions into one serie
  for stat in stat_info: dfp_ICRU["Others"+stat] = 0
  
  for ion in list_ions:
    if ion not in list_ions_plotted:
      for stat in stat_info:
        dfp_ICRU["Others"+stat] = dfp_ICRU["Others"+stat] + dfp_ICRU[ion+stat]
        dfp_ICRU.drop(ion+stat,inplace=True,axis=1)

  
  dfp_ICRU["arealThick"] = dfp_ICRU.index*1.5 # conversion from thickness (cm) to areal thickness or surface density (g/cm2) for a density of 1.5 g/cm3
  
  for p in list_ions_plotted+['Others','Total']:
    for stat in stat_info:  
      dfp_ICRU[p+"_normTot"+stat]=dfp_ICRU[p+stat]/dfp_ICRU["Total"]
      dfp_ICRU[p+"_norm0"+stat]=dfp_ICRU[p+stat]/dfp_ICRU[p].iloc[0]
  
  list_norm = ["_normTot","_norm0"]
  
  fig, ax = plt.subplots(1,2,figsize=(15,7))
  
  for p in list_ions_plotted:
    #for i,norm in enumerate(list_norm):
    norm = "_normTot"
    ax[0].errorbar(dfp_ICRU["arealThick"],100*dfp_ICRU[p+norm],yerr=[100*dfp_ICRU[p+norm+"_low_std"],100*dfp_ICRU[p+norm+"_up_std"]],marker='o',markersize=4,label=p,color=Ioncolors[p])

    norm = "_norm0"
    ax[1].errorbar(dfp_ICRU["arealThick"],dfp_ICRU[p+norm],yerr=[dfp_ICRU[p+norm+"_low_std"],dfp_ICRU[p+norm+"_up_std"]],marker='o',markersize=4,label=p,color=Ioncolors[p])
  
  norm = "_normTot"
  ax[0].errorbar(dfp_ICRU["arealThick"],100*dfp_ICRU["Others"+norm],yerr=[100*dfp_ICRU["Others"+norm+"_low_std"],100*dfp_ICRU["Others"+norm+"_up_std"]],marker='o',markersize=4,label="Others")
  norm = "_norm0"
  ax[1].errorbar(dfp_ICRU["arealThick"],dfp_ICRU["Others"+norm],yerr=[dfp_ICRU["Others"+norm+"_low_std"],dfp_ICRU["Others"+norm+"_up_std"]],marker='o',markersize=4,label="Others")
  ax[1].errorbar(dfp_ICRU["arealThick"],dfp_ICRU["Total"+norm],yerr=[dfp_ICRU["Total"+norm+"_low_std"],dfp_ICRU["Total"+norm+"_up_std"]],linestyle='--',marker='s',markersize=4,label="Total",color='k')

  listnormcol = [c for c in dfp_ICRU.columns if "normTot" in c and "low" not in c and "up" not in c]
  print ("contribution frm primaries")
  print (dfp_ICRU[listnormcol])

  listnormcol = [c for c in dfp_ICRU.columns if "norm0" in c and "low" not in c and "up" not in c]
  print ("Normalized doses")
  print (dfp_ICRU[listnormcol])
  print ("_______________________________________________________")
  
  
  ax[0].set_ylabel("Contribution from primaries (%)",fontsize=15)
  ax[0].set_xlabel("1.136 $g/cm^2$ Al + regolith ($g/cm^2$)",fontsize=15)
  ax[1].set_xlabel("1.136 $g/cm^2$ Al + regolith ($g/cm^2$)",fontsize=15)
  ax[1].set_ylabel("Normalized dose equivalent",fontsize=15)
  ax[0].legend(fontsize=14)
  ax[0].grid(True)
  ax[1].legend(fontsize=14)
  ax[1].grid(True)
  
  plt.subplots_adjust(wspace=0.23)
  plt.savefig("../figures/DEICRUSphere.png",bbox_inches="tight")

def nsimul_particles(df_ICRP,df_ICRU):

    df_ICRP["thick"] = df_ICRP["thick"].astype(int)


    list_thickness = sorted(df_ICRP["thick"].unique())

    #color_thick = ['blue','red','green','orange','magenta']

    ##### Section plotting and saving the number of simulated particles for each king and each scenario #####
    df_particles = df_ICRP.groupby(by=["thick","Z","ipart"]).agg({'iN':['sum','count']}).reset_index()

    new_columns = []
    for cols in df_particles.columns:
        if cols[1]!='count': new_columns.append(cols[0])
        else: new_columns.append(cols[1])
    df_particles.columns = new_columns


    df_particles["iN"] = df_particles["iN"]/df_particles["count"]
    df_particles["iN"] = df_particles["iN"].astype(int)
    df_particles.drop("count",inplace=True,axis=1)

    df_particles = df_particles.sort_values(by=["Z"],ascending=True)
    
    df_nparticles = pd.DataFrame()
    fig,ax = plt.subplots(2,figsize=(15,10),sharex=True, gridspec_kw={'hspace': 0.0})
    
    
    
    for ith,thick in enumerate(list_thickness):
        df_particles_thick = df_particles[df_particles["thick"]==thick]
        df_particles_thick.set_index("ipart",inplace=True)
        ax[0].plot(df_particles_thick.index,df_particles_thick["iN"],color=color_thick[ith],label=label_thick(thick),marker='x',linestyle="-")
        df_nparticles["IFHP_"+str(thick)+"cm"] = df_particles_thick["iN"]
    

    list_icru_thick = df_ICRU["thick"].unique().tolist()

    for ith, thick in enumerate(list_icru_thick):
        df_icru_thick = df_ICRU[df_ICRU["thick"]==thick]
        df_icru_thick.set_index("ipart",inplace=True)
        ax[1].plot(df_icru_thick.index,df_icru_thick["iN"],label="ICRU " + str(thick),marker='^',color=color_thick[ith])
        
        df_nparticles["ICRU_"+str(thick)+"cm"] = df_icru_thick["iN"]
    
    
    for i in range(2):
        ax[i].legend()
        ax[i].grid(True)
        ax[i].set_yscale('log')
        ax[i].set_xlabel('Particle charge (Z)')
        ax[i].set_ylabel("Number of simulated particles")
    plt.savefig("../figures/ParticleNumber.png",bbox_inches="tight")
    plt.close()
    
    
    parts = df_nparticles.index
    df_nparticles.insert(0,'Z',[toZA[p][0] for p in parts])
    df_nparticles = df_nparticles.transpose()
    
    df_nparticles.to_csv("../figures/number_sim_particles.csv",index=True)

def  print_icrp_stats(df_icrp):

  # get list of thicknesses
  list_thick = df_icrp["thick"].unique().tolist()
  print (list_thick)


  print ("\n*********************************************\n")
  print ("\t stats icrp145")
  print ("\n*********************************************\n")
  print (df_icrp)
  pd.options.display.float_format = '{:.2f}'.format
  for thick in list_thick:
    print ("\n*********************************************\n")
    df_thick = df_icrp[df_icrp["thick"]==thick][["thick","group","wT","ipart","iN","DE","DE_low_std","DE_up_std"]]
    for stat in stat_info:
      df_thick["DE"+stat] = df_thick["DE"+stat]*df_thick["wT"]

    df_thick = df_thick.groupby(by=["ipart"]).agg({"DE":'sum',"DE_low_std":'sum',"DE_up_std":'sum','iN':['sum','count']}).reset_index()

    new_columns = []
    for cols in df_thick.columns:
        if cols[1]!='count': new_columns.append(cols[0])
        else: new_columns.append(cols[1])
    df_thick.columns = new_columns

    df_thick["iN"] = df_thick["iN"]/df_thick["count"]
    df_thick.iN = df_thick.iN.astype(int)
    df_thick.drop("count",axis=1,inplace=True)
    total = float(df_thick["DE"].sum())
    std_up_total = float(df_thick["DE_up_std"].sum())
    std_low_total = float(df_thick["DE_low_std"].sum())
    print ("----------------------")
    print ("thickness = " ,thick,"cm")
    print ("----------------------")
    print (" -> nsim = " ,df_thick["iN"].sum())
    print (" -> DE   = ",'{:.2f}'.format(total),"-",str('{:.2f}'.format(std_low_total/total*100))+"%","+",str('{:.2f}'.format(std_up_total/total*100))+"%")
    print ("main std :")
    df_thick["dev(%)"] = (abs(df_thick["DE_low_std"])+abs(df_thick["DE_up_std"]))/total*100
    df_thick.sort_values(by=["dev(%)"],ascending=False,inplace=True)
    df_thick = df_thick[df_thick["dev(%)"]>0.5][["ipart","iN","DE","dev(%)"]]
    df_thick.set_index("ipart",inplace=True)
    df_thick.index.name = None
    #df_thick = df_thick.transpose()
    print (df_thick)
  print ("\n\n")


def plot_DE_Q_Particles(df_ICRP):


    df_weights = pd.read_csv("weightParticles.csv")
    df_weights = df_weights.sort_values(by="Z")
    sumweights = df_weights["wP"].sum()
    df_weights["percentwP"] = df_weights["wP"]/sumweights*100

    Zlist = df_weights["Z"].tolist()
    wP = df_weights["percentwP"].tolist()
   

    list_thickness = sorted(df_ICRP["thick"].unique())
    fig,ax = plt.subplots(1,2,figsize=(15,7),sharex=True)

    ax[0].plot(Zlist,wP,color="k",marker='s',label="relative gcr abundance",markersize=3)

    list_thickness = [0,30,200]

    for ith,thick in enumerate(list_thickness):
    
        ## Group by particles, loose detail of organ contributions
        df_particles = df_ICRP[df_ICRP["thick"]==thick].copy()

        #df_Q = df_particles.groupby(by=["ipart"]).sum()
        df_Q = df_particles.copy()
        for stat in stat_info:
           df_Q["DE"+stat] = df_Q["DE"+stat]*df_Q["mass[g]"]
           df_Q["Dose"+stat] = df_Q["Dose"+stat]*df_Q["mass[g]"]

        df_Q = df_Q.groupby(by=["ipart","Z"]).sum().reset_index()
        for stat in stat_info:
           df_Q["DE"+stat] = df_Q["DE"+stat]/df_Q["mass[g]"]
           df_Q["Dose"+stat] = df_Q["Dose"+stat]/df_Q["mass[g]"]

        df_Q["Q"] = df_Q["DE"]/df_Q["Dose"] 
        df_Q["Q_low_std"] = df_Q["DE"]/df_Q["Dose"]*np.sqrt((df_Q["DE_low_std"]/df_Q["DE"])**2+(df_Q["Dose_low_std"]/df_Q["Dose"])**2)
        df_Q["Q_up_std"] = df_Q["DE"]/df_Q["Dose"]*np.sqrt((df_Q["DE_up_std"]/df_Q["DE"])**2+(df_Q["Dose_up_std"]/df_Q["Dose"])**2)

        df_Q = df_Q.sort_values(by=["Z"],ascending=True)
        #print (df_Q)
        #continue

        #df_Q["Q"] = 

        for stat in stat_info:
            df_particles["DE_p"+stat] = df_particles["DE"+stat]/df_particles["DE"].sum() * 100
            df_particles["Dose_p"+stat] = df_particles["Dose"+stat]/df_particles["Dose"].sum() * 100
    
        df_particles.drop(["wT","group"],inplace=True,axis=1)
        d_group_rules = {}
        for stat in stat_info:
          d_group_rules["DE_p"+stat] = 'sum'
          d_group_rules["Dose_p"+stat] = 'sum'
        df_particles = df_particles.groupby(by=["thick","ipart"],as_index=False).agg(d_group_rules)
    
    
        listp = df_particles["ipart"].tolist()
        listZ = [toZA[p][0] for p in listp]
        df_particles["Z"] = listZ
     
        df_particles = df_particles.sort_values(by=["thick","Z"],ascending=True)


    
        ax[0].errorbar(df_particles["Z"],df_particles["DE_p"],yerr=[df_particles["DE_p_low_std"],df_particles["DE_p_up_std"]],label=label_thick(thick),color=get_thick_color(thick),marker="s",markersize=3)
        ax[1].plot(df_Q["Z"],df_Q["Q"],label=label_thick(thick),color=get_thick_color(thick))
        ax[1].fill_between(df_Q["Z"],y1=df_Q["Q"]-df_Q["Q_low_std"],y2=df_Q["Q"]+df_Q["Q_up_std"],color=get_thick_color(thick),alpha=0.2)
    
        for ip,p in enumerate(part_label):
            if df_particles[df_particles["Z"]==toZA[p][0]]["DE_p"].iloc[0] > part_ytxt[ip]:
                part_ytxt[ip] = df_particles[df_particles["Z"]==toZA[p][0]]["DE_p"].iloc[0]
        print ("Particle contributions")
        print (df_particles[["Z","DE_p"]])
        print ("Quality factors")
        print (df_Q[["Z","Q"]])
        #listcols = [c for c in df_particles.columns if "low" not in c and "up" not in c and "Q" in c]
        #print (df_Q[listcols])
        print ("____________________________")
    

    
    for ip, p in enumerate(part_label):
        h  = wP[toZA[p][0]-1]
        if part_ytxt[ip]>h:
            h = part_ytxt[ip]

        #for ith,thick in enumerate(list_thickness):
        #    print (h,part_ytxt[ip])
        #    if h>part_ytxt[ip]: h=part_ytxt[ip]
        #ax[0].annotate(p, xy=(toZA[p][0], part_ytxt[ip]), xytext=(toZA[p][0]-0.12,part_ytxt[ip]*1.25),fontsize=13)
        ax[0].annotate(p, xy=(toZA[p][0], h), xytext=(toZA[p][0]-0.12,h*1.50),fontsize=13)
    
    ax[0].set_ylabel("Contribution to effective dose (%)",fontsize=15)
    ax[0].set_xlabel("Particle charge (Z)",fontsize=15)
    ax[0].set_yscale("log")
    
    #ax[1].plot(dfp2["Z"],dfp2["Q"],label="Al + 45 $g.cm^{-2}$ regolith",color="red")
    #ax[1].fill_between(dfp2["Z"],y1=dfp2["Q"]-dfp2["Q_low_std"]/2,y2=dfp2["Q"]+dfp2["Q_up_std"]/2,color="red",alpha=0.2)
    #ax[1].fill_between(dfp3["Z"],y1=dfp3["Q"]-dfp3["Q_low_std"]/2,y2=dfp3["Q"]+dfp3["Q_up_std"]/2,color="green",alpha=0.2)
    ax[1].set_ylabel("Body-averaged mean quality factor",fontsize=15)
    ax[1].set_xlabel("Particle charge (Z)",fontsize=15)
    ax[1].grid(True)
    
    #ax.set_xticks(xx)
    #ax.set_xticklabels(dfp1["Z"],rotation=0,ha='center')
    #ax[1].legend(fontsize=12,loc='upper left')
    ax[0].legend(fontsize=10)
    ax[0].grid(True)
    
    plt.savefig("../figures/DE-Q-Particles.png",bbox_inches="tight")
    plt.close()

def plot_DE_Organs(df_ICRP):

    ## Group by organs, loose detail of primary contributions
    df_group = df_ICRP.groupby(by=["thick","group","wT","mass[g]"]).sum().reset_index()
    df_group.drop(["ipart","iN"],inplace=True,axis=1)

    
    dff = pd.DataFrame()

    list_thickness = df_group["thick"].unique().tolist()

    print ("Global Absorbed doses and dose equivalent")
    for thickness in list_thickness:
        df_dose = df_group[df_group["thick"]==thickness]
        print (thickness, "Abs. Dose: ",sum(df_dose["Dose"]*df_dose["mass[g]"])/sum(df_dose["mass[g]"]),sum(df_dose["Dose_low_std"]*df_dose["mass[g]"])/sum(df_dose["mass[g]"]),sum(df_dose["Dose_up_std"]*df_dose["mass[g]"])/sum(df_dose["mass[g]"]))
        print ("\tDose Equivalent: ",sum(df_dose["DE"]*df_dose["mass[g]"])/sum(df_dose["mass[g]"]),sum(df_dose["DE_low_std"]*df_dose["mass[g]"])/sum(df_dose["mass[g]"]),sum(df_dose["DE_up_std"]*df_dose["mass[g]"])/sum(df_dose["mass[g]"]))
    print ("_____________________________________________")
    #sys.exit()

    for thickness in list_thickness:
        df_thick = df_group[df_group['thick']==thickness]
        df_thick.set_index("group",inplace=True)
    
        if thickness==0:
            dff.index = df_thick.index
            dff['wT'] = df_thick["wT"]
            dff['mass[g]'] = df_thick["mass[g]"]
            
    
        for val in ['Dose','DE']:
            dff[val+str(thickness)] = df_thick[val]
            dff[val+str(thickness)+"_low_std"] = df_thick[val+"_low_std"]
            dff[val+str(thickness)+"_up_std"] = df_thick[val+"_up_std"]

    #dff = dff[(dff["DE0"]!=0) & (dff["DE30"]!=0) & (dff["DE100"]!=0)]
    dff.sort_values(by=["DE0"],ascending=False,inplace=True)


    #print ("Absorbed Doses")
    #print (dff[(dff["wT"]!=0)]["Dose0"])
    #sys.exit()

    #print ("NON CANCER ORGANS")
    #print (dff[dff["wT"]==0][["DE0","DE0_up_std","DE30","DE30_up_std","DE100","DE100_up_std"]])


    df_cancer_organs = dff[dff["wT"]!=0]

    remainder_organs = ["thymus","oral mucosa","extrathoracic","muscle","lymphatic nodes","spleen","heart","small intestine","kidney","uterus","pancreas","gallbladder","adrenal"]

    df_remainder = df_cancer_organs[df_cancer_organs.index.isin(remainder_organs)]
    df_non_remainder = df_cancer_organs[~df_cancer_organs.index.isin(remainder_organs)]

    # Calculate weighted average for the rows in listorgans
    weighted_avg = {
        "group": ["remainder"],  # New name for the grouped row
        #"value": [(df_remainder["value"] * df_remainder["wT"]).sum() / df_remainder["wT"].sum()],
        "wT": [df_remainder["wT"].sum()],
        "mass[g]": [df_remainder["mass[g]"].sum()]
    }


    for val in ['Dose','DE']:
        for thick in list_thickness:
            for suf in ['','_low_std','_up_std']:
                weighted_avg[val+str(thick)+suf] = [(df_remainder[val+str(thick)+suf] * df_remainder["wT"]).sum() / df_remainder["wT"].sum()]

    grouped_row = pd.DataFrame(weighted_avg)
    grouped_row.set_index("group",inplace=True)


    #print (grouped_row)
    #sys.exit()

    df_cancer_organs = pd.concat([df_non_remainder,grouped_row])



    for thick in list_thickness:
        df_cancer_organs["Q"+str(thick)] = df_cancer_organs["DE"+str(thick)]/df_cancer_organs["Dose"+str(thick)]

        print (thick,"cm DE : ",sum(df_cancer_organs["DE"+str(thick)]*df_cancer_organs["wT"]),"-",sum(df_cancer_organs["DE"+str(thick)+"_low_std"]*df_cancer_organs["wT"]),"+",sum(df_cancer_organs["DE"+str(thick)+"_up_std"]*df_cancer_organs["wT"]))
        print ("\tQ = ",(df_cancer_organs["DE"+str(thick)]/df_cancer_organs["Dose"+str(thick)]).mean())

    listcolQ = [c for c in df_cancer_organs.columns if "Q" in c]
    print ("Organs quality factors")
    print (df_cancer_organs[listcolQ])
    print ("\n________________________________________")


    list_thickness = [0, 30, 100, 150, 200]
    list_thickness = [0, 30, 100, 200]


    fig,ax = plt.subplots(1,figsize=(15,7))
    
    width = 0.18

    x = np.arange(len(df_cancer_organs))
    for ith, thick in enumerate(list_thickness):

        if ith == 0:
          ax.bar(x-width/2.0*len(list_thickness)+ith*width,df_cancer_organs["Dose"+str(thick)],\
    	  width=width,\
    	  yerr=[df_cancer_organs["Dose"+str(thick)+"_low_std"],df_cancer_organs["Dose"+str(thick)+"_up_std"]],\
    	  label="absorbed dose",color="white",edgecolor='black', hatch='//',alpha=1.0)
          lab = label_thick(thick)
        else:
          lab = label_thick(thick)

        ax.bar(x-width/2.0*len(list_thickness)+ith*width,df_cancer_organs["DE"+str(thick)],\
    	width=width,\
    	yerr=[df_cancer_organs["DE"+str(thick)+"_low_std"],df_cancer_organs["DE"+str(thick)+"_up_std"]],\
    	#label="DE (4 mm Al-2219 + "+str(thick)+" cm regolith)",color=get_thick_color(thick))
    	label=lab,color=thick_col[ith])

    for ith, thick in enumerate(list_thickness):

        ax.bar(x-width/2.0*len(list_thickness)+ith*width,df_cancer_organs["Dose"+str(thick)],\
    	width=width,\
    	yerr=[df_cancer_organs["Dose"+str(thick)+"_low_std"],df_cancer_organs["Dose"+str(thick)+"_up_std"]],\
    	#label="dose ("+str(thick)+" $g.cm^{-2}$)",color=thick_col[thick],edgecolor='black', hatch='//',alpha=1.0)
    	color=thick_col[ith],edgecolor='black', hatch='//',alpha=1.0)

    	#label="dose ("+str(thick)+" cm)",color=get_thick_color(thick),edgecolor='black', hatch='//')
    
    ax.set_ylabel("Organ dose equivalent (mSv/y)\n and absorbed dose (mGy/y)",fontsize=15)
    
    
    ax.set_xticks(x)
    ax.set_xticklabels(df_cancer_organs.index,rotation=90,ha='center',fontsize=14)
    ax.legend(ncol=2,fontsize=10)

    list_DE = [c for c in df_cancer_organs.columns if "DE" in c and "low" not in c and "up" not in c]
    print ("--------------------------------------")
    print ("DE in organs")
    print (df_cancer_organs[list_DE])

    list_Dose = [c for c in df_cancer_organs.columns if "Dose" in c and "low" not in c and "up" not in c]
    print ("AD in organs")
    print (df_cancer_organs[list_Dose])
    print ("--------------------------------------")
    
    #ax.set_xticks(df_thick["organId"])
    
    plt.savefig("../figures/DE-Organs.png",bbox_inches="tight")
    plt.close()

    return df_cancer_organs

def model_dose(df_ICRP,df_ICRU):
    
    df_icrug = df_ICRU[["thick","DE","DE_low_std","DE_up_std","Dose","Dose_low_std","Dose_up_std"]].groupby("thick").sum()

    xthick_icru = df_icrug.index.tolist()
    for i in range(len(xthick_icru)): xthick_icru[i]*=1.5
    yde = (df_icrug["DE"]).tolist()
    print (xthick_icru)
    print (yde)

    def func(x,a,b,c,d): 
        return (a*x**2+b*x+yde[0])/(c*x+1) * np.exp(-d*x)

    popt, pcov = curve_fit(func, xthick_icru, yde)

    p0_icru = popt

    print (" --- Model parameters --- ")
    print (popt)
    print ("h0 = ",yde[0])
    print ("q = ",str(popt[0]/yde[0]))
    print ("p = ",str(popt[1]/yde[0]))
    print ("k = ",str(popt[2]))
    print ("d = ",str(popt[3]))

    xxicru = np.linspace(xthick_icru[0],xthick_icru[-1],100)
    yyicru = [func(x,*popt) for x in xxicru]

    fig,ax = plt.subplots(1,figsize=(14,7))
    

    df_group = df_ICRP.groupby(by=["thick","group","wT","mass[g]"]).sum().reset_index()
    df_group.drop(["ipart","iN"],inplace=True,axis=1)
    print (df_group)


    list_thick = df_group["thick"].unique().tolist()
    #for col in df_cancer.columns:
    #    if "DE" not in col: continue
    #    if "_" in col: continue
    #    list_thick.append(int(col.replace("DE","")))


    d_de = {"thick":[],"EDE":[],"EDE_low":[],"EDE_up":[],"DE":[],"DE_low":[],"DE_up":[],"Dose":[],"Dose_low":[],"Dose_up":[],"DE_cancer":[],"DE_cancer_low":[],"DE_cancer_up":[]}
    for thick in list_thick:
      colde = "DE"
      d_de["thick"].append(thick)
      df_c = df_group[df_group["thick"]==thick]
      d_de["EDE"].append(sum(df_c[colde]*df_c["wT"]))
      d_de["EDE_low"].append(sum(df_c[colde+"_low_std"]*df_c["wT"]))
      d_de["EDE_up"].append(sum(df_c[colde+"_up_std"]*df_c["wT"]))
      #print (col,sum(df_cancer[colde]*df_cancer["wT"]))
      d_de["DE"].append(sum(df_c[colde]*df_c["mass[g]"])/sum(df_c["mass[g]"]))
      d_de["DE_low"].append(sum(df_c[colde+"_low_std"]*df_c["mass[g]"])/sum(df_c["mass[g]"]))
      d_de["DE_up"].append(sum(df_c[colde+"_up_std"]*df_c["mass[g]"])/sum(df_c["mass[g]"]))
      coldose = "Dose"
      d_de["Dose"].append(sum(df_c[coldose]*df_c["mass[g]"])/sum(df_c["mass[g]"]))
      d_de["Dose_low"].append(sum(df_c[coldose+"_low_std"]*df_c["mass[g]"])/sum(df_c["mass[g]"]))
      d_de["Dose_up"].append(sum(df_c[coldose+"_up_std"]*df_c["mass[g]"])/sum(df_c["mass[g]"]))

      df_c =df_c[df_c["wT"]!=0]
      d_de["DE_cancer"].append(sum(df_c[colde]*df_c["mass[g]"])/sum(df_c["mass[g]"]))
      d_de["DE_cancer_low"].append(sum(df_c[colde+"_low_std"]*df_c["mass[g]"])/sum(df_c["mass[g]"]))
      d_de["DE_cancer_up"].append(sum(df_c[colde+"_up_std"]*df_c["mass[g]"])/sum(df_c["mass[g]"]))
    df_tot = pd.DataFrame(d_de)
    df_tot.set_index("thick",inplace=True)
    df_tot.index.name = None

    df_tot["IS_DE"]  = df_icrug["DE"]
    df_tot["IS_Dose"]  = df_icrug["Dose"]
    df_tot["err_ISDE/ICRPDE"] = abs(df_tot["IS_DE"]-df_tot["DE"])/(df_tot["IS_DE"]+df_tot["DE"])*200
    df_tot["err_ISDE/ICRPEDE"] = abs(df_tot["IS_DE"]-df_tot["EDE"])/(df_tot["IS_DE"]+df_tot["EDE"])*200
    df_tot["err_ICRPDE/ICRPEDE"] = abs(df_tot["DE"]-df_tot["EDE"])/(df_tot["DE"]+df_tot["EDE"])*200
    df_tot["err_ICRPDE_cancer/ICRPEDE"] = abs(df_tot["DE_cancer"]-df_tot["EDE"])/(df_tot["DE_cancer"]+df_tot["EDE"])*200
    df_tot["err_ISDose/ICRPDose"] = abs(df_tot["IS_Dose"]-df_tot["Dose"])/(df_tot["IS_Dose"]+df_tot["Dose"])*200



    print ("_____________________________")
    print ("Error between Doses/ human phantoms")
    cols_err = [c for c in df_tot.columns if "err" in c]
    print (df_tot[cols_err])

    print ("_____________________________")

    

    xthick = []
    for i in range(len(list_thick)): xthick.append(list_thick[i]*1.5)

    yde = df_tot["DE"].tolist()

    def func(x,a,b,c,d):
        return (a*x**2+b*x+yde[0])/(c*x+1) * np.exp(-d*x)
    
    popt, pcov = curve_fit(func, xthick, yde,p0=p0_icru,method='dogbox')
    print (" --- Model parameters for DE in IFHP--- ")
    print (popt)
    print ("h0 = ",yde[0])
    print ("q = ",str(popt[0]/yde[0]))
    print ("p = ",str(popt[1]/yde[0]))
    print ("k = ",str(popt[2]))
    print ("d = ",str(popt[3]))

    xx = np.linspace(xthick[0],xthick[-1],451)
    yyde = [func(x,*popt) for x in xx]


    yede = df_tot["EDE"].tolist()

    def func(x,a,b,c,d):
       return (a*x**2+b*x+yede[0])/(c*x+1) * np.exp(-d*x)
    popt, pcov = curve_fit(func, xthick, yede,p0=p0_icru,method='dogbox')
    print (" --- Model parameters for EDE in IFHP--- ")
    print (popt)
    print ("h0 = ",yede[0])
    print ("q = ",str(popt[0]/yede[0]))
    print ("p = ",str(popt[1]/yede[0]))
    print ("k = ",str(popt[2]))
    print ("d = ",str(popt[3]))

    xxx = np.linspace(xthick[0],1500,1501)
    yyyede = [func(x,*popt) for x in xxx]

    print ("EDE attenuations")
    yyyede0=yyyede[0]
    print ("0 cm = ",yyyede0)
    ninegcm2 = True
    sixteengcm2 = True
    fiftypercent = True
    earthlevel = True

    for i in range(len(yyyede)):
        #print ("\t",xxx[i],yyyede[i],yyyede[i]/yyyede0)
        if ninegcm2 and xxx[i]>=10: 
            print ("\t",xxx[i],yyyede[i],yyyede[i]/yyyede0)
            ninegcm2=False
        if sixteengcm2 and xxx[i]>=30: 
            print ("\t",xxx[i],yyyede[i],yyyede[i]/yyyede0)
            sixteengcm2=False
        if fiftypercent and yyyede[i]/yyyede0<=0.5: 
            print ("\t",xxx[i],yyyede[i],yyyede[i]/yyyede0)
            fiftypercent=False
        if earthlevel and yyyede[i]<=2.4: 
            print ("\t",xxx[i],yyyede[i],yyyede[i]/yyyede0)
            earthlevel=False

    df_matthiae = pd.read_csv("EDE_matthiae.csv")
    df_matthiae["EDE"] = df_matthiae["EDE"]*365
    df_matthiae["std"] = df_matthiae["std"]*365

    df_dobynde = pd.read_csv("DE_Dobynde.csv")
    df_dobynde["DE"] = df_dobynde["DE"]*10
    diff_depth = [0]
    depth = df_dobynde["depth"].tolist()
    diff_depth = diff_depth + [ depth[i+1] - depth[i] for i in range(len(depth)-1)]
    density = []
    for d in depth:
        if d<=22: density.append(1.76)
        elif d<=71: density.append(2.11)
        else: density.append(1.78)
    df_dobynde["diff_depth"] = diff_depth
    df_dobynde["density"] = density
    df_dobynde["athick"] = df_dobynde["density"] * df_dobynde["diff_depth"]
    df_dobynde["athick"] = df_dobynde["athick"].cumsum()

    yyede = [func(x,*popt) for x in xx]
    
    ax.errorbar(xthick,yde,yerr=[df_tot["DE_low"],df_tot["DE_up"]],linestyle='',marker='o', markersize=4,color='darkred',label="DE IFHP")
    ax.plot(xx,yyde,linestyle="-",color="r",label='Fit DE IFHP',alpha=0.5)


    ax.errorbar(xthick,df_tot["EDE"],yerr=[df_tot["EDE_low"],df_tot["EDE_up"]],linestyle='',marker='o', markersize=4,color='black',label="EDE IFHP")
    ax.plot(xx,yyede,linestyle="-",color="black",label='Fit EDE IFHP',alpha=0.5)



    ax.errorbar(xthick,df_tot["Dose"],yerr=[df_tot["Dose_low"],df_tot["Dose_up"]],linestyle='',marker='o', markersize=4,color='orange',label="Absorbed dose IFHP")

    ax.errorbar(xthick_icru,df_icrug["DE"],yerr=[df_icrug["DE_low_std"],df_icrug["DE_up_std"]],linestyle='',markersize=3,marker='s',label="DE ICRU sphere",color='darkblue')
    ax.errorbar(xthick_icru,df_icrug["Dose"],yerr=[df_icrug["Dose_low_std"],df_icrug["Dose_up_std"]],linestyle='',markersize=3,marker='s',label="Absorbed Doses ICRU sphere",color='darkslategray')
    ax.plot(xxicru,yyicru,linestyle="-",color="b",label="Fit DE ICRU sphere")

    ax.plot(df_matthiae["thick(g/cm2)"],df_matthiae["EDE"],linestyle='',marker='x', markersize=5,color='magenta',label="DE (Matthiä & Berger 2024)")
    ax.plot(df_dobynde["athick"],df_dobynde["DE"],linestyle='-',marker='', color='cyan',label="DE (Dobynde & Guo 2024)")

    ax.legend(ncol=2,fontsize=13)
    #ax.set_xlabel("1.136 $g/cm^2$ Al + regolith ($g/cm^2$)",fontsize=15)
    ax.set_xlabel("Inner shield + regolith ($g/cm^2$)",fontsize=15)
    ax.set_ylabel("doses ($mSv/y$) or (mGy/y)",fontsize=15)
    plt.xticks(fontsize=13)
    ax.grid(True)
    
    plt.savefig("../figures/Fit_DE.png",bbox_inches="tight")





 
