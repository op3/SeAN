#ifndef ABSORPTION_H
#define ABSORPTION_H 1

#include <string>
#include <vector>

#include "Math/Interpolator.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TGraph.h"
#include "TAxis.h"

#include "Config.h"
#include "Settings.h"

using std::string;
using std::vector;

class Absorption{
private:
	Settings settings;

	vector< vector<double> > matt;

public:
	Absorption(Settings &s){ settings = s; };
	~Absorption(){
		delete &matt;
	};

	//void read_massattenuation_NIST(vector<double> &energy_bins, vector<double> &massattenuation_bins, string massAttenuation_ID, double mass);

	void const_beam(const vector<double> &energy_bins, vector<double> &incident_beam_histogram);

	void gauss_beam(const vector<double> &energy_bins, vector<double> &incident_beam_histogram);

	void arbitrary_beam(const vector<double> &energy_bins, vector<double> &incident_beam_histogram, const vector< vector<double> > &incident_beam_file);

	void photon_flux_density(vector<double> &dopplercs_bins, vector<double> &massattenuation_bins, vector<double> &z_bins, vector<double> &incident_beam_bins, vector<vector<double> > &photon_flux_density_bins);

	void resonance_absorption_density(vector<double> &dopplercs_bins, vector<vector<double> > &photon_flux_density_bins, vector<vector<double> > &resonance_absorption_density_bins);

	void plot_massattenuation(vector<double> &energy_bins, vector<double> &massattenuation_bins, string title, TCanvas *canvas, TLegend* legend, string legend_entry);

	void plot_total_massattenuation(string title, TCanvas *canvas, TLegend* legend, string legend_entry);

	void plot_photon_flux_density(vector<double> &energy_bins, vector<double> &z_bins, vector<vector<double> > &photon_flux_density_bins, string title, TCanvas *canvas, TLegend* legend, string legend_entry);

	void plot_resonance_absorption_density(vector<double> &energy_bins, vector<double> &z_bins, vector<vector<double> > &resonance_absorption_density_bins, string title, TCanvas *canvas, TLegend* legend, string legend_entry);

};

#endif
