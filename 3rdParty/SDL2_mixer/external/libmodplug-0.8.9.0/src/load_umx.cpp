/*
 * This source code is public domain.
 *
 * Epic Games Unreal UMX container loading for libmodplug
 * Written by O. Sezer <sezero@users.sourceforge.net>
 * UPKG parsing partially based on Unreal Media Ripper (UMR) v0.3
 * by Andy Ward <wardwh@swbell.net>, with additional updates
 * by O. Sezer - see git repo at https://github.com/sezero/umr/
 * Retrieves the offset, size and object type directly from umx.
*/

#include "stdafx.h"
#include "sndfile.h"


typedef LONG fci_t;		/* FCompactIndex */

#define UPKG_HDR_TAG	0x9e2a83c1

struct _genhist {	/* for upkg versions >= 68 */
	LONG export_count;
	LONG name_count;
};

struct upkg_hdr {
	DWORD tag;	/* UPKG_HDR_TAG */
	LONG file_version;
	DWORD pkg_flags;
	LONG name_count;	/* number of names in name table (>= 0) */
	LONG name_offset;		/* offset to name table  (>= 0) */
	LONG export_count;	/* num. exports in export table  (>= 0) */
	LONG export_offset;		/* offset to export table (>= 0) */
	LONG import_count;	/* num. imports in export table  (>= 0) */
	LONG import_offset;		/* offset to import table (>= 0) */

	/* number of GUIDs in heritage table (>= 1) and table's offset:
	 * only with versions < 68. */
	LONG heritage_count;
	LONG heritage_offset;
	/* with versions >= 68:  a GUID, a dword for generation count
	 * and export_count and name_count dwords for each generation: */
	DWORD guid[4];
	LONG generation_count;
#define UPKG_HDR_SIZE 64			/* 64 bytes up until here */
	/*struct _genhist *gen;*/
};

#define UMUSIC_IT	0
#define UMUSIC_S3M	1
#define UMUSIC_XM	2
#define UMUSIC_MOD	3
#define UMUSIC_WAV	4
#define UMUSIC_MP2	5

static const char *mustype[] = {
	"IT", "S3M", "XM", "MOD",
	NULL
};

/* decode an FCompactIndex.
 * original documentation by Tim Sweeney was at
 * http://unreal.epicgames.com/Packages.htm
 * also see Unreal Wiki:
 * http://wiki.beyondunreal.com/Legacy:Package_File_Format/Data_Details
 */
static fci_t get_fci (const char *in, int *pos)
{
	LONG a;
	int size;

	size = 1;
	a = in[0] & 0x3f;

	if (in[0] & 0x40) {
		size++;
		a |= (in[1] & 0x7f) << 6;

		if (in[1] & 0x80) {
			size++;
			a |= (in[2] & 0x7f) << 13;

			if (in[2] & 0x80) {
				size++;
				a |= (in[3] & 0x7f) << 20;

				if (in[3] & 0x80) {
					size++;
					a |= (in[4] & 0x3f) << 27;
				}
			}
		}
	}

	if (in[0] & 0x80)
		a = -a;

	*pos += size;

	return a;
}

static int get_objtype (const BYTE *membase, LONG memlen,
			LONG ofs, int type)
{
	if (type == UMUSIC_IT) {
	_retry:
		if (memcmp(membase + ofs, "IMPM", 4) == 0)
			return UMUSIC_IT;
		return -1;
	}
	if (type == UMUSIC_XM) {
		if (memcmp(membase + ofs, "Extended Module: ", 17) != 0)
			return -1;
		if (*(membase + ofs + 37) != 0x1a) return -1;
		return UMUSIC_XM;
	}

	if (type == UMUSIC_S3M) {
		if (memcmp(membase + ofs + 44, "SCRM", 4) == 0)
			return UMUSIC_S3M;
		/*return -1;*/
		/* SpaceMarines.umx and Starseek.umx from Return to NaPali
		 * report as "s3m" whereas the actual music format is "it" */
		goto _retry;
	}

	if (type == UMUSIC_MOD) {
		membase += ofs + 1080;
		if (memcmp(membase, "M.K.", 4) == 0 || memcmp(membase, "M!K!", 4) == 0)
			return UMUSIC_MOD;
		return -1;
	}

	return -1;
}

static int read_export (const BYTE *membase, LONG memlen,
			const struct upkg_hdr *hdr,
			LONG *ofs, LONG *objsize)
{
	char buf[40];
	int idx = 0, t;

	memcpy(buf, membase + *ofs, 40);

	if (hdr->file_version < 40) idx += 8;	/* 00 00 00 00 00 00 00 00 */
	if (hdr->file_version < 60) idx += 16;	/* 81 00 00 00 00 00 FF FF FF FF FF FF FF FF 00 00 */
	get_fci(&buf[idx], &idx);		/* skip junk */
	t = get_fci(&buf[idx], &idx);		/* type_name */
	if (hdr->file_version > 61) idx += 4;	/* skip export size */
	*objsize = get_fci(&buf[idx], &idx);
	*ofs += idx;	/* offset for real data */

	return t;	/* return type_name index */
}

static int read_typname(const BYTE *membase, LONG memlen,
			const struct upkg_hdr *hdr,
			int idx, char *out)
{
	int i, s;
	long l;
	char buf[64];

	if (idx >= hdr->name_count) return -1;
	buf[63] = '\0';
	for (i = 0, l = 0; i <= idx; i++) {
		memcpy(buf, membase + hdr->name_offset + l, 63);
		if (hdr->file_version >= 64) {
			s = *(signed char *)buf; /* numchars *including* terminator */
			if (s <= 0 || s > 64) return -1;
			l += s + 5;	/* 1 for buf[0], 4 for int32_t name_flags */
		} else {
			l += (long)strlen(buf);
			l +=  5;	/* 1 for terminator, 4 for int32_t name_flags */
		}
	}

	strcpy(out, (hdr->file_version >= 64)? &buf[1] : buf);
	return 0;
}

static int probe_umx   (const BYTE *membase, LONG memlen,
			const struct upkg_hdr *hdr,
			LONG *ofs, LONG *objsize)
{
	int i, idx, t;
	LONG s, pos;
	char buf[64];

	/* Find the offset and size of the first IT, S3M or XM
	 * by parsing the exports table. The umx files should
	 * have only one export. Kran32.umx from Unreal has two,
	 * but both pointing to the same music. */
	s = memlen - hdr->export_offset;
	if (s <= 0) return -1;
	if (s > 64) s = 64;
	memcpy(buf, membase + hdr->export_offset, s);
	for (; s < 64; ++s) buf[s] = 0x0; /* really? */

	idx = 0;

	get_fci(&buf[idx], &idx);	/* skip class_index */
	get_fci(&buf[idx], &idx);	/* skip super_index */
	if (hdr->file_version >= 60) idx += 4; /* skip int32 package_index */
	get_fci(&buf[idx], &idx);	/* skip object_name */
	idx += 4;			/* skip int32 object_flags */

	s = get_fci(&buf[idx], &idx);	/* get serial_size */
	if (s <= 0) return -1;
	pos = get_fci(&buf[idx],&idx);	/* get serial_offset */
	if (pos < 0 || pos > memlen - 40) return -1;

	if ((t = read_export(membase, memlen, hdr, &pos, &s)) < 0) return -1;
	if (s <= 0 || s > memlen - pos) return -1;

	if (read_typname(membase, memlen, hdr, t, buf) < 0) return -1;
	for (i = 0; mustype[i] != NULL; i++) {
		if (!strcasecmp(buf, mustype[i])) {
			t = i;
			break;
		}
	}
	if (mustype[i] == NULL) return -1;
	if ((t = get_objtype(membase, memlen, pos, t)) < 0) return -1;

	*ofs = pos;
	*objsize = s;
	return t;
}

static int probe_header (void *header)
{
	struct upkg_hdr *hdr;
	unsigned char *p;
	DWORD *swp;
	int i;

	/* byte swap the header - all members are 32 bit LE values */
	p = (unsigned char *) header;
	swp = (DWORD *) header;
	for (i = 0; i < UPKG_HDR_SIZE/4; i++, p += 4) {
		swp[i] = p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
	}

	hdr = (struct upkg_hdr *) header;
	if (hdr->tag != UPKG_HDR_TAG) {
		return -1;
	}
	if (hdr->name_count	< 0	||
	    hdr->name_offset	< 0	||
	    hdr->export_count	< 0	||
	    hdr->export_offset	< 0	||
	    hdr->import_count	< 0	||
	    hdr->import_offset	< 0	) {
		return -1;
	}

#if 0
	return 0;
#else
	switch (hdr->file_version) {
	case 35: case 37:	/* Unreal beta - */
	case 40: case 41:				/* 1998 */
	case 61:/* Unreal */
	case 62:/* Unreal Tournament */
	case 63:/* Return to NaPali */
	case 64:/* Unreal Tournament */
	case 66:/* Unreal Tournament */
	case 68:/* Unreal Tournament */
	case 69:/* Tactical Ops */
	case 83:/* Mobile Forces */
		return 0;
	}

	return -1;/* Unknown upkg version for an UMX */
#endif
}

static int process_upkg (const BYTE *membase, LONG memlen,
			 LONG *ofs, LONG *objsize)
{
	char header[UPKG_HDR_SIZE];

	memcpy(header, membase, UPKG_HDR_SIZE);
	if (probe_header(header) < 0)
		return -1;

	return probe_umx(membase, memlen, (struct upkg_hdr *)header, ofs, objsize);
}

BOOL CSoundFile::ReadUMX(const BYTE *lpStream, DWORD dwMemLength)
//---------------------------------------------------------------
{
	int type;
	LONG ofs = 0, size = 0;

	if (!lpStream || dwMemLength < 0x800 || dwMemLength > 0x7fffffff)
		return FALSE;
	type = process_upkg(lpStream, (LONG)dwMemLength, &ofs, &size);
	if (type < 0) return FALSE;

	// Rip Mods from UMX
	switch (type) {
	case UMUSIC_IT:  return ReadIT(lpStream + ofs, size);
	case UMUSIC_S3M: return ReadS3M(lpStream + ofs, size);
	case UMUSIC_XM:  return ReadXM(lpStream + ofs, size);
	case UMUSIC_MOD: return ReadMod(lpStream + ofs, size);
	}
	return FALSE;
}

