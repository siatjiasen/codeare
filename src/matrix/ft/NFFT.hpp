/*
 *  codeare Copyright (C) 2007-2010 Kaveh Vahedipour
 *                                  Forschungszentrum Juelich, Germany
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

#ifndef __NFFT_HPP__
#define __NFFT_HPP__

#include "NFFTTraits.hpp"
#include "Algos.hpp"
#include "FT.hpp"
#include "CX.hpp"
#include "Creators.hpp"
/**
 * @brief Matrix templated ND non-equidistand Fourier transform with NFFT 3 (TU Chemnitz)<br/>
 *        Double and single precision
 */
template <class T>
class NFFT : public FT<T> {


	typedef typename NFFTTraits<double>::Plan   Plan;
	typedef typename NFFTTraits<double>::Solver Solver;
    typedef typename std::complex<T> CT;
	
public:

	/**
	 * @brief         Default constructor
	 */
	NFFT() :
		m_initialised (false),
		m_have_pc (false),
		m_imgsz (0),
		m_M (0),
		m_maxit (0),
		m_rank (0),
		m_m(0) {};

	/**
	 * @brief          Construct NFFT plans for forward and backward FT with credentials
	 * 
	 * @param  imsize  Matrix of side length of the image space
	 * @param  nk      # k-space points
	 * @param  m       Spatial cut-off of FT
	 * @param  alpha   Oversampling factor
	 * @param  b0      Off-resonance maps if available
	 * @param  pc      Phase correction applied before forward or after adjoint transforms (default: empty)
	 * @param  eps     Convergence criterium for inverse transform (default: 1.0e-7)
	 * @param  maxit   Maximum # NFFT iterations (default: 3)
	 */
	NFFT        (const Matrix<size_t>& imsize, const size_t& nk, const size_t m = 1, 
				 const T alpha = 1.0, const Matrix<T> b0 = Matrix<T>(1),
				 const Matrix< std::complex<T> > pc = Matrix< std::complex<T> >(1),
				 const T eps = 7.0e-4, const size_t maxit = 2) {
		
		m_M     = nk;
		m_imgsz = 2;
		m_m = m;
		
		m_N = imsize.Container();
		m_n = m_N;//ceil (alpha*m_N);

		m_rank = numel(imsize);
		
		m_imgsz = 2*prod(m_N);

		m_epsilon = eps;
		m_maxit   = maxit;
		
		NFFTTraits<double>::Init (m_N, m_M, m_n, m_m, m_fplan, m_iplan);

		if (pc.Size() > 1)
			m_have_pc = true;
		
		m_pc   = pc;
		m_cpc  = conj(pc);
		
		m_initialised = true;
		
	}
	
	
	NFFT (const Params& p) {

		if (p.exists("nk")) {// Number of kspace samples
			try {
				m_M = unsigned_cast(p["nk"]);
			} catch (const boost::bad_any_cast& e) {
				printf ("**ERROR - NFFT: Numer of ksppace samples need to be specified\n%s\n", e.what());
				assert(false);
			}
		} else {
			printf ("**ERROR - NFFT: Numer of ksppace samples need to be specified\n");
			assert(false);
		}

		if (p.exists("imsz")) {// Image domain size
			try {
				m_N = boost::any_cast<container<size_t> >(p["imsz"]);
			} catch (const boost::bad_any_cast& e) {
				printf ("**ERROR - NFFT: Image domain dimensions need to be specified\n%s\n", e.what());
				assert(false);
			}
		} else {
			printf ("**ERROR - NFFT: Image domain dimensions need to be specified\n");
			assert(false);
		}
		m_n = m_N;

		if (p.exists("m")) {
			try {
				m_m = unsigned_cast (p["m"]);
			} catch (const boost::bad_any_cast&) {
				printf ("  WARNING - NFFT: Could not interpret input for oversampling factor m. Defaulting to 1.");
				m_m = 1;
			}
		} else {
			m_m = 1;
		}

		T alpha = 1.;
		if (p.exists("alpha")) {
			try {
				alpha = fp_cast(p["alpha"]);
			} catch (const boost::bad_any_cast&) {
				printf ("  WARNING - NFFT: Could not interpret input for oversampling factor alpha. Defaulting to 1.0");
			}
		}

		for (size_t i = 0; i < m_N.size(); ++i)
			m_n[i] = ceil(alpha*m_N[i]);

		m_rank = m_N.size();

		m_imgsz = 2*prod(m_N);

		if (p.exists("epsilon")) {
			try {
				m_epsilon = fp_cast (p["epsilon"]);
			} catch (const boost::bad_any_cast&) {
				printf ("  WARNING - NFFT: Could not interpret input for convergence criterium epsilon. Defaulting to 0.0007");
				m_epsilon = 7.e-4f;
			}
		} else {
			m_epsilon = 7.e-4f;
		}

		if (p.exists("maxit")) {
			try {
				m_maxit = unsigned_cast (p["maxit"]);
			} catch (const boost::bad_any_cast&) {
				printf ("  WARNING - NFFT: Could not interpret input for maximum NFFT steps. Defaulting to 3");
				m_maxit = 3;
			}
		} else {
			m_maxit = 3;
		}

		NFFTTraits<double>::Init (m_N, m_M, m_n, m_m, m_fplan, m_iplan);

		if (p.exists("pc")) {
			try {
				m_pc = boost::any_cast<std::complex<T> >(p["pc"]);
			} catch (const boost::bad_any_cast& e) {
				printf ("  WARNING - NFFT: Phase correction matrix corrupt?\n%s\n", e.what());
			}
			m_cpc = conj(m_pc);
			m_have_pc = true;
		}

		m_initialised = true;

	}

	/**
	 * @brief Copy conctructor
	 */
	NFFT (const NFFT<T>& ft) {
		*this = ft;
	}


	/**
	 * @brief        Clean up and destruct
	 */ 
	~NFFT () {
		
		if (m_initialised)
			NFFTTraits<double>::Finalize (m_fplan, m_iplan);

	}


	/**
	 * @brief 	Assignement
	 */
	inline NFFT<T>& operator= (const NFFT<T>& ft) {

		m_initialised = ft.m_initialised;
		m_have_pc     = ft.m_have_pc;
		m_rank        = ft.m_rank;
		m_pc          = ft.m_pc;
		m_cpc         = ft.m_cpc;
		m_N           = ft.m_N;
		m_n           = ft.m_n;
		m_M           = ft.m_M;
		m_maxit       = ft.m_maxit;
		m_epsilon     = ft.m_epsilon;
		m_imgsz       = ft.m_imgsz;
		m_m           = ft.m_m;
		NFFTTraits<double>::Init (m_N, m_M, m_n, m_m, m_fplan, m_iplan);
	    return *this;
		
	}

	/**
	 * @brief      Assign k-space 
	 * 
	 * @param  k   Kspace trajectory
	 */
	inline void 
	KSpace (const Matrix<T>& k) {
		
		size_t cpsz = k.Size();
		assert (cpsz = m_fplan.M_total * m_rank);
		std::copy (k.Begin(), k.End(), m_fplan.x);

	}
	
	
	/**
	 * @brief      Assign k-space weigths (jacobian of trajectory with regards to time) 
	 * 
	 * @param  w   Weights
	 */
	inline void 
	Weights (const Matrix<T>& w) {
		
		size_t cpsz = w.Size();
		assert (cpsz = m_fplan.M_total);
		std::copy (w.Begin(), w.End(), m_iplan.w);

		NFFTTraits<double>::Weights (m_fplan, m_iplan);
		NFFTTraits<double>::Psi     (m_fplan);
		
	}
	
	
	/**
	 * @brief    Forward transform
	 *
	 * @param  m To transform
	 * @return   Transform
	 */
	Matrix< std::complex<T> >
	Trafo       (const Matrix< std::complex<T> >& m) const {

		Matrix< std::complex<T> > out (m_M,1);
		double* tmpd;
		T* tmpt;
		
		tmpd = (double*) m_fplan.f_hat;
		tmpt = (T*) m.Ptr();
		std::copy (tmpt, tmpt+m_imgsz, tmpd);
		
		NFFTTraits<double>::Trafo (m_fplan);
		
		tmpd = (double*) m_fplan.f;
		tmpt = (T*) out.Ptr();
		std::copy (tmpd, tmpd+2*m_M, tmpt);
		
		return squeeze(out);

	}
	
	
	/**
	 * @brief    Backward transform
	 *
	 * @param  m To transform
	 * @return   Transform
	 */
	Matrix< std::complex<T> >
	Adjoint     (const Matrix< std::complex<T> >& m) const {

        Matrix< std::complex<T> > out (m_N);
        double* tmpd;
        T* tmpt;

        tmpd = (double*) m_iplan.y;
        tmpt = (T*) m.Ptr();
        std::copy (tmpt, tmpt+2*m_M, tmpd);

		NFFTTraits<double>::ITrafo ((Plan&) m_fplan, (Solver&) m_iplan, m_maxit, m_epsilon);

		tmpd = (double*) m_iplan.f_hat_iter;
		tmpt = (T*) out.Ptr();
		std::copy (tmpd, tmpd+m_imgsz, tmpt);
		
		return out;
		
	}


	/**
	 * @brief     NFFT plan
	 *
	 * @return    Plan
	 */
	Plan*
	FPlan         () {
		return &m_fplan;
	}
	
	/**
	 * @brief     NFFT plan
	 *
	 * @return    Plan
	 */
	Solver*
	IPlan         () {
		return &m_iplan;
	}
	
private:
	
	bool       m_initialised;   /**< @brief Memory allocated / Plans, well, planned! :)*/
	bool       m_have_pc;

	size_t     m_rank;

	Matrix<CT> m_pc;            /**< @brief Phase correction (applied after inverse trafo)*/
	Matrix<CT> m_cpc;           /**< @brief Phase correction (applied before forward trafo)*/
	
	container<size_t> m_N;      /**< @brief Image matrix side length (incl. k_{\\omega})*/
	container<size_t> m_n;      /**< @brief Oversampling */

	size_t     m_M;             /**< @brief Number of k-space knots */
	size_t     m_maxit;         /**< @brief Number of Recon iterations (NFFT 3) */
	T          m_epsilon;       /**< @brief Convergence criterium */
	T          m_alpha;
	size_t     m_imgsz;
	
	Plan       m_fplan;         /**< nfft  plan */
	Solver     m_iplan;         /**< infft plan */
	
    size_t     m_m;

};


#endif




