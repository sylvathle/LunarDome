#include "GCRFlux.hh"


GCRFlux::GCRFlux():ions(getIons())
{
	DefineCommands();

	SetGCRcsv("../scripts/gcr_2020-02-01_2021-02-01.csv");
}


GCRFlux::~GCRFlux() 
{
	delete fMessenger;
}


void GCRFlux::LoadGCRFromCSV()
{
	std::ifstream file(csvFile);
    	std::string line;

    	if (!file.is_open()) {
    	    std::cerr << "EError opening file: " << csvFile << std::endl;
    	    return;
    	}

    	// Read the header line
    	if (!std::getline(file, line)) {
    	    std::cerr << "Error reading header line" << std::endl;
    	    return;
    	}

    	// Tokenize the header line
    	std::istringstream headerStream(line);
    	std::string ionname;
	G4bool isEneCol=true;
	
	Nbins.clear();
	binsEnergy.clear();
	binsFlux.clear();
	ionsName.clear();
	listIntegratedFluxParticleKind.clear();
	listParticleWeight.clear();

    	while (std::getline(headerStream, ionname, ',')) {
		if (isEneCol) {isEneCol=false; continue;}
		ionsName.push_back(G4String(ionname));
	}

	std::vector<G4double> old_values;
	for (int i=0;i<ionsName.size();i++)
	{
		old_values.push_back(0.);
		listIntegratedFluxParticleKind.push_back(0.);
		Nbins[i+1]=0;
	}

	G4double old_ene=0;
	G4bool firstDataLine=true;
	G4double A,Z;

    	while (std::getline(file, line)) {
    	    	std::istringstream lineStream(line);
	
    	    	G4double ene,value;
    	    	G4double columnIndex = -1;
    	    	while (lineStream >> value) {
			if (columnIndex==-1) 
			{old_ene=ene; ene = value;}
			else
			{
				A = (G4double) ions[ionsName[columnIndex]].getA();
				Z = (G4double) ions[ionsName[columnIndex]].getZ();
				value = value/A;
				binsEnergy[Z].push_back(ene*A);
				binsFlux[Z].push_back(value);
				
				Nbins[Z] = Nbins[Z]+1;
				if (firstDataLine) {firstDataLine=false;}
				else {listIntegratedFluxParticleKind[columnIndex]+= (value+old_values[columnIndex])*(ene-old_ene)*A/(2.0) ;}
					
				old_values[columnIndex] = value;
			}
    		        if (lineStream.peek() == ',') lineStream.ignore();
    		        columnIndex++;
    		}
    	}

   	file.close();

	total_particles = 0.0;
	for (int i=0;i<listIntegratedFluxParticleKind.size();i++) 
	{
		total_particles+=listIntegratedFluxParticleKind[i];
	}
	for (int i=0;i<listIntegratedFluxParticleKind.size();i++) 
	{
		listParticleWeight.push_back(listIntegratedFluxParticleKind[i]/total_particles);
	}

	G4cout << "Total flux (#/m^2/sr/s) = " << total_particles << G4endl;

	nIonsKind = ionsName.size();
}


G4double GCRFlux::GetEnergyFlux(G4int Z, G4double E) const
{
	// Search Right index
	G4int id = 0;
	for (G4int i=0;i<binsEnergy.at(Z).size()-1;i++)
	{if ((E>=binsEnergy.at(Z)[i]) && E<=binsEnergy.at(Z)[i+1]) {id = i; break;}}
	G4double a = (binsFlux.at(Z)[id+1]-binsFlux.at(Z)[id])/(binsEnergy.at(Z)[id+1]-binsEnergy.at(Z)[id]);
	G4double b = binsFlux.at(Z)[id+1] - a * binsEnergy.at(Z)[id+1];
	return a*E + b ;
}

void GCRFlux::SetGCRcsv(G4String csv) 
{
	csvFile = csv;
	LoadGCRFromCSV();
}

void GCRFlux::DefineCommands()
{
	fMessenger = new G4GenericMessenger(this, "/SIM/gcr/","GCR csv file");

	auto& gcrcsvCmd = fMessenger->DeclareMethod("gcrFile",&GCRFlux::SetGCRcsv,"Set GCR source, csv format");
	gcrcsvCmd.SetParameterName("gcrFile", true);
	gcrcsvCmd.SetDefaultValue("../scripts/gcr_2020-02-01_2021-02-01.csv");

}







