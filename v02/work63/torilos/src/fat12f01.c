#include <torilos/fat12f01.h>
#include <torilos/mymm01.h>
#include <torilos/debug01.h>
#include <torilos/myutil01.h>
#include <torilos/mylib01.h>
#include <torilos/trutil.h>

/*
 * extract fat12 all 
 */
void fat12_extr_fat12(unsigned short* fat12_ext, unsigned char* fat12) {
	int ind_raw = 0;
	unsigned short high, low;
	unsigned char *p_raw;
	unsigned short *p_ext;

	p_raw = fat12;
	p_ext = fat12_ext;
	while(ind_raw < 9*512) {
		fat12_extr_fat12_3b(&high, &low, p_raw);
		*p_ext++ = high;
		*p_ext++ = low;
		p_raw += 3;
		ind_raw += 3;
	}

}

void fat12f_open_file(HND_FAT12F_FILE* hnd_f, 
					  FAT12_DIRENT* fat12_dirent, 
					  unsigned short* fat12_ext) {
	
	int i=0;
	unsigned int restsize;
	FDC_SENSEINT_RESULT fdc_sint_res;
	FDC_READDATA_RESULT fdc_rd_res;
	int cur_cyl=-1, cur_hd=-1;
	unsigned char* destbuf;
	unsigned int srcaddr;

	fat12f_setup_hndl(hnd_f, fat12_dirent->clstr_no, fat12_ext);

	hnd_f->buff = (char*)(destbuf 
		= (unsigned char*) mm_malloc_pg_knl(fat12_dirent->size+1));

	hnd_f->buff[fat12_dirent->size] = '\0';

	restsize = fat12_dirent->size;

	while (i < hnd_f->clstr_cnt) {
		if (cur_cyl != hnd_f->chs[i].cyl) {
			/* seek cylinder */
			fdc_send_seek(hnd_f->chs[i].cyl);
			fancy_wait_fdcint();

			/* sense interrupt */
			fdc_sense_int(&fdc_sint_res);

			debug_varval01("fd-sr0=", fdc_sint_res.sr0, 1);
			debug_varval01("fd-cyl=", fdc_sint_res.cyl_no, 1);

			cur_cyl = hnd_f->chs[i].cyl;
			cur_hd = -1;
		}
		
		if (cur_hd != hnd_f->chs[i].hd) {
		
			debug_puts("start READ DATA");

			/* send READ DATA and receive result */
			fdc_readtrack(&fdc_rd_res, hnd_f->chs[i].cyl, hnd_f->chs[i].hd);

			cur_hd = hnd_f->chs[i].hd;
			debug_puts("end reading");
		}
		srcaddr = (hnd_f->chs[i].sec-1) * 512;
		if (restsize >= 512) {
			my_memcpy(destbuf, (void*)srcaddr, 512);
			restsize -= 512;
			destbuf += 512;
		}
		else {
			my_memcpy(destbuf, (void*)srcaddr, restsize);
			restsize = 0;
		}
		i++;
	}
}

void fat12f_setup_hndl(HND_FAT12F_FILE* hnd_f, 
					   unsigned short start_clstr_no, 
					   unsigned short* fat12_ext) {
	
	int cnt = 0;
	unsigned short next_clstr_no;


	hnd_f->clstr_no[0] = start_clstr_no;
	clstr2chs(&(hnd_f->chs[0]), start_clstr_no);
	next_clstr_no = fat12_ext[start_clstr_no];
	cnt++;
	
	while(0xff0 > next_clstr_no) {
		hnd_f->clstr_no[cnt] = next_clstr_no;
		clstr2chs(&(hnd_f->chs[cnt]), next_clstr_no);
		next_clstr_no = fat12_ext[next_clstr_no];
		cnt++;
	}
	hnd_f->clstr_cnt = cnt;
	
}

//void fat12f_freefile(HND_FAT12F_FILE* hnd_f, PG_MNGR* pg_mngr) {
void fat12f_freefile(HND_FAT12F_FILE* hnd_f) {
	
	//mm_free(pg_mngr, (unsigned int)hnd_f->buff);
	mm_free((unsigned int)hnd_f->buff);
}

void print_hnd_fat12f_file(HND_FAT12F_FILE* hnd_f) {
	int i;
	char buff[256], buff2[256];

	debug_varval01("hnd->cnt", hnd_f->clstr_cnt, 0);
	for(i=0; i<hnd_f->clstr_cnt; i++) {
		sprint_varval(buff, "clstr", hnd_f->clstr_no[i], 1);
		sprint_varval(buff2, ", cyl", hnd_f->chs[i].cyl, 0);
		my_strcat(buff, buff2);
		sprint_varval(buff2, ", hd", hnd_f->chs[i].hd, 0);
		my_strcat(buff, buff2);
		sprint_varval(buff2, ", sec", hnd_f->chs[i].sec, 0);
		my_strcat(buff, buff2);
		debug_puts(buff);
	}
}

void clstr2chs(CYL_HD_SEC* chs, int clstr_no) {
	int addr;
	int blk;
	int wk;

	/* calc address */
	addr = 0x3e00 + (0x200 * clstr_no);
	
	blk = addr / 0x200;
	
	wk = blk / 18;

	/* cylinder no */
	chs->cyl = wk / 2;

	/* head address */
	chs->hd = wk % 2 ? 1 : 0;

	/* sector no */
	chs->sec = blk % 18 + 1;

}
