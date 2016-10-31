
#include <iostream>
#include <fstream>
#include <algorithm>
#include <CL/cl.hpp>

using std::endl;
using std::cout;
using std::string;

class OpenCLHost
{
public:
	OpenCLHost();
	
	void getReady();
	
	void loadKernel(string kernelFile);
	
	void buildAndRun();
	
private:
	
	bool isReady=false;
	
	cl::Device device;
	cl::Context context;
	cl::Program::Sources sources;
};

