#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){
	char command[15];
	char numb[7];
	double start = 1;
	double end = 1000;
	for(int i = start; i <= end; i++){
		printf("%.2lf%%\n", ((i - start)/(end - start + 1)) * 100);
		strcpy(command, "./a ");
		sprintf(numb, "%d", i);
		strcat(command, numb);
		FILE *fp = popen(command, "r");
		if(fp != NULL){
			FILE *f = fopen("MemoryZone", "a");
			if(f != NULL){
				fprintf(f, "PID%d:\n", i);
				char path[1024];
				while (fgets(path, sizeof(path)-1, fp) != NULL) {
					fprintf(f, "\t%s", path);
				}
			}
			fprintf(f, "\n");
			fclose(f);
		}
		fclose(fp);
	}
	printf("Completed!\n");
	return 0;
}
