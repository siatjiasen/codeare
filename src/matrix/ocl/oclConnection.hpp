# ifndef __OCL_CONNECTION_HPP__



  /************
   ** makros **
   ************/
  # define __OCL_CONNECTION_HPP__ 
  # define __CL_ENABLE_EXCEPTIONS



  /**************
   ** includes **
   **************/

  // OpenCL
  # include <CL/cl.hpp>

  // C++ std. headers
  # include <map>
  # include <list>

  // ocl
  # include "oclSettings.hpp"



  /**************************
   ** forward declarations **
   **************************/
  class oclDataObject;
  class oclFunctionObject;
  class vclAlgoFunctor;
  


  /**************************
   ** class: oclConnection **
   **  (singleton)         **
   **************************/
  class oclConnection
  {

  
    public:

      /***********************
       ** enumeration types **
       ***********************/
      enum KernelType {KERNEL, VCL};
      enum SyncType   {SYNC, ASYNC};


      /**********************
       ** type definitions **
       **********************/
      typedef cl::Platform      clPlatform;
      typedef cl::Device        clDevice;
      typedef cl::Context       clContext;
      typedef cl::Program       clProgram;
      typedef cl::CommandQueue  clCommandQueue;
      typedef cl::Kernel        clKernel;
      typedef cl::Buffer        clBuffer;
      typedef std::vector <clPlatform>      clPlatforms;
      typedef std::vector <clDevice>        clDevices;
      typedef std::vector <clCommandQueue>  clCommandQueues;
      typedef std::vector <clKernel>        clKernels;
      typedef std::vector <clBuffer *>      clBuffers;
      

      /***************
       ** functions **
       ***************/
      inline
      static
      oclConnection *
      Instance              ();

      void
      Initialise            ();
    
      void
      Finalise              ();
      
      oclFunctionObject * const
      makeFunctionObject    (const string & kernel_name,
                             oclDataObject * const * const args, const int & num_args,
                             const KernelType kernel_type, const SyncType sync_type);
                              
      void
      addDataObject         (oclDataObject * const ocl_obj);
      void
      removeDataObject      (const oclDataObject * const ocl_obj);
                          
      void
      run                   (oclFunctionObject * const func_obj) const;

      // getters for OpenCL objects
      const clPlatform      getPlatform       () const { return m_plat; }
      const clDevices       getDevices        () const { return m_devs; }
      const clContext       getContext        () const { return m_cont; }
      const clCommandQueues getCommandQueues  () const { return m_comqs; }
      const clProgram       getProgram        () const { return m_prog; }
      
      
      /****************************************
       ** functions for OpenCL functionality **
       ****************************************/
      
      /**
       * @brief             load cpu data to GPU and return corresponding buffer
       *                      
       */
      template <class T>
      void
      loadToGPU             (       T   * const cpu_arg,
                             ::size_t           size,
                             clBuffer   * const buffer);
      
      
      /**
       * @brief             create buffer for given object id
       */
      template <class T>
      void
      createBuffer          (                T   * const cpu_arg,
                             const    ::size_t           size,
                             const oclObjectID           obj_id);
      
      
      /**
       * @brief             load data from given buffer to given cpu pointer
       *
       * @param             size - ... in bytes
       */
      template <class T>
      void
      loadToCPU             (const clBuffer * const buffer,
                                          T * const cpu_arg,
                             const ::size_t         size)
      {
      
        std::cout << "oclConnection :: loadToCPU" << std::endl;
        
        try
        {
        
          // create new buffer
          clBuffer * p_tmp_buffer = new clBuffer(m_cont, CL_MEM_READ_WRITE, size, cpu_arg, &m_error);
          
          std::cout << " -- size: " << size << " Bytes" << std::endl;
          
          // read data from given buffer
          m_error = m_comqs [0] . enqueueReadBuffer (/* * buffer */ *buffer, CL_TRUE, 0, size, cpu_arg, NULL, NULL);
        
        }
        catch (cl::Error cle)
        {

          cout << "error: " << errorString (cle.err()) << endl;

        }
      
      }
      
  
      // activate kernel (stays activated until another kernel is activated
      int
      activateKernel        (const std::string kernelname);
    
      // run kernel with given dimensions
      int
      runKernel             (const cl::NDRange & global_dims,
                             const cl::NDRange & local_dims);
    
    
      // set argument for kernel by pointer, optionally return created buffer
      template <typename T>
      cl_int
      setKernelArg            (int           num,
                               T           * in_argument,
                               ::size_t      size,
                               cl_mem_flags  mem_flag,
                               clBuffer    * out = NULL)
      {
    
        // create new buffer
        clBuffer * p_tmp_buffer = new clBuffer(m_cont, mem_flag, size, in_argument, &m_error);

        // if buffer is used for input, copy data to device
        if (   mem_flag == CL_MEM_READ_ONLY
            || mem_flag == CL_MEM_READ_WRITE)
        {
      
          // loop over command queues, write information to device (global memory)
          for (clCommandQueues::iterator it = m_comqs.begin(); it < m_comqs.end(); ++it)
          {
        
            m_error = it -> enqueueWriteBuffer (*p_tmp_buffer, CL_TRUE, 0, size, in_argument, NULL, NULL);

          }
        
        }
      
        // register buffer as argument      
        m_error = mp_actKernel -> setArg (num, *p_tmp_buffer);
        m_buffers [num_kernel].push_back (p_tmp_buffer);

        // if buffer is used for output, return buffer object
        if (   mem_flag == CL_MEM_READ_WRITE
            || mem_flag == CL_MEM_WRITE_ONLY)
        {

          out = p_tmp_buffer;

        }
        else // delete buffer, if not needed
        {

          out = NULL;
  
        }
  
      }
  
  
      // TODO: distinction between read/write, read_only, write_only !!!
      // set argument for kernel by buffer
      template <typename T>
      cl_int
      setKernelArg            (int            num,
                               clBuffer     * buf,
                               T            * in_argument,
                               ::size_t       size)
      {
      
        cout << " ** !! not to use !! ** " << endl;

        for (clCommandQueues::iterator it = m_comqs.begin(); it < m_comqs.end(); ++it)
        {

          m_error = it -> enqueueWriteBuffer (*buf, CL_TRUE, 0, size, in_argument, NULL, NULL);

        }

        m_error = mp_actKernel -> setArg (num, *buf);

      }
      
      
      /**
       * @brief               register kernel argument
       */
      cl_int
      setKernelArg            (int              num,
                               oclDataObject *  obj_id);
    
    
      // get argument from activated kernel
      template <typename T>
      cl_int
      getKernelArg            (int        num,
                               T        * out_argument,
                               ::size_t   size)
      {

        try {

          m_error = m_comqs [0] . enqueueReadBuffer (*(m_buffers [num_kernel] [num]), CL_TRUE, 0, size, out_argument, NULL, NULL);

        } catch (cl::Error cle) {

          cout << "error: " << errorString (cle.err()) << endl;

        }

        return m_error;

      }
    
    
      void
      print_ocl_objects      ();
    
    
    private:
    
      /**********************
       ** member variables **
       **********************/
      static oclConnection                                * mp_inst;
      std::map <oclObjectID, oclDataObject * const>         m_current_ocl_objects;
      std::list <oclObjectID>                               m_loaded_ocl_objects;
      std::map <clBuffer *, int>                            m_current_buffers;
      
      /********************
       ** OpenCL members **
       ********************/
      clPlatform              m_plat;       // available platforms
      clDevices               m_devs;       // vector of available devices on m_plat
      clContext               m_cont;       // context used associated with m_devs
      clCommandQueues         m_comqs;      // each command queue is associated with corresponding device from m_devs
      clProgram               m_prog;       // program containing whole source code
      clKernels               m_kernels;    // kernels available in m_prog
      std::vector<clBuffers>  m_buffers;    // vectors of buffers for each kernels arguments
      cl_int                  m_error;      // error variable
      bool                    m_verbose;    // verbosity level
      clKernel              * mp_actKernel;
      int                     num_kernel;

      static const char * ReadSource   (const char *, int * size);
             int          BuildProgram ();
             const char * errorString (cl_int e);

  
      /******************
       ** constructors **
       ******************/
      oclConnection         (const char *,
                             cl_device_type device_type = CL_DEVICE_TYPE_GPU,
                             bool verbose = false);
      
      oclConnection         (oclConnection &) { /*TODO*/};
        

  }; // class oclConnection
  
  

  /*******************
   ** includes (II) **
   *******************/
   
  // ocl
  # include "oclFunctionObject.hpp"
  # include "oclDataObject.hpp"
  # include "oclKernelObject.hpp"
  # include "oclViennaClObject.hpp"
  # include "oclAsyncKernelObject.hpp"
  
  

  /**************************************
   ** initialization of static members **
   **************************************/  
  oclConnection *
  oclConnection ::
  mp_inst                   = NULL;
  
  
  
  /**************************
   ** function definitions **
   **************************/

  # include "oclConnection.cpp"
  


      void
      oclConnection :: 
      print_ocl_objects      ()
      {
      
        std::cout << " --------------------------------------------------- " << std::endl;
        std::cout << " oclDataObjects:" << std::endl;
        std::map <oclObjectID, oclDataObject * const> :: iterator it;
        for (it = m_current_ocl_objects.begin (); it != m_current_ocl_objects.end (); it++)
        {
        
          std::cout << " (" << it -> second -> getID () << ")" << std::endl;
        
        }
      
        std::cout << " --------------------------------------------------- " << std::endl;
      
      }


  
  /**
   * @brief             retrieve instance of oclConnection
   */
  inline
  oclConnection *
  oclConnection ::
  Instance              ()
  {
  
    if (mp_inst == NULL)
      mp_inst = new oclConnection ( "./src/matrix/ocl/test.cl", CL_DEVICE_TYPE_GPU, true );
      
    return mp_inst;
    
  }
  
  
  
  /**
   * @brief               register kernel argument
   */
  cl_int
  oclConnection ::
  setKernelArg            (int              num,
                           oclDataObject *  p_arg)
  {
 
    // register kernel argument
    m_error = mp_actKernel -> setArg (num, * p_arg -> getBuffer ());
    
    std::cout << " -!-> setKernelArg: " << errorString (m_error) << std::endl;
  
  }
  
  
  
  /**
   * @brief             add oclDataObject to list of existing objects
   */
  void
  oclConnection ::
  addDataObject         (oclDataObject * const ocl_obj)
  {
  
    std::cout << "oclConnection :: addDataObject (...)" << std::endl;

    // insert new data object
    m_current_ocl_objects.insert (std::pair <oclObjectID, oclDataObject * const> (ocl_obj -> getID (), ocl_obj));
    
    // in case of existing buffer, update corresponding lists
    if (ocl_obj -> getMemState ())
    {
      
      std::cout << " *!* existing buffer (in addDObj) *!* " << std::endl;
      
      // list of loaded (on GPU) oclObjects
      m_loaded_ocl_objects.push_back (ocl_obj -> getID ());
      
      // increase reference count for buffer
      std::map <clBuffer *, int> :: iterator tmp_it = m_current_buffers.find (ocl_obj -> getBuffer ());
      
      if (tmp_it == m_current_buffers.end ())
      {
        std::cout << " *!* Caution: Buffer not found *!*" << std::endl;
        throw new int (-1);
      }

      // increase
      tmp_it -> second ++;
      
    }

  }
  
  
  
  /**
   * @brief             remove given object from
   *                        1.  list of existing oclObjects
   *                        2.  list of loaded oclObjects
   *                    delete buffer, if needed
   */
  void
  oclConnection ::
  removeDataObject      (const oclDataObject * const ocl_obj)
  {
    
    std::cout << "oclConnection :: removeDataObject (...)" << std::endl;
    
    // does ocl_obj has a buffer to be handled?
    if (ocl_obj -> getMemState ())
    {
        
      // decrease buffer reference count
      std::map <clBuffer *, int> :: iterator tmp_it =  m_current_buffers.find (ocl_obj -> getBuffer ());
      
      if (tmp_it == m_current_buffers.end ())
      {
        std::cout << " *!* Caution: Buffer not found *!*" << std::endl;
        throw new int (-1);
      }
    
      std::cout << " => handle buffer!!! (ref_count: " << tmp_it -> second << ")" << std::endl;
      
      // delete buffer object in case of no left references
      if (-- tmp_it -> second == 0)
      {
    
        std::cout << " *!* delete buffer *!*" << std::endl;
      
        // remove buffer from list
        m_current_buffers.erase (tmp_it);
        
        // delete buffer object
 //       delete tmp_it -> first;
    
      }
    
      // remove from list of loaded oclObjects
      m_loaded_ocl_objects.remove (ocl_obj -> getID ());
  
    }
    
    // remove given data object
    m_current_ocl_objects.erase (ocl_obj -> getID ());
    
  }
  
  
  // create function object for running an OpenCL kernel
  oclFunctionObject * const
  oclConnection ::
  makeFunctionObject    (const string & algo_name,
                         oclDataObject * const * const args, const int & num_args,
                         const KernelType kernel_type, const SyncType sync_type)
  {
    
    oclFunctionObject * kernel_obj;
   
    if (kernel_type == KERNEL)
    {
    
      if (sync_type == SYNC)
      {
        kernel_obj = new oclKernelObject (algo_name, args, num_args);
      }
      else
      {
  //      kernel_obj = new oclAsyncKernelObject (kernel_name, args, num_args);
      }

    }
    else
    {
    
      if (sync_type == SYNC)
      {
        kernel_obj = new oclViennaClObject (algo_name, args, num_args);
      }
      else
      {
  //      kernel_obj = new oclAsyncVCLObject (algo, args, num_args);
      }    
    
    }

    return kernel_obj;
    
  }
  
  
  /**
   * @brief             create buffer for given object id
   *
   * @param             size      ... in bytes
   */
  template <class T>
  void
  oclConnection ::
  createBuffer          (                T   * const cpu_arg,
                         const    ::size_t           size,
                         const oclObjectID           obj_id)
  {
    
    std::cout << " * oclConnection :: createBuffer (id: " << obj_id << ")" << std::endl;
    
    // find corresponding data object
    std::map <oclObjectID, oclDataObject * const> :: iterator tmp_it = m_current_ocl_objects.find (obj_id);
  
    // check if data object exists
    if (tmp_it == m_current_ocl_objects.end ())
    {
      std::cout << " *!* Caution: oclDataObject (" << obj_id << ") does not exist! *!*" << std::endl;
      throw int (-1);
    }
      
    // retrieve data object
    oclDataObject * const p_tmp_obj = tmp_it -> second;
        
    // check if buffer exists
    if (p_tmp_obj -> getMemState ())
    {
      std::cout << " *!* Caution: oclDataObject (" << obj_id << ") already has a GPU buffer! *!*" << std::endl;
      throw int (-1);
    }

    // create buffer object
    p_tmp_obj -> setBuffer (new clBuffer (m_cont, CL_MEM_READ_WRITE, size, cpu_arg, &m_error));
    std::cout << " -!-> createBuffer: " << errorString (m_error) << std::endl;
      
    // add to buffer list
    m_current_buffers.insert (std::pair <clBuffer *, int> (p_tmp_obj -> getBuffer (), 1));
        
    // add data object to list of loaded objects
    m_loaded_ocl_objects.push_back (obj_id);
      
  }
  
  
  /**
   * @brief             load cpu data to GPU and return corresponding buffer
   *                      - TODO (just copied from setKernelArg (...)
   *
   * @param             size    ... in bytes
   */
  template <class T>
  void
  oclConnection ::
  loadToGPU             (       T   * const cpu_arg,
                         ::size_t           size,
                         clBuffer   * const buffer)
  {
    
    std::cout << " * oclConnection :: loadToGPU" << std::endl;
    
    // loop over command queues, write information to device (global memory)
    for (clCommandQueues::iterator it = m_comqs.begin(); it < m_comqs.end(); ++it)
    {
                   
      try {
        m_error = it -> enqueueWriteBuffer (*buffer, CL_TRUE, 0, size, cpu_arg, NULL, NULL);
      } catch (cl::Error cle) {
        cout << "error code: " << cle.what() << " -> " << cle.err() << " (" << errorString (cle.err()) << ")" << endl;
      }
 
    }
     
  }



# endif // __OCL_CONNECTION_HPP__
