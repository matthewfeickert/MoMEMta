/*
 *  MoMEMta: a modular implementation of the Matrix Element Method
 *  Copyright (C) 2016  Universite catholique de Louvain (UCL), Belgium
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <momemta/ParameterSet.h>
#include <momemta/Module.h>

/** \brief Generate points distributed according to a Breit-Wigner
 *
 * Transform a phase-space point given by Cuba, distributed uniformly between 0 and 1,
 * into a quantity distributed according to a relativistic Breit-Wigner distribution
 * of mass \f$m\f$ and width \f$\Gamma\f$.
 *
 * This transformation has the effect of removing a peak associated to a propagator in the integrand.
 * Indeed, the change of variable applied is:
 * \f[
 *      x \to s(x) = m \Gamma \tan (y(x)) + m^2 \text{, with } y(x) = - \atan(m/\Gamma) + \left( \pi/2 + \atan(m/\Gamma) \right) x
 * \f]
 * with Jacobian:
 * \f[
 *      \frac{ds}{dx} = \left( \pi/2 + \atan(m/\Gamma) \right) m \Gamma \cos^{-2}(y(x))
 * \f]
 * If \f$p(s)\f$ is a propagator to be integrated on \f$s = 0 \to \infty\f$, applying this change of variable makes the calculation trivial:
 * \f[
 *      \int_0^{\infty} ds p(s) = \int_0^1 dx \frac{ds}{dx} p(s(x)) = \int_0^1 \text{ (constant) } dx
 * \f]
 *
 * ### Integration dimension
 *
 * This module adds **1** dimension to the integration.
 *
 * ### Parameters
 *
 *   | Name | Type | %Description |
 *   |------|------|--------------|
 *   | `mass` | double | Mass of the propagator to be integrated over (GeV). |
 *   | `width` | double | Width of the propagator to be integrated over (GeV). |
 * 
 * ### Inputs
 *
 *   | Name | Type | %Description |
 *   |------|------|--------------|
 *   | `ps_point` | double | Phase-space point generated by CUBA. |
 *
 * ### Outputs
 *
 *   | Name | Type | %Description |
 *   |------|------|--------------|
 *   | `s` | double | Invariant mass squared of the propagator |
 *   | `jacobian` | double | Jacobian associated to the transformation |
 *
 * \ingroup modules
 */
class BreitWignerGenerator: public Module {
    public:

        BreitWignerGenerator(PoolPtr pool, const ParameterSet& parameters): Module(pool, parameters.getModuleName()),
            mass(parameters.get<double>("mass")),
            width(parameters.get<double>("width")),
            m_ps_point(parameters.get<InputTag>("ps_point")) {
            m_ps_point.resolve(pool);
        };

        virtual void work() override {

            double psPoint = m_ps_point.get<double>();
            const double range = M_PI / 2. + std::atan(mass / width);
            const double y = - std::atan(mass / width) + range * psPoint;

            *s = mass * width * std::tan(y) + (mass * mass);
            *jacobian = range * mass * width / (std::cos(y) * std::cos(y));
        }

        virtual size_t dimensions() const override {
            return 1;
        }

    private:
        const float mass;
        const float width;
        InputTag m_ps_point;

        std::shared_ptr<double> s = produce<double>("s");
        std::shared_ptr<double> jacobian = produce<double>("jacobian");


};
REGISTER_MODULE(BreitWignerGenerator);
