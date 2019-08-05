From https://github.com/pratikd650/Teensy_U2F/blob/master/Teensy_U2F.cpp line 292

Instructions to generate attestation certificate using open ssl
https://wiki.openssl.org/index.php/Command_Line_Elliptic_Curve_Operations
https://www.guyrutenberg.com/2013/12/28/creating-self-signed-ecdsa-ssl-certificate-using-openssl/
 P-256 (also secp256r1)  EC key pair is    W = dG   (Note secp256k1 is Koblitz curve - not P256)
 d = private key is it 256 bits (32 bytes)
 G = generator point - it is part of the curve definition
 W = public key point - it is a (256, 256) bits  - 64 bytes
1) Generate a key pair - the private key will be saved in PKCS8 format in ecprivkey.pem

`openssl ecparam -name prime256v1 -genkey -noout -out ecprivkey.pem`

2) Dump out the private key in hex format - it will be a 32 byte key

`openssl asn1parse  -in ecprivkey.pem`

3) Compute the public key from the private key and the curve

`openssl ec -in ecprivkey.pem -pubout -out ecpubkey.pem`

4) Dump out the public key in hex format - it will be 66 byte - the first two bytes are 00 04,

`openssl ec -in ecprivkey.pem -pubout -text`

after that is the point W - 32 byte + 32 byte

5) Generate a self signed certificate

`openssl req -new -x509 -key ecprivkey.pem -out certificate.pem -days 3650`

For the Certificate name give a unique certificate name.

6) Display the certificate

`openssl x509 -in certificate.pem -text -noout`

7) Convert PEM certificate to DER

`openssl x509 -outform der -in certificate.pem -out certificate.der`

8) Generate a usable c-array for source code

`xxd --include certificate.der`

Copy output into appropriate array in 'Certificates.cpp', overwriting existing values

9) Repeat steps 7 & 8 for public key and private key

So:

```
openssl asn1parse -in ecprivkey.pem 2>/dev/null | grep 'HEX DUMP' | perl -pe 's/^.*\[HEX DUMP\]:(.+)$/$1/' 2>/dev/null | xxd -r -p > privkey.der && xxd --include privkey.der

openssl ec -in ecprivkey.pem -pubout -text 2>/dev/null | perl -0777 -ne 'print /pub:.+ASN1/sg' 2>/dev/null | sed -e '/pub:/d;/ASN1/d' | perl -pe 's/^\s+(.+):?$/$1/gm' 2>/dev/null | perl -pe 's/\n//' 2>/dev/null | perl -pe 's/(.{2}):?/$1/g' 2>/dev/null | xxd -r -p > pubkey.der && xxd --include pubkey.der
```

and copy the arrays into the correct arrays in Certificates.cpp.

If any arrays have different lengths than shown in Certificates.hpp, update these too.
