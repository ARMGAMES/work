/*
  CRC.H

  See CRC.C for details.

*/

#include <cstddef>

#ifndef VV_CRC_LOADED
#define VV_CRC_LOADED

/*
 * CRC Seed Vaules
 */
#define CRCVALUE UINT32
#define CRCBYTES 4

/*
 * CRC Seed Vaules
 */
#define CRC_SIDF_SEED (CRCVALUE)(-1)		/* 0xFFFFFFFF */
#define CRC_PSWD_SEED (CRCVALUE)(-1)		/* 0xFFFFFFFF */
#define CRC_NORM_SEED (CRCVALUE)(-1)		/* 0xFFFFFFFF */

/*
 * CRC Polynomials
 */
#define CRC_POLY_AUTODIN2 0x04C11DB7L


/*
 * CRC types supported.
 */
enum crc_t {
  CRC_NONE     = 0,  /* Undefined */
  CRC_DMS1     = 1,  /* VytalNet's EVAULT Data Mgmt System CRC */
  CRC_DMS2     = 2,  /* VytalNet's EVAULT Data Mgmt System CRC */
  CRC_AUTODIN2 = 3,  /* Typical CRC used by AUTODIN-II, ethernet, etc. */
  CRC_SIDF     = 4,  /* SIDF - same as AUTODIN-II */
  CRC_ALT_1    = 5  /* Reversed AUTODIN-II */
};


/*
 * The CRC handle.
 */
typedef struct crc {
  CRCVALUE tab[16][256];   /* CRC lookup table */
  CRCVALUE poly;	   /* generator polynomial */
  int reflected;       /* true if the whole bit-stream in a 32-bit word is reversed */
} CRC;


/*
 * Function prototypes
 */
CRC * crc_create(enum crc_t);
CRCVALUE crc_calculate(CRC *, char *, size_t, CRCVALUE);

CRCVALUE crc_byte_calculate(CRC *, char *, size_t, CRCVALUE);

CRC* crc_destroy(CRC *);

#endif	/* __CRC_LOADED */

