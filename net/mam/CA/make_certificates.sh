
#CA  Certificate Authority 
#pki public key infrastructure
#csr Certificate Signing Request 
#crt Certificate 

sh clean.sh

# make self-signed CA certificates
CA_NAME=$(hostname)"-ca"
openssl ecparam -genkey -name prime256v1 -out ca_key.pem
openssl req -new -sha256 -nodes -key ca_key.pem -subj="/C=US/ST=CA/L=LosAngeles/O=thefirm/CN=$CA_NAME" -out ca_cert.req
#openssl req -in ca_cert.req -text -noout
openssl req -x509 -sha256 -nodes -days 365 -key ca_key.pem -in ca_cert.req -out ca_cert.pem
#openssl x509 -in ca_cert.pem -text -noout

# make client certificates, signed by CA certificates
CLIENT_NAME=$(hostname)"-client"
openssl ecparam -genkey -name prime256v1 -out client_key.pem
openssl req -new -sha256 -nodes -key client_key.pem -subj="/C=US/ST=CA/L=LosAngeles/O=thefirm/CN=$CLIENT_NAME" -out client_cert.req
openssl x509 -req -sha256 -days 365 -in client_cert.req -CA=ca_cert.pem -CAkey=ca_key.pem -CAcreateserial -out client_cert.pem &> /dev/null
#openssl x509 -in client_cert.pem -text -noout

# make server certificates, signed by CA certificates
SERVER_NAME=$(hostname)"-server"
openssl ecparam -genkey -name prime256v1 -out server_key.pem
openssl req -new -sha256 -nodes -key server_key.pem -subj="/C=US/ST=CA/L=LosAngeles/O=thefirm/CN=$SERVER_NAME" -out server_cert.req
#openssl req -in server_cert.req -text -noout
openssl x509 -req -sha256 -days 365 -in server_cert.req -CA=ca_cert.pem -CAkey=ca_key.pem -CAcreateserial -out server_cert.pem &> /dev/null
#openssl x509 -in server_cert.pem -text -noout

# 4.1.1 Generate Private Key
# openssl genrsa –out ubs_rcXXXX.key 1024
# 4.1.2 Certificate Signing Request Generation:
# openssl req -new -key ubs_rcXXXX.key -out ubs_rcXXXX.csr
# 4.1.3 Send Certificate to UBS
# Email the ubs_rcXXXX.csr file to UBS support team (ol-ecstlondon@ubs.com)
# 4.1.3 Send Certificate to UBS
# Email the ubs_rcXXXX.csr file to UBS support team (ol-ecstlondon@ubs.com)
#  UBS will return the signed certificate (ubs_rcXXXX.pem) as well as 
#  the UBS FixSpec Certificate Authority (ca_ubs_fixspec.pem) to you within 24 hours.

# openssl genrsa 1024 > CA.key                                (generates private key)
# openssl req -new -key CA.key -x509 -days 1095 -out CA.crt   (certificate/public key generated from private key)
# openssl x509 -req -days 1095 -in server_cert.csr -CA CA.crt -CAkey CA.key -CAcreateserial -out server_cert.crt (signing certificate/public key signed by CA)

# https://security.stackexchange.com/questions/150078/missing-x509-extensions-with-an-openssl-generated-certificate
# openssl genrsa -out etcd1-key.pem 2048
# openssl req -new -key etcd1-key.pem -config openssl.conf -subj '/CN=etcd' -out etcd1.csr
# openssl x509 -req -in etcd1.csr -CA ca.pem -CAkey ca-key.pem -CAcreateserial -out etcd1.pem -days 1024 -sha256

