#define EIGEN3_ENABLED
#ifdef EIGEN3_ENABLED
# include <Eigen/Core>
#endif

#include <map>
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

  Cpg() {
    _init_nn();
    _angles.resize(_nn.get_nb_neurons());
  }

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

  void _init_nn(){
    int k = 0;
    for (size_t i = 0; i < 8; ++i)
    {
      float x = 1; // TODO
      float r = 1; // TODO
      float omega = 1; // TODO

      vertex_desc_t v = _nn.add_neuron(boost::lexical_cast<std::string>(i));
      _nn.get_neuron_by_vertex(v).get_pf().set_omega(omega);
      _nn.get_neuron_by_vertex(v).get_pf().set_x(x);
      _nn.get_neuron_by_vertex(v).get_pf().set_r(r);
      _oscs.push_back(v);
    }


    // couplings
    // _nn.add_connection(_oscs[0], _oscs[1], std::make_pair(0.0, 0.0));

    _nn.init();
  }
};


#ifdef  TEST_CPG
int main() {
  Cpg cpg;
  for (size_t i = 0; i < 100; ++i)
  {
    cpg.step();
    std::cout<<cpg.angles()[0]<<std::endl;
  }
}
#endif
