#ifndef PHONONDENSITY_H
#define PHONONDENSITY_H 1

#include <vector>

#include "Config.h"
#include "Settings.h"
#include "Integrator.h"
#include "Writer.h"

using std::vector;

class PhononDensity{

private:
	vector<double> mu_bins;
	vector<double> mu_hist;
	vector<double> alpha_s_mean;
	vector<double> sine_sum;
	vector<double> cosine_sum;
	vector<double> q_s_squared_over_E_squared;

	Settings settings;
	Integrator *integrator;
	Writer *writer;

public:
	PhononDensity(Settings &s){ 
		settings = s; 
		integrator = new Integrator();
		writer = new Writer();
	};

	~PhononDensity(){}

	void calculate_mu_bins(const unsigned int target_number);
	void calculate_alpha_s_mean(const vector<double> &omega_s_file, const unsigned int target_number, const unsigned int n_modes);
	void calculate_q_s_squared_over_E_squared(const vector<double> &omega_s_file, const unsigned int target_number, const unsigned int n_modes);

	void calculate_sine_sum(const vector<double> &omega_s_file, const unsigned int n_modes);
	void calculate_cosine_sum(const vector<double> &omega_s_file, const unsigned int n_modes);

	void calculateCrossSection(const vector<double> &energy_bins, const vector<double> &energy_boosted, vector<double> &crosssection_histogram, const vector<double> &omega_s_file, const unsigned int target_number);

// **********************************************************************
// Idealistic implementation, if ALL phonons were known
// **********************************************************************
//
//	void calculate_mu_bins(const unsigned int target_number);
//	void calculate_alpha_s_mean(const vector<double> &omega_s_file, const unsigned int target_number, const unsigned int n_modes);
//	void calculate_q_s_squared_over_E_squared(const vector<double> &omega_s_file, const vector< vector<double> > &e_s_file, const double p[3], const unsigned int target_number, const unsigned int n_modes);
//
//	void calculate_sine_sum(const vector<double> &omega_s_file, const unsigned int n_modes);
//	void calculate_cosine_sum(const vector<double> &omega_s_file, const unsigned int n_modes);
//
//	void calculateCrossSection(const vector<double> &energy_bins, const vector<double> &energy_boosted, vector<double> &crosssection_histogram, const vector<double> &omega_s_file, const vector< vector<double> > (&e_s_file), const vector< vector<double> > (&p_file), const unsigned int target_number);
};

#endif