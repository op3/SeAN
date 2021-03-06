#include "Math/PdfFuncMathCore.h"
#include "Math/ProbFuncMathCore.h"

#include "Constants.h"
#include "Grid.h"
#include "Nucleus.h"
#include "VoigtProfile.h"

Nucleus::Nucleus():excited_states(0){}
Nucleus::Nucleus(vector<ExcitedState> exc_sta, const double m, const unsigned int tJ):
    excited_states(exc_sta),
    mass(m), two_J(tJ)
{}

double Nucleus::doppler_width( const size_t i, const double temperature ) const {
    return excited_states[i].get_excitation_energy()
        *sqrt(2.*Constants::kB*temperature/(mass*Constants::u));
}

vector<double> Nucleus::doppler_width( const double temperature ) const {
    vector<double> widths(excited_states.size());

    for(size_t i = 0; i < excited_states.size(); ++i){
        widths[i] = doppler_width(i, temperature);
    }

    return widths;
}

double Nucleus::energy_integrated_cs( const size_t i ) const {
    return Constants::pi_squared*Constants::hbarc_squared
        /(excited_states[i].get_excitation_energy()*excited_states[i].get_excitation_energy())
        *(excited_states[i].get_two_J()+1)/(two_J+1)*excited_states[i].get_ground_state_width();
}

vector<double> Nucleus::energy_integrated_cs() const {
    vector<double> ene_int_cs(excited_states.size());

    for(size_t i = 0; i < excited_states.size(); ++i){
        ene_int_cs[i] = energy_integrated_cs(i);
    }

    return ene_int_cs;
}

vector<double> Nucleus::energies(const double e_min, const double e_max,
    const size_t n_energies_per_state) const {
    size_t n_energies = n_energies_per_state*excited_states.size();
    vector<double> ene(n_energies, 0.);

    Grid grid;

    vector<double> ene_state(n_energies_per_state);
    for(size_t i = 0; i < excited_states.size(); ++i){
        ene_state = grid.breit_wigner(excited_states[i].get_excitation_energy(),
            excited_states[i].get_total_width(), e_min, e_max, n_energies_per_state);
        for(size_t j = 0; j < n_energies_per_state; ++j){
            ene[j+i*n_energies_per_state] = ene_state[j];
        }
    }

    grid.strictly_increasing(ene);

    return ene;
}

vector<double> Nucleus::cross_section_coverage(const double e_min, const double e_max) const {
    vector<double> cs_cov(excited_states.size());

    for(size_t i = 0; i < excited_states.size(); ++i){
        cs_cov[i] = ROOT::Math::breitwigner_cdf(e_max,
            excited_states[i].get_total_width(), excited_states[i].get_excitation_energy())
            -ROOT::Math::breitwigner_cdf(e_min,
            excited_states[i].get_total_width(), excited_states[i].get_excitation_energy());
    }

    return cs_cov;
}

vector<double> Nucleus::cross_section(const vector<double> &energies) const {
    vector<double> cs(energies.size(), 0.);
    vector<double> ene_int_cs = energy_integrated_cs();

    for(size_t i = 0; i < excited_states.size(); ++i){
        for(size_t j = 0; j < energies.size(); ++j){
            cs[j] += ene_int_cs[i]*ROOT::Math::breitwigner_pdf(energies[j],
                excited_states[i].get_total_width(), excited_states[i].get_excitation_energy());
        }
    }

    return cs;
}

vector<double> Nucleus::cs_doppler_broadened(const vector<double> &energies, const double temperature) const {
    vector<double> cs(energies.size(), 0.);
    vector<double> ene_int_cs = energy_integrated_cs();
    vector<double> dop_wid = doppler_width(temperature);

    VoigtProfile voigt;
    double sigma{0.}, gamma{0.}, excitation_energy{0.};

    for(size_t i = 0; i < excited_states.size(); ++i){
        sigma = dop_wid[i]*Constants::inverse_sqrt_two; // This is the actual standard deviation of the normal-distributed
                                                        // absorption cross section in the limit 
                                                        // Doppler width >> level width
        gamma = excited_states[i].get_total_width();
        excitation_energy = excited_states[i].get_excitation_energy();
        for(size_t j = 0; j < energies.size(); ++j){
            cs[j] += ene_int_cs[i]*voigt.pdf(energies[j]-excitation_energy, sigma, gamma);
        }
    }

    return cs;
}