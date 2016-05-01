/*
 This file is part of BitPunch
 Copyright (C) 2014-2015 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>

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
#include <bitpunch/bitpunch.h>
#include <cryptobox/box.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>


int testKDF(){
    BPU_T_GF2_Vector *extended_pwd, *pwd, *salt;
    BPU_gf2VecMalloc(&pwd,64);
    BPU_gf2VecMalloc(&salt,64);
    BPU_gf2VecMalloc(&extended_pwd,512);
    BPU_gf2VecKDF(extended_pwd,pwd, salt, 512);
    return 0;
}


int testKeyGen(){


    return 0;
}

int testCryptoBox(){
    int rc = 0;
    char *pk = NULL;
    int size;
    BPU_T_UN_Mecs_Params params;
    BPU_T_GF2_Vector *pt_dem_a, *ct_kem;
       BPU_gf2VecMalloc(&ct_kem,3072);
       //ToDo: check -> must be more than 128
       //test 10 Megabeytes - 83886080 bits - segfault
       //(2048 - 1498) * 1498 - a public key size for given params
       BPU_gf2VecMalloc(&pt_dem_a,416);
       //BPU_gf2VecRand(pt_dem_a,20);

    /***************************************/
    // mce initialisation t = 50, m = 11
    //fprintf(stderr, "Basic GOPPA Initialisation...\n");
    if (BPU_mecsInitParamsGoppa(&params, 11, 50, 0)) {
        BPU_gf2VecFree(&ct_kem);
        BPU_gf2VecFree(&pt_dem_a);
        fprintf(stderr, "Error params\n");
        return 1;
    }
    BPU_T_Mecs_Ctx *ctx = NULL;
    if (BPU_mecsInitCtx(&ctx, &params, BPU_EN_MECS_BASIC_GOPPA)) {
        BPU_gf2VecFree(&ct_kem);
        BPU_gf2VecFree(&pt_dem_a);
        BPU_mecsFreeParamsGoppa(&params);
        BPU_mecsFreeCtx(&ctx);
        fprintf(stderr, "Mecs init\n");
        return 1;
    }
    /***************************************/
    //fprintf(stderr, "Key generation...\n");
    // key pair generation
    
    if (BPU_mecsGenKeyPair(ctx)) {
        BPU_gf2VecFree(&ct_kem);
        BPU_gf2VecFree(&pt_dem_a);
        BPU_mecsFreeParamsGoppa(&params);
        BPU_mecsFreeCtx(&ctx);
        fprintf(stderr,"Key generation error");
        return 1;
    }
    


    //Encoding pub key
    if (BPU_asn1EncodePubKey(&pk, &size, ctx)) {
        fprintf(stderr, "Encode pubkey error");
        free(pk);
        BPU_mecsFreeParamsGoppa(&params);
        BPU_mecsFreeCtx(&ctx);

        return 1;
    }
    
    //BPU_printError("Calling cryptobox...\n");
    
    if(BPU_cryptobox_send(ct_kem,pt_dem_a, pk,size) == 1){
        fprintf(stderr, "Hybrid scheme error\n");
        free(pk);
        BPU_mecsFreeCtx(&ctx);
        BPU_mecsFreeParamsGoppa(&params);
        BPU_gf2VecFree(&ct_kem);
        BPU_gf2VecFree(&pt_dem_a);
    return 1;
    }
    
    
   if(BPU_cryptobox_recieve(pt_dem_a,ct_kem, ctx)){
        BPU_printError("Hybrid scheme error");
        free(pk);
        BPU_mecsFreeCtx(&ctx);
        BPU_mecsFreeParamsGoppa(&params);
        BPU_gf2VecFree(&ct_kem);
        BPU_gf2VecFree(&pt_dem_a);
    return 1;
    }
        

    //Releasing used memory
    BPU_gf2VecFree(&ct_kem);
    BPU_gf2VecFree(&pt_dem_a);
    BPU_mecsFreeCtx(&ctx);
    BPU_mecsFreeParamsGoppa(&params);

    return rc;
}


int testKeyExchange(){
    int rc = 0;
    // MUST BE NULL
    BPU_T_Mecs_Ctx *ctx_A = NULL;
    BPU_T_Mecs_Ctx *ctx_B = NULL;
    BPU_T_Mecs_Ctx *ctx_E = NULL;
    BPU_T_UN_Mecs_Params params;
    BPU_T_GF2_Vector *r1,*r2_rec,*pub_vec, *s2_rec,*s3_rec,*s1, *s3,*s3_kem, *ct_kem,*s2_kem, *m1_rec,*r2,*r3,*pke_rec,*r3r1;
    char *pke = NULL;
    char *pkb = NULL;
    char *pka = NULL;
    char *pke_buf = NULL;
    int buf_size;
    //char *pke_buf = NULL;
    int size = 0;
    int size2 = 0;

    BPU_gf2VecMalloc(&r1,256);
    BPU_gf2VecMalloc(&r2,256);
    BPU_gf2VecMalloc(&r3,256);

    //A generates r1
    BPU_gf2VecRand(r1,20);

    /***************************************/
    // mce initialisation t = 50, m = 11
    fprintf(stderr, "Basic GOPPA Initialisation...\n");
    if (BPU_mecsInitParamsGoppa(&params, 11, 50, 0)) {
        BPU_gf2VecFree(&r1);
        BPU_gf2VecFree(&r2);
        BPU_gf2VecFree(&r3);
        BPU_mecsFreeParamsGoppa(&params);
        return 1;
    }

    if (BPU_mecsInitCtx(&ctx_A, &params, BPU_EN_MECS_BASIC_GOPPA)) {
        BPU_gf2VecFree(&r1);
        BPU_gf2VecFree(&r2);
        BPU_gf2VecFree(&r3);
        BPU_mecsFreeParamsGoppa(&params);
        return 1;
    }
    /***************************************/
    fprintf(stderr, "Key generation...\n");
    // key pair generation
    if (BPU_mecsGenKeyPair(ctx_A)) {
        BPU_printError("Key generation error");
        BPU_gf2VecFree(&r1);
        BPU_gf2VecFree(&r2);
        BPU_gf2VecFree(&r3);
        BPU_mecsFreeCtx(&ctx_A);
        BPU_mecsFreeParamsGoppa(&params);
        return 1;
    }

    if (BPU_mecsInitCtx(&ctx_B, &params, BPU_EN_MECS_BASIC_GOPPA)) {
        BPU_printError("Key generation error");
        BPU_gf2VecFree(&r1);
        BPU_gf2VecFree(&r2);
        BPU_gf2VecFree(&r3);
        BPU_mecsFreeCtx(&ctx_A);
        BPU_mecsFreeCtx(&ctx_B);
        BPU_mecsFreeParamsGoppa(&params);
        return 1;
    }
    /***************************************/
    fprintf(stderr, "Key generation...\n");
    // key pair generation
    if (BPU_mecsGenKeyPair(ctx_B)) {
        BPU_printError("Key generation error");
        BPU_gf2VecFree(&r1);
        BPU_gf2VecFree(&r2);
        BPU_gf2VecFree(&r3);
        BPU_mecsFreeCtx(&ctx_A);
        BPU_mecsFreeCtx(&ctx_B);
        BPU_mecsFreeParamsGoppa(&params);
        BPU_printError("Key generation error");
        return 1;
    }

    if (BPU_mecsInitCtx(&ctx_E, &params, BPU_EN_MECS_BASIC_GOPPA)) {
        BPU_printError("Key generation error");
        BPU_gf2VecFree(&r1);
        BPU_gf2VecFree(&r2);
        BPU_gf2VecFree(&r3);
        BPU_mecsFreeCtx(&ctx_A);
        BPU_mecsFreeCtx(&ctx_B);
        BPU_mecsFreeParamsGoppa(&params);
        return 1;
    }
    /***************************************/
    fprintf(stderr, "Key generation...\n");
    // key pair generation
    if (BPU_mecsGenKeyPair(ctx_E)) {
        BPU_printError("Key generation error");
        BPU_gf2VecFree(&r1);
        BPU_gf2VecFree(&r2);
        BPU_gf2VecFree(&r3);
        BPU_mecsFreeCtx(&ctx_A);
        BPU_mecsFreeCtx(&ctx_B);
        BPU_mecsFreeCtx(&ctx_E);
        BPU_mecsFreeParamsGoppa(&params);
        return 1;
    }

    BPU_gf2VecMalloc(&ct_kem,ctx_A->ct_len);

    //gain pre-shared PKE_A
    if (BPU_asn1EncodePubKey(&pka, &size, ctx_A)) {
        BPU_gf2VecFree(&r1);
        BPU_gf2VecFree(&r2);
        BPU_gf2VecFree(&r3);
        free(pka);
        BPU_mecsFreeCtx(&ctx_A);
        BPU_mecsFreeCtx(&ctx_B);
        BPU_mecsFreeCtx(&ctx_E);
        BPU_mecsFreeParamsGoppa(&params);
            return 1;
    }

    //gain pre-shared PKE_B
    if (BPU_asn1EncodePubKey(&pkb, &size, ctx_B)) {
        BPU_gf2VecFree(&r1);
        BPU_gf2VecFree(&r2);
        BPU_gf2VecFree(&r3);
        free(pkb);
        BPU_mecsFreeCtx(&ctx_A);
        BPU_mecsFreeCtx(&ctx_B);
        BPU_mecsFreeCtx(&ctx_E);
        BPU_mecsFreeParamsGoppa(&params);
            return 1;
    }

    //Encode ephemeral pk into buffer
    if (BPU_asn1EncodePubKey(&pke, &size2, ctx_E)) {
        BPU_gf2VecFree(&r1);
        BPU_gf2VecFree(&r2);
        BPU_gf2VecFree(&r3);
        free(pke);
        BPU_mecsFreeCtx(&ctx_A);
        BPU_mecsFreeCtx(&ctx_B);
        BPU_mecsFreeCtx(&ctx_E);
        BPU_mecsFreeParamsGoppa(&params);
            return 1;
    }
    BPU_printError("Encoding pub key to buffer");
    BPU_gf2VecMalloc(&pub_vec,size2);
    BPU_gf2ArraytoVector(pub_vec,pke);

    //Allocate memory for m1 = PKE | r1
    BPU_gf2VecMalloc(&s1,pub_vec->len + r1->len);
    BPU_gf2VecConcat(s1, pub_vec, r1);

    //A sends m1 to B
    if(BPU_cryptobox_send(ct_kem,s1, pkb,size)){
        BPU_printError("Hybrid scheme error");
        BPU_gf2VecFree(&r1);
        BPU_gf2VecFree(&r2);
        BPU_gf2VecFree(&r3);
        BPU_gf2VecFree(&s1);
        BPU_gf2VecFree(&ct_kem);
        free(pkb);
        free(pke);
        free(pka);
        BPU_mecsFreeCtx(&ctx_A);
        BPU_mecsFreeCtx(&ctx_B);
        BPU_mecsFreeCtx(&ctx_E);
        BPU_mecsFreeParamsGoppa(&params);
    return 1;
    }

    BPU_gf2VecMalloc(&m1_rec,s1->len);
    //B recieves s1
    if(BPU_cryptobox_recieve(m1_rec,ct_kem, ctx_B)){
         BPU_printError("Hybrid scheme error");
         BPU_gf2VecFree(&r1);
         BPU_gf2VecFree(&r2);
         BPU_gf2VecFree(&r3);
         BPU_gf2VecFree(&s1);
         BPU_gf2VecFree(&m1_rec);
         BPU_gf2VecFree(&ct_kem);
         free(pkb);
         free(pke);
         free(pka);
         BPU_mecsFreeCtx(&ctx_A);
         BPU_mecsFreeCtx(&ctx_B);
         BPU_mecsFreeCtx(&ctx_E);
         BPU_mecsFreeParamsGoppa(&params);
     return 1;
     }
    BPU_gf2VecMalloc(&pke_rec,m1_rec->len - r1->len);
    //B crops PKE
    BPU_gf2VecCrop(pke_rec,m1_rec,0, m1_rec->len - r1->len);
    //B generates r2 and r3
    BPU_gf2VecRand(r2,20);
    BPU_gf2VecRand(r3,20);

    //BPU_allocateBuffer(&pke_buf,&buf_size,pke_rec->len);
    //ToDo: BPU_gf2VectortoArray(pke_rec,pke_buf, &buf_size);

    //B encrypts s2
     BPU_gf2VecMalloc(&s2_kem, 4000);
    if(BPU_cryptobox_send(s2_kem,r2, pke,size)){
        BPU_printError("Hybrid scheme error");
        BPU_gf2VecFree(&r1);
        BPU_gf2VecFree(&r2);
        BPU_gf2VecFree(&r3);
        BPU_gf2VecFree(&s1);
        BPU_gf2VecFree(&s2_kem);
        BPU_gf2VecFree(&m1_rec);
        BPU_gf2VecFree(&ct_kem);
        free(pkb);
        free(pke);
        free(pka);
        BPU_mecsFreeCtx(&ctx_A);
        BPU_mecsFreeCtx(&ctx_B);
        BPU_mecsFreeCtx(&ctx_E);
        BPU_mecsFreeParamsGoppa(&params);
    return 1;
    }

    //B creates s3 = s2|r3|r1
    BPU_gf2VecMalloc(&s3, r3->len + r1->len + s2_kem->len);
    BPU_gf2VecMalloc(&r3r1, r3->len + r1->len);
    BPU_gf2VecConcat(r3r1, r3, r1);
    BPU_gf2VecConcat(s3, s2_kem, r3r1);

    //B sends s3 to A
    BPU_gf2VecMalloc(&s3_kem, s3->len);
    if(BPU_cryptobox_send(s3_kem,s3, pka,size)){
        BPU_printError("Hybrid scheme error");
        BPU_gf2VecFree(&r1);
        BPU_gf2VecFree(&r2);
        BPU_gf2VecFree(&r3);
        BPU_gf2VecFree(&s1);
        BPU_gf2VecFree(&s3);
        BPU_gf2VecFree(&r3r1);
        BPU_gf2VecFree(&s2_kem);
        BPU_gf2VecFree(&m1_rec);
        BPU_gf2VecFree(&ct_kem);
        free(pkb);
        free(pke);
        free(pka);
        BPU_mecsFreeCtx(&ctx_A);
        BPU_mecsFreeCtx(&ctx_B);
        BPU_mecsFreeCtx(&ctx_E);
        BPU_mecsFreeParamsGoppa(&params);
    return 1;
    }

    //A decrypts s3
    BPU_gf2VecMalloc(&s3_rec, s3_kem->len);
    if(BPU_cryptobox_recieve(s3_rec,s3_kem,ctx_A)){
         BPU_printError("Hybrid scheme error");
         BPU_gf2VecFree(&r1);
         BPU_gf2VecFree(&r2);
         BPU_gf2VecFree(&r3);
         BPU_gf2VecFree(&s1);
         BPU_gf2VecFree(&s3);
          BPU_gf2VecFree(&s3_rec);
           BPU_gf2VecFree(&s3_kem);
         BPU_gf2VecFree(&r3r1);
         BPU_gf2VecFree(&s2_kem);
         BPU_gf2VecFree(&m1_rec);
         BPU_gf2VecFree(&ct_kem);
         free(pkb);
         free(pke);
         free(pka);
         BPU_mecsFreeCtx(&ctx_A);
         BPU_mecsFreeCtx(&ctx_B);
         BPU_mecsFreeCtx(&ctx_E);
         BPU_mecsFreeParamsGoppa(&params);
     return 1;
     }

    //A decrypts r2
    BPU_gf2VecMalloc(&s2_rec, s3_rec->len - r3r1->len);
    BPU_gf2VecCrop(s2_rec,s3_rec,0, s3_rec->len - r3r1->len);

   BPU_gf2VecMalloc(&r2_rec, r2->len);
    if(BPU_cryptobox_recieve(r2_rec,s2_rec,ctx_E)){
         BPU_printError("Hybrid scheme error");
         BPU_gf2VecFree(&r1);
         BPU_gf2VecFree(&r2);
         BPU_gf2VecFree(&r3);
         BPU_gf2VecFree(&s1);
         BPU_gf2VecFree(&s3);
          BPU_gf2VecFree(&s3_rec);
           BPU_gf2VecFree(&s3_kem);
           BPU_gf2VecFree(&r2_rec);
           BPU_gf2VecFree(&s2_rec);
         BPU_gf2VecFree(&r3r1);
         BPU_gf2VecFree(&s2_kem);
         BPU_gf2VecFree(&m1_rec);
         BPU_gf2VecFree(&ct_kem);
         free(pkb);
         free(pke);
         free(pka);
         BPU_mecsFreeCtx(&ctx_A);
         BPU_mecsFreeCtx(&ctx_B);
         BPU_mecsFreeCtx(&ctx_E);
         BPU_mecsFreeParamsGoppa(&params);
     return 1;
     }

    //ToDo: uvolni pamat
    BPU_gf2VecFree(&r1);
    BPU_gf2VecFree(&r2);
    BPU_gf2VecFree(&r3);
    BPU_gf2VecFree(&s1);
    BPU_gf2VecFree(&s3);
    BPU_gf2VecFree(&s3_rec);
    BPU_gf2VecFree(&s3_kem);
    BPU_gf2VecFree(&r2_rec);
    BPU_gf2VecFree(&s2_rec);
    BPU_gf2VecFree(&r3r1);
    BPU_gf2VecFree(&s2_kem);
    BPU_gf2VecFree(&m1_rec);
    BPU_gf2VecFree(&ct_kem);
    free(pkb);
    free(pke);
    free(pka);
    BPU_mecsFreeCtx(&ctx_A);
    BPU_mecsFreeCtx(&ctx_B);
    BPU_mecsFreeCtx(&ctx_E);
    BPU_mecsFreeParamsGoppa(&params);

    return rc;
}



int main(int argc, char **argv) {
	int rc = 0;
    // MUST BE NULL

	srand(time(NULL));

    //rc += testKDF();
     rc += testCryptoBox();

     //rc += testKeyExchange();

	return rc;
}
