/**
 * Projekt IPK-proj2
 * Hlavicka DNS dotazu
 * @brief Hlavicka DNS dotazu
 * @Author Matej Mitas, xmitas02
 * @file ipk-header.c
 */

#include "ipk-header.h"

// vytvor struct pro parametry
ipk18_dns_Header_opt * ipk18_dns_Header_init_opt() {
	// naalokujeme novy struct pro priznaky
	ipk18_dns_Header_opt *opt = (ipk18_dns_Header_opt *) malloc(sizeof(ipk18_dns_Header_opt));
	if (opt == NULL)
		return NULL;
	return opt;
}

// nacti parametry
void ipk18_dns_Header_read_opt(ipk18_dns_Header *header, ipk18_dns_Header_opt *opt) {
	// masky pro jednotlive priznaky
	uint16_t mask_opcode = 0x7800;
	uint16_t mask_one_bit = 0x400;
	uint16_t mask_z = 0x70;
	uint16_t mask_rcode = 0xF;
	// je nutno preves na spravne poradi (klient)
	uint16_t opts = ntohs(header->opt);
	// vymaskujeme jednotlive hodnoty
	opt->qr = opts >> 15;
	opt->opcode = (opts & mask_opcode) >> 11;
	opt->aa = (opts & mask_one_bit) >> 10;
	opt->tc = (opts & (mask_one_bit >> 1)) >> 9;
	opt->rd = (opts & (mask_one_bit >> 2)) >> 8;
	opt->ra = (opts & (mask_one_bit >> 3)) >> 7;
	opt->z = (opts & mask_z) >> 4;
	opt->rcode = opts & mask_rcode;
}

int ipk18_dns_Header_asses_opt(ipk18_dns_Header_opt *opt) {
	// typ zpravy
	if (opt->qr == 0)
		return ipk18_error_msg("Server neposlal odpoved, ale otazku (QR: 0)");
	// vlastnosti zpravy
	if (opt->opcode != 0)
		return ipk18_error_msg("Standartni otazka, ale jinak odpoved (Opcode: !0)");
	// vlastnosti zpravy
	if (opt->tc == 1)
		return ipk18_error_msg("Server poslal odpoved v poradku, ale je moc dlouha (TC: 1)");
	// precteme navratovou hodnotu
	switch (opt->rcode) {
		case 1:
			return ipk18_error_msg("FormErr (Rcode: 1)");
			break;
		case 2:
			return ipk18_error_msg("ServFail (Rcode: 2)");
			break;
		case 3:
			return ipk18_error_msg("Jmeno neexistuje - NXDomain (Rcode: 3)");
			break;
		case 4: 
			return ipk18_error_msg("NotImp (Rcode: 4)");
			break;
		case 5:
			return ipk18_error_msg("Refused (Rcode: 5)");
			break;
	}
	// jinak vratime ok
	return PRG_OK;
}

// uvolnime pamet
void ipk18_dns_Header_del_opt(ipk18_dns_Header_opt *opt) {
	free(opt);
}
