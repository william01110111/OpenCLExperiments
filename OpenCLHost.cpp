
#include "OpenCLHost.h"

OpenCLHost::OpenCLHost()
{
	
}

void OpenCLHost::getReady()
{
	//get all platforms (drivers)
    std::vector<cl::Platform> all_platforms;
    cl::Platform::get(&all_platforms);
    if(all_platforms.size()==0){
        std::cout<<" No platforms found. Check OpenCL installation!\n";
        exit(1);
    }
    
    cout << "Platforms found:" << endl;
    
    for (unsigned i=0; i<all_platforms.size(); ++i)
	{
		cout << "	" << i << ". " << all_platforms[i].getInfo<CL_PLATFORM_NAME>() << endl;
	}
	
	cout << endl;
    
    cl::Platform default_platform=all_platforms[0];
    
    cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << endl;
 
	cout << endl;
	
    //get default device of the default platform
    std::vector<cl::Device> all_devices;
    default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
    if(all_devices.size()==0){
        std::cout<<" No devices found. Check OpenCL installation!\n";
        exit(1);
    }
    
    cout << "Devices:" << endl;
    
    for (unsigned i=0; i<all_devices.size(); ++i)
	{
		cout << "	" << i << ". " << all_devices[i].getInfo<CL_DEVICE_NAME>() << endl;
	}
    
    cout << endl;
    
    device=all_devices[0];
    std::cout<< "Using device: " << device.getInfo<CL_DEVICE_NAME>() << endl;
    
    cout << endl;
    
    cout << "creting context..." << endl;
    
    context=cl::Context({device});
    
    cout << "context created" << endl;
 
}

void OpenCLHost::loadKernel(string kernelFile)
{
	// kernel calculates for each element C=A+B
    string kernel_code;
    
    std::ifstream ifs(kernelFile);
    
    if (!ifs.is_open())
    {
        cout << "kernel file error" << endl;
        exit(1);
    }
    
    kernel_code.assign( (std::istreambuf_iterator<char>(ifs) ),
                (std::istreambuf_iterator<char>()));
    
    cout << "kernel code:" << endl << kernel_code << endl;
    
    sources.push_back({kernel_code.c_str(),kernel_code.length()});
}

void OpenCLHost::buildAndRun()
{
	
    cl::Program program(context,sources);
    
    cout << "starting build..." << endl;
    
    if(program.build({device})!=CL_SUCCESS){
        std::cout<<" Error building: "<<program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device)<<"\n";
        exit(1);
    }
    
    cout << "build succeeded!" << endl;
    
    const int dataSize=100;
    
    // create buffers on the device
    
    cl::Buffer buffer_A(context,CL_MEM_READ_WRITE,sizeof(int)*dataSize);
    cl::Buffer buffer_B(context,CL_MEM_READ_WRITE,sizeof(int)*dataSize);
    cl::Buffer buffer_C(context,CL_MEM_READ_WRITE,sizeof(int)*dataSize);
 
    //int A[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    //int B[] = {0, 1, 2, 0, 1, 2, 0, 1, 2, 0};
    
    int A[dataSize], B[dataSize];
    
    for (int i=0; i<dataSize; ++i)
    {
        A[i]=rand();
        B[i]=rand();
    }
    
    //create queue to which we will push commands for the device.
    
    cl::CommandQueue queue(context,device);
 
    //write arrays A and B to the device
    queue.enqueueWriteBuffer(buffer_A,CL_TRUE,0,sizeof(int)*dataSize,A);
    queue.enqueueWriteBuffer(buffer_B,CL_TRUE,0,sizeof(int)*dataSize,B);
	
    //run the kernel
    //cl::KernelFunctor simple_add(cl::Kernel(program,"simple_add"),queue,cl::NullRange,cl::NDRange(10),cl::NullRange);
    //simple_add(buffer_A,buffer_B,buffer_C);
    //alternative way to run the kernel
    
    cl::Kernel kernel_add=cl::Kernel(program,"simple_add");
    kernel_add.setArg(0,buffer_A);
    kernel_add.setArg(1,buffer_B);
    kernel_add.setArg(2,buffer_C);
    queue.enqueueNDRangeKernel(kernel_add,cl::NullRange,cl::NDRange(dataSize),cl::NullRange);
    queue.finish();
 
    int C[dataSize];
    //read result C from the device to array C
    queue.enqueueReadBuffer(buffer_C,CL_TRUE,0,sizeof(int)*dataSize,C);
 
    std::cout<<" result: \n";
    
    for(int i=0;i<dataSize;i++){
        std::cout<<C[i]<<" ";
    }
}

