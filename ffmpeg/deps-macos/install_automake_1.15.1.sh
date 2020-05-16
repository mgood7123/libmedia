# unlink automake
brew unlink automake

# install automake
brew install ./automakeffmpeg.rb
brew test ./automakeffmpeg.rb

# automake may have already been install
# in the case automake was not already installed, re-link to make sure
brew unlink ./automakeffmpeg.rb
brew link ./automakeffmpeg.rb