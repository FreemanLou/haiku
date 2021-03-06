/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 * Test that shm_open does not open the shared memory object for writing when
 * calling shm_open with O_RDONLY even if the mode set write permission.
 *
 * The test use ftruncate to check the object is not open for writing.
 */

/* ftruncate was formerly an XOPEN extension. We define _XOPEN_SOURCE here to
   avoid warning if the implementation does not program ftruncate as a base 
   interface */
#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "posixtest.h"

#define BUF_SIZE 8
#define SHM_NAME "posixtest_20-1"

int main() {
	int fd, result;
	
	result = shm_unlink(SHM_NAME);
	if(result != 0 && errno != ENOENT) { 
		/* The shared memory object exist and shm_unlink can not 
		   remove it. */
		perror("An error occurs when calling shm_unlink()");
		return PTS_UNRESOLVED;
	}

	fd = shm_open(SHM_NAME, O_RDONLY|O_CREAT, S_IRUSR);
	if(fd == -1) {
		perror("An error occurs when calling shm_open()");
		return PTS_UNRESOLVED;
	}

	result = ftruncate(fd, BUF_SIZE);

	if(result == -1 && errno == EINVAL) {
		printf("Test PASSED\n");
		shm_unlink(SHM_NAME);
		return PTS_PASS;
	} else if(result == 0){
		printf("The shared memory object is opened for writing.\n");
		shm_unlink(SHM_NAME);
		return PTS_FAIL;
	}

	perror("ftruncate");
	shm_unlink(SHM_NAME);
	return PTS_FAIL;
}
