/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "PorousFlowDiffusionCoeffConst.h"

template<>
InputParameters validParams<PorousFlowDiffusionCoeffConst>()
{
  InputParameters params = validParams<PorousFlowMaterialVectorBase>();
  params.addRequiredParam<std::vector<Real> >("tortuosity", "List of tortuosities. Order is i) phase 0; ii) phase 1; etc");
  params.addRequiredParam<std::vector<Real> >("diffusion_coeff", "List of diffusion coefficients.  Order is i) component 0 in phase 0; ii) component 1 in phase 0 ...; component 0 in phase 1; ... component k in phase n (m^2/s");
  params.addClassDescription("This Material provides constant tortuosity and diffusion coefficients");
  return params;
}

PorousFlowDiffusionCoeffConst::PorousFlowDiffusionCoeffConst(const InputParameters & parameters) :
    PorousFlowMaterialVectorBase(parameters),

    _tortuosity(declareProperty<std::vector<Real> >("PorousFlow_tortuosity")),
    _dtortuosity_dvar(declareProperty<std::vector<std::vector<Real> > >("dPorousFlow_tortuosity_dvar")),
    _diffusion_coeff(declareProperty<std::vector<std::vector<Real> > >("PorousFlow_diffusion_coeff")),
    _ddiffusion_coeff_dvar(declareProperty<std::vector<std::vector<std::vector<Real> > > >("dPorousFlow_diffusion_coeff_dvar")),
    _input_tortuosity(getParam<std::vector<Real> >("tortuosity")),
    _input_diffusion_coeff(getParam<std::vector<Real> >("diffusion_coeff"))
{
  /// Check that the number of tortuosities entered is equal to the number of phases
  if (_input_tortuosity.size() != _num_phases)
    mooseError("The number of tortuosity values entered is not equal to the number of phases specified in the Dictator");

  /// Also, the number of diffusion coefficients must be equal to the num_phases * num_components
  if (_input_diffusion_coeff.size() != _num_phases * _num_components)
    mooseError("The number of diffusion coefficients entered is not equal to the number of phases multiplied by the number of fluid components");
}

void
PorousFlowDiffusionCoeffConst::computeQpProperties()
{
  _tortuosity[_qp] = _input_tortuosity;

  _diffusion_coeff[_qp].resize(_num_phases);
  _ddiffusion_coeff_dvar[_qp].resize(_num_phases);
  _dtortuosity_dvar[_qp].resize(_num_phases);

  for (unsigned int ph = 0; ph < _num_phases; ++ph)
  {
    _diffusion_coeff[_qp][ph].resize(_num_components);
    _ddiffusion_coeff_dvar[_qp][ph].resize(_num_components);
    _dtortuosity_dvar[_qp][ph].assign(_num_var, 0.0);

    for (unsigned int comp = 0; comp < _num_components; ++comp)
    {
      _diffusion_coeff[_qp][ph][comp] = _input_diffusion_coeff[ph + comp];
      _ddiffusion_coeff_dvar[_qp][ph][comp].assign(_num_var, 0.0);
    }
  }
}
