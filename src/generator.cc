#include "generator.hh"
#include "Randomize.hh"
#include "ions.hh"

#include "G4AnalysisManager.hh"

#include <math.h> 
// Function to pick an index from the vector of ions kind based on probabilities list
int pickIndex(const std::vector<double>& probabilities) {
    // Calculate cumulative probabilities
    std::vector<double> cumulativeProbabilities(probabilities.size());
    double sum = 0.0;
    double factor = 1000000.0;
    for (size_t i = 0; i < probabilities.size(); ++i) {
        sum += probabilities[i]*factor;
        cumulativeProbabilities[i] = sum;
    }

    // Generate a random number between 0 and 1
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, factor);
    double randNum = dis(gen);

    // Find the index corresponding to the segment containing the random number
    for (size_t i = 0; i < cumulativeProbabilities.size(); ++i) {
        if (randNum < cumulativeProbabilities[i]) {
            return i;
        }
    }

    // In case of rounding errors, return the last index
    return probabilities.size() - 1;
}


//Constructor, take gcrFlux class by default
MyPrimaryGenerator::MyPrimaryGenerator():gcrFlux(new GCRFlux())
{
	fParticleGun = new G4ParticleGun(1);

	halfSphere = true;

	nevent = 0;
	nrejected = 0;

	rsource = 2.1*m;
	rsource = 3.5*m;

	SetBeamRadius(rsource);

	
	// Prepare table of ions
	ionTable = G4IonTable::GetIonTable();
	ionTable->CreateAllIon();

	// Preparing random generator
	biasRndm = new G4SPSRandomGenerator();

	// Prepare energy distribution of all ions based on the modified class SBG4EneDistribution
	for (G4int Z=1;Z<=gcrFlux->GetNIons();Z++)
	{
		G4String ionname(gcrFlux->GetIonName(Z-1));
		SBG4SPSEneDistribution *eneGenerator = new SBG4SPSEneDistribution();
		eneGenerator->SetBiasRndm(biasRndm);

		// Load energy distribution of specific ion Z
		eneGenerator->SetEnergyDisType("Arb");
    	    	listEneGenerator[ionname] = eneGenerator;
		for (G4int ie=0;ie<gcrFlux->GetNbins(Z);ie++)
		{
			G4ThreeVector dat(gcrFlux->GetEneVal(Z,ie),gcrFlux->GetFluxVal(Z,ie));
			listEneGenerator[ionname]->ArbEnergyHisto(dat);
		}
		listEneGenerator[ionname]->ArbInterpolate("Lin");
	}
	
	// Number of ions used (usually 28)
	G4int n_ions_kind = gcrFlux->GetNIons();
	
	// Initialize Nsimulated particle for each kind
	for (int i=0;i<n_ions_kind;i++) {Npart.push_back(0);} 
	// Preparing ration of simulated particles, equal for all ions by default
	for (int i=0;i<n_ions_kind;i++) {ratioPart.push_back(G4double(1.0));} 
	// Weigths of simulated particles (normalized ratios)
	for (int i=0;i<n_ions_kind;i++) {weightToUse.push_back(G4double(1.0/n_ions_kind));} 
	
	// Size of sample, useful if many /run/beamOn are done in the same run.
	sampleSize = 1e5;

	// Preparing commands concerning the generator for the macros
	DefineCommands();

	// Generating random seed based on time measured in nanosecond to ensure distinct names 
	//	in case of multiparallel runs
	std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);

	G4Random::setTheEngine(new CLHEP::RanecuEngine());
	G4long seed = nanoseconds.count()%10000000; //1;//time(NULL);
	G4Random::setTheSeed(seed);

}

MyPrimaryGenerator::~MyPrimaryGenerator()
{
	delete fParticleGun;
	delete fMess;
}

// Class used to generate one particle, it select the ion, the position where it originates, its momentum and energy
void MyPrimaryGenerator::GeneratePrimaries(G4Event* anEvent)
{

	// pick an ion in the list based on the probability defined in weightsToUse
	idParticle = pickIndex(weightToUse);
	G4String particleName = gcrFlux->GetIonName(idParticle);
	G4ParticleDefinition* particleDef = ionTable->GetIon(gcrFlux->GetZ(particleName),gcrFlux->GetA(particleName),0*keV);
	
	// Define its energy
	G4double energy = listEneGenerator[particleName]->GenerateOne(particleDef);

	// Generate polar angle between -pi and pi
	G4double thetapos = CLHEP::RandFlat::shoot(-PI,PI);
	
	// Generate random z between ground and sphere of source
	G4double posz0 = CLHEP::RandFlat::shoot(lowPosZ,rsource);

	// Compute the x and y positions from z and theta
	G4double r = sqrt(rsource*rsource-posz0*posz0);
	G4double posx0 = r * cos(thetapos);
	G4double posy0 = r * sin(thetapos);

	// Define position
	G4ThreeVector pos(posx0,posy0,posz0);


	// Define momentum within cosine distribution
	G4ThreeVector mom = GenMomentum(pos);
	++nevent;

	// If momentum is going upward repeat the process of spacial selection of particle parameters
	while (mom[2]>=0 && halfSphere)
	{
		thetapos = CLHEP::RandFlat::shoot(-PI,PI);
		posz0 = CLHEP::RandFlat::shoot(lowPosZ,rsource);
		r = sqrt(rsource*rsource-posz0*posz0);
		posx0 = r * cos(thetapos);
		posy0 = r * sin(thetapos);

		pos = G4ThreeVector(posx0,posy0,posz0);

		mom = GenMomentum(pos);
		++nevent;
		++nrejected;
	}
	/*G4cout << (G4double) (nrejected)/(G4double)(nevent) << G4endl;

	G4double theta = acos(pos[2]/sqrt(pos[0]*pos[0]+pos[1]*pos[1]+pos[2]*pos[2]));
	G4double phi = atan(pos[1]/pos[0]);

	std::ofstream outFile("../genDistrib.txt",std::ios::app);
	outFile << theta << "," << phi << "," << pos[0] << "," << pos[1] << "," << pos[2] << std::endl;
	outFile.close();*/

	

	// By now the particle will be generated and is accounted for simulations
	Npart[G4int(particleDef->GetPDGCharge())-1]++;


	// Assign parameters to the gun
	fParticleGun->SetParticlePosition(pos);
	fParticleGun->SetParticleMomentumDirection(mom);
	fParticleGun->SetParticleEnergy(energy);
	fParticleGun->SetParticleDefinition(particleDef);
	fParticleGun->GeneratePrimaryVertex(anEvent);
}



// Function generating a cosine distribution from the source point
G4ThreeVector MyPrimaryGenerator::GenMomentum(G4ThreeVector pos)
{
	// Normalize pos
	G4double normpos = sqrt(pos[0]*pos[0]+pos[1]*pos[1]+pos[2]*pos[2]);
	G4double normposx = pos[0]/normpos;
	G4double normposy = pos[1]/normpos;
	G4double normposz = pos[2]/normpos;

	//Calculate vparallel
	G4double randpar = CLHEP::RandFlat::shoot(0.0,1.0);
	G4double sizevpar = -sqrt(1-randpar);
	G4double xpar = normposx * sizevpar;
	G4double ypar = normposy * sizevpar;
	G4double zpar = normposz * sizevpar;

	
	// Calculate vperpendicular
	// First we need a base in the perpendicular plane
	// First base
	G4double normb1 = sqrt(zpar*zpar+xpar*xpar);
	G4double b1x = zpar/normb1;
	G4double b1y = 0.0;
	G4double b1z = -xpar/normb1;

	//Second base
	G4double b2x = normposy*b1z - normposz*b1y;
	G4double b2y = normposz*b1x - normposx*b1z;
	G4double b2z = normposx*b1y - normposy*b1x;

	
	G4double sizevperp = sqrt(1.0 - sizevpar*sizevpar);	

	G4double theta = CLHEP::RandFlat::shoot(-PI,PI);
	G4double mx0rot = sizevperp * (b1x * cos(theta) + b2x * sin(theta)) + xpar;
	G4double my0rot = sizevperp * (b1y * cos(theta) + b2y * sin(theta)) + ypar;
	G4double mz0rot = sizevperp * (b1z * cos(theta) + b2z * sin(theta)) + zpar;

	return G4ThreeVector(mx0rot,my0rot,mz0rot);

}

void MyPrimaryGenerator::SetParticleRatio(G4double Z, G4double rat)
{
	ratioPart[G4int(Z)-1] = rat;
	
	// Normalize ratio part to get weightToUse
	G4double tot = 0.;
	for (G4int i=0;i<weightToUse.size();i++){tot+=ratioPart[i];}
	if (tot==0) return;
	for (G4int i=0;i<weightToUse.size();i++){weightToUse[i]=ratioPart[i]/tot;}
}

// Prepare everything that depends on the radius of the beam source
void MyPrimaryGenerator::SetBeamRadius(G4double r)
{
	rsource = r;
	
	//G4cout << "SetBeamRadius  " << rsource << G4endl;
	beamSurface = 4.0 * PI * rsource*rsource/m/m;
	lowPosZ = -rsource;
	factorSphere = 1.0; //Case GCR is taken from all directions
	secondToYear = 365.0 * 24.0 * 3600.0;

	// If scene is of the Lunar surface (it is), the starting surface is half sphere, the mimal z position is 0, 
	//   and we are only considering half of the possible origins (the other being blocked by the Moon).
	if (halfSphere) 
	{
		beamSurface/=2.0; //If we use half sphere, the surface should be halfen from the full surface
		lowPosZ=0.0; // The minimum position of the surface source is 0.0
		// The factor 0.5 stands for the fact that no particle can be generated from the half-sphere below the surface
		// The factor 3/4 eliminates te 25% of particles that are generated from above the surface with upward momentum (positive z momentum), this factor was calculated analitically
		factorSphere = 0.5*3.0/4.0;
	}
	missionFactor = factorSphere*beamSurface*secondToYear*2.0*PI;
}

// Macros commands:
// 	-- sampleSize (= how many particles are generated before values are averaged and recorded)
//	-- radbeam: define the radius of the beam
//	-- ratio of particle generated
void MyPrimaryGenerator::DefineCommands()
{
	fMess = new G4GenericMessenger(this, "/SIM/scoring/","Set sample size");
	auto& samplesizeCmd = fMess->DeclareMethod("sampleSize",&MyPrimaryGenerator::SetSampleSize,"Set size of sample");
	samplesizeCmd.SetParameterName("sampleSize", true);
	samplesizeCmd.SetDefaultValue("1e5");
	
	fMess = new G4GenericMessenger(this, "/SIM/scoring/","Set radius beam");
	auto& radiusbeamCmd = fMess->DeclareMethodWithUnit("radbeam","mm",&MyPrimaryGenerator::SetBeamRadius,"Set radius beam");
	radiusbeamCmd.SetParameterName("radbeam", true);
	radiusbeamCmd.SetDefaultValue("3004*mm");

	fMess = new G4GenericMessenger(this, "/SIM/generate/","Set particle ratio");
	auto& partRatioCmd = fMess->DeclareMethod("rat",&MyPrimaryGenerator::SetParticleRatio,"Set particle ratio");
	partRatioCmd.SetParameterName("rat", true);
	partRatioCmd.SetDefaultValue("1 1");
}









