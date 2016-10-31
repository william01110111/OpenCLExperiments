
class addClass
{
public:
	void go();
};

void addClass::go()
{
	C[get_global_id(0)]=A[get_global_id(0)]+B[get_global_id(0)];
}

void kernel simple_add(global const int* A, global const int* B, global int* C){
	
	addClass a;
	a.go();
}

