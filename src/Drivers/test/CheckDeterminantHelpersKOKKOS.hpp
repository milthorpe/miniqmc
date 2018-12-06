////////////////////////////////////////////////////////////////////////////////
// This file is distributed under the University of Illinois/NCSA Open Source
// License.  See LICENSE file in top directory for details.
//
// Copyright (c) 2018 QMCPACK developers.
//
// File developed by:
// Peter Doak, doakpw@ornl.gov, Oak Ridge National Lab
//
// File created by:
// Peter Doak, doakpw@ornl.gov, Oak Ridge National Lab
////////////////////////////////////////////////////////////////////////////////
#ifndef QMCPLUSPLUS_CHECK_DETERMINANT_HELPERS_KOKKOS_HPP
#define QMCPLUSPLUS_CHECK_DETERMINANT_HELPERS_KOKKOS_HPP

#include "Drivers/check_determinant.h"

namespace qmcplusplus
{

template<>
void CheckDeterminantHelpers<Devices::KOKKOS>::finalize()
{
  Kokkos::finalize();
}

template<>
void CheckDeterminantHelpers<Devices::KOKKOS>::initialize(int argc, char** argv)
{
  std::cout << "CheckDeterminantHelpers<DDT::KOKKOS>::initialize" << '\n';
  Kokkos::initialize(argc, argv);
}

template<>
double CheckDeterminantHelpers<Devices::KOKKOS>::runThreads(int np,
							      PrimeNumberSet<uint32_t>& myPrimes,
							      ParticleSet& ions,
							      int& nsteps,
							      int& nsubsteps)
{
  auto main_function = KOKKOS_LAMBDA (int thread_id, double& accumulated_error)
  {
    printf(" thread_id = %d\n",thread_id);
    CheckDeterminantHelpers<Devices::KOKKOS>
    ::thread_main(thread_id, myPrimes, ions, nsteps, nsubsteps, accumulated_error);
  };
  double accumulated_error = 0.0;

#if defined(KOKKOS_ENABLE_OPENMP) && !defined(KOKKOS_ENABLE_CUDA)
  // The kokkos check_determinant was never threaded
  // could be with
  // Kokkos::OpenMP::thread_pool_size();
  int num_threads = 1; 
  int ncrews = 1;   
  int crewsize = std::max(1,num_threads/ncrews); 
  printf(" In partition master with %d threads, %d crews.  Crewsize = %d \n",num_threads,ncrews,crewsize);
  Kokkos::parallel_reduce(crewsize, main_function, accumulated_error);
  //Kokkos::OpenMP::partition_master(main_function,nmovers,crewsize);
#else
  main_function(0, accumulated_error );
#endif  
  return accumulated_error;
}

  template class CheckDeterminantHelpers<Devices::KOKKOS>;
}

#endif