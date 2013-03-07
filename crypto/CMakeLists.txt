set(CRYPTO_LIBRARY_SOURCES
    ${CHROME_DEPS_DIR}/crypto/hmac.cc
    ${CHROME_DEPS_DIR}/crypto/secure_util.cc
    ${CHROME_DEPS_DIR}/crypto/third_party/nss/sha512.cc
    ${CHROME_DEPS_DIR}/crypto/ghash.cc
    ${CHROME_DEPS_DIR}/crypto/ec_private_key_nss.cc
    ${CHROME_DEPS_DIR}/crypto/ec_signature_creator.cc
    ${CHROME_DEPS_DIR}/crypto/ec_signature_creator_nss.cc
    ${CHROME_DEPS_DIR}/crypto/encryptor.cc
    ${CHROME_DEPS_DIR}/crypto/encryptor_nss.cc
    ${CHROME_DEPS_DIR}/crypto/hmac_nss.cc
    ${CHROME_DEPS_DIR}/crypto/p224_spake.cc
    ${CHROME_DEPS_DIR}/crypto/nss_util.cc
    ${CHROME_DEPS_DIR}/crypto/openpgp_symmetric_encryption.cc
    ${CHROME_DEPS_DIR}/crypto/p224.cc
    ${CHROME_DEPS_DIR}/crypto/random.cc
    ${CHROME_DEPS_DIR}/crypto/rsa_private_key.cc
    ${CHROME_DEPS_DIR}/crypto/rsa_private_key_nss.cc
    ${CHROME_DEPS_DIR}/crypto/secure_hash_default.cc
    ${CHROME_DEPS_DIR}/crypto/sha2.cc
    ${CHROME_DEPS_DIR}/crypto/signature_creator_nss.cc
    ${CHROME_DEPS_DIR}/crypto/signature_verifier_nss.cc
    ${CHROME_DEPS_DIR}/crypto/symmetric_key_nss.cc
    ${CHROME_DEPS_DIR}/crypto/third_party/nss/pk11akey.cc
    ${CHROME_DEPS_DIR}/crypto/third_party/nss/secsign.cc
)

set(NACL_SHARED_GIO_INCLUDE_DIRECTORIES
    ${CHROME_DEPS_DIR}/src/third_party
    ${CHROME_DEPS_DIR}/src/shared/gio
)
    
include(${CHROME_DEPS_DIR}/common.txt)
add_definitions(-fPIC)

include_directories(${NACL_SHARED_GIO_INCLUDE_DIRECTORIES})
add_library(${NACL_SHARED_GIO_LIBRARY_NAME} STATIC ${NACL_SHARED_GIO_LIBRARY_SOURCES})
