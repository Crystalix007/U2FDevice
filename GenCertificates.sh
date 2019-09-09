#!/usr/bin/env sh

sed --version | grep "GNU" 2>&1 1>/dev/null
if [ $? -ne 0 ]; then
	printf "Requires the GNU version of SED.\n"
	return -1
fi

[ -d Keys ] || mkdir Keys

openssl ecparam -name prime256v1 -genkey -noout -out Keys/ecprivkey.pem
openssl ec -in Keys/ecprivkey.pem -pubout -out Keys/ecpubkey.pem 2>/dev/null
openssl req -new -x509 -key Keys/ecprivkey.pem -out Keys/certificate.pem -days 3650
openssl x509 -outform der -in Keys/certificate.pem -out Keys/certificate.der 2>/dev/null

certificate="$(xxd --include Keys/certificate.der | sed -e '0,/{/d;/};/,$d' -e 's/^\s\+/\t/g')"
certificateSize="$(printf "%s" "${certificate}" | wc -w)"
printf "%s" "${certificate}" | sed -e '/\/\/ Generate attestation certificate here/{r/dev/stdin
d }' Certificates.cpp.template > Keys/Certificates.cpp.template.1

privkey="$(openssl ec -in Keys/ecprivkey.pem -pubout -text -noout 2>/dev/null | sed -e '0,/priv:/d;/pub:/,$d' -e 's/\s//g;s/:/, /g;' -e 's/^/\t/g;s/\s\+$//g' -e 's/\(\s\)/\10x/g')"
privSize="$(printf "%s" "${privkey}" | wc -w)"
printf "%s\n" "${privkey}" | sed -e '/\/\/ Generate private key here/{r/dev/stdin
d }' Keys/Certificates.cpp.template.1 > Keys/Certificates.cpp.template.2

pubkey="$(openssl ec -in Keys/ecprivkey.pem -pubout -text -noout 2>/dev/null | sed -e '0,/pub:/d;/ASN1/,$d' -e 's/\s//g;s/:/, /g;' -e 's/^/\t/g;s/\s\+$//g' -e 's/\(\s\)/\10x/g')"
pubSize="$(printf "%s" "${pubkey}" | wc -w)"
printf "%s\n" "${pubkey}" | sed -e '/\/\/ Generate public key here/{r/dev/stdin
d }' Keys/Certificates.cpp.template.2 > Certificates.cpp

sed -e "s/\\/\\* attestation certificate size \\*\\//${certificateSize}/; s/\\/\\* attestation private key size \\*\\//${privSize}/; s/\\/\\* attestion public key size \\*\\//${pubSize}/" Certificates.hpp.template > Certificates.hpp
