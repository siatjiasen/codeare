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

#ifndef __DIRECT_METHOD_HPP__
#define __DIRECT_METHOD_HPP__

#include "ReconStrategy.hpp"

using namespace RRServer;


/**
 * @brief Reconstruction startegies
 */
namespace RRStrategy {

	/**
	 * @brief Empty recon for test purposes
	 */
	class DirectMethod : public ReconStrategy {
		
		
	public:
		

		/**
		 * @brief Default constructor
		 */
		DirectMethod  () {};
		

		/**
		 * @brief Default destructor
		 */
		virtual 
		~DirectMethod () {};
		
		
		/**
		 * @brief Do nothing 
		 */
		virtual RRSModule::error_code
		Process ();
		

		/**
		 * @brief Do nothing 
		 */
		virtual RRSModule::error_code
		Init ();
		
		/**
		 * @brief Do nothing 
		 */
		virtual RRSModule::error_code
		Finalise () {

			return OK;

		}
		

	private: 
		
		double           m_dt;       /*!< @brief Simulation time steps                                        */
		bool             m_verbose;  /*!< @brief Verbose (Store magnetisation for every dt. HANDLE WITH CARE) */ 
		int              m_np;       /*!< @brief Number of OMP threads */

	};

}
#endif /* __DIRECT_METHOD_H__ */

