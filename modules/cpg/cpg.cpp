#include "cpg.h"

void Cpg::configure(const std::vector<float>& omega,
		    const std::vector<float>& x,
		    const std::vector<float>& r,
		    const std::vector<std::tuple<int, int, float, float> >& couplings)
{
	assert(omega.size() == x.size());
	assert(omega.size() == r.size());

	for (size_t i = 0; i < omega.size(); ++i) {
		vertex_desc_t v = _nn.add_neuron(boost::lexical_cast<std::string>(i));
		_nn.get_neuron_by_vertex(v).get_pf().set_omega(omega[i]);
		_nn.get_neuron_by_vertex(v).get_pf().set_x(x[i]);
		_nn.get_neuron_by_vertex(v).get_pf().set_r(r[i]);
		_oscs.push_back(v);
	}

	// couplings
	for (size_t i = 0; i < couplings.size(); ++i) {
		const std::tuple<int, int, float, float>& t = couplings[i];
		_nn.add_connection(_oscs[std::get < 0 > (t)],
				   _oscs[std::get < 1 > (t)],
				   std::make_pair(std::get<2>(t), std::get<3>(t)));
	}
	_nn.init();
	_angles.resize(_nn.get_nb_neurons());
}
