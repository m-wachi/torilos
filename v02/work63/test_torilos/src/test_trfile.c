#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <torilos/trfile.h>

int test_make_abspath();
int test_dirpath_file();

int main(int argc, char** argv) {
	int errcnt = 0;
	

	errcnt += test_make_abspath();
	
	errcnt += test_dirpath_file();
	
	if (errcnt)
		return -1;
		
	puts("OK - test passed.");
	return 0;
}

int test_make_abspath() {
	int errcnt = 0, rc, i;
	char abspath_buf[512], cwdpath[512], filepath[512];
	char *p1;
	
	strcpy(cwdpath, "/dir1");
	strcpy(filepath, "path1");
	rc = tfl_make_abspath(abspath_buf, filepath, cwdpath);
	if (rc) {
		fprintf(stderr, "Error - tfl_make_abspath(): rc=%d\n", rc);
		errcnt++;
	}
	if (strcmp("/dir1/path1", abspath_buf)) {
		fprintf(stderr, "Error - tfl_make_abspath(): abspath_buf=%s\n", abspath_buf);
		errcnt++;
	}

	strcpy(cwdpath, "/dir1");
	strcpy(filepath, "/path1");
	rc = tfl_make_abspath(abspath_buf, filepath, cwdpath);
	if (rc) {
		fprintf(stderr, "Error - tfl_make_abspath(): rc=%d\n", rc);
		errcnt++;
	}
	if (strcmp("/path1", abspath_buf)) {
		fprintf(stderr, "Error - tfl_make_abspath(): abspath_buf=%s\n", abspath_buf);
		errcnt++;
	}

	strcpy(cwdpath, "/dir1");
	strcpy(filepath, "path1/");
	rc = tfl_make_abspath(abspath_buf, filepath, cwdpath);
	if (rc) {
		fprintf(stderr, "Error - tfl_make_abspath(): rc=%d\n", rc);
		errcnt++;
	}
	if (strcmp("/dir1/path1", abspath_buf)) {
		fprintf(stderr, "Error - tfl_make_abspath(): abspath_buf=%s\n", abspath_buf);
		errcnt++;
	}

	strcpy(cwdpath, "/");
	strcpy(filepath, "/");
	rc = tfl_make_abspath(abspath_buf, filepath, cwdpath);
	if (rc) {
		fprintf(stderr, "Error - tfl_make_abspath(): rc=%d\n", rc);
		errcnt++;
	}
	if (strcmp("/", abspath_buf)) {
		fprintf(stderr, "Error - tfl_make_abspath(): abspath_buf=%s\n", abspath_buf);
		errcnt++;
	}

	strcpy(cwdpath, "/");
	strcpy(filepath, "path1");
	rc = tfl_make_abspath(abspath_buf, filepath, cwdpath);
	if (rc) {
		fprintf(stderr, "Error - tfl_make_abspath(): rc=%d\n", rc);
		errcnt++;
	}
	if (strcmp("/path1", abspath_buf)) {
		fprintf(stderr, "Error - tfl_make_abspath(): abspath_buf=%s\n", abspath_buf);
		errcnt++;
	}
	
	strcpy(filepath, "abc/def/ghi");
	p1 = tfl_forward_after_slash(filepath);
	if (p1 != filepath + 4) {
		fprintf(stderr, "Error - tfl_forward_after_slash(): p1 error\n");
		errcnt++;
	}

	strcpy(filepath, "abc/def/ghi");
	p1 = filepath + 8;
	p1 = tfl_backward_prev_slash(p1, filepath);
	if (p1 != filepath + 4) {
		fprintf(stderr, "Error - tfl_backward_prev_slash(): p1 error\n");
		errcnt++;
	}

	
	memset(cwdpath, 0, 8);
	memset(cwdpath, 'Z', 6);
	strcpy(filepath, "abc/ef/ghi");
	rc = tfl_copy_until_slash(cwdpath, filepath);
	if (strcmp("abc/ZZ", cwdpath)) {
		fprintf(stderr, "Error - tfl_copy_until_slash(): cwdpath=%s\n", cwdpath);
		errcnt++;
	}
	if (rc != 4) {
		fprintf(stderr, "Error - tfl_copy_until_slash(): rc=%d\n", rc);
		errcnt++;
	}
	
	strcpy(filepath, "abc/def/../ghi");
	strcpy(cwdpath, filepath);
	tfl_remove_period_exp(filepath);
	if (strcmp("abc/ghi", filepath)) {
		fprintf(stderr, "Error - tfl_remove_period_exp(): before=%s, after=%s\n", cwdpath, filepath);
		errcnt++;
	}

	strcpy(filepath, "abc/def/../ghi/..");
	strcpy(cwdpath, filepath);
	tfl_remove_period_exp(filepath);
	if (strcmp("abc", filepath)) {
		fprintf(stderr, "Error - tfl_remove_period_exp(): before=%s, after=%s\n", cwdpath, filepath);
		errcnt++;
	}

	strcpy(filepath, "abc/def/ghi/");
	strcpy(cwdpath, filepath);
	tfl_remove_period_exp(filepath);
	if (strcmp("abc/def/ghi", filepath)) {
		fprintf(stderr, "Error - tfl_remove_period_exp(): before=%s, after=%s\n", cwdpath, filepath);
		errcnt++;
	}

	strcpy(filepath, "abc/./def/ghi/");
	strcpy(cwdpath, filepath);
	tfl_remove_period_exp(filepath);
	if (strcmp("abc/def/ghi", filepath)) {
		fprintf(stderr, "Error - tfl_remove_period_exp(): before=%s, after=%s\n", cwdpath, filepath);
		errcnt++;
	}
	
	strcpy(filepath, "abc/def/ghi/../../jkl");
	strcpy(cwdpath, filepath);
	tfl_remove_period_exp(filepath);
	if (strcmp("abc/jkl", filepath)) {
		fprintf(stderr, "Error - tfl_remove_period_exp(): before=%s, after=%s\n", cwdpath, filepath);
		errcnt++;
	}

	strcpy(filepath, "abc/..");
	strcpy(cwdpath, filepath);
	tfl_remove_period_exp(filepath);
	if (strlen(filepath)) {
		fprintf(stderr, "Error - tfl_remove_period_exp(): before=%s, after=%s\n", cwdpath, filepath);
		errcnt++;
	}
	
	
	//
	// one period test
	//
	strcpy(cwdpath, "/dir1");
	strcpy(filepath, ".");
	rc = tfl_make_abspath(abspath_buf, filepath, cwdpath);
	if (rc) {
		fprintf(stderr, "Error - tfl_make_abspath(): rc=%d\n", rc);
		errcnt++;
	}
	if (strcmp("/dir1", abspath_buf)) {
		fprintf(stderr, "Error - tfl_make_abspath(): "
				"cwdpath=%s, filepath=%s, "
				"abspath_buf=%s\n", cwdpath, filepath, abspath_buf);
		errcnt++;
	}
	
	//
	// two period test
	//
	strcpy(cwdpath, "/dir1");
	strcpy(filepath, "..");
	rc = tfl_make_abspath(abspath_buf, filepath, cwdpath);
	if (rc) {
		fprintf(stderr, "Error - tfl_make_abspath(): rc=%d\n", rc);
		errcnt++;
	}
	if (strcmp("/", abspath_buf)) {
		fprintf(stderr, "Error - tfl_make_abspath(): "
				"cwdpath=%s, filepath=%s, "
				"abspath_buf=%s\n", cwdpath, filepath, abspath_buf);
		errcnt++;
	}
	
	strcpy(cwdpath, "/");
	strcpy(cwdpath, "/dir1");
	for(i=0; i<257; i++) {
		filepath[i] = 'a';
	}
	filepath[i] = 0;
	
	rc = tfl_make_abspath(abspath_buf, filepath, cwdpath);
	if (rc != -1) {
		fprintf(stderr, "Error - tfl_make_abspath(): rc=%d\n", rc);
		errcnt++;
	}

	for(i=0; i<250; i++) {
		cwdpath[i] = 'a';
	}
	cwdpath[i] = 0;
	strcpy(filepath, "123456");
	
	rc = tfl_make_abspath(abspath_buf, filepath, cwdpath);
	if (rc != -2) {
		fprintf(stderr, "Error - tfl_make_abspath(): rc=%d\n", rc);
		errcnt++;
	}

	return errcnt;
	
}

int test_dirpath_file() {
	int errcnt=0;
	char file[256];
	char dirpath[256];
	char abs_path[256];
	
	strcpy(abs_path, "/abc/def/jkl");
	tfl_dirpath_file(dirpath, file, abs_path);
	if (strcmp(dirpath,"/abc/def") || strcmp(file, "jkl")) {
		fprintf(stderr, "Error - tfl_dirpath_file(): dir_path=%s, file=%s, abs_path=%s\n",
				dirpath, file, abs_path);
		errcnt++;
	}
	
	strcpy(abs_path, "/abd");
	tfl_dirpath_file(dirpath, file, abs_path);
	if (strcmp(dirpath,"/") || strcmp(file, "abd")) {
		fprintf(stderr, "Error - tfl_dirpath_file(): dir_path=%s, file=%s, abs_path=%s\n",
				dirpath, file, abs_path);
		errcnt++;
	}
	
	return errcnt;
}

