/*
 This file is part of BitPunch
 Copyright (C) 2015 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef BOX_H
#define BOX_H

#include <bitpunch/config.h>
#include <bitpunch/debugio.h>
#include <bitpunch/crypto/mecs.h>
#include <bitpunch/math/gf2types.h>
#include <bitpunch/crypto/mecsctx.h>

#include <bitpunch/asn1/asn1.h>

static const char *encsalt = "PBKDF2-SALT-CONST-ENCRYPTION";
static const char *macsalt = "PBKDF2-SALT-CONST-MAC";
static const char *ivsalt = "AES-IV";
#define BPU_MAC_LEN 256
#define AES_SIZE 128


/**
 * The function encrypts in and puts the output to out.
 * @param  out[out] output vector
 * @param  in[in]   input message
 * @param  pk[in]   a publick-key in ASN.1 format
 * @param  size[in]   a size of the publick-key 
 * @return                0 on succes else, non-zero value
 */
/// Add padding to message.
int BPU_cryptobox_send(BPU_T_GF2_Vector *out, BPU_T_GF2_Vector *in, const char *pk, int size);

/**
 * The function decrypts in and puts the output to out.
 * @param  out[out] output vector
 * @param  in[in]   input message
 * @param  ctx[in]   a MECS context
 * @return                0 on succes else, non-zero value
 */
int BPU_cryptobox_recieve(BPU_T_GF2_Vector *out, BPU_T_GF2_Vector *in, const BPU_T_Mecs_Ctx *ctx2);

#endif // BOX_H
