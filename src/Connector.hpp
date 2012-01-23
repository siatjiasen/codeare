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

#ifndef __CONNECTOR_HPP__
#define __CONNECTOR_HPP__

#ifdef __WIN32__ 
    #include "RRSModule.h"
#else
    #include "RRSModule.hh"
#endif

using namespace RRSModule;

#include "Matrix.hpp"

namespace RRClient {

/**
 * @brief Connector skeleton
 *        Abstraction layer for local or remote access to reconstruction schemes
 */
template <class T>
class Connector {

public:
	

	/**
	 * @brief  Default constructor 
	 * @see    Connector (const connection_type)
	 */
	Connector () {

	}
	
	
	/**
	 * @brief       Construct with service name and debug level.
	 *
	 * @param name  Service name
	 * @param debug Trace level
	 */
	Connector       (const char* name, const char* debug);
	
	
	/**
	 * @brief 
	 */
	virtual 
	~Connector () {
		
	}


	/**
	 * @brief           Request data procession on remote service
	 *
	 * @param  name     Recon method
	 * @return          Error code
	 */ 
	virtual error_code              
	Process             (const char* name) {
		m_conn->Process(name);
	}
	
	
	/**
	 * @brief           Prepare backend
	 *
	 * @param  name     Recon method
	 * @return          Error code
	 */ 
	virtual error_code              
	Prepare             (const char* name) {
		m_conn->Prepare(name);
	}
	
	
	/**
	 * @brief           Initialise remote service
	 *
	 * @param  name     Recon method
	 * @return          Error code
	 */ 
	virtual error_code              
	Init                (const char* name) {
		m_conn->Init(name);
	}
	
	
	/**
	 * @brief           Finalise remote service
	 *
	 * @param  name     Recon method
	 * @return          Error error
	 */ 
	virtual error_code              
	Finalise            (const char* name) {
		m_conn->Finalise(name);
	}
	
	
	/**
	 * @brief           Transmit measurement data to remote service
	 *
	 * @param  name     Name
	 * @param  m        Complex data
	 */
	template <class S> void 
	SetMatrix           (const std::string& name, Matrix<S>& m) const {
		m_conn->SetMatrix (name, m);
	}
	
	
	/**
	 * @brief           Retrieve manipulated data from remote service
	 *
	 * @param  name     Name
	 * @param  m        Receive storage
	 */
	template <class S> void 
	GetMatrix           (const std::string& name, Matrix<S>& m) const {
		m_conn->GetMatrix (name, m);
	}
		
		
	/**
	 * @brief          Read configuration from XML file
	 *
	 * @param  fname   Name of input file
	 */
	template <class S> void 
	ReadConfig        (S config) {
		m_conn->ReadConfig (config);
	}
	

	/**
	 * @brief           Set a string type attribute
	 *
	 * @param  name     Attribute name 
	 * @param  value    Attribute value
	 */
	template <class S> inline void
	SetAttribute        (const char* name, S value) {
		m_conn->SetAttribute (name, value);
	}

	
	/**
	 * @brief           Set a string type attribute
	 *
	 * @param  name     Attribute name 
	 * @param  value    Attribute value
	 */
	template <class S> inline void
	Attribute        (const char* name, S* value) {
		m_conn->SetAttribute (name, value);
	}

	


	
protected:
	
	
	T* m_conn;
	
};
	

}
#endif
