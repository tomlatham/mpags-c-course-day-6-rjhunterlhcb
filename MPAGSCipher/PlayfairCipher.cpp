
// Standard library includes
#include <string>
#include <algorithm>

// Out project headers
#include "Alphabet.hpp"
#include "PlayfairCipher.hpp"

PlayfairCipher::PlayfairCipher( const std::string& key )
{
  this->setKey( key );
}

void PlayfairCipher::setKey( const std::string& key )
{
  // Store the original key
  key_ = key;

  // Append the alphabet to the key
  key_ += Alphabet::alphabet;

  // Make sure the key is upper case
  std::transform( std::begin(key_), std::end(key_), std::begin(key_), ::toupper );

  // Remove non-alphabet characters
  key_.erase( std::remove_if( std::begin(key_), std::end(key_), [](char c){ return !std::isalpha(c); } ),
		  std::end(key_) );

  // Change J -> I
  std::transform( std::begin(key_), std::end(key_), std::begin(key_), [](char c){ return (c == 'J') ? 'I' : c; } );

  // Remove duplicated letters
  std::string lettersFound {""};
  auto detectDuplicates = [&](char c) {
	  if ( lettersFound.find(c) == std::string::npos ) {
		  lettersFound += c;
		  return false;
	  } else {
		  return true;
	  }
  };
  key_.erase( std::remove_if( std::begin(key_), std::end(key_), detectDuplicates ), std::end(key_) );

  // Store the coordinates of each letter
  for ( std::string::size_type i{0}; i < key_.size(); ++i ) {
	  std::string::size_type row{ i/5 };
	  std::string::size_type column{ i%5 };

	  auto coords = std::make_pair( row, column );

	  charLookup_[ key_[i] ] = coords;
	  coordLookup_[ coords ] = key_[i];
  }
}

std::string PlayfairCipher::applyCipher( const std::string& inputText, const CipherMode cipherMode ) const
{
  // Create the output string, initially a copy of the input text
  std::string outputText {inputText};

  // Change J -> I
  std::transform( std::begin(outputText), std::end(outputText), std::begin(outputText), [](char c){ return (c == 'J') ? 'I' : c; } );

  // Find repeated characters (but only when they occur within a bigram)
  // and add an X (or a Q for repeated X's) between them
  std::string tmpText {""};
  for (std::string::size_type i{0}; i < outputText.size(); i+=2) {
    // Always add the first of the bigram
    tmpText += outputText[i];
    if ( i+1 == outputText.size() ) {
      // If this was the last character then we need to break out
      break;
    } else if ( outputText[i] != outputText[i+1] ) {
      // If the two characters in the bigram are different,
      // simply add the second one as well
      tmpText += outputText[i+1];
    } else {
      // Otherwise, if two characters in the bigram are the same,
      // we instead add an X (or a Q if the first was an X)
      tmpText += (outputText[i] == 'X') ? 'Q' : 'X';
      // Need to decrement i since the second character in this bigram now
      // becomes the first character in the next one
      --i;
    }
  }
  outputText = tmpText;

  // If the size of the input is odd, add a trailing Z
  if ( (outputText.size() % 2) == 1 ) {
    outputText += 'Z';
  }

  // Loop over the input digraphs
  for (std::string::size_type i{0}; i < outputText.size(); i+=2) {

    // Find the coordinates in the grid for each digraph
    auto pointOneIter = charLookup_.find( outputText[i] );
    auto pointTwoIter = charLookup_.find( outputText[i+1] );
    PlayfairCoords pointOne = pointOneIter->second;
    PlayfairCoords pointTwo = pointTwoIter->second;

    // Find whether the two points are on a row, a column or form a rectangle/square
    // Then apply the appropriate rule to these coords to get new coords
    if ( pointOne.first == pointTwo.first ) {

      // Row - so increment/decrement the column indices (modulo 5)
      switch ( cipherMode ) {
	case CipherMode::Encrypt :
	  pointOne.second = (pointOne.second + 1) % 5;
	  pointTwo.second = (pointTwo.second + 1) % 5;
	  break;
	case CipherMode::Decrypt :
	  pointOne.second = (5 + pointOne.second - 1) % 5;
	  pointTwo.second = (5 + pointTwo.second - 1) % 5;
	  break;
      }

    } else if ( pointOne.second == pointTwo.second ) {

      // Column - so increment/decrement the row indices (modulo 5)
      switch ( cipherMode ) {
	case CipherMode::Encrypt :
	  pointOne.first = (pointOne.first + 1) % 5;
	  pointTwo.first = (pointTwo.first + 1) % 5;
	  break;
	case CipherMode::Decrypt :
	  pointOne.first = (5 + pointOne.first - 1) % 5;
	  pointTwo.first = (5 + pointTwo.first - 1) % 5;
	  break;
      }

    } else {

      // Rectangle/Square - so keep the rows the same and swap the columns
      // (NB the operation is actually the same regardless of encrypt/decrypt
      // since applying the same operation twice gets you back to where you were)
      std::swap( pointOne.second, pointTwo.second );

    }

    // Find the letters associated with the new coords
    auto charOneIter = coordLookup_.find( pointOne );
    auto charTwoIter = coordLookup_.find( pointTwo );
    char charOne = charOneIter->second;
    char charTwo = charTwoIter->second;

    // Make the replacements
    outputText[i] = charOne;
    outputText[i+1] = charTwo;
  }

  // Return the output text
  return outputText;
}
