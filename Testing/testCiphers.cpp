//! Unit Tests for MPAGSCipher Cipher Classes
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <map>
#include <string>

#include "Cipher.hpp"
#include "CipherFactory.hpp"
#include "CipherMode.hpp"
#include "CipherType.hpp"

std::map<CipherType,std::string> keys = {
  std::pair<CipherType,std::string>(CipherType::Caesar,"10"),
  std::pair<CipherType,std::string>(CipherType::Playfair,"hello"),
  std::pair<CipherType,std::string>(CipherType::Vigenere,"hello")
};

std::map<CipherType,std::string> plainText = {
  std::pair<CipherType,std::string>(CipherType::Caesar,"HELLOWORLD"),
  std::pair<CipherType,std::string>(CipherType::Playfair,"BOBISXSOMESORTOFIUNIORCOMPLEXQXENOPHONEONEZEROTHINGZ"),
  std::pair<CipherType,std::string>(CipherType::Vigenere,"THISISQUITEALONGMESSAGESOTHEKEYWILLNEEDTOREPEATAFEWTIMES")
};

std::map<CipherType,std::string> cipherText = {
  std::pair<CipherType,std::string>(CipherType::Caesar,"ROVVYGYBVN"),
  std::pair<CipherType,std::string>(CipherType::Playfair,"FHIQXLTLKLTLSUFNPQPKETFENIOLVSWLTFIAFTLAKOWATEQOKPPA"),
  std::pair<CipherType,std::string>(CipherType::Vigenere,"ALTDWZUFTHLEWZBNQPDGHKPDCALPVSFATWZUIPOHVVPASHXLQSDXTXSZ")
};

bool testCipher( const Cipher& cipher, const CipherMode mode, const std::string& inputText, const std::string& outputText )
{
  return cipher.applyCipher( inputText, mode ) == outputText;
}

TEST_CASE("Cipher encryption", "[ciphers]") {
  for ( auto& elem : keys ) {
    auto cipher = cipherFactory( elem.first, elem.second );
    if ( cipher ) {
      REQUIRE( testCipher( *cipher, CipherMode::Encrypt, plainText[elem.first], cipherText[elem.first]) );
    }
  }
}

TEST_CASE("Cipher decryption", "[ciphers]") {
  for ( auto& elem : keys ) {
    auto cipher = cipherFactory( elem.first, elem.second );
    if ( cipher ) {
      REQUIRE( testCipher( *cipher, CipherMode::Decrypt, cipherText[elem.first], plainText[elem.first]) );
    }
  }
}
