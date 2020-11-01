/**********
Copyright (c) 2019, Xilinx, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********/
#include "xcl2.hpp"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;


#define CELL_SIZE 525
#define FACE_SIZE 3756


int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
    return EXIT_FAILURE;
  }

  std::string binaryFile = argv[1];

  int nCells = CELL_SIZE;
  int nFaces = FACE_SIZE;
  cl_int err;
  cl::Kernel krnl_myAmul;
  cl::CommandQueue q;
  cl::Context context;
  // Allocate Memory in Host Memory
	size_t cell_vector_size_bytes = sizeof(double) * CELL_SIZE;
	size_t double_face_vector_size_bytes = sizeof(double) * FACE_SIZE;
	size_t int_face_vector_size_bytes = sizeof(int) * FACE_SIZE;

  std::vector<double, aligned_allocator<double>> diagPtr(CELL_SIZE);
  std::vector<double, aligned_allocator<double>> psiPtr(CELL_SIZE);
  std::vector<double, aligned_allocator<double>> lowerPtr(FACE_SIZE);
  std::vector<double, aligned_allocator<double>> upperPtr(FACE_SIZE);
  std::vector<int, aligned_allocator<int>> uPtr(FACE_SIZE);
  std::vector<int, aligned_allocator<int>> lPtr(FACE_SIZE);

  std::vector<double, aligned_allocator<double>> ApsiPtr(CELL_SIZE);
  std::vector<double, aligned_allocator<double>> ApsiPtr_sw_results(CELL_SIZE);


	string line;
	ifstream myfile("/mnt/data/workspace/thlaberidis/Data_myResidual.txt");
	int counter=0;

	bool apsi_ready=false;
	bool diag_ready=false;
	bool psi_ready=false;
	bool upper_ready=false;
	bool lower_ready=false;
	bool u_ready=false;
	bool l_ready=false;



	if(myfile.is_open())
	{

		while(getline(myfile,line))
		{

			if(diag_ready && counter<nCells){
				diagPtr[counter] = std::stod(line);
				counter++;
			}
			if(apsi_ready && counter<nCells){
				ApsiPtr_sw_results[counter] = std::stod(line);
				counter++;
			}
			if(psi_ready && counter<nCells){
				psiPtr[counter] = std::stod(line);
				counter++;
			}
			if(upper_ready && counter<nFaces){
				upperPtr[counter] = std::stod(line);
				counter++;
			}
			if(lower_ready && counter<nFaces){
				lowerPtr[counter] = std::stod(line);
				counter++;
			}
			if(u_ready && counter<nFaces){
				uPtr[counter] = std::stod(line);
				counter++;
			}
			if(l_ready && counter<nFaces){
				lPtr[counter] = std::stod(line);
				counter++;
			}

			if(line=="upperPtr:"){
				apsi_ready = false;
				diag_ready = false;
				psi_ready = false;
				upper_ready = true;
				lower_ready = false;
				u_ready = false;
				l_ready = false;
				counter = 0;
			}
			if(line=="psiPtr:"){
				apsi_ready = false;
				diag_ready = false;
				psi_ready = true;
				upper_ready = false;
				lower_ready = false;
				u_ready = false;
				l_ready = false;
				counter = 0;
			}
			if(line=="diagPtr:"){
				apsi_ready = false;
				diag_ready = true;
				psi_ready = false;
				upper_ready = false;
				lower_ready = false;
				u_ready = false;
				l_ready = false;
				counter = 0;
			}
			if(line=="ApsiPtr:"){
				apsi_ready = true;
				diag_ready = false;
				psi_ready = false;
				upper_ready = false;
				lower_ready = false;
				u_ready = false;
				l_ready = false;
				counter = 0;
			}
			if(line=="lowerPtr:"){
				apsi_ready = false;
				diag_ready = false;
				psi_ready = false;
				upper_ready = false;
				lower_ready = true;
				u_ready = false;
				l_ready = false;
				counter = 0;
			}
			if(line=="uPtr:"){
				apsi_ready = false;
				diag_ready = false;
				psi_ready = false;
				upper_ready = false;
				lower_ready = false;
				u_ready = true;
				l_ready = false;
				counter = 0;
			}
			if(line=="lPtr:"){
				apsi_ready = false;
				diag_ready = false;
				psi_ready = false;
				upper_ready = false;
				lower_ready = false;
				u_ready = false;
				l_ready = true;
				counter = 0;
			}


		}
		myfile.close();
	}
	else cout<<"unable to open file";



  // Create the test data and Software Result

	for (int cell=0; cell<nCells; cell++)
	{
		ApsiPtr_sw_results[cell]= diagPtr[cell]*psiPtr[cell];
	}

	for (int face=0; face<nFaces; face++)
	{
		ApsiPtr_sw_results[lPtr[face]] += upperPtr[face]*psiPtr[uPtr[face]];
		ApsiPtr_sw_results[uPtr[face]] += lowerPtr[face]*psiPtr[lPtr[face]];
	}


  // OPENCL HOST CODE AREA START
  auto devices = xcl::get_xil_devices();
  // read_binary_file() is a utility API which will load the binaryFile
  // and will return the pointer to file buffer.
  auto fileBuf = xcl::read_binary_file(binaryFile);
  cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
  int valid_device = 0;
  for (unsigned int i = 0; i < devices.size(); i++) {
    auto device = devices[i];
    // Creating Context and Command Queue for selected Device
    OCL_CHECK(err, context = cl::Context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(err, q = cl::CommandQueue(context, device,
                                        CL_QUEUE_PROFILING_ENABLE, &err));

    std::cout << "Trying to program device[" << i
              << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
    cl::Program program(context, {device}, bins, NULL, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
    } else {
      std::cout << "Device[" << i << "]: program successful!\n";
      OCL_CHECK(err, krnl_myAmul = cl::Kernel(program, "myAmul", &err));
      valid_device++;
      break; // we break because we found a valid device
    }
  }
  if (valid_device == 0) {
    std::cout << "Failed to program any device found, exit!\n";
    exit(EXIT_FAILURE);
  }

  // Allocate Buffer in Global Memory
  OCL_CHECK(err, cl::Buffer buffer_ApsiPtr(
                     context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                     cell_vector_size_bytes, ApsiPtr.data(), &err));

  OCL_CHECK(err, cl::Buffer buffer_diagPtr(
                     context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                     cell_vector_size_bytes, diagPtr.data(), &err));

  OCL_CHECK(err, cl::Buffer buffer_psiPtr(
                     context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                     cell_vector_size_bytes, psiPtr.data(), &err));

	OCL_CHECK(err, cl::Buffer buffer_lowerPtr(
                     context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                     double_face_vector_size_bytes, lowerPtr.data(), &err));

	OCL_CHECK(err, cl::Buffer buffer_upperPtr(
                     context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                     double_face_vector_size_bytes, upperPtr.data(), &err));

	OCL_CHECK(err, cl::Buffer buffer_uPtr(
                     context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                     int_face_vector_size_bytes, uPtr.data(), &err));

	OCL_CHECK(err, cl::Buffer buffer_lPtr(
                     context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                     int_face_vector_size_bytes, lPtr.data(), &err));



  OCL_CHECK(err, err = krnl_myAmul.setArg(0, nCells));
  OCL_CHECK(err, err = krnl_myAmul.setArg(1, nFaces));
  OCL_CHECK(err, err = krnl_myAmul.setArg(2, buffer_ApsiPtr));
  OCL_CHECK(err, err = krnl_myAmul.setArg(3, buffer_diagPtr));
  OCL_CHECK(err, err = krnl_myAmul.setArg(4, buffer_psiPtr));
  OCL_CHECK(err, err = krnl_myAmul.setArg(5, buffer_lowerPtr));
  OCL_CHECK(err, err = krnl_myAmul.setArg(6, buffer_upperPtr));
  OCL_CHECK(err, err = krnl_myAmul.setArg(7, buffer_uPtr));
  OCL_CHECK(err, err = krnl_myAmul.setArg(8, buffer_lPtr));




  // Copy input data to device global memory
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_ApsiPtr},
                                                  0 /* 0 means from host*/));
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_diagPtr},
                                                    0 /* 0 means from host*/));
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_psiPtr},
                                                    0 /* 0 means from host*/));
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_lowerPtr},
                                                    0 /* 0 means from host*/));
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_upperPtr},
                                                    0 /* 0 means from host*/));
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_uPtr},
                                                    0 /* 0 means from host*/));
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_lPtr},
                                                    0 /* 0 means from host*/));
  // Launch the Kernel
  OCL_CHECK(err, err = q.enqueueTask(krnl_myAmul));

  // Copy Result from Device Global Memory to Host Local Memory
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_ApsiPtr},
                                                  CL_MIGRATE_MEM_OBJECT_HOST));
/*  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_diagPtr},
                                                  CL_MIGRATE_MEM_OBJECT_HOST));
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_psiPtr},
                                                  CL_MIGRATE_MEM_OBJECT_HOST));
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_lowerPtr},
                                                  CL_MIGRATE_MEM_OBJECT_HOST));
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_upperPtr},
                                                  CL_MIGRATE_MEM_OBJECT_HOST));
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_uPtr},
                                                  CL_MIGRATE_MEM_OBJECT_HOST));
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_lPtr},
                                                  CL_MIGRATE_MEM_OBJECT_HOST));
*/  OCL_CHECK(err, err = q.finish());

  // OPENCL HOST CODE AREA END

  // Compare the results of the Device to the simulation
  int match = 0;
  for (int i = 0; i < nCells; i++) {
    if (ApsiPtr[i] != ApsiPtr_sw_results[i]) {
      std::cout << "Error: Result mismatch" << std::endl;
      //std::cout << "i = " << i << " CPU result = " << ApsiPtr_sw_results[i]
      //          << " Device result = " << ApsiPtr[i] << std::endl;
      match = 1;
      break;
    } else {
      //std::cout << ApsiPtr[i] << " ";
      //if (((i + 1) % 16) == 0)
      //  std::cout << std::endl;
    }
  }

  std::cout << "TEST " << (match ? "FAILED" : "PASSED") << std::endl;

  return (match ? EXIT_FAILURE : EXIT_SUCCESS); //match ? EXIT_FAILURE : EXIT_SUCCESS
}
