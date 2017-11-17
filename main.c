/*include*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


/*main!*/
int main(int argc, char** argv)
{
	char* subPlace = "";
	char* buf;
	char* prevPlace;
	char* writebuf;
	char* writeplace;
	char* bufptr;
	int readLen = 0;
	int totalLen = 0;
	struct stat statusBuffer;
	int status;
	int fileSize;
	int counter = 0;
	int writeSize;

	if (argc != 3)
	{
		printf("Error argc\n");
		return 1;
	}

	char path[514] = { '\0' };  //512 for env; 1 for "/"; 1 for "\0";


	char* dir = getenv("HW1DIR");
	char* tempfile = getenv("HW1TF");


	if (dir == NULL)
	{
		printf("HW1DIR doesnt exist\n");
		return 1;

	}

	if (tempfile == NULL)
	{
		printf("HW1TF doesnt exist\n");
		return 1;
	}

	strcat(path, dir);
	strcat(path, "/");
	strcat(path, tempfile);

	int fd = open(path, O_RDONLY);

	if (fd < 0)
	{
		printf("Error opening file: %s\n", strerror(errno));
		return 1;
	}
	else
	{
		printf("File is opened. Descriptor %d\n", fd);
	}


	//check the size of this file 
	status = fstat(fd, &statusBuffer);
	if (status != 0)
	{
		printf("Error checking file size: %s\n", strerror(errno));
		return 1;
	}

	fileSize = statusBuffer.st_size;

	buf = (char*)malloc((fileSize + 1) * sizeof(char));
	if (!buf)
	{
		close(fd);
		return 1;
	}

	bufptr = buf;


	while ((totalLen < fileSize) && ((readLen = read(fd, bufptr, fileSize)) != 0))
	{
		if (readLen < 0)
		{
			printf("Error reading from file: %s\n", strerror(errno));
			free(buf);
			close(fd);
			return 1;
		}

		bufptr += readLen;
		totalLen += readLen;
	}

	prevPlace = buf;
	buf[fileSize] = '\0'; // string closer
	subPlace = strstr(buf, argv[1]);
	if (subPlace == NULL)
	{
		fwrite(buf, sizeof(char), strlen(buf), stdout);
		free(buf);
		close(fd);
		return 0;
	}

	while (subPlace != NULL)//substring exists in the buffer
	{
		counter++;
		prevPlace = subPlace + strlen(argv[1]);
		subPlace = strstr(prevPlace, argv[1]);
	}


	writeSize = fileSize + (counter*(strlen(argv[2]) - strlen(argv[1])));
	writebuf = (char*)malloc((writeSize + 1) * sizeof(char));
	if (!writebuf)
	{
		free(buf);
		close(fd);
		return 1;
	}
	writebuf[writeSize] = '\0';

	prevPlace = buf;
	writeplace = writebuf;

	subPlace = strstr(buf, argv[1]);

	while (subPlace != NULL)//substring exists in the buffer - aproved before
	{
		strncpy(writeplace, prevPlace, subPlace - prevPlace);
		writeplace = writeplace + (subPlace - prevPlace);
		strcpy(writeplace, argv[2]);
		writeplace += strlen(argv[2]);
		prevPlace = subPlace + strlen(argv[1]);

		subPlace = strstr(prevPlace, argv[1]);
	}

	strncpy(writeplace, prevPlace, strlen(prevPlace));
	fwrite(writebuf, sizeof(char), strlen(writebuf), stdout);

	free(writebuf);
	free(buf);
	close(fd);
	return 0;

}
