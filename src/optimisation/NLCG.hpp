/*
 * NLCG.hpp
 *
 *  Created on: Apr 1, 2015
 *      Author: kvahed
 */

#ifndef _NLCG_HPP_
#define _NLCG_HPP_

#include <NonLinear.hpp>

template<class T> inline T try_to_fetch (const Params& p, const std::string& key, const T& def) {
    try {
        return p.Get<T>(key);
    } catch (const PARAMETER_MAP_EXCEPTION& e) {
        printf ("**WARNING**: key %s not in parameter map.\n", key.c_str());
        return def;
    } catch (const boost::bad_any_cast& e) {
        printf ("**WARNING**: conversion failed on parameter %s", key.c_str());
        return def;
    }
}


#ifdef _MSC_VER
std::string ofstr = "    %02Iu - nrms: %1.4e, l-search: %d, ";
#else
std::string ofstr = "    %02zu - nrms: %1.4e, l-search: %d, ";
#endif

namespace codeare {
    namespace optimisation {
        
template<class T> class NLCG : public NonLinear<T> {

public:
    
    NLCG (const Params& p) : NonLinear<T>::NonLinear (p) {
        _cgiter = try_to_fetch<int> (p, "cgiter", 0);
        _cgconv = try_to_fetch<float> (p, "cgconv", 0);
        _lsiter = try_to_fetch<int> (p, "lsiter", 0);
        _lsa = try_to_fetch<float> (p, "lsa", 0);
        _lsb = try_to_fetch<float> (p, "lsb", 0);
        printf ("  Iterations: CG(%i) LS(%i)\n", _cgiter, _lsiter);
        printf ("  Conv: CG(%.2e)\n", _cgconv);
        printf ("  LS brackets: lsa(%.2e) lsb(%.2e)", _lsa, _lsb);
        
    }
    
	virtual ~NLCG() {};
    
    inline virtual void Minimise (Operator<T>* A, Matrix<T>& x) {

        typename TypeTraits<T>::RT t0  = 1.0, t = 1.0, z = 0., xn = norm(x), rmse, bk, f0, f1, dxn;
    
        _g0  = A->df (x);
        _dx  = -_g0;
    
        for (size_t k = 0; k < (size_t)_cgiter; k++) {
        
            A->Update(_dx);
            t = t0;
        
            f0 = A->obj (x, _dx, z, rmse);
        
            int i = 0;
            while (i < _lsiter) {
                t *= _lsb;
                f1 = A->obj (x, _dx, t, rmse);
                if (f1 <= f0 - (_lsa * t * abs(_g0.dotc(_dx))))
                    break;
                ++i;
            }
        
            printf (ofstr.c_str(), k, rmse, i); fflush (stdout);
        
            if (i == _lsiter) {
                printf ("Reached max line search, exiting... \n");
                return;
            }
        
            if      (i > 2) t0 *= _lsb;
            else if (i < 1) t0 /= _lsb;
        
            // Update image
            x  += _dx * t;
        
            // CG computation
            _g1 =  A->df (x);
            bk  =  real(_g1.dotc(_g1)) / real(_g0.dotc(_g0));
            _g0 =  _g1;
            _dx = -_g1 + _dx * bk;
            dxn =  norm(_dx)/xn;
        
            printf ("dxnrm: %0.4f\n", dxn);
            if (dxn < _cgconv)
                break;
        
        }

    }
        
private:    

    Matrix<T> _dx, _g0, _g1;
    typename TypeTraits<T>::RT _cgconv, _lsa, _lsb;
    int _lsiter, _cgiter;
    

};
        
    }}

#endif // _NLCG_HPP_
