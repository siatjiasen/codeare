#ifndef __IO_HPP__
#define __IO_HPP__

#include "Toolbox.hpp"
#include "tinyxml/tinyxml.h"
#include "tinyxml/xpath_static.h"
#include "mdhVB15.h"
#include "Algos.hpp"

#ifdef HAVE_NIFTI1_IO_H
#include <nifti1_io.h>
#endif

#ifdef HAVE_H5CPP_H
#include <H5Cpp.h>
using namespace H5;
#endif

#ifdef HAVE_CDF_H
#include <cdf.h>
#endif


#include <limits>
#include <map>
#include <sstream>
#include <iomanip>

using namespace TinyXPath;
using namespace std;


class IO {
	
public:
	
	IO ();

	virtual ~IO();

	inline static bool
	FExists (const char* fname) {
		
		ifstream fs (fname);
		
		if (fs)
			return true;
		else
			return false;
		
	}
	
	
	template <class T> inline static std::string 
	DimsToString (const Matrix<T>& M) {
		
		std::stringstream ss;
		
		for (size_t i = 0; i <= Algos::HDim(M); i++)
			ss << (int)M.Dim(i) << " ";
		
		return ss.str();
		
	}
	
	
	template <class T> inline static const char* 
	DimsToCString (const Matrix<T>& M) {
		
		return DimsToString(M).c_str();
		
	}
	
	
	template <class T> inline static std::string 
	ResToString (const Matrix<T>& M) {
		
		stringstream ss;
		
		for (size_t i = 0; i <= Algos::HDim(M); i++)
			ss << M.Res(i) << " ";
		
		return ss.str();
		
	}


	template <class T> inline static const char* 
	ResToCString (const Matrix<T>& M) {
		
		return ResToString(M).c_str();
		
	}
	
	
	template <class T> inline static bool
	PRDump (const Matrix<T>& M, const string fname) {
		
		FILE *fp;
		
		if ((fp=fopen(fname.c_str(), "wb"))==NULL) {
			printf("Cannot open %s file.\n", fname.c_str());
			return false;
		}
		
		if (fwrite(&M[0], sizeof(float), M.Size(), fp) != M.Size()) {
			printf("File read error.");
			fclose(fp);
			return false;
		}
		
		fclose(fp);
		
		return true;
		
	}
	

	template <class T> inline static bool 
	Dump (const Matrix<T>& M, const string fname, const string dname, const string dloc = "", const io_strategy ios = HDF5) {
		
		if      (ios == HDF5)
			return H5Dump (M, fname, dname, dloc);
#ifdef HAVE_MAT_H
		else if (ios == MATLAB)
			return MXDump (M, fname, dname, dloc);
#endif
		else if (ios == NIFTI)
			return NIDump (M, fname);
		else
			return PRDump (M, fname);
	
	}


	template <class T> bool 
	H5Dump (const Matrix<T>& M, const string fname, const string dname, const string dloc = "") {
	
		size_t i = 0;
		
		if (fname != "") {
			
#ifdef HAVE_H5CPP_H
			try {
				
#ifndef VERBOSE
				Exception::dontPrint();
#endif
				
				H5File        file; 
				
				try {
					file = H5File  (fname, H5F_ACC_TRUNC);
#ifdef VERBOSE
					printf ("File %s opened for RW\n", fname.c_str());
#endif
				} catch (Exception e) {
					file = H5File  (fname, H5F_ACC_TRUNC);
#ifdef VERBOSE
					printf ("File %s created for RW\n", fname.c_str());
#endif
				}
				
				Group group, *tmp;
				
				std::string dloctmp = dloc;
				if (dloctmp.length() == 0)
					dloctmp = "/";
				
				try {
					
					group = file.openGroup(dloctmp);
#ifdef VERBOSE
					printf ("Group %s opened for writing\n", dloctmp.c_str()) ;
#endif
					
				} catch (Exception e) {
					
					size_t    depth   = 0;
					
					vector<string> sv;
					
					Toolbox::Instance()->Split (sv, dloctmp, "/");

					for (size_t i = 0; i < sv.size(); i++) {
						
						try {
							if (depth)
								group = (*tmp).openGroup(sv.at(i));
							else
								group = file.openGroup(sv.at(i));
						} catch (Exception e) {
							if (depth)
								group = (*tmp).createGroup(sv.at(i));
							else
								group = file.createGroup(sv.at(i));
						}
						
						tmp = &group;
						depth++;
						
					}
					
					group = (*tmp);
					
				}
				
				// One more field for complex numbers
				size_t tmpdim = (typeid(T) == typeid(cxfl)) ? INVALID_DIM+1 : INVALID_DIM;
				hsize_t* dims = new hsize_t[tmpdim];

				for (i = 0; i < INVALID_DIM; i++)
					dims[i] = M.Dim(INVALID_DIM-1-i);
			
				if (typeid(T) == typeid(cxfl))
					dims[INVALID_DIM] = 2;
				
				DataSpace space (tmpdim, dims);
				PredType*  type;
				
				delete [] dims;

				string _dname = dname;
				
				if (typeid(T) == typeid(cxfl)) {
					type = (PredType*) new FloatType (PredType::NATIVE_FLOAT);
					if (dname == "") 
						_dname = "cxfl";
				} else if (typeid(T) == typeid(double)) {
					type = (PredType*) new FloatType (PredType::NATIVE_DOUBLE);
					if (dname == "") 
						_dname = "double";
				} else if (typeid(T) == typeid(short)){
					type = (PredType*) new IntType   (PredType::NATIVE_SHORT);
					if (dname == "") 
						_dname = "pixel";
				}
				
				DataSet set = group.createDataSet(_dname, (*type), space);
				
				set.write   (&M[0], (*type));
				set.close   ();
				space.close ();
				file.close  ();
				
			} catch(FileIException      e) {
				e.printError();
				return false;
			} catch(DataSetIException   e) {
				e.printError();
				return false;
			} catch(DataSpaceIException e) {
				e.printError();
				return false;
			} catch(DataTypeIException  e) {
				e.printError();
				return false;
			}
			
#else // HAVE_H5CPP_H
			
			printf ("HDF5 ERROR - Didn't dump nothin'")
		
#endif // HAVE_H5CPP_H
				
				}
		
		return true;
		
	}
	

	
	template <class T> static bool
	RSAdjust (Matrix<T>& M, const std::string& fname) {
		
		size_t dimk;
		size_t dimv;
		
		// XML file name, run cmd and repository
		stringstream    xmlf;
		stringstream    cmd;
		
		// XML objects
		TiXmlDocument*       doc = new TiXmlDocument();
		TiXmlElement*       meta;
		TiXmlNode*           vol;
		
		// Dimensions in XProtocol
		
		map < string, size_t > dims;
		dims["NImageCols"] =  0; dims["NLinMeas"] =  1; dims["NSlcMeas"] =  2; dims["NParMeas"] =  3;
		dims[  "NEcoMeas"] =  4; dims["NPhsMeas"] =  5; dims["NRepMeas"] =  6; dims["NSetMeas"] =  7;
		dims[  "NSegMeas"] =  8; dims[  "RawCha"] =  9; dims["NIdaMeas"] = 10; dims["NIdbMeas"] = 11;
		dims[  "NIdcMeas"] = 12; dims["NIddMeas"] = 13; dims["NIdeMeas"] = 14; dims["NAveMeas"] = 15; 
		
		string channels  = "RawCha";
		string doublevals [2] = {"RoFOV", "PeFOV"};
		string slcthickn = "SliceThickness";
		
		// Create XML output from XProt
		cmd << "/usr/local/bin/convert.pl ";
		cmd << fname;
		size_t ec = system (cmd.str().c_str());

		// Output filename
		xmlf << fname;
		xmlf << ".xml";
		
		// Read XML and go to rawobjectprovider for dimensions
		doc->LoadFile (xmlf.str().c_str());
		meta = doc->RootElement();

		TiXmlNode* rootfunctor    = TinyXPath::XNp_xpath_node 
			(meta, "/Meta/XProtocol[1]/ParamMap[1]/ParamMap[1]/Pipe[1]/PipeService[@name='Online1']/ParamFunctor[@name='root']");
		
		// Dimensions
		vol  = rootfunctor->FirstChild ("ParamLong");
		map < string, size_t >::iterator it;
		string key;
		
		do {
			
			key = ((TiXmlElement*)vol)->Attribute("name");
			it = dims.find (key);
			
			if (it != dims.end()) {
				
				dimk = dims[key];
				dimv = atoi( ((TiXmlElement*)vol)->Attribute("value"));
				
				if (dimk == 0) dimv *= 2;
				if (dimv == 0) dimv = 1;
				
				M.Dim(dimk) = dimv;
				
			}
			
		} while ((vol = rootfunctor->IterateChildren ("ParamLong", vol))!=NULL);
		
		M.Res(0) = (float)TinyXPath::d_xpath_double
			(meta, "/Meta/XProtocol[1]/ParamMap[1]/ParamMap[1]/Pipe[1]/PipeService[@name='Online1']/ParamFunctor[@name='root']/ParamDouble[@name='RoFOV']/@value") / M.Dim(0) * 2;
		M.Res(1) = (float)TinyXPath::d_xpath_double 
			(meta, "/Meta/XProtocol[1]/ParamMap[1]/ParamMap[1]/Pipe[1]/PipeService[@name='Online1']/ParamFunctor[@name='root']/ParamDouble[@name='PeFOV']/@value") / M.Dim(1);
		M.Res(3) = (float)TinyXPath::d_xpath_double
			(meta, "/Meta/XProtocol[1]/ParamMap[1]/ParamMap[1]/Pipe[1]/PipeService[@name='Online1']/ParamFunctor[@name='root']/ParamArray[@name='SliceThickness']/ParamDouble[1]/Precision[1]/@value") / M.Dim(3);
		
		M.Dim(9) = i_xpath_int 
			(meta, "/Meta/XProtocol[1]/ParamMap[1]/ParamMap[1]/Pipe[1]/PipeService[@name='Online2']/ParamFunctor[@name='rawobjprovider']/ParamLong[@name='RawCha']/@value"); 
		
		M.Reset();
	
		delete doc;
		return true;
		
	}


	inline static void
	ProgressBar (const std::string& pre, const std::string& post, const short& p) {
		
		assert (p >=   0);
		assert (p <= 100);
		
		std::cout << "\r";
		std::cout << pre.c_str();
		std::cout << " | ";
		std::cout << bars.substr(0, p/2) << " " <<  blancs.substr(0, 50-p/2) << "| " << std::setw(3) << std::setfill(' ') << p << "% done";
		
	}


	template <class T> static bool 
	RAWRead (Matrix<T>& M, const std::string& fname, const std::string& version) {
	
		// Get size information from XProtocol and resize 
		ticks  tic = getticks();
		
		RSAdjust(M, fname);
		
		printf ("%s: %s\n", fname.c_str(), DimsToCString(M));

		FILE*         f;
		sMDH*         mdh;
		unsigned      l      = 0;
		size_t        nscans = (M.Size() / M.Dim(COL));
		unsigned      start;
		unsigned      size, read;
		
		// Assess data size
		f = fopen (fname.c_str(), "rb");
		read = fread (&l, sizeof(unsigned), 1, f);
		fseek (f,     l, SEEK_SET);
		start = ftell(f);
		fseek (f,    -1, SEEK_END);
		size  = ftell(f);
		fseek (f, start, SEEK_SET);
		
		//printf ("Found %.1f MB of header and data.\n", (float)size / MB);
		stringstream pre;
		
		float bsize = (float) size / (1024.0*1024.0);
		pre << "  Reading " << bsize << "MB ... ";

		// Headers
		mdh  = (sMDH*) malloc (nscans * sizeof(sMDH));
		size_t n    = 0.0;
		char   mask[8];
		bool   done = false;

		const std::string post = "";
		
		size_t i   = 0;
		
		for (i = 0; i < nscans; i++) {
			
			if (i % 250 == 0 || i == nscans-1)
				ProgressBar (pre.str(), post, (short)round((float)(i+1)/(float)nscans*100.0));
		
			read = fread (&mdh[i], sizeof(sMDH), 1, f);
		
			long m = 0;
			memcpy (&m, mdh[i].aulEvalInfoMask, 2 * sizeof (unsigned));
			if (m == 0x00000001) {
				ProgressBar (pre.str(), post, 100);
				done = true;
				break;
			}
			
			n = mdh[i].sLC.ushLine       * M.Dim(0) +
				mdh[i].sLC.ushSlice      * M.Dim(0) * M.Dim(1) +
				mdh[i].sLC.ushPartition  * M.Dim(0) * M.Dim(1) * M.Dim(2) +
				mdh[i].sLC.ushEcho       * M.Dim(0) * M.Dim(1) * M.Dim(2) * M.Dim(3) +
				mdh[i].sLC.ushPhase      * M.Dim(0) * M.Dim(1) * M.Dim(2) * M.Dim(3) * M.Dim(4) +
				mdh[i].sLC.ushRepetition * M.Dim(0) * M.Dim(1) * M.Dim(2) * M.Dim(3) * M.Dim(4) * M.Dim(5) +
				mdh[i].sLC.ushSet        * M.Dim(0) * M.Dim(1) * M.Dim(2) * M.Dim(3) * M.Dim(4) * M.Dim(5) * M.Dim(6) +
				mdh[i].sLC.ushSeg        * M.Dim(0) * M.Dim(1) * M.Dim(2) * M.Dim(3) * M.Dim(4) * M.Dim(5) * M.Dim(6) * M.Dim(7) +
				mdh[i].ushChannelId      * M.Dim(0) * M.Dim(1) * M.Dim(2) * M.Dim(3) * M.Dim(4) * M.Dim(5) * M.Dim(6) * M.Dim(7) * M.Dim(8);
			
			read = fread (&M[n], sizeof (complex<float>), M.Dim(0), f);
			
		}
		
		fclose (f);
		
		float e = ((float)elapsed(getticks(),tic) / (float)Toolbox::Instance()->ClockRate());
		printf ("\n  done. Loaded %i records (%f MB/s).\n", (int)i, bsize/e);
		
		return true;
		
	}
	
	template <class T> inline static bool 
	Read (Matrix<T>& M, const std::string& fname, const std::string& dname, const std::string& dloc = "", const io_strategy& ios = HDF5) {
		
		if      (ios == HDF5)
			return H5Read (M, fname, dname, dloc);
#ifdef HAVE_MAT_H
		else if (ios == MATLAB)
			return MXRead (M, fname, dname, dloc);
#endif
		else if (ios == NIFTI)
			return NIRead (M, fname);
		
		return true;
		
	}


	template <class T> static bool 
	H5Read (Matrix<T>& M, const string fname, const string dname, const string dloc = "") {
		
		if (fname != "") {
			
#ifdef HAVE_H5CPP_H
			
#ifndef VERBOSE
			Exception::dontPrint();
#endif
			
			try {
				
				
				H5File    file (fname, H5F_ACC_RDONLY);
				DataSet   dataset = file.openDataSet(dloc+"/"+dname);
				DataSpace space   = dataset.getSpace();
				
				hsize_t*  dims    = (hsize_t*) malloc (space.getSimpleExtentNdims() * sizeof (hsize_t));
				size_t    ndim    = space.getSimpleExtentDims(dims, NULL);
				
				if (typeid(T) == typeid(cxfl)) 
					ndim--;
				
				for (size_t i = 0; i < ndim; i++)
					M.Dim(i) = dims[ndim-i-1];
				
				for (size_t i = ndim; i < INVALID_DIM; i++)
					M.Dim(i) = 1;
				
				M.Reset();

				cout << "HDF5 file: rank(" << ndim << "), dimensions(";
				for (size_t i = 0; i < ndim; i++) {
					cout << (unsigned long)(dims[i]);
					if (i == ndim - 1)
						cout << ")" << endl;
					else
						cout << " x ";
				}

				
				PredType*  type;
				
				if (typeid(T) == typeid(cxfl))
					type = (PredType*) new FloatType (PredType::NATIVE_FLOAT);
				else if (typeid(T) == typeid(double))
					type = (PredType*) new FloatType (PredType::NATIVE_DOUBLE);
				else
					type = (PredType*) new IntType   (PredType::NATIVE_SHORT);
				
				dataset.read (&M[0], (*type));
				
				space.close();
				dataset.close();
				file.close();
				
			} catch(FileIException      e) {
				e.printError();
				return false;
			} catch(DataSetIException   e) {
				e.printError();
				return false;
			} catch(DataSpaceIException e) {
				e.printError();
				return false;
			} catch(DataTypeIException  e) {
				e.printError();
				return false;
			}
			
#else // HAVE_H5CPP_H
		
			printf ("HDF5 ERROR - Didn't read nothin'")
		
#endif // HAVE_H5CPP_H
				
				}
		
		return true;
		
	}
	
	
#ifdef HAVE_MAT_H
	
	template <class T> static bool
	MXRead (Matrix<T>& M, const string fname, const string dname, const string dloc = "") {
		
		// Open file ---------------------------------
		
		MATFile*  mf = matOpen (fname.c_str(), "r");
		
		if (mf == NULL) {
			printf ("Error opening file %s\n", fname.c_str());
			return false;
		}
		// -------------------------------------------
		
		// Get dimensions ----------------------------
		
		mxArray*      mxa = matGetVariable (mf, dname.c_str());
		
		if (mxa == NULL) {
			printf ("Error opening variable %s\n", dname.c_str());
			return false;
		}
		
		mxClassID     mcid = mxGetClassID(mxa);
		
		mwSize        ndim = mxGetNumberOfDimensions(mxa);
		const mwSize*  dim = mxGetDimensions(mxa);
		
		for (size_t i = 0; i < ndim; i++)
			M.Dim(i) = (size_t)dim[i];
		
		for (size_t i = ndim; i < INVALID_DIM; i++)
			M.Dim(i) = 1;
		
		M.Reset();
		// -------------------------------------------
		
		// Copy from memory block ----------------------
		
		printf ("  Reading %s: (%s) ... ", dname.c_str(), DimsToCString(M)); fflush(stdout);
		
		if (typeid(T) == typeid(double) || typeid(T) == typeid(double)) {
			memcpy(&M[0], mxGetPr(mxa), M.Size() * sizeof(T));
		} else if (typeid(T) == typeid(cxfl)) {
			if (mxGetPi(mxa) != NULL)
				for (size_t i = 0; i < M.Size(); i++) {
					float f[2] = {((float*)mxGetPr(mxa))[i], ((float*)mxGetPi(mxa))[i]}; // Template compilation. Can't create T(real,imag) 
					memcpy(&M[i], f, 2 * sizeof(float));
				}
			else
				for (size_t i = 0; i <M.Size(); i++) {
					float f[2] = {((float*)mxGetPr(mxa))[i], 0.0}; 
					memcpy(&M[i], f, 2 * sizeof(float));
				}
		} else
			for (size_t i = 0; i <M.Size(); i++)
				M[i] = ((T*)mxGetPr(mxa))[i];
		
		printf ("done\n");
		// -------------------------------------------
		
		// Clean up and close file -------------------
		if (mxa != NULL)
			mxDestroyArray(mxa);
		
		if (matClose(mf) != 0) {
			printf ("Error closing file %s\n",fname.c_str());
			return false;
		}
		// -------------------------------------------
		
		return true;
		
	}

	
	template <class T> static bool
	MXDump (Matrix<T>& M, MATFile* mf, const string dname, const string dloc = "") {
		
		// Declare dimensions and allocate array -----
		
		mwSize   dim[INVALID_DIM];
		
		for (size_t i = 0; i < INVALID_DIM; i++)
			dim[i] = (mwSize)M.Dim(i);
	
		mxArray*  mxa;

		if      (typeid(T) == typeid(double))
			mxa = mxCreateNumericArray (INVALID_DIM, dim, mxDOUBLE_CLASS,    mxREAL);
		else if (typeid(T) == typeid(float))
			mxa = mxCreateNumericArray (INVALID_DIM, dim, mxSINGLE_CLASS,    mxREAL);
		else if (typeid(T) == typeid(cxfl))
			mxa = mxCreateNumericArray (INVALID_DIM, dim, mxSINGLE_CLASS, mxCOMPLEX);
		else if (typeid(T) == typeid(short))
			mxa = mxCreateNumericArray (INVALID_DIM, dim,  mxINT16_CLASS,    mxREAL);
		// -------------------------------------------
		
		
		// Copy to memory block ----------------------
		
		if (typeid(T) == typeid(cxfl))
			for (size_t i = 0; i <M.Size(); i++) {
				((float*)mxGetPr(mxa))[i] = ((float*)&M[0])[2*i+0]; // Template compilation workaround
				((float*)mxGetPi(mxa))[i] = ((float*)&M[0])[2*i+1]; // Can't use .imag() .real(). Consider double/short 
			}
		else 
			memcpy(mxGetPr(mxa), &M[0],M.Size() * sizeof(T));
		
		// -------------------------------------------
		
		// Write data --------------------------------
		int status = matPutVariable(mf, dname.c_str(), mxa);
		
		if (status != 0) {
			printf("%s :  Error using matPutVariable on line %d\n", __FILE__, __LINE__);
			return false;
		}
		// -------------------------------------------
		
		
		// Clean up RAM ------------------------------
		
		if (mxa != NULL)
			mxDestroyArray(mxa);
		// -------------------------------------------
		
		return true;
		
	}
	

	
	template <class T> static bool 
	MXDump (Matrix<T>& M, const string fname, const string dname, const string dloc = "") {
		
		// Open file ---------------------------------
		
		MATFile*  mf = matOpen (fname.c_str(), "w");
		
		if (mf == NULL) {
			printf ("Error creating file %s\n", fname.c_str());
			return false;
		}
		// -------------------------------------------
		
		MXDump (M, mf, dname, dloc);	
		
		
		// Close file --------------------------------
		
		if (matClose(mf) != 0) {
			printf ("Error closing file %s\n",fname.c_str());
			return false;
		}
		// -------------------------------------------
		
		return true;
		
	}
	
#endif
	
	
	template <class T> static bool
	NIDump (Matrix<T>& M, const string fname) {
		
		if (fname != "") {
			
#ifdef HAVE_NIFTI1_IO_H
			
			Matrix<T>      tmp = M;
			Algos::Squeeze(tmp);
			
			size_t            l   = fname.length();
			
			nifti_1_header header;
			header.sizeof_hdr = 348;
			header.dim[0] = Algos::HDim(tmp) + 1;
			header.pixdim[0] = Algos::HDim(tmp) + 1;
			
			if (Algos::HDim(tmp) > 7) {
				printf ("Cannot dump more than 8 dimensions to NIFTI FILE\n.");
				return false;
			}
			
			for (size_t i = 0; i < 7; i++) {
				header.dim   [i+1] = tmp.Dim(i);
				header.pixdim[i+1] = tmp.Res(i);
			}
			
			if      (typeid(T) == typeid(cxfl))
				header.datatype = 32;
			else if (typeid(T) == typeid(double))
				header.datatype = 64;
			else if (typeid(T) == typeid(short))
				header.datatype = 256;
			
			nifti_image* ni = nifti_convert_nhdr2nim(header, NULL);
			
			ni->nifti_type = 1;
			
			// Single nii.gz file
			ni->fname = (char*) calloc(1,l); 
			strcpy(ni->fname,fname.c_str());
			ni->iname = (char*) calloc(1,l); 
			strcpy(ni->iname,fname.c_str());
			
			ni->data = (void*) malloc (M.Size() * sizeof (T));
			memcpy (ni->data, &M[0], M.Size() * sizeof (T));
			
			nifti_image_write (ni);
			nifti_image_free (ni); 
			
			return true;
			
#else 
			
			return false;
			
#endif
			
		} else {
			
			return false;
			
		}
		
	}
	
	
	template <class T> static bool
	NIRead (Matrix<T>& M, const string fname) {
		
		if (fname != "") {
			
#ifdef HAVE_NIFTI1_IO_H
			
			nifti_image* ni = nifti_image_read (fname.c_str(), 1);
			
			if (ni == NULL) 
				return false;
			
			for (size_t i = 0; i < ni->dim[0]; i++)
				if (ni->dim[i+1] > 1) {
					M.Dim(i) = ni->dim[i+1];
					M.Res(i) = ni->pixdim[i+1];
			}
			
			for (size_t i = ni->dim[0]; i < INVALID_DIM; i++) {
				M.Dim(i) = 1;
				M.Res(i) = 0.0;
			}
			
			M.Reset();
			
			if ((ni->datatype == 16 || ni->datatype == 64) && typeid(T) == typeid(double)) {
				if (ni->datatype == 64)
					memcpy (&M[0], ni->data,M.Size()*sizeof(T));
				else 
					for (size_t i = 0; i <M.Size(); i++ )
						M[i] = ((float*)ni->data)[i];
			} else if ((ni->datatype == 32 || ni->datatype == 1792) && typeid(T) == typeid(cxfl)) {
				if (ni->datatype == 32)
					memcpy (&M[0], ni->data,M.Size()*sizeof(T));
				else 
					for (size_t i = 0; i <M.Size(); i++) {
						float f[2] = {((double*)ni->data)[2*i], ((double*)ni->data)[2*i+1]};
						memcpy(&M[i], f, 2 * sizeof(float));
					}
			} else if ((ni->datatype == 256 || ni->datatype == 4) && typeid(T) == typeid(short)) {
				if (ni->datatype == 256 || ni->datatype == 4)
					memcpy (&M[0], ni->data, Algos::SizeInRAM(M));
				
			} else {
				printf (" Unsupported data type %i!", ni->datatype);
				return false;
			}
			
			nifti_image_free (ni);
			
			return true;
			
#else 
			
			return false;
			
#endif
			
		} else {
			
			return false;
			
		}
		
	}
	
	
	// Function not finished yet. Do not use.
	template <class T> static bool 
	CDFDump (const Matrix<T>& M, const string fname, const string dname, const string dloc) {
		
#ifdef HAVE_CDF_H
		
		CDFid     id;                // CDF identifier.
		CDFstatus status;            // CDF completion status.
		
		FILE*     fp;
		
		long cType;                 // Compression type
		long cParms[CDF_MAX_PARMS]; // Compression parameters
		
		status = CDFcreateCDF (fname.c_str(), &id);
		
		if (status != CDF_OK) 
			return false;
		
		long dims [Algos::HDim(M)];
		long dimv [Algos::HDim(M)];
		
		for (int i = 0; i < Algos::HDim(M); i++) {
			dims[i] = M.Dim(i);
			dimv[i] = VARY;
		}
		
		long imvn;
		
		status = CDFcreatezVar (id, dname.c_str(), CDF_FLOAT, 1L, (long)HDim(), dims, VARY, dimv, &imvn);
		
		if (status != CDF_OK) 
			return false;
		
		cType = GZIP_COMPRESSION;
		cParms[0] = 5;             /* GZIP compression level */
		status = CDFsetzVarCompression (id, imvn, cType, cParms);
		
		if (status != CDF_OK) 
			return false;
		
		status = CDFcloseCDF (id);
		
		if (status != CDF_OK) 
			return false;
		
		return true;
		
#else 
		
		return false;
		
#endif
		
	}
	

	// Function not finished yet. Do not use.
	template <class T> static bool
	CDFRead (Matrix<T>& M, const string fname, const string dname, const string dloc) {
		
		return false;
		
	}
	
};
	
#endif //__IO_HPP__