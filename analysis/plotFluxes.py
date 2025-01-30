import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
import sys,os
import numpy as np

def integrate_trapezoidal(group,minE,maxE):
  # Ensure the group is sorted by 'okE'
  group = group.sort_values(by='okE_min')
  group = group[(group["okE_min"]>minE) & (group["okE_max"]<maxE)]
  # Use numpy's trapz function to perform the trapezoidal integration
  integral = (group['count']*(group["okE_min"]+group["okE_max"])/2).sum()
  #integral = np.trapz(y=group['count'], x=group['okE'])
  return integral

plt.rcParams['xtick.labelsize'] = 14
plt.rcParams['ytick.labelsize'] = 14

pd.options.mode.copy_on_write = True

toZA= {"H":[1,1], "He":[2,4], "Li":[3,7], "Be":[4,9], "B":[5,11], "C":[6,12], "N":[7,14], "O":[8,16], "F":[9,19], "Ne":[10,21], "Na":[11,23], "Mg":[12,24], "Al":[13,27], "Si":[14,28], "P":[15,30], "S":[16,32], "Cl":[17,35], "Ar":[18,40], "K":[19,39], "Ca":[20,40], "Sc":[21,45], "Ti":[22,48], "V":[23,51], "Cr":[24,52], "Mn":[25,55], "Fe":[26,56], "Co":[27,59], "Ni":[28,59], "proton":[1,1]}

Ioncolors = {"proton":"darkred","alpha":"grey","O16":"cyan","C12":"lawngreen","neutron":"darkgreen","e+":"mediumpurple","gamma":"gold","Fe56":"red","Mg24":"slateblue","Si28":"sandybrown","N14":"mistyrose","pi+":"lightcoral","pi-":"rosybrown","deuteron":"olivedrab","He3":"lime","e-":"blue","kaon+":"silver","kaon0L":"gainsboro","triton":"indigo","kaon0S":"fuchsia","lambda":"salmon","B11":"darkviolet","Ne21":"darkorchid","kaon-":"seagreen","O15":"cadetblue","sigma+":"peru","Li6":"forestgreen","mu+":"teal","mu-":"lightblue","anti_proton":"tomato","sigma-":"saddlebrown","anti_neutron":"royalblue","xi0":"olivedrab","C11":"teal","N13":"moccasin","N15":"khaki","O14":"cornflowerblue","Mn53":"rosybrown","C13":"crimson","He6":"palevioletred","Li7":"plum","Al27":"thistle"}

def extendIonName(ion):
  return ion+str(toZA[ion][1])

def getZ(ion):
  for II in toZA.keys():
    if ion==extendIonName(II):
      return toZA[II][0]
  if ion=="proton": return 1
  if ion=="alpha": return 2
  return -1

# Convert Flux to total energy of the flux
# Take as argument a dataframe that must have at least columns "okE" for the energy bining and "count" for the number of particles withing the corresponding bin.
def FluxToE(df):
  okE = df["okE"].tolist()
  N = df["count"].tolist()
  E = 0
  for i,e in enumerate(okE):
    E = E+e*N[i]
  return E

#res_dir = sys.argv[1]
res_dir = "../results/IcruSphere/"
res_dir = "../results/ICRP145/"
res_dir = "./"


flux_csv = "data/flux.csv"

df = pd.read_csv(res_dir+"/"+flux_csv)

#ions = df["Iparticle"].tolist()
#Z = []
#for ion in ions: Z.append(getZ(ion))

#df["Z"] = Z

Nbins = 200
logEmin = -6 # 1 KeV
logEmax = 6.8 # ~5600 GeV
#logEmax = 4.698970004336020 #50 GeV


#df.rename({"okE_extend":"okE"},axis=1,inplace=True)
#df["okE"] = 10 ** (df["okE"] * (logEmax-logEmin)/Nbins + logEmin)
df["okE_min"] = 10 ** (df["okE"] * (logEmax-logEmin)/Nbins + logEmin)
df["okE_max"] = 10 **  ((df["okE"]+1) * (logEmax-logEmin)/Nbins + logEmin)
df["okE_c"] = 10 **  ((df["okE"]+0.5) * (logEmax-logEmin)/Nbins + logEmin)

#Surface of the source 200 cm for the inner radius + thickness of rego + 20.6 cm to start a bit further, converted in meters
df["surfSource"] = (df["thick"]+220.6)/100 
df["surfSource"] = 2.5
#print (df[df["thick"]==0][["thick","surfSource"]])
df["surfSource"] = 2*np.pi*df["surfSource"]**2
df["count"] = df["count"] / (df["okE_max"]-df["okE_min"]) * df["surfSource"]
df.drop("surfSource",axis=1,inplace=True)

#sys.exit()
#df = df.sort_values(by=["okE_min"],ascending=False)
#print (df.head())
#print (min(df["okE_min"]),max(df["okE_max"]))
#sys.exit()

#print (min(df["okE_min"]), max(df["okE_max"]))
#sys.exit()
#df["dE"] = 


#print (list_ions)
df.sort_values(by=["thick","Z","Oparticle","okE_min"],inplace=True)

#print (df[df["Oparticle"]=="neutron"])

#if not os.path.exists("../figures/png_fluxes"):
#  os.mkdir("../figures/png_fluxes")

outsiders = ["gamma"]

#opart = "e-"

list_ipart = df["Iparticle"].unique().tolist()
list_opart = df["Oparticle"].unique().tolist()
list_thick = df["thick"].unique().tolist()
#print df()
#print (list_thick)
#sys.exit()
#llist_opart = [["proton","neutron","gamma"],["e+","e-","pi+","pi-"]]
#list_thick = [0,5,30,100,200]

df_opart = df.copy()#[df["Oparticle"]=="proton"]

#sys.exit()

#df_opart.drop(['Iparticle','Z'],inplace=True,axis=1)
# group count for all primaries

elims = [0.1,1,10,100,1000,10000,100000]

ncols = 3
nrows = 2

plt.rcParams['xtick.labelsize'] = 18
plt.rcParams['ytick.labelsize'] = 18

#fig,ax = plt.subplots(nrows,ncols+1,figsize=(24,12),sharey=False,width=)
#fig = plt.figure(figsize=(24, 12))
#gs = gridspec.GridSpec(nrows, ncols+1, width_ratios=[1, 1, 1, 0.3])
#
#
#
#full_plot_particles = []
#full_legend_particles = []
#
#
#for ie in range(len(elims)-1):
#
#  iex = int((ie)/ncols)
#  iey = (ie)%ncols
#  print (ncols,iex,iey)
#  ax = fig.add_subplot(gs[iex, iey])
#
#  if (iey==0): ax.set_ylabel("Flux energy ($MeV.m^{-2}.s^{-1}$)",fontsize=22)
#  if (iex==1 and iey==1): ax.set_xlabel('Regolith thickness (cm)', fontsize=22)
#
#  #print (df_opart)
#  df_e = df_opart[(df_opart["okE_max"]<elims[ie+1]) & (df_opart["okE_min"]>elims[ie])]
#  #print (df_e[df_e["Oparticle"]=="proton"])
#  df_e = df_e.groupby(by=["thick","Oparticle"],as_index=False).apply(integrate_trapezoidal,elims[ie],elims[ie+1])#.reset_index()
#  df_e.set_index("thick",inplace=True)
#  
#  print (df_e)
#  #continue
#  df_e.columns = ["Oparticle","E"]
#
#  df_eav = df_e[df_e["Oparticle"].isin(list_opart)]
#  df_eav = df_eav.pivot(columns='Oparticle',values='E')
#
#  df_eav["Total"] =  df_eav.sum(axis=1)
#
#
#  list_mainparticle = ["gamma","neutron","e-","e+","pi+","alpha","proton"]
#
#  df_eav["Other"] =  df_eav["Total"]
#  print (df_eav.columns)
#  for p in list_mainparticle: 
#    if p not in df_eav.columns: continue
#    ln = ax.plot(df_eav.index,df_eav[p],marker='.',label=p,color=Ioncolors[p])
#    if p not in full_plot_particles:
#      full_plot_particles.append(p)
#      full_legend_particles.append(ln)
#    df_eav["Other"] = df_eav["Other"] - df_eav[p]
#
#  ln = ax.plot(df_eav.index,df_eav["Other"],marker='.',linestyle="--",label="Other",color="k")
#  if ie==len(elims)-2:
#    full_plot_particles.append("Other")
#    full_legend_particles.append(ln)
#  
#  ax.grid(True)
#
#  ax.set_title("$10^{"+str(int(np.log10(elims[ie])))+"}<E<"+"10^{"+str(int(np.log10(elims[ie+1])))+"}$ (MeV)",fontsize=22)
#  ax.set_yscale("log")
# # ax[ie].legend(fontsize=12,ncol=2)
#  #ax[iex,iey].set_ylim(ymax=ax[iex,iey].get_ylim()[1]*1.5)
# # ax.set_zorder(-1)
#
#lns = full_legend_particles[0]
#for ll in full_legend_particles[1:]: lns = lns+ll
#  
#labs = [l.get_label() for l in lns]
##ax[0].set_zorder(1)
#
#
#ax = fig.add_subplot(gs[:, 3])
#ax.axis('off')
#ax.legend(lns, labs, loc='center',fontsize=20,ncol=1,frameon=True)
#
##ax = fig.add_subplot(gs[iex, iey])
#
##ax[0,1].set_ylabel("Flux energy ($MeV.m^{-2}.s^{-1}$)",fontsize=22)
##ax[1,0].set_ylabel("Flux energy ($MeV.m^{-2}.s^{-1}$)",fontsize=22)
##plt.subplots_adjust(wspace=0)
#

df_bon2020 = pd.read_csv("../inputs/gcr_2020-02-01_2021-02-01.csv")
df_bon2020.set_index("Energy (MeV/n)",inplace=True)
xxbon2020 = df_bon2020.index.tolist()
Hbon2020 = df_bon2020["H"].tolist()
Hebon2020 = df_bon2020["He"].tolist()

#plt.close()

plt.rcParams['xtick.labelsize'] = 14
plt.rcParams['ytick.labelsize'] = 14

#df_eav.append(df_eav.sum(numeric_only=True),ignore_index=True)
#sys.exit()

primaryPlotted = sys.argv[1]
#primaryPlotted = "All"
#primaryPlotted = "proton"
if primaryPlotted in list_ipart: df = df[df["Iparticle"]==primaryPlotted]

thick = 0

list_thick = [0,30,100,200]

#listopart = ["proton","alpha","neutron","pi+","e-","e+","gamma","mu-","mu+","deuteron","He3"]
listopart = ["proton","alpha","neutron","pi+","e-","e+","gamma"]

#for thick in list_thick:
#  dft = df[df["thick"]==thick]
#
#  dft.drop(['Z','Iparticle','thick'],inplace=True,axis=1)
#
#  dft = dft.groupby(by=['Oparticle','okE'],as_index=False).sum()
#  dfsum = dft.copy()
#  dfsum['E'] = dfsum['okE']*dfsum['count']
#  dfsum.drop(['okE','count'],axis=1,inplace=True)
#  #dfsum.drop(['okE','count'],axis=1,inplace=True)
#  dfsum = dfsum.groupby(by=['Oparticle'],as_index=False).sum()
#  dfsum.sort_values(by=['E'],inplace=True,ascending=False)
#
#  listOparticles = dfsum['Oparticle']
#  particle_index = {particle: index for index, particle in enumerate(listOparticles)}
#  dft['particle_index'] = dft['Oparticle'].map(particle_index)
#  dft = dft.sort_values(by=['particle_index', 'okE'])
#  dft.drop(columns=['particle_index'], inplace=True)
#
# # listopart = dfsum["Oparticle"].tolist()
#
#  fig, ax  = plt.subplots(1,figsize=(10,7))
#
#  for opart in listopart:
#    #if opart in outsiders: continue
#    dfo = dft[dft["Oparticle"]==opart]
#    #if opart=="alpha": dfo["okE"]=dfo["okE"]/4
#    ax.plot(dfo['okE'],dfo['count'],'.',markersize=6,label=opart,color=Ioncolors[opart])
#
#  ax.set_ylabel("Particle Flux ($m^{-2}.s^{-1}$)",fontsize=18)
#  ax.set_xlabel("Particle Energy (MeV)",fontsize=18)
#  ax.grid(True)
#
#  ax.set_yscale("log")
#  ax.set_xscale("log")
#
#  ax.set_xlim(2e-3,2e5)
#  #ax.set_ylim(1e6,3e8)
#  #ax.set_ylim(2e4,6e6)
#
#  ax.legend()
#
#  plt.savefig("../figures/png_fluxes/"+primaryPlotted+"-primaries-"+str(thick)+"cm.png",bbox_inches="tight")
#  plt.close()

thick_col = ['#ff0000', '#bf0040', '#7f007f', '#4000bf', '#0000ff']


fig, ax = plt.subplots(len(listopart),1,figsize=(14,30),sharex=False,sharey=False)

for i,opart in enumerate(listopart):
  dft = df[df["Oparticle"]==opart]


  dft.drop(['Z','Iparticle',"Oparticle"],inplace=True,axis=1)


  dft = dft.groupby(by=['thick','okE','okE_c','okE_min','okE_max'],as_index=False).sum()

  #dfsum = dft.copy()
  #dfsum['E'] = dfsum['okE_c']*dfsum['count']
  #dfsum.drop(['okE','count'],axis=1,inplace=True)
  #dfsum = dfsum.groupby(by=['thick'],as_index=False).sum()
  #dfsum.sort_values(by=['E'],inplace=True,ascending=False)

  #listOparticles = dfsum['Oparticle']
  #particle_index = {particle: index for index, particle in enumerate(listOparticles)}
  #dft['particle_index'] = dft['Oparticle'].map(particle_index)
  #dft = dft.sort_values(by=['particle_index', 'okE'])


  for ith,thick in enumerate(list_thick):
    dfo = dft[dft["thick"]==thick]
    dfo = dfo.sort_values(by=["okE"])
    #print (dfo)
    if opart!="alpha": ax[i].plot(dfo['okE_c'],dfo['count'],'.',markersize=3,label=str(thick)+"cm",color=thick_col[ith])
    else: ax[i].plot(dfo['okE_c']/4,dfo['count']*4,'.',markersize=3,label=str(thick)+"cm",color=thick_col[ith])


  ax[i].grid(True)
  ax[i].set_title(opart)
  ax[i].set_xscale("log")
  ax[i].set_yscale("log")

ax[0].plot(xxbon2020,Hbon2020,color=Ioncolors["proton"],linestyle="--")
ax[1].plot(xxbon2020,Hebon2020,color=Ioncolors["alpha"],linestyle="--")

ax[0].legend()
plt.subplots_adjust(wspace=0)

plt.savefig("../figures/Fluxes_per_thick.png",bbox_inches="tight")
plt.close()




fig, ax  = plt.subplots(1,len(list_thick),figsize=(20,7),sharey=True,sharex=True)

for it, thick in enumerate(list_thick):
  dft = df[df["thick"]==thick]

  dft.drop(['Z','Iparticle','thick'],inplace=True,axis=1)

  dft = dft.groupby(by=['Oparticle','okE','okE_c','okE_min','okE_max'],as_index=False).sum()
  dfsum = dft.copy()
  dfsum['E'] = dfsum['okE_c']*dfsum['count']
  dfsum.drop(['okE','count'],axis=1,inplace=True)
  dfsum = dfsum.groupby(by=['Oparticle'],as_index=False).sum()
  dfsum.sort_values(by=['E'],inplace=True,ascending=False)

  listOparticles = dfsum['Oparticle']
  particle_index = {particle: index for index, particle in enumerate(listOparticles)}
  dft['particle_index'] = dft['Oparticle'].map(particle_index)
  dft = dft.sort_values(by=['particle_index', 'okE'])
  dft.drop(columns=['particle_index'], inplace=True)


  for opart in listopart:
    #if opart in outsiders: continue
    #if opart!="proton": continue
    #print (thick,opart)
    dfo = dft[dft["Oparticle"]==opart]
    
    #if opart=="gamma": print (thick,"gamma\n",dfo[(dfo["okE_min"]<50) & (dfo["okE_max"]>0.3)][['okE_min','okE_max','count']])
    print (thick,opart)
    #with pd.option_context('display.max_rows', None, 'display.max_columns', None): print (dfo[['okE_min','okE_max','count']])
    max_count_row = dfo.loc[dfo['count'].idxmax()]
    print(max_count_row)
    #if opart=="gamma":
    #  with pd.option_context('display.max_rows', None, 'display.max_columns', None):
    #    print (dfo[["okE","count"]])
    if opart!="alpha": ax[it].plot(dfo['okE_c'],dfo['count'],'.',markersize=4,label=opart,color=Ioncolors[opart])
    else: ax[it].plot(dfo['okE_c']/4,dfo['count']*4,'.',markersize=4,label=opart,color=Ioncolors[opart])
    #print (dfo["count"])
    #widths = (-dfo['okE_min']+dfo['okE_max']).tolist()
    #ax[it].bar(dfo['okE_min'],dfo['count'],width=widths,label=opart,edgecolor=Ioncolors[opart],align='edge',facecolor='none')

  ax[it].plot(xxbon2020,Hbon2020,color=Ioncolors["proton"],linestyle="--")
  ax[it].plot(xxbon2020,Hebon2020,color=Ioncolors["alpha"],linestyle="--")

for it, thick in enumerate(list_thick):
  ax[it].set_yscale("log")
  ax[it].set_xscale("log")
  #ax[it].set_ylim(ymin=1e-5)
  #ax[it].set_xlim(xmin=1e-4)
  if thick==0: ax[it].set_title("1.136 $g.cm^{-2}$ Al-2219",fontsize=14)
  else: ax[it].set_title("Al + "+str(int(thick*1.5))+" $g.cm^{-2}$ regolith",fontsize=14)
  ax[it].grid(True)

ax[0].legend(ncol=1,fontsize=12)
ax[0].set_ylabel("Particle Flux ($m^{-2}.sr^{-1}.s^{-1}.MeV^{-1}$)",fontsize=18)
#ax.grid(True)
#plt.set_ylabel("Particle Flux (N particle per year)")
#plt.set_xlabel("Particle Energy (MeV)")
fig.text(0.5, 0.03, 'Particle Energy (MeV)', ha='center',fontsize=18)
plt.subplots_adjust(wspace=0)
plt.savefig("../figures/"+primaryPlotted+"-primaries.png",bbox_inches="tight")
plt.close()


listopart = ["proton","alpha","C12","O16","Mg24","Si28","Fe56"]
listopart = ["proton","alpha","O16","Fe56"]

convert = {"proton":"H","alpha":"He","C12":"C","O16":"O","Mg24":"Mg","Si28":"Si","Fe56":"Fe"}

fig, ax  = plt.subplots(1,len(list_thick),figsize=(20,7),sharey=True,sharex=True)

for it, thick in enumerate(list_thick):
  dft = df[df["thick"]==thick]

  dft.drop(['Z','Iparticle','thick'],inplace=True,axis=1)

  dft = dft.groupby(by=['Oparticle','okE','okE_c','okE_min','okE_max'],as_index=False).sum()
  dfsum = dft.copy()
  dfsum['E'] = dfsum['okE']*dfsum['count']
  dfsum.drop(['okE','count'],axis=1,inplace=True)
  dfsum = dfsum.groupby(by=['Oparticle'],as_index=False).sum()
  dfsum.sort_values(by=['E'],inplace=True,ascending=False)

  listOparticles = dfsum['Oparticle']
  particle_index = {particle: index for index, particle in enumerate(listOparticles)}
  dft['particle_index'] = dft['Oparticle'].map(particle_index)
  dft = dft.sort_values(by=['particle_index', 'okE'])
  dft.drop(columns=['particle_index'], inplace=True)

  for opart in listopart:
    #if opart in outsiders: continue
    dfo = dft[dft["Oparticle"]==opart]
    #if opart=="pi+":
    #  with pd.option_context('display.max_rows', None, 'display.max_columns', None):
    #    print (dfo[["okE","count"]])
    ax[it].plot(dfo['okE_c']/toZA[convert[opart]][1],dfo['count']*toZA[convert[opart]][1],'.',markersize=4,label=opart,color=Ioncolors[opart])
    ax[it].plot(xxbon2020,df_bon2020[convert[opart]],color=Ioncolors[opart],linestyle="--")


for it, thick in enumerate(list_thick):
  ax[it].set_yscale("log")
  ax[it].set_xscale("log")
  #ax[it].set_ylim(ymin=1e-10)
  #ax[it].set_title("4 mm Al-2219 + \n"+str(thick)+" cm regolith",fontsize=17)
  if thick==0: ax[it].set_title("1.136 $g.cm^{-2}$ Al-2219",fontsize=14)
  else: ax[it].set_title("Al + "+str(int(thick*1.5))+" $g.cm^{-2}$ regolith",fontsize=14)
  ax[it].grid(True)

ax[0].legend(fontsize=13, ncol=2)
ax[0].set_ylabel("Particle Flux ($m^{-2}.sr^{-1}.s^{-1}.MeV^{-1}$)",fontsize=18)
#ax.grid(True)
#plt.set_ylabel("Particle Flux (N particle per year)")
#plt.set_xlabel("Particle Energy (MeV)")
fig.text(0.5, 0.03, 'Particle Energy (MeV)', ha='center',fontsize=18)
plt.subplots_adjust(wspace=0)
plt.savefig("../figures/heavy-primaries.png",bbox_inches="tight")
