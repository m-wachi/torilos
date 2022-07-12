#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <torilos/trblkbuf.h>

int main(int argc, char** argv) {
	int i, rc, total, errcnt=0;
	char* buff;

	TBLBF_BLKCELL* blkcell1;
	TBLBF_BLKCELL* blkcell2;
	
	puts("trblkbuf test.");
	
	
	buff = malloc(4096 * 2);
	
	rc = tblbf_init(buff, 2, 1024);
	if (rc != 0) {
		fprintf(stderr, "Error - tblbf_init: rc=%d\n", rc );
		errcnt++;
	}
	

	total = tblbf_get_blkcells_count();
	
	rc = tblbf_get_freeblk_count();
	
	if (total != 8 || rc != 7) {
		fprintf(stderr, "Error - initialized count: blk total=%d, free=%d\n", total, rc);
		errcnt++;
	}
	
	blkcell1 = tblbf_alc_free_blkcell();
	blkcell2 = tblbf_alc_free_blkcell();
	
	rc = tblbf_get_freeblk_count();
	
	if (rc != 5) {
		fprintf(stderr, "Error - tblbf_alc_free_blkcell(): blk total=%d, free=%d\n", total, rc);
		errcnt++;
	}
	

	tblbf_free_blkcell(blkcell1);
	tblbf_free_blkcell(blkcell2);
	
	rc = tblbf_get_freeblk_count();
	
	if (rc!=7) {
		fprintf(stderr, "Error - tblbf_free_blkcell(): blk total=%d, free=%d\n", total, rc);
		errcnt++;
	}
	
	blkcell1 = tblbf_alc_free_blkcell();
	blkcell2 = tblbf_alc_free_blkcell();
	tblbf_free_blkcell(blkcell1);
	tblbf_free_blkcell(blkcell1);
	
	rc = tblbf_get_freeblk_count();
	if (rc!=6) {
		fprintf(stderr, "Error - tblbf_free_blkcell() - same block freed twice.\n\t: blk total=%d, free=%d\n", total, rc);
		errcnt++;
	}
	
	if (errcnt)
		exit(1);
	
	puts("OK - test passed.");
	exit(0);
	
	
}

