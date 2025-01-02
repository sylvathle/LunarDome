#include "ReadCSV.hh"

std::map<G4int, G4double> GetwT(const G4String& filename) {
    std::map<int, double> dataMap;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return dataMap;
    }

    std::string line;
    getline(file, line); // Read and discard header line

    while (getline(file, line)) {
        std::istringstream ss(line);
        std::string token;

        // Tokenize the line based on comma delimiter
        std::vector<std::string> tokens;
        while (getline(ss, token, ',')) {
            tokens.push_back(token);
        }

        // Assuming the format is consistent, index 0 is Organ_ID and index 6 is wT
        if (tokens.size() >= 7) {
            try {
                int organID = std::stoi(tokens[0]);
                double wT = std::stod(tokens[8]);
                dataMap[organID] = wT;
            } catch (const std::exception& e) {
                std::cerr << "Error parsing line: " << line << std::endl;
            }
        }
    }

    file.close();
    return dataMap;
}

std::map<G4int, G4String> GetOrgansGroup(const G4String& filename) {
    std::map<int, G4String> dataMap;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return dataMap;
    }

    std::string line;
    getline(file, line); // Read and discard header line

    while (getline(file, line)) {
        std::istringstream ss(line);
        std::string token;

        // Tokenize the line based on comma delimiter
        std::vector<std::string> tokens;
        while (getline(ss, token, ',')) {
            tokens.push_back(token);
        }

        // Assuming the format is consistent, index 0 is Organ_ID and index 6 is wT
        if (tokens.size() >= 7) {
            try {
                int organID = std::stoi(tokens[0]);
                G4String organ(tokens[5]);
                dataMap[organID] = organ;
            } catch (const std::exception& e) {
                std::cerr << "Error parsing line: " << line << std::endl;
            }
        }
    }

    file.close();
    return dataMap;
}

std::map<G4int, G4int> generateNumberedMap(const std::map<G4int, G4String>& organs) {
    std::map<std::string, G4int> organCount;
    std::map<G4int, G4int> numberedOrgans;

    int count = 0;
    for (const auto& organ : organs) {
        const G4String& organName = organ.second;
        if (organCount.find(organName) == organCount.end()) {
            organCount[organName] = count;
            count++;
        }
        numberedOrgans[organ.first] = organCount[organName];
    }

    return numberedOrgans;
}

G4int GetNGroups(std::map<G4int, G4int> groups)
{
	G4int maxval = -1;
	for (const auto& g: groups)
	{
		if (g.second>maxval) {maxval=g.second;}
	}
	return maxval;
}



