/***************************************************************************
 *   Copyright (C) 2005 by Ernest Nieto                                    *
 *   ernest.nieto@gmail.com                                                *
 *   Copyright (C) 2007 by Joan Sala                                       *
 *   jsala@ac.upc.edu                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "nmgmgenbandwith.h"

#include <iostream>
using namespace std;



/**
 * Contructor of the class
 * @return 
 */
NMGMGenBandwith::NMGMGenBandwith()
{
}

/**
 * Destructor of the class
 * @return 
 */
NMGMGenBandwith::~NMGMGenBandwith()
{
}

/**
 * Returns the ATM Bandwith 
 * @param rate Rate
 * @param packetSize Packet size
 * @param isIPv6 Using IPv6
 * @param isRaw Raw mode (size inclued Ip headers)
 * @param isBPSatIPlevel BPS are at IP level
 * @param isLinkLevel Counts BPS up to ATM level
 * @param countHeaderATM Counts all excep the ATM headers
 * @param isKbps The result will be show in Kbps (if true), if false in bps
 * @return The bandwith or -1 if error
 */
int NMGMGenBandwith::getATMBandwith(int rate, int packetSize, bool isIPv6, bool isRaw,
		bool isBPSatIPlevel, bool isLinkLevel, bool countAllExceptHeaderATM, bool isKbps)
{
	int mida_paquet, mida_paquet_ip, bit_paquet, cela_paquet;
	int bw_atm, bps_ip;

	if (rate <= 0 or packetSize<0) return -1; 

	if (isIPv6)
	{
		if (isRaw) mida_paquet_ip =  packetSize;
		else
			mida_paquet_ip =  packetSize + ATM_IP6_HDR + ATM_UDP_HDR;
	}
	else
	{
		if (isRaw) mida_paquet_ip = packetSize;
		else mida_paquet_ip = packetSize + ATM_IP4_HDR + ATM_UDP_HDR;
	}

	if (mida_paquet_ip <= 20) 
	{
		cerr << "The UDP payload has to be bigger than 20 (size: "<< mida_paquet_ip<<")."<<endl;
		return -1;
	}

	if (isLinkLevel) 
	{  // Cas amb el que estem calculant una amplada de banda a nivell ATM (amb cel·les i tot)
		mida_paquet = mida_paquet_ip + ATM_PAYLOAD_AAL5 + ATM_PAYLOAD_LLC;
		cela_paquet = mida_paquet / ATM_PAYLOAD_ATM;

		if ((mida_paquet / ATM_PAYLOAD_ATM) % ATM_PAYLOAD_ATM)
			cela_paquet++;

		// Bit paquet conté el tamany a nivell ATM del paquet, incloent la mida de LLC/SNAP i AAL5
		if (countAllExceptHeaderATM) // En aquest cas no comptem el payload de les capçaleres cel·les!!!!
			bit_paquet = ATM_PAYLOAD_ATM * cela_paquet * 8;
		else bit_paquet = ATM_MIDA_ATM * cela_paquet * 8;

		bw_atm = rate * bit_paquet;
		bps_ip = rate * mida_paquet_ip * 8;
		if (isBPSatIPlevel)  return isKbps? bps_ip/1024:bps_ip;
		else return isKbps? bw_atm/1024:bw_atm;
	}
	else
	{  // Cas més genèric on intentem emular el que fa Cisco (comptant l'amplada com ho fa ell!)
		mida_paquet = mida_paquet_ip + ATM_PAYLOAD_LLC + ATM_PAYLOAD_HALF_AAL5;
		bit_paquet = mida_paquet * 8;
		bw_atm = rate * bit_paquet;
		bps_ip = rate * mida_paquet_ip * 8;
		return isKbps?bps_ip/1024:bps_ip;
		//if(isBPSatIPlevel) return isKbps?bps_ip/1024:bps_ip;
		//else return isKbps? bw_atm/1024:bw_atm;
	}
}

/**
 * Returns the Ethernet Bandwith 
 * @param rate Rate
 * @param packetSize Packet size
 * @param isIPv6 Using IPv6
 * @param isRaw Raw mode (size inclued Ip headers)
 * @param isBPSatIPlevel BPS are at IP level
 * @param isKbps The result will be show in Kbps (if true), if false in bps
 * @return The bandwith or -1 if error
 */
int NMGMGenBandwith::getEthernetBandwith(int rate, int packetSize, bool isIPv6, bool isRaw,
					bool isBPSatIPlevel, bool isKbps)
{
	int mida_paquet, mida_paquet_ip, bit_paquet;
        int bw_eth, bps_ip;

	if (rate <= 0 or packetSize<0) return -1;

	if (isIPv6)
	{
		if (isRaw) mida_paquet_ip =  packetSize;
		else mida_paquet_ip =  packetSize + ETH_IP6_HDR + ETH_UDP_HDR;
	}
	else
	{
		if (isRaw) mida_paquet_ip = packetSize;
		else mida_paquet_ip = packetSize + ETH_IP4_HDR + ETH_UDP_HDR;
	}

	if (mida_paquet_ip <= 20)
	{
		cerr << "The UDP payload has to be bigger than 20 (size: "<<mida_paquet_ip<< ")."<<endl;
		return -1;
	}

	mida_paquet = mida_paquet_ip + ETH_PAYLOAD_LLC;
	bit_paquet = mida_paquet * 8;
	bw_eth = rate * bit_paquet;
	bps_ip = rate * mida_paquet_ip * 8;
	
	if (isBPSatIPlevel) return isKbps? bps_ip/1024:bps_ip;
	else return isKbps?bw_eth/1024:bw_eth;
}


/************************************************************************************************************
				Ethernet Bandwith (original netmeter code)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define PAYLOAD_LLC  18
#define IP4_HDR      20
#define IP6_HDR      40
#define UDP_HDR      8

int usage();

int main(int argc, char *argv[]) {
	int sel_opt, is_verbose, is_ipv6, is_raw, is_ip, is_kbps;
	int mida_paquet, mida_paquet_ip, bit_paquet, cela_paquet;
        int bw_eth, bps_ip, rate_;

	// Initialitzations
	is_ipv6 = 0; is_verbose = 0, is_raw = 0, is_ip = 0, is_kbps = 0;
	rate_ = -1;

	// Main option parsing loop
	opterr = 0;
	while(1) {
		sel_opt = getopt(argc, argv, "vhrikR:6");
		if (sel_opt == -1)
			break;
		switch (sel_opt) {
		case 'v':
			is_verbose = 1;
		break;
		case 'h':
			usage();
			exit(0);
		break;
		case 'r':
			is_raw = 1;
		break;
		case 'i':
			is_ip = 1;
		break;
		case 'k':
			is_kbps = 1;
		break;
		case 'R':
			rate_ = atoi(optarg);
		break;
		case '6':
			is_ipv6 = 1;
		break;
		default:
			usage();
			exit(-2);
		break;
		}
	}
	if (rate_ <= 0) {
		fprintf(stderr,"Rate is a mandatory argument and has to be > 0\n");
		usage();
		exit(-1);
	}
	if (!argv[optind]) {
		fprintf(stderr,"Packet size is mandatory\n");
		usage();
		exit(-1);
	}
	if (is_ipv6) {
		if (is_raw)
			mida_paquet_ip =  atoi(argv[optind]);
		else
			mida_paquet_ip =  atoi(argv[optind]) + IP6_HDR + UDP_HDR;
	} else {
		if (is_raw)
			mida_paquet_ip = atoi(argv[optind]);
		else
			mida_paquet_ip = atoi(argv[optind]) + IP4_HDR + UDP_HDR;
	}

	if (mida_paquet_ip <= 20) {
		fprintf(stderr,"The UDP payload has to be bigger than 20\n");
		return -1;
	}

	mida_paquet = mida_paquet_ip + PAYLOAD_LLC;
	bit_paquet = mida_paquet * 8;
	bw_eth = rate_ * bit_paquet;
	bps_ip = rate_ * mida_paquet_ip * 8;
	if (is_verbose) {
		fprintf(stdout,"Mida paquet IP amb capçalera: %d bytes\n",mida_paquet_ip);
		fprintf(stdout,"BW(ETH) = %d%s\n",is_kbps ? rate_/1024 : rate_, is_kbps ? "Kbps" : "bps");
		fprintf(stdout,"BW(IP)  = %d%s\n",is_kbps ? bps_ip/1024 : bps_ip, is_kbps ? "Kbps" : "bps");
	} else if (is_ip) {
		fprintf(stdout,"%d",is_kbps ? bps_ip/1024 : bps_ip);
	} else {
		fprintf(stdout,"%d",is_kbps ? bw_eth/1024 : bw_eth);
	}
	return 0;
}

int usage() {
	fprintf(stdout,"usage: ./bw [-v] [-6] [-l] [-r] [-e] [-k] -R pkt/sec <mida_paquet>\n");
	fprintf(stdout,"\tUtilitat per comptar l'amplada de banda que ocupa un rate realment a nivell Ethernet\n");
	fprintf(stdout,"  -v: Verbose, mostra informació extra per pantalla\n");
	fprintf(stdout,"  -i: BPS are at IP level\n");
	fprintf(stdout,"  -6: Suposem que treballem amb paquets IPv6\n");
	fprintf(stdout,"  -r: Raw Mode, es suposa que la mida del paquet inclou fins capçaleres IP [inclusive]\n");
	fprintf(stdout,"  -k: Les unitats de sortida de l'amplada de banda seran amb Kbps\n");
	return 0;
}


************************************************************************************************************/


/************************************************************************************************************
				ATM BANDWITH (original netmeter code)


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define PAYLOAD_ATM  48
#define MIDA_ATM     53
#define PAYLOAD_AAL5 8
#define PAYLOAD_HALF_AAL5 4
#define PAYLOAD_LLC  8
#define IP4_HDR      20
#define IP6_HDR      40
#define UDP_HDR      8

int usage();

int main(int argc, char *argv[]) {
	int sel_opt, is_verbose, is_ipv6, is_link, is_raw, is_ip, no_hdrATM, is_kbps;
	int mida_paquet, mida_paquet_ip, bit_paquet, cela_paquet;
        int bw_atm, bps_ip, rate_;

	// Initialitzations
	is_ipv6 = 0; is_verbose = 0, is_link = 0, is_raw = 0, no_hdrATM = 0, is_ip = 0, is_kbps = 0;
	rate_ = -1;

	// Main option parsing loop
	opterr = 0;
	while(1) {
		sel_opt = getopt(argc, argv, "vlhreikR:6");
		if (sel_opt == -1)
			break;
		switch (sel_opt) {
		case 'v':
			is_verbose = 1;
		break;
		case 'l':
			is_link = 1;
		break;
		case 'h':
			usage();
			exit(0);
		break;
		case 'r':
			is_raw = 1;
		break;
		case 'e':
			no_hdrATM = 1;
		break;
		case 'i':
			is_ip = 1;
		break;
		case 'k':
			is_kbps = 1;
		break;
		case 'R':
			rate_ = atoi(optarg);
		break;
		case '6':
			is_ipv6 = 1;
		break;
		default:
			usage();
			exit(-2);
		break;
		}
	}
	if (rate_ <= 0) {
		fprintf(stderr,"Rate is a mandatory argument and has to be > 0\n");
		usage();
		exit(-1);
	}
	if (!argv[optind]) {
		fprintf(stderr,"Packet size is mandatory\n");
		usage();
		exit(-1);
	}
	if (is_ipv6) {
		if (is_raw)
			mida_paquet_ip =  atoi(argv[optind]);
		else
			mida_paquet_ip =  atoi(argv[optind]) + IP6_HDR + UDP_HDR;
	} else {
		if (is_raw)
			mida_paquet_ip = atoi(argv[optind]);
		else
			mida_paquet_ip = atoi(argv[optind]) + IP4_HDR + UDP_HDR;
	}

	if (mida_paquet_ip <= 20) {
		fprintf(stderr,"The UDP payload has to be bigger than 20\n");
		return -1;
	}

	if (is_link) {  // Cas amb el que estem calculant una amplada de banda a nivell ATM (amb cel·les i tot)
		mida_paquet = mida_paquet_ip + PAYLOAD_AAL5 + PAYLOAD_LLC;
		cela_paquet = mida_paquet / PAYLOAD_ATM;
		if ((mida_paquet / PAYLOAD_ATM) % PAYLOAD_ATM)
			cela_paquet++;
		// Bit paquet conté el tamany a nivell ATM del paquet, incloent la mida de LLC/SNAP i AAL5
		if (no_hdrATM) {
			// En aquest cas no comptem el payload de les capçaleres cel·les!!!!
			bit_paquet = PAYLOAD_ATM * cela_paquet * 8;
		} else {
			bit_paquet = MIDA_ATM * cela_paquet * 8;
		}
		bw_atm = rate_ * bit_paquet;
		bps_ip = rate_ * mida_paquet_ip * 8;
		if (is_verbose) {
			fprintf(stdout,"Cel·les que ocupa el paquet: %d\n",cela_paquet);
			fprintf(stdout,"El paquet IP a nivell ATM ocupa %d bits\n",bit_paquet);
			fprintf(stdout,"Mida paquet IP amb capçalera: %d bytes\n",mida_paquet_ip);
			fprintf(stdout,"Bandwidth ATM level: %d%s\n",is_kbps ? rate_/1024 : rate_, is_kbps ? "Kbps" : "bps");
			fprintf(stdout,"Bandwidth IP  level: %d%s\n",is_kbps ? bps_ip/1024 : bps_ip, is_kbps ? "Kbps" : "bps");
		} else if (is_ip) {
			fprintf(stdout,"%d",is_kbps ? bps_ip/1024 : bps_ip);
		} else {
			fprintf(stdout,"%d",is_kbps ? bw_atm/1024 : bw_atm);
		}
	}
	else {  // Cas més genèric on intentem emular el que fa Cisco (comptant l'amplada com ho fa ell!)
		mida_paquet = mida_paquet_ip + PAYLOAD_LLC + PAYLOAD_HALF_AAL5;
		bit_paquet = mida_paquet * 8;
		bw_atm = rate_ * bit_paquet;
		bps_ip = rate_ * mida_paquet_ip * 8;
		if (is_verbose) {
			fprintf(stdout,"Mida paquet IP amb capçalera: %d bytes\n",mida_paquet_ip);
			fprintf(stdout,"Paquets per segon a nivell ATM: %d pkts/s.\n",rate_);
			fprintf(stdout,"BW(ATM) = %8d %s\n",is_kbps ? bw_atm/1024 : bw_atm, is_kbps ? "Kbps" : "bps");
			fprintf(stdout,"BW(IP)  = %8d %s\n",is_kbps ? bps_ip/1024 : bps_ip, is_kbps ? "Kbps" : "bps");
		}
		else {
			fprintf(stdout,"%d",is_kbps ? bps_ip/1024 : bps_ip);
		}
	}
	return 0;
}

int usage() {
	fprintf(stdout,"usage: ./bw [-v] [-6] [-l] [-r] [-e] [-k] -R pkt/sec <mida_paquet>\n");
	fprintf(stdout,"\tUtilitat per comptar l'amplada de banda que ocupa un rate realment a nivell AAL5\n");
	fprintf(stdout,"\t\tS'ha seguit la política de Cisco: Comptar la capçalera LLC/SNAP i la meitat del trailer AAL5\n");
	fprintf(stdout,"  -v: Verbose, mostra informació extra per pantalla\n");
	fprintf(stdout,"  -i: BPS are at IP level\n");
	fprintf(stdout,"  -6: Suposem que treballem amb paquets IPv6\n");
	fprintf(stdout,"  -l: Link Level, compta els bps fins a nivell ATM\n");
	fprintf(stdout,"  -r: Raw Mode, es suposa que la mida del paquet inclou fins capçaleres IP [inclusive]\n");
	fprintf(stdout,"  -e: Ho compta tot excepte les capçaleres ATM\n");
	fprintf(stdout,"  -k: Les unitats de sortida de l'amplada de banda seran amb Kbps\n");
	return 0;
}

************************************************************************************************************/


