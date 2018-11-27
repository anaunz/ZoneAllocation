#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

long zoneCalculate(char *addrStart, char *addrEnd, char *pid) {
	char command[63] = "sudo ./pagemap ";
	int dma = 0;
	int dma32 = 0;
	int norm = 0;
	int fault = 0;
	strcat(command, pid);
	strcat(command, " 0x");
	strcat(command, addrStart);
	strcat(command, " 0x");
	strcat(command, addrEnd);
	FILE *fp = popen(command, "r");
	if(fp != NULL){
		char path[1024];
		while (fgets(path, sizeof(path)-1, fp) != NULL) {
			//printf("%s", path);
			if(path[0] == '1'){
				unsigned long long pfn = -1;
				sscanf(path + 2, "%llx", &pfn);
				if(pfn * 4096 < 16777215) dma++;
				else if(pfn * 4096 < 4294967295) dma32++;
				else norm++;
			}
			else fault++;
		}
	}
	else printf("Command Error: %s\n", command);
	pclose(fp);
	//printf("function: %d %d %d %d\n", dma, dma32, norm, fault);
	long zone = dma*100000000 + dma32*10000 + norm;
	return zone;
}


int main(int argc, char *argv[]) {
	char pid[7];
	char path[63] = "/proc/";

	if(argc == 1){
		printf("Enter PID(%d): ", getpid());
		fgets(pid, 7, stdin);
	}
	else if(argc == 2) strcpy(pid, argv[1]);
	else return -1;

	for(int i = 0; i < 7; i++) if(pid[i] == '\n') pid[i] = '\0';
	strcat(path, pid);
	strcat(path, "/maps");

	FILE *fp = fopen(path, "r");
	if(fp != NULL){
		char addressStart[4095][20];
		char addressEnd[4095][20];
		int k = 0;
		while(!feof(fp)){
			char temp[511];
			fgets(temp, 511, fp);
			for(int i = 0; i < 30; i++) if(temp[i] == '-') temp[i] = ' ';
			sscanf(temp, "%s %s", addressStart[k], addressEnd[k]);
			//printf("%s - %s\n", addressStart[k], addressEnd[k]);
			k++;
		}
		int dma, dma32, norm = 0;
		for(int i = 0; i < k - 2; i++){
			long temp = zoneCalculate(addressStart[i], addressEnd[i], pid);
			//printf("%lu\n", temp);
			dma += (temp % 10000000000) / 1000000000;
			dma32 += (temp % 1000000000) / 10000;
			norm += temp % 10000;
		}
		printf("------------------\nDMA: %d KiB\nDMA32: %d KiB\nNormal: %d KiB\n------------------\nTotal %d KiB\n------------------\n", dma * 4, dma32 * 4, norm * 4, dma * 4 + dma32 * 4 + norm * 4);
		fclose(fp);
		return 0;
	}
	else{
		printf("PID doesn't exist\n");
		return -1;
	}
}
