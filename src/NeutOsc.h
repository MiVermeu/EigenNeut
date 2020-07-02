#ifndef NEUTOSC_H__
#define NEUTOSC_H__

#include <iostream>
#include <Eigen/Dense>
#include <complex>

namespace neutosc {

std::complex<double> If(0,1);

// A struct to hold neutrino oscillation parameters.
struct OscPars {
  double th12 = 0.5843;
  double th23 = 0.738;
  double th13 = 0.148;
  double Dm21sq = 7.5e-5; // In eV^2
  double Dm31sq = 2.457e-3; // In eV^2
  double Dm32sq = Dm31sq-Dm21sq; // In eV^2
  double dCP = 1.38 * 3.14159265;

  bool operator==(OscPars& other) const {
    return th12 == other.th12 && th23 == other.th23 && th13 == other.th13 &&
           Dm21sq == other.Dm21sq && Dm31sq == other.Dm31sq &&
           Dm32sq == other.Dm32sq && dCP == other.dCP;
  }
  bool operator!=(OscPars& other) const { return !operator==(other); }
};

class Oscillator {
  private:
	// Neutrino oscillation parameter struct.
  OscPars op;

  // Oscillation matrix.
  Eigen::Matrix3cd U;
  // Mass difference matrix.
  Eigen::Matrix3d Dmsq;

  public:
  Oscillator() {
    update();
  } // Oscillator::Oscillator

  void update() {
    const double s12 = sin(op.th12);
    const double s23 = sin(op.th23);
    const double s13 = sin(op.th13);
    const double c12 = cos(op.th12);
    const double c23 = cos(op.th23);
    const double c13 = cos(op.th13);

    Eigen::Matrix3cd U1;
    U1 << 1, 0, 0,
          0, c23, s23,
          0, -s23, c23;

    Eigen::Matrix3cd U2;
    U2 << c13, 0, s13*(cos(op.dCP)-If*sin(op.dCP)),
          0, 1, 0,
          -s13*(cos(op.dCP)+If*sin(op.dCP)), 0, c13;

    Eigen::Matrix3cd U3;
    U3 << c12, s12, 0,
          -s12, c12, 0,
          0, 0, 1;

    U = U1*U2*U3;

    Dmsq << 0, -op.Dm21sq, -op.Dm31sq,
         op.Dm21sq, 0, -op.Dm32sq,
         op.Dm31sq, op.Dm32sq, 0;

  } // Oscillator::update()

  // Expose the neutrino oscillation parameter set to mess with it.
  OscPars& pars() { return op; }

  Eigen::Vector3d trans(int nu1, double E, double L) {
    Eigen::Vector3d P(0,0,0);
    // P(nu1) += 1; // delta_nu1nu2

    for(int nu2 = 0; nu2 < 3; ++nu2) {
      P(nu2) += pow(abs(conj(U(nu1,0))*U(nu2,0)),2) + pow(abs(conj(U(nu1,1))*U(nu2,1)),2) + pow(abs(conj(U(nu1,2))*U(nu2,2)),2) +
                2*(conj(U(nu1,0))*U(nu2,0)*U(nu1,1)*conj(U(nu2,1)) * exp(-If*2.54*Dmsq(0,1)*L/E)).real() +
                2*(conj(U(nu1,0))*U(nu2,0)*U(nu1,2)*conj(U(nu2,2)) * exp(-If*2.54*Dmsq(0,2)*L/E)).real() +
                2*(conj(U(nu1,1))*U(nu2,1)*U(nu1,2)*conj(U(nu2,2)) * exp(-If*2.54*Dmsq(1,2)*L/E)).real();
    }
    
    // // "Simplified" version that does not work right now.
    // for(int j = 0; j < 2; ++j) {
    //   for(int i = j+1; i < 3; ++i) {
    //     for(int nu = 0; nu < 3; ++nu) {
    //       P(nu) -= 4*(conj(U(nu1,i))*U(nu,i)*U(nu1,j)*conj(U(nu,j))).real() * pow(sin(1.27*Dmsq(i,j)*L/E), 2);
    //       P(nu) += 2*(conj(U(nu1,i))*U(nu,i)*U(nu1,j)*conj(U(nu,j))).imag() * sin(1.27*Dmsq(i,j)*L/E);
    //     }
    //   }
    // }
    return P;
  } // Oscillator::trans()
}; // class Oscillator

} // namespace neutosc

#endif