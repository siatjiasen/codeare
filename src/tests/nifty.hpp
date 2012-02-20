template <class T> bool
nitest (Connector<T>* rc) {

#ifdef HAVE_NIFTI1_IO_H
	
	std::string    cf  = std::string (base + std::string(config));
	std::string    df  = std::string (base + std::string(data));
	std::string    mat = std::string (base + std::string("betted.mat"));
	std::string    nii = std::string (base + std::string("betted2.nii.gz"));
	
	Matrix<double> d;
	IO::NIRead (d, df);
#ifdef HAVE_MAT_H	
	IO::MXDump (d, mat, "betted");
#endif
	IO::NIDump (d, nii);

	Matrix<cxfl> slp = Matrix<cxfl>::Phantom3D(196); 
	IO::NIDump (slp, nii);
#ifdef HAVE_MAT_H	
	IO::MXDump (slp, mat, "betted");
#endif

#else

	std::cout << "No nifti support compiled in. Bailing out." << std::endl;

#endif

	return true;

}


