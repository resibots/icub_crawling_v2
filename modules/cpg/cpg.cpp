#include "cpg.h"

void Cpg::_init_nn()
{
  int k = 0;
  for (size_t i = 0; i < 4; ++i)
  {
    float x = 0; // TODO
    float r = 0.25; // TODO // amplitude
    float omega = 2; // TODO // phase

    vertex_desc_t v = _nn.add_neuron(boost::lexical_cast<std::string>(i));
    _nn.get_neuron_by_vertex(v).get_pf().set_omega(omega);
    _nn.get_neuron_by_vertex(v).get_pf().set_x(x);
    _nn.get_neuron_by_vertex(v).get_pf().set_r(r);
    _oscs.push_back(v);
  }


  // couplings
  _nn.add_connection(_oscs[0], _oscs[1], std::make_pair(20.0, 1.0));
  _nn.add_connection(_oscs[0], _oscs[2], std::make_pair(20.0, 1.0));

  _nn.init();
}
