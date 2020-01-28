#include "commPKCS7.h"
#include "commlib.h"



void CommPKCS7_Sign::init(const char * x509file, const char * pkeyFile)
{
	BIO * in;
	in=BIO_new_file(x509file,"r");
	PASSERT3(in);
    x509=PEM_read_bio_X509(in,0,0,0);
	PASSERT3(x509);
	BIO_free(in);
	in=BIO_new_file(pkeyFile,"r");
	PASSERT3(in);
	pkey=PEM_read_bio_PrivateKey(in,0,0,0);
	PASSERT3(pkey);
	BIO_free(in);
}

void CommPKCS7_Verify::init(const char * x509file)
{
	BIO * in;
	in=BIO_new_file(x509file,"r");
	PASSERT3(in);
    x509=PEM_read_bio_X509(in,0,0,0);
	PASSERT3(x509);
	BIO_free(in);
}

void CommPKCS7_Sign::sign(const BYTE * dataToSign, size_t sizeData, PBlock & ret)
{
	PKCS7 *p7;
	PKCS7_SIGNER_INFO *si;
	p7=PKCS7_new();
	PASSERT3(p7);
	PKCS7_set_type(p7,NID_pkcs7_signed);

	si=PKCS7_add_signature(p7,x509,pkey,EVP_sha1());
	PASSERT3(si);

//	PKCS7_add_signed_attribute(si, NID_pkcs9_contentType, V_ASN1_OBJECT,
//							   OBJ_nid2obj(NID_pkcs7_data));

	PKCS7_content_new(p7,NID_pkcs7_data);
	BIO * p7bio=PKCS7_dataInit(p7,NULL);
	PASSERT3(p7bio);
	BIO_write(p7bio,dataToSign,sizeData);
	PKCS7_dataFinal(p7,p7bio);
	BIO_free_all(p7bio);

	BIO * mem;
	mem = BIO_new(BIO_s_mem());
	PASSERT3(mem);
	i2d_PKCS7_bio(mem, p7);
	size_t length = BIO_ctrl(mem,BIO_CTRL_PENDING,0,0);
	if (length>0)
	{
		BYTE * p = ret.append(length);
		BIO_read(mem,p,length);
	}
	BIO_free(mem);
	PKCS7_free(p7);
}

bool CommPKCS7_Verify::verify(BYTE* dataToVerify, size_t sizeData, PBlock & ret)
{
	BIO * mem;
	mem = BIO_new_mem_buf((void *)dataToVerify, sizeData);
	PASSERT3(mem);
	PKCS7 * p7 = d2i_PKCS7_bio(mem, 0);
	PASSERT3(p7);
	BIO_free(mem);
	if(!PKCS7_type_is_signed(p7)) 
	{
		PKCS7_free(p7);
		PTRACE3("Data not signed");
		return false;
	}

	BIO * p7bio=PKCS7_dataInit(p7,NULL);
	PASSERT3(p7bio);
	char buf[1024*4];

	mem = BIO_new(BIO_s_mem());
	int i,j;
	for (;;)
		{
		i=BIO_read(p7bio,buf,sizeof(buf));
		if (i <= 0) break;
		BIO_write(mem,buf,i);
		}
	size_t length = BIO_ctrl(mem,BIO_CTRL_PENDING,0,0);
	if (length>0)
	{
		BYTE * p = ret.append(length);
		BIO_read(mem,p,length);
	}
	BIO_free(mem);
	
	STACK_OF(PKCS7_SIGNER_INFO) *sk;
	PKCS7_SIGNER_INFO *si;

	sk=PKCS7_get_signer_info(p7);


	/* Now Verify All Signatures */

	bool retBool=false;
    for (i=0; i<sk_PKCS7_SIGNER_INFO_num(sk); i++)
	{
		si=sk_PKCS7_SIGNER_INFO_value(sk,i);
		j=PKCS7_signatureVerify(p7bio,p7,si, x509);
		PTRACE3("Verify sign %d - j=%d",i,j);
		if (j >0) 
		{
			retBool = true;
			break;
		}
	}
	BIO_free_all(p7bio);
	PKCS7_free(p7);
	return retBool;
}
