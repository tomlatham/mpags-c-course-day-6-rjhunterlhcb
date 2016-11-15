
// Standard library includes
#include <string>
#include <algorithm>

// Out project headers
#include "Alphabet.hpp"
#include "CaesarCipher.hpp"
#include "VigenereCipher.hpp"

VigenereCipher::VigenereCipher( const std::string& key )
{
  this->setKey( key );
}

void VigenereCipher::setKey( const std::string& key )
{
  // Store the original key
  key_ = key;

  // Make sure the key is upper case
  std::transform( std::begin(key_), std::end(key_), std::begin(key_), ::toupper );

  // Remove non-alphabet characters
  key_.erase( std::remove_if( std::begin(key_), std::end(key_), [](char c){ return !std::isalpha(c); } ),
		  std::end(key_) );

  // Loop through the key
  for ( const char letter : key_ ) {

    // Find the position of the letter in the alphabet
    for ( Alphabet::AlphabetSize i {0}; i < Alphabet::size; ++i ) {

      if ( letter == Alphabet::alphabet[i] ) {

	// Construct a Caesar cipher object using the position as the key
	CaesarCipher cc {i};

	// Store the Caesar cipher object so that it can be retrieved based on the character in the key
	charLookup_.insert( std::make_pair( letter, cc ) );

	break;
      }
    }
  }

}

std::string VigenereCipher::applyCipher( const std::string& inputText, const CipherMode cipherMode ) const
{
  // Create the output string
  std::string outputText {""};

  // Loop through the text
  for ( std::string::size_type i {0}; i < inputText.size(); ++i ) {

    // Determine the corresponding character in the key based on the position
    // (repeating the key if necessary)
    char keyChar = key_[ i % key_.size() ];

    // Find the corresponding Caesar cipher
    auto iter = charLookup_.find( keyChar );

    // Use it to encrypt/decrypt the character of the text
    std::string oldChar { inputText[i] };
    std::string newChar { iter->second.applyCipher( oldChar, cipherMode ) };

    // Put the new character into the output
    outputText += newChar;
  }

  // Return the output text
  return outputText;
}

