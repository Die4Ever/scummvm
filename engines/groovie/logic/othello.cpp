/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * This file is dual-licensed.
 * In addition to the GPLv2 license mentioned above, MojoTouch has exclusively licensed
 * this code on November 10th, 2021, to be use in closed-source products.
 * Therefore, any contributions (commits) to it will also be dual-licensed.
 *
 */

#include "groovie/logic/othello.h"
#include "groovie/groovie.h"

namespace Groovie {

// Ghidra crap
typedef uint16 ushort;
typedef int undefined4;
typedef uint16 undefined2;
typedef byte undefined;

#pragma pack(push, 1)
struct int24 {
	int data : 24;
};
#pragma pack(pop)
typedef int24 undefined3;
typedef int24 int3;

// these 3 control rng in some weird way
int rng_a_DAT_0044fa94 = 0;
int rng_b_DAT_0044fa98 = 0;
int rng_c_DAT_0044fa9c = 0;

//uint FUN_00412a70(uint param_1, undefined4 param_2, uint param_3)
ushort ai_rng_FUN_00412a70(/*uint param_1,*/ /*undefined4 &param_2,*/ /*uint &param_3*/) {
	uint uVar1;
	ushort uVar2;
	ushort uVar3;
	ushort uVar4;
	ushort uVar5 = 0;
	//short local_4;

	//uVar2 = param_1 & 0xffff0000 | (uint)rng_a_DAT_0044fa94;
	uVar2 = (uint)rng_a_DAT_0044fa94;
	if (((rng_a_DAT_0044fa94 == 0) && (rng_b_DAT_0044fa98 == 0)) && (rng_c_DAT_0044fa9c == 0)) {
		rng_c_DAT_0044fa9c = 0xc;
		//uVar2 = CONCAT22((short)((param_1 & 0xffff0000) >> 0x10), 0xe6);
		uVar2 = 230;
		rng_b_DAT_0044fa98 = 0x1c;
	}
	/*local_4 = 0x10;
	uVar5 = 0;
	do {
		uVar3 = (uVar2 >> 1) + uVar5;
		uVar5 = uVar2 & 1;
		uVar4 = rng_c_DAT_0044fa9c & 0x80;
		rng_c_DAT_0044fa9c = ((uVar3 >> 2 ^ rng_b_DAT_0044fa98) & 1) + rng_c_DAT_0044fa9c * 2;
		uVar1 = rng_b_DAT_0044fa98;
		rng_b_DAT_0044fa98 = (uVar4 >> 7) + rng_b_DAT_0044fa98 * 2;
		//param_3 = param_3 & 0xffff0000 | (uVar1 & 0x80) >> 7;
		param_3 = ((uVar1 & 0x80) >> 7);
		local_4 = local_4 + -1;
		uVar2 = param_3 + uVar2 * 2;
	} while (local_4 != 0);*/
	for (short i = 16; i > 0; i--) {
		uVar3 = (uVar2 >> 1) + uVar5;
		uVar5 = uVar2 & 1;
		uVar4 = rng_c_DAT_0044fa9c & 0x80;
		rng_c_DAT_0044fa9c = ((uVar3 >> 2 ^ rng_b_DAT_0044fa98) & 1) + rng_c_DAT_0044fa9c * 2;
		uVar1 = rng_b_DAT_0044fa98;
		rng_b_DAT_0044fa98 = (uVar4 >> 7) + rng_b_DAT_0044fa98 * 2;
		//param_3 = param_3 & 0xffff0000 | (uVar1 & 0x80) >> 7;
		uint param_3 = ((uVar1 & 0x80) >> 7);
		uVar2 = param_3 + uVar2 * 2;
	}
	rng_a_DAT_0044fa94 = uVar2;
	//return uVar2 & 0xffff0000 | (uint)(ushort)((short)uVar2 << 8 | rng_b_DAT_0044fa98);
	return (uVar2 << 8 | rng_b_DAT_0044fa98);
}

uint ai_rng_FUN_00412b50(/*uint param_1, undefined4 param_2,*/ /*uint param_3*/)

{
	uint uVar1;
	uint uVar2;
	//uint extraout_ECX;
	//undefined4 extraout_EDX;

	uVar1 = ai_rng_FUN_00412a70(/*param_1,*/ /*param_2,*/ /*param_3*/);
	//uVar2 = FUN_00412a70(uVar1, extraout_EDX, extraout_ECX);
	uVar2 = ai_rng_FUN_00412a70(/*uVar1,*/ /*param_2,*/ /*param_3*/);
	return uVar1 << 16 | uVar2 & 0xffff;
}

void *_calloc(uint num, uint size) {
	void *p = malloc(num * size);
	memset(p, 0, num * size);
	return p;
}

// end of ghidra defaults

size_t DAT_0044d818;
undefined4 *DAT_0044d710 = 0;
//void *_othello_func_pointer_DAT_00464e68 = 0;
int(__cdecl *_othello_func_pointer_DAT_00464e68)(int) = 0;
int *DAT_00464f70 = 0;
int *DAT_00464e6c = 0;
int DAT_0044d820 = 0;
char DAT_0044da44;
undefined _DAT_0044d834;
undefined _DAT_0044d828;
undefined4 DAT_0044d82c;
char DAT_0044da40;
char DAT_0044da41;
char DAT_0044da42;
char DAT_0044d720;
undefined _DAT_00464e64;
undefined DAT_0044d728;
char ***DAT_00464e70 = 0;
auto &PTR_00464e70 = DAT_00464e70;
undefined DAT_0044dac8;
undefined DAT_0044da58;
undefined DAT_0044da48;
undefined DAT_0044da4c;
undefined DAT_0044da50;
int DAT_0044d838;
undefined _DAT_0044d81c;


uint CONCAT31(uint a, uint b) {
	uint ret;
	ret = (a << 8) & 0xffffff00;
	ret |= b & 0xff;
	return ret;
}

uint CONCAT22(uint a, uint b) {
	uint ret;
	ret = (a << 16) & 0xffff0000;
	ret |= b & 0xffff;
	return ret;
}


void othello_calloc_FUN_004121d0(void)

{
	void *pvVar1;
	int iVar2;
	int iVar3;
	int iVar4;
	int iVar5;

	DAT_0044d710 = (undefined4 *)_calloc(DAT_0044d818, 0x9c);
	if (DAT_0044d710 == (void *)0x0) {
		//error_FUN_0040b560(s_Othello_couldn't_alloc_freeboard_0044dc90,1);
		error("s_Othello_couldn't_alloc_freeboard_0044dc90,1");
	}
	pvVar1 = _calloc(DAT_0044d818, 0x40);
	if (pvVar1 == (void *)0x0) {
		//error_FUN_0040b560(s_Othello_couldn't_alloc_boardspac_0044dc6c,1);
		error("s_Othello_couldn't_alloc_boardspac_0044dc6c,1");
	}
	iVar5 = 0;
	if (0 < (int)DAT_0044d818) {
		iVar4 = 0;
		do {
			*(int *)((int)DAT_0044d710 + iVar4 + -0x9c) = (int)DAT_0044d710 + iVar4;
			iVar2 = 8;
			iVar3 = iVar4;
			do {
				*(void **)((int)DAT_0044d710 + iVar3 + 0x7c) = pvVar1;
				pvVar1 = (void *)((int)pvVar1 + 8);
				iVar2 += -1;
				iVar3 = iVar3 + 4;
			} while (iVar2 != 0);
			iVar4 += 0x9c;
			iVar5 += 1;
		} while (iVar5 < (int)DAT_0044d818);
	}
	*(undefined4 *)((int)DAT_0044d710 + DAT_0044d818 * 0x9c + -0x9c) = 0;
	DAT_0044d818 = 10;
	return;
}

void othello_calloc_FUN_004120e0(void)

{
	undefined4 uVar1;

	if (DAT_0044d710 == (undefined4 *)0x0) {
		othello_calloc_FUN_004121d0();
	}
	uVar1 = *DAT_0044d710;
	*DAT_0044d710 = 0;
	DAT_0044d710 = (undefined4 *)uVar1;
	return;
}

/* WARNING: Could not reconcile some variable overlaps */

void othello_FUN_004122a0(void)

{
	int iVar1;
	char **ppcVar2;
	char ***pppcVar3;
	char **ppcVar4;
	int iVar5;
	int iVar6;
	int iVar7;
	int local_18;
	char local_10;
	int local_c;
	int local_4;

	pppcVar3 = (char ***)_calloc(0x1e4, 4);
	if (pppcVar3 == (char ***)0x0) {
		error("error_FUN_0040b560(s_Othello_couldn't_alloc_pointers_0044dcd4,1);");
	}
	ppcVar4 = (char **)_calloc(0x7e0, 1);
	if (ppcVar4 == (char **)0x0) {
		error("error_FUN_0040b560(s_Othello_couldn't_alloc_bytes_0044dcb4,1);");
	}
	local_4 = 0;
	local_10 = '\0';
	local_c = 0;
	do {
		local_18 = 0;
		do {
			iVar5 = -1;
			(&PTR_00464e70)[local_c + local_18] = pppcVar3;
			do {
				iVar6 = -1;
				do {
					if ((iVar5 != 0) || (iVar6 != 0)) {
						*pppcVar3 = ppcVar4;
						iVar1 = local_4 + iVar5;
						ppcVar2 = ppcVar4;
						for (iVar7 = local_18 + iVar6;
							 (((-1 < iVar1 && (iVar1 < 8)) && (-1 < iVar7)) && (iVar7 < 8)); iVar7 += iVar6) {
							*(char *)ppcVar2 = (char)iVar7 + (char)iVar1 * '\b';
							iVar1 = iVar1 + iVar5;
							ppcVar2 = (char **)((int)ppcVar2 + 1);
						}
						if ((local_4 + iVar5 != iVar1) || (ppcVar4 = ppcVar2, local_18 + iVar6 != iVar7)) {
							ppcVar4 = (char **)((int)ppcVar2 + 1);
							pppcVar3 = pppcVar3 + 1;
							*(char *)ppcVar2 = local_10 + (char)local_18;
						}
					}
					iVar6 += 1;
				} while (iVar6 < 2);
				iVar5 += 1;
			} while (iVar5 < 2);
			*pppcVar3 = (char **)0x0;
			pppcVar3 = pppcVar3 + 1;
			local_18 += 1;
		} while (local_18 < 8);
		local_c += 8;
		local_10 += '\b';
		local_4 += 1;
	} while (local_c < 0x40);
	return;
}


void __cdecl othello_FUN_00411d40(int param_1)

{
	char *pcVar1;
	int iVar2;
	int *piVar3;
	int iVar4;

	/*iVar2 =*/ othello_calloc_FUN_004120e0();
	piVar3 = (int *)(iVar2 + 0x7c);
	iVar2 = 0;
	do {
		iVar4 = 0;
		do {
			pcVar1 = (char *)(param_1 + 0x19 + iVar2 + iVar4);
			if (*pcVar1 == DAT_0044da40) {
				*(undefined *)(*piVar3 + iVar4) = 0;
			}
			if (*pcVar1 == DAT_0044da41) {
				*(undefined *)(*piVar3 + iVar4) = 1;
			}
			if (*pcVar1 == DAT_0044da42) {
				*(undefined *)(*piVar3 + iVar4) = 2;
			}
			iVar4 += 1;
		} while (iVar4 < 8);
		piVar3 = piVar3 + 1;
		iVar2 += 10;
	} while (iVar2 < 0x50);
	return;
}


int __cdecl othello_func_pointee_FUN_00412720(int param_1)

{
	char cVar1;
	char cVar2;
	char cVar3;
	char *pcVar4;
	int iVar5;
	int local_58[4];
	int local_48;
	int local_44;
	int local_40;
	int local_3c;
	int local_38;
	int local_34;
	int local_30;
	int local_2c;
	int local_28;
	int local_24;
	int local_20;
	int local_1c;
	int local_18;
	int local_14;
	int local_10;
	int local_c;
	int local_8;
	int local_4;

	local_58[2] = 0;
	pcVar4 = *(char **)(param_1 + 0x7c);
	local_58[0] = 0;
	local_48 = (int)pcVar4[0x38];
	local_10 = (int)pcVar4[0x3a];
	local_c = (int)pcVar4[0x3b];
	local_8 = (int)pcVar4[0x3c];
	local_44 = (int)pcVar4[0x3f];
	cVar1 = pcVar4[1];
	local_4 = (int)pcVar4[0x3d];
	iVar5 = (int)*pcVar4;
	local_40 = (int)pcVar4[2];
	local_3c = (int)pcVar4[3];
	local_38 = (int)pcVar4[4];
	local_34 = (int)pcVar4[5];
	local_58[3] = (int)pcVar4[7];
	local_2c = (int)pcVar4[0x17];
	local_24 = (int)pcVar4[0x1f];
	local_1c = (int)pcVar4[0x27];
	local_14 = (int)pcVar4[0x2f];
	local_30 = (int)pcVar4[0x10];
	local_28 = (int)pcVar4[0x18];
	local_20 = (int)pcVar4[0x20];
	local_18 = (int)pcVar4[0x28];
	cVar2 = pcVar4[2];
	local_58[1] = *(int *)(&DAT_0044dac8 +
						   ((char)(&DAT_0044da58)
								[(int)pcVar4[0x3e] +
								 (int)(char)(&DAT_0044da58)
									 [(int)pcVar4[0x3d] +
									  (int)(char)(&DAT_0044da58)
										  [(int)pcVar4[0x3c] +
										   (int)(char)(&DAT_0044da58)
											   [(int)pcVar4[0x3b] +
												(int)(char)(&DAT_0044da58)
													[(int)pcVar4[0x3a] +
													 (int)(char)(&DAT_0044da58)
														 [(int)pcVar4[0x39] +
														  (int)(char)(&DAT_0044da58)[local_48]]]]]]] +
							local_44) *
							   4) +
				  *(int *)(&DAT_0044dac8 +
						   ((char)(&DAT_0044da58)
								[(int)pcVar4[0x37] +
								 (int)(char)(&DAT_0044da58)
									 [(int)pcVar4[0x2f] +
									  (int)(char)(&DAT_0044da58)
										  [(int)pcVar4[0x27] +
										   (int)(char)(&DAT_0044da58)
											   [(int)pcVar4[0x1f] +
												(int)(char)(&DAT_0044da58)
													[(int)pcVar4[0x17] +
													 (int)(char)(&DAT_0044da58)
														 [(int)pcVar4[0xf] +
														  (int)(char)(&DAT_0044da58)[local_58[3]]]]]]]] +
							local_44) *
							   4) +
				  *(int *)(&DAT_0044dac8 +
						   ((char)(&DAT_0044da58)
								[(int)pcVar4[0x30] +
								 (int)(char)(&DAT_0044da58)
									 [(int)pcVar4[0x28] +
									  (int)(char)(&DAT_0044da58)
										  [(int)pcVar4[0x20] +
										   (int)(char)(&DAT_0044da58)
											   [(int)pcVar4[0x18] +
												(int)(char)(&DAT_0044da58)
													[(int)pcVar4[0x10] +
													 (int)(char)(&DAT_0044da58)
														 [(int)(char)(&DAT_0044da58)[iVar5] +
														  (int)pcVar4[8]]]]]]] +
							local_48) *
							   4) +
				  *(int *)(&DAT_0044dac8 +
						   ((char)(&DAT_0044da58)
								[(int)pcVar4[6] +
								 (int)(char)(&DAT_0044da58)
									 [(int)pcVar4[5] +
									  (int)(char)(&DAT_0044da58)
										  [(int)pcVar4[4] +
										   (int)(char)(&DAT_0044da58)
											   [(int)pcVar4[3] +
												(int)(char)(&DAT_0044da58)
													[(int)pcVar4[2] +
													 (int)(char)(&DAT_0044da58)
														 [(int)(char)(&DAT_0044da58)[iVar5] + (int)cVar1]]]]]] +
							local_58[3]) *
							   4);
	local_58[iVar5] = local_58[iVar5] + 0x32;
	cVar3 = pcVar4[3];
	local_58[cVar1] = local_58[cVar1] + 4;
	local_58[cVar2] = local_58[cVar2] + 0x10;
	cVar1 = pcVar4[4];
	local_58[cVar3] = local_58[cVar3] + 0xc;
	local_58[cVar1] = local_58[cVar1] + 0xc;
	cVar1 = pcVar4[6];
	local_58[pcVar4[5]] = local_58[pcVar4[5]] + 0x10;
	cVar2 = pcVar4[7];
	local_58[cVar1] = local_58[cVar1] + 4;
	cVar1 = pcVar4[8];
	local_58[cVar2] = local_58[cVar2] + 0x32;
	cVar2 = pcVar4[9];
	local_58[cVar1] = local_58[cVar1] + 4;
	local_58[cVar2] = local_58[cVar2] - (int)(char)(&DAT_0044da48)[iVar5];
	cVar1 = pcVar4[0xb];
	local_58[pcVar4[10]] = local_58[pcVar4[10]] - (int)(char)(&DAT_0044da4c)[local_40];
	local_58[cVar1] = local_58[cVar1] - (int)(char)(&DAT_0044da50)[local_3c];
	local_58[pcVar4[0xc]] = local_58[pcVar4[0xc]] - (int)(char)(&DAT_0044da50)[local_38];
	local_58[pcVar4[0xd]] = local_58[pcVar4[0xd]] - (int)(char)(&DAT_0044da4c)[local_34];
	local_58[pcVar4[0xe]] = local_58[pcVar4[0xe]] - (int)(char)(&DAT_0044da48)[local_58[3]];
	cVar1 = pcVar4[0x10];
	local_58[pcVar4[0xf]] = local_58[pcVar4[0xf]] + 4;
	cVar2 = pcVar4[0x11];
	local_58[cVar1] = local_58[cVar1] + 0x10;
	local_58[cVar2] = local_58[cVar2] - (int)(char)(&DAT_0044da4c)[local_30];
	cVar1 = pcVar4[0x15];
	local_58[pcVar4[0x12]] = local_58[pcVar4[0x12]] + 1;
	cVar2 = pcVar4[0x16];
	local_58[cVar1] = local_58[cVar1] + 1;
	local_58[cVar2] = local_58[cVar2] - (int)(char)(&DAT_0044da4c)[local_2c];
	cVar1 = pcVar4[0x18];
	local_58[pcVar4[0x17]] = local_58[pcVar4[0x17]] + 0x10;
	cVar2 = pcVar4[0x19];
	cVar3 = pcVar4[0x1e];
	local_58[cVar1] = local_58[cVar1] + 0xc;
	local_58[cVar2] = local_58[cVar2] - (int)(char)(&DAT_0044da50)[local_28];
	cVar1 = pcVar4[0x1f];
	local_58[cVar3] = local_58[cVar3] - (int)(char)(&DAT_0044da50)[local_24];
	local_58[cVar1] = local_58[cVar1] + 0xc;
	cVar1 = pcVar4[0x21];
	local_58[pcVar4[0x20]] = local_58[pcVar4[0x20]] + 0xc;
	cVar2 = pcVar4[0x26];
	local_58[cVar1] = local_58[cVar1] - (int)(char)(&DAT_0044da50)[local_20];
	local_58[cVar2] = local_58[cVar2] - (int)(char)(&DAT_0044da50)[local_1c];
	cVar1 = pcVar4[0x28];
	local_58[pcVar4[0x27]] = local_58[pcVar4[0x27]] + 0xc;
	cVar2 = pcVar4[0x29];
	local_58[cVar1] = local_58[cVar1] + 0x10;
	local_58[cVar2] = local_58[cVar2] - (int)(char)(&DAT_0044da4c)[local_18];
	cVar1 = pcVar4[0x2d];
	local_58[pcVar4[0x2a]] = local_58[pcVar4[0x2a]] + 1;
	local_58[cVar1] = local_58[cVar1] + 1;
	local_58[pcVar4[0x2e]] = local_58[pcVar4[0x2e]] - (int)(char)(&DAT_0044da4c)[local_14];
	local_58[pcVar4[0x2f]] = local_58[pcVar4[0x2f]] + 0x10;
	cVar1 = pcVar4[0x31];
	local_58[pcVar4[0x30]] = local_58[pcVar4[0x30]] + 4;
	local_58[cVar1] = local_58[cVar1] - (int)(char)(&DAT_0044da48)[local_48];
	cVar1 = pcVar4[0x33];
	local_58[pcVar4[0x32]] = local_58[pcVar4[0x32]] - (int)(char)(&DAT_0044da4c)[local_10];
	local_58[cVar1] = local_58[cVar1] - (int)(char)(&DAT_0044da50)[local_c];
	local_58[pcVar4[0x34]] = local_58[pcVar4[0x34]] - (int)(char)(&DAT_0044da50)[local_8];
	cVar1 = pcVar4[0x36];
	local_58[pcVar4[0x35]] = local_58[pcVar4[0x35]] - (int)(char)(&DAT_0044da4c)[local_4];
	cVar2 = pcVar4[0x37];
	local_58[cVar1] = local_58[cVar1] - (int)(char)(&DAT_0044da48)[local_44];
	local_58[cVar2] = local_58[cVar2] + 4;
	cVar1 = pcVar4[0x39];
	local_58[pcVar4[0x38]] = local_58[pcVar4[0x38]] + 0x32;
	cVar2 = pcVar4[0x3a];
	local_58[cVar1] = local_58[cVar1] + 4;
	cVar1 = pcVar4[0x3b];
	local_58[cVar2] = local_58[cVar2] + 0x10;
	cVar2 = pcVar4[0x3c];
	local_58[cVar1] = local_58[cVar1] + 0xc;
	cVar1 = pcVar4[0x3d];
	local_58[cVar2] = local_58[cVar2] + 0xc;
	cVar2 = pcVar4[0x3e];
	local_58[cVar1] = local_58[cVar1] + 0x10;
	cVar1 = pcVar4[0x3f];
	local_58[cVar2] = local_58[cVar2] + 4;
	local_58[cVar1] = local_58[cVar1] + 0x32;
	return local_58[1] - local_58[2];
}


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl othello_FUN_004124a0(int param_1, int param_2)

{
	undefined *puVar1;
	char *pcVar2;
	char cVar3;
	char **ppcVar4;
	int iVar5;
	int iVar6;
	char *pcVar7;
	undefined *puVar8;
	undefined uVar9;
	int iVar10;
	int iVar11;
	undefined *puVar12;
	char *pcVar13;
	bool bVar14;

	iVar10 = (_DAT_00464e64 == 0) + 1;
	bVar14 = _DAT_00464e64 == 0;
	/*iVar5 =*/othello_calloc_FUN_004120e0();
	iVar11 = 2 - (uint)bVar14;
	puVar12 = *(undefined **)(iVar5 + 0x7c);
	puVar1 = puVar12 + 0x40;
	puVar8 = *(undefined **)(param_1 + 0x7c);
	for (; puVar12 < puVar1; puVar12 = puVar12 + 1) {
		uVar9 = *puVar8;
		puVar8 = puVar8 + 1;
		*puVar12 = uVar9;
	}
	iVar5 = *(int *)(iVar5 + 0x7c);
	ppcVar4 = (char **)(&DAT_00464e70)[param_2];
	uVar9 = (undefined)iVar10;
	pcVar13 = *ppcVar4;
	while (pcVar13 != (char *)0x0) {
		pcVar13 = *ppcVar4;
		iVar6 = (int)*(char *)(*pcVar13 + iVar5);
		pcVar7 = pcVar13;
		if (iVar11 == iVar6) {
			do {
				pcVar2 = pcVar7 + 1;
				pcVar7 = pcVar7 + 1;
			} while (*(char *)(*pcVar2 + iVar5) == iVar11);
			if (*(char *)(*pcVar7 + iVar5) == iVar10) {
				while (iVar11 == iVar6) {
					cVar3 = *pcVar13;
					pcVar13 = pcVar13 + 1;
					*(undefined *)(cVar3 + iVar5) = uVar9;
					iVar6 = (int)*(char *)(*pcVar13 + iVar5);
				}
			}
		}
		ppcVar4 = ppcVar4 + 1;
		pcVar13 = *ppcVar4;
	}
	*(undefined *)(param_2 + iVar5) = uVar9;
	return;
}

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl othello_FUN_00412130(int param_1, int param_2)

{
	int *piVar1;
	int iVar2;
	int iVar3;
	bool bVar4;
	int iVar5;
	int iVar6;
	int *piVar7;
	int iVar8;

	iVar6 = param_2 / 2;
	do {
		iVar5 = iVar6;
		if (iVar6 < 1) {
			return;
		}
		for (; iVar5 < param_2; iVar5 = iVar5 + 1) {
			iVar8 = iVar5 - iVar6;
			if (-1 < iVar8) {
				piVar7 = (int *)(param_1 + iVar8 * 4);
				do {
					iVar2 = *(int *)(param_1 + (iVar8 + iVar6) * 4);
					iVar3 = *(int *)(*piVar7 + 0x78);
					if (_DAT_00464e64 == 0) {
						bVar4 = true;
						if (iVar3 <= *(int *)(iVar2 + 0x78))
							goto LAB_00412193;
					} else {
						piVar1 = (int *)(iVar2 + 0x78);
						bVar4 = true;
						if (*piVar1 == iVar3 || *piVar1 < iVar3) {
						LAB_00412193:
							bVar4 = false;
						}
					}
					if (!bVar4)
						break;
					iVar2 = *piVar7;
					piVar1 = (int *)(param_1 + (iVar8 + iVar6) * 4);
					*piVar7 = *piVar1;
					piVar7 = piVar7 + -iVar6;
					iVar8 -= iVar6;
					*piVar1 = iVar2;
				} while (-1 < iVar8);
			}
		}
		iVar6 /= 2;
	} while (true);
}


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __cdecl othello_FUN_004123d0(int *param_1)

{
	int iVar1;
	int iVar2;
	undefined4 uVar3;
	char **ppcVar4;
	char *pcVar5;
	char cVar6;
	int *piVar7;
	int *piVar8;
	int iVar9;
	bool bVar10;
	int local_c;
	char **local_4;

	iVar9 = 0;
	bVar10 = _DAT_00464e64 == 0;
	local_c = 0;
	iVar1 = param_1[0x1f];
	local_4 = (char **)&DAT_00464e70;
	cVar6 = '\x02' - (_DAT_00464e64 == 0);
	piVar7 = param_1;
	do {
		piVar8 = piVar7;
		if (*(char *)(iVar9 + iVar1) == '\0') {
			ppcVar4 = (char **)*local_4;
			do {
				do {
					pcVar5 = *ppcVar4;
					ppcVar4 = ppcVar4 + 1;
					if (pcVar5 == (char *)0x0)
						goto LAB_00412467;
				} while (*(char *)(*pcVar5 + iVar1) != cVar6);
				do {
					pcVar5 = pcVar5 + 1;
				} while (*(char *)(*pcVar5 + iVar1) == cVar6);
			} while (*(char *)(*pcVar5 + iVar1) != (char)(bVar10 + '\x01'));
			piVar8 = piVar7 + 1;
			/*iVar2 =*/othello_FUN_004124a0((int)param_1, iVar9);
			local_c += 1;
			*piVar7 = iVar2;
			uVar3 = (*_othello_func_pointer_DAT_00464e68)(iVar2);
			*(undefined4 *)(iVar2 + 0x78) = uVar3;
		}
	LAB_00412467:
		local_4 = local_4 + 1;
		iVar9 += 1;
		piVar7 = piVar8;
		if (0x3f < iVar9) {
			othello_FUN_00412130((int)param_1, local_c);
			param_1[local_c] = 0;
			return local_c;
		}
	} while (true);
}


int __cdecl othello_func_pointee_FUN_00412c10(int param_1)

{
	char cVar1;
	char cVar2;
	char *pcVar3;
	int local_c[3];

	pcVar3 = *(char **)(param_1 + 0x7c);
	local_c[0] = 0;
	local_c[1] = 0;
	cVar1 = pcVar3[1];
	local_c[2] = 0;
	local_c[*pcVar3] = local_c[*pcVar3] + 1;
	cVar2 = pcVar3[2];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[3];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[4];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[5];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[6];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[7];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[8];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[9];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[10];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0xb];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0xc];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0xd];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0xe];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0xf];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x10];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x11];
	local_c[cVar2] = local_c[cVar2] + 1;
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x13];
	local_c[pcVar3[0x12]] = local_c[pcVar3[0x12]] + 1;
	cVar2 = pcVar3[0x14];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x15];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x16];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x17];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x18];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x19];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x1a];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x1b];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x1c];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x1d];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x1e];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x1f];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x20];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x21];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x22];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x23];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x24];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x25];
	local_c[cVar2] = local_c[cVar2] + 1;
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x27];
	local_c[pcVar3[0x26]] = local_c[pcVar3[0x26]] + 1;
	cVar2 = pcVar3[0x28];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x29];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x2a];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x2b];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x2c];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x2d];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x2e];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x2f];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x30];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x31];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x32];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x33];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x34];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x35];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x36];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x37];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x38];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x39];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x3a];
	local_c[cVar1] = local_c[cVar1] + 1;
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar1 = pcVar3[0x3c];
	local_c[pcVar3[0x3b]] = local_c[pcVar3[0x3b]] + 1;
	cVar2 = pcVar3[0x3d];
	local_c[cVar1] = local_c[cVar1] + 1;
	cVar1 = pcVar3[0x3e];
	local_c[cVar2] = local_c[cVar2] + 1;
	cVar2 = pcVar3[0x3f];
	local_c[cVar1] = local_c[cVar1] + 1;
	local_c[cVar2] = local_c[cVar2] + 1;
	return (local_c[1] - local_c[2]) * 4;
}


void __cdecl othello_FUN_00412110(undefined4 *param_1)

{
	*param_1 = (undefined4)DAT_0044d710;
	DAT_0044d710 = param_1;
	return;
}


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __cdecl othello_FUN_00411f70(int *param_1, int param_2, int param_3, int param_4)

{
	int *piVar1;
	uint uVar2;
	bool bVar3;
	int iVar4;
	int iVar5;
	int iVar6;
	int local_c;
	int local_8;
	int **local_4;

	local_c = othello_FUN_004123d0(param_1);
	if (local_c == 0) {
		_DAT_00464e64 = (uint)(_DAT_00464e64 == 0);
		local_c = othello_FUN_004123d0(param_1);
		if (local_c == 0) {
			iVar4 = othello_func_pointee_FUN_00412c10((int)param_1);
			return iVar4;
		}
	}
	iVar4 = param_2 + -1;
	local_8 = (-(uint)(_DAT_00464e64 == 0) & 200) - 100;// ??
	uVar2 = (uint)(_DAT_00464e64 == 0);
	iVar6 = 0;
	if (0 < local_c) {
		local_4 = (int **)param_1;
		do {
			piVar1 = *local_4;
			_DAT_00464e64 = uVar2;
			if (iVar4 == 0) {
				iVar5 = piVar1[0x1e];
			} else {
				if (uVar2 == 0) {
					iVar5 = othello_FUN_00411f70(piVar1, iVar4, local_8, param_4);
				} else {
					iVar5 = othello_FUN_00411f70(piVar1, iVar4, param_3, local_8);
				}
			}
			if (local_8 < iVar5 != uVar2) {
				if (uVar2 == 0) {
					bVar3 = true;
					if (iVar5 < param_4)
						goto LAB_0041207d;
				} else {
					bVar3 = true;
					if (param_3 < iVar5) {
					LAB_0041207d:
						bVar3 = false;
					}
				}
				local_8 = iVar5;
				if (bVar3) {
					for (; iVar6 < local_c; iVar6 += 1) {
						othello_FUN_00412110((undefined4 *)param_1[iVar6]);
					}
					return iVar5;
				}
			}
			iVar6 += 1;
			othello_FUN_00412110(piVar1);
			local_4 = local_4 + 1;
		} while (iVar6 < local_c);
	}
	return local_8;
}

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __cdecl othello_FUN_00411e70(int **param_1)

{
	char *pcVar1;
	int *piVar2;
	int iVar3;
	int iVar4;
	char *pcVar5;
	undefined4 uVar6;
	int iVar7;
	int iVar8;
	int local_8;
	int local_4;

	local_4 = -0x65;
	iVar7 = -100;
	if (DAT_0044d720 == '\0') {
		_DAT_00464e64 = 1;
	}
	iVar3 = othello_FUN_004123d0(*param_1);
	if (iVar3 == 0) {
		return 0;
	}
	iVar8 = 0;
	if (0 < iVar3) {
		do {
			_DAT_00464e64 = (uint)(_DAT_00464e64 == 0);
			iVar4 = othello_FUN_00411f70((int *)(*param_1)[iVar8], *(int *)(&DAT_0044d728 + DAT_0044d820 * 4), iVar7,
										 100);
			if (local_4 < iVar4) {
				iVar7 = iVar4;
				local_8 = iVar8;
				local_4 = iVar4;
			}
			iVar8 += 1;
		} while (iVar8 < iVar3);
	}
	iVar7 = (*param_1)[0x1f] + -1;
	pcVar5 = (char *)(*(int *)((*param_1)[local_8] + 0x7c) + -1);
	do {
		do {
			pcVar5 = pcVar5 + 1;
			pcVar1 = (char *)(iVar7 + 1);
			iVar7 += 1;
		} while (*pcVar5 == *pcVar1);
	} while (*pcVar1 != '\0');
	iVar7 = 0;
	if (0 < iVar3) {
		do {
			if (local_8 != iVar7) {
				othello_FUN_00412110((undefined4 *)(*param_1)[iVar7]);
			}
			iVar7 += 1;
		} while (iVar7 < iVar3);
	}
	piVar2 = (int *)(*param_1)[local_8];
	/*uVar6 =*/ othello_FUN_00412110(*param_1);
	*param_1 = piVar2;
	if (DAT_0044d720 == '\0') {
		DAT_0044d820 += 1;
	}
	//return CONCAT31((int3)((uint)uVar6 >> 8), 1);
	return CONCAT31((uint)uVar6 >> 8, 1);
}





/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

uint __cdecl othello_FUN_00412600(int **param_1, char param_2, char param_3)

{
	uint uVar1;
	undefined4 uVar2;
	int *piVar3;
	int iVar4;
	int iVar5;
	uint uVar6;
	uint uVar7;

	_DAT_00464e64 = 0;
	uVar1 = othello_FUN_004123d0(*param_1);
	if (uVar1 == 0) {
		return 0;
	}
	uVar7 = uVar1 & 0xffffff00;
	if (param_2 == '*') {
		DAT_0044d720 = 1;
		uVar2 = othello_FUN_00411e70(param_1);
		DAT_0044d720 = 0;
	LAB_004126bb:
		DAT_0044d820 += 1;
		//return CONCAT31((int3)((uint)uVar2 >> 8), 1);
		return CONCAT31((uint)uVar2 >> 8, 1);
	}
	iVar4 = (int)param_3;
	iVar5 = (int)param_2;
	if ((((-1 < iVar4) && (iVar4 < 8)) && (-1 < iVar5)) && (iVar5 < 8)) {
		piVar3 = *param_1;
		uVar7 = piVar3[iVar4 + 0x1f];
		if (*(char *)(uVar7 + iVar5) == '\0') {
			uVar7 = 0;
			while (uVar6 = *(uint *)(*piVar3 + 0x7c + iVar4 * 4), *(char *)(uVar6 + iVar5) == '\0') {
				piVar3 = piVar3 + 1;
				uVar7 += 1;
				if (uVar7 == uVar1) {
					return uVar6 & 0xffffff00;
				}
			}
			uVar6 = 0;
			if (0 < (int)uVar1) {
				do {
					if (uVar7 != uVar6) {
						othello_FUN_00412110((undefined4 *)(*param_1)[uVar6]);
					}
					uVar6 += 1;
				} while ((int)uVar6 < (int)uVar1);
			}
			piVar3 = (int *)(*param_1)[uVar7];
			/*uVar2 =*/ othello_FUN_00412110(*param_1);
			*param_1 = piVar3;
			goto LAB_004126bb;
		}
	}
	return uVar7 & 0xffffff00;
}

uint __cdecl othello_FUN_004126d0(int param_1)

{
	char cVar1;
	char *pcVar2;
	char cVar3;
	char local_4[4];

	cVar3 = '@';
	local_4[0] = '\0';
	local_4[1] = '\0';
	local_4[2] = '\0';
	pcVar2 = *(char **)(param_1 + 0x7c);
	do {
		cVar1 = *pcVar2;
		pcVar2 = pcVar2 + 1;
		local_4[cVar1] = local_4[cVar1] + '\x01';
		cVar3 += -1;
	} while (cVar3 != '\0');
	if (local_4[2] < local_4[1]) {
		//return CONCAT31((int3)(((uint)pcVar2 & 0xffffff00) >> 8), 1);
		return CONCAT31(((uint)pcVar2 & 0xffffff00) >> 8, 1);
	}
	return (uint)pcVar2 & 0xffffff00 | (uint)(byte)((local_4[2] <= local_4[1]) + 2);
}


void __cdecl othello_init_FUN_00411db0(int param_1, int param_2, int param_3)

{
	int iVar1;
	char *pcVar2;
	char cVar3;
	int iVar4;
	int *piVar5;
	int iVar6;
	int local_8;
	int *local_4;

	if (param_1 == 0) {
		iVar4 = 0;
		piVar5 = (int *)(param_2 + 0x7c);
		do {
			iVar6 = 0;
			do {
				iVar1 = param_3 + iVar6;
				pcVar2 = (char *)(*piVar5 + iVar6);
				iVar6 += 1;
				//*(undefined1 *)(iVar1 + 0x19 + iVar4) = (&DAT_0044da40)[*pcVar2];
				*(char *)(iVar1 + 0x19 + iVar4) = (&DAT_0044da40)[*pcVar2];
			} while (iVar6 < 8);
			piVar5 = piVar5 + 1;
			iVar4 += 10;
		} while (iVar4 < 0x50);
		return;
	}
	local_4 = (int *)(param_1 + 0x7c);
	piVar5 = (int *)(param_2 + 0x7c);
	local_8 = 0;
	do {
		iVar4 = 0;
		do {
			cVar3 = (&DAT_0044da40)[*(char *)(*piVar5 + iVar4)];
			*(char *)(param_3 + 0x19 + local_8 + iVar4) = cVar3;
			if ((*(char *)(*local_4 + iVar4) == *(char *)(*piVar5 + iVar4)) &&
				(*(char *)(*piVar5 + iVar4) != '\0')) {
				*(char *)(param_3 + 0x19 + local_8 + iVar4) = cVar3 + ' ';
			}
			iVar4 += 1;
		} while (iVar4 < 8);
		local_4 = local_4 + 1;
		local_8 += 10;
		piVar5 = piVar5 + 1;
	} while (local_8 < 0x50);
	return;
}


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void othello_init_FUN_00412570(void)

{
	int iVar1;
	int iVar2;
	int iVar3;
	int *piVar4;

	/*iVar1 =*/ othello_calloc_FUN_004120e0();
	piVar4 = (int *)(iVar1 + 0x7c);
	iVar3 = 8;
	do {
		iVar2 = 0;
		do {
			iVar2 += 1;
			*(undefined *)(*piVar4 + -1 + iVar2) = 0;
		} while (iVar2 < 8);
		piVar4 = piVar4 + 1;
		iVar3 += -1;
	} while (iVar3 != 0);
	*(char *)(*(int *)(iVar1 + 0x8c) + 4) = '\x02' - (DAT_0044d838 == _DAT_0044d81c);
	*(undefined *)(*(int *)(iVar1 + 0x88) + 3) = *(undefined *)(*(int *)(iVar1 + 0x8c) + 4);
	*(char *)(*(int *)(iVar1 + 0x8c) + 3) = (DAT_0044d838 == _DAT_0044d81c) + '\x01';
	*(undefined *)(*(int *)(iVar1 + 0x88) + 4) = *(undefined *)(*(int *)(iVar1 + 0x8c) + 3);
	return;
}

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl othello_coins_run_FUN_00411aa0(undefined *param_1)

{
	undefined4 uVar1;
	uint uVar2;
	int iVar3;
	int *piVar4x;
	int *piVar4;

	byte op = param_1[1];

	switch (op) {
	case 0:// init
		*param_1 = 0;
		/*_othello_func_pointer_DAT_00464e68 = othello_func_pointee_FUN_00412720;
		othello_FUN_004122a0();
		if (DAT_0044d710 != 0) {
			DAT_0044d710 = 0;
			DAT_0044d818 = 0x40;
		}
		othello_calloc_FUN_004121d0();*/
		//*DAT_00464f70 = (int *)*/ othello_init_FUN_00412570();
		//*DAT_00464e6c = (int *)*/ othello_calloc_FUN_004120e0();
		/*DAT_0044d820 = 0;
		othello_init_FUN_00411db0(0, (int)DAT_00464f70, (int)param_1);*/
		param_1[4] = 1;
		return;
	case 1:
		DAT_0044da44 = 1;
		return;
	case 2:
		piVar4 = DAT_00464f70;
		piVar4x = DAT_00464e6c;
		for (iVar3 = 0x27; iVar3 != 0; iVar3 += -1) {
			*piVar4x = *piVar4;
			piVar4 = piVar4 + 1;
			piVar4x = piVar4x + 1;
		}
		if (DAT_0044d820 < 0x3c) {
			/*if (_DAT_0044d834 < DAT_0044d820) {
				_othello_func_pointer_DAT_00464e68 = othello_func_pointee_FUN_00412c10;
				_DAT_0044d828 = DAT_0044d82c;
			}
			DAT_0044da44 = '\0';
			uVar1 = othello_FUN_00412600(&DAT_00464f70, param_1[3], param_1[2]);*/
			param_1[4] = (char)uVar1;
		} else {
			//uVar2 = othello_FUN_004126d0((int)DAT_00464f70);
			*param_1 = (char)uVar2;
			param_1[4] = 1;
		}
		//othello_init_FUN_00411db0((int)DAT_00464e6c, (int)DAT_00464f70, (int)param_1);
		return;
	case 3:
		piVar4 = DAT_00464f70;
		piVar4x = DAT_00464e6c;
		for (iVar3 = 0x27; iVar3 != 0; iVar3 += -1) {
			*piVar4x = *piVar4;
			piVar4 = piVar4 + 1;
			piVar4x = piVar4x + 1;
		}
		if (DAT_0044d820 < 0x3c) {
			/*if (_DAT_0044d834 < DAT_0044d820) {
				_othello_func_pointer_DAT_00464e68 = othello_func_pointee_FUN_00412c10;
				_DAT_0044d828 = DAT_0044d82c;
			}*/
			param_1[3] = 0x2a;
			//uVar1 = othello_FUN_00412600(&DAT_00464f70, '*', param_1[2]);
			param_1[4] = (char)uVar1;
			if ((char)uVar1 == '\0') {
				DAT_0044da44 = '\x01';
			} else {
				DAT_0044da44 = '\0';
			}
		} else {
			//uVar2 = othello_FUN_004126d0((int)DAT_00464f70);
			*param_1 = (char)uVar2;
			param_1[4] = 1;
		}
		//othello_init_FUN_00411db0((int)DAT_00464e6c, (int)DAT_00464f70, (int)param_1);
		return;
	case 4:
		piVar4 = DAT_00464f70;
		piVar4x = DAT_00464e6c;
		/*for (iVar3 = 0x27; iVar3 != 0; iVar3 += -1) {
			*piVar4x = *piVar4;
			piVar4 = piVar4 + 1;
			piVar4x = piVar4x + 1;
		}*/
		if (DAT_0044d820 < 0x3c) {
			if (_DAT_0044d834 < DAT_0044d820) {
				_othello_func_pointer_DAT_00464e68 = othello_func_pointee_FUN_00412c10;
				_DAT_0044d828 = DAT_0044d82c;
			}
			//uVar1 = othello_FUN_00411e70(&DAT_00464f70);
			param_1[4] = (char)uVar1;
			if (((char)uVar1 == '\0') && (DAT_0044da44 != '\0')) {
				//uVar2 = othello_FUN_004126d0((int)DAT_00464f70);
				*param_1 = (char)uVar2;
			}
		} else {
			//uVar2 = othello_FUN_004126d0((int)DAT_00464f70);
			*param_1 = (char)uVar2;
			param_1[4] = 0;
		}
		//othello_init_FUN_00411db0((int)DAT_00464e6c, (int)DAT_00464f70, (int)param_1);
		return;
	case 5:
		DAT_0044d820 = (int)(char)param_1[2];
		//*DAT_00464f70 = (int *)*/ othello_FUN_00411d40((int)param_1);
		//othello_FUN_004122a0();
		//othello_FUN_004123d0(DAT_00464f70);
		param_1[4] = 1;
	}
	return;
}

OthelloGame::OthelloGame() : _random("OthelloGame") {
}

void OthelloGame::run(byte *scriptVariables) {
	// TODO
	othello_coins_run_FUN_00411aa0(scriptVariables);
}

} // namespace Groovie
