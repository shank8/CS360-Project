#include "headers/type.h"



int main(int argc, char * argv[], char * env[])
{
	// DEVICE SELECT
	get_device();

	// INITIALIZE 
	init();
	
	// MOUNT ROOT
	mount_root();

	// PROCESS LOOP
	while(1)
	{
		break;
	}
	
	quit();
	return 0;
}

