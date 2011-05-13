/*
 *  jrrs Copyright (C) 2007-2010 Kaveh Vahedipour
 *                               Forschungszentrum Jülich, Germany
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but 
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 
 *  02110-1301  USA
 */

#ifndef __CGSENSE_HPP__
#define __CGSENSE_HPP__

#include "ReconStrategy.hpp"

#include "nfft3util.h"
#include "nfft3.h"

typedef std::complex<float> raw;

using namespace RRServer;


namespace RRStrategy {
	
	/**
	 * @brief Non uniform FFT
	 */
	class CGSENSE : public ReconStrategy {
		
		
	public:
		
		/**
		 * @brief Default constructor
		 */
		CGSENSE  ();
		
		/**
		 * @brief Default destructor
		 */
		virtual 
		~CGSENSE () {};
		
		/**
		 * @brief Process conjugate gradient SENSE
		 */
		virtual RRSModule::error_code
		Process ();

		/**
		 * @brief Initialise NuFFT plans
		 */
		virtual RRSModule::error_code
		Init ();
		
		
		
	private:
		
		int                 m_iter;            /**< Maximum number of NuFFT solver iterations */
		int                 m_verbose;         /**< Verbose should give back the reconstruction series? */
		
		Matrix < raw >      m_sens;            /**< Sensitivity maps                */
		Matrix < raw >      m_measured;        /**< Measured data                   */
		Matrix < double >   m_weights;         /**< K-space weights                 */
		
		nfft_plan           m_fplan;           /**< NuFFT plan                      */
		solver_plan_complex m_iplan;           /**< iNuFFT plan                     */
		
		double              m_epsilon;         /**< NuFFT convergence criterium     */
		double              m_maxit;           /**< Maximum number of NuFFT solver iterations */
		double              m_cgeps;           /**< CG SENSE convergence criterium  */
		double              m_cgmaxit;         /**< Maximum number of CG iterations */
		
		int*                m_N;
		int*                m_n;
		int                 m_M;
		int                 m_dim;

		double*             m_ftw;
		double*             m_ftk;
		
		double*             kmax;
		
	};
	
}
#endif /* __CGSENSE_H__ */