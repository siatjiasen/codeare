/*
 *  jrrs Copyright (C) 2007-2010 Kaveh Vahedipour
 *                               Forschungszentrum Juelich, Germany
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

#ifndef __GRAPPA_HPP__
#define __GRAPPA_HPP__

#include "ReconStrategy.hpp"


/**
 * @brief Reconstruction startegies
 */
namespace RRStrategy {

	/**
	 * @brief GRAPPA PPI reconstruction
	 */
	class GRAPPA : public ReconStrategy {
		
		
	public:
		
		/**
		 * @brief Default constructor
		 */
		GRAPPA  () :
			m_nc(0),
			m_testcase(0),
			m_verbose(0),
			m_noise(0),
			m_acsinc(0),
			m_nt(0),
			m_nr(0),
			m_ns(0)
		{};
		
		/**
		 * @brief Default destructor
		 */
		virtual 
		~GRAPPA () {};
		
		
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
		Prepare ();
		
		/**
		 * @brief Do nothing 
		 */
		virtual RRSModule::error_code
		Finalise ();


	private: 
		
		
		int           m_dim;      /**< Reco dimension {2,3}                   */
		int           m_verbose;  /**< Verbose output                         */
		int           m_nc;       /**< # of coils                             */
		int           m_testcase; /**< Test case                              */
		int           m_noise;    /**< Add noise to signal (for testing)      */
		int           m_acsinc;   /**< ACS scans included                     */
		int           m_ns;       /**< # of source points                     */
		int           m_nt;       /**< # of target points                     */
		int           m_nr;       /**< # of kernel reps in ACS                */
		int           m_data_dim[3]; /**< Kernel dimensions                   */
		int           m_acs_dim[3];  /**< Kernel dimensions                   */
		int           m_kern_dim[3]; /**< Kernel dimensions                   */
		int           m_d[3];        /**< Step size                           */

		float         m_R[3];        /**< Acceleration factor in each dim     */

		Matrix<cxfl>  m_weights;  /**< Weights                                */
	  
	};



	void ComputeWeights (const int& nc, const int* acs_dim, const int* kern_dim, const int* d, const float* R, const Matrix<cxfl>& acs, Matrix<cxfl>& weights) {
		
		ticks       tic     = getticks();
		printf ("  (Re-)Computing weights \n");
		
		// # Kernel repititios in ACS
		int nr = (acs_dim[1] - (kern_dim[1]-1) * R[1]) * (acs_dim[0] - (kern_dim[0] - 1));
		int ns = nc * kern_dim[0] * kern_dim[1]; // # Source points
		int nt = nc * (R[1]-1);      // # Target points
		
		Matrix<cxfl> s (ns, nr);          // Source 
		Matrix<cxfl> t (nt, nr);          // Target
		
		int c = 0;
		

		//Grappa pattern for standard kernel 5x4
		Matrix<double> p = Matrix<double>::Zeros ((kern_dim[1]-1)*R[1]+1, kern_dim[0], nc);
		printf ("  patch size in ACS: %s\n", p.DimsToCString());
		
		// Source points
		for (int lin = 0; lin < kern_dim[1]; lin++)
			for (int col = 0; col < kern_dim[0]; col++)
				p (col,lin*R[1],0) = 1.0;

		// Target points
		size_t lins = 1 + (ceil(kern_dim[1]/2)-1) * R[1];
		for (int lin = lins; lin < lins + R[1] -1; lin++)
			p(ceil(p.Dim(0)/2), lin) = -1.0;
			

		printf ("  source matrix size %s\n", s.DimsToCString());

		int ni  = acs_dim[0] - d[0];
		int nj  = acs_dim[1] - (kern_dim[1]-1) * R[1];
 
		for (int i = d[0]; i < ni; i++) 
			for (int j = 0, pos = 0; j < nj; j++, c++) 
				for (int col = 0; col < kern_dim[0]; col++)
					for (int lin = 0; lin < kern_dim[1]; lin++)
						for (int ch = 0; ch < nc; ch++, pos++)
							s.At(pos + c*s.Dim(0)) = acs(i+col,j+lin,ch);
		
		//s.MXDump ("s.mat", "s", "");
		//p.MXDump ("p.mat", "p", "");
		//acs->MXDump ("acs.mat", "acs", "");

		
		//s = s.Pinv();
		  
		/*
		  weights = t->*s;
		*/
		printf ("done. (%.4f s)\n", elapsed(getticks(), tic) / Toolbox::Instance()->ClockRate());
			
		}
		

	
}
#endif /* __GRAPPA_H__ */



		// Grappa pattern for standard kernel 5x4
		/*Matrix<double> p = Matrix<double>::Zeros ((kern_dim[1]-1)*R[1]+1, kern_dim[0], nc);
		printf ("  patch size in ACS: %s", p.DimsToCString());
		
		// Source points
		for (int lin = 0; lin < kern_dim[1]; lin++)
			for (int col = 0; col < kern_dim[0]; col++)
				p (col,lin*R[1],0) = 1.0;

		// Target points
		size_t lins = 1 + (ceil(kern_dim[1]/2)-1) * R[1];
		for (int lin = lins; lin < lins + R[1] -1; lin++)
			p(ceil(p.Dim(0)/2), lin) = -1.0;
			

		for (int i = 1; i < nc; i++) 
			memcpy (&p[i*p.Width()*p.Height()], &p[0], p.Width()*p.Height()*sizeof(p[0]));
		*/			
		// Construct source point matrix for inversion ----
		// for (points along column)
		// dor (column)

