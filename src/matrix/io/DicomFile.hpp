/*
 * Dicom.hpp
 *
 *  Created on: Sep 11, 2015
 *      Author: kvahed
 */

#ifndef SRC_MATRIX_IO_DICOM_HPP_
#define SRC_MATRIX_IO_DICOM_HPP_

#include "IOFile.hpp"

#include "itkVersion.h"

#include "itkImage.h"
#include "itkMinimumMaximumImageFilter.h"

#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkNumericSeriesFileNames.h"

#include "itkImageSeriesReader.h"
#include "itkImageSeriesWriter.h"

#include <itksys/SystemTools.hxx>

#if ITK_VERSION_MAJOR >= 4
#include "gdcmUIDGenerator.h"
#else
#include "gdcm/src/gdcmFile.h"
#include "gdcm/src/gdcmUtil.h"
#endif

namespace codeare {
namespace matrix {
namespace io {

class DicomFile : public IOFile {
public:

	DicomFile  (const std::string& fname, const IOMode mode = READ,
			Params params = Params(), const bool verbose = false) :
				IOFile(fname, mode, params, verbose) {
		std::stringstream testfilestr;
		testfilestr << std::string(".");
		testfilestr << fname;
		ofstream testfile(testfilestr.str());
		if (!testfile.is_open())
			throw OPEN_RW_FAILED;
		std::remove(testfilestr.str().c_str());
		typedef itk::NumericSeriesFileNames OutputNamesGeneratorType;
		OutputNamesGeneratorType::Pointer outputNames = OutputNamesGeneratorType::New();
		this->m_status = OK;
	}

	template<class T> Matrix<T>
			Read (const std::string& uri) throw () {
		return Matrix<T>();
	}

	template<class T> bool
			Write (const Matrix<T>& M, const std::string& uri) throw () {

		return true;
	}


	/**
	 * @brief  Default destructor
	 */
	virtual ~DicomFile () {}

};

} /* namespace io */
} /* namespace matrix */
} /* namespace codeare */

#endif /* SRC_MATRIX_IO_DICOM_HPP_ */
