#ifndef CPG_HPP_
#define CPG_HPP_

#define EIGEN3_ENABLED
#ifdef EIGEN3_ENABLED
# include <Eigen/Core>
#endif

#include <map>
#include <tuple>
#include <nn.hpp>


class Cpg {
public:
  typedef std::pair<float, float> weight_t;
  typedef nn::PfIjspeert<nn::params::Vectorf<3> > pf_t;
  typedef nn::AfDirectT<weight_t> af_t;
  typedef nn::Neuron<pf_t, af_t, weight_t> neuron_t;
  typedef nn::Connection<weight_t, weight_t> connection_t;
  typedef nn::NN<neuron_t, connection_t> nn_t;
  typedef nn_t::vertex_desc_t vertex_desc_t;

  Cpg() { }
  void configure(const std::vector<float>& omega,
    const std::vector<float>& x,
    const std::vector<float>& r,
    const std::vector<std::tuple<int, int, float, float> >& couplings);

  void step() {
    _nn.step(_in);
    for (size_t i = 0; i < _nn. get_nb_neurons(); ++i)
      _angles[i] = _nn.get_neuron_by_vertex(_nn.get_neuron(i)).get_pf().get_theta_i();
  }

  const std::vector<float> angles() const {
    return _angles;
  }
protected:
  nn_t _nn;
  std::vector<vertex_desc_t> _oscs;
  std::vector<float> _angles;
  std::vector<weight_t> _in;

  // Main parameters:
  // - phi_i: phase lag
  // - r_i: amplitude
  // - x_i: offset
  // - _omega: frequency
  void _init_nn();
};

#endif
