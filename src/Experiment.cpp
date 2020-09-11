/*    
    This file is part of SeAN.

    SeAN is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SeAN is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SeAN.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <sstream>

#include "Experiment.h"
#include "Config.h"
#include "Status.h"

using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::string;
using std::regex;
using std::regex_replace;
using std::stringstream;

Experiment::Experiment(Settings &s):writer(s){
	settings = s;
}

void Experiment::initialize(){
	createEnergyBins(settings.emin, settings.emax);
	createTargets();
}

void Experiment::createEnergyBins(double emin, double emax){

	energy_bins.reserve(settings.nbins_e);

	double delta_e = (double) (emax - emin)/(settings.nbins_e - 1);

	for(unsigned int i = 0; i < settings.nbins_e; ++i)
		energy_bins.push_back(emin + i*delta_e);
}

void Experiment::createTargets(){

	unsigned int ntargets = (unsigned int) settings.targetNames.size();	

	for(unsigned int i = 0; i < ntargets; ++i){
		targets.push_back(Target(i, settings));
		targets[i].initialize(energy_bins);
	}
}

void Experiment::crossSections(){

	unsigned int ntargets = (unsigned int) settings.targetNames.size();	
	bool skip_calculation = false;
	stringstream sta_str;

	for(unsigned int i = 0; i < ntargets; ++i){
		
		// Before calculating the cross section, check whether a target with the same resonances
		// already exists. If yes, the cross section can simply be copied.
		if(i > 0){
			for(unsigned int j = 0; j < i; ++j){
				if(settings.equal_resonances(i, j)){
					targets[i].copyCrossSection(targets[j]);
					skip_calculation = true;
					targets[i].copied_from = (int) j;
					sta_str << "Copied cross section for target #" << i+1 << " from target #" << j+1 << ". ";
					Status::print(sta_str.str(), true);
					break;
				}
			}
		}

		if(!skip_calculation){
			if(settings.direct){
				targets[i].calculateCrossSectionDirectly(energy_bins);
			} else{
				targets[i].calculateCrossSection(energy_bins);
			}
		}

	}
}

void Experiment::transmission(){

	unsigned int ntargets = (unsigned int) settings.targetNames.size();	

	for(unsigned int i = 0; i < ntargets; ++i){
		// The incident beam on the first target is user-defined, the rest is determined by the transmission of the previous target
		if(i == 0){
			targets[0].calculateIncidentBeam(energy_bins);
			targets[0].calculateTransmission();
		} else{
			targets[i].calculateIncidentBeam(targets[i-1].getPhotonFluxDensity());
			targets[i].calculateTransmission();
		}

		if(settings.status){
			stringstream sta_str;
			sta_str << "Calculation of transmission through target #" << i+1 << " finished.";
			Status::print(sta_str.str(), true);
		}
	}
}

void Experiment::transmission_thin_target(){

	unsigned int ntargets = (unsigned int) settings.targetNames.size();	

	for(unsigned int i = 0; i < ntargets; ++i){
		// In the thin-target approximation, it is assumed that the impact of every single target
		// on the photon intensity distribution of the beam is negligible.
		// Consequently, each target uses the original incident beam.
		// Furthermore, it is assumed that the attenuation of the beam within every single target
		// is negligible, so that the number of reactions is simply given by the product of the 
		// cross section and the area density of the target.
		if(i == 0){
			targets[0].calculateIncidentBeam(energy_bins);
			targets[0].calculateTransmissionThinTarget();
		} else{
			targets[i].calculateIncidentBeam(energy_bins);
			targets[i].calculateTransmissionThinTarget();
		}

		if(settings.status){
			stringstream sta_str;
			sta_str << "Calculation of transmission through target #" << i+1 << " finished.";
			Status::print(sta_str.str(), true);
		}
	}
}

void Experiment::resonant_scattering(){

	unsigned int ntargets = (unsigned int) settings.targetNames.size();	

	for(unsigned int i = 0; i < ntargets; ++i){
		targets[i].calculateResonantScattering(energy_bins, false);
		if(settings.uncertainty){
			// For the uncertainty estimate, recalculate the transmission with a cross section
			// scaled by the relative deviation of the integral of the calculated cross section
			// from the analytically integrated cross section.
			targets[i].calculateTransmission(targets[i].get_ana_num_ratio());
			targets[i].calculateResonantScattering(energy_bins, true);
		}
		if(settings.status){
			stringstream sta_str;
			sta_str << "Calculation of resonant scattering on target #" << i+1 << " finished.";
			Status::print(sta_str.str(), true);
		}
	}
}

void Experiment::resonant_scattering_thin_target(){

	unsigned int ntargets = (unsigned int) settings.targetNames.size();	

	for(unsigned int i = 0; i < ntargets; ++i){
		targets[i].calculateResonantScatteringThinTarget(energy_bins);

		if(settings.status){
			stringstream sta_str;
			sta_str << "Calculation of resonant scattering in thin-target approximation on target #" << i+1 << " finished.";
			Status::print(sta_str.str(), true);
		}
	}
}

string Experiment::result_string(unsigned int n_setting) const {

	stringstream resss;
	resss<< HORIZONTAL_LINE << "\n";
	resss << ">>> SeAN RESULTS #" << n_setting << "\n";
	resss << "TARGET NAME\tRESONANT SCATTERING";
	if(settings.uncertainty)
		resss << " +- CS_UNCERTAINTY +- SCATTERING_UNCERTAINTY";
	if(settings.thin_target)
		resss << "\t[THIN TARGET (THIN / THICK)]";
	resss << "\n";

	return resss.str();
}

string Experiment::uncertainty_string() const {

	unsigned int ntargets = (unsigned int) settings.targetNames.size();	

	stringstream unss;
	
	for(unsigned int i = 0; i < ntargets; ++i){
		unss << HORIZONTAL_LINE << "\n";
		unss << ">>> TARGET #" << i+1 << "\t: NUMERICAL UNCERTAINTY ESTIMATES\n";
		unss << targets[i].uncertainty_string();
	}

	return unss.str();
}

void Experiment::print_results(unsigned int n_setting){

	unsigned int ntargets = (unsigned int) settings.targetNames.size();	

	if(settings.uncertainty){
		cout << uncertainty_string();
	}
	cout << result_string(n_setting);

	for(unsigned int i = 0; i < ntargets; ++i){
		targets[i].print_results();
	}
}

void Experiment::plot(unsigned int n_setting) {

	unsigned int ntargets = (unsigned int) settings.targetNames.size();	

	for(unsigned int i = 0; i < ntargets; ++i){
		targets[i].plot(energy_bins, n_setting);
	}
}

void Experiment::write(unsigned int n_setting) {

	stringstream filename;
	stringstream sta_str;

	// Write energy bins
	filename << "energy_bins";
	writer.write1DHistogram(energy_bins, filename.str(), "Energy / eV");
	if(settings.status){
		sta_str << "Wrote energy bins to " << settings.outputfile << "_" << filename.str() << TXT_SUFFIX;
		Status::print(sta_str.str(), true);
	}
	writer.write1DCalibration(energy_bins, CAL_FILE_NAME, filename.str());

	unsigned int ntargets = (unsigned int) settings.targetNames.size();

	for(unsigned int i = 0; i < ntargets; ++i){
		targets[i].write(energy_bins, n_setting);
	}
	if(settings.status){
		sta_str.str("");
		sta_str << "Wrote energy calibration to " << settings.outputfile << "_" << CAL_FILE_NAME << CAL_SUFFIX;
		Status::print(sta_str.str(), true);
	}
}

void Experiment::write_results(string outputfile, unsigned int n_setting) const{

	stringstream filename;
	filename << outputfile;

	ofstream ofile;
	ofile.open(filename.str(), std::ios_base::out | std::ios_base::app);

        if(!ofile.is_open()){
                cout << "Error: " << __FILE__ << ":" << __LINE__ << ": "; 
		cout << " write_results(): File '" << outputfile << "' could not be opened." << endl;
		abort();
	}
	
	unsigned int ntargets = (unsigned int) settings.targetNames.size();	

	if(settings.uncertainty)
		ofile << uncertainty_string();
	ofile << result_string(n_setting);
	ofile.close();

	for(unsigned int i = 0; i < ntargets; ++i){
		targets[i].write_results(outputfile);
	}
}
