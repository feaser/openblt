/*------------------------------------------------------------/
/ Allocate a contiguous area to the file
/------------------------------------------------------------*/

/* Declarations of FatFs internal functions accessible from applications.
/  This is intended to be used by disk checking/fixing or dirty hacks :-) */
DWORD clust2sect (FATFS *fs, DWORD clst);
DWORD get_fat (FATFS *fs, DWORD clst);
FRESULT put_fat (FATFS *fs, DWORD clst, DWORD val);


DWORD allocate_contiguous_clusters (    /* Returns file start sector number */
    FIL* fp,    /* Pointer to the open file object */
    DWORD len   /* Number of bytes to allocate */
)
{
    DWORD csz, tcl, ncl, ccl, cl;
#if _FATFS != 82786 /* Check if R0.09b */
#error This function may not be compatible with this revision of FatFs module.
#endif

    if (f_lseek(fp, 0) || !len)     /* Check if the given parameters are valid */
        return 0;
    csz = 512UL * fp->fs->csize;    /* Cluster size in unit of byte (assuming 512 bytes/sector) */
    tcl = (len + csz - 1) / csz;    /* Total number of clusters required */
    len = tcl * csz;                /* Round-up file size to the cluster boundary */

    /* Check if the existing cluster chain is contiguous */
    if (len == fp->fsize) {
        ncl = 0; ccl = fp->sclust;
        do {
            cl = get_fat(fp->fs, ccl);  /* Get the cluster status */
            if (cl + 1 < 3) return 0;   /* Hard error? */
            if (cl != ccl + 1 &&; cl < fp->fs->n_fatent) break;  /* Not contiguous? */
            ccl = cl;
        } while (++ncl < tcl);
        if (ncl == tcl)             /* Is the file contiguous? */
            return clust2sect(fp->fs, fp->sclust);  /* Return file start sector */
    }

    if (f_truncate(fp)) return 0;   /* Remove the existing chain */

    /* Find a free contiguous area */
    ccl = cl = 2; ncl = 0;
    do {
        if (cl >= fp->fs->n_fatent) return 0;   /* No contiguous area is found. */
        if (get_fat(fp->fs, cl)) {  /* Encounterd a cluster in use */
            do {    /* Skip the block of used clusters */
                cl++;
                if (cl >= fp->fs->n_fatent) return 0;   /* No contiguous area is found. */
            } while (get_fat(fp->fs, cl));
            ccl = cl; ncl = 0;
        }
        cl++; ncl++;
    } while (ncl < tcl);

    /* Create a contiguous cluster chain */
    fp->fs->last_clust = ccl - 1;
    if (f_lseek(fp, len)) return 0;

    return clust2sect(fp->fs, fp->sclust);  /* Return file start sector */
}


int main (void)
{
    FRESULT fr;
    DRESULT dr;
    FIL fil;
    DWORD org;


    f_mount(0, &Fatfs);

    /* Open or create a file */
    fr = f_open(&fil, "pagefile.dat", FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
    if (fr) return 1;

    /* Check if the file is 64MB in size and occupies a contiguous area.
    /  If not, a contiguous area is re-allocated to the file. */
    org = allocate_contiguous_clusters(&fil, 0x4000000);
    if (!org) {
        printf("Function failed due to any error or insufficient contiguous area.\n");
        f_close(&fil);
        return 1;
    }

    /* Now you can read/write the file with disk functions bypassing the file system layer.
    /  Note that 4th argument of the disk read/write function is BYTE, so that you may need
    /  to bypass the disk functions for large number of multiple sector transfer. */

    dr = disk_write(fil.fs->drv, Buff, org, 128);   /* Write 64K bytes from top of the file */

    ...

    f_close(&fil);
    return 0;
}
