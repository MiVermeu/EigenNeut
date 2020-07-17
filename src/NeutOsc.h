#ifndef NEUTOSC_H__
#define NEUTOSC_H__

#include <iostream>
#include <Eigen/Dense>
#include <unsupported/Eigen/MatrixFunctions>
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
  double dCP = 1.38 * 3.14159265;
  double rho = 0; // In kg/m^3

  bool operator==(OscPars& other) const {
    return th12 == other.th12 && th23 == other.th23 && th13 == other.th13 &&
           Dm21sq == other.Dm21sq && Dm31sq == other.Dm31sq && dCP == other.dCP;
  }
  bool operator!=(OscPars& other) const { return !operator==(other); }
};

class Oscillator {
  private:
	// Neutrino oscillation parameter struct.
  OscPars op;

  // Oscillation matrix.
  Eigen::Matrix3cd U;
  Eigen::Matrix3cd Ud;
  // Hamiltonian and matter potential.
  Eigen::Matrix3cd H;
  Eigen::Matrix3cd V;
  Eigen::Matrix3cd Hexp;
  Eigen::Matrix3cd Vexp;
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

    // Construct mixing matrix.
    Eigen::Matrix3cd U1;
    U1 << 1, 0, 0,
          0, c23, s23,
          0, -s23, c23;
    Eigen::Matrix3cd U2;
    U2 << c13, 0, s13*exp(-If*op.dCP),
          0, 1, 0,
          -s13*exp(If*op.dCP), 0, c13;
    Eigen::Matrix3cd U3;
    U3 << c12, s12, 0,
          -s12, c12, 0,
          0, 0, 1;
    U = U1*U2*U3;
    Ud = U.adjoint();

    // Hamiltonian and matter potential.
    H << 0, 0, 0,
         0, op.Dm21sq, 0,
         0, 0, op.Dm31sq;
    
    const double Gf = 4.54164e-37; // Reduced Fermi constant * (c*hbar)^2 in m^2.
    const double Ne = op.rho/(1.672e-27)/2; // Electron number density in m^-3.
    V(0,0) = sqrt(2)*Gf*Ne * 1e3; // Multiply and convert to km^-1.

  } // Oscillator::update()

  // Expose the neutrino oscillation parameter set to mess with it.
  OscPars& pars() { return op; }

  // // Analytical determination of neutrino oscillation at fixed distance L (deprecated).
  // Eigen::Vector3d trans(const int nu1, const double E, const double L) const {
  //   Eigen::Vector3d P(0,0,0);
  //   P(nu1) += 1; // delta_nu1nu2
    
  //   // Perform oscillation for each outcome nu2.
  //   for(int j = 0; j < 2; ++j) {
  //     for(int i = j+1; i < 3; ++i) {
  //       for(int nu2 = 0; nu2 < 3; ++nu2) {
  //         P(nu2) -= 4*(conj(U(nu1,i))*U(nu2,i)*U(nu1,j)*conj(U(nu2,j))).real() * pow(sin(1.27*Dmsq(i,j)*L/E), 2)
  //                   - 2*(conj(U(nu1,i))*U(nu2,i)*U(nu1,j)*conj(U(nu2,j))).imag() * sin(2*1.27*Dmsq(i,j)*L/E);
  //       }
  //     }
  //   }
  //   return P;
  // } // Oscillator::trans()

  // General transformation function that decides between vacuum and matter oscillation.
  std::vector<Eigen::Vector3d> trans(const int nu1, const double E, const double L, const double step = 10) const {
    return op.rho==0? transvac(nu1, E, L, step): transmat(nu1, E, L, step);
  } // Oscillator::trans()

  // Analytical determination of neutrino oscillation at distance L using Hamiltonian.
  std::vector<Eigen::Vector3d> transvac(const int nu1, const double E, const double L, const double step = 10) const {
    Eigen::Vector3cd nu(0,0,0);
    nu(nu1) = 1;
    std::vector<Eigen::Vector3d> result(L/step);
    const double conv = 2.534; // Conversion factor from natural to useful units.
    for(int x=0; x<L; x+=step) {
      Eigen::Matrix3cd Hexp = -If*H/E*conv*x; // Temporary Hamiltonian to component-wise exponentiate.
      for(int i=0; i<3; ++i) Hexp(i,i) = exp(Hexp(i,i));
      result[x/step] = (U*Hexp*Ud*nu).cwiseAbs2();
    }
    return result;
  } // Oscillator::transvac()

  // Analytical neutrino oscillation in matter using Hamiltonian.
  std::vector<Eigen::Vector3d> transmat(const int nu1, const double E, const double L, const double step = 10) const {
    Eigen::Vector3cd nu(0,0,0);
    nu(nu1) = 1;
    std::vector<Eigen::Vector3d> result(L/step);
    // Propagate.
    const double conv = 2.534; // Conversion factor from natural to useful units.
    const int N = 128; // Large enough N for Lie product formula.
    for(int x=0; x<L; x+=step) {
      Eigen::Matrix3cd Hexp = -If*H/E*conv*x/N; // Temporary Hamiltonian to component-wise exponentiate.
      for(int i=0; i<3; ++i) Hexp(i,i) = exp(Hexp(i,i));
      Eigen::Matrix3cd Vexp = -If*V*x/N; // Temporary matter potential to component-wise exponentiate.
      for(int i=0; i<3; ++i) Vexp(i,i) = exp(Vexp(i,i));
      // Slow matrix power. Better than exponent...
      Eigen::MatrixPower<Eigen::Matrix3cd> Apow(Hexp*Ud*Vexp*U);
      result[x/step] = (U*Apow(N) *Ud*nu).cwiseAbs2();
    }
    return result;
  } // Oscillator::transmat()

  // Numeric expression for a range of neutrino oscillation probabilities.
  // This function works great and has the exciting potential of making matter
  // effects trivial to implement. However, the required step size is so small
  // that any reasonable L makes it impossible to animate in a fun way...
  std::vector<Eigen::Vector3d> numtrans(const int nu1, const double E,
    const double L, const double step = 0.1) const {
    std::vector<Eigen::Vector3d> result;

    // Set local Hamiltonian with entered energy.
    const double conv = 2.534; // Conversion factor from natural to useful units.
  
    // Transform initial neutrino to mass basis.
    Eigen::Vector3cd nu(0,0,0);
    nu(nu1) = 1;
    nu = Ud*nu;

    // Propagate and push flavour basis neutrino vector to result.
    result.push_back((U*nu).cwiseAbs2());
    for(double l = 0; l < L; l+=step) {
      nu += -If*H*conv*step/E*nu;
      result.push_back((U*nu).cwiseAbs2());
    }

    return result;
  } // Oscillator::numtrans()
}; // class Oscillator

} // namespace neutosc

#endif