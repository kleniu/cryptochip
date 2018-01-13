#!/bin/bash
set -e

ISSUER_PRIVKEY=./issuer_prikey.pem
ISSUER_CSR=./issuer_csr.pem
ISSUER_CERT=./issuer_cert.pem

################################################################################

function genIssuer() {
    # generate private key
    echo "##### Generating private key."
    openssl ecparam -genkey -name prime256v1 -out $ISSUER_PRIVKEY
    cat $ISSUER_PRIVKEY
    echo "##### Generated key parameters:"
    openssl ecparam -in $ISSUER_PRIVKEY -text -noout
    echo "##### Generated primary key:"
    openssl ec -in $ISSUER_PRIVKEY -text -noout

    # generate SCR
    echo
    echo "##### Generating CSR"
    openssl req -new -subj "/C=PL/L=Machu Picchu/O=Issuers/OU=Testers/CN=foo.bar.pl" -sha256 -key $ISSUER_PRIVKEY -out $ISSUER_CSR
    cat $ISSUER_CSR
    echo "##### Generated CSR"
    openssl req -in $ISSUER_CSR -text -noout

    # generate SSC
    echo
    echo "##### Generating SSC."
    openssl x509 -in $ISSUER_CSR -req -signkey $ISSUER_PRIVKEY -days 3650 -out $ISSUER_CERT
    rm $ISSUER_CSR # we don't need this anymore
    cat $ISSUER_CERT
    echo "##### Generated SSC"
    openssl x509 -in $ISSUER_CERT -text -noout
}

################################################################################

genIssuer
